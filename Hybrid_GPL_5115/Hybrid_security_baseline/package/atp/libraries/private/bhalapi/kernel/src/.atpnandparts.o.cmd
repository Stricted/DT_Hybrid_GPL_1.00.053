cmd_/usr1/xuhui/Hybrid_security_baseline/package/atp/libraries/private/bhalapi/kernel/src/atpnandparts.o := /usr1/xuhui/Hybrid_security_baseline/output/host/usr/bin/arm-hsan-linux-uclibcgnueabi-gcc -Wp,-MD,/usr1/xuhui/Hybrid_security_baseline/package/atp/libraries/private/bhalapi/kernel/src/.atpnandparts.o.d  -nostdinc -isystem /opt/toolchain_hisi/arm-hsan-linux-uclibcgnueabi/bin/../lib/gcc/arm-hsan-linux-uclibcgnueabi/4.4.6/include -Iinclude  -I/usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include -include include/linux/autoconf.h -D__KERNEL__ -Iinclude -I/usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include -include include/linux/autoconf.h -mlittle-endian -Iarch/arm/mach-sd5115/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -I/usr1/xuhui/Hybrid_security_baseline/output/staging/usr/include -O2 -marm -fno-omit-frame-pointer -mapcs -mno-sched-prolog -mabi=aapcs-linux -mno-thumb-interwork -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -Wframe-larger-than=1024 -fno-stack-protector -fno-omit-frame-pointer -fno-optimize-sibling-calls -Wdeclaration-after-statement -Wno-pointer-sign -fwrapv -fno-dwarf2-cfi-asm -DCFG_LINUX_NET_PACKED -I. -I/include -I/usr1/xuhui/Hybrid_security_baseline/driver/hisi///include -I/usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/include -I/flash -I -I -I/usr1/xuhui/Hybrid_security_baseline/output/staging/usr/include   -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(atpnandparts)"  -D"KBUILD_MODNAME=KBUILD_STR(atpnandparts)"  -c -o /usr1/xuhui/Hybrid_security_baseline/package/atp/libraries/private/bhalapi/kernel/src/atpnandparts.o /usr1/xuhui/Hybrid_security_baseline/package/atp/libraries/private/bhalapi/kernel/src/atpnandparts.c

