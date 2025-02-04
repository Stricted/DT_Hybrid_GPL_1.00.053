/*
 *  linux/mm/oom_kill.c
 * 
 *  Copyright (C)  1998,2000  Rik van Riel
 *	Thanks go out to Claus Fischer for some serious inspiration and
 *	for goading me into coding this file...
 *
 *  The routines in this file are used to kill a process when
 *  we're seriously out of memory. This gets called from __alloc_pages()
 *  in mm/page_alloc.c when we really run out of memory.
 *
 *  Since we won't call these routines often (on a well-configured
 *  machine) this file will double as a 'coding guide' and a signpost
 *  for newbie kernel hackers. It features several pointers to major
 *  kernel subsystems and hints as to where to find out what things do.
 */

#include <linux/oom.h>
#include <linux/mm.h>
#include <linux/err.h>
#include <linux/sched.h>
#include <linux/swap.h>
#include <linux/timex.h>
#include <linux/jiffies.h>
#include <linux/cpuset.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/memcontrol.h>
#include <linux/security.h>
#ifdef CONFIG_CRASH_DUMPFILE
#include <linux/time.h>
#include <linux/rtc.h>
#include "bhal.h"
//#include "atpflashlayout.h"
bool g_bWriteCrashFirstTime = 1;
char g_acWriteCrashBuf[2048];

extern struct timezone sys_tz;
extern int bhalWriteFlash(void *pszDataBuffer, unsigned long ulFlashAddr, unsigned long ulLen);
extern unsigned long ATP_FLASH_GetFlashAreaSize(unsigned long enCfgAreaType);
extern int ATP_FLASH_WriteFlashArea(unsigned long enCfgAreaType, void *pszString, unsigned long strLen, unsigned long offset);
extern int ATP_FLASH_ReadFlashArea(unsigned long enCfgAreaType,void *string,unsigned long strLen, unsigned long offset);
extern int ATP_FLASH_EraseFlashArea(unsigned long enCfgAreaType);
#endif

int sysctl_panic_on_oom;
int sysctl_oom_kill_allocating_task;
int sysctl_oom_dump_tasks;
static DEFINE_SPINLOCK(zone_scan_lock);
/* #define DEBUG */

/**
 * badness - calculate a numeric value for how bad this task has been
 * @p: task struct of which task we should calculate
 * @uptime: current uptime in seconds
 *
 * The formula used is relatively simple and documented inline in the
 * function. The main rationale is that we want to select a good task
 * to kill when we run out of memory.
 *
 * Good in this context means that:
 * 1) we lose the minimum amount of work done
 * 2) we recover a large amount of memory
 * 3) we don't kill anything innocent of eating tons of memory
 * 4) we want to kill the minimum amount of processes (one)
 * 5) we try to kill the process the user expects us to kill, this
 *    algorithm has been meticulously tuned to meet the principle
 *    of least surprise ... (be careful when you change it)
 */

