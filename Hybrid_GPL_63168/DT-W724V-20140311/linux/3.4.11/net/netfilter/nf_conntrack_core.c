/* Connection state tracking for netfilter.  This is separated from,
   but required by, the NAT layer; it can also be used by an iptables
   extension. */

/* (C) 1999-2001 Paul `Rusty' Russell
 * (C) 2002-2006 Netfilter Core Team <coreteam@netfilter.org>
 * (C) 2003,2004 USAGI/WIDE Project <http://www.linux-ipv6.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/types.h>
#include <linux/netfilter.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/skbuff.h>
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>
#include <linux/stddef.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/jhash.h>
#include <linux/err.h>
#include <linux/percpu.h>
#include <linux/moduleparam.h>
#include <linux/notifier.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/socket.h>
#include <linux/mm.h>
#include <linux/nsproxy.h>
#include <linux/rculist_nulls.h>
#if defined(CONFIG_BCM_KF_BLOG)
#include <linux/blog.h>
#include <linux/iqos.h>
#endif

#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_l3proto.h>
#include <net/netfilter/nf_conntrack_l4proto.h>
#include <net/netfilter/nf_conntrack_expect.h>
#include <net/netfilter/nf_conntrack_helper.h>
#include <net/netfilter/nf_conntrack_core.h>
#include <net/netfilter/nf_conntrack_extend.h>
#include <net/netfilter/nf_conntrack_acct.h>
#include <net/netfilter/nf_conntrack_ecache.h>
#include <net/netfilter/nf_conntrack_zones.h>
#include <net/netfilter/nf_conntrack_timestamp.h>
#include <net/netfilter/nf_conntrack_timeout.h>
#include <net/netfilter/nf_nat.h>
#include <net/netfilter/nf_nat_core.h>

#include <linux/ip.h>
#ifdef CONFIG_ATP_HYBRID
#include <net/dsfield.h>
#include <linux/netfilter/xt_dscp.h>
#endif

/*start of 问题单:AU4D00925，连接跟踪信息清空。by 00126165 2009-10-25*/
#ifdef CONFIG_ATP_CONNTRACK_CLEAN
#define WAN_UP_TIMER_OUT           2
struct timer_list 		wan_up_timer;        /*定时器*/
static void nf_conntrack_user_cleanup(struct net *net);

int nf_conntrack_clean __read_mostly;
EXPORT_SYMBOL_GPL(nf_conntrack_clean);
#endif
/*end of 问题单:AU4D00925，连接跟踪信息清空。by 00126165 2009-10-25*/

#if defined(CONFIG_BCM_KF_RUNNER)
#if defined(CONFIG_BCM_RDPA) || defined(CONFIG_BCM_RDPA_MODULE)
#include <net/bl_ops.h>
#endif /* CONFIG_BCM_RUNNER */
#endif /* CONFIG_BCM_KF_RUNNER */


#define NF_CONNTRACK_VERSION	"0.5.0"

int (*nfnetlink_parse_nat_setup_hook)(struct nf_conn *ct,
				      enum nf_nat_manip_type manip,
				      const struct nlattr *attr) __read_mostly;
EXPORT_SYMBOL_GPL(nfnetlink_parse_nat_setup_hook);

#ifdef CONFIG_ATP_HYBRID_GREACCEL
hi_nf_conntrack_death_hook pf_hi_nf_conntrack_death_hook;
unsigned int hi_nf_conntrack_register_death_hook(hi_nf_conntrack_death_hook hook)
{
    pf_hi_nf_conntrack_death_hook = hook;
    return 0;
}
EXPORT_SYMBOL(hi_nf_conntrack_register_death_hook);
#endif



DEFINE_SPINLOCK(nf_conntrack_lock);
EXPORT_SYMBOL_GPL(nf_conntrack_lock);

unsigned int nf_conntrack_htable_size __read_mostly;
EXPORT_SYMBOL_GPL(nf_conntrack_htable_size);

unsigned int nf_conntrack_max __read_mostly;
EXPORT_SYMBOL_GPL(nf_conntrack_max);

DEFINE_PER_CPU(struct nf_conn, nf_conntrack_untracked);
EXPORT_PER_CPU_SYMBOL(nf_conntrack_untracked);

unsigned int nf_conntrack_hash_rnd __read_mostly;
EXPORT_SYMBOL_GPL(nf_conntrack_hash_rnd);

#if defined(CONFIG_BCM_KF_NETFILTER)
/* bugfix for lost connection */
LIST_HEAD(lo_safe_list);
LIST_HEAD(hi_safe_list);
#endif

#ifdef CONFIG_ATP_CONNTRACK_CLEAN
/*add by luokunling ,l00192527 ,2013/2/1 记录当前fon登录用户lan IP*/
unsigned int nf_conntrack_fonip = 0x00000000;
#endif

static u32 hash_conntrack_raw(const struct nf_conntrack_tuple *tuple, u16 zone)
{
	unsigned int n;

	/* The direction must be ignored, so we hash everything up to the
	 * destination ports (which is a multiple of 4) and treat the last
	 * three bytes manually.
	 */
	n = (sizeof(tuple->src) + sizeof(tuple->dst.u3)) / sizeof(u32);
	return jhash2((u32 *)tuple, n, zone ^ nf_conntrack_hash_rnd ^
		      (((__force __u16)tuple->dst.u.all << 16) |
			  tuple->dst.protonum));
}

static u32 __hash_bucket(u32 hash, unsigned int size)
{
	return ((u64)hash * size) >> 32;
}

static u32 hash_bucket(u32 hash, const struct net *net)
{
	return __hash_bucket(hash, net->ct.htable_size);
}

static u_int32_t __hash_conntrack(const struct nf_conntrack_tuple *tuple,
				  u16 zone, unsigned int size)
{
	return __hash_bucket(hash_conntrack_raw(tuple, zone), size);
}

static inline u_int32_t hash_conntrack(const struct net *net, u16 zone,
				       const struct nf_conntrack_tuple *tuple)
{
	return __hash_conntrack(tuple, zone, net->ct.htable_size);
}

bool
nf_ct_get_tuple(const struct sk_buff *skb,
		unsigned int nhoff,
		unsigned int dataoff,
		u_int16_t l3num,
		u_int8_t protonum,
		struct nf_conntrack_tuple *tuple,
		const struct nf_conntrack_l3proto *l3proto,
		const struct nf_conntrack_l4proto *l4proto)
{
	memset(tuple, 0, sizeof(*tuple));

	tuple->src.l3num = l3num;
	if (l3proto->pkt_to_tuple(skb, nhoff, tuple) == 0)
		return false;

	tuple->dst.protonum = protonum;
	tuple->dst.dir = IP_CT_DIR_ORIGINAL;

	return l4proto->pkt_to_tuple(skb, dataoff, tuple);
}
EXPORT_SYMBOL_GPL(nf_ct_get_tuple);

bool nf_ct_get_tuplepr(const struct sk_buff *skb, unsigned int nhoff,
		       u_int16_t l3num, struct nf_conntrack_tuple *tuple)
{
	struct nf_conntrack_l3proto *l3proto;
	struct nf_conntrack_l4proto *l4proto;
	unsigned int protoff;
	u_int8_t protonum;
	int ret;

	rcu_read_lock();

	l3proto = __nf_ct_l3proto_find(l3num);
	ret = l3proto->get_l4proto(skb, nhoff, &protoff, &protonum);
	if (ret != NF_ACCEPT) {
		rcu_read_unlock();
		return false;
	}

	l4proto = __nf_ct_l4proto_find(l3num, protonum);

	ret = nf_ct_get_tuple(skb, nhoff, protoff, l3num, protonum, tuple,
			      l3proto, l4proto);

	rcu_read_unlock();
	return ret;
}
EXPORT_SYMBOL_GPL(nf_ct_get_tuplepr);

bool
nf_ct_invert_tuple(struct nf_conntrack_tuple *inverse,
		   const struct nf_conntrack_tuple *orig,
		   const struct nf_conntrack_l3proto *l3proto,
		   const struct nf_conntrack_l4proto *l4proto)
{
	memset(inverse, 0, sizeof(*inverse));

	inverse->src.l3num = orig->src.l3num;
	if (l3proto->invert_tuple(inverse, orig) == 0)
		return false;

	inverse->dst.dir = !orig->dst.dir;

	inverse->dst.protonum = orig->dst.protonum;
	return l4proto->invert_tuple(inverse, orig);
}
EXPORT_SYMBOL_GPL(nf_ct_invert_tuple);

static void
clean_from_lists(struct nf_conn *ct)
{
	pr_debug("clean_from_lists(%p)\n", ct);
	hlist_nulls_del_rcu(&ct->tuplehash[IP_CT_DIR_ORIGINAL].hnnode);
	hlist_nulls_del_rcu(&ct->tuplehash[IP_CT_DIR_REPLY].hnnode);

	/* Destroy all pending expectations */
	nf_ct_remove_expectations(ct);
}

#if defined(CONFIG_BCM_KF_NETFILTER)
void death_by_timeout(unsigned long ul_conntrack);
#endif
#if defined(CONFIG_BCM_KF_BLOG) && defined(CONFIG_BCM_KF_NETFILTER) && defined(CONFIG_BLOG)
static void blog_death_by_timeout(unsigned long ul_conntrack);
#endif

