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
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/pm_runtime.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/initval.h>
#include <sound/soc.h>

#include "davinci-pcm.h"
#include "davinci-mcasp.h"





/*
#include <linux/slab.h>

#include <mach/board-am335xevm.h>
#include <plat/omap_device.h>
#include <plat/am33xx.h>
#include <sound/soc.h>

#include <linux/platform_device.h>
#include <plat/omap_hwmod.h>
#include <plat/omap_device.h>
#include <plat/config_pwm.h>
#include <linux/of.h>

#include "davinci-pcm.h"
#include "davinci-mcasp.h"
*/
/*
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/export.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/davinci_emac.h>
#include <linux/cpsw.h>
#include <linux/phy.h>
#include <linux/etherdevice.h>
#include <linux/dma-mapping.h>
#include <linux/can/platform/d_can.h>
#include <linux/platform_data/uio_pruss.h>
#include <linux/pwm/pwm.h>
#include <linux/mfd/ti_tscadc.h>

#include <mach/hardware.h>
#include <mach/irqs.h>
#include <mach/board-am335xevm.h>
#include <asm/mach-types.h>
#include <asm/mach/map.h>
#include <asm/pmu.h>

#ifdef	CONFIG_OMAP3_EDMA
#include <mach/edma.h>
#endif

#include <asm/hardware/asp.h>

#include <plat/tc.h>
#include <plat/board.h>
#include <plat/mcbsp.h>
#include <plat/mmc.h>
#include <plat/dma.h>
#include <plat/omap_hwmod.h>
#include <plat/omap_device.h>
#include <plat/omap4-keypad.h>
#include <plat/am33xx.h>
#include <plat/config_pwm.h>
#include <plat/cpu.h>
#include <plat/gpmc.h>
#include <plat/smartreflex.h>
#include <plat/am33xx.h>
*/






/* LCD controller similar DA8xx */
//#include <video/da8xx-fb.h>
/*
#include "mux.h"
#include "control.h"
#include "devices.h"
#include "omap_opp_data.h"
*/
//#include <linux/init.h>

//#include "devices.h"
#define DAVINCI_PCM_FMTBITS	(\
				SNDRV_PCM_FMTBIT_S8	|\
				SNDRV_PCM_FMTBIT_U8	|\
				SNDRV_PCM_FMTBIT_S16_LE	|\
				SNDRV_PCM_FMTBIT_S16_BE	|\
				SNDRV_PCM_FMTBIT_U16_LE	|\
				SNDRV_PCM_FMTBIT_U16_BE	|\
				SNDRV_PCM_FMTBIT_S24_LE	|\
				SNDRV_PCM_FMTBIT_S24_BE	|\
				SNDRV_PCM_FMTBIT_U24_LE	|\
				SNDRV_PCM_FMTBIT_U24_BE	|\
				SNDRV_PCM_FMTBIT_S32_LE	|\
				SNDRV_PCM_FMTBIT_S32_BE	|\
				SNDRV_PCM_FMTBIT_U32_LE	|\
				SNDRV_PCM_FMTBIT_U32_BE)


static struct snd_pcm_hardware pcm_hardware_playback = {
	.info = (SNDRV_PCM_INFO_INTERLEAVED | SNDRV_PCM_INFO_BLOCK_TRANSFER |
		 SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_MMAP_VALID |
		 SNDRV_PCM_INFO_PAUSE | SNDRV_PCM_INFO_RESUME|
		 SNDRV_PCM_INFO_BATCH),
	.formats = DAVINCI_PCM_FMTBITS,
	.rates = (SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_16000 |
		  SNDRV_PCM_RATE_22050 | SNDRV_PCM_RATE_32000 |
		  SNDRV_PCM_RATE_44100 | SNDRV_PCM_RATE_48000 |
		  SNDRV_PCM_RATE_88200 | SNDRV_PCM_RATE_96000 |
		  SNDRV_PCM_RATE_KNOT),
	.rate_min = 8000,
	.rate_max = 96000,
	.channels_min = 2,
	.channels_max = 384,
	.buffer_bytes_max = 128 * 1024,
	.period_bytes_min = 32,
	.period_bytes_max = 8 * 1024,
	.periods_min = 16,
	.periods_max = 255,
	.fifo_size = 0,
};


