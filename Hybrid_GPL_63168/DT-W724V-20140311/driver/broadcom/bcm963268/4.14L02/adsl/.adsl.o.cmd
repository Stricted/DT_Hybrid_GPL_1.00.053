cmd_/home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/adsl.o := /home/liyupeng/zengyao/6.13/DT-W724V-20140311/output/host/usr/bin/mips-unknown-linux-uclibc-gcc -Wp,-MD,/home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/.adsl.o.d  -nostdinc -isystem /opt/toolchains/crosstools-mips-gcc-4.6-linux-3.4-uclibc-0.9.32-binutils-2.21/usr/lib/gcc/mips-unknown-linux-uclibc/4.6.2/include -I/home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include -Iarch/mips/include/generated -Iinclude  -include /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/include/linux/kconfig.h -D__KERNEL__ -I/home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include -Iarch/mips/include/generated -Iinclude -include /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/include/linux/kconfig.h -D"VMLINUX_LOAD_ADDRESS=0x80010000" -D"DATAOFFSET=0" -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -I/home/liyupeng/zengyao/6.13/DT-W724V-20140311/output/staging/usr/include -O2 -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -march=mips32 -Wa,-mips32 -Wa,--trap -I/home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/mach-bcm963xx -I/home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/../../bcmdrivers/opensource/include/bcm963xx -I/home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/../../shared/opensource/include/bcm963xx -I/home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/mach-generic -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -DCC_HAVE_ASM_GOTO -DVECTORING -DADSL_ANNEXB -DSUPPORT_STATUS_BACKUP -DLMEM_ACCESS_WORKAROUND -DCONFIG_VDSL_SUPPORTED -DUSE_CXSY_OVH_MSG_WORKAROUND -DUSE_CXSY_OVH_MSG_COUNTER_WORKAROUND -DUSE_CXSY_OVH_MSG_DISABLE_POLLING_CMD -DLINUX_FW_VERSION=414 -DXDSL_DRV -fms-extensions  -DMODULE -mlong-calls -DMODULE -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(adsl)"  -D"KBUILD_MODNAME=KBUILD_STR(adsldd)" -c -o /home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/adsl.o /home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/adsl.c

source_/home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/adsl.o := /home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/adsl.c

