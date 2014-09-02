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
	
	dma_src=dmab_slic->area;
	
    mcasp_count=mcasp_count+1;
	period_size = 0xfa0;                  	  //snd_pcm_lib_period_bytes(substream);
	dma_offset  = mcasp_count*period_size; // prtd->period * period_size;
	dma_pos     = /*0x8ad80000*/dma_src+dma_offset;  // runtime->dma_addr+ dma_offset;
	fifo_level  = 0x20;			  //prtd->params->fifo_level;
                 
	mcasp_count=mcasp_count+1;
	//printk("runtime->dma_addr=0x%x,prtd->period=0x%x\n\r",prtd->period,runtime->dma_addr);
	printk("+enqueue_dma_=0x%x,dma_src=0x%x\n\r",mcasp_count,dma_pos);
	
	
	data_type = 0x2;//prtd->params->data_type;
    count = period_size / data_type;


	//printk("DaTA_TYPE=0x%x\n\r");


     if (fifo_level)
	   count /= fifo_level;
	
	 
     //memcpy(0xffd80000,rawData,131072);
     
	 src = dma_pos;
	 dst = 0x46400000;   //prtd->params->dma_addr;
	 src_bidx = data_type;
	 dst_bidx = 0;
	 src_cidx = data_type * fifo_level;
	 dst_cidx = 0;
	
	 
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
	switch(ch_status) 
	{
	case DMA_COMPLETE:
		//irqraised1 = 1;
		DMA_PRINTK ("\n Callback 1: Channel %d status is: %u\n",lch, ch_status);
		enqueue_dma();
		break;
	case DMA_CC_ERROR:
		//irqraised1 = -1;
		DMA_PRINTK ("\nFrom Callback 1: DMA_CC_ERROR occured on Channel %d\n", lch);
		break;
	default:
		break;
	}
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
	edma_read_slot( 0xbf/*prtd->asp_link[0]*/,&edma_par /* &prtd->asp_params*/);
	edma_write_slot(0xa/*prtd->asp_channel*/, &edma_par/*&prtd->asp_params*/);	

	enqueue_dma();

}