static void
destroy_conntrack(struct nf_conntrack *nfct)
{
	struct nf_conn *ct = (struct nf_conn *)nfct;
	struct net *net = nf_ct_net(ct);
	struct nf_conntrack_l4proto *l4proto;


#if defined(CONFIG_BCM_KF_BLOG) && defined(CONFIG_BLOG)
	pr_debug("destroy_conntrack(%p) blog keys[0x%08x,0x%08x]\n",
		ct, ct->blog_key[IP_CT_DIR_ORIGINAL], ct->blog_key[IP_CT_DIR_REPLY]);


	/* Conntrack going away, notify blog client */
	if ( (ct->blog_key[IP_CT_DIR_ORIGINAL] != BLOG_KEY_NONE) || 
			(ct->blog_key[IP_CT_DIR_REPLY] != BLOG_KEY_NONE) )
	{
		/*
		 *  Blog client may perform the following blog requests:
		 *	- FLOWTRACK_KEY_SET BLOG_PARAM1_DIR_ORIG 0
		 *	- FLOWTRACK_KEY_SET BLOG_PARAM1_DIR_REPLY 0
		 *	- FLOWTRACK_EXCLUDE
		 */
		blog_notify(DESTROY_FLOWTRACK, (void*)ct,
					(uint32_t)ct->blog_key[IP_CT_DIR_ORIGINAL],
					(uint32_t)ct->blog_key[IP_CT_DIR_REPLY]);

		/* Safe: In case blog client does not set key to 0 explicilty */
		ct->blog_key[IP_CT_DIR_ORIGINAL] = BLOG_KEY_NONE;
		ct->blog_key[IP_CT_DIR_REPLY]    = BLOG_KEY_NONE;
		ct->prev_idle = 0;
	}
	clear_bit(IPS_BLOG_BIT, &ct->status);	/* Disable further blogging */
#else
	pr_debug("destroy_conntrack(%p)\n", ct);
#endif

	NF_CT_ASSERT(atomic_read(&nfct->use) == 0);
	NF_CT_ASSERT(!timer_pending(&ct->timeout));

	/* To make sure we don't get any weird locking issues here:
	 * destroy_conntrack() MUST NOT be called with a write lock
	 * to nf_conntrack_lock!!! -HW */
	rcu_read_lock();
	l4proto = __nf_ct_l4proto_find(nf_ct_l3num(ct), nf_ct_protonum(ct));
	if (l4proto && l4proto->destroy)
		l4proto->destroy(ct);

	rcu_read_unlock();

	spin_lock_bh(&nf_conntrack_lock);
	/* Expectations will have been removed in clean_from_lists,
	 * except TFTP can create an expectation on the first packet,
	 * before connection is in the list, so we need to clean here,
	 * too. */
	nf_ct_remove_expectations(ct);

#if defined(CONFIG_BCM_KF_RUNNER)
#if defined(CONFIG_BCM_RDPA) || defined(CONFIG_BCM_RDPA_MODULE)
	BL_OPS(net_netfilter_nf_conntrack_core_destroy_conntrack(ct));
#endif /* CONFIG_BCM_RUNNER */
#endif /* CONFIG_BCM_KF_RUNNER */
    
#if defined(CONFIG_BCM_KF_XT_MATCH_LAYER7) && \
	(defined(CONFIG_NETFILTER_XT_MATCH_LAYER7) || defined(CONFIG_NETFILTER_XT_MATCH_LAYER7_MODULE))
	if(ct->layer7.app_proto)
		kfree(ct->layer7.app_proto);
	if(ct->layer7.app_data)
		kfree(ct->layer7.app_data);
#endif

	/* We overload first tuple to link into unconfirmed list. */
	if (!nf_ct_is_confirmed(ct)) {
		BUG_ON(hlist_nulls_unhashed(&ct->tuplehash[IP_CT_DIR_ORIGINAL].hnnode));
		hlist_nulls_del_rcu(&ct->tuplehash[IP_CT_DIR_ORIGINAL].hnnode);
	}

	NF_CT_STAT_INC(net, delete);
	spin_unlock_bh(&nf_conntrack_lock);

	if (ct->master)
#if defined(CONFIG_BCM_KF_NETFILTER)
	{
	    /* start DTS2014032511045 conntrack lead to rcu pending by f00110348 */
#ifdef CONFIG_ATP_BRCM	
	    spin_lock_bh(&nf_conntrack_lock);
#endif
		list_del(&ct->derived_list);
#ifdef CONFIG_ATP_BRCM
        spin_unlock_bh(&nf_conntrack_lock);
#endif
        /* end DTS2014032511045 conntrack lead to rcu pending by f00110348 */
#endif
		nf_ct_put(ct->master);
#if defined(CONFIG_BCM_KF_NETFILTER)
	}
#endif

#if defined(CONFIG_BCM_KF_NETFILTER)
	/* Disconnect all child connections that have infinit timeout */
	if (!list_empty(&ct->derived_connections)) {
		struct nf_conn *child, *tmp;

		list_for_each_entry_safe(child, tmp, &ct->derived_connections,
			derived_list) {
			if (child->derived_timeout == 0xFFFFFFFF &&
			    del_timer(&child->timeout))
				death_by_timeout((unsigned long)child);
		}
	}
#endif

	pr_debug("destroy_conntrack: returning ct=%p to slab\n", ct);
	nf_conntrack_free(ct);
}

void nf_ct_delete_from_lists(struct nf_conn *ct)
{
	struct net *net = nf_ct_net(ct);

	nf_ct_helper_destroy(ct);
	spin_lock_bh(&nf_conntrack_lock);
	/* Inside lock so preempt is disabled on module removal path.
	 * Otherwise we can get spurious warnings. */
	NF_CT_STAT_INC(net, delete_list);
	clean_from_lists(ct);
	spin_unlock_bh(&nf_conntrack_lock);
}
EXPORT_SYMBOL_GPL(nf_ct_delete_from_lists);

static void death_by_event(unsigned long ul_conntrack)
{
	struct nf_conn *ct = (void *)ul_conntrack;
	struct net *net = nf_ct_net(ct);

	if (nf_conntrack_event(IPCT_DESTROY, ct) < 0) {
		/* bad luck, let's retry again */
		ct->timeout.expires = jiffies +
			(random32() % net->ct.sysctl_events_retry_timeout);
		add_timer(&ct->timeout);
		return;
	}
	/* we've got the event delivered, now it's dying */
	set_bit(IPS_DYING_BIT, &ct->status);
	spin_lock(&nf_conntrack_lock);
	hlist_nulls_del(&ct->tuplehash[IP_CT_DIR_ORIGINAL].hnnode);
	spin_unlock(&nf_conntrack_lock);
	nf_ct_put(ct);
}

void nf_ct_insert_dying_list(struct nf_conn *ct)
{
	struct net *net = nf_ct_net(ct);

	/* add this conntrack to the dying list */
	spin_lock_bh(&nf_conntrack_lock);
	hlist_nulls_add_head(&ct->tuplehash[IP_CT_DIR_ORIGINAL].hnnode,
			     &net->ct.dying);
	spin_unlock_bh(&nf_conntrack_lock);
	/* set a new timer to retry event delivery */
	setup_timer(&ct->timeout, death_by_event, (unsigned long)ct);
	ct->timeout.expires = jiffies +
		(random32() % net->ct.sysctl_events_retry_timeout);
	add_timer(&ct->timeout);
}
EXPORT_SYMBOL_GPL(nf_ct_insert_dying_list);

void death_by_timeout(unsigned long ul_conntrack)
{
	struct nf_conn *ct = (void *)ul_conntrack;
	struct nf_conn_tstamp *tstamp;

#if defined(CONFIG_BCM_KF_RUNNER)
#if defined(CONFIG_BCM_RDPA) || defined(CONFIG_BCM_RDPA_MODULE)
	BL_OPS_CR(net_netfilter_nf_conntrack_core_death_by_timeout(ct));
#endif /* CONFIG_BCM_RUNNER */
#endif /* CONFIG_BCM_KF_RUNNER */

#ifdef CONFIG_ATP_HYBRID_GREACCEL
    if (pf_hi_nf_conntrack_death_hook != NULL)
    {
        if (pf_hi_nf_conntrack_death_hook(ct, HI_NF_NORMAL_DEATH_HOOK) == 1)
        {
            printk("bug: should return 0");
        }
    }
#endif


	tstamp = nf_conn_tstamp_find(ct);
	if (tstamp && tstamp->stop == 0)
		tstamp->stop = ktime_to_ns(ktime_get_real());

	if (!test_bit(IPS_DYING_BIT, &ct->status) &&
	    unlikely(nf_conntrack_event(IPCT_DESTROY, ct) < 0)) {
		/* destroy event was not delivered */
		nf_ct_delete_from_lists(ct);
		nf_ct_insert_dying_list(ct);
		return;
	}
	set_bit(IPS_DYING_BIT, &ct->status);
	nf_ct_delete_from_lists(ct);
	nf_ct_put(ct);
}
EXPORT_SYMBOL_GPL(death_by_timeout);

#if defined(CONFIG_BCM_KF_BLOG) && defined(CONFIG_BCM_KF_NETFILTER) && defined(CONFIG_BLOG)
void __nf_ct_time(struct nf_conn *ct, BlogCtTime_t *ct_time_p)
{
	/* Cases:
	* a) conn has been active, prev_idle = 0, idle_jiffies = 0
	* b) conn becomes idle,  prev_idle = 0, idle_jiffies != 0
	* c) conn becomes idle in prev timeout and then becomes active again.
	*    prev_idle = 0, and idle_jiffies != 0.
	* d) conn was idle in prev timeout and is still idle.
	*    prev_idle != 0, and idle_jiffies != 0.
	*
	*    In the first three cases (a) to (c), timer should be restarted
	*    after adjustment for idle_jiffies.
	*
	*    In the last case (d), on expiry it is time to destroy the conn.
	*/
	if ((!ct->prev_idle) || (!ct_time_p->idle_jiffies)) {
		unsigned long newtime;

		if (timer_pending(&ct->timeout))
			del_timer(&ct->timeout);

		ct->prev_timeout.expires = ct->timeout.expires;
		newtime= jiffies + (ct_time_p->extra_jiffies - ct_time_p->idle_jiffies);
		ct->timeout.expires = newtime;
		add_timer(&ct->timeout);
		ct->prev_idle = ct_time_p->idle_jiffies;
	} else {
		if (timer_pending(&ct->timeout))
			del_timer(&ct->timeout);

		death_by_timeout((unsigned long) ct);
	}
}

static void blog_death_by_timeout(unsigned long ul_conntrack)
{
	struct nf_conn *ct = (void *)ul_conntrack;

#ifdef CONFIG_ATP_HYBRID_GREACCEL
    if (pf_hi_nf_conntrack_death_hook != NULL)
    {
        if (pf_hi_nf_conntrack_death_hook(ct, HI_NF_TIMEOUT_DEATH_HOOK) == 1)
        {
            ct->timeout.expires = jiffies + 10 * HZ;
            add_timer(&ct->timeout);
            return;
        }
    }
#endif

    if (ct->blog_key[BLOG_PARAM1_DIR_ORIG] != BLOG_KEY_NONE || 
		ct->blog_key[BLOG_PARAM1_DIR_REPLY] != BLOG_KEY_NONE) {
		BlogCtTime_t ct_time;

		blog_query(QUERY_FLOWTRACK, (void*)ct, 
			ct->blog_key[BLOG_PARAM1_DIR_ORIG],
			ct->blog_key[BLOG_PARAM1_DIR_REPLY], (uint32_t) &ct_time);

		__nf_ct_time(ct, &ct_time);
	} else {
		if (timer_pending(&ct->timeout))
			del_timer(&ct->timeout);

		death_by_timeout((unsigned long) ct);
	}
}

void __nf_ct_time_update(struct nf_conn *ct, BlogCtTime_t *ct_time_p)
{
	unsigned long newtime;

	if (!timer_pending(&ct->timeout))
    	return;

	if (ct->blog_key[BLOG_PARAM1_DIR_ORIG] != BLOG_KEY_NONE || 
		ct->blog_key[BLOG_PARAM1_DIR_REPLY] != BLOG_KEY_NONE) {
        ct->prev_idle = 0;

        del_timer(&ct->timeout);

    	newtime = jiffies + (ct_time_p->extra_jiffies - ct_time_p->idle_jiffies);
		ct->prev_timeout.expires = jiffies;
		ct->timeout.expires = newtime;
		add_timer(&ct->timeout);
    }
}
#endif

/*
 * Warning :
 * - Caller must take a reference on returned object
 *   and recheck nf_ct_tuple_equal(tuple, &h->tuple)
 * OR
 * - Caller must lock nf_conntrack_lock before calling this function
 */
static struct nf_conntrack_tuple_hash *
____nf_conntrack_find(struct net *net, u16 zone,
		      const struct nf_conntrack_tuple *tuple, u32 hash)
{
	struct nf_conntrack_tuple_hash *h;
	struct hlist_nulls_node *n;
	unsigned int bucket = hash_bucket(hash, net);

	/* Disable BHs the entire time since we normally need to disable them
	 * at least once for the stats anyway.
	 */
	local_bh_disable();
begin:
	hlist_nulls_for_each_entry_rcu(h, n, &net->ct.hash[bucket], hnnode) {
		if (nf_ct_tuple_equal(tuple, &h->tuple) &&
		    nf_ct_zone(nf_ct_tuplehash_to_ctrack(h)) == zone) {
			NF_CT_STAT_INC(net, found);
			local_bh_enable();
			return h;
		}
		NF_CT_STAT_INC(net, searched);
	}
	/*
	 * if the nulls value we got at the end of this lookup is
	 * not the expected one, we must restart lookup.
	 * We probably met an item that was moved to another chain.
	 */
	if (get_nulls_value(n) != bucket) {
		NF_CT_STAT_INC(net, search_restart);
		goto begin;
	}
	local_bh_enable();

	return NULL;
}