deps_/usr1/xuhui/Hybrid_security_baseline/package/atp/libraries/private/bhalapi/kernel/src/atpnandparts.o := \
  /usr1/xuhui/Hybrid_security_baseline/package/atp/libraries/private/bhalapi/kernel/src/atpnandparts.c \
    $(wildcard include/config/index.h) \
  include/linux/mtd/mtd.h \
    $(wildcard include/config/mtd/partitions.h) \
    $(wildcard include/config/mtd/debug.h) \
    $(wildcard include/config/mtd/debug/verbose.h) \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbd.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/types.h \
  include/asm-generic/int-ll64.h \
  include/linux/posix_types.h \
  include/linux/stddef.h \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
    $(wildcard include/config/brcm/bounce.h) \
  include/linux/compiler-gcc4.h \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/posix_types.h \
  include/linux/module.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/kallsyms.h) \
    $(wildcard include/config/markers.h) \
    $(wildcard include/config/tracepoints.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/module/unload.h) \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/sysfs.h) \
  include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  include/linux/poison.h \
  include/linux/prefetch.h \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/processor.h \
    $(wildcard include/config/mmu.h) \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/arm/thumb.h) \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/hwcap.h \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/cache.h \
    $(wildcard include/config/aeabi.h) \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/system.h \
    $(wildcard include/config/cpu/xsc3.h) \
    $(wildcard include/config/cpu/fa526.h) \
    $(wildcard include/config/cpu/sa1100.h) \
    $(wildcard include/config/cpu/sa110.h) \
    $(wildcard include/config/cpu/32v6k.h) \
  include/linux/linkage.h \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/linkage.h \
  include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
    $(wildcard include/config/x86.h) \
  include/linux/typecheck.h \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/irqflags.h \
  include/asm-generic/cmpxchg-local.h \
  include/linux/stat.h \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/stat.h \
  include/linux/time.h \
  include/linux/cache.h \
    $(wildcard include/config/arch/has/cache/line/size.h) \
  include/linux/kernel.h \
    $(wildcard include/config/mips/brcm.h) \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/spinlock/sleep.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/dynamic/debug.h) \
    $(wildcard include/config/ring/buffer.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
  /opt/toolchain_hisi/arm-hsan-linux-uclibcgnueabi/bin/../lib/gcc/arm-hsan-linux-uclibcgnueabi/4.4.6/include/stdarg.h \
  include/linux/bitops.h \
    $(wildcard include/config/generic/find/first/bit.h) \
    $(wildcard include/config/generic/find/last/bit.h) \
    $(wildcard include/config/generic/find/next/bit.h) \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/bitops.h \
  include/asm-generic/bitops/non-atomic.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/sched.h \
  include/asm-generic/bitops/hweight.h \
  include/asm-generic/bitops/lock.h \
  include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  include/linux/ratelimit.h \
  include/linux/param.h \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/param.h \
    $(wildcard include/config/hz.h) \
  include/linux/dynamic_debug.h \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/swab.h \
  include/linux/byteorder/generic.h \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/debug/bugverbose.h) \
  include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
  include/linux/seqlock.h \
  include/linux/spinlock.h \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/generic/lockbreak.h) \
    $(wildcard include/config/preempt.h) \
    $(wildcard include/config/debug/lock/alloc.h) \
  include/linux/preempt.h \
    $(wildcard include/config/debug/preempt.h) \
    $(wildcard include/config/preempt/notifiers.h) \
  include/linux/thread_info.h \
    $(wildcard include/config/compat.h) \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/thread_info.h \
    $(wildcard include/config/arm/thumbee.h) \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/fpstate.h \
    $(wildcard include/config/vfpv3.h) \
    $(wildcard include/config/iwmmxt.h) \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/domain.h \
    $(wildcard include/config/io/36.h) \
  include/linux/stringify.h \
  include/linux/bottom_half.h \
  include/linux/spinlock_types.h \
  include/linux/spinlock_types_up.h \
  include/linux/lockdep.h \
    $(wildcard include/config/lockdep.h) \
    $(wildcard include/config/lock/stat.h) \
    $(wildcard include/config/generic/hardirqs.h) \
  include/linux/spinlock_up.h \
  include/linux/spinlock_api_up.h \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/atomic.h \
  include/asm-generic/atomic.h \
  include/linux/math64.h \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/div64.h \
  include/linux/kmod.h \
  include/linux/gfp.h \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/zone/dma32.h) \
    $(wildcard include/config/highmem.h) \
  include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/unevictable/lru.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/sparsemem.h) \
    $(wildcard include/config/arch/populates/node/map.h) \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/cgroup/mem/res/ctlr.h) \
    $(wildcard include/config/have/memory/present.h) \
    $(wildcard include/config/need/node/memmap/size.h) \
    $(wildcard include/config/need/multiple/nodes.h) \
    $(wildcard include/config/have/arch/early/pfn/to/nid.h) \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/sparsemem/extreme.h) \
    $(wildcard include/config/nodes/span/other/nodes.h) \
    $(wildcard include/config/holes/in/zone.h) \
    $(wildcard include/config/arch/has/holes/memorymodel.h) \
  include/linux/wait.h \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/current.h \
  include/linux/threads.h \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/base/small.h) \
  include/linux/numa.h \
    $(wildcard include/config/nodes/shift.h) \
  include/linux/init.h \
    $(wildcard include/config/hotplug.h) \
  include/linux/section-names.h \
  include/linux/nodemask.h \
  include/linux/bitmap.h \
  include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/string.h \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/hugetlb/page.h) \
    $(wildcard include/config/hugetlb/page/size/variable.h) \
  include/linux/bounds.h \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/page.h \
    $(wildcard include/config/cpu/copy/v3.h) \
    $(wildcard include/config/cpu/copy/v4wt.h) \
    $(wildcard include/config/cpu/copy/v4wb.h) \
    $(wildcard include/config/cpu/copy/feroceon.h) \
    $(wildcard include/config/cpu/copy/fa.h) \
    $(wildcard include/config/cpu/xscale.h) \
    $(wildcard include/config/cpu/copy/v6.h) \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/glue.h \
    $(wildcard include/config/cpu/arm610.h) \
    $(wildcard include/config/cpu/arm710.h) \
    $(wildcard include/config/cpu/abrt/lv4t.h) \
    $(wildcard include/config/cpu/abrt/ev4.h) \
    $(wildcard include/config/cpu/abrt/ev4t.h) \
    $(wildcard include/config/cpu/abrt/ev5tj.h) \
    $(wildcard include/config/cpu/abrt/ev5t.h) \
    $(wildcard include/config/cpu/abrt/ev6.h) \
    $(wildcard include/config/cpu/abrt/ev7.h) \
    $(wildcard include/config/cpu/pabrt/ifar.h) \
    $(wildcard include/config/cpu/pabrt/noifar.h) \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/memory.h \
    $(wildcard include/config/page/offset.h) \
    $(wildcard include/config/dram/size.h) \
    $(wildcard include/config/dram/base.h) \
  include/linux/const.h \
  arch/arm/mach-sd5115/include/mach/memory.h \
    $(wildcard include/config/hisi/sd5115/af/fpga.h) \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/sizes.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/sparsemem/vmemmap.h) \
  include/asm-generic/page.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/have/arch/nodedata/extension.h) \
    $(wildcard include/config/memory/hotremove.h) \
  include/linux/notifier.h \
  include/linux/errno.h \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/errno.h \
  include/asm-generic/errno.h \
  include/asm-generic/errno-base.h \
  include/linux/mutex.h \
    $(wildcard include/config/debug/mutexes.h) \
  include/linux/rwsem.h \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
  include/linux/rwsem-spinlock.h \
  include/linux/srcu.h \
  include/linux/topology.h \
    $(wildcard include/config/sched/smt.h) \
    $(wildcard include/config/sched/mc.h) \
  include/linux/cpumask.h \
    $(wildcard include/config/disable/obsolete/cpumask/functions.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/cpumask/offstack.h) \
    $(wildcard include/config/debug/per/cpu/maps.h) \
  include/linux/smp.h \
    $(wildcard include/config/use/generic/smp/helpers.h) \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/topology.h \
  include/asm-generic/topology.h \
  include/linux/elf.h \
  include/linux/elf-em.h \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/elf.h \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/user.h \
  include/linux/kobject.h \
  include/linux/sysfs.h \
  include/linux/kref.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/alpha.h) \
    $(wildcard include/config/ia64.h) \
    $(wildcard include/config/ppc64.h) \
  include/linux/marker.h \
  include/linux/tracepoint.h \
  include/linux/rcupdate.h \
    $(wildcard include/config/classic/rcu.h) \
    $(wildcard include/config/tree/rcu.h) \
    $(wildcard include/config/preempt/rcu.h) \
  include/linux/completion.h \
  include/linux/rcuclassic.h \
    $(wildcard include/config/rcu/cpu/stall/detector.h) \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/local.h \
  include/asm-generic/local.h \
  include/linux/percpu.h \
    $(wildcard include/config/have/dynamic/per/cpu/area.h) \
  include/linux/slab.h \
    $(wildcard include/config/slab/debug.h) \
    $(wildcard include/config/debug/objects.h) \
    $(wildcard include/config/slub.h) \
    $(wildcard include/config/slob.h) \
    $(wildcard include/config/debug/slab.h) \
  include/linux/slab_def.h \
    $(wildcard include/config/kmemtrace.h) \
  include/trace/kmemtrace.h \
  include/linux/kmalloc_sizes.h \
  include/linux/pfn.h \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/percpu.h \
  include/asm-generic/percpu.h \
    $(wildcard include/config/have/setup/per/cpu/area.h) \
  include/linux/percpu-defs.h \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/module.h \
    $(wildcard include/config/arm/unwind.h) \
  include/linux/uio.h \
  include/linux/device.h \
    $(wildcard include/config/debug/devres.h) \
  include/linux/ioport.h \
  include/linux/klist.h \
  include/linux/pm.h \
    $(wildcard include/config/pm/sleep.h) \
  include/linux/semaphore.h \
  /usr1/xuhui/Hybrid_security_baseline/linux/2.6.30/arch/arm/include/asm/device.h \
    $(wildcard include/config/dmabounce.h) \
  include/linux/pm_wakeup.h \
    $(wildcard include/config/pm.h) \
  include/linux/mtd/compatmac.h \
  include/mtd/mtd-abi.h \
    $(wildcard include/config/hisi/sd5115.h) \
    $(wildcard include/config/brcmnand/mtd/extension.h) \
  include/linux/mtd/partitions.h \
    $(wildcard include/config/mtd/cmdline/parts.h) \
  /usr1/xuhui/Hybrid_security_baseline/output/staging/usr/include/atpnandparts.h \
  /usr1/xuhui/Hybrid_security_baseline/output/staging/usr/include/kerneltag.h \

/usr1/xuhui/Hybrid_security_baseline/package/atp/libraries/private/bhalapi/kernel/src/atpnandparts.o: $(deps_/usr1/xuhui/Hybrid_security_baseline/package/atp/libraries/private/bhalapi/kernel/src/atpnandparts.o)

$(deps_/usr1/xuhui/Hybrid_security_baseline/package/atp/libraries/private/bhalapi/kernel/src/atpnandparts.o):