unsigned long badness(struct task_struct *p, unsigned long uptime)
{
	unsigned long points, cpu_time, run_time;
	struct mm_struct *mm;
	struct task_struct *child;

	task_lock(p);
	mm = p->mm;
	if (!mm) {
		task_unlock(p);
		return 0;
	}

	/*
	 * The memory size of the process is the basis for the badness.
	 */
	points = mm->total_vm;

	/*
	 * After this unlock we can no longer dereference local variable `mm'
	 */
	task_unlock(p);

	/*
	 * swapoff can easily use up all memory, so kill those first.
	 */
	if (p->flags & PF_SWAPOFF)
		return ULONG_MAX;

	/*
	 * Processes which fork a lot of child processes are likely
	 * a good choice. We add half the vmsize of the children if they
	 * have an own mm. This prevents forking servers to flood the
	 * machine with an endless amount of children. In case a single
	 * child is eating the vast majority of memory, adding only half
	 * to the parents will make the child our kill candidate of choice.
	 */
	list_for_each_entry(child, &p->children, sibling) {
		task_lock(child);
		if (child->mm != mm && child->mm)
			points += child->mm->total_vm/2 + 1;
		task_unlock(child);
	}

	/*
	 * CPU time is in tens of seconds and run time is in thousands
         * of seconds. There is no particular reason for this other than
         * that it turned out to work very well in practice.
	 */
	cpu_time = (cputime_to_jiffies(p->utime) + cputime_to_jiffies(p->stime))
		>> (SHIFT_HZ + 3);

	if (uptime >= p->start_time.tv_sec)
		run_time = (uptime - p->start_time.tv_sec) >> 10;
	else
		run_time = 0;

	if (cpu_time)
		points /= int_sqrt(cpu_time);
	if (run_time)
		points /= int_sqrt(int_sqrt(run_time));

	/*
	 * Niced processes are most likely less important, so double
	 * their badness points.
	 */
	if (task_nice(p) > 0)
		points *= 2;

	/*
	 * Superuser processes are usually more important, so we make it
	 * less likely that we kill those.
	 */
	if (has_capability_noaudit(p, CAP_SYS_ADMIN) ||
	    has_capability_noaudit(p, CAP_SYS_RESOURCE))
		points /= 4;

	/*
	 * We don't want to kill a process with direct hardware access.
	 * Not only could that mess up the hardware, but usually users
	 * tend to only have this flag set on applications they think
	 * of as important.
	 */
	if (has_capability_noaudit(p, CAP_SYS_RAWIO))
		points /= 4;

	/*
	 * If p's nodes don't overlap ours, it may still help to kill p
	 * because p may have allocated or otherwise mapped memory on
	 * this node before. However it will be less likely.
	 */
	if (!cpuset_mems_allowed_intersects(current, p))
		points /= 8;

	/*
	 * Adjust the score by oomkilladj.
	 */
	if (p->oomkilladj) {
		if (p->oomkilladj > 0) {
			if (!points)
				points = 1;
			points <<= p->oomkilladj;
		} else
			points >>= -(p->oomkilladj);
	}

#ifdef DEBUG
	printk(KERN_DEBUG "OOMkill: task %d (%s) got %lu points\n",
	p->pid, p->comm, points);
#endif
	return points;
}

/*
 * Determine the type of allocation constraint.
 */
static inline enum oom_constraint constrained_alloc(struct zonelist *zonelist,
						    gfp_t gfp_mask)
{
#ifdef CONFIG_NUMA
	struct zone *zone;
	struct zoneref *z;
	enum zone_type high_zoneidx = gfp_zone(gfp_mask);
	nodemask_t nodes = node_states[N_HIGH_MEMORY];

	for_each_zone_zonelist(zone, z, zonelist, high_zoneidx)
		if (cpuset_zone_allowed_softwall(zone, gfp_mask))
			node_clear(zone_to_nid(zone), nodes);
		else
			return CONSTRAINT_CPUSET;

	if (!nodes_empty(nodes))
		return CONSTRAINT_MEMORY_POLICY;
#endif

	return CONSTRAINT_NONE;
}

/*
 * Simple selection loop. We chose the process with the highest
 * number of 'points'. We expect the caller will lock the tasklist.
 *
 * (not docbooked, we don't want this one cluttering up the manual)
 */
static struct task_struct *select_bad_process(unsigned long *ppoints,
						struct mem_cgroup *mem)
{
	struct task_struct *g, *p;
	struct task_struct *chosen = NULL;
	struct timespec uptime;
	*ppoints = 0;

	do_posix_clock_monotonic_gettime(&uptime);
	do_each_thread(g, p) {
		unsigned long points;

		/*
		 * skip kernel threads and tasks which have already released
		 * their mm.
		 */
		if (!p->mm)
			continue;
		/* skip the init task */
		if (is_global_init(p))
			continue;
		if (mem && !task_in_mem_cgroup(p, mem))
			continue;

		/*
		 * This task already has access to memory reserves and is
		 * being killed. Don't allow any other task access to the
		 * memory reserve.
		 *
		 * Note: this may have a chance of deadlock if it gets
		 * blocked waiting for another task which itself is waiting
		 * for memory. Is there a better alternative?
		 */
		if (test_tsk_thread_flag(p, TIF_MEMDIE))
			return ERR_PTR(-1UL);

		/*
		 * This is in the process of releasing memory so wait for it
		 * to finish before killing some other task by mistake.
		 *
		 * However, if p is the current task, we allow the 'kill' to
		 * go ahead if it is exiting: this will simply set TIF_MEMDIE,
		 * which will allow it to gain access to memory reserves in
		 * the process of exiting and releasing its resources.
		 * Otherwise we could get an easy OOM deadlock.
		 */
		if (p->flags & PF_EXITING) {
			if (p != current)
				return ERR_PTR(-1UL);

			chosen = p;
			*ppoints = ULONG_MAX;
		}

		if (p->oomkilladj == OOM_DISABLE)
			continue;

		points = badness(p, uptime.tv_sec);
		if (points > *ppoints || !chosen) {
			chosen = p;
			*ppoints = points;
		}
	} while_each_thread(g, p);