struct nf_conntrack_tuple_hash *
__nf_conntrack_find(struct net *net, u16 zone,
		    const struct nf_conntrack_tuple *tuple)
{
	return ____nf_conntrack_find(net, zone, tuple,
				     hash_conntrack_raw(tuple, zone));
}
EXPORT_SYMBOL_GPL(__nf_conntrack_find);

/* Find a connection corresponding to a tuple. */
static struct nf_conntrack_tuple_hash *
__nf_conntrack_find_get(struct net *net, u16 zone,
			const struct nf_conntrack_tuple *tuple, u32 hash)
{
	struct nf_conntrack_tuple_hash *h;
	struct nf_conn *ct;

	rcu_read_lock();
begin:
	h = ____nf_conntrack_find(net, zone, tuple, hash);
	if (h) {
		ct = nf_ct_tuplehash_to_ctrack(h);
		if (unlikely(nf_ct_is_dying(ct) ||
			     !atomic_inc_not_zero(&ct->ct_general.use)))
			h = NULL;
		else {
			if (unlikely(!nf_ct_tuple_equal(tuple, &h->tuple) ||
				     nf_ct_zone(ct) != zone)) {
				nf_ct_put(ct);
				goto begin;
			}
		}
	}
	rcu_read_unlock();

	return h;
}

struct nf_conntrack_tuple_hash *
nf_conntrack_find_get(struct net *net, u16 zone,
		      const struct nf_conntrack_tuple *tuple)
{
	return __nf_conntrack_find_get(net, zone, tuple,
				       hash_conntrack_raw(tuple, zone));
}
EXPORT_SYMBOL_GPL(nf_conntrack_find_get);

static void __nf_conntrack_hash_insert(struct nf_conn *ct,
				       unsigned int hash,
				       unsigned int repl_hash)
{
	struct net *net = nf_ct_net(ct);

	hlist_nulls_add_head_rcu(&ct->tuplehash[IP_CT_DIR_ORIGINAL].hnnode,
			   &net->ct.hash[hash]);
	hlist_nulls_add_head_rcu(&ct->tuplehash[IP_CT_DIR_REPLY].hnnode,
			   &net->ct.hash[repl_hash]);
}

int
nf_conntrack_hash_check_insert(struct nf_conn *ct)
{
	struct net *net = nf_ct_net(ct);
	unsigned int hash, repl_hash;
	struct nf_conntrack_tuple_hash *h;
	struct hlist_nulls_node *n;
	u16 zone;

	zone = nf_ct_zone(ct);
	hash = hash_conntrack(net, zone,
			      &ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple);
	repl_hash = hash_conntrack(net, zone,
				   &ct->tuplehash[IP_CT_DIR_REPLY].tuple);

	spin_lock_bh(&nf_conntrack_lock);

	/* See if there's one in the list already, including reverse */
	hlist_nulls_for_each_entry(h, n, &net->ct.hash[hash], hnnode)
		if (nf_ct_tuple_equal(&ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple,
				      &h->tuple) &&
		    zone == nf_ct_zone(nf_ct_tuplehash_to_ctrack(h)))
			goto out;
	hlist_nulls_for_each_entry(h, n, &net->ct.hash[repl_hash], hnnode)
		if (nf_ct_tuple_equal(&ct->tuplehash[IP_CT_DIR_REPLY].tuple,
				      &h->tuple) &&
		    zone == nf_ct_zone(nf_ct_tuplehash_to_ctrack(h)))
			goto out;

	add_timer(&ct->timeout);
	nf_conntrack_get(&ct->ct_general);
	__nf_conntrack_hash_insert(ct, hash, repl_hash);
	NF_CT_STAT_INC(net, insert);
	spin_unlock_bh(&nf_conntrack_lock);

	return 0;

out:
	NF_CT_STAT_INC(net, insert_failed);
	spin_unlock_bh(&nf_conntrack_lock);
	return -EEXIST;
}
EXPORT_SYMBOL_GPL(nf_conntrack_hash_check_insert);

/* Confirm a connection given skb; places it in hash table */
int
__nf_conntrack_confirm(struct sk_buff *skb)
{
	unsigned int hash, repl_hash;
	struct nf_conntrack_tuple_hash *h;
	struct nf_conn *ct;
	struct nf_conn_help *help;
	struct nf_conn_tstamp *tstamp;
	struct hlist_nulls_node *n;
	enum ip_conntrack_info ctinfo;
	struct net *net;
	u16 zone;

	ct = nf_ct_get(skb, &ctinfo);
	net = nf_ct_net(ct);

	/* ipt_REJECT uses nf_conntrack_attach to attach related
	   ICMP/TCP RST packets in other direction.  Actual packet
	   which created connection will be IP_CT_NEW or for an
	   expected connection, IP_CT_RELATED. */
	if (CTINFO2DIR(ctinfo) != IP_CT_DIR_ORIGINAL)
		return NF_ACCEPT;

	zone = nf_ct_zone(ct);
	/* reuse the hash saved before */
	hash = *(unsigned long *)&ct->tuplehash[IP_CT_DIR_REPLY].hnnode.pprev;
	hash = hash_bucket(hash, net);
	repl_hash = hash_conntrack(net, zone,
				   &ct->tuplehash[IP_CT_DIR_REPLY].tuple);

	/* We're not in hash table, and we refuse to set up related
	   connections for unconfirmed conns.  But packet copies and
	   REJECT will give spurious warnings here. */
	/* NF_CT_ASSERT(atomic_read(&ct->ct_general.use) == 1); */

	/* No external references means no one else could have
	   confirmed us. */
	NF_CT_ASSERT(!nf_ct_is_confirmed(ct));
	pr_debug("Confirming conntrack %p\n", ct);

	spin_lock_bh(&nf_conntrack_lock);

	/* We have to check the DYING flag inside the lock to prevent
	   a race against nf_ct_get_next_corpse() possibly called from
	   user context, else we insert an already 'dead' hash, blocking
	   further use of that particular connection -JM */

	if (unlikely(nf_ct_is_dying(ct))) {
		spin_unlock_bh(&nf_conntrack_lock);
		return NF_ACCEPT;
	}

	/* See if there's one in the list already, including reverse:
	   NAT could have grabbed it without realizing, since we're
	   not in the hash.  If there is, we lost race. */
	hlist_nulls_for_each_entry(h, n, &net->ct.hash[hash], hnnode)
		if (nf_ct_tuple_equal(&ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple,
				      &h->tuple) &&
		    zone == nf_ct_zone(nf_ct_tuplehash_to_ctrack(h)))
			goto out;
	hlist_nulls_for_each_entry(h, n, &net->ct.hash[repl_hash], hnnode)
		if (nf_ct_tuple_equal(&ct->tuplehash[IP_CT_DIR_REPLY].tuple,
				      &h->tuple) &&
		    zone == nf_ct_zone(nf_ct_tuplehash_to_ctrack(h)))
			goto out;

	/* Remove from unconfirmed list */
	hlist_nulls_del_rcu(&ct->tuplehash[IP_CT_DIR_ORIGINAL].hnnode);

	/* Timer relative to confirmation time, not original
	   setting time, otherwise we'd get timer wrap in
	   weird delay cases. */
	ct->timeout.expires += jiffies;
	add_timer(&ct->timeout);

#if defined(CONFIG_BCM_KF_RUNNER)
#if defined(CONFIG_BCM_RDPA) || defined(CONFIG_BCM_RDPA_MODULE)
		BL_OPS(net_netfilter_nf_conntrack_core_nf_conntrack_confirm(ct, skb));
#endif /* CONFIG_BCM_RUNNER */
#endif /* CONFIG_BCM_KF_RUNNER */

    
	atomic_inc(&ct->ct_general.use);
	ct->status |= IPS_CONFIRMED;

	/* set conntrack timestamp, if enabled. */
	tstamp = nf_conn_tstamp_find(ct);
	if (tstamp) {
		if (skb->tstamp.tv64 == 0)
			__net_timestamp((struct sk_buff *)skb);

		tstamp->start = ktime_to_ns(skb->tstamp);
	}
	/* Since the lookup is lockless, hash insertion must be done after
	 * starting the timer and setting the CONFIRMED bit. The RCU barriers
	 * guarantee that no other CPU can find the conntrack before the above
	 * stores are visible.
	 */
	__nf_conntrack_hash_insert(ct, hash, repl_hash);
	NF_CT_STAT_INC(net, insert);
	spin_unlock_bh(&nf_conntrack_lock);

	help = nfct_help(ct);
	if (help && help->helper)
		nf_conntrack_event_cache(IPCT_HELPER, ct);

	nf_conntrack_event_cache(master_ct(ct) ?
				 IPCT_RELATED : IPCT_NEW, ct);
	return NF_ACCEPT;

out:
	NF_CT_STAT_INC(net, insert_failed);
	spin_unlock_bh(&nf_conntrack_lock);
	return NF_DROP;
}
EXPORT_SYMBOL_GPL(__nf_conntrack_confirm);

/* Returns true if a connection correspondings to the tuple (required
   for NAT). */
int
nf_conntrack_tuple_taken(const struct nf_conntrack_tuple *tuple,
			 const struct nf_conn *ignored_conntrack)
{
	struct net *net = nf_ct_net(ignored_conntrack);
	struct nf_conntrack_tuple_hash *h;
	struct hlist_nulls_node *n;
	struct nf_conn *ct;
	u16 zone = nf_ct_zone(ignored_conntrack);
	unsigned int hash = hash_conntrack(net, zone, tuple);

	/* Disable BHs the entire time since we need to disable them at
	 * least once for the stats anyway.
	 */
	rcu_read_lock_bh();
	hlist_nulls_for_each_entry_rcu(h, n, &net->ct.hash[hash], hnnode) {
		ct = nf_ct_tuplehash_to_ctrack(h);
		if (ct != ignored_conntrack &&
		    nf_ct_tuple_equal(tuple, &h->tuple) &&
		    nf_ct_zone(ct) == zone) {
			NF_CT_STAT_INC(net, found);
			rcu_read_unlock_bh();
			return 1;
		}
		NF_CT_STAT_INC(net, searched);
	}
	rcu_read_unlock_bh();

	return 0;
}
EXPORT_SYMBOL_GPL(nf_conntrack_tuple_taken);

#if defined(CONFIG_BCM_KF_NETFILTER)
static int regardless_drop(struct net *net, struct sk_buff *skb)
{
	struct nf_conn *ct = NULL;
	struct list_head *tmp;
	int dropped = 0;

	/* Choose the first one (also the oldest one). LRU */
	spin_lock_bh(&nf_conntrack_lock);
	if (!list_empty(&lo_safe_list)) {
		list_for_each(tmp, &lo_safe_list) {
			ct = container_of(tmp, struct nf_conn, safe_list);
			if (unlikely(!atomic_inc_not_zero(&ct->ct_general.use)))
				ct = NULL;

			if (ct)
				break;
		}
	}

	if (!ct && (blog_iq(skb) == IQOS_PRIO_HIGH) ) {
		list_for_each(tmp, &hi_safe_list) {
			ct = container_of(tmp, struct nf_conn, safe_list);
			if (unlikely(!atomic_inc_not_zero(&ct->ct_general.use)))
				ct = NULL;

			if (ct)
				break;
		}
	}
	spin_unlock_bh(&nf_conntrack_lock);

	if (!ct) {
		return dropped;
	}

	if (del_timer(&ct->timeout)) {
		death_by_timeout((unsigned long)ct);
		dropped = 1;
		NF_CT_STAT_INC_ATOMIC(net, early_drop);
	} 
	/*else{
	* this happens when the ct at safelist head is removed from the timer list 
	* but not yet freed due to ct->ct_general.use > 1. This ct will be freed when its
	* ref count is dropped to zero. At this point we dont create new connections 
	* until some old connection are freed.
	* } 
	*/     

	nf_ct_put(ct);
	return dropped;
}
#else
#define NF_CT_EVICTION_RANGE	8

