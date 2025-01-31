/*
 Copyright 2004-2010 Broadcom Corp. All Rights Reserved.

 <:label-BRCM:2011:DUAL/GPL:standard    
 
 Unless you and Broadcom execute a separate written software license
 agreement governing use of this software, this software is licensed
 to you under the terms of the GNU General Public License version 2
 (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
 with the following added to such license:
 
    As a special exception, the copyright holders of this software give
    you permission to link this software with independent modules, and
    to copy and distribute the resulting executable under terms of your
    choice, provided that you also meet, for each linked independent
    module, the terms and conditions of the license of that module.
    An independent module is a module which is not derived from this
    software.  The special exception does not apply to any modifications
    of the software.
 
 Not withstanding the above, under no circumstances may you combine
 this software in any way with any other Broadcom software provided
 under a license other than the GPL, without Broadcom's express prior
 written consent.
 
 :>
*/
#ifndef _ETHSW_DMA_H_
#define _ETHSW_DMA_H_

/****************************************************************************
    Prototypes
****************************************************************************/

int ethsw_phy_pll_up(int ephy_and_gphy);
uint32 ethsw_ephy_auto_power_down_wakeup(void);
uint32 ethsw_ephy_auto_power_down_sleep(void);
int ethsw_reset_ports(struct net_device *dev);
int ethsw_enable_sar_port(void);
int ethsw_disable_sar_port(void);
int ethsw_save_port_state(void);
int ethsw_restore_port_state(void);
int ethsw_port_to_phyid(int port);
void ethsw_port_based_vlan(int port_map, int wan_port_bit, int txSoftSwitchingMap);

void ethsw_configure_ports(int port_map, int *pphy_id);

#if defined(CONFIG_BCM96828) && !defined(CONFIG_EPON_HGU)
void saveEthPortToRxIudmaConfig(uint8 port, uint8 iudma);
int restoreEthPortToRxIudmaConfig(uint8 port);
int enet_learning_ctrl(uint32_t portMask, uint8_t enable);
int bcm_fun_enet_drv_handler(void *ptr);
int epon_uni_to_uni_ctrl(unsigned int portMap, int val);
#endif

void bcmeapi_ethsw_init_config(void);

#endif /* _ETHSW_LEGACY_H_ */
