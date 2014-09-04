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
#include <sound/soc-dapm.h>

#include "davinci-pcm.h"
#include "davinci-mcasp.h"


#include "include/Ab_arm_TestBuf.h"


/*****************************************************************************/
/*Local INCLUDES							     */
/*****************************************************************************/
#include "include/Ab_arm_slicdrv.h"


//Берём pLatform  Device для Mcasp
extern struct platform_device   *get_mcasp();
extern struct snd_soc_dai       *get_codec_dai();  
extern struct snd_soc_dai       *get_cpu_dai();
extern struct davinci_audio_dev *get_davinci_audio_dev();


extern struct snd_soc_codec     *get_codec();
extern struct snd_pcm_hw_params *get_snd_pcm_hw();



extern struct snd_pcm *get_pcm();
extern struct snd_pcm_substream *start_play_codec();
extern struct snd_pcm_substream *get_stream();


extern int soc_pcm_trigger(struct snd_pcm_substream *substream,int cmd);



//Init Codec API
extern int aic3x_set_dai_fmt   (struct snd_soc_dai *codec_dai,unsigned int fmt);
extern int aic3x_set_dai_sysclk(struct snd_soc_dai *codec_dai,int clk_id, unsigned int freq, int dir);
extern int aic3x_hw_params     (struct snd_soc_codec *codec);
extern int aic3x_mute		   (struct snd_soc_dai *dai,int mute);




//Init McASP API
extern int  davinci_mcasp_startup(struct snd_pcm_substream *substream,struct snd_soc_dai *dai);
extern int  davinci_mcasp_set_dai_fmt(struct snd_soc_dai *cpu_dai,unsigned int fmt);
extern int  davinci_mcasp_hw_params(struct snd_pcm_substream *substream,struct snd_pcm_hw_params *params,struct snd_soc_dai *cpu_dai);
extern int  davinci_mcasp_trigger(struct snd_pcm_substream *substream,int cmd, struct snd_soc_dai *cpu_dai);


//Init Stream API
extern int  davinci_pcm_open        (struct snd_pcm_substream *stream);
extern int  snd_pcm_attach_substream(struct snd_pcm *pcm, int stream,struct file *file,struct snd_pcm_substream **rsubstream);
extern void snd_pcm_detach_substream(struct snd_pcm_substream *stream); 
extern int  snd_pcm_prepare(struct snd_pcm_substream *substream,struct file *file);


extern int  snd_pcm_open_substream(struct snd_pcm *pcm, int stream,struct file *file,struct snd_pcm_substream **rsubstream);
extern void snd_pcm_post_prepare(struct snd_pcm_substream *substream, int state);
extern int  snd_pcm_do_prepare(struct snd_pcm_substream *substream, int state);


extern int snd_pcm_pre_start(struct snd_pcm_substream *substream, int state);
extern int snd_pcm_do_start(struct snd_pcm_substream *substream, int state);

extern int davinci_pcm_prepare(struct snd_pcm_substream *substream);
extern int davinci_pcm_trigger(struct snd_pcm_substream *substream, int cmd);

extern int snd_soc_dapm_stream_event(struct snd_soc_pcm_runtime *rtd,const char *stream, int event);


//extern void  snd_soc_dapm_stream_event(rtd,codec_dai->driver->playback.stream_name,SND_SOC_DAPM_STREAM_START);
struct snd_dma_buffer    *dmab_slic=NULL;
struct snd_soc_dai 	     *cpu_dai;              //mcasp Sitara interface
struct snd_soc_dai 	     *codec_dai;            //hardware_codec TLV_aic interface
struct snd_soc_codec     *codec;                //codec interface
struct snd_pcm 			 *pcm;                  //pcm codec structure





/**************************************************************************************************
Syntax:      	    bool Init_Arm_McASP_interface();
Parameters:     	none
Remarks:			bool Init_Arm_McASP_interface();
Return Value:	    1  =>  Success  ,0 => Failure
***************************************************************************************************/
bool Init_Arm_McASP_interface()
{
	
	struct snd_pcm_hw_params *params=0;
	struct snd_pcm_substream *rsubstream=0;
	
	int i;
	size_t i_size=0xfa00;  //64000
	
	struct snd_pcm_str * pstr;
	struct snd_pcm_runtime *runtime;
	size_t size;
	
	struct snd_pcm_ops  *hardware_ops;
	struct snd_soc_pcm_runtime *rtd; 
	struct snd_soc_platform *platform;
	
	memset(&rsubstream,0x0000,sizeof(rsubstream));
	memset(&params,0x0000,sizeof(params));
	
	int ret = 0;
 
	//Работает УРА наш кодек
	pcm  = get_pcm();
	codec_dai=get_codec_dai();
	cpu_dai=get_cpu_dai();
	codec=get_codec();
	

	davinci_mcasp_startup(rsubstream,cpu_dai);           //
	davinci_mcasp_set_dai_fmt(cpu_dai,0x1305);           //
	davinci_mcasp_hw_params(rsubstream,params,cpu_dai);  //
	 
	 
	
}




/**************************************************************************************************
Syntax:      	    bool Init_Arm_AIC3106_low_level_codec_i2c()
Parameters:     	none
Remarks:			bool Init_Arm_McASP_interface();
Return Value:	    1  =>  Success  ,0 => Failure
***************************************************************************************************/
bool Init_Arm_AIC3106_low_level_codec_i2c()
{
		
	//printk("CODEC->Name='%s'\n\r",codec_dai->name);
	 //Setup наш аппаратный кодек  кодек  TLV.
	aic3x_set_dai_fmt(codec_dai,0x1305);                 //Идёт первая функция 
	aic3x_set_dai_sysclk(codec_dai,0x0,0x16e3600,0x1);   //Идёт вторая функция
	aic3x_hw_params (codec);                             //Идёт Третья функция
	aic3x_mute      (codec_dai,0x00);                    //Четвёртая функция  

	//printk("+bool Init_Arm_AIC3106_low_level_codec_i2c()+\n\r");		

return 1;	
}





