	return chosen;
}

/**
 * dump_tasks - dump current memory state of all system tasks
 * @mem: target memory controller
 *
 * Dumps the current memory state of all system tasks, excluding kernel threads.
 * State information includes task's pid, uid, tgid, vm size, rss, cpu, oom_adj
 * score, and name.
 *
 * If the actual is non-NULL, only tasks that are a member of the mem_cgroup are
 * shown.
 *
 * Call with tasklist_lock read-locked.
 */
static void dump_tasks(const struct mem_cgroup *mem)
{
	struct task_struct *g, *p;

	printk(KERN_INFO "[ pid ]   uid  tgid total_vm      rss cpu oom_adj "
	       "name\n");
	do_each_thread(g, p) {
		struct mm_struct *mm;

		if (mem && !task_in_mem_cgroup(p, mem))
			continue;
		if (!thread_group_leader(p))
			continue;

		task_lock(p);
		mm = p->mm;
		if (!mm) {
			/*
			 * total_vm and rss sizes do not exist for tasks with no
			 * mm so there's no need to report them; they can't be
			 * oom killed anyway.
			 */
			task_unlock(p);
			continue;
		}
		printk(KERN_INFO "[%5d] %5d %5d %8lu %8lu %3d     %3d %s\n",
		       p->pid, __task_cred(p)->uid, p->tgid, mm->total_vm,
		       get_mm_rss(mm), (int)task_cpu(p), p->oomkilladj,
		       p->comm);
		task_unlock(p);
	} while_each_thread(g, p);
}

/*
 * Send SIGKILL to the selected  process irrespective of  CAP_SYS_RAW_IO
 * flag though it's unlikely that  we select a process with CAP_SYS_RAW_IO
 * set.
 */
static void __oom_kill_task(struct task_struct *p, int verbose)
{
	if (is_global_init(p)) {
		WARN_ON(1);
		printk(KERN_WARNING "tried to kill init!\n");
		return;
	}

	if (!p->mm) {
		WARN_ON(1);
		printk(KERN_WARNING "tried to kill an mm-less task!\n");
		return;
	}

	if (verbose)
		printk(KERN_ERR "Killed process %d (%s)\n",
				task_pid_nr(p), p->comm);

	/*
	 * We give our sacrificial lamb high priority and access to
	 * all the memory it needs. That way it should be able to
	 * exit() and clear out its resources quickly...
	 */
	p->rt.time_slice = HZ;
	set_tsk_thread_flag(p, TIF_MEMDIE);

	force_sig(SIGKILL, p);
}

static int oom_kill_task(struct task_struct *p)
{
	struct mm_struct *mm;
	struct task_struct *g, *q;

	mm = p->mm;

	/* WARNING: mm may not be dereferenced since we did not obtain its
	 * value from get_task_mm(p).  This is OK since all we need to do is
	 * compare mm to q->mm below.
	 *
	 * Furthermore, even if mm contains a non-NULL value, p->mm may
	 * change to NULL at any time since we do not hold task_lock(p).
	 * However, this is of no concern to us.
	 */

	if (mm == NULL)
		return 1;

	/*
	 * Don't kill the process if any threads are set to OOM_DISABLE
	 */
	do_each_thread(g, q) {
		if (q->mm == mm && q->oomkilladj == OOM_DISABLE)
			return 1;
	} while_each_thread(g, q);

	__oom_kill_task(p, 1);

	/*
	 * kill all processes that share the ->mm (i.e. all threads),
	 * but are in a different thread group. Don't let them have access
	 * to memory reserves though, otherwise we might deplete all memory.
	 */
	do_each_thread(g, q) {
		if (q->mm == mm && !same_thread_group(q, p))
			force_sig(SIGKILL, q);
	} while_each_thread(g, q);

	return 0;
}

