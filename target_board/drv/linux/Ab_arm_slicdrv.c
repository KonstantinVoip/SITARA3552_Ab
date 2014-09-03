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
#include <sound/soc-dapm.h>

#include "davinci-pcm.h"
#include "davinci-mcasp.h"


#include "include/Ab_arm_TestBuf.h"


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

#define DMA_PRINTK(ARGS...)  printk(KERN_INFO "<%s>: ",__FUNCTION__);printk(ARGS)
#define DMA_FN_IN printk(KERN_INFO "[%s]: start\n", __FUNCTION__)
#define DMA_FN_OUT printk(KERN_INFO "[%s]: end\n",__FUNCTION__)









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
extern int davinci_pcm_open        (struct snd_pcm_substream *stream);
extern int snd_pcm_attach_substream(struct snd_pcm *pcm, int stream,struct file *file,struct snd_pcm_substream **rsubstream);
extern void snd_pcm_detach_substream(struct snd_pcm_substream *stream); 
extern int snd_pcm_prepare(struct snd_pcm_substream *substream,struct file *file);


extern int snd_pcm_open_substream(struct snd_pcm *pcm, int stream,struct file *file,struct snd_pcm_substream **rsubstream);
extern void snd_pcm_post_prepare(struct snd_pcm_substream *substream, int state);
extern int snd_pcm_do_prepare(struct snd_pcm_substream *substream, int state);


extern int snd_pcm_pre_start(struct snd_pcm_substream *substream, int state);
extern int snd_pcm_do_start(struct snd_pcm_substream *substream, int state);

extern int davinci_pcm_prepare(struct snd_pcm_substream *substream);
extern int davinci_pcm_trigger(struct snd_pcm_substream *substream, int cmd);

extern int snd_soc_dapm_stream_event(struct snd_soc_pcm_runtime *rtd,const char *stream, int event);


//extern void  snd_soc_dapm_stream_event(rtd,codec_dai->driver->playback.stream_name,SND_SOC_DAPM_STREAM_START);
struct snd_dma_buffer *dmab_slic=NULL;



/*****************************************************************************/
/*Local INCLUDES							     */
/*****************************************************************************/
#include "include/Ab_arm_slicdrv.h"




/*****************************************************************************/
/*	PUBLIC FUNCTION DEFINITIONS					     */
/*****************************************************************************/
static void enqueue_dma()
{
	dma_addr_t dma_pos;
	dma_addr_t src, dst;
	static unsigned int mcasp_count;
	unsigned int period_size;
	unsigned int dma_offset;
	unsigned int data_type;
	unsigned short acnt;
	unsigned int fifo_level;
	unsigned short src_bidx, dst_bidx;
	unsigned short src_cidx, dst_cidx;
	unsigned int count; 
	unsigned int dma_src;
	
	dma_src=dmab_slic->addr;
	
    mcasp_count=mcasp_count+1;
	period_size = 0xfa0;                  	  //snd_pcm_lib_period_bytes(substream);
	dma_offset  = mcasp_count*period_size; // prtd->period * period_size;
	dma_pos     = /*0x8ad80000*/dma_src+dma_offset;  // runtime->dma_addr+ dma_offset;
	fifo_level  = 0x20;			  //prtd->params->fifo_level;
                 
	//mcasp_count=mcasp_count+1;
	//printk("runtime->dma_addr=0x%x,prtd->period=0x%x\n\r",prtd->period,runtime->dma_addr);
	//printk("+enqueue_dma_=0x%x,dma_pos=0x%x,dma_src=0x%x,dma_offset=0x%x\n\r",mcasp_count,dma_pos,dma_src,dma_offset);
	
	
	data_type = 0x2;//prtd->params->data_type;
    count = period_size / data_type;


	//printk("DaTA_TYPE=0x%x\n\r");


     if (fifo_level)
     {	 
	   count /= fifo_level;
     } 
     memcpy(0xffd50000,stereo_voice_buffer,0x20000);
     
	 src = dma_pos;
	 dst = 0x46400000;
	 src_bidx = data_type;
	 dst_bidx = 0;
	 src_cidx = data_type * fifo_level;
	 dst_cidx = 0;

	 printk("SRC=0x%x|src_cidx=0x%x|count=0x%x,dma_offset=0x%x|dma_pos=0x%x\n\r",src,src_cidx,count,dma_offset,dma_pos); 
	 
	 acnt = 0x2;
	 edma_set_src(0xbf, src, INCR, W8BIT);
	 edma_set_dest(0xbf, dst, INCR, W8BIT);

	 edma_set_src_index(0xbf, src_bidx, src_cidx);
	 edma_set_dest_index(0xbf, dst_bidx, dst_cidx);
	
	
	 if (!fifo_level)
	 { 
			edma_set_transfer_params(0xbf, acnt, count, 1, 0,ASYNC);
	 } 
			
	 else
	 {		
		    edma_set_transfer_params(0xbf, acnt, fifo_level,count, fifo_level,ABSYNC);
	 }
	 
	
}