static void dma_trigger()
{
    int ret=0;
      
    ret=edma_start(0xa/*prtd->asp_channel*/);
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
	 pcm  = get_pcm();
	 //printk("pcm_name='%s'\n\r",pcm->card->dev);
	 
	
	 
	 //Работает УРА наш кодек
	 codec_dai=get_codec_dai();
	 cpu_dai=get_cpu_dai();
	 codec=get_codec();
	
	 //params=get_snd_pcm_hw();
	 
	 
	 dev=get_davinci_audio_dev();
      
	  
	 
	 
	 //printk("CODEC->Name='%s'\n\r",codec_dai->name);
	 //Setup наш аппаратный кодек  кодек  TLV.
	 aic3x_set_dai_fmt(codec_dai,0x1305);                 //Идёт первая функция 
	 aic3x_set_dai_sysclk(codec_dai,0x0,0x16e3600,0x1);   //Идёт вторая функция
	 aic3x_hw_params (codec);                             //Идёт Третья функция
	 aic3x_mute      (codec_dai,0x00);                    //Четвёртая функция  


	 davinci_mcasp_startup(rsubstream,cpu_dai);           //
	 davinci_mcasp_set_dai_fmt(cpu_dai,0x1305);
	 davinci_mcasp_hw_params(rsubstream,params,cpu_dai);
	 
	 
     ppcm = &pcm_hardware_playback;
     printk("ppcm->formats=0x%x,ppcm->info=0x%x,ppcm->rates=0x%x,\n\r",ppcm->formats,ppcm->info,ppcm->rates); 
     dma_request();
     
     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	 //davinci_pcm_open(rsubstream);  //вся инициализацтя проходит как надо
	 //snd_pcm_open_substream(pcm, 0x0,file,&rsubstream);   //Открываем ПОТОК на буфер не нужно работаем без этого пока.
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
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 ////////////////////////////////////делаем  большую заглушку//////////////////////////////	 
	// snd_pcm_attach_substream(pcm, 0x0,file,&rsubstream); 
	 ////////////////////////////////////////////////////
	pstr = &pcm->streams[0];
	if (pstr->substream == NULL || pstr->substream_count == 0)
	{
			return -ENODEV;
	}
    
	
	
	//pstr->substream->ops->prepare(rsubstream);
	//pstr->substream->ops->trigger(rsubstream, SNDRV_PCM_TRIGGER_START); 

	//hardware_ops=&pstr->substream->ops;
	/*
	hardware_ops->prepare(rsubstream);
	hardware_ops->trigger(rsubstream, SNDRV_PCM_TRIGGER_START); 
	*/
	
	
	
	 
	for (rsubstream = pstr->substream; rsubstream; rsubstream = rsubstream->next)
			if (!SUBSTREAM_BUSY(rsubstream))
				break;
	      __ok:
		if (rsubstream == NULL)
			return -EAGAIN; 
	 
	 
    runtime = kzalloc(sizeof(*runtime), GFP_KERNEL);
	if (runtime == NULL)
	{
			return -ENOMEM;
	}
			
			
			
    size = PAGE_ALIGN(sizeof(struct snd_pcm_mmap_status));
	runtime->status = snd_malloc_pages(size, GFP_KERNEL);
		
		
    if (runtime->status == NULL) 
	{
			kfree(runtime);
			return -ENOMEM;
	 }
	 
	memset((void*)runtime->status, 0, size);
	size = PAGE_ALIGN(sizeof(struct snd_pcm_mmap_control));
	runtime->control = snd_malloc_pages(size, GFP_KERNEL);
	 
	 
	if (runtime->control == NULL)
	{
		snd_free_pages((void*)runtime->status,PAGE_ALIGN(sizeof(struct snd_pcm_mmap_status)));
		kfree(runtime);
		return -ENOMEM;
	}
	
	memset((void*)runtime->control, 0, size);
	runtime->status->state = SNDRV_PCM_STATE_OPEN;
	
	
	
	rsubstream->runtime = runtime;
	rsubstream->private_data = pcm->private_data;
	rsubstream->ref_count = 1;
	rsubstream->f_flags = 0;//file->f_flags;
	rsubstream->pid = 0;//get_pid(task_pid(current));
	
	
	/////////////////////////////////////////////////////////////////
	rtd=rsubstream->private_data;
	platform=rtd->platform;


////////////////////PREPARE   FUNCTIONS////////////////////////////	
	
	
	
	if (rtd->dai_link->ops && rtd->dai_link->ops->prepare) 
	{
		printk("<<<+++rtd->dai_link->ops->prepare+++>>>>\n\r+");
		ret = rtd->dai_link->ops->prepare(rsubstream);

		if (ret < 0) 
		{
			printk(KERN_ERR "asoc: machine prepare error\n");
			
		}
	}

	if (platform->driver->ops && platform->driver->ops->prepare) 
	{
		printk("<<<++++platform->driver->ops++++>>>\n\r+");
		
		  
		 dma_prepare();
		//ret = platform->driver->ops->prepare(rsubstream);
	     
	
		
		if (ret < 0) 
		{
			printk(KERN_ERR "asoc: platform prepare error\n");
			
		}
	}

	if (codec_dai->driver->ops->prepare)
	 {
		printk("<<++++codec_dai->driver->ops->prepare+++>>\n\r+");
		ret = codec_dai->driver->ops->prepare(rsubstream, codec_dai);

		
		if (ret < 0) 
		{
			printk(KERN_ERR "asoc: codec DAI prepare error\n");
			
		}
	}

	if (cpu_dai->driver->ops->prepare) 
	{
		printk("<<<++++++cpu_dai->driver->ops->prepare++++>>>\n\r+");
		ret = cpu_dai->driver->ops->prepare(rsubstream, cpu_dai);
		

		if (ret < 0) 
		{
			printk(KERN_ERR "asoc: cpu DAI prepare error\n");
			
		}
	}

	/* cancel any delayed stream shutdown that is pending */
	if (rsubstream->stream == SNDRV_PCM_STREAM_PLAYBACK &&codec_dai->pop_wait) 
	{
		printk("<<<<<+substream->stream == SNDRV_PCM_STREAM_PLAYBACK &&codec_dai->pop_wait++>>>\n\r+");
		codec_dai->pop_wait = 0;
		cancel_delayed_work(&rtd->delayed_work);
	}


	
	
	
	if (rsubstream->stream == SNDRV_PCM_STREAM_PLAYBACK)
	{
	snd_soc_dapm_stream_event(rtd,codec_dai->driver->playback.stream_name,SND_SOC_DAPM_STREAM_START);
	}
	else
	{	
	snd_soc_dapm_stream_event(rtd,codec_dai->driver->capture.stream_name,SND_SOC_DAPM_STREAM_START);
    }
	snd_soc_dai_digital_mute(codec_dai, 0);

	
	
    /////////////////////////////TRIGGER_FUNCTIONS////////////////////////////////	
	/////////////////////////////TRIGGER_FUNCTIONS////////////////////////////////	
	/////////////////////////////TRIGGER_FUNCTIONS////////////////////////////////	
	
	if (codec_dai->driver->ops->trigger) 
	{		
		
		printk("<<++++codec_dai->driver->ops->trigger+++>>\n\r+");
		ret = codec_dai->driver->ops->trigger(rsubstream, 0x1/*cmd*/, codec_dai);
		if (ret < 0)	
		{
			return ret;
		}
	}
	
	if (platform->driver->ops && platform->driver->ops->trigger) 
	{
		///!!!!!!!
		
		printk("<<++++platform->driver->ops->trigger+++>>\n\r+");
		dma_trigger();
		//davinci_pcm_trigger(rsubstream,0x1);
		//ret = platform->driver->ops->trigger(rsubstream,0x1 /*cmd*/);
		
		/*
		if (ret < 0)
		{
		  return ret;
		} */
    
	}

	
  if (cpu_dai->driver->ops->trigger)
  {
	   //!!!!!!!!!
	   printk("<<++++cpu_dai->driver->ops->trigger+++>>\n\r+");
	   davinci_mcasp_trigger(rsubstream,0x1,cpu_dai); 
	   
	   //ret = cpu_dai->driver->ops->trigger(rsubstream,0x1 /*cmd*/, cpu_dai);
	  /*
	   if (ret < 0)
	   {
	   return ret;
	   } 
        */
   }
	
	
  
  
  
//#endif	
		
	//soc_pcm_trigger(rsubstream,0x1);
	//pstr->substream_opened++;
	//soc_pcm_prepare(rsubstream);
	////////////////////////////////////////////////////////////////////////
	
	
	
	
	//rsubstream->ops->prepare(rsubstream);
    //rsubstream->ops->trigger(rsubstream, SNDRV_PCM_TRIGGER_START); 	
	
     
    
	
	/*
	pstr->substream->ops->prepare(rsubstream);
	pstr->substream->ops->trigger(rsubstream, SNDRV_PCM_TRIGGER_START); 
	*/
	
	
	/*
	davinci_pcm_prepare(rsubstream);
	for(i=0;i<10;i++)
		{
		davinci_pcm_trigger(rsubstream,0x1);
		}
	davinci_mcasp_trigger(rsubstream,0x1,cpu_dai); 
	*/
	
	
	 
	 
	 //ops.prepare(rsubstream);
		 /*
		 davinci_pcm_prepare(rsubstream);
		 davinci_mcasp_trigger(rsubstream,0x1,cpu_dai); 
		 */
	 
	 
	 
	 
	 
	 
	 
	/* 
	 snd_pcm_do_prepare(rsubstream,0x80002);   //Проверяю ТЕСТ
	 snd_pcm_post_prepare(rsubstream,0x80002);
	 snd_pcm_pre_start(rsubstream,0x3);
	 snd_pcm_do_start (rsubstream,0x3);
	
	*/ 
	 
	 

	 //davinci_mcasp_trigger(rsubstream,0x1,cpu_dai); 
     /*
	 snd_pcm_open_substream(pcm, 0x0,file,&rsubstream);
	 snd_pcm_pre_start(rsubstream,0x3);
     */
	 
	 //snd_pcm_attach_substream(pcm, 0x0,file,&rsubstream);
	 
	 
	 
	 
	 
	 
	 //Стартуем в самую последнюю очередь  самое последнее действие
	 // davinci_mcasp_trigger(rsubstream,0x1,cpu_dai);
	 
	 
	 //snd_pcm_attach_substream(pcm, 0x0,file,&rsubstream);
	 //настраиваем интрефейс McASP который будет смотреть в TDM пока PCM отсчёты.
	 //davinci_mcasp_startup(rsubstream,cpu_dai);
	 //davinci_pcm_open(rsubstream);
	 //davinci_mcasp_set_dai_fmt(cpu_dai,0x1305);
	 //davinci_mcasp_hw_params(rsubstream,params,cpu_dai);
	 
	 
	 //davinci_pcm_open(rsubstream);
	 //davinci_mcasp_trigger(rsubstream,0x1,cpu_dai); //отдадим на съедение 
//#endif		 
	 
	 //snd_pcm_detach_substream(rsubstream);
	 
	 
	 
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





















