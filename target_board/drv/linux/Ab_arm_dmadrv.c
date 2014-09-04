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
******************************************************************************




/*****************************************************************************/
/*	COMMON CONFIGURATION						     */
/*****************************************************************************/

////

/*****************************************************************************/
/* LOCAL INCLUDES							     */
/*****************************************************************************/
#include "include/Ab_arm_dmadrv.h"





/*****************************************************************************/
/*	EXTERN DEFENITION													     */
/*****************************************************************************/
extern struct snd_pcm *get_pcm();




/*****************************************************************************/
/*	DEFINE DEFINITIONS													     */
/*****************************************************************************/
#define DMA_PRINTK(ARGS...)  printk(KERN_INFO "<%s>: ",__FUNCTION__);printk(ARGS)
#define DMA_FN_IN printk(KERN_INFO "[%s]: start\n", __FUNCTION__)
#define DMA_FN_OUT printk(KERN_INFO "[%s]: end\n",__FUNCTION__)




/**************************************************************************************************
Syntax:      	    bool Init_Arm_EDMA_interface()
Parameters:     	none
Remarks:			bool Init_Arm_McASP_interface();
Return Value:	    1  =>  Success  ,0 => Failure
***************************************************************************************************/
bool Init_Arm_EDMA_interface()
{

	 
    //Это моя рабочая часть работает нужно  обновление буфера в  реальном времени подсовывать буфер в реальном времени
/*
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
*/

	return 1;
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
	dma_pos     = dma_src+dma_offset;  // runtime->dma_addr+ dma_offset;
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





/*****************************************************************************/
/*	PUBLIC FUNCTION DEFINITIONS					     */
/*****************************************************************************/
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
/*****************************************************************************/
/*	PUBLIC FUNCTION DEFINITIONS					     */
/*****************************************************************************/
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
/*****************************************************************************/
/*	PUBLIC FUNCTION DEFINITIONS					     */
/*****************************************************************************/

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
Syntax:      	    Clear_Arm_EDMA_interface()
Parameters:     	none
Remarks:			bool Init_Arm_McASP_interface();
Return Value:	    1  =>  Success  ,0 => Failure
***************************************************************************************************/
bool Clear_Arm_EDMA_interface()
{

    



	return 1;
}