static int oom_kill_process(struct task_struct *p, gfp_t gfp_mask, int order,
			    unsigned long points, struct mem_cgroup *mem,
			    const char *message)
{
	struct task_struct *c;

	if (printk_ratelimit()) {
		printk(KERN_WARNING "%s invoked oom-killer: "
			"gfp_mask=0x%x, order=%d, oomkilladj=%d\n",
			current->comm, gfp_mask, order, current->oomkilladj);
		task_lock(current);
		cpuset_print_task_mems_allowed(current);
		task_unlock(current);
		dump_stack();
		mem_cgroup_print_oom_info(mem, current);
		show_mem();
		if (sysctl_oom_dump_tasks)
			dump_tasks(mem);
	}

	/*
	 * If the task is already exiting, don't alarm the sysadmin or kill
	 * its children or threads, just set TIF_MEMDIE so it can die quickly
	 */
	if (p->flags & PF_EXITING) {
		__oom_kill_task(p, 0);
		return 0;
	}

	printk(KERN_ERR "%s: kill process %d (%s) score %li or a child\n",
					message, task_pid_nr(p), p->comm, points);

	/* Try to kill a child first */
	list_for_each_entry(c, &p->children, sibling) {
		if (c->mm == p->mm)
			continue;
		if (!oom_kill_task(c))
			return 0;
	}
	return oom_kill_task(p);
}

#ifdef CONFIG_CGROUP_MEM_RES_CTLR
void mem_cgroup_out_of_memory(struct mem_cgroup *mem, gfp_t gfp_mask)
{
	unsigned long points = 0;
	struct task_struct *p;

	read_lock(&tasklist_lock);
retry:
	p = select_bad_process(&points, mem);
	if (PTR_ERR(p) == -1UL)
		goto out;

	if (!p)
		p = current;

	if (oom_kill_process(p, gfp_mask, 0, points, mem,
				"Memory cgroup out of memory"))
		goto retry;
out:
	read_unlock(&tasklist_lock);
}
#endif

static BLOCKING_NOTIFIER_HEAD(oom_notify_list);

int register_oom_notifier(struct notifier_block *nb)
{
	return blocking_notifier_chain_register(&oom_notify_list, nb);
}
EXPORT_SYMBOL_GPL(register_oom_notifier);

int unregister_oom_notifier(struct notifier_block *nb)
{
	return blocking_notifier_chain_unregister(&oom_notify_list, nb);
}
EXPORT_SYMBOL_GPL(unregister_oom_notifier);

/*
 * Try to acquire the OOM killer lock for the zones in zonelist.  Returns zero
 * if a parallel OOM killing is already taking place that includes a zone in
 * the zonelist.  Otherwise, locks all zones in the zonelist and returns 1.
 */
int try_set_zone_oom(struct zonelist *zonelist, gfp_t gfp_mask)
{
	struct zoneref *z;
	struct zone *zone;
	int ret = 1;

	spin_lock(&zone_scan_lock);
	for_each_zone_zonelist(zone, z, zonelist, gfp_zone(gfp_mask)) {
		if (zone_is_oom_locked(zone)) {
			ret = 0;
			goto out;
		}
	}

	for_each_zone_zonelist(zone, z, zonelist, gfp_zone(gfp_mask)) {
		/*
		 * Lock each zone in the zonelist under zone_scan_lock so a
		 * parallel invocation of try_set_zone_oom() doesn't succeed
		 * when it shouldn't.
		 */
		zone_set_flag(zone, ZONE_OOM_LOCKED);
	}

out:
	spin_unlock(&zone_scan_lock);
	return ret;
}

