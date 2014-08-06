/**********************************************************************************************************************
*                                        (c) COPYRIGHT by LONIIS.                                             *
*                                            All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : mpcdrvmain.c
*
* Description : Sitara 3352 linux driver kernel module init 
*
* Author      : Konstantin Shiluaev..
*
******************************************************************************
******************************************************************************
*
* Module's Description Record:..
* ============================
*
* $ProjectRevision: 0.0.1 $
* $Source: 
* $State: Debug$
* $Revision: 0.0.1 $ $Name: $
* $Date: 2012/09/21 10:40:51 $recieve_matrica_commutacii_packet.state=true;
* $Locker: $
*
* Module's revision history:
* ==========================
* --------- $Log: mpcdrvmain.c $
* --------- Initial revision
******************************************************************************/


/*-----------------------------------------------------------------------------
GENERAL NOTES
-----------------------------------------------------------------------------*/

/*****************************************************************************/
/*	COMMON CONFIGURATION						     */
/*****************************************************************************/
/*
 * mdelay(unsigned long millisecond)
 * udelay(unsigned long microsecond)
 * ndelay(unsigned long nanosecond);
 * 
 */
////

/*****************************************************************************/
/*	INCLUDES							     */
/*****************************************************************************/
#include <linux/module.h> // Needed by all modules
#include <linux/kernel.h> // Needed for KERN_ALERT
#include <linux/init.h>   // Needed for the macros
#include <linux/delay.h>  // mdelay ,msleep,
#include <linux/timer.h>  // timer 
#include <linux/ktime.h>  // hardware timer (ktime)



#include "include/Ab_arm_maindrv.h"











/**************************************************************************************************
Syntax:      	    int init_module(void)
Parameters:     	none
Remarks:			load the kernel module driver - invoked by insmod. 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
int mpc_init_module(void)
{

	/*
	** We use the miscfs to register our device.
	*/
    printk("init_module_I-tdm() called\n"); 
       
return 0;
}

/**************************************************************************************************
Syntax:      	    void cleanup_module(void)
Parameters:     	none
Remarks:			unload the kernel module driver - invoked by rmmod

Return Value:	    none

***************************************************************************************************/
void mpc_cleanup_module(void)
{	
    printk("exit_module() I-TDM called\n");
}

/*****************************************************************************/
/*	PUBLIC FUNCTION DEFINITIONS					     */
/*****************************************************************************/


/*****************************************************************************/
/*	MODULE DESCRIPTION				     */
/*****************************************************************************/
module_init(mpc_init_module);
module_exit(mpc_cleanup_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Konstantin Shiluaev <k_sjiluaev@supertel.spb.su>");
MODULE_DESCRIPTION("Test ARM SITARA 3352 I-TDM driver");






















