/**********************************************************************************************************************
*                                        (c) COPYRIGHT by  LO ZNIIS.                                             *
*                                            All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : Ab_arm_DMA_drv.c
*
* Description : Sitara 3352 linux driver for DMA channel to MCASP Device
*
* Author      : Konstantin Shiluaev..
*
******************************************************************************/
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




/*****************************************************************************/
/* LOCAL INCLUDES							     */
/*****************************************************************************/
#include "include/Ab_arm_dmadrv.h"
#include "include/Ab_arm_TestBuf.h"
#include "edma.h"



/*****************************************************************************/
/*	GLOBAL  DEFENITION						     				 			 */
/*****************************************************************************/
struct snd_dma_buffer    *dmab_slic=NULL;
static void enqueue_dma();
static void dma_request();
static void dma_prepare();
static void dma_trigger();


//EDMA GLOBAL Sitara Parameters
int dma_channel=0;
int dma_slot=0;
const char *playback_stream_name="Playback";
struct snd_pcm 			          *l_pcm=0;                  //pcm codec structure





/*****************************************************************************/
/*	EXTERN DEFENITION													     */
/*****************************************************************************/
extern struct snd_pcm           *get_pcm();
extern struct snd_soc_dai       *get_codec_dai();  
extern struct snd_soc_dai       *get_cpu_dai();
extern int  davinci_mcasp_trigger(struct snd_pcm_substream *substream,int cmd, struct snd_soc_dai *cpu_dai);

/*****************************************************************************/
/*	DEFINE DEFINITIONS													     */
/*****************************************************************************/
#define DMA_PRINTK(ARGS...)  printk(KERN_INFO "<%s>: ",__FUNCTION__);printk(ARGS)
#define DMA_FN_IN printk(KERN_INFO "[%s]: start\n", __FUNCTION__)
#define DMA_FN_OUT printk(KERN_INFO "[%s]: end\n",__FUNCTION__)


/**************************************************************************************************
Syntax:      	    static void callback1(unsigned lch, u16 ch_status, void *data)
Parameters:     	unsigned lch, u16 ch_status, void *data
Remarks:			Start Testing EDMA Callback1 function
***************************************************************************************************/
static void callback1(unsigned lch, u16 ch_status, void *data)
{

	//printk("+dma_irq+\n\r");
	
	if (unlikely(ch_status != DMA_COMPLETE))
	{
		printk("ulikley_DMA_no_complete\n\r");
		return;
    }
	enqueue_dma();
	
}


/**************************************************************************************************
Syntax:      	    bool Init_Arm_EDMA_interface()
Parameters:     	none
Remarks:			bool Init_Arm_McASP_interface();
Return Value:	    1  =>  Success  ,0 => Failure
***************************************************************************************************/
bool Init_Arm_EDMA_interface()
{
    struct snd_soc_dai 	     *l_codec_dai=0;            //hardware_codec TLV_aic interface
   
    struct snd_pcm_substream *l_rsubstream=0;           //substream Codec structure
    struct snd_soc_dai 	     *l_cpu_dai=0; 
    size_t i_size=0xfa00;     //64000
    //Это моя рабочая часть работает нужно  обновление буфера в  реальном времени подсовывать буфер в реальном времени
    
     
    l_codec_dai=get_codec_dai(); 
    l_pcm      =get_pcm();
    l_cpu_dai  =get_cpu_dai();
    		
    memset(&l_rsubstream,0x0000,sizeof(l_rsubstream)); 		
    		
    
	 dma_request();
	 dmab_slic = kzalloc(sizeof(*dmab_slic), GFP_KERNEL);
	 if (! dmab_slic)
	 {
	 	printk("?ERROR ALLOCATE DMAB?\n\r");
	 	return -ENOMEM;
	 }

	 if (snd_dma_alloc_pages(0x2,l_pcm->card->dev,i_size, dmab_slic) < 0) 
	 {
	 	printk("?ERROR ALLOCATE snd_dma_alloc_pages?\n\r");
	 	kfree(dmab_slic);
	    return -ENOMEM;
	 }

	 printk("preallocate_dma_buffer:cpu_viewed_area=%p,device_viewed_addr=%p,size=%d\n", (void *) dmab_slic->area, (void *)dmab_slic->addr, i_size);
     ////////////////////PREPARE   FUNCTIONS////////////////////////////	
	 dma_prepare();
	 
	 
	 //printk("stream_name='%s'\n\r",l_codec_dai->driver->playback.stream_name);
	 
	 
	 snd_soc_dapm_stream_event(l_pcm->private_data,playback_stream_name,SND_SOC_DAPM_STREAM_START);
	 snd_soc_dai_digital_mute(l_codec_dai, 0);
	 dma_trigger();
	 davinci_mcasp_trigger(l_rsubstream,0x1,l_cpu_dai); 


	return 1;
}