/*
 * Clears the ZONE_OOM_LOCKED flag for all zones in the zonelist so that failed
 * allocation attempts with zonelists containing them may now recall the OOM
 * killer, if necessary.
 */
void clear_zonelist_oom(struct zonelist *zonelist, gfp_t gfp_mask)
{
	struct zoneref *z;
	struct zone *zone;

	spin_lock(&zone_scan_lock);
	for_each_zone_zonelist(zone, z, zonelist, gfp_zone(gfp_mask)) {
		zone_clear_flag(zone, ZONE_OOM_LOCKED);
	}
	spin_unlock(&zone_scan_lock);
}

/*
 * Must be called with tasklist_lock held for read.
 */
static void __out_of_memory(gfp_t gfp_mask, int order)
{
	struct task_struct *p;
		unsigned long points;

	if (sysctl_oom_kill_allocating_task)
		if (!oom_kill_process(current, gfp_mask, order, 0, NULL,
				"Out of memory (oom_kill_allocating_task)"))
			return;
retry:
		/*
		 * Rambo mode: Shoot down a process and hope it solves whatever
		 * issues we may have.
		 */
		p = select_bad_process(&points, NULL);

		if (PTR_ERR(p) == -1UL)
			return;

		/* Found nothing?!?! Either we hang forever, or we panic. */
		if (!p) {
			read_unlock(&tasklist_lock);
			panic("Out of memory and no killable processes...\n");
		}

		if (oom_kill_process(p, gfp_mask, order, points, NULL,
				     "Out of memory"))
			goto retry;
	}

/*
 * pagefault handler calls into here because it is out of memory but
 * doesn't know exactly how or why.
 */
void pagefault_out_of_memory(void)
{
	unsigned long freed = 0;

	blocking_notifier_call_chain(&oom_notify_list, 0, &freed);
	if (freed > 0)
		/* Got some memory back in the last second. */
		return;

	/*
	 * If this is from memcg, oom-killer is already invoked.
	 * and not worth to go system-wide-oom.
	 */
	if (mem_cgroup_oom_called(current))
		goto rest_and_return;

	if (sysctl_panic_on_oom)
		panic("out of memory from page fault. panic_on_oom is selected.\n");

	read_lock(&tasklist_lock);
	__out_of_memory(0, 0); /* unknown gfp_mask and order */
	read_unlock(&tasklist_lock);

	/*
	 * Give "p" a good chance of killing itself before we
	 * retry to allocate memory.
	 */
rest_and_return:
	if (!test_thread_flag(TIF_MEMDIE))
		schedule_timeout_uninterruptible(1);
}


/*start add by q00122007 2010-06-25*/
#ifdef CONFIG_CRASH_DUMPFILE

