/*
<:copyright-BRCM:2002:GPL/GPL:standard

   Copyright (c) 2002 Broadcom Corporation
   All Rights Reserved

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation (the "GPL").

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.


A copy of the GPL is available at http://www.broadcom.com/licenses/GPLv2.php, or by
writing to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.

:>
*/
/***********************************************/
/*                                                                           */
/*   MODULE:  bcm_ext_timer.h                                   */
/*   PURPOSE: Timer specific information.                     */
/*                                                                           */
/***********************************************/
#ifndef _BCM_EXT_TIMER_H
#define _BCM_EXT_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif


/*Defines the numbers of the existing HW timres*/
typedef enum
{
    EXT_TIMER_INVALID = -1,
    EXT_TIMER_0       = 0,
    EXT_TIMER_1       = 1,
    EXT_TIMER_2       = 2,
    EXT_TIMER_NUM
}EXT_TIMER_NUMBER;


/*****************************************************************************/
/*          Function Prototypes                                              */
/*****************************************************************************/
typedef void (*ExtTimerHandler)(int param);

int init_hw_timers(void);
int ext_timer_alloc(EXT_TIMER_NUMBER timer_num, unsigned int timer_period, ExtTimerHandler timer_callback, unsigned int param );
int ext_timer_free(EXT_TIMER_NUMBER timer_num);
int ext_timer_stop(EXT_TIMER_NUMBER timer_num);
int ext_timer_start(EXT_TIMER_NUMBER timer_num);
int  ext_timer_read_count(EXT_TIMER_NUMBER timer_num, unsigned int* count);


#ifdef __cplusplus
}
#endif

#endif /* _BCM_EXT_TIMER_H */