/*****************************************************************************/
/*	PUBLIC FUNCTION DEFINITIONS					     */
/*****************************************************************************/
static void enqueue_dma()
{
	dma_addr_t dma_pos;
	dma_addr_t src, dst;
	static unsigned int mcasp_count;
	static unsigned int period_count;
	
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
	if(mcasp_count==4)
	{
	    printk("!!!!!RESET_PERIOD_BUFFER=%d!!!!\n\r",period_count);
		mcasp_count=0;	
		period_count++;  
	}
    
    period_size = 0xfa0;                  	        //snd_pcm_lib_period_bytes(substream);
	dma_offset  = mcasp_count*period_size;          //prtd->period * period_size;
	dma_pos     = dma_src+dma_offset;               //runtime->dma_addr+ dma_offset;
	fifo_level  = 0x20;			                    //prtd->params->fifo_level;
     
	
	
	printk("DMA_POS=0x%x\n\r",dma_pos);
	
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
     
     
     if(period_count==0)
     {	 
     memcpy(0xffd50000,stereo_voice_buffer,32000);
     }
 
     
     
     
 /*    
     if(period_count==1)
     {
     memcpy(0xffd50000,&stereo_voice_buffer[128000],128000); 	 
     }
     if(period_count==1)
     {
     memcpy(0xffd50000,&stereo_voice_buffer[128000],128000); 	 
     }
     if(period_count==1)
     {
     memcpy(0xffd50000,&stereo_voice_buffer[128000],128000); 	 
     }
     if(period_count==1)
     {
     memcpy(0xffd50000,&stereo_voice_buffer[128000],128000); 	 
     }
 */    
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
	 src = dma_pos;
	 dst = 0x46400000;
	 src_bidx = data_type;
	 dst_bidx = 0;
	 src_cidx = data_type * fifo_level;
	 dst_cidx = 0;

	 //printk("SRC=0x%x|src_cidx=0x%x|count=0x%x,dma_offset=0x%x|dma_pos=0x%x\n\r",src,src_cidx,count,dma_offset,dma_pos); 
	 
	 acnt = 0x2;
	 edma_set_src(dma_slot , src, INCR, W8BIT);
	 edma_set_dest(dma_slot , dst, INCR, W8BIT);

	 edma_set_src_index(dma_slot , src_bidx, src_cidx);
	 edma_set_dest_index(dma_slot , dst_bidx, dst_cidx);
	
	
	 if (!fifo_level)
	 { 
			edma_set_transfer_params(dma_slot , acnt, count, 1, 0,ASYNC);
	 } 
			
	 else
	 {		
		    edma_set_transfer_params(dma_slot , acnt, fifo_level,count, fifo_level,ABSYNC);
	 }
	 

}



/*****************************************************************************/
/*	PUBLIC FUNCTION DEFINITIONS					     */
/*****************************************************************************/
static void dma_request()
{
	struct edmacc_param edma_par;	
	//Работает кусок pcm/dma-request выделяет нужные каналы.		
	dma_channel = edma_alloc_channel (0xa, callback1, NULL, 0x2);
		
	printk("Ab_Arm_dma_Cnannel=%d\n\r",dma_channel);
		
	// Request a Link Channel 
	dma_slot = edma_alloc_slot (EDMA_CTLR(dma_channel), EDMA_SLOT_ANY);
	printk("Ab_Arm_dma_slot=result=%d\n\r",dma_slot);
		
	edma_read_slot(dma_slot, &edma_par);
	edma_par.opt |= TCINTEN |EDMA_TCC(EDMA_CHAN_SLOT(dma_channel));
	edma_par.link_bcntrld = EDMA_CHAN_SLOT(dma_slot) << 5;
	edma_write_slot(dma_slot, &edma_par);
		
}
/*****************************************************************************/
/*	PUBLIC FUNCTION DEFINITIONS					     */
/*****************************************************************************/
static void dma_prepare()
{
	struct edmacc_param edma_par;
//	printk("Ab_ARM_DMA_PREPARE\n\r");
	enqueue_dma();
	edma_par.opt |= TCINTEN |EDMA_TCC(EDMA_CHAN_SLOT(dma_channel));
    edma_par.link_bcntrld = EDMA_CHAN_SLOT(dma_slot) << 5;
	edma_read_slot(dma_slot,&edma_par);
	edma_write_slot(dma_channel, &edma_par);	
	enqueue_dma();

}
/*****************************************************************************/
/*	PUBLIC FUNCTION DEFINITIONS					     */
/*****************************************************************************/

static void dma_trigger()
{
    int ret=0;
      
    ret=edma_start(dma_channel);
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
Syntax:      	    Clear_Arm_EDMA_interface()
Parameters:     	none
Remarks:			bool Init_Arm_McASP_interface();
Return Value:	    1  =>  Success  ,0 => Failure
***************************************************************************************************/
bool Clear_Arm_EDMA_interface()
{
     
	 snd_soc_dapm_stream_event(l_pcm->private_data,playback_stream_name,SND_SOC_DAPM_STREAM_STOP);
     snd_dma_free_pages(dmab_slic);
     edma_free_channel(dma_channel);
     edma_free_slot(dma_slot);

     
     
     
//	 snd_dma_free_pages(dmab_slic);
//	 edma_free_channel(dma_channel);
//   edma_free_slot(dma_slot);
//   snd_soc_dapm_stream_event(l_pcm->private_data,l_codec_dai->driver->playback.stream_name,SND_SOC_DAPM_STREAM_STOP);
//   dma_free_coherent(test_pcm->card->dev, size, buf->, buf.addr);
//   dma_free_writecombine(test_pcm->card->dev, size, dmab->area, dmab->addr);
//   dmab->area=0;
	
	return 1;
}