deps_/home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/adsl.o := \
    $(wildcard include/config/bcm963x8.h) \
  include/linux/version.h \
  include/linux/kernel.h \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/bcm/kf/printk/int/enabled.h) \
    $(wildcard include/config/bcm/printk/int/enabled.h) \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/atomic/sleep.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/ring/buffer.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/atp/common.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/compaction.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
  include/linux/sysinfo.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
    $(wildcard include/config/bcm/kf/unaligned/exception.h) \
    $(wildcard include/config/mips/bcm963xx.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/types.h \
    $(wildcard include/config/64bit/phys/addr.h) \
  include/asm-generic/int-ll64.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/linux/posix_types.h \
  include/linux/stddef.h \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/bcm/kf/bounce.h) \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
    $(wildcard include/config/brcm/bounce.h) \
  include/linux/compiler-gcc4.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/posix_types.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/sgidefs.h \
  include/asm-generic/posix_types.h \
  /opt/toolchains/crosstools-mips-gcc-4.6-linux-3.4-uclibc-0.9.32-binutils-2.21/usr/lib/gcc/mips-unknown-linux-uclibc/4.6.2/include/stdarg.h \
  include/linux/linkage.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/linkage.h \
  include/linux/bitops.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/bitops.h \
    $(wildcard include/config/cpu/mipsr2.h) \
  include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/preempt/rt/full.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
  include/linux/typecheck.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/irqflags.h \
    $(wildcard include/config/mips/mt/smtc.h) \
    $(wildcard include/config/irq/cpu.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/hazards.h \
    $(wildcard include/config/cpu/cavium/octeon.h) \
    $(wildcard include/config/cpu/mipsr1.h) \
    $(wildcard include/config/mips/alchemy.h) \
    $(wildcard include/config/cpu/bmips.h) \
    $(wildcard include/config/cpu/loongson2.h) \
    $(wildcard include/config/cpu/r10000.h) \
    $(wildcard include/config/cpu/r5500.h) \
    $(wildcard include/config/cpu/rm9000.h) \
    $(wildcard include/config/cpu/sb1.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/cpu-features.h \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/32bit.h) \
    $(wildcard include/config/cpu/mipsr2/irq/vi.h) \
    $(wildcard include/config/cpu/mipsr2/irq/ei.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/cpu.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/cpu-info.h \
    $(wildcard include/config/mips/mt/smp.h) \
    $(wildcard include/config/bcm/kf/cpu/data/cpuid.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/cache.h \
    $(wildcard include/config/mips/l1/cache/shift.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/mach-generic/kmalloc.h \
    $(wildcard include/config/dma/coherent.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/mach-bcm963xx/cpu-feature-overrides.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/barrier.h \
    $(wildcard include/config/cpu/has/sync.h) \
    $(wildcard include/config/sgi/ip28.h) \
    $(wildcard include/config/cpu/has/wb.h) \
    $(wildcard include/config/weak/ordering.h) \
    $(wildcard include/config/weak/reordering/beyond/llsc.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/addrspace.h \
    $(wildcard include/config/cpu/r8000.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/mach-generic/spaces.h \
    $(wildcard include/config/dma/noncoherent.h) \
    $(wildcard include/config/bcm/kf/fixaddr/top.h) \
    $(wildcard include/config/bcm968500.h) \
  include/linux/const.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/break.h \
  include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
    $(wildcard include/config/debug/bugverbose.h) \
    $(wildcard include/config/preempt/rt/base.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/byteorder.h \
  include/linux/byteorder/big_endian.h \
  include/linux/swab.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/swab.h \
  include/linux/byteorder/generic.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/war.h \
    $(wildcard include/config/cpu/r4000/workarounds.h) \
    $(wildcard include/config/cpu/r4400/workarounds.h) \
    $(wildcard include/config/cpu/daddi/workarounds.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/mach-bcm963xx/war.h \
  include/asm-generic/bitops/non-atomic.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/ffz.h \
  include/asm-generic/bitops/find.h \
    $(wildcard include/config/generic/find/first/bit.h) \
  include/asm-generic/bitops/sched.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/arch_hweight.h \
  include/asm-generic/bitops/arch_hweight.h \
  include/asm-generic/bitops/const_hweight.h \
  include/asm-generic/bitops/le.h \
  include/asm-generic/bitops/ext2-atomic.h \
  include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  include/linux/printk.h \
    $(wildcard include/config/early/printk.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/dynamic/debug.h) \
  include/linux/init.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/hotplug.h) \
  include/linux/dynamic_debug.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/div64.h \
  include/asm-generic/div64.h \
  include/linux/sched.h \
    $(wildcard include/config/sched/debug.h) \
    $(wildcard include/config/bcm/kf/schedaudit.h) \
    $(wildcard include/config/prove/rcu.h) \
    $(wildcard include/config/no/hz.h) \
    $(wildcard include/config/lockup/detector.h) \
    $(wildcard include/config/detect/hung/task.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/core/dump/default/elf/headers.h) \
    $(wildcard include/config/sched/autogroup.h) \
    $(wildcard include/config/virt/cpu/accounting.h) \
    $(wildcard include/config/bsd/process/acct.h) \
    $(wildcard include/config/taskstats.h) \
    $(wildcard include/config/audit.h) \
    $(wildcard include/config/cgroups.h) \
    $(wildcard include/config/inotify/user.h) \
    $(wildcard include/config/fanotify.h) \
    $(wildcard include/config/epoll.h) \
    $(wildcard include/config/posix/mqueue.h) \
    $(wildcard include/config/keys.h) \
    $(wildcard include/config/perf/events.h) \
    $(wildcard include/config/schedstats.h) \
    $(wildcard include/config/task/delay/acct.h) \
    $(wildcard include/config/fair/group/sched.h) \
    $(wildcard include/config/rt/group/sched.h) \
    $(wildcard include/config/preempt/notifiers.h) \
    $(wildcard include/config/blk/dev/io/trace.h) \
    $(wildcard include/config/preempt/rcu.h) \
    $(wildcard include/config/tree/preempt/rcu.h) \
    $(wildcard include/config/rcu/boost.h) \
    $(wildcard include/config/compat/brk.h) \
    $(wildcard include/config/generic/hardirqs.h) \
    $(wildcard include/config/cc/stackprotector.h) \
    $(wildcard include/config/sysvipc.h) \
    $(wildcard include/config/auditsyscall.h) \
    $(wildcard include/config/rt/mutexes.h) \
    $(wildcard include/config/debug/mutexes.h) \
    $(wildcard include/config/lockdep.h) \
    $(wildcard include/config/block.h) \
    $(wildcard include/config/task/xacct.h) \
    $(wildcard include/config/cpusets.h) \
    $(wildcard include/config/futex.h) \
    $(wildcard include/config/compat.h) \
    $(wildcard include/config/fault/injection.h) \
    $(wildcard include/config/latencytop.h) \
    $(wildcard include/config/function/graph/tracer.h) \
    $(wildcard include/config/wakeup/latency/hist.h) \
    $(wildcard include/config/missed/timer/offsets/hist.h) \
    $(wildcard include/config/cgroup/mem/res/ctlr.h) \
    $(wildcard include/config/have/hw/breakpoint.h) \
    $(wildcard include/config/highmem.h) \
    $(wildcard include/config/cpumask/offstack.h) \
    $(wildcard include/config/have/unstable/sched/clock.h) \
    $(wildcard include/config/irq/time/accounting.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/proc/fs.h) \
    $(wildcard include/config/cfs/bandwidth.h) \
    $(wildcard include/config/stack/growsup.h) \
    $(wildcard include/config/debug/stack/usage.h) \
    $(wildcard include/config/preempt/count.h) \
    $(wildcard include/config/preempt.h) \
    $(wildcard include/config/cgroup/sched.h) \
    $(wildcard include/config/mm/owner.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/hz.h) \
  include/linux/capability.h \
  include/linux/threads.h \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/base/small.h) \
  include/linux/timex.h \
  include/linux/time.h \
    $(wildcard include/config/arch/uses/gettimeoffset.h) \
  include/linux/cache.h \
    $(wildcard include/config/arch/has/cache/line/size.h) \
  include/linux/seqlock.h \
  include/linux/spinlock.h \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/generic/lockbreak.h) \
    $(wildcard include/config/debug/lock/alloc.h) \
  include/linux/preempt.h \
    $(wildcard include/config/debug/preempt.h) \
  include/linux/thread_info.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/thread_info.h \
    $(wildcard include/config/page/size/4kb.h) \
    $(wildcard include/config/bcm/kf/thread/size/fix.h) \
    $(wildcard include/config/page/size/8kb.h) \
    $(wildcard include/config/page/size/16kb.h) \
    $(wildcard include/config/page/size/32kb.h) \
    $(wildcard include/config/page/size/64kb.h) \
    $(wildcard include/config/mips32/o32.h) \
    $(wildcard include/config/mips32/n32.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/processor.h \
    $(wildcard include/config/cavium/octeon/cvmseg/size.h) \
    $(wildcard include/config/mips/mt/fpaff.h) \
    $(wildcard include/config/cpu/has/prefetch.h) \
  include/linux/cpumask.h \
    $(wildcard include/config/debug/per/cpu/maps.h) \
    $(wildcard include/config/bcm/kf/cpp/support.h) \
    $(wildcard include/config/disable/obsolete/cpumask/functions.h) \
  include/linux/bitmap.h \
  include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/string.h \
    $(wildcard include/config/cpu/r3000.h) \
  include/linux/bug.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/cachectl.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/mipsregs.h \
    $(wildcard include/config/cpu/vr41xx.h) \
    $(wildcard include/config/hugetlb/page.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/prefetch.h \
  include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  include/linux/poison.h \
    $(wildcard include/config/illegal/pointer/value.h) \
  include/linux/stringify.h \
  include/linux/bottom_half.h \
  include/linux/spinlock_types.h \
    $(wildcard include/config/dt/common.h) \
  include/linux/spinlock_types_raw.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/spinlock_types.h \
  include/linux/lockdep.h \
    $(wildcard include/config/lock/stat.h) \
  include/linux/spinlock_types_nort.h \
  include/linux/rwlock_types.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/spinlock.h \
  include/linux/rwlock.h \
  include/linux/spinlock_api_smp.h \
    $(wildcard include/config/inline/spin/lock.h) \
    $(wildcard include/config/inline/spin/lock/bh.h) \
    $(wildcard include/config/inline/spin/lock/irq.h) \
    $(wildcard include/config/inline/spin/lock/irqsave.h) \
    $(wildcard include/config/inline/spin/trylock.h) \
    $(wildcard include/config/inline/spin/trylock/bh.h) \
    $(wildcard include/config/uninline/spin/unlock.h) \
    $(wildcard include/config/inline/spin/unlock/bh.h) \
    $(wildcard include/config/inline/spin/unlock/irq.h) \
    $(wildcard include/config/inline/spin/unlock/irqrestore.h) \
  include/linux/rwlock_api_smp.h \
    $(wildcard include/config/inline/read/lock.h) \
    $(wildcard include/config/inline/write/lock.h) \
    $(wildcard include/config/inline/read/lock/bh.h) \
    $(wildcard include/config/inline/write/lock/bh.h) \
    $(wildcard include/config/inline/read/lock/irq.h) \
    $(wildcard include/config/inline/write/lock/irq.h) \
    $(wildcard include/config/inline/read/lock/irqsave.h) \
    $(wildcard include/config/inline/write/lock/irqsave.h) \
    $(wildcard include/config/inline/read/trylock.h) \
    $(wildcard include/config/inline/write/trylock.h) \
    $(wildcard include/config/inline/read/unlock.h) \
    $(wildcard include/config/inline/write/unlock.h) \
    $(wildcard include/config/inline/read/unlock/bh.h) \
    $(wildcard include/config/inline/write/unlock/bh.h) \
    $(wildcard include/config/inline/read/unlock/irq.h) \
    $(wildcard include/config/inline/write/unlock/irq.h) \
    $(wildcard include/config/inline/read/unlock/irqrestore.h) \
    $(wildcard include/config/inline/write/unlock/irqrestore.h) \
  include/linux/atomic.h \
    $(wildcard include/config/arch/has/atomic/or.h) \
    $(wildcard include/config/generic/atomic64.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/atomic.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/cmpxchg.h \
  include/asm-generic/cmpxchg-local.h \
  include/asm-generic/atomic-long.h \
  include/asm-generic/atomic64.h \
  include/linux/math64.h \
  include/linux/param.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/timex.h \
  include/linux/jiffies.h \
  include/linux/rbtree.h \
  include/linux/errno.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/errno.h \
  include/asm-generic/errno-base.h \
  include/linux/nodemask.h \
  include/linux/numa.h \
    $(wildcard include/config/nodes/shift.h) \
  include/linux/mm_types.h \
    $(wildcard include/config/split/ptlock/cpus.h) \
    $(wildcard include/config/have/cmpxchg/double.h) \
    $(wildcard include/config/have/aligned/struct/page.h) \
    $(wildcard include/config/want/page/debug/flags.h) \
    $(wildcard include/config/kmemcheck.h) \
    $(wildcard include/config/aio.h) \
    $(wildcard include/config/mmu/notifier.h) \
    $(wildcard include/config/transparent/hugepage.h) \
  include/linux/auxvec.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/auxvec.h \
  include/linux/prio_tree.h \
  include/linux/rwsem.h \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
  include/linux/rwsem-spinlock.h \
  include/linux/completion.h \
  include/linux/wait.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/current.h \
  include/asm-generic/current.h \
  include/linux/page-debug-flags.h \
    $(wildcard include/config/page/poisoning.h) \
    $(wildcard include/config/page/guard.h) \
    $(wildcard include/config/page/debug/something/else.h) \
  include/linux/rcupdate.h \
    $(wildcard include/config/rcu/torture/test.h) \
    $(wildcard include/config/tree/rcu.h) \
    $(wildcard include/config/rcu/trace.h) \
    $(wildcard include/config/tiny/rcu.h) \
    $(wildcard include/config/tiny/preempt/rcu.h) \
    $(wildcard include/config/debug/objects/rcu/head.h) \
    $(wildcard include/config/preempt/rt.h) \
    $(wildcard include/config/bcm/kf/rcu/constant/bug.h) \
  include/linux/debugobjects.h \
    $(wildcard include/config/debug/objects.h) \
    $(wildcard include/config/debug/objects/free.h) \
  include/linux/rcutree.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/page.h \
    $(wildcard include/config/cpu/mips32.h) \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/sparsemem.h) \
    $(wildcard include/config/need/multiple/nodes.h) \
  include/linux/pfn.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/io.h \
  include/asm-generic/iomap.h \
    $(wildcard include/config/has/ioport.h) \
    $(wildcard include/config/pci.h) \
    $(wildcard include/config/generic/iomap.h) \
  include/asm-generic/pci_iomap.h \
    $(wildcard include/config/no/generic/pci/ioport/map.h) \
    $(wildcard include/config/generic/pci/iomap.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/pgtable-bits.h \
    $(wildcard include/config/cpu/tx39xx.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/mach-generic/ioremap.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/mach-generic/mangle-port.h \
    $(wildcard include/config/swap/io/space.h) \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
  include/asm-generic/getorder.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/mmu.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/kmap_types.h \
    $(wildcard include/config/debug/highmem.h) \
  include/asm-generic/kmap_types.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/ptrace.h \
    $(wildcard include/config/cpu/has/smartmips.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/isadep.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/cputime.h \
  include/asm-generic/cputime.h \
  include/linux/smp.h \
    $(wildcard include/config/use/generic/smp/helpers.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/smp.h \
    $(wildcard include/config/bcm/kf/mips/bcm963xx.h) \
    $(wildcard include/config/bcm/hostmips/pwrsave/timers.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/smp-ops.h \
    $(wildcard include/config/smp/up.h) \
    $(wildcard include/config/mips/cmp.h) \
  include/linux/sem.h \
  include/linux/ipc.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/ipcbuf.h \
  include/asm-generic/ipcbuf.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/sembuf.h \
  include/linux/signal.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/signal.h \
    $(wildcard include/config/trad/signals.h) \
  include/asm-generic/signal-defs.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/sigcontext.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/siginfo.h \
  include/asm-generic/siginfo.h \
  include/linux/pid.h \
  include/linux/percpu.h \
    $(wildcard include/config/need/per/cpu/embed/first/chunk.h) \
    $(wildcard include/config/need/per/cpu/page/first/chunk.h) \
    $(wildcard include/config/have/setup/per/cpu/area.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/percpu.h \
  include/asm-generic/percpu.h \
  include/linux/percpu-defs.h \
    $(wildcard include/config/debug/force/weak/per/cpu.h) \
  include/linux/topology.h \
    $(wildcard include/config/sched/smt.h) \
    $(wildcard include/config/sched/mc.h) \
    $(wildcard include/config/sched/book.h) \
    $(wildcard include/config/use/percpu/numa/node/id.h) \
    $(wildcard include/config/have/memoryless/nodes.h) \
  include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/zone/dma32.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/have/memblock/node/map.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/no/bootmem.h) \
    $(wildcard include/config/have/memory/present.h) \
    $(wildcard include/config/need/node/memmap/size.h) \
    $(wildcard include/config/have/memblock/node.h) \
    $(wildcard include/config/have/arch/early/pfn/to/nid.h) \
    $(wildcard include/config/sparsemem/extreme.h) \
    $(wildcard include/config/have/arch/pfn/valid.h) \
    $(wildcard include/config/nodes/span/other/nodes.h) \
    $(wildcard include/config/holes/in/zone.h) \
    $(wildcard include/config/arch/has/holes/memorymodel.h) \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/hugetlb/page/size/variable.h) \
  include/generated/bounds.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/memory/hotremove.h) \
    $(wildcard include/config/have/arch/nodedata/extension.h) \
  include/linux/notifier.h \
  include/linux/mutex.h \
    $(wildcard include/config/have/arch/mutex/cpu/relax.h) \
  include/linux/srcu.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/topology.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/mach-generic/topology.h \
  include/asm-generic/topology.h \
  include/linux/proportions.h \
  include/linux/percpu_counter.h \
  include/linux/seccomp.h \
    $(wildcard include/config/seccomp.h) \
  include/linux/rculist.h \
  include/linux/rtmutex.h \
    $(wildcard include/config/debug/rt/mutexes.h) \
  include/linux/plist.h \
    $(wildcard include/config/debug/pi/list.h) \
  include/linux/resource.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/resource.h \
  include/asm-generic/resource.h \
  include/linux/timer.h \
    $(wildcard include/config/timer/stats.h) \
    $(wildcard include/config/debug/objects/timers.h) \
  include/linux/ktime.h \
    $(wildcard include/config/ktime/scalar.h) \
  include/linux/hrtimer.h \
    $(wildcard include/config/high/res/timers.h) \
    $(wildcard include/config/timerfd.h) \
  include/linux/timerqueue.h \
  include/linux/task_io_accounting.h \
    $(wildcard include/config/task/io/accounting.h) \
  include/linux/latencytop.h \
  include/linux/cred.h \
    $(wildcard include/config/debug/credentials.h) \
    $(wildcard include/config/security.h) \
    $(wildcard include/config/user/ns.h) \
  include/linux/key.h \
    $(wildcard include/config/sysctl.h) \
  include/linux/sysctl.h \
    $(wildcard include/config/atp/hybrid/greaccel.h) \
    $(wildcard include/config/atp/port/scan.h) \
    $(wildcard include/config/atp/igmp.h) \
    $(wildcard include/config/dt/igmp/qqic.h) \
    $(wildcard include/config/atp/hybrid/reorder.h) \
    $(wildcard include/config/dt/qos.h) \
    $(wildcard include/config/atp/conntrack/clean.h) \
  include/linux/selinux.h \
    $(wildcard include/config/security/selinux.h) \
  include/linux/llist.h \
    $(wildcard include/config/arch/have/nmi/safe/cmpxchg.h) \
  include/linux/hardirq.h \
  include/linux/ftrace_irq.h \
    $(wildcard include/config/ftrace/nmi/enter.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/hardirq.h \
  include/asm-generic/hardirq.h \
  include/linux/irq_cpustat.h \
  include/linux/irq.h \
    $(wildcard include/config/s390.h) \
    $(wildcard include/config/irq/release/method.h) \
    $(wildcard include/config/generic/pending/irq.h) \
  include/linux/gfp.h \
    $(wildcard include/config/pm/sleep.h) \
  include/linux/mmdebug.h \
    $(wildcard include/config/debug/vm.h) \
    $(wildcard include/config/debug/virtual.h) \
  include/linux/irqreturn.h \
  include/linux/irqnr.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/irq.h \
    $(wildcard include/config/i8259.h) \
    $(wildcard include/config/mips/mt/smtc/irqaff.h) \
    $(wildcard include/config/mips/mt/smtc/im/backstop.h) \
  include/linux/irqdomain.h \
    $(wildcard include/config/irq/domain.h) \
    $(wildcard include/config/of/irq.h) \
  include/linux/radix-tree.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/mipsmtregs.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/mach-generic/irq.h \
    $(wildcard include/config/bcm96838.h) \
    $(wildcard include/config/irq/cpu/rm7k.h) \
    $(wildcard include/config/irq/cpu/rm9k.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/irq_regs.h \
  include/linux/irqdesc.h \
    $(wildcard include/config/irq/preflow/fasteoi.h) \
    $(wildcard include/config/sparse/irq.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/hw_irq.h \
  include/linux/aio.h \
  include/linux/workqueue.h \
    $(wildcard include/config/debug/objects/work.h) \
    $(wildcard include/config/freezer.h) \
  include/linux/aio_abi.h \
  include/linux/uio.h \
  include/linux/interrupt.h \
    $(wildcard include/config/irq/forced/threading.h) \
    $(wildcard include/config/generic/irq/probe.h) \
  include/linux/kref.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/output/staging/usr/include/boardparms.h \
    $(wildcard include/config/mdio.h) \
    $(wildcard include/config/mdio/pseudo/phy.h) \
    $(wildcard include/config/spi/ssb/0.h) \
    $(wildcard include/config/spi/ssb/1.h) \
    $(wildcard include/config/spi/ssb/2.h) \
    $(wildcard include/config/spi/ssb/3.h) \
    $(wildcard include/config/mmap.h) \
    $(wildcard include/config/gpio/mdio.h) \
    $(wildcard include/config/hs/spi/ssb/0.h) \
    $(wildcard include/config/hs/spi/ssb/1.h) \
    $(wildcard include/config/hs/spi/ssb/2.h) \
    $(wildcard include/config/hs/spi/ssb/3.h) \
    $(wildcard include/config/hs/spi/ssb/4.h) \
    $(wildcard include/config/hs/spi/ssb/5.h) \
    $(wildcard include/config/hs/spi/ssb/6.h) \
    $(wildcard include/config/hs/spi/ssb/7.h) \
    $(wildcard include/config/bcm96828.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/output/staging/usr/include/bcmtypes.h \
    $(wildcard include/config/file.h) \
    $(wildcard include/config/psi.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/BcmOs.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/output/staging/usr/include/board.h \
    $(wildcard include/config/bcm96368.h) \
    $(wildcard include/config/bcm96816.h) \
    $(wildcard include/config/bcm96818.h) \
    $(wildcard include/config/bcm/cpld1.h) \
    $(wildcard include/config/bcm/avs/pwrsave.h) \
    $(wildcard include/config/bcm/ddr/self/refresh/pwrsave.h) \
    $(wildcard include/config/bcm/pwrmngt/ddr/sr/api.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/output/staging/usr/include/bcm_hwdefs.h \
    $(wildcard include/config/brcm/ikos.h) \
    $(wildcard include/config/bcm963381.h) \
    $(wildcard include/config/bcm96328.h) \
    $(wildcard include/config/bcm96362.h) \
    $(wildcard include/config/bcm963268.h) \
    $(wildcard include/config/bcm96318.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/output/staging/usr/include/bcm_map.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/output/staging/usr/include/63268_map.h \
    $(wildcard include/config/master/en.h) \
    $(wildcard include/config/flowc/ch1/en.h) \
    $(wildcard include/config/flowc/ch3/en.h) \
    $(wildcard include/config/flowc/ch5/en.h) \
    $(wildcard include/config/flowc/ch7/en.h) \
    $(wildcard include/config/endma.h) \
    $(wildcard include/config/pkthalt.h) \
    $(wildcard include/config/bursthalt.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/output/staging/usr/include/bcmtypes.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/output/staging/usr/include/63268_common.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/output/staging/usr/include/63268_intr.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/output/staging/usr/include/63268_map_part.h \
    $(wildcard include/config/lock.h) \
    $(wildcard include/config/2/bar1/size/mask.h) \
    $(wildcard include/config/2/bar1/disable.h) \
    $(wildcard include/config/bcm/gmac.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/output/staging/usr/include/bcm_intr.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/output/staging/usr/include/63268_intr.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/output/staging/usr/include/bcmadsl.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/output/staging/usr/include/AdslMibDef.h \
    $(wildcard include/config/vdsl/supported.h) \
    $(wildcard include/config/bcm96358.h) \
    $(wildcard include/config/bcm96348.h) \
    $(wildcard include/config/bcm96338.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/softdsl/AdslCoreDefs.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/softdsl/../adslcore63268B/adsl_defs.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/softdsl/AdslXfaceData.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/softdsl/CircBuf.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/output/staging/usr/include/AdslMibDef.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/output/staging/usr/include/DiagDef.h \
    $(wildcard include/config/a.h) \
    $(wildcard include/config/c.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/BcmAdslCore.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/AdslCore.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/softdsl/SoftDsl.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/softdsl/AdslCoreDefs.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/softdsl/SoftModem.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/softdsl/SoftModemTypes.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/softdsl/SoftDslG993p2.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/softdsl/Que.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/softdsl/SoftAtmVc.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/BcmAdslDiag.h \
  include/linux/module.h \
    $(wildcard include/config/sysfs.h) \
    $(wildcard include/config/unused/symbols.h) \
    $(wildcard include/config/kallsyms.h) \
    $(wildcard include/config/tracepoints.h) \
    $(wildcard include/config/event/tracing.h) \
    $(wildcard include/config/module/unload.h) \
    $(wildcard include/config/constructors.h) \
    $(wildcard include/config/debug/set/module/ronx.h) \
  include/linux/stat.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/stat.h \
  include/linux/kmod.h \
  include/linux/elf.h \
  include/linux/elf-em.h \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/elf.h \
    $(wildcard include/config/mips32/compat.h) \
  include/linux/kobject.h \
  include/linux/sysfs.h \
  include/linux/kobject_ns.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/alpha.h) \
    $(wildcard include/config/ia64.h) \
    $(wildcard include/config/ppc64.h) \
  include/linux/tracepoint.h \
  include/linux/static_key.h \
  include/linux/jump_label.h \
    $(wildcard include/config/jump/label.h) \
    $(wildcard include/config/preempt/base.h) \
  include/linux/export.h \
    $(wildcard include/config/symbol/prefix.h) \
    $(wildcard include/config/modversions.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/module.h \
    $(wildcard include/config/cpu/mips32/r1.h) \
    $(wildcard include/config/cpu/mips32/r2.h) \
    $(wildcard include/config/cpu/mips64/r1.h) \
    $(wildcard include/config/cpu/mips64/r2.h) \
    $(wildcard include/config/cpu/r4300.h) \
    $(wildcard include/config/cpu/r4x00.h) \
    $(wildcard include/config/cpu/tx49xx.h) \
    $(wildcard include/config/cpu/r5000.h) \
    $(wildcard include/config/cpu/r5432.h) \
    $(wildcard include/config/cpu/r6000.h) \
    $(wildcard include/config/cpu/nevada.h) \
    $(wildcard include/config/cpu/rm7000.h) \
    $(wildcard include/config/cpu/xlr.h) \
    $(wildcard include/config/cpu/xlp.h) \
  /home/liyupeng/zengyao/6.13/DT-W724V-20140311/linux/3.4.11/arch/mips/include/asm/uaccess.h \

/home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/adsl.o: $(deps_/home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/adsl.o)

$(deps_/home/liyupeng/zengyao/6.13/DT-W724V-20140311/driver/broadcom/bcm963268/4.14L02/adsl/adsl.o):