/* There's a small race here where we may free a just-assured
   connection.  Too bad: we're in trouble anyway. */
static noinline int early_drop(struct net *net, unsigned int hash)
{
	/* Use oldest entry, which is roughly LRU */
	struct nf_conntrack_tuple_hash *h;
	struct nf_conn *ct = NULL, *tmp;
	struct hlist_nulls_node *n;
	unsigned int i, cnt = 0;
	int dropped = 0;

	rcu_read_lock();
	for (i = 0; i < net->ct.htable_size; i++) {
		hlist_nulls_for_each_entry_rcu(h, n, &net->ct.hash[hash],
					 hnnode) {
			tmp = nf_ct_tuplehash_to_ctrack(h);
			if (!test_bit(IPS_ASSURED_BIT, &tmp->status))
				ct = tmp;
			cnt++;
		}

		if (ct != NULL) {
			if (likely(!nf_ct_is_dying(ct) &&
				   atomic_inc_not_zero(&ct->ct_general.use)))
				break;
			else
			ct = NULL;
		}

		if (cnt >= NF_CT_EVICTION_RANGE)
			break;

		hash = (hash + 1) % net->ct.htable_size;
	}
	rcu_read_unlock();

	if (!ct)
		return dropped;

	if (del_timer(&ct->timeout)) {
		death_by_timeout((unsigned long)ct);
		/* Check if we indeed killed this entry. Reliable event
		   delivery may have inserted it into the dying list. */
		if (test_bit(IPS_DYING_BIT, &ct->status)) {
			dropped = 1;
			NF_CT_STAT_INC_ATOMIC(net, early_drop);
		}
	}
	nf_ct_put(ct);
	return dropped;
}
#endif

void init_nf_conntrack_hash_rnd(void)
{
	unsigned int rand;

	/*
	 * Why not initialize nf_conntrack_rnd in a "init()" function ?
	 * Because there isn't enough entropy when system initializing,
	 * and we initialize it as late as possible.
   */     
	do {
		get_random_bytes(&rand, sizeof(rand));
	} while (!rand);
	cmpxchg(&nf_conntrack_hash_rnd, 0, rand);
}


#if defined(CONFIG_BCM_KF_NETFILTER)
static struct nf_conn *
__nf_conntrack_alloc(struct net *net, u16 zone, 
				   struct sk_buff *skb,
				   const struct nf_conntrack_tuple *orig,
				   const struct nf_conntrack_tuple *repl,
		     gfp_t gfp, u32 hash)
#else
static struct nf_conn *
__nf_conntrack_alloc(struct net *net, u16 zone,
				   const struct nf_conntrack_tuple *orig,
				   const struct nf_conntrack_tuple *repl,
		     gfp_t gfp, u32 hash)
#endif
{
	struct nf_conn *ct;

	if (unlikely(!nf_conntrack_hash_rnd)) {
		init_nf_conntrack_hash_rnd();
		/* recompute the hash as nf_conntrack_hash_rnd is initialized */
		hash = hash_conntrack_raw(orig, zone);
	}

	/* We don't want any race condition at early drop stage */
	atomic_inc(&net->ct.count);

	if (nf_conntrack_max &&
	    unlikely(atomic_read(&net->ct.count) > nf_conntrack_max)) {
#if defined(CONFIG_BCM_KF_NETFILTER)
        /* Sorry, we have to kick LRU out regardlessly. */
		if (!regardless_drop(net, skb)) {
				atomic_dec(&net->ct.count);
			if (net_ratelimit())
			printk(KERN_WARNING
				"nf_conntrack: table full, dropping"
				" packet.\n");
			return ERR_PTR(-ENOMEM);
		}
#else       

		if (!early_drop(net, hash_bucket(hash, net))) {
			atomic_dec(&net->ct.count);
			if (net_ratelimit())
				printk(KERN_WARNING
						"nf_conntrack: table full, dropping"
						" packet.\n");
				return ERR_PTR(-ENOMEM);
			}
#endif
	}

	/*
	 * Do not use kmem_cache_zalloc(), as this cache uses
	 * SLAB_DESTROY_BY_RCU.
	 */
	ct = kmem_cache_alloc(net->ct.nf_conntrack_cachep, gfp);
	if (ct == NULL) {
		atomic_dec(&net->ct.count);
		return ERR_PTR(-ENOMEM);
	}

#if defined(CONFIG_BCM_KF_NETFILTER)
	INIT_LIST_HEAD(&ct->safe_list);
	INIT_LIST_HEAD(&ct->derived_connections);
	INIT_LIST_HEAD(&ct->derived_list);
	ct->derived_timeout = 0;
#endif

	/*
	 * Let ct->tuplehash[IP_CT_DIR_ORIGINAL].hnnode.next
	 * and ct->tuplehash[IP_CT_DIR_REPLY].hnnode.next unchanged.
	 */
	memset(&ct->tuplehash[IP_CT_DIR_MAX], 0,
	       offsetof(struct nf_conn, proto) -
	       offsetof(struct nf_conn, tuplehash[IP_CT_DIR_MAX]));
#if defined(CONFIG_BCM_KF_NETFILTER)
	/* Broadcom changed the position of these two fields.  They used to be
	   in the area being memset to 0 */
	ct->master = 0;
	ct->status = 0;
#endif

#if defined(CONFIG_BCM_KF_BLOG) && defined(CONFIG_BLOG)
	pr_debug("nf_conntrack_alloc: ct<%p> BLOGible\n", ct );
	set_bit(IPS_BLOG_BIT, &ct->status);  /* Enable conntrack blogging */

	/* new conntrack: reset blog keys */
	ct->blog_key[IP_CT_DIR_ORIGINAL] = BLOG_KEY_NONE;
	ct->blog_key[IP_CT_DIR_REPLY]    = BLOG_KEY_NONE;
	ct->prev_idle = 0;
	ct->iq_prio = blog_iq(skb);   
    ct->prev_timeout.expires = jiffies;
#endif
	spin_lock_init(&ct->lock);
	ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple = *orig;
	ct->tuplehash[IP_CT_DIR_ORIGINAL].hnnode.pprev = NULL;
	ct->tuplehash[IP_CT_DIR_REPLY].tuple = *repl;
	/* save hash for reusing when confirming */
	*(unsigned long *)(&ct->tuplehash[IP_CT_DIR_REPLY].hnnode.pprev) = hash;
	/* Don't set timer yet: wait for confirmation */
#if defined(CONFIG_BCM_KF_BLOG) && defined(CONFIG_BLOG)
	setup_timer(&ct->timeout, blog_death_by_timeout, (unsigned long)ct);
#else
	setup_timer(&ct->timeout, death_by_timeout, (unsigned long)ct);
#endif

#if defined(CONFIG_BCM_KF_XT_MATCH_LAYER7) && \
	(defined(CONFIG_NETFILTER_XT_MATCH_LAYER7) || defined(CONFIG_NETFILTER_XT_MATCH_LAYER7_MODULE))
	ct->layer7.app_proto = NULL;
	ct->layer7.app_data = NULL;
	ct->layer7.app_data_len = 0;
#endif

#if defined(CONFIG_BCM_KF_RUNNER)
#if defined(CONFIG_BCM_RDPA) || defined(CONFIG_BCM_RDPA_MODULE)
#if defined(CONFIG_BCM_RUNNER_RG) || defined(CONFIG_BCM_RUNNER_RG_MODULE)
        ct->bl_ctx = NULL;
        BL_OPS(net_netfilter_nf_conntrack_core_nf_conntrack_alloc(ct));
#endif /* CONFIG_BCM_RUNNER_RG || CONFIG_BCM_RUNNER_RG_MODULE */
#endif /* CONFIG_BCM_RUNNER */
#endif /* CONFIG_BCM_KF_RUNNER */
	write_pnet(&ct->ct_net, net);
#ifdef CONFIG_NF_CONNTRACK_ZONES
	if (zone) {
		struct nf_conntrack_zone *nf_ct_zone;

		nf_ct_zone = nf_ct_ext_add(ct, NF_CT_EXT_ZONE, GFP_ATOMIC);
		if (!nf_ct_zone)
			goto out_free;
		nf_ct_zone->id = zone;
	}
#endif
	/*
	 * changes to lookup keys must be done before setting refcnt to 1
	 */
	smp_wmb();
	atomic_set(&ct->ct_general.use, 1);
	return ct;

#ifdef CONFIG_NF_CONNTRACK_ZONES
out_free:
	atomic_dec(&net->ct.count);
	kmem_cache_free(net->ct.nf_conntrack_cachep, ct);
	return ERR_PTR(-ENOMEM);
#endif
}

#if defined(CONFIG_BCM_KF_NETFILTER)
struct nf_conn *nf_conntrack_alloc(struct net *net, u16 zone,
				   struct sk_buff *skb,
				   const struct nf_conntrack_tuple *orig,
				   const struct nf_conntrack_tuple *repl,
				   gfp_t gfp)
{
	return __nf_conntrack_alloc(net, zone, skb, orig, repl, gfp, 0);
}
#else
struct nf_conn *nf_conntrack_alloc(struct net *net, u16 zone,
				   const struct nf_conntrack_tuple *orig,
				   const struct nf_conntrack_tuple *repl,
				   gfp_t gfp)
{
	return __nf_conntrack_alloc(net, zone, orig, repl, gfp, 0);
}
#endif
EXPORT_SYMBOL_GPL(nf_conntrack_alloc);

void nf_conntrack_free(struct nf_conn *ct)
{
	struct net *net = nf_ct_net(ct);

#if defined(CONFIG_BCM_KF_RUNNER)
#if defined(CONFIG_BCM_RDPA) || defined(CONFIG_BCM_RDPA_MODULE)
	BL_OPS(net_netfilter_nf_conntrack_core_nf_conntrack_free(ct));
#endif /* CONFIG_BCM_RUNNER */
#endif /* CONFIG_BCM_KF_RUNNER */

#if defined(CONFIG_BCM_KF_NETFILTER)
	/* bugfix for lost connections */
	spin_lock_bh(&nf_conntrack_lock);
	list_del(&ct->safe_list);
	spin_unlock_bh(&nf_conntrack_lock);
#endif

	nf_ct_ext_destroy(ct);
	atomic_dec(&net->ct.count);
	nf_ct_ext_free(ct);
	kmem_cache_free(net->ct.nf_conntrack_cachep, ct);
}
EXPORT_SYMBOL_GPL(nf_conntrack_free);

/* Allocate a new conntrack: we return -ENOMEM if classification
   failed due to stress.  Otherwise it really is unclassifiable. */