int ATP_Krnl_Crash_RegWriteFlash(const char * pszBuffer)
{

	int lRet;
    unsigned int ulFlashLen;
    unsigned int ulLogLen;	
	ATP_Crash_FLASH_HEAD_ST  stDummy;
	char *pstLogString = NULL;
	unsigned int ulPreFlashLen;
	bool bErase = 0;
	static char acTimeBuf[256] = {0};
	char *pszWriteBuf = NULL;
	unsigned int ulWriteLen = 0;
	static char acCrashTimeTmp[32] = {0};
	char acCrashTime[24] = {0};
	unsigned int ulTimeLenTmp = 0;
	unsigned int ulTimeLen = 0;
	static unsigned int ulThisHasAddLen = 0;
	struct rtc_time tm;
	struct timeval time;
	printk("ATP_Crash_RegWriteFlash \r\n");	
	if(strstr(pszBuffer, "out of memory"))
	{		
		if(!g_bWriteCrashFirstTime)
		{
			return 1;
		}
		
	}
    if (!pszBuffer || 0 == strlen(pszBuffer))
    {
        return 1;
    }

	pszWriteBuf = (char *)kmalloc(2048 + 1,GFP_ATOMIC);
	
	if(!pszWriteBuf)
	{
		return 1;
	}
	memset(pszWriteBuf, 0, sizeof(pszWriteBuf));
	memset(&stDummy, 0, sizeof(stDummy));
	if(g_bWriteCrashFirstTime)
	{			

		do_gettimeofday(&time);
		rtc_time_to_tm(time.tv_sec - (sys_tz.tz_minuteswest * 60), &tm);
		sprintf(acCrashTimeTmp, "%4d-%02d-%02d %02d:%02d:%02d", 1900 + tm.tm_year, tm.tm_mon+1, tm.tm_mday,
				tm.tm_hour, tm.tm_min, tm.tm_sec);
		sprintf(acTimeBuf,"\n--------------------crash time:%s-------------------------\n",acCrashTimeTmp);		
		printk("%s",acTimeBuf);
		snprintf(pszWriteBuf, 2048, "%s", acTimeBuf);
	}	
				

	ulTimeLenTmp = 0;
	while(ulTimeLenTmp < strlen(acCrashTimeTmp))
	{
		if((' ' != acCrashTimeTmp[ulTimeLenTmp]) && (':' != acCrashTimeTmp[ulTimeLenTmp]))
		{
			acCrashTime[ulTimeLen] = acCrashTimeTmp[ulTimeLenTmp];
			ulTimeLen++;
		}

		ulTimeLenTmp++;
	}
	
	
	snprintf(pszWriteBuf, 2048,"%s%s", pszWriteBuf, pszBuffer);
	ulWriteLen = strlen(pszWriteBuf);

	printk("*");	
	ulFlashLen = ATP_FLASH_GetFlashAreaSize(BSP_E_FLASH_AREA_LOG);
	printk("@");		
	printk("*");	
    lRet = ATP_FLASH_ReadFlashArea(BSP_E_FLASH_AREA_LOG, &stDummy, sizeof(ATP_Crash_FLASH_HEAD_ST), 0);
    if (0 != lRet)
    {
    	printk("ATP_Crash_RegWriteFlash ATP_FLASH_ReadFlashArea error1\r\n");
        //return 1;
    }
	printk("@");	
	stDummy.bNewCrash = 1;
	strcpy(stDummy.acTime, acCrashTime);
    if (0 != strcmp(stDummy.acMagic, ATP_CRASHDUMP_MAGIC_STRING))
    {
		memset(&stDummy, 0, sizeof(ATP_CRASHDUMP_MAGIC_STRING));
		strcpy(stDummy.acMagic, ATP_CRASHDUMP_MAGIC_STRING);	
		stDummy.ulFlashBuffLen = ulWriteLen + sizeof(ATP_Crash_FLASH_HEAD_ST);

		pstLogString = kmalloc(ulWriteLen,GFP_ATOMIC);	
		if(!pstLogString)
		{
			printk("ATP_Crash_RegWriteFlash malloc error\r\n");
			kfree(pszWriteBuf);
			return 1;
		}
		memset(pstLogString, 0, stDummy.ulFlashBuffLen);
		memcpy(pstLogString,&stDummy, sizeof(ATP_Crash_FLASH_HEAD_ST));
		memcpy(pstLogString+sizeof(ATP_Crash_FLASH_HEAD_ST),pszWriteBuf,ulWriteLen);
    }
	else
	{
		ulPreFlashLen = stDummy.ulFlashBuffLen;
		stDummy.ulFlashBuffLen = stDummy.ulFlashBuffLen + ulWriteLen;
		if(stDummy.ulFlashBuffLen > ulFlashLen)
		{
#if 0		
			bErase = 1;
			printk("ATP_Crash_RegWriteFlash buff full\r\n");
			if(!g_bWriteCrashFirstTime)
			{
				kfree(pszWriteBuf);
				return 1;
			}
			ATP_FLASH_EraseFlashArea(BSP_E_FLASH_AREA_LOG);
			stDummy.ulFlashBuffLen = sizeof(ATP_Crash_FLASH_HEAD_ST) + ulWriteLen;
#endif
			ulPreFlashLen = ulFlashLen - ulWriteLen;
			stDummy.ulFlashBuffLen = ulFlashLen;
		}

		printk("*");	
		pstLogString = kmalloc(stDummy.ulFlashBuffLen,GFP_ATOMIC);
		if(!pstLogString)
		{
			printk("ATP_Crash_RegWriteFlash malloc error2\r\n");
			kfree(pszWriteBuf);
			return 1;
		}
		printk("@");	
		memset(pstLogString, 0, stDummy.ulFlashBuffLen);
		if(!bErase)
		{					
			printk("*");	
			lRet = ATP_FLASH_ReadFlashArea(BSP_E_FLASH_AREA_LOG, pstLogString, ulPreFlashLen, 0);
			printk("@");	
		    if (0 != lRet)
		    {
		    	printk("ATP_Crash_RegWriteFlash ATP_FLASH_ReadFlashArea error\r\n");
				kfree(pstLogString);
				kfree(pszWriteBuf);
		        return 1;
		    }
#if 0			
			printk("ulThisHasAddLen=%d,ulWriteLen=%d, move destoffset=%d,srcoffset=%d, movelen=%d\r\n", 
				ulThisHasAddLen, ulWriteLen, sizeof(ATP_Crash_FLASH_HEAD_ST) + ulThisHasAddLen + ulWriteLen,
				sizeof(ATP_Crash_FLASH_HEAD_ST) + ulThisHasAddLen,ulPreFlashLen - sizeof(ATP_Crash_FLASH_HEAD_ST) - ulThisHasAddLen);
#endif
			printk("*");
			memmove(pstLogString + sizeof(ATP_Crash_FLASH_HEAD_ST) + ulThisHasAddLen + ulWriteLen, pstLogString + sizeof(ATP_Crash_FLASH_HEAD_ST) + ulThisHasAddLen, ulPreFlashLen - sizeof(ATP_Crash_FLASH_HEAD_ST) - ulThisHasAddLen);
			printk("@");
		}
		else
		{
			ulPreFlashLen = sizeof(ATP_Crash_FLASH_HEAD_ST);
		}
		
		memcpy(pstLogString, &stDummy, sizeof(ATP_Crash_FLASH_HEAD_ST));	
		memcpy(pstLogString + sizeof(ATP_Crash_FLASH_HEAD_ST) + ulThisHasAddLen,pszWriteBuf,ulWriteLen);

		
		
	}
    
	if(g_bWriteCrashFirstTime)
	{
		g_bWriteCrashFirstTime = 0;
	}
	
	ulLogLen = 	stDummy.ulFlashBuffLen;

    ulThisHasAddLen += ulWriteLen;
	if(ulThisHasAddLen + sizeof(ATP_Crash_FLASH_HEAD_ST) > ulFlashLen)
	{
		ulThisHasAddLen = ulFlashLen - sizeof(ATP_Crash_FLASH_HEAD_ST);
	}
	
    ulFlashLen = ulFlashLen > ulLogLen ? ulLogLen : ulFlashLen;
	stDummy.ulFlashBuffLen = ulFlashLen;
	printk("Crash WriteLen=%d\r\n", ulFlashLen);
	memcpy(pstLogString,&stDummy, sizeof(ATP_Crash_FLASH_HEAD_ST));
    //lRet = bhalWriteFlash((char*)pstLogString, BHAL_FLASH_BASE + BHAL_LOG_AREA_START_OFFSET, ulFlashLen);
	//lRet = ATP_FLASH_WriteFlashArea(BSP_E_FLASH_AREA_LOG, pstLogString, ulFlashLen, 0); 	
    if (0 != lRet)
    {
    	printk("ATP_Crash_RegWriteFlash write error\r\n");
		kfree(pstLogString);
		kfree(pszWriteBuf);
        return 1;
    }
    printk("ATP_Crash_RegWriteFlash success\r\n");	
	
	kfree(pstLogString);
	kfree(pszWriteBuf);
    return 0;

}

