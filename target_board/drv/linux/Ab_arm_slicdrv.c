/**********************************************************************************************************************
*                                        (c) COPYRIGHT by LONIIS.                                             *
*                                            All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : Ab_arm_slicdrv.c
*
* Description : Sitara 3352 linux driver kernel module for SLIC <-> McASP Data recieve and transmit (for AMC3352_board)    
*               Sitara 3352 linux driver kernel module for Audio Output <-> McASP Data recieve and transmit (for Starter_KIT) 
                  

* Author      : Konstantin Shiluaev.
*
******************************************************************************
-----------------------------------------------------------------------------
GENERAL NOTES:
1.This Module functions control SLIC for SPI bus
2.Recieve and Transmit PCM data(in or from SLIC device)
3.Test function for Transmit Recieve Module  Fuction 
MCASP_VERSION_3->>>for Sitara Processor AM335x
MACH-OMAP2     ->>>for Sitara Processor AM335x 

-----------------------------------------------------------------------------*/

/*****************************************************************************/
/*	Linux Kernel Include for  MCasp header					*/
/*****************************************************************************/
///home/kosta/Sitara_ltib/sitara-board-port-linux/arch/arm/include/asm/hardware/asp.h
///home/kosta/Sitara_ltib/sitara-board-port-linux/arch/arm/mach-omap2/include/mach
//#include "asm/hardware/asp.h"
///home/kosta/Sitara_ltib/sitara-board-port-linux/arch/arm/mach-omap2

//Стартовая Инициализация Платы находиться в файле board-am335xevm.c



/*****************************************************************************/
/*Local INCLUDES							     */
/*****************************************************************************/
#include "include/Ab_arm_slicdrv.h"

//добавим edma.h






/*****************************************************************************/
/*	PUBLIC FUNCTION DEFINITIONS					     */
/*****************************************************************************/

/**************************************************************************************************
Syntax:      	    bool Init_Arm_McASP_interface();
Parameters:     	none
Remarks:			bool Init_Arm_McASP_interface();
Return Value:	    1  =>  Success  ,0 => Failure
***************************************************************************************************/
bool Init_Arm_McASP_interface()
{
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	printk("+Init_Arm_McASP_interface()+\n\r");		
	
	
return 1;	
}




/**************************************************************************************************
Syntax:      	    bool Init_Arm_AIC3106_low_level_codec_i2c()
Parameters:     	none
Remarks:			bool Init_Arm_McASP_interface();
Return Value:	    1  =>  Success  ,0 => Failure
***************************************************************************************************/
bool Init_Arm_AIC3106_low_level_codec_i2c()
{
	
  printk("+bool Init_Arm_AIC3106_low_level_codec_i2c()+\n\r");		
	
	
return 1;	
}





