/**************************************************************************************************
Syntax:      	    static void callback1(unsigned lch, u16 ch_status, void *data)
Parameters:     	unsigned lch, u16 ch_status, void *data
Remarks:			Start Testing EDMA Callback1 function
***************************************************************************************************/
static void callback1(unsigned lch, u16 ch_status, void *data)
{

	printk("+dma_irq+\n\r");
	
	if (unlikely(ch_status != DMA_COMPLETE))
	{
		printk("ulikley_DMA_no_complete\n\r");
		return;
    }
	enqueue_dma();
	
/*	
	switch(ch_status) 
	{
	case DMA_COMPLETE:
		//irqraised1 = 1;
		//DMA_PRINTK ("\n Callback 1: Channel %d status is: %u\n",lch, ch_status);
		//mdelay(100);
		enqueue_dma();
		break;
	case DMA_CC_ERROR:
		//irqraised1 = -1;
		DMA_PRINTK ("\nFrom Callback 1: DMA_CC_ERROR occured on Channel %d\n", lch);
		break;
	default:
		break;
	}
*/


}

static void dma_request()
{
	struct edmacc_param edma_par;	
	int result=0;
	//Работает кусок pcm/dma-request выделяет нужные каналы.		
	result = edma_alloc_channel (0xa, callback1, NULL, 0x2);
		
	printk("DMA_CHANNEL=result=%d\n\r",result);
		
	// Request a Link Channel 
	result = edma_alloc_slot (EDMA_CTLR(0xa), EDMA_SLOT_ANY);
	printk("SLOT=result=%d\n\r",result);
		
	edma_read_slot(0xbf, &edma_par);
	edma_par.opt |= TCINTEN |EDMA_TCC(EDMA_CHAN_SLOT(0xa));
	edma_par.link_bcntrld = EDMA_CHAN_SLOT(0xbf) << 5;
	edma_write_slot(0xbf, &edma_par);
		
}

static void dma_prepare()
{
	struct edmacc_param edma_par;
	printk("Kosta_DMA_PREPARE\n\r");
	
	enqueue_dma();
	
	edma_par.opt |= TCINTEN |EDMA_TCC(EDMA_CHAN_SLOT(0xa));
    edma_par.link_bcntrld = EDMA_CHAN_SLOT(0xbf) << 5;
	edma_read_slot( 0xbf,&edma_par);
	edma_write_slot(0xa, &edma_par);	

	enqueue_dma();

}