static struct nf_conntrack_tuple_hash *
init_conntrack(struct net *net, struct nf_conn *tmpl,
	       const struct nf_conntrack_tuple *tuple,
	       struct nf_conntrack_l3proto *l3proto,
	       struct nf_conntrack_l4proto *l4proto,
	       struct sk_buff *skb,
	       unsigned int dataoff, u32 hash)
{
	struct nf_conn *ct;
	struct nf_conn_help *help;
	struct nf_conntrack_tuple repl_tuple;
	struct nf_conntrack_ecache *ecache;
	struct nf_conntrack_expect *exp;
	u16 zone = tmpl ? nf_ct_zone(tmpl) : NF_CT_DEFAULT_ZONE;
	struct nf_conn_timeout *timeout_ext;
	unsigned int *timeouts;
#ifdef CONFIG_ATP_HYBRID    
    u_int8_t dscp = 0;
#endif
	if (!nf_ct_invert_tuple(&repl_tuple, tuple, l3proto, l4proto)) {
		pr_debug("Can't invert tuple.\n");
		return NULL;
	}

#if defined(CONFIG_BCM_KF_NETFILTER)
	ct = __nf_conntrack_alloc(net, zone, skb, tuple, &repl_tuple, GFP_ATOMIC,
				  hash);
#else
	ct = __nf_conntrack_alloc(net, zone, tuple, &repl_tuple, GFP_ATOMIC,
				  hash);
#endif
	if (IS_ERR(ct))
		return (struct nf_conntrack_tuple_hash *)ct;

	timeout_ext = tmpl ? nf_ct_timeout_find(tmpl) : NULL;
	if (timeout_ext)
		timeouts = NF_CT_TIMEOUT_EXT_DATA(timeout_ext);
	else
		timeouts = l4proto->get_timeouts(net);

	if (!l4proto->new(ct, skb, dataoff, timeouts)) {
		nf_conntrack_free(ct);
		pr_debug("init conntrack: can't track with proto module\n");
		return NULL;
	}

	if (timeout_ext)
		nf_ct_timeout_ext_add(ct, timeout_ext->timeout, GFP_ATOMIC);

	nf_ct_acct_ext_add(ct, GFP_ATOMIC);
	nf_ct_tstamp_ext_add(ct, GFP_ATOMIC);

	ecache = tmpl ? nf_ct_ecache_find(tmpl) : NULL;
	nf_ct_ecache_ext_add(ct, ecache ? ecache->ctmask : 0,
				 ecache ? ecache->expmask : 0,
			     GFP_ATOMIC);

	spin_lock_bh(&nf_conntrack_lock);
#if defined(CONFIG_BCM_KF_NETFILTER)
	/* bugfix for lost connections */
	if (ct->iq_prio == IQOS_PRIO_HIGH)
		list_add_tail(&ct->safe_list, &hi_safe_list);
	else
		list_add_tail(&ct->safe_list, &lo_safe_list);
#endif
	exp = nf_ct_find_expectation(net, zone, tuple);
	if (exp) {
		pr_debug("conntrack: expectation arrives ct=%p exp=%p\n",
			 ct, exp);
		/* Welcome, Mr. Bond.  We've been expecting you... */
		__set_bit(IPS_EXPECTED_BIT, &ct->status);
		ct->master = exp->master;
#if defined(CONFIG_BCM_KF_NETFILTER)
		list_add(&ct->derived_list,
			 &exp->master->derived_connections);
		if (exp->flags & NF_CT_EXPECT_DERIVED_TIMEOUT) {
			ct->derived_timeout = exp->derived_timeout;
		}
#endif
		if (exp->helper) {
			help = nf_ct_helper_ext_add(ct, GFP_ATOMIC);
			if (help)
				rcu_assign_pointer(help->helper, exp->helper);
		}

#ifdef CONFIG_NF_CONNTRACK_MARK
		ct->mark = exp->master->mark;
#endif
#ifdef CONFIG_NF_CONNTRACK_SECMARK
		ct->secmark = exp->master->secmark;
#endif
		nf_conntrack_get(&ct->master->ct_general);
		NF_CT_STAT_INC(net, expect_new);
	} else {
		__nf_ct_try_assign_helper(ct, tmpl, GFP_ATOMIC);
		NF_CT_STAT_INC(net, new);
	}
	
#ifdef CONFIG_ATP_HYBRID
    ct->mark |= (skb->mark & PPP_TRIGER_MARK);/*打上lan mark*/
    if (ETH_P_IP == skb->protocol)
    {
	    dscp = ipv4_get_dsfield(ip_hdr(skb)) >> XT_DSCP_SHIFT;
    }
    else if (ETH_P_IPV6 == skb->protocol)
    {
        dscp = ipv6_get_dsfield(ipv6_hdr(skb)) >> XT_DSCP_SHIFT;
    }
	/* dscp default is 0 */
    
    if (dscp)/*dscp值设置进mark*/
    {
        ct->mark |= (dscp << DSCP_MARK_OFFSET);
    }    
#endif

	/* Overload tuple linked list to put us in unconfirmed list. */
	hlist_nulls_add_head_rcu(&ct->tuplehash[IP_CT_DIR_ORIGINAL].hnnode,
		       &net->ct.unconfirmed);

	spin_unlock_bh(&nf_conntrack_lock);

	if (exp) {
		if (exp->expectfn)
			exp->expectfn(ct, exp);
		nf_ct_expect_put(exp);
	}

	return &ct->tuplehash[IP_CT_DIR_ORIGINAL];
}

/* On success, returns conntrack ptr, sets skb->nfct and ctinfo */
static inline struct nf_conn *
resolve_normal_ct(struct net *net, struct nf_conn *tmpl,
		  struct sk_buff *skb,
		  unsigned int dataoff,
		  u_int16_t l3num,
		  u_int8_t protonum,
		  struct nf_conntrack_l3proto *l3proto,
		  struct nf_conntrack_l4proto *l4proto,
		  int *set_reply,
		  enum ip_conntrack_info *ctinfo)
{
	struct nf_conntrack_tuple tuple;
	struct nf_conntrack_tuple_hash *h;
	struct nf_conn *ct;
	u16 zone = tmpl ? nf_ct_zone(tmpl) : NF_CT_DEFAULT_ZONE;
	u32 hash;


	if (!nf_ct_get_tuple(skb, skb_network_offset(skb),
			     dataoff, l3num, protonum, &tuple, l3proto,
			     l4proto)) {
		pr_debug("resolve_normal_ct: Can't get tuple\n");
		return NULL;
	}

	/* look for tuple match */
	hash = hash_conntrack_raw(&tuple, zone);
	h = __nf_conntrack_find_get(net, zone, &tuple, hash);
	if (!h) {
		h = init_conntrack(net, tmpl, &tuple, l3proto, l4proto,
				   skb, dataoff, hash);
		if (!h)
			return NULL;
		if (IS_ERR(h))
			return (void *)h;
	}
	ct = nf_ct_tuplehash_to_ctrack(h);

	/* It exists; we have (non-exclusive) reference. */
	if (NF_CT_DIRECTION(h) == IP_CT_DIR_REPLY) {
		*ctinfo = IP_CT_ESTABLISHED_REPLY;
		/* Please set reply bit if this packet OK */
		*set_reply = 1;
	} else {
		/* Once we've had two way comms, always ESTABLISHED. */
		if (test_bit(IPS_SEEN_REPLY_BIT, &ct->status)) {
			pr_debug("nf_conntrack_in: normal packet for %p\n", ct);
			*ctinfo = IP_CT_ESTABLISHED;
		} else if (test_bit(IPS_EXPECTED_BIT, &ct->status)) {
			pr_debug("nf_conntrack_in: related packet for %p\n",
				 ct);
			*ctinfo = IP_CT_RELATED;
		} else {
			pr_debug("nf_conntrack_in: new packet for %p\n", ct);
			*ctinfo = IP_CT_NEW;
		}
		*set_reply = 0;
	}
	skb->nfct = &ct->ct_general;
	skb->nfctinfo = *ctinfo;
#if defined(CONFIG_BCM_KF_BLOG) && defined(CONFIG_BLOG)
	{
		struct nf_conn_help * help = nfct_help(ct);

		if ( (help != (struct nf_conn_help *)NULL) &&
			 (help->helper != (struct nf_conntrack_helper *)NULL) &&
			 (help->helper->name && strcmp(help->helper->name, "BCM-NAT")) )
		{
			pr_debug("nf_conntrack_in: skb<%p> ct<%p> helper<%s> found\n",
						skb, ct, help->helper->name);
			clear_bit(IPS_BLOG_BIT, &ct->status);
		}
		if (test_bit(IPS_BLOG_BIT, &ct->status))    /* OK to blog ? */
		{
			uint32_t ct_type=(l3num==PF_INET)?BLOG_PARAM2_IPV4:BLOG_PARAM2_IPV6;
			pr_debug("nf_conntrack_in: skb<%p> blog<%p> ct<%p>\n",
						skb, blog_ptr(skb), ct );

            if (protonum == IPPROTO_GRE) 
                ct_type = BLOG_PARAM2_GRE_IPV4; 

			blog_link(FLOWTRACK, blog_ptr(skb),
					  (void*)ct, CTINFO2DIR(skb->nfctinfo), ct_type);
		}
		else
		{
			pr_debug("nf_conntrack_in: skb<%p> ct<%p> NOT BLOGible<%p>\n",
						skb, ct, blog_ptr(skb));
			blog_skip(skb);                         /* No blogging */
		}
	}
#endif

	return ct;
}

unsigned int
nf_conntrack_in(struct net *net, u_int8_t pf, unsigned int hooknum,
		struct sk_buff *skb)
{
	struct nf_conn *ct, *tmpl = NULL;
	enum ip_conntrack_info ctinfo;
	struct nf_conntrack_l3proto *l3proto;
	struct nf_conntrack_l4proto *l4proto;
	struct nf_conn_timeout *timeout_ext;
	unsigned int *timeouts;
	unsigned int dataoff;
	u_int8_t protonum;
	int set_reply = 0;
	int ret;

	if (skb->nfct) {
	/* Previously seen (loopback or untracked)?  Ignore. */
		tmpl = (struct nf_conn *)skb->nfct;
		if (!nf_ct_is_template(tmpl)) {
		NF_CT_STAT_INC_ATOMIC(net, ignore);
		return NF_ACCEPT;
	}
		skb->nfct = NULL;
	}

	/* rcu_read_lock()ed by nf_hook_slow */
	l3proto = __nf_ct_l3proto_find(pf);
	ret = l3proto->get_l4proto(skb, skb_network_offset(skb),
				   &dataoff, &protonum);
	if (ret <= 0) {
		pr_debug("not prepared to track yet or error occurred\n");
		NF_CT_STAT_INC_ATOMIC(net, error);
		NF_CT_STAT_INC_ATOMIC(net, invalid);
		ret = -ret;
		goto out;
	}

	l4proto = __nf_ct_l4proto_find(pf, protonum);

	/* It may be an special packet, error, unclean...
	 * inverse of the return code tells to the netfilter
	 * core what to do with the packet. */
	if (l4proto->error != NULL) {
		ret = l4proto->error(net, tmpl, skb, dataoff, &ctinfo,
				     pf, hooknum);
		if (ret <= 0) {
			NF_CT_STAT_INC_ATOMIC(net, error);
			NF_CT_STAT_INC_ATOMIC(net, invalid);
			ret = -ret;
			goto out;
		}
		/* ICMP[v6] protocol trackers may assign one conntrack. */
		if (skb->nfct)
			goto out;
	}

	ct = resolve_normal_ct(net, tmpl, skb, dataoff, pf, protonum,
			       l3proto, l4proto, &set_reply, &ctinfo);
	if (!ct) {
		/* Not valid part of a connection */
		NF_CT_STAT_INC_ATOMIC(net, invalid);
		ret = NF_ACCEPT;
		goto out;
	}

	if (IS_ERR(ct)) {
		/* Too stressed to deal. */
		NF_CT_STAT_INC_ATOMIC(net, drop);
		ret = NF_DROP;
		goto out;
	}

	NF_CT_ASSERT(skb->nfct);

	/* Decide what timeout policy we want to apply to this flow. */
	timeout_ext = nf_ct_timeout_find(ct);
	if (timeout_ext)
		timeouts = NF_CT_TIMEOUT_EXT_DATA(timeout_ext);
	else
		timeouts = l4proto->get_timeouts(net);

