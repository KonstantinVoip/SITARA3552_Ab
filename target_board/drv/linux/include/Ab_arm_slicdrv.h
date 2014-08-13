/**********************************************************************************************************************
*                                        (c) COPYRIGHT by LONIIS.                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : mpcdrvmain.h
*
* Description : definitions and declarations for mpc Ab_ARM_I-TDM driver kernel module 
*
* Author      : Konstantin Shiluaev
*
******************************************************************************
******************************************************************************
*
* Module's Description Record:
* ============================
*
* $ProjectRevision: 0.0.1 $
* $Source: 
* $State: Debug$
* $Revision: 0.0.1 $ $Name: $
* $Date: 2012/09/21 10:40:51 $
* $Locker: $
*
* Module's revision history:
* ==========================
* --------- $Log: mpcdrv.h $
* --------- Initial revision
******************************************************************************/

/*-----------------------------------------------------------------------------
 GENERAL NOTES
 ///////////////////////////////////////////////////////////////////////////////////////////////
 
 static u8 am335x_evm_sk_iis_serializer_direction1[] = {
	INACTIVE_MODE,	INACTIVE_MODE,	TX_MODE,	INACTIVE_MODE,
	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
};
 
 DESCRIPTION  of MCASP1
 static struct snd_platform_data am335x_evm_sk_snd_data1 = {
	.tx_dma_offset	= 0x46400000,	// McASP1 
	//.rx_dma_offset	= 0x46400000,
	.op_mode	= DAVINCI_MCASP_IIS_MODE,
	.num_serializer	= ARRAY_SIZE(am335x_evm_sk_iis_serializer_direction1),
	.tdm_slots	= 2,
	.serial_dir	= am335x_evm_sk_iis_serializer_direction1,
	.asp_chan_q	= EVENTQ_2,
	.version	= MCASP_VERSION_3,
	.txnumevt	= 32,
	.get_context_loss_count	=
			omap_pm_get_dev_context_loss_count,
};

//////////////////////////////////////////////////////////// 'mcasp' class////////////////////////////////////////// 

static struct omap_hwmod_class_sysconfig am33xx_mcasp_sysc = {
	.rev_offs	= 0x0,
	.sysc_offs	= 0x4,
	.sysc_flags	= SYSC_HAS_SIDLEMODE,
	.idlemodes	= (SIDLE_FORCE | SIDLE_NO | SIDLE_SMART),
	.sysc_fields	= &omap_hwmod_sysc_type3,
};

static struct omap_hwmod_class am33xx_mcasp_hwmod_class = {
	.name	= "mcasp",
	.sysc	= &am33xx_mcasp_sysc,
};

// mcasp0 
static struct omap_hwmod_irq_info am33xx_mcasp0_irqs[] = {
	{ .name = "ax", .irq = 80, },
	{ .name = "ar", .irq = 81, },
	{ .irq = -1 }
};

static struct omap_hwmod_dma_info am33xx_mcasp0_edma_reqs[] = {
	{ .name = "tx", .dma_req = AM33XX_DMA_MCASP0_X, },
	{ .name = "rx", .dma_req = AM33XX_DMA_MCASP0_R, },
	{ .dma_req = -1 }
};

static struct omap_hwmod_addr_space am33xx_mcasp0_addr_space[] = {
	{
		.pa_start	= 0x48038000,
		.pa_end		= 0x48038000 + (SZ_1K * 12) - 1,
		.flags		= ADDR_TYPE_RT
	},
	{ }
};

static struct omap_hwmod_ocp_if am33xx_l3_slow__mcasp0 = {
	.master		= &am33xx_l3slow_hwmod,
	.slave		= &am33xx_mcasp0_hwmod,
	.clk		= "mcasp0_ick",
	.addr		= am33xx_mcasp0_addr_space,
	.user		= OCP_USER_MPU,
};

static struct omap_hwmod_ocp_if *am33xx_mcasp0_slaves[] = {
	&am33xx_l3_slow__mcasp0,
};

static struct omap_hwmod am33xx_mcasp0_hwmod = {
	.name		= "mcasp0",
	.class		= &am33xx_mcasp_hwmod_class,
	.clkdm_name	= "l3s_clkdm",
	.mpu_irqs	= am33xx_mcasp0_irqs,
	.sdma_reqs	= am33xx_mcasp0_edma_reqs,
	.main_clk	= "mcasp0_fck",
	.prcm		= {
		.omap4	= {
			.clkctrl_offs	= AM33XX_CM_PER_MCASP0_CLKCTRL_OFFSET,
			.modulemode	= MODULEMODE_SWCTRL,
		},
	},
	.slaves		= am33xx_mcasp0_slaves,
	.slaves_cnt	= ARRAY_SIZE(am33xx_mcasp0_slaves),
};

// mcasp1 
static struct omap_hwmod_irq_info am33xx_mcasp1_irqs[] = {
	{ .name = "ax", .irq = 82, },
	{ .name = "ar", .irq = 83, },
	{ .irq = -1 }
};

static struct omap_hwmod_dma_info am33xx_mcasp1_edma_reqs[] = {
	{ .name = "tx", .dma_req = AM33XX_DMA_MCASP1_X, },
	{ .name = "rx", .dma_req = AM33XX_DMA_MCASP1_R, },
	{ .dma_req = -1 }
};


static struct omap_hwmod_addr_space am33xx_mcasp1_addr_space[] = {
	{
		.pa_start	= 0x4803C000,
		.pa_end		= 0x4803C000 + (SZ_1K * 12) - 1,
		.flags		= ADDR_TYPE_RT
	},
	{ }
};

static struct omap_hwmod_ocp_if am33xx_l3_slow__mcasp1 = {
	.master		= &am33xx_l3slow_hwmod,
	.slave		= &am33xx_mcasp1_hwmod,
	.clk		= "mcasp1_ick",
	.addr		= am33xx_mcasp1_addr_space,
	.user		= OCP_USER_MPU,
};

static struct omap_hwmod_ocp_if *am33xx_mcasp1_slaves[] = {
	&am33xx_l3_slow__mcasp1,
};

static struct omap_hwmod am33xx_mcasp1_hwmod = {
	.name		= "mcasp1",
	.class		= &am33xx_mcasp_hwmod_class,
	.clkdm_name	= "l3s_clkdm",
	.mpu_irqs	= am33xx_mcasp1_irqs,
	.sdma_reqs	= am33xx_mcasp1_edma_reqs,
	.main_clk	= "mcasp1_fck",
	.prcm		= {
		.omap4	= {
			.clkctrl_offs	= AM33XX_CM_PER_MCASP1_CLKCTRL_OFFSET,
			.modulemode	= MODULEMODE_SWCTRL,
		},
	},
	.slaves		= am33xx_mcasp1_slaves,
	.slaves_cnt	= ARRAY_SIZE(am33xx_mcasp1_slaves),
};
-----------------------------------------------------------------------------*/
#ifndef ABARMSLICDRV_H
#define ABARMSLICDRV_H

#include <linux/kernel.h> // Needed for KERN_ALERT

/*Hardware Plata */

#define AM335x_STARTER_KIT
#define AM335x_AMC




bool Init_Arm_McASP_interface();
bool Init_Arm_AIC3106_low_level_codec_i2c();



/*for AMC  */
//bool Init_Arm_SI32260_SLIC_spi();










/*****************************************************************************/
/*	COMMON CONFIGURATION						     */
/*****************************************************************************/


/*****************************************************************************/
/*	INCLUDES							     */


/*****************************************************************************/
/*	PUBLIC MACROS							     */
/*****************************************************************************/




/*****************************************************************************/
/*	PUBLIC DATA TYPES						     */
/*****************************************************************************/


/*****************************************************************************/
/*	PUBLIC FUNCTION PROTOTYPES					     */
/*****************************************************************************/


/*****************************************************************************/
/*	PUBLIC GLOBALS							     */
/*****************************************************************************/
#endif /* MPCDRVMAIN_H */