static struct snd_pcm_hardware pcm_hardware_capture = {
	.info = (SNDRV_PCM_INFO_INTERLEAVED | SNDRV_PCM_INFO_BLOCK_TRANSFER |
		 SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_MMAP_VALID |
		 SNDRV_PCM_INFO_PAUSE |
		 SNDRV_PCM_INFO_BATCH),
	.formats = DAVINCI_PCM_FMTBITS,
	.rates = (SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_16000 |
		  SNDRV_PCM_RATE_22050 | SNDRV_PCM_RATE_32000 |
		  SNDRV_PCM_RATE_44100 | SNDRV_PCM_RATE_48000 |
		  SNDRV_PCM_RATE_88200 | SNDRV_PCM_RATE_96000 |
		  SNDRV_PCM_RATE_KNOT),
	.rate_min = 8000,
	.rate_max = 96000,
	.channels_min = 2,
	.channels_max = 384,
	.buffer_bytes_max = 128 * 1024,
	.period_bytes_min = 32,
	.period_bytes_max = 8 * 1024,
	.periods_min = 16,
	.periods_max = 255,
	.fifo_size = 0,
};
























//Берём pLatform  Device для Mcasp
extern struct platform_device   *get_mcasp();
extern struct snd_soc_dai       *get_codec_dai();  
extern struct snd_soc_codec     *get_codec();
extern struct snd_pcm_hw_params *get_snd_pcm_hw();



extern struct snd_pcm *get_pcm();
extern struct snd_pcm_substream *start_play_codec();
extern struct snd_pcm_substream *get_stream();






//Init Codec API
extern int aic3x_set_dai_fmt   (struct snd_soc_dai *codec_dai,unsigned int fmt);
extern int aic3x_set_dai_sysclk(struct snd_soc_dai *codec_dai,int clk_id, unsigned int freq, int dir);
extern int aic3x_hw_params     (struct snd_soc_codec *codec);
extern int aic3x_mute		   (struct snd_soc_dai *dai,int mute);

//Init Stream API
extern int davinci_pcm_open        (struct snd_pcm_substream *stream);
extern int snd_pcm_attach_substream(struct snd_pcm *pcm, int stream,struct file *file,struct snd_pcm_substream **rsubstream);
extern void snd_pcm_detach_substream(struct snd_pcm_substream *stream); 

/*****************************************************************************/
/*Local INCLUDES							     */
/*****************************************************************************/
#include "include/Ab_arm_slicdrv.h"

//добавим edma.h


/*
struct snd_dma_buffer *buf;
struct davinci_runtime_data *data;
struct evm_snd_platform_data *evm_data;
struct davinci_audio_dev  *dev;
struct snd_soc_dai *cpu_dai;
*/



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
	struct platform_device *pdev;
	struct device *mcasp;
	struct snd_soc_dai 	     *cpu_dai;              //mcasp Sitara
	struct snd_soc_dai 	     *codec_dai;            //hardware_codec 
	struct snd_soc_codec     *codec;                //
	struct snd_pcm_hw_params *params;
	struct snd_pcm_substream *stream;
	struct snd_pcm *pcm;
	
	struct snd_pcm_hardware *ppcm;
	
	struct file *file=0;
	struct snd_pcm_substream *rsubstream=0;
	memset(&file,0x0000,sizeof(file));
	//Для  кодека.TLV 
	
	/*
	struct davinci_audio_dev *dev;
	struct resource *mem, *ioarea, *res;
	struct snd_platform_data *pdata;
	int ret = 0;
	*/
	
	//Работает у нас получаеться взять platform device mcasp
	/*
	pdev=get_mcasp();	
	printk("OK__pdev=%s\n\r",pdev->name);
	*/
	
	//У нас работает интерфейс mcasp1 можем получить device mcasp;
    //const char *oh_name="mcasp1";
	/*
	struct device *mcasp;
	mcasp=omap_device_get_by_hwmod_name(oh_name);
	*/
	
	 /*
	 pcm  = get_pcm();
	 printk("pcm_name='%s'\n\r",pcm->name);
	 */
	 
	 
	 //Работает УРА наш кодек
//#if 0
	 codec_dai=get_codec_dai();
     codec=get_codec();