	ret = l4proto->packet(ct, skb, dataoff, ctinfo, pf, hooknum, timeouts);
	if (ret <= 0) {
		/* Invalid: inverse of the return code tells
		 * the netfilter core what to do */
		pr_debug("nf_conntrack_in: Can't track with proto module\n");
		nf_conntrack_put(skb->nfct);
		skb->nfct = NULL;
		NF_CT_STAT_INC_ATOMIC(net, invalid);
		if (ret == -NF_DROP)
			NF_CT_STAT_INC_ATOMIC(net, drop);
		ret = -ret;
		goto out;
	}

#if defined(CONFIG_BCM_KF_RUNNER)
#if defined(CONFIG_BCM_RDPA) || defined(CONFIG_BCM_RDPA_MODULE)
	BL_OPS(net_netfilter_nf_conntrack_core_nf_conntrack_in(ct, skb));
#endif /* CONFIG_BCM_RUNNER */
#endif /* CONFIG_BCM_KF_RUNNER */

	if (set_reply && !test_and_set_bit(IPS_SEEN_REPLY_BIT, &ct->status))
		nf_conntrack_event_cache(IPCT_REPLY, ct);

#if defined(CONFIG_BCM_KF_NETFILTER)
	/* Maintain LRU list. The least recently used ctt is on the head */
	if (ctinfo == IP_CT_ESTABLISHED ||
	    ctinfo == IP_CT_ESTABLISHED + IP_CT_IS_REPLY) {
		spin_lock_bh(&nf_conntrack_lock);
		/* Update ct as latest used */
		if (ct->iq_prio == IQOS_PRIO_HIGH)
			list_move_tail(&ct->safe_list, &hi_safe_list);
		else
			list_move_tail(&ct->safe_list, &lo_safe_list);

		spin_unlock_bh(&nf_conntrack_lock);
	}
#endif

out:
	if (tmpl) {
		/* Special case: we have to repeat this hook, assign the
		 * template again to this packet. We assume that this packet
		 * has no conntrack assigned. This is used by nf_ct_tcp. */
		if (ret == NF_REPEAT)
			skb->nfct = (struct nf_conntrack *)tmpl;
		else
			nf_ct_put(tmpl);
	}

	return ret;
}
EXPORT_SYMBOL_GPL(nf_conntrack_in);

bool nf_ct_invert_tuplepr(struct nf_conntrack_tuple *inverse,
			  const struct nf_conntrack_tuple *orig)
{
	bool ret;

	rcu_read_lock();
	ret = nf_ct_invert_tuple(inverse, orig,
				 __nf_ct_l3proto_find(orig->src.l3num),
				 __nf_ct_l4proto_find(orig->src.l3num,
						      orig->dst.protonum));
	rcu_read_unlock();
	return ret;
}
EXPORT_SYMBOL_GPL(nf_ct_invert_tuplepr);

/* Alter reply tuple (maybe alter helper).  This is for NAT, and is
   implicitly racy: see __nf_conntrack_confirm */
void nf_conntrack_alter_reply(struct nf_conn *ct,
			      const struct nf_conntrack_tuple *newreply)
{
	struct nf_conn_help *help = nfct_help(ct);

	/* Should be unconfirmed, so not in hash table yet */
	NF_CT_ASSERT(!nf_ct_is_confirmed(ct));

	pr_debug("Altering reply tuple of %p to ", ct);
	nf_ct_dump_tuple(newreply);

	ct->tuplehash[IP_CT_DIR_REPLY].tuple = *newreply;
	if (ct->master || (help && !hlist_empty(&help->expectations)))
		return;

	rcu_read_lock();
	__nf_ct_try_assign_helper(ct, NULL, GFP_ATOMIC);
	rcu_read_unlock();
}
EXPORT_SYMBOL_GPL(nf_conntrack_alter_reply);

/* Refresh conntrack for this many jiffies and do accounting if do_acct is 1 */
void __nf_ct_refresh_acct(struct nf_conn *ct,
			  enum ip_conntrack_info ctinfo,
			  const struct sk_buff *skb,
			  unsigned long extra_jiffies,
			  int do_acct)
{
	NF_CT_ASSERT(ct->timeout.data == (unsigned long)ct);
	NF_CT_ASSERT(skb);

	/* Only update if this is not a fixed timeout */
	if (test_bit(IPS_FIXED_TIMEOUT_BIT, &ct->status))
		goto acct;

	/* If not in hash table, timer will not be active yet */
	if (!nf_ct_is_confirmed(ct)) {
		ct->timeout.expires = extra_jiffies;
	} else {
		unsigned long newtime = jiffies + extra_jiffies;

		/* Only update the timeout if the new timeout is at least
		   HZ jiffies from the old timeout. Need del_timer for race
		   avoidance (may already be dying). */
		if (newtime - ct->timeout.expires >= HZ)
			mod_timer_pending(&ct->timeout, newtime);
	}

#if defined(CONFIG_BCM_KF_BLOG) && defined(CONFIG_BCM_KF_NETFILTER) && defined(CONFIG_BLOG)
	/* Check if the flow is blogged i.e. currently being accelerated */
	if (ct->blog_key[BLOG_PARAM1_DIR_ORIG] != BLOG_KEY_NONE || 
		ct->blog_key[BLOG_PARAM1_DIR_REPLY] != BLOG_KEY_NONE) {
	    /* Maintain LRU list. The least recently used ct is on the head */
	    /*
		 * safe_list through blog refresh is updated at an interval refresh is called 
		 * If that interval is large - it is possible that a connection getting high traffic 
		 * may be seen as LRU by conntrack. 
		 */
		/* start DTS2014032511045 conntrack lead to rcu pending by f00110348 */ 
#ifdef CONFIG_ATP_BRCM		 
        spin_lock_bh(&nf_conntrack_lock);
#endif
		if (ct->iq_prio == IQOS_PRIO_HIGH)
			list_move_tail(&ct->safe_list, &hi_safe_list);
		else
			list_move_tail(&ct->safe_list, &lo_safe_list);
#ifdef CONFIG_ATP_BRCM        
        spin_unlock_bh(&nf_conntrack_lock);
#endif
        /* end DTS2014032511045 conntrack lead to rcu pending by f00110348 */
	}
#endif
acct:
	if (do_acct) {
		struct nf_conn_counter *acct;

		acct = nf_conn_acct_find(ct);
		if (acct) {
			atomic64_inc(&acct[CTINFO2DIR(ctinfo)].packets);
			atomic64_add(skb->len, &acct[CTINFO2DIR(ctinfo)].bytes);
		}
	}
}
EXPORT_SYMBOL_GPL(__nf_ct_refresh_acct);

bool __nf_ct_kill_acct(struct nf_conn *ct,
		       enum ip_conntrack_info ctinfo,
		       const struct sk_buff *skb,
		       int do_acct)
{
	if (do_acct) {
		struct nf_conn_counter *acct;

		acct = nf_conn_acct_find(ct);
		if (acct) {
			atomic64_inc(&acct[CTINFO2DIR(ctinfo)].packets);
			atomic64_add(skb->len - skb_network_offset(skb),
				     &acct[CTINFO2DIR(ctinfo)].bytes);
		}
	}

	if (del_timer(&ct->timeout)) {
		ct->timeout.function((unsigned long)ct);
		return true;
	}
	return false;
}
EXPORT_SYMBOL_GPL(__nf_ct_kill_acct);

#ifdef CONFIG_NF_CONNTRACK_ZONES
static struct nf_ct_ext_type nf_ct_zone_extend __read_mostly = {
	.len	= sizeof(struct nf_conntrack_zone),
	.align	= __alignof__(struct nf_conntrack_zone),
	.id	= NF_CT_EXT_ZONE,
};
#endif

#if IS_ENABLED(CONFIG_NF_CT_NETLINK)

#include <linux/netfilter/nfnetlink.h>
#include <linux/netfilter/nfnetlink_conntrack.h>
#include <linux/mutex.h>

/* Generic function for tcp/udp/sctp/dccp and alike. This needs to be
 * in ip_conntrack_core, since we don't want the protocols to autoload
 * or depend on ctnetlink */
int nf_ct_port_tuple_to_nlattr(struct sk_buff *skb,
			       const struct nf_conntrack_tuple *tuple)
{
	NLA_PUT_BE16(skb, CTA_PROTO_SRC_PORT, tuple->src.u.tcp.port);
	NLA_PUT_BE16(skb, CTA_PROTO_DST_PORT, tuple->dst.u.tcp.port);
	return 0;

nla_put_failure:
	return -1;
}
EXPORT_SYMBOL_GPL(nf_ct_port_tuple_to_nlattr);

const struct nla_policy nf_ct_port_nla_policy[CTA_PROTO_MAX+1] = {
	[CTA_PROTO_SRC_PORT]  = { .type = NLA_U16 },
	[CTA_PROTO_DST_PORT]  = { .type = NLA_U16 },
};
EXPORT_SYMBOL_GPL(nf_ct_port_nla_policy);

int nf_ct_port_nlattr_to_tuple(struct nlattr *tb[],
			       struct nf_conntrack_tuple *t)
{
	if (!tb[CTA_PROTO_SRC_PORT] || !tb[CTA_PROTO_DST_PORT])
		return -EINVAL;

	t->src.u.tcp.port = nla_get_be16(tb[CTA_PROTO_SRC_PORT]);
	t->dst.u.tcp.port = nla_get_be16(tb[CTA_PROTO_DST_PORT]);

	return 0;
}
EXPORT_SYMBOL_GPL(nf_ct_port_nlattr_to_tuple);

int nf_ct_port_nlattr_tuple_size(void)
{
	return nla_policy_len(nf_ct_port_nla_policy, CTA_PROTO_MAX + 1);
}
EXPORT_SYMBOL_GPL(nf_ct_port_nlattr_tuple_size);
#endif

/* Used by ipt_REJECT and ip6t_REJECT. */
static void nf_conntrack_attach(struct sk_buff *nskb, struct sk_buff *skb)
{
	struct nf_conn *ct;
	enum ip_conntrack_info ctinfo;

	/* This ICMP is in reverse direction to the packet which caused it */
	ct = nf_ct_get(skb, &ctinfo);
	if (CTINFO2DIR(ctinfo) == IP_CT_DIR_ORIGINAL)
		ctinfo = IP_CT_RELATED_REPLY;
	else
		ctinfo = IP_CT_RELATED;

	/* Attach to new skbuff, and increment count */
	nskb->nfct = &ct->ct_general;
	nskb->nfctinfo = ctinfo;
	nf_conntrack_get(nskb->nfct);
}

/* Bring out ya dead! */
static struct nf_conn *
get_next_corpse(struct net *net, int (*iter)(struct nf_conn *i, void *data),
		void *data, unsigned int *bucket)
{
	struct nf_conntrack_tuple_hash *h;
	struct nf_conn *ct;
	struct hlist_nulls_node *n;

	spin_lock_bh(&nf_conntrack_lock);
	for (; *bucket < net->ct.htable_size; (*bucket)++) {
		hlist_nulls_for_each_entry(h, n, &net->ct.hash[*bucket], hnnode) {
			ct = nf_ct_tuplehash_to_ctrack(h);
			if (iter(ct, data))
				goto found;
		}
	}
	hlist_nulls_for_each_entry(h, n, &net->ct.unconfirmed, hnnode) {
		ct = nf_ct_tuplehash_to_ctrack(h);
		if (iter(ct, data))
			set_bit(IPS_DYING_BIT, &ct->status);
	}
	spin_unlock_bh(&nf_conntrack_lock);
	return NULL;
found:
	atomic_inc(&ct->ct_general.use);
	spin_unlock_bh(&nf_conntrack_lock);
	return ct;
}

