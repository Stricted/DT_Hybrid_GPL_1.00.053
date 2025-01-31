/* 
* <:copyright-BRCM:2011:GPL/GPL:standard
* 
*    Copyright (c) 2011 Broadcom Corporation
*    All Rights Reserved
* 
* Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed
* to you under the terms of the GNU General Public License version 2
* (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
* with the following added to such license:
* 
*    As a special exception, the copyright holders of this software give
*    you permission to link this software with independent modules, and
*    to copy and distribute the resulting executable under terms of your
*    choice, provided that you also meet, for each linked independent
*    module, the terms and conditions of the license of that module.
*    An independent module is a module which is not derived from this
*    software.  The special exception does not apply to any modifications
*    of the software.
* 
* Not withstanding the above, under no circumstances may you combine
* this software in any way with any other Broadcom software provided
* under a license other than the GPL, without Broadcom's express prior
* written consent.
* 
* :>
*/
#include <linux/init.h>
#include <linux/types.h>
#include <linux/pci.h>

#include <bcm_intr.h>
#include <bcm_map_part.h>
#include <bcmpci.h>

static char irq_tab_bcm63xx[] __initdata = {
#if (defined(CONFIG_BCM96368) || defined(CONFIG_BCM96816))
    [0] = INTERRUPT_ID_MPI,
    [1] = INTERRUPT_ID_MPI,
    [2] = INTERRUPT_ID_MPI,
#endif
#if defined(CONFIG_BCM96362) || defined(CONFIG_BCM963268)
    [WLAN_ONCHIP_DEV_SLOT] = INTERRUPT_ID_WLAN,
#endif
#if defined(CONFIG_USB)
    [USB_HOST_SLOT] = INTERRUPT_ID_USBH,
    [USB20_HOST_SLOT] = INTERRUPT_ID_USBH20
#endif
};


static void bcm63xx_fixup_header(struct pci_dev *dev)
{
    uint32 memaddr;
    uint32 size;

    memaddr = pci_resource_start(dev, 0);
    size = pci_resource_len(dev, 0);

    if (dev->bus->number == BCM_BUS_PCI) {
        switch (PCI_SLOT(dev->devfn)) {
#if defined(CONFIG_USB)
            case USB_HOST_SLOT:
                dev->resource[0].flags |= IORESOURCE_PCI_FIXED; // prevent linux from reallocating resources
                break;
    
            case USB20_HOST_SLOT:
               dev->resource[0].flags |= IORESOURCE_PCI_FIXED; // prevent linux from reallocating resources
               break;
#endif
        }
    }
}
DECLARE_PCI_FIXUP_HEADER(PCI_ANY_ID, PCI_ANY_ID, bcm63xx_fixup_header);


static void bcm63xx_fixup_final(struct pci_dev *dev)
{
    uint32 memaddr;
    uint32 size;
#if defined(CONFIG_BCM96368)
    uint32 resno;
#endif

    memaddr = pci_resource_start(dev, 0);
    size = pci_resource_len(dev, 0);

    if (dev->bus->number == BCM_BUS_PCI) {
        switch (PCI_SLOT(dev->devfn)) {
#if defined(CONFIG_BCM96368)
            case 0:   
                // Move device in slot 0 to a different memory range
                // In case this is a CB device, it will be accessed via l2pmremap1
                // which will have CARDBUS_MEM bit set
                for (resno = 0; resno < 6; resno++) {
                    if (dev->resource[resno].end && (dev->resource[resno].start < BCM_CB_MEM_BASE)) {
                        dev->resource[resno].start += (BCM_CB_MEM_BASE - BCM_PCI_MEM_BASE);
                        dev->resource[resno].end += (BCM_CB_MEM_BASE - BCM_PCI_MEM_BASE);
                        dev->resource[resno].flags |= IORESOURCE_PCI_FIXED; // prevent linux from reallocating resources
                    }
                }   
                break;
#endif                
         }
    } 

    if (dev->bus->number == BCM_BUS_PCIE_DEVICE) {
        switch (PCI_SLOT(dev->devfn)) {
#if defined(CONFIG_BCM96362) || defined(CONFIG_BCM963268)
            case WLAN_ONCHIP_DEV_SLOT:
                 if(((dev->device<<16)|dev->vendor) == WLAN_ONCHIP_PCI_ID) {
                      dev->resource[0].end   = WLAN_CHIPC_BASE+ WLAN_ONCHIP_RESOURCE_SIZE -1 ;
                      dev->resource[0].start = WLAN_CHIPC_BASE;
                 }
#endif           
		break;
        }
    }
}
DECLARE_PCI_FIXUP_FINAL(PCI_ANY_ID, PCI_ANY_ID, bcm63xx_fixup_final);


#if defined(CONFIG_BCM96816)
static char irq_tab_pcie_bcm63xx_pcie_bus[] __initdata = {	
    /* SWITCH_8232104c DS Ports */
    [BCM_BUS_PCIE_DEVICE+2] = 0, /*INTA*/
    [BCM_BUS_PCIE_DEVICE+3] = 1, /*INTB*/
    [BCM_BUS_PCIE_DEVICE+4] = 2  /*INTC*/
};
#endif /* defined(CONFIG_BCM96816) */

int pcibios_plat_dev_init(struct pci_dev *dev)
{
    return 0;
}

int __init pcibios_map_irq(struct pci_dev *dev, u8 slot, u8 pin)
{
#if defined(CONFIG_BCM96816)
	if (dev->bus->number >= BCM_BUS_PCIE_DEVICE) {
		return INTERRUPT_ID_PCIE_A+(irq_tab_pcie_bcm63xx_pcie_bus[dev->bus->number]+slot)%4;
    }
#endif
#if defined(CONFIG_BCM96362) || defined(CONFIG_BCM96328) || defined(CONFIG_BCM963268) || defined(CONFIG_BCM96828) || defined(UBUS2_PCIE)
	if ((dev->bus->number >= BCM_BUS_PCIE_DEVICE) &&
		 ((dev->class >> 8) != PCI_CLASS_BRIDGE_PCI)) {
        return INTERRUPT_ID_PCIE_RC;
  }    
#endif
    return irq_tab_bcm63xx[slot];
}