static void dma_trigger()
{
    int ret=0;
      
    ret=edma_start(0xa);
	if (ret != 0) 
	{
	printk ("edma3__dma: davinci_start_dma failed \n");
	}
    else
    {        
    printk("DMA_START=OK =0x%x\n\r",ret);    
    }
	
}



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
	
	
	
	
	struct snd_pcm_hw_params *params=0;
	struct snd_pcm_substream *stream;
	struct snd_pcm *pcm;
	
	struct davinci_audio_dev  *dev;
	struct snd_pcm_hardware *ppcm;
	struct file *file=0;
	struct snd_pcm_substream *rsubstream=0;
	
	int i;
	//struct snd_dma_buffer *dmab=NULL;
	size_t i_size=0xfa00;  //64000
	
	//struct snd_pcm_ops operation;
	//struct snd_pcm_ops  ops;
	struct snd_pcm_str * pstr;
	struct snd_pcm_runtime *runtime;
	size_t size;
	
	struct snd_pcm_ops  *hardware_ops;
	struct snd_soc_pcm_runtime *rtd; 
	struct snd_soc_platform *platform;
	
	memset(&rsubstream,0x0000,sizeof(rsubstream));
	memset(&file,0x0000,sizeof(file));
	memset(&params,0x0000,sizeof(params));
	
	int ret = 0;
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

//#if 0		
	//rsubstream=start_play_codec();
	 //
	 
	 //printk("pcm_name='%s'\n\r",pcm->card->dev);
	 
	
	 
	 //Работает УРА наш кодек
	 pcm  = get_pcm();
	 codec_dai=get_codec_dai();
	 cpu_dai=get_cpu_dai();
	 codec=get_codec();
	      
	 
	 //printk("CODEC->Name='%s'\n\r",codec_dai->name);
	 //Setup наш аппаратный кодек  кодек  TLV.
	 aic3x_set_dai_fmt(codec_dai,0x1305);                 //Идёт первая функция 
	 aic3x_set_dai_sysclk(codec_dai,0x0,0x16e3600,0x1);   //Идёт вторая функция
	 aic3x_hw_params (codec);                             //Идёт Третья функция
	 aic3x_mute      (codec_dai,0x00);                    //Четвёртая функция  


	 davinci_mcasp_startup(rsubstream,cpu_dai);          //
	 davinci_mcasp_set_dai_fmt(cpu_dai,0x1305);
	 davinci_mcasp_hw_params(rsubstream,params,cpu_dai); //
	 
	 
	 
	 
	 
	 
	 //snd_pcm_open_substream(pcm, 0x0,file,&rsubstream); 
	 //snd_pcm_attach_substream(pcm, 0x0,file,&rsubstream);
	// davinci_pcm_open(rsubstream);	 
	// rsubstream->ops->prepare(rsubstream);
	 //rsubstream->ops->trigger(rsubstream, SNDRV_PCM_TRIGGER_START); 
	 
	 
	 
	 //Это  моя рабочая часть нужно что-то сделать с синхронизацией звука

	 dma_request();
	 dmab_slic = kzalloc(sizeof(*dmab_slic), GFP_KERNEL);
	 if (! dmab_slic)
	 {
	 	printk("?ERROR ALLOCATE DMAB?\n\r");
	 	return -ENOMEM;
	 }

	 if (snd_dma_alloc_pages(0x2,pcm->card->dev,i_size, dmab_slic) < 0) 
	 {
	 	printk("?ERROR ALLOCATE snd_dma_alloc_pages?\n\r");
	 	kfree(dmab_slic);
	     return -ENOMEM;
	 }

	 printk("preallocate_dma_buffer:cpu_viewed_area=%p,device_viewed_addr=%p,size=%d\n", (void *) dmab_slic->area, (void *)dmab_slic->addr, i_size);
     ////////////////////PREPARE   FUNCTIONS////////////////////////////	
	 dma_prepare();
	 snd_soc_dapm_stream_event(pcm->private_data,codec_dai->driver->playback.stream_name,SND_SOC_DAPM_STREAM_START);
	 snd_soc_dai_digital_mute(codec_dai, 0);
	 dma_trigger();
	 davinci_mcasp_trigger(rsubstream,0x1,cpu_dai); 

	
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





















