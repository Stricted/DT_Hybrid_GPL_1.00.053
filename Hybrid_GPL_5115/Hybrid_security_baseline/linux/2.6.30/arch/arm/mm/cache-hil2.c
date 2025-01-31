#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>

#include <asm/cacheflush.h>
#include <asm/irq.h>

#include <mach/irqs.h>
#include <mach/platform.h>
#include <mach/early-debug.h>
#include "cache-hil2.h"

#define CACHE_LINE_SIZE		L2_LINE_SIZE

#define DRIVER_NAME "l2cache"

static void __iomem *l2cache_base;
static int l2cache_flag = 0;

static void l2cache_release(struct device *dev)
{
}

static int l2cache_probe(struct platform_device *pdev)
{
	return 0;
}

static int l2cache_remove(struct platform_device *pdev)
{
	return 0;
}

static inline void sync_writel(unsigned long val, unsigned long reg,
			       unsigned long complete_mask)
{
	writel(val, l2cache_base + reg);
}

static inline void cache_sync(void)
{
    unsigned int reg;
    reg = readl(l2cache_base + REG_L2_SYNC);  
}

/******************************************************************************
*
* l2_invalid_auto -  auto invalid l2 cache
* 
* This routine auto invalid l2 cache 
*	
*******************************************************************************/ 
static void l2_invalid_auto(unsigned int way_num)      /* need invalid cache way num */
{
    unsigned int reg;
    
    cache_sync();
    reg = (way_num << BIT_L2_MAINT_AUTO_WAYADDRESS) | (0x1 << BIT_L2_MAINT_AUTO_START);
    sync_writel(reg, REG_L2_MAINT_AUTO,reg );
    while(!( readl(l2cache_base + REG_L2_RINT) & ( 0X1 << BIT_L2_RINT_AUTO_END ))); 
    reg = readl(l2cache_base + REG_L2_RINT);
    sync_writel(reg, REG_L2_INTCLR,reg);
    cache_sync();
}

/******************************************************************************
 *
 * l2_cache_auto-  auto clean l2 cache
 *
 * This routine auto clean l2 cache
 *
 *******************************************************************************/
static void l2_cache_auto(unsigned int way_num)      /* need clean cache way num */
{
	unsigned int reg;

	cache_sync();
	reg = (way_num << BIT_L2_MAINT_AUTO_WAYADDRESS) | (0x1 << BIT_L2_MAINT_AUTO_START | 0x2);
	sync_writel(reg, REG_L2_MAINT_AUTO,reg );
	while(!( readl(l2cache_base + REG_L2_RINT) & ( 0X1 << BIT_L2_RINT_AUTO_END )));
	reg = readl(l2cache_base + REG_L2_RINT);
	sync_writel(reg, REG_L2_INTCLR,reg);
	cache_sync();
}     

void l2cache_inv_all(void)
{
	unsigned int way_num;
	
	if(!l2cache_flag)
	{
		printk("%s:l2cache_flag = %d,L2 cache may not enable!\n",__FUNCTION__,l2cache_flag);
		return;
	}

	cache_sync();
	/*invalidate cache  all-way*/
	for(way_num = 0; way_num < L2_WAY_NUM; way_num ++)
    	{
		l2_invalid_auto(way_num);
    	}
	cache_sync();
}
EXPORT_SYMBOL(l2cache_inv_all);

void l2cache_clean_all(void)
{
	unsigned int way_num;

	if(!l2cache_flag)
	{
		printk("%s:l2cache_flag = %d,L2 cache may not enable!\n",__FUNCTION__,l2cache_flag);
		return;
	}

	cache_sync();
	/*invalidate cache  all-way*/
	for(way_num = 0; way_num < L2_WAY_NUM; way_num ++)
    	{
		l2_cache_auto(way_num);
    	}
	cache_sync();
}
EXPORT_SYMBOL(l2cache_clean_all);

void l2cache_flush_all(void)
{
	if(!l2cache_flag)
	{
		printk("%s:l2cache_flag = %d,L2 cache may not enable!\n",__FUNCTION__,l2cache_flag);
		return;
	}
	l2cache_clean_all();
	l2cache_inv_all();
}
EXPORT_SYMBOL(l2cache_flush_all);