/*Start of Change by z00175982 for supplementary exceptional reboot 2010-12-24 DTS2010101601212 */
EXPORT_SYMBOL(ATP_Krnl_Crash_RegWriteFlash);
EXPORT_SYMBOL(g_acWriteCrashBuf);
/*End of Change by z00175982 for supplementary exceptional reboot 2010-12-24 DTS2010101601212 */

#endif
/*end add by q00122007 2010-06-25*/
/**
 * out_of_memory - kill the "best" process when we run out of memory
 * @zonelist: zonelist pointer
 * @gfp_mask: memory allocation flags
 * @order: amount of memory being requested as a power of 2
 *
 * If we run out of memory, we have the choice between either
 * killing a random task (bad), letting the system crash (worse)
 * OR try to be smart about which process to kill. Note that we
 * don't have to be perfect here, we just have to be good.
 */
void out_of_memory(struct zonelist *zonelist, gfp_t gfp_mask, int order)
{
#if defined (CONFIG_MIPS_BRCM)
#define OOM_REBOOT_DELAY (5)
#define OOM_REBOOT_INTERVAL (HZ*120)
	static int oom_count=0;
	static unsigned long oom_timestamp=0;
#else
	enum oom_constraint constraint;
#endif
	unsigned long freed = 0;

	blocking_notifier_call_chain(&oom_notify_list, 0, &freed);
	if (freed > 0)
		/* Got some memory back in the last second. */
		return;
#if defined (CONFIG_MIPS_BRCM)

	/* For our embedded system, most of the processes are considered essential. */
	/* Randomly killing a process is no better than a reboot so we won't kill process here*/
	
	printk(KERN_WARNING "\n\n%s triggered out of memory codition (oom killer not called): "
		"gfp_mask=0x%x, order=%d, oomkilladj=%d\n\n",
		current->comm, gfp_mask, order, current->oomkilladj);
#ifdef CONFIG_CRASH_DUMPFILE	
	memset(g_acWriteCrashBuf, 0, sizeof(g_acWriteCrashBuf));
	sprintf(g_acWriteCrashBuf,"\n\n%s triggered out of memory codition (oom killer not called): "
		"gfp_mask=0x%x, order=%d, oomkilladj=%d\n\n",
		current->comm, gfp_mask, order, current->oomkilladj);	
	
#endif
	dump_stack();
	show_mem();
	printk("\n");
	/*
	 * The process that triggered the oom is not necessarily the one that
	 * caused it.  dump_tasks shows all tasks and their memory usage.
	 */
	read_lock(&tasklist_lock);
	dump_tasks(NULL);
	read_unlock(&tasklist_lock);

	/* Reboot if OOM, but don't do it immediately - just in case this can be too sensitive */
	if ((jiffies - oom_timestamp) > OOM_REBOOT_INTERVAL) {
		oom_timestamp = jiffies;
		oom_count = 0;		
	}
	else {
		oom_count++;
		if (oom_count >= OOM_REBOOT_DELAY) {
#ifdef CONFIG_CRASH_DUMPFILE				
		snprintf(g_acWriteCrashBuf, 2048, "%sReboot due to persistent out of memory codition..", g_acWriteCrashBuf);			
		ATP_Krnl_Crash_RegWriteFlash(g_acWriteCrashBuf);
#endif			
			panic("Reboot due to persistent out of memory codition..");
		}
	}
#ifdef CONFIG_CRASH_DUMPFILE	
	ATP_Krnl_Crash_RegWriteFlash(g_acWriteCrashBuf);	
#endif		
	schedule_timeout_interruptible(HZ*5);

#else

	if (sysctl_panic_on_oom == 2)
		panic("out of memory. Compulsory panic_on_oom is selected.\n");

	/*
	 * Check if there were limitations on the allocation (only relevant for
	 * NUMA) that may require different handling.
	 */
	constraint = constrained_alloc(zonelist, gfp_mask);
	read_lock(&tasklist_lock);

	switch (constraint) {
	case CONSTRAINT_MEMORY_POLICY:
		oom_kill_process(current, gfp_mask, order, 0, NULL,
				"No available memory (MPOL_BIND)");
		break;

	case CONSTRAINT_NONE:
		if (sysctl_panic_on_oom)
			panic("out of memory. panic_on_oom is selected\n");
		/* Fall-through */
	case CONSTRAINT_CPUSET:
		__out_of_memory(gfp_mask, order);
		break;
	}

	read_unlock(&tasklist_lock);

	/*
	 * Give "p" a good chance of killing itself before we
	 * retry to allocate memory unless "p" is current
	 */
	if (!test_thread_flag(TIF_MEMDIE))
		schedule_timeout_uninterruptible(1);
#endif
}