//	 params=get_snd_pcm_hw();
	 
	 //printk("CODEC->Name='%s'\n\r",codec_dai->name);
	 //Вызываем  кодек.
	 aic3x_set_dai_fmt(codec_dai,0x1305);                 //Идёт первая функция 
	 aic3x_set_dai_sysclk(codec_dai,0x0,0x16e3600,0x1);   //Идёт вторая функция
	 aic3x_hw_params (codec);                             //Идёт Третья функция
	 aic3x_mute      (codec_dai,0x00);                    //Четвёртая функция  
//#endif	
	
	 
	 
	 
	 //snd_pcm_attach_substream(pcm, 0x0,file,&rsubstream);
	 //davinci_pcm_open(rsubstream);
	// snd_pcm_detach_substream(rsubstream);
	 
	 
	 
	//////////////////////////Пробуем запустить Звук Получить функцию ///////////////
	//stream=get_stream(); 
	
	/*
	stream=start_play_codec();
	davinci_pcm_open(stream);
	stream->stream =0;
	ppcm = (stream->stream == SNDRV_PCM_STREAM_PLAYBACK) ? &pcm_hardware_playback : &pcm_hardware_capture;
	snd_soc_set_runtime_hwparams(&stream, ppcm);
	*/
	
	
	 
	//printk("Stream ='%s'\n\r",rsubstream->name);
	//printk("<===OK===>\n\r");
	
	
	
	
#if 0	
	
////////////////////////////////////////////////////////////////////////////////
	
	
	dev = kzalloc(sizeof(struct davinci_audio_dev), GFP_KERNEL);
    if (!dev)
    {
    	printk("?Fail DEV kzallocъ?\n\r");
    }
		
////////////////////////////////////////////////////////////////////////////////	
    mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!mem) 
    {
      printk("?NO_RESURCE?\n\r");	
    }
//////////////////////////////////////////////////////////////////////////////////	
    ioarea = request_mem_region(mem->start,resource_size(mem), pdev->name);
    if (!ioarea) 
    {
    		
    	printk("?NO_RESURCE_IO_AREA?\n\r");	
    	    //dev_err(&pdev->dev, "Audio region already claimed\n");
    		//ret = -EBUSY;
    		//goto err_release_data;
    }
/////////////////////////////////////////////////////////////////////////////////////
    pdata = pdev->dev.platform_data;
    pm_runtime_enable(&pdev->dev);
 
/////////////////////////////////////////////////////////////////////////////////////
    ret = pm_runtime_get_sync(&pdev->dev);
    if (ret < 0)
    {
    	
    	printk("?NO_PM_RUNTIME_GET_SYBC?\n\r");
    	//dev_err(&pdev->dev, "failed to get runtime pm\n");
    	//goto err_release_region;
    }
/////////////////////////////////////////////////////////////////////////////////////
    dev->base = ioremap(mem->start, resource_size(mem));
    if (!dev->base)
    {
    		
   
    	
    	    //dev_err(&pdev->dev, "ioremap failed\n");
    		//ret = -ENOMEM;
    		//goto err_release_clk;
    }
#endif
    
    
	//omap_hwmod_lookup(oh_name);
  //  #endif
	
	//p_devices=platform_device_alloc(oh_name,0);
	//printk("0x%x=p_devices,%s \n\r",p_devices);
	/*
	 struct platform_device *platform_device_alloc(const char *name, int id)
	*/
	
	
	//pdev=mcasp->platform_data;
	/*
	
	dev = kzalloc(sizeof(struct davinci_audio_dev), GFP_KERNEL);
	if (!dev)
	{		
		return	-ENOMEM;
	}
	
	printk("rrrrrrr\n\r");
	*/
	
	/*
	mem = platform_get_resource(p_devices, IORESOURCE_MEM, 0);
	if (!mem) {
	dev_err(&p_devices->dev, "no mem resource?\n");
	ret = -ENODEV;
	//goto err_release_data;
	 }
	*/

	//dev = snd_soc_dai_get_drvdata(cpu_dai);
	//struct davinci_audio_dev *dev = snd_soc_dai_get_drvdata(dai);
	//printk("")
   // printk("+Init_Arm_McASP_interface()=0x%x+\n\r",mcasp);		
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





