void l2cache_inv_range(unsigned long start, unsigned long end)
{
	unsigned long addr;
 	unsigned long reg;
    	
	if(!l2cache_flag)
	{
		printk("%s:l2cache_flag = %d,L2 cache may not enable!\n",__FUNCTION__,l2cache_flag);
		return;
	}

#ifdef CONFIG_CACHE_HIL2_DEBUG
	printk("%s: start addr: 0x%x, end addr: 0x%x\n",__FUNCTION__,(unsigned int)(start),(unsigned int)(end));
#endif
	cache_sync();
	start &= ~(CACHE_LINE_SIZE - 1);

	for (addr = start; addr < end; addr += CACHE_LINE_SIZE){
	    	reg = addr | BIT_L2_INVALID_BYADDRESS;
    		sync_writel(reg, REG_L2_INVALID,reg);	
	}
	cache_sync();
}
EXPORT_SYMBOL(l2cache_inv_range);

void l2cache_clean_range(unsigned long start, unsigned long end)
{
	unsigned long addr;
	unsigned long reg;

	if(!l2cache_flag)
	{
		printk("%s:l2cache_flag = %d,L2 cache may not enable!\n",__FUNCTION__,l2cache_flag);
		return;
	}
	
#ifdef CONFIG_CACHE_HIL2_DEBUG
	printk("%s: start addr: 0x%x, end addr: 0x%x\n",__FUNCTION__,(unsigned int)(start),(unsigned int)(end));
#endif
	cache_sync();
	start &= ~(CACHE_LINE_SIZE - 1);
	for (addr = start; addr < end; addr += CACHE_LINE_SIZE)
	{
	    	reg = addr | BIT_L2_CLEAN_BYADDRESS;
    		sync_writel(reg, REG_L2_CLEAN,reg);
	}
	cache_sync();
}
EXPORT_SYMBOL(l2cache_clean_range);

void l2cache_flush_range(unsigned long start, unsigned long end)
{
	unsigned long addr;
	
	if(!l2cache_flag)
	{
		printk("%s:l2cache_flag = %d,L2 cache may not enable!\n",__FUNCTION__,l2cache_flag);
		return;
	}
	
#ifdef CONFIG_CACHE_HIL2_DEBUG
	printk("%s: start addr: 0x%x, end addr: 0x%x\n",__FUNCTION__,(unsigned int)(start),(unsigned int)(end));
#endif
	cache_sync();
	start &= ~(CACHE_LINE_SIZE - 1);
	for (addr = start; addr < end; addr += CACHE_LINE_SIZE)
	{
		sync_writel(addr | 1, REG_L2_CLEAN, 1);
		sync_writel(addr | 1, REG_L2_INVALID,1);
	}	
	cache_sync();
}
EXPORT_SYMBOL(l2cache_flush_range);

static irqreturn_t l2cache_handle(int irq, void *dev_id)
{
	unsigned int ret1,ret2,ret3;
	
	ret1 = readl(l2cache_base + REG_L2_RINT);
	ret2 = readl(l2cache_base + REG_L2_SPECIAL_CHECK0);
	ret3 = readl(l2cache_base + REG_L2_SPECIAL_CHECK1);
	writel(0,l2cache_base + REG_L2_INTCLR);//clear inner INT
	printk("L2cache interrupt,REG_L2_RINT is 0x%x, REG_L2_SPECIAL_CHECK0 is 0x%x,REG_L2_SPECIAL_CHECK1 is 0x%x!\n",ret1,ret2,ret3);
	return IRQ_HANDLED;
}

void l2cache_init(void)
{
	int ret1,ret2,ret3,aux,aux_val = 0;
	unsigned long flags;
	
	local_irq_save(flags);

	l2cache_base = (void __iomem *)IO_ADDRESS(REG_BASE_L2CACHE); 
	
	/* disable L2cache */
	writel(0, l2cache_base + REG_L2_CTRL);

	aux_val |= (0x1 << BIT_L2_AUCTRL_EVENT_BUS_EN) | (0x1 << BIT_L2_AUCTRL_MONITOR_EN);

	aux = readl(l2cache_base + REG_L2_AUCTRL);
	aux |= aux_val;
	writel(aux, l2cache_base + REG_L2_AUCTRL);

	/* clean last error int */
	writel(0, l2cache_base + REG_L2_RINT);
	writel(0, l2cache_base + REG_L2_SPECIAL_CHECK0);
	writel(0, l2cache_base + REG_L2_SPECIAL_CHECK1);

	/* enable inner INT except BIT_L2_INTMASK_AUTO_END */
	writel(0x3fff,l2cache_base + REG_L2_INTMASK);

	l2cache_flag = 1;

	l2cache_inv_all();

	/* enable L2cache */
	writel(1, l2cache_base + REG_L2_CTRL);

	ret1 = request_irq(INTNR_L2CACHE_CHK0_INT,l2cache_handle,0,"L2 chk0",NULL);
	ret2 = request_irq(INTNR_L2CACHE_CHK1_INT,l2cache_handle,0,"L2 chk1",NULL);
	ret3 = request_irq(INTNR_L2CACHE_INT_COMB,l2cache_handle,0,"L2 com",NULL);
	if(ret1 || ret2 || ret3)
		printk("request l2 cache irq error!\n");

	local_irq_restore(flags);

	printk(KERN_INFO "L2cache cache controller enabled\n");
}