void nf_ct_iterate_cleanup(struct net *net,
			   int (*iter)(struct nf_conn *i, void *data),
			   void *data)
{
	struct nf_conn *ct;
	unsigned int bucket = 0;

	while ((ct = get_next_corpse(net, iter, data, &bucket)) != NULL) {
		/* Time to push up daises... */
		if (del_timer(&ct->timeout))
			death_by_timeout((unsigned long)ct);
		/* ... else the timer will get him soon. */

		nf_ct_put(ct);
	}
}
EXPORT_SYMBOL_GPL(nf_ct_iterate_cleanup);

struct __nf_ct_flush_report {
	u32 pid;
	int report;
};

static int kill_report(struct nf_conn *i, void *data)
{
	struct __nf_ct_flush_report *fr = (struct __nf_ct_flush_report *)data;
	struct nf_conn_tstamp *tstamp;

	tstamp = nf_conn_tstamp_find(i);
	if (tstamp && tstamp->stop == 0)
		tstamp->stop = ktime_to_ns(ktime_get_real());

	/* If we fail to deliver the event, death_by_timeout() will retry */
	if (nf_conntrack_event_report(IPCT_DESTROY, i,
				      fr->pid, fr->report) < 0)
		return 1;

	/* Avoid the delivery of the destroy event in death_by_timeout(). */
	set_bit(IPS_DYING_BIT, &i->status);
	return 1;
}

#ifdef CONFIG_ATP_CONNTRACK_CLEAN
static int kill_all(struct nf_conn *i, void *data)
{
	struct __nf_ct_flush_report *fr = (struct __nf_ct_flush_report *)data;

	/* get_next_corpse sets the dying bit for us */
	nf_conntrack_event_report(IPCT_DESTROY,
				  i,
				  fr->pid,
				  fr->report);

/*start added by t00191685 for HG658B DTS2012010505145 2012-1-31*/				  
#if defined(CONFIG_BCM963268) && defined(CONFIG_NF_CONNTRACK_MARK)

/* Start of added by f00120964 for conntrack clean with gateway service 2012-6-18 */
#ifdef CONFIG_DT_QOS
	if((i->mark & NFMARK_TR069_CONTRACK) == NFMARK_TR069_CONTRACK)
#else
	 if(((i->mark & NFMARK_TR069_CONTRACK) == NFMARK_TR069_CONTRACK)
		||((i->mark & NFMARK_RTCP) == NFMARK_RTCP)
		||((i->mark & NFMARK_RTP) == NFMARK_RTP)
		||((i->mark & NFMARK_SIP) == NFMARK_SIP)
		||((i->mark & CONNTRACK_AVOID_SERVICE) == CONNTRACK_AVOID_SERVICE))
#endif /* CONFIG_DT_QOS */
	 {
		return 0;
	 }

/* End of added by f00120964 for conntrack clean with gateway service 2012-6-18 */
#endif
/*end added by t00191685 for HG658B DTS2012010505145 2012-1-31*/	

/* TR069的连接不清除, NFMARK_TR069_CONTRACK需要与用户态定义的值保持一致 */
#if defined(CONFIG_BCM96362) && defined(CONFIG_NF_CONNTRACK_MARK)
/*START modify: LIQI 20111027 HG656c use MARK 0x3010 for high priority*/
#if CONFIG_BCM_EXT_SWITCH
	return ((i->mark & 0xffff) != 0x3010);
#else
	return ((i->mark & 0xffff) != 0x3003);
#endif
/*END modify: LIQI 20111027 HG656c use MARK 0x3010 for high priority */
#else
	return 1;
#endif
}
#else
static int kill_all(struct nf_conn *i, void *data)
{
	return 1;
}

#endif

void nf_ct_free_hashtable(void *hash, unsigned int size)
{
	if (is_vmalloc_addr(hash))
		vfree(hash);
	else
		free_pages((unsigned long)hash,
			   get_order(sizeof(struct hlist_head) * size));
}
EXPORT_SYMBOL_GPL(nf_ct_free_hashtable);

void nf_conntrack_flush_report(struct net *net, u32 pid, int report)
{
	struct __nf_ct_flush_report fr = {
		.pid 	= pid,
		.report = report,
	};
	nf_ct_iterate_cleanup(net, kill_report, &fr);
}
EXPORT_SYMBOL_GPL(nf_conntrack_flush_report);

static void nf_ct_release_dying_list(struct net *net)
{
	struct nf_conntrack_tuple_hash *h;
	struct nf_conn *ct;
	struct hlist_nulls_node *n;

	spin_lock_bh(&nf_conntrack_lock);
	hlist_nulls_for_each_entry(h, n, &net->ct.dying, hnnode) {
		ct = nf_ct_tuplehash_to_ctrack(h);
		/* never fails to remove them, no listeners at this point */
		nf_ct_kill(ct);
	}
	spin_unlock_bh(&nf_conntrack_lock);
}

static int untrack_refs(void)
{
	int cnt = 0, cpu;

	for_each_possible_cpu(cpu) {
		struct nf_conn *ct = &per_cpu(nf_conntrack_untracked, cpu);

		cnt += atomic_read(&ct->ct_general.use) - 1;
	}
	return cnt;
}

static void nf_conntrack_cleanup_init_net(void)
{
	while (untrack_refs() > 0)
		schedule();

	nf_conntrack_helper_fini();
	nf_conntrack_proto_fini();
#ifdef CONFIG_NF_CONNTRACK_ZONES
	nf_ct_extend_unregister(&nf_ct_zone_extend);
#endif
}

static void nf_conntrack_cleanup_net(struct net *net)
{
#if defined(CONFIG_BCM_KF_NETFILTER)
	int try_counter = 0;
	unsigned long start = jiffies;
	unsigned long end = start + HZ;
#endif
 i_see_dead_people:
	nf_ct_iterate_cleanup(net, kill_all, NULL);
	nf_ct_release_dying_list(net);
	if (atomic_read(&net->ct.count) != 0) {
#if defined(CONFIG_BCM_KF_NETFILTER)
		if (jiffies >= end) {
			printk("waiting for %d conntrack to be cleaned, "
			       "tried %d times\n",
			       atomic_read(&net->ct.count), try_counter);
			end += HZ;
		}
		try_counter++;
#endif
	schedule();
		goto i_see_dead_people;
	}

	nf_ct_free_hashtable(net->ct.hash, net->ct.htable_size);
	nf_conntrack_timeout_fini(net);
	nf_conntrack_ecache_fini(net);
	nf_conntrack_tstamp_fini(net);
	nf_conntrack_acct_fini(net);
	nf_conntrack_expect_fini(net);
	kmem_cache_destroy(net->ct.nf_conntrack_cachep);
	kfree(net->ct.slabname);
	free_percpu(net->ct.stat);
#if defined(CONFIG_BCM_KF_NETFILTER)
	end = jiffies;
	if (end - start > HZ) {
		printk("nf_conntrack took %lu milliseconds to clean up\n",
		       (end - start) * 1000 / HZ);
	}
#endif
}

/* Mishearing the voices in his head, our hero wonders how he's
   supposed to kill the mall. */
void nf_conntrack_cleanup(struct net *net)
{
	if (net_eq(net, &init_net))
		RCU_INIT_POINTER(ip_ct_attach, NULL);

	/* This makes sure all current packets have passed through
	   netfilter framework.  Roll on, two-stage module
	   delete... */
	synchronize_net();

	nf_conntrack_cleanup_net(net);

	if (net_eq(net, &init_net)) {
		RCU_INIT_POINTER(nf_ct_destroy, NULL);
		nf_conntrack_cleanup_init_net();
	}
}

#ifdef CONFIG_ATP_CONNTRACK_CLEAN
static int kill_dns(struct nf_conn *i, void *data)
{
	return (i->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u.udp.port == htons(53));
}
static int kill_sip(struct nf_conn *i, void *data)
{
	unsigned short sipport = i->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u.udp.port;
	return ((sipport == htons(5060)) || ((ntohs(sipport) >= 7070) && (ntohs(sipport) <= 7079)));
}
//kill h323 connection log when you registe and unregiste h323 module
static int kill_h323(struct nf_conn *i, void *data)
{    
    unsigned short h323port = i->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u.udp.port;    
    return (h323port == htons(1719));
}

//kill fon connection log when you login fon
/*Begin:Modied by luokunling l00192527 ,fon用户登录成功之后.只清楚当前用户连接跟踪,不清楚所有fon用户*/
static int kill_fon(struct nf_conn *i, void *data)
{   
    unsigned int fonip = 0x00AC1102;
    /* 源地址必须为172.17.2.2- 172.17.2.17中的一个*/
    if( nf_conntrack_fonip != (i->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u3.ip) )
        return false;

    /* 目的地址不能为172.17.2.x */
    if( fonip == (i->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u3.ip >>8) )
        return false;

    return true;
    
}
/*Begin:added by luokunling l00192527 */
static int kill_dport(struct nf_conn *i, void *data)
{
	return (ntohs(i->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u.udp.port) == (unsigned short)*(unsigned int*)data);
}

/*start of 问题单:AU4D00925，连接跟踪信息清空。by 00126165 2009-10-25*/
static void wan_up_idle_timer(unsigned long ptr)
{
    nf_conntrack_clean = 9;
    /*Start of ATP 2008-11-22 for AU4D01074 by c47036: TIMER超时后会自动删除 */
    //del_timer(&wan_up_timer);
    /*End of ATP 2008-11-22 for AU4D01074 by c47036: TIMER超时后会自动删除 */
}
/*end of 问题单:AU4D00925，连接跟踪信息清空。by 00126165 2009-10-25*/

static void nf_conntrack_user_cleanup(struct net *net)
{
    switch (nf_conntrack_clean)
    {
        case 0: 
            // do nothing
            break;
        case 1: 
            // clean dns conntrack
            nf_ct_iterate_cleanup(net, kill_dns, NULL);
            break;
        case 2: 
            // clean sip conntrack
            nf_ct_iterate_cleanup(net, kill_sip, NULL);
            break;
        case 3:
            // clean h323 conntrack
            nf_ct_iterate_cleanup(net, kill_h323, NULL); 
            break;		
        case 4:
            // clean fon conntrack
            nf_ct_iterate_cleanup(net, kill_fon, NULL); 
            break;		      
        case 9: 
            // clean all for debug
            nf_ct_iterate_cleanup(net, kill_all, NULL);
            /*确保全部清空*/
            nf_ct_iterate_cleanup(net, kill_all, NULL);
            nf_ct_iterate_cleanup(net, kill_all, NULL);
            break;
        /*start of 问题单:AU4D00925，连接跟踪信息清空。by 00126165 2009-10-25*/
        case 18:
            // clean all for debug
            nf_ct_iterate_cleanup(net, kill_all, NULL);
            /* 确保全部清除 */
            nf_ct_iterate_cleanup(net, kill_all, NULL);
            nf_ct_iterate_cleanup(net, kill_all, NULL);
            /*Start of ATP 2008-11-22 for AU4D01074 by c47036: 重复添加TIMER之前需删除 */
            del_timer(&wan_up_timer);
            /*End of ATP 2008-11-22 for AU4D01074 by c47036: 重复添加TIMER之前需删除 */
            init_timer(&wan_up_timer);
	        wan_up_timer.expires = jiffies + WAN_UP_TIMER_OUT * HZ;
	        wan_up_timer.function = wan_up_idle_timer;
	        add_timer(&wan_up_timer);
            break;
        /*end of 问题单:AU4D00925，连接跟踪信息清空。by 00126165 2009-10-25*/
        default: 
            // clean by dest port
            nf_ct_iterate_cleanup(net, kill_dport, &nf_conntrack_clean);
            break;            
    }
    nf_conntrack_clean = 0;
}
#endif

void *nf_ct_alloc_hashtable(unsigned int *sizep, int nulls)
{
	struct hlist_nulls_head *hash;
	unsigned int nr_slots, i;
	size_t sz;

	BUILD_BUG_ON(sizeof(struct hlist_nulls_head) != sizeof(struct hlist_head));
	nr_slots = *sizep = roundup(*sizep, PAGE_SIZE / sizeof(struct hlist_nulls_head));
	sz = nr_slots * sizeof(struct hlist_nulls_head);
	hash = (void *)__get_free_pages(GFP_KERNEL | __GFP_NOWARN | __GFP_ZERO,
					get_order(sz));
	if (!hash) {
		printk(KERN_WARNING "nf_conntrack: falling back to vmalloc.\n");
		hash = vzalloc(sz);
	}

	if (hash && nulls)
		for (i = 0; i < nr_slots; i++)
			INIT_HLIST_NULLS_HEAD(&hash[i], i);

	return hash;
}
EXPORT_SYMBOL_GPL(nf_ct_alloc_hashtable);

int nf_conntrack_set_hashsize(const char *val, struct kernel_param *kp)
{
	int i, bucket;
	unsigned int hashsize, old_size;
	struct hlist_nulls_head *hash, *old_hash;
	struct nf_conntrack_tuple_hash *h;
	struct nf_conn *ct;

	if (current->nsproxy->net_ns != &init_net)
		return -EOPNOTSUPP;

	/* On boot, we can set this without any fancy locking. */
	if (!nf_conntrack_htable_size)
		return param_set_uint(val, kp);

	hashsize = simple_strtoul(val, NULL, 0);
	if (!hashsize)
		return -EINVAL;

	hash = nf_ct_alloc_hashtable(&hashsize, 1);
	if (!hash)
		return -ENOMEM;

	/* Lookups in the old hash might happen in parallel, which means we
	 * might get false negatives during connection lookup. New connections
	 * created because of a false negative won't make it into the hash
	 * though since that required taking the lock.
	 */
	spin_lock_bh(&nf_conntrack_lock);
	for (i = 0; i < init_net.ct.htable_size; i++) {
		while (!hlist_nulls_empty(&init_net.ct.hash[i])) {
			h = hlist_nulls_entry(init_net.ct.hash[i].first,
					struct nf_conntrack_tuple_hash, hnnode);
			ct = nf_ct_tuplehash_to_ctrack(h);
			hlist_nulls_del_rcu(&h->hnnode);
			bucket = __hash_conntrack(&h->tuple, nf_ct_zone(ct),
						  hashsize);
			hlist_nulls_add_head_rcu(&h->hnnode, &hash[bucket]);
		}
	}
	old_size = init_net.ct.htable_size;
	old_hash = init_net.ct.hash;

	init_net.ct.htable_size = nf_conntrack_htable_size = hashsize;
	init_net.ct.hash = hash;
	spin_unlock_bh(&nf_conntrack_lock);

	nf_ct_free_hashtable(old_hash, old_size);
	return 0;
}
EXPORT_SYMBOL_GPL(nf_conntrack_set_hashsize);

module_param_call(hashsize, nf_conntrack_set_hashsize, param_get_uint,
		  &nf_conntrack_htable_size, 0600);

void nf_ct_untracked_status_or(unsigned long bits)
{
	int cpu;

	for_each_possible_cpu(cpu)
		per_cpu(nf_conntrack_untracked, cpu).status |= bits;
}
EXPORT_SYMBOL_GPL(nf_ct_untracked_status_or);

static int nf_conntrack_init_init_net(void)
{
	int max_factor = 8;
	int ret, cpu;

	/* Idea from tcp.c: use 1/16384 of memory.  On i386: 32MB
	 * machine has 512 buckets. >= 1GB machines have 16384 buckets. */
	if (!nf_conntrack_htable_size) {
		nf_conntrack_htable_size
			= (((totalram_pages << PAGE_SHIFT) / 16384)
			   / sizeof(struct hlist_head));
		if (totalram_pages > (1024 * 1024 * 1024 / PAGE_SIZE))
			nf_conntrack_htable_size = 16384;
		if (nf_conntrack_htable_size < 32)
			nf_conntrack_htable_size = 32;

		/* Use a max. factor of four by default to get the same max as
		 * with the old struct list_heads. When a table size is given
		 * we use the old value of 8 to avoid reducing the max.
		 * entries. */
		max_factor = 4;
	}
	nf_conntrack_max = max_factor * nf_conntrack_htable_size;

	printk(KERN_INFO "nf_conntrack version %s (%u buckets, %d max)\n",
	       NF_CONNTRACK_VERSION, nf_conntrack_htable_size,
	       nf_conntrack_max);

	ret = nf_conntrack_proto_init();
	if (ret < 0)
		goto err_proto;

	ret = nf_conntrack_helper_init();
	if (ret < 0)
		goto err_helper;

#ifdef CONFIG_NF_CONNTRACK_ZONES
	ret = nf_ct_extend_register(&nf_ct_zone_extend);
	if (ret < 0)
		goto err_extend;
#endif
	/* Set up fake conntrack: to never be deleted, not in any hashes */
	for_each_possible_cpu(cpu) {
		struct nf_conn *ct = &per_cpu(nf_conntrack_untracked, cpu);
		write_pnet(&ct->ct_net, &init_net);
		atomic_set(&ct->ct_general.use, 1);
	}
	/*  - and look it like as a confirmed connection */
	nf_ct_untracked_status_or(IPS_CONFIRMED | IPS_UNTRACKED);
	return 0;

#ifdef CONFIG_NF_CONNTRACK_ZONES
err_extend:
	nf_conntrack_helper_fini();
#endif
err_helper:
	nf_conntrack_proto_fini();
err_proto:
	return ret;
}

/*
 * We need to use special "null" values, not used in hash table
 */
#define UNCONFIRMED_NULLS_VAL	((1<<30)+0)
#define DYING_NULLS_VAL		((1<<30)+1)

static int nf_conntrack_init_net(struct net *net)
{
	int ret;

	atomic_set(&net->ct.count, 0);
	INIT_HLIST_NULLS_HEAD(&net->ct.unconfirmed, UNCONFIRMED_NULLS_VAL);
	INIT_HLIST_NULLS_HEAD(&net->ct.dying, DYING_NULLS_VAL);
	net->ct.stat = alloc_percpu(struct ip_conntrack_stat);
	if (!net->ct.stat) {
		ret = -ENOMEM;
		goto err_stat;
	}

	net->ct.slabname = kasprintf(GFP_KERNEL, "nf_conntrack_%p", net);
	if (!net->ct.slabname) {
		ret = -ENOMEM;
		goto err_slabname;
	}

	net->ct.nf_conntrack_cachep = kmem_cache_create(net->ct.slabname,
							sizeof(struct nf_conn), 0,
							SLAB_DESTROY_BY_RCU, NULL);
	if (!net->ct.nf_conntrack_cachep) {
		printk(KERN_ERR "Unable to create nf_conn slab cache\n");
		ret = -ENOMEM;
		goto err_cache;
	}

	net->ct.htable_size = nf_conntrack_htable_size;
	net->ct.hash = nf_ct_alloc_hashtable(&net->ct.htable_size, 1);
	if (!net->ct.hash) {
		ret = -ENOMEM;
		printk(KERN_ERR "Unable to create nf_conntrack_hash\n");
		goto err_hash;
	}
	ret = nf_conntrack_expect_init(net);
	if (ret < 0)
		goto err_expect;
	ret = nf_conntrack_acct_init(net);
	if (ret < 0)
		goto err_acct;
	ret = nf_conntrack_tstamp_init(net);
	if (ret < 0)
		goto err_tstamp;
	ret = nf_conntrack_ecache_init(net);
	if (ret < 0)
		goto err_ecache;
	ret = nf_conntrack_timeout_init(net);
	if (ret < 0)
		goto err_timeout;

#if defined(CONFIG_BCM_KF_BLOG) && defined(CONFIG_BLOG)
	blog_cttime_update_fn = (blog_cttime_upd_t) __nf_ct_time_update;
#endif

	return 0;

err_timeout:
	nf_conntrack_ecache_fini(net);
err_ecache:
	nf_conntrack_tstamp_fini(net);
err_tstamp:
	nf_conntrack_acct_fini(net);
err_acct:
	nf_conntrack_expect_fini(net);
err_expect:
	nf_ct_free_hashtable(net->ct.hash, net->ct.htable_size);
err_hash:
	kmem_cache_destroy(net->ct.nf_conntrack_cachep);
err_cache:
	kfree(net->ct.slabname);
err_slabname:
	free_percpu(net->ct.stat);
err_stat:
	return ret;
}

s16 (*nf_ct_nat_offset)(const struct nf_conn *ct,
			enum ip_conntrack_dir dir,
			u32 seq);
EXPORT_SYMBOL_GPL(nf_ct_nat_offset);

int nf_conntrack_init(struct net *net)
{
	int ret;

	if (net_eq(net, &init_net)) {
		ret = nf_conntrack_init_init_net();
		if (ret < 0)
			goto out_init_net;
	}
	ret = nf_conntrack_init_net(net);
	if (ret < 0)
		goto out_net;

	if (net_eq(net, &init_net)) {
		/* For use by REJECT target */
		RCU_INIT_POINTER(ip_ct_attach, nf_conntrack_attach);
		RCU_INIT_POINTER(nf_ct_destroy, destroy_conntrack);

		/* Howto get NAT offsets */
		RCU_INIT_POINTER(nf_ct_nat_offset, NULL);
	}
	return 0;

out_net:
	if (net_eq(net, &init_net))
		nf_conntrack_cleanup_init_net();
out_init_net:
	return ret;
}

#ifdef CONFIG_ATP_CONNTRACK_CLEAN
/*Begin:added by luokunling l00192527 ,fon用户登录成功之后.只清楚当前用户连接跟踪,不清楚所有fon用户*/

/*一下代码是proc目录下 增加一个foncurrent_loginip文件项,
待fon用户login/logout写入当前的lan侧IP地址*/
extern struct proc_dir_entry proc_root;
struct proc_dir_entry *proc_foncurrentloginip = NULL;
static int proc_foncurrent_loginip_wr (struct file *file,
                             const char *buffer,
                             unsigned long count,
                             void *data)
{
    char buf[256] = {0};
    unsigned int ulfoncurloginip = 0x00000000;
    /*no need write operation*/
   snprintf(buf,sizeof(buf),"%s",buffer);
   sscanf(buf,"%d",&ulfoncurloginip);
   nf_conntrack_fonip = ulfoncurloginip;
   nf_ct_iterate_cleanup(&init_net, kill_fon, NULL);
   return count;
}

static int proc_foncurrent_loginip_rd (char *page, char **start, off_t off, int count, 
								int *eof, void *data)
{
    int len = 0;
    return len;
}

static void proc_foncurrent_loginip_create(void)
{
    proc_foncurrentloginip = create_proc_entry("foncurrent_loginip", S_IFREG | S_IRUSR , &proc_root ); 
    proc_foncurrentloginip->write_proc = (read_proc_t *)proc_foncurrent_loginip_wr;
    proc_foncurrentloginip->read_proc = (write_proc_t *)proc_foncurrent_loginip_rd;    
}
static void proc_foncurrent_loginip_remove(void)
{
    /* no problem if it was not registered */
    remove_proc_entry("foncurrent_loginip", NULL /* parent dir */);
}
static int __init init_foncurrent_loginip(void)
{
    proc_foncurrent_loginip_create();
}
static void __exit exit_foncurrent_loginip(void)
{
     proc_foncurrent_loginip_remove(); 
}

module_init(init_foncurrent_loginip);
module_exit(exit_foncurrent_loginip);
/*Begin:added by luokunling l00192527,2013/2/1*/
#endif