#ifdef CONFIG_PM
static int l2cache_suspend(struct platform_device *dev, pm_message_t state)
{
	int ret=0;
	unsigned long flags;
	
	/* disable L2cache */
	local_irq_save(flags);

	writel(0, l2cache_base + REG_L2_CTRL);
	l2cache_flush_all();
	l2cache_flag = 0;

	free_irq(INTNR_L2CACHE_CHK0_INT,NULL);
	free_irq(INTNR_L2CACHE_CHK1_INT,NULL);
	free_irq(INTNR_L2CACHE_INT_COMB,NULL);

	local_irq_restore(flags);
	
	return ret;
}

static int l2cache_resume(struct platform_device *dev)
{
	unsigned long aux,aux_val = 0;
	unsigned long flags,ret,ret1,ret2;

	local_irq_save(flags);

	/* disable L2cache */
	writel(0, l2cache_base + REG_L2_CTRL);

	/* monitor */
	aux_val |= (0x1 << BIT_L2_AUCTRL_EVENT_BUS_EN) | (0x1 << BIT_L2_AUCTRL_MONITOR_EN);
	
	aux = readl(l2cache_base + REG_L2_AUCTRL);
	aux |= aux_val;
	writel(aux, l2cache_base + REG_L2_AUCTRL);

	/* clean last error int */
	writel(0, l2cache_base + REG_L2_RINT);
	writel(0, l2cache_base + REG_L2_SPECIAL_CHECK0);
	writel(0, l2cache_base + REG_L2_SPECIAL_CHECK1);

	/* enable inner INT except BIT_L2_INTMASK_AUTO_END */
	writel(0x3fff,l2cache_base + REG_L2_INTMASK);

	l2cache_flag = 1;
	l2cache_inv_all();

	/* enable L2cache */
	writel(1, l2cache_base + REG_L2_CTRL);

	ret  = request_irq(INTNR_L2CACHE_CHK0_INT,l2cache_handle,0,"L2 chk0",NULL);
	ret1 = request_irq(INTNR_L2CACHE_CHK1_INT,l2cache_handle,0,"L2 chk1",NULL);
	ret2 =request_irq(INTNR_L2CACHE_INT_COMB,l2cache_handle,0,"L2 com",NULL);
	if (ret || ret1 || ret2)
		printk("request l2 cache irq error!\n");

	local_irq_restore(flags);

	return 0;
}
#else
#define l2cache_suspend	NULL
#define l2cache_resume	NULL
#endif

static struct resource l2cache_resources[] = {
	[0]={
		.start          = REG_BASE_L2CACHE,
		.end            = REG_BASE_L2CACHE + 0x10000 - 1,
		.flags          = IORESOURCE_MEM,
	},
	[1] = {
		.start          = INTNR_L2CACHE_CHK0_INT,
		.end            = INTNR_L2CACHE_CHK0_INT,
		.flags          = IORESOURCE_IRQ,
	},

};

static struct platform_device l2cache_device = {
	.name           = "l2cache",
	.id             = 0,
	.dev = {
		.release  = l2cache_release,
	},
	.num_resources  = ARRAY_SIZE(l2cache_resources),
	.resource       = l2cache_resources,
};

static struct platform_driver l2cache_driver = {
	.probe         = l2cache_probe,
	.remove        = l2cache_remove,
	.suspend       = l2cache_suspend,
	.resume        = l2cache_resume,
	.driver        = {
		.name          = DRIVER_NAME,
	},
};

static int __init l2cache_driver_init(void)
{	
	int ret = 0;
	
	ret = platform_device_register(&l2cache_device);
	if(ret){
		printk("Platform device register is failed!");
		return ret;
	}

	ret = platform_driver_register(&l2cache_driver);
	if(ret){
		platform_device_unregister(&l2cache_device);
		printk("Platform driver register is failed!");
		return ret;
	}
	return ret;
}

static void __exit l2cache_driver_exit(void)
{
	platform_driver_unregister(&l2cache_driver);
	platform_device_unregister(&l2cache_device);
}

module_init(l2cache_driver_init);
module_exit(l2cache_driver_exit);

#ifdef MODULE
MODULE_AUTHOR("Hisilicon Drive Group");
MODULE_DESCRIPTION("L2cache driver for the Hisilicon L2cache Controller");
MODULE_LICENSE("GPL");
#endif
