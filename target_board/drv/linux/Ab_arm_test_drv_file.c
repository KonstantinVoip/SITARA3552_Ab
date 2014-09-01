/**********************************************************************************************************************
*                                        (c) COPYRIGHT by LONIIS.                                             *
*                                            All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : Ab_arm_test_drv_file.c
*
* Description : Testin Files for ARM Sitara AM3352 processor
*
* Author      : Konstantin Shiluaev..
*
******************************************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/moduleparam.h>
#include <linux/sysctl.h>
#include <linux/mm.h>
#include <linux/dma-mapping.h>
#include <linux/kernel.h>
#include <asm/dma.h>
#include "memory.h"
#include <mach/hardware.h>
#include <mach/irqs.h>
#include "edma.h"
#include <plat/omap_device.h>


#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/initval.h>
#include <sound/soc.h>

#include "davinci-pcm.h"
#include "davinci-mcasp.h"

/* McASP Instance related macros. */
#define MCASP_INST_BASE               (SOC_MCASP_1_CTRL_REGS)
#define MCASP_DATA_REGS               (SOC_MCASP_1_DATA_REGS)
#define MCASP_FIFO_REGS               (SOC_MCASP_1_FIFO_REGS)
#define MCASP_TX_INT                  (SYS_INT_MCATXINT1)

/* EDMA Related Macros. */
#define EDMA_INST_BASE                (SOC_EDMA30CC_0_REGS)
#define EDMA_CHANNEL_IN_USE           (EDMA3_CHA_MCASP1_TX)
#define EDMA_COMPLTN_INT_NUM          (SYS_INT_EDMACOMPINT)




/*****************************************************************************/
/*Local INCLUDES							     */
/*****************************************************************************/
#include "include/Ab_arm_test_drv_file.h"

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




extern struct snd_pcm     *get_pcm();
extern struct snd_soc_dai *get_cpu_dai();


#undef EDMA3_DEBUG
#define EDMA3_DEBUG 1

#ifdef EDMA3_DEBUG
#define DMA_PRINTK(ARGS...)  printk(KERN_INFO "<%s>: ",__FUNCTION__);printk(ARGS)
#define DMA_FN_IN printk(KERN_INFO "[%s]: start\n", __FUNCTION__)
#define DMA_FN_OUT printk(KERN_INFO "[%s]: end\n",__FUNCTION__)
#else
#define DMA_PRINTK( x... )
#define DMA_FN_IN
#define DMA_FN_OUT
#endif


#define MAX_DMA_TRANSFER_IN_BYTES   (32768)
#define STATIC_SHIFT                3
#define TCINTEN_SHIFT               20
#define ITCINTEN_SHIFT              21
#define TCCHEN_SHIFT                22
#define ITCCHEN_SHIFT               23

static volatile int irqraised1 = 0;
static volatile int irqraised2 = 0;



int edma3_memtomemcpytest_dma(int acnt, int bcnt, int ccnt, int sync_mode, int event_queue);
int edma3_memtomemcpytest_dma_link(int acnt, int bcnt, int ccnt, int sync_mode, int event_queue);
int edma3_memtomemcpytest_dma_chain(int acnt, int bcnt, int ccnt, int sync_mode, int event_queue);

dma_addr_t dmaphyssrc1 = 0;
dma_addr_t dmaphyssrc2 = 0;
dma_addr_t dmaphysdest1 = 0;  
dma_addr_t dmaphysdest2 = 0;

char *dmabufsrc1 = NULL;
char *dmabufsrc2 = NULL;
char *dmabufdest1 = NULL;
char *dmabufdest2 = NULL;

static int acnt = 512;
static int bcnt = 8;
static int ccnt = 8;


//
struct snd_dma_buffer buf;
struct snd_pcm *test_pcm;
struct snd_soc_dai *test_cpu_dai;
struct davinci_pcm_dma_params *pa;
struct snd_pcm_hardware *ppcm;
struct davinci_runtime_data *prtd;




//Вот он главный файл для работы с AUDIO McASP драйвером
struct davinci_audio_dev *audio_dev;


size_t size=0x8000;//0x20000; //32768  //131072 byte
/*****************************************************************************/
/*	Extern function Defenition						                 		*/
/*****************************************************************************/



/*****************************************************************************/
/*	Static TEST Local function Defenition						                 */
/*****************************************************************************/
static void test_sizeof_function();
static void test_array_function();

static int test_edma_function();
static int test_edma_function_end();



/*****************************************************************************/
/*	COMMON CONFIGURATION						     */
/*****************************************************************************/


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
		irqraised1 = 1;
		DMA_PRINTK ("\n Callback 1: Channel %d status is: %u\n",lch, ch_status);
		break;
	case DMA_CC_ERROR:
		irqraised1 = -1;
		DMA_PRINTK ("\nFrom Callback 1: DMA_CC_ERROR occured on Channel %d\n", lch);
		break;
	default:
		break;
	}
}
/**************************************************************************************************
Syntax:      	    static void callback1(unsigned lch, u16 ch_status, void *data)
Parameters:     	unsigned lch, u16 ch_status, void *data
Remarks:			Start Testing EDMA Callback2 function
***************************************************************************************************/
static void callback2(unsigned lch, u16 ch_status, void *data)
{
	switch(ch_status) 
	{
	case DMA_COMPLETE:
		irqraised2 = 1;
		//DMA_PRINTK ("\n From Callback 1: Channel %d status is: %u\n",lch, ch_status);
		break;
	case DMA_CC_ERROR:
		irqraised2 = -1;
		DMA_PRINTK ("\nFrom Callback 1: DMA_CC_ERROR occured on Channel %d\n", lch);
		break;
	default:
		break;
	}
}


/**************************************************************************************************
Syntax:      	    void Start_Test_Sitara_arm_func()
Parameters:     	none
Remarks:			Start Testing function  for ARM processor 
Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
int Start_Test_Sitara_arm_func()
{
 int result=0;	
	//printk("START_TEST_FUNCTION\n\r");
	//Nabor Testov for ARM Sitara processor	
	//TEST 1
	//test_sizeof_function();
	//TEST 2
	//test_array_function();	
	//TEST 3 EDMA Example 
	test_edma_function();
	test_edma_function_end();
	//TEST 4
	//TEST 5 
	return result;	

}




/**************************************************************************************************
Syntax:      	    static int test_edma_function()
Parameters:     	unsigned lch, u16 ch_status, void *data
Remarks:			Start Testing EDMA 
***************************************************************************************************/
static int test_edma_function()
{
int result = 0;
int iterations = 0;
int numTCs = 3;
int modes = 2;
int i=0,j=0;


//unsigned char *area;
//dma_addr_t addr=0;
//size_t size=0x20000; //131072 byte


/*
struct device *mcasp;
const char *oh_name="mcasp1";
mcasp=omap_device_get_by_hwmod_name(oh_name);
*/

//Вычищаем буфер
memset(&buf,0x0000,sizeof(buf));
memset(&test_pcm,0x0000,sizeof(test_pcm));
test_pcm  = get_pcm();



buf.dev.type=SNDRV_DMA_TYPE_DEV; 
buf.dev.dev = test_pcm->card->dev;
buf.private_data = NULL;
buf.area = dma_alloc_writecombine(test_pcm->card->dev, size,&buf.addr, GFP_KERNEL);
buf.bytes = size;

printk("preallocate_dma_buffer:cpu_viewed_area=%p,device_viewed_addr=%p,size=%d\n", (void *) buf.area, (void *) buf.addr, size);


test_cpu_dai=get_cpu_dai();
audio_dev=snd_soc_dai_get_drvdata(test_cpu_dai);

if(!audio_dev)
{
	
	printk("ERROR!\n\r"); 
}

test_cpu_dai->playback_dma_data=audio_dev->dma_params;
//printk("mcasp_audio_dai:audio_dev->=0x%x',audio_dev->=0x%x\n\r",audio_dev->dma_params,audio_dev->sample_rate);
/*
printk("test_cpu_dai->name='%s',test_cpu_dai->playback_dma_data=0x%x\n\r",test_cpu_dai->name,test_cpu_dai->playback_dma_data);
*/

pa=test_cpu_dai->playback_dma_data;
if (!pa){
	printk("ERROR!\n\r"); 
}
	

//printk("pcm_dma_params:pa->acnt=%p,pa->channel=%p, pa->sram_size=%d\n", (void *) pa->dma_addr, (void *) pa->channel, (void *) pa->sram_size);
ppcm = &pcm_hardware_playback;








///////////////////////////////



/*
dmabufsrc1 = dma_alloc_coherent (NULL, MAX_DMA_TRANSFER_IN_BYTES,&dmaphyssrc1, 0);
DMA_PRINTK( "\nSRC1:\t%x", dmaphyssrc1);
if (!dmabufsrc1) 
{
	DMA_PRINTK ("dma_alloc_coherent failed for dmaphyssrc1\n");
	return -ENOMEM;
}
*/



/*
dmabufdest1 = dma_alloc_coherent (NULL, MAX_DMA_TRANSFER_IN_BYTES,&dmaphysdest1, 0);
DMA_PRINTK( "\nDST1:\t%x", dmaphysdest1);

if (!dmabufdest1)
{
	DMA_PRINTK("dma_alloc_coherent failed for dmaphysdest1\n");
	dma_free_coherent(NULL, MAX_DMA_TRANSFER_IN_BYTES, dmabufsrc1,dmaphyssrc1);
	return -ENOMEM;
}*/



//for (j = 0 ; j < numTCs ; j++)
//{
      
//	for (i = 0 ; i < modes ; i++) 
//	{	
	 





	 result = edma3_memtomemcpytest_dma(acnt, bcnt, ccnt, i, j);
	
//	}
	 	
//}








//DMA_PRINTK( "\nSRC1:\t0x%x,area1=%p\n\r", addr,(void *) area); 
 
		//printk ("\nInitializing edma_test module\n");

		//DMA_PRINTK ( "\nACNT=%d, BCNT=%d, CCNT=%d", acnt, bcnt, ccnt);

		/* allocate consistent memory for DMA
		 * dmaphyssrc1(handle)= device viewed address.
		 * dmabufsrc1 = CPU-viewed address
		 */

#if 0        
         
	    dmabufsrc1 = dma_alloc_coherent (NULL, MAX_DMA_TRANSFER_IN_BYTES,&dmaphyssrc1, 0);
		DMA_PRINTK( "\nSRC1:\t%x", dmaphyssrc1);
		if (!dmabufsrc1) 
		{
			DMA_PRINTK ("dma_alloc_coherent failed for dmaphyssrc1\n");
			return -ENOMEM;
		}

		
		
		
		dmabufdest1 = dma_alloc_coherent (NULL, MAX_DMA_TRANSFER_IN_BYTES,&dmaphysdest1, 0);
		DMA_PRINTK( "\nDST1:\t%x", dmaphysdest1);
		
		if (!dmabufdest1)
		{
			DMA_PRINTK("dma_alloc_coherent failed for dmaphysdest1\n");
			dma_free_coherent(NULL, MAX_DMA_TRANSFER_IN_BYTES, dmabufsrc1,dmaphyssrc1);
			return -ENOMEM;
		}
#endif  
     
 

		
/*		
		dmabufsrc2 = dma_alloc_coherent (NULL, MAX_DMA_TRANSFER_IN_BYTES,&dmaphyssrc2, 0);
		DMA_PRINTK( "\nSRC2:\t%x", dmaphyssrc2);
		if (!dmabufsrc2) 
		{
			DMA_PRINTK ("dma_alloc_coherent failed for dmaphyssrc2\n");

			dma_free_coherent(NULL, MAX_DMA_TRANSFER_IN_BYTES, dmabufsrc1,dmaphyssrc1);
			dma_free_coherent(NULL, MAX_DMA_TRANSFER_IN_BYTES, dmabufdest1,dmaphysdest1);
			return -ENOMEM;
		}

		dmabufdest2 = dma_alloc_coherent (NULL, MAX_DMA_TRANSFER_IN_BYTES,&dmaphysdest2, 0);
		DMA_PRINTK( "\nDST2:\t%x", dmaphysdest2);
		if (!dmabufdest2)
		{
			DMA_PRINTK ("dma_alloc_coherent failed for dmaphysdest2\n");
			dma_free_coherent(NULL, MAX_DMA_TRANSFER_IN_BYTES, dmabufsrc1,dmaphyssrc1);
			dma_free_coherent(NULL, MAX_DMA_TRANSFER_IN_BYTES, dmabufdest1,dmaphysdest1);
			dma_free_coherent(NULL, MAX_DMA_TRANSFER_IN_BYTES, dmabufsrc2,dmaphyssrc2);
			return -ENOMEM;
		}
*/

		
		
#if 0		
		
		for (iterations = 0 ; iterations < 10 ; iterations++)
		{
		  	DMA_PRINTK ("Iteration = %d\n", iterations);

			for (j = 0 ; j < numTCs ; j++)
			{ //TC
				DMA_PRINTK ("TC = %d\n", j);

				for (i = 0 ; i < modes ; i++) 
				{	//sync_mode
					DMA_PRINTK ("Mode = %d\n", i);

					/* Run all EDMA3 test cases */
					DMA_PRINTK ("Starting edma3_memtomemcpytest_dma\n");
					result = edma3_memtomemcpytest_dma(acnt, bcnt, ccnt, i, j);
					if (0 == result) 
					{
						printk("edma3_memtomemcpytest_dma passed\n");
					} 
					else 
					{
						printk("edma3_memtomemcpytest_dma failed\n");
					}
                    
					
					/*
					if (0 == result) {
						DMA_PRINTK ("Starting edma3_memtomemcpytest_dma_link\n");
						result = edma3_memtomemcpytest_dma_link(acnt, bcnt, ccnt, i, j);
						if (0 == result) {
							printk("edma3_memtomemcpytest_dma_link passed\n");
						} else {
							printk("edma3_memtomemcpytest_dma_link failed\n");
						}
					}*/
                    /*
					if (0 == result) 
					{
						DMA_PRINTK ("Starting edma3_memtomemcpytest_dma_chain\n");
						result = edma3_memtomemcpytest_dma_chain(acnt, bcnt, ccnt, i, j);
						if (0 == result) 
						{
							printk("edma3_memtomemcpytest_dma_chain passed\n");
						} 
						else
						{
							printk("edma3_memtomemcpytest_dma_chain failed\n");
						}
					}*/

					
				}

			}
		 
		}
#endif
		
		
		
return result;	
}









/* DMA Channel, Mem-2-Mem Copy, ASYNC Mode, INCR Mode */
int edma3_memtomemcpytest_dma (int acnt, int bcnt, int ccnt, int sync_mode, int event_queue)
{
	int result = 0;
	unsigned int dma_ch = 0;
	int i;
	int count = 0;
	unsigned int Istestpassed = 0u;
	unsigned int numenabled = 0;
	unsigned int BRCnt = 0;
	int srcbidx = 0;
	int desbidx = 0;
	int srccidx = 0;
	int descidx = 0;
	struct edmacc_param edma_par;
   
	printk("data_size=acnt*bcnt*ccnt=%d_bytes\n\r",acnt*bcnt*ccnt);
    

	/* Set B count reload as B count. */
	BRCnt = bcnt;

	/* Setting up the SRC/DES Index */
	srcbidx = acnt;
	desbidx = acnt;

	/* A Sync Transfer Mode */
	srccidx = acnt;
	descidx = acnt;


	
	
//Работает кусок pcm/dma-request выделяет нужные каналы.	
/*	
	result = edma_alloc_channel (0xa, callback1, NULL, 0x2);
	
	//result=edma_alloc_channel(pa->channel,callback1, NULL,0x2);
	printk("DMA_CHANNEL=result=%d\n\r",result);
	
	// Request a Link Channel 
	result = edma_alloc_slot (EDMA_CTLR(0xa), EDMA_SLOT_ANY);
	printk("SLOT=result=%d\n\r",result);
	
	
	edma_read_slot(0xbf, &edma_par);
	edma_par.opt |= TCINTEN |EDMA_TCC(EDMA_CHAN_SLOT(0xa));
	edma_par.link_bcntrld = EDMA_CHAN_SLOT(0xbf) << 5;
	edma_write_slot(0xbf, &edma_par);
*/	
	
	
	
	
	
#if 0	
	/* Initalize source and destination buffers */
	for (count = 0u; count < (acnt*bcnt*ccnt); count++) 
	{
		dmabufsrc1[count] = 'A' + (count % 26);
		buf.area[count]=0;
	  //buf.area[count]='A' + (count % 26);
	}
	
	
	
	
	
	
	if (result < 0)
	{
			DMA_PRINTK ("\nedma3_memtomemcpytest_dma::edma_alloc_channel failed for dma_ch, error:%d\n", result);
			return result;
	}

	dma_ch = result;
	
	
	edma_set_src  (dma_ch, (unsigned long)(dmaphyssrc1), INCR, W8BIT);
	edma_set_dest (dma_ch, (unsigned long)(pa->dma_addr), INCR, W8BIT);
	//edma_set_dest (dma_ch, (unsigned long)(0x11223344), INCR, W8BIT);
	
	//edma_set_dest (dma_ch, (unsigned long)(dmaphysdest1), INCR, W8BIT);
	
	
    edma_set_src_index (dma_ch, srcbidx, srccidx);
    edma_set_dest_index (dma_ch, desbidx, descidx);

	/* A Sync Transfer Mode */
	edma_set_transfer_params (dma_ch, acnt, bcnt, ccnt, BRCnt, ASYNC);

	/* Enable the Interrupts on Channel 1 */
	edma_read_slot (dma_ch, &param_set);
	param_set.opt |= (1 << ITCINTEN_SHIFT);
	param_set.opt |= (1 << TCINTEN_SHIFT);
	param_set.opt |= EDMA_TCC(EDMA_CHAN_SLOT(dma_ch));
	edma_write_slot (dma_ch, &param_set);
    
    
	numenabled = bcnt * ccnt;
	printk("numenabled =%d\n\r",bcnt * ccnt);
    
	for (i = 0; i < numenabled; i++) 
	{
			irqraised1 = 0;

			/*
			 * Now enable the transfer as many times as calculated above.
			 */
			result = edma_start(dma_ch);
			if (result != 0) 
			{
				DMA_PRINTK ("edma3__dma: davinci_start_dma failed \n");
				break;
			}

			//printk ("WHILE COMPLET IRQ\n\r");
			 
			/* Wait for the Completion ISR. */
			while (irqraised1 == 0u);

			/* Check the status of the completed transfer */
			if (irqraised1 < 0) 
			{
				/* Some error occured, break from the FOR loop. */
				DMA_PRINTK ("edma3__dma: Event Miss Occured!!!\n");
				break;
			}
	 }//END FOR

     
	 /*
		if (0 == result) 
		{
			for (i = 0; i < (acnt*bcnt*ccnt); i++) 
			{
			
				//printk("%x",buf.area[i]);
				if (dmabufsrc1[i] !=dmabufdest1[i]) 
				{
					DMA_PRINTK ("\n edma3_dma: Data write-read matching failed at = %u\n",i);
					Istestpassed = 0u;
					break;
				}
			}
			if (i == (acnt*bcnt*ccnt))
			{
				Istestpassed = 1u;
			}
			edma_stop(dma_ch);
			edma_free_channel(dma_ch);
		}
       

		
		if (Istestpassed == 1u) 
		{
			DMA_PRINTK ("\nedma3__dma: EDMA Data Transfer Successfull \n");
		} 
		else 
		{
			DMA_PRINTK ("\nedma3__dma: EDMA Data Transfer Failed \n");
		}
       */
		
#endif		

return result;
}




/* 2 DMA Channels Linked, Mem-2-Mem Copy, ASYNC Mode, INCR Mode */
int edma3_memtomemcpytest_dma_link(int acnt, int bcnt, int ccnt, int sync_mode, int event_queue)
{
	int result = 0;
	unsigned int dma_ch1 = 0;
	unsigned int dma_ch2 = 0;
	int i;
	int count = 0;
	unsigned int Istestpassed1 = 0u;
	unsigned int Istestpassed2 = 0u;
	unsigned int numenabled = 0;
	unsigned int BRCnt = 0;
	int srcbidx = 0;
	int desbidx = 0;
	int srccidx = 0;
	int descidx = 0;
	struct edmacc_param param_set;

	/* Initalize source and destination buffers */
	for (count = 0u; count < (acnt*bcnt*ccnt); count++) {
		dmabufsrc1[count] = 'A' + (count % 26);
		dmabufdest1[count] = 0;

		dmabufsrc2[count] = 'A' + (count % 26);
		dmabufdest2[count] = 0;
	}

	/* Set B count reload as B count. */
	BRCnt = bcnt;

	/* Setting up the SRC/DES Index */
	srcbidx = acnt;
	desbidx = acnt;

	/* A Sync Transfer Mode */
	srccidx = acnt;
	descidx = acnt;

	result = edma_alloc_channel (EDMA_CHANNEL_ANY, callback1, NULL, event_queue);

	if (result < 0) {
		DMA_PRINTK ("edma3_memtomemcpytest_dma_link::edma_alloc_channel "
				"failed for dma_ch1, error:%d\n", result);
		return result;
	}

	dma_ch1 = result;
	edma_set_src (dma_ch1, (unsigned long)(dmaphyssrc1), INCR, W8BIT);
	edma_set_dest (dma_ch1, (unsigned long)(dmaphysdest1), INCR, W8BIT);
	edma_set_src_index (dma_ch1, srcbidx, srccidx);
	edma_set_dest_index (dma_ch1, desbidx, descidx);
	edma_set_transfer_params (dma_ch1, acnt, bcnt, ccnt, BRCnt, ASYNC);

	/* Enable the Interrupts on Channel 1 */
	edma_read_slot (dma_ch1, &param_set);
	param_set.opt |= (1 << ITCINTEN_SHIFT);
	param_set.opt |= (1 << TCINTEN_SHIFT);
	param_set.opt |= EDMA_TCC(EDMA_CHAN_SLOT(dma_ch1));
	edma_write_slot(dma_ch1, &param_set);

	/* Request a Link Channel */
	result = edma_alloc_slot (0, EDMA_SLOT_ANY);

	if (result < 0) {
		DMA_PRINTK ("\nedma3_memtomemcpytest_dma_link::edma_alloc_slot "
				"failed for dma_ch2, error:%d\n", result);
		return result;
	}

	dma_ch2 = result;
	edma_set_src (dma_ch2, (unsigned long)(dmaphyssrc2), INCR, W8BIT);
	edma_set_dest (dma_ch2, (unsigned long)(dmaphysdest2), INCR, W8BIT);
	edma_set_src_index (dma_ch2, srcbidx, srccidx);
	edma_set_dest_index (dma_ch2, desbidx, descidx);
	edma_set_transfer_params (dma_ch2, acnt, bcnt, ccnt, BRCnt, ASYNC);

	/* Enable the Interrupts on Channel 2 */
	edma_read_slot (dma_ch2, &param_set);
	param_set.opt |= (1 << ITCINTEN_SHIFT);
	param_set.opt |= (1 << TCINTEN_SHIFT);
	param_set.opt |= EDMA_TCC(EDMA_CHAN_SLOT(dma_ch1));
	edma_write_slot(dma_ch2, &param_set);

	/* Link both the channels */
	edma_link(dma_ch1, dma_ch2);

	numenabled = bcnt * ccnt;

	for (i = 0; i < numenabled; i++) {
		irqraised1 = 0;

		/*
		 * Now enable the transfer as many times as calculated above.
		 */
		result = edma_start(dma_ch1);
		if (result != 0) {
			DMA_PRINTK ("edma3_memtomemcpytest_dma_link: davinci_start_dma failed \n");
			break;
		}

		/* Wait for the Completion ISR. */
		while (irqraised1 == 0u);

		/* Check the status of the completed transfer */
		if (irqraised1 < 0) {
			/* Some error occured, break from the FOR loop. */
			DMA_PRINTK ("edma3_memtomemcpytest_dma_link: "
					"Event Miss Occured!!!\n");
			break;
		}
	}

	if (result == 0) {
		for (i = 0; i < numenabled; i++) {
			irqraised1 = 0;

			/*
			 * Now enable the transfer as many times as calculated above
			 * on the LINK channel.
			 */
			result = edma_start(dma_ch1);
			if (result != 0) {
				DMA_PRINTK ("\nedma3_memtomemcpytest_dma_link: davinci_start_dma failed \n");
				break;
			}

			/* Wait for the Completion ISR. */
			while (irqraised1 == 0u);

			/* Check the status of the completed transfer */
			if (irqraised1 < 0) {
				/* Some error occured, break from the FOR loop. */
				DMA_PRINTK ("edma3_memtomemcpytest_dma_link: "
						"Event Miss Occured!!!\n");
				break;
			}
		}
	}

	if (0 == result) {
		for (i = 0; i < (acnt*bcnt*ccnt); i++) {
			if (dmabufsrc1[i] != dmabufdest1[i]) {
				DMA_PRINTK ("\nedma3_memtomemcpytest_dma_link(1): Data "
						"write-read matching failed at = %u\n",i);
				Istestpassed1 = 0u;
				break;
			}
		}
		if (i == (acnt*bcnt*ccnt)) {
			Istestpassed1 = 1u;
		}

		for (i = 0; i < (acnt*bcnt*ccnt); i++) {
			if (dmabufsrc2[i] != dmabufdest2[i]) {
				DMA_PRINTK ("\nedma3_memtomemcpytest_dma_link(2): Data "
						"write-read matching failed at = %u\n",i);
				Istestpassed2 = 0u;
				break;
			}
		}
		if (i == (acnt*bcnt*ccnt)) {
			Istestpassed2 = 1u;
		}

		edma_stop(dma_ch1);
		edma_free_channel(dma_ch1);

		edma_stop(dma_ch2);
		edma_free_slot(dma_ch2);
		edma_free_channel(dma_ch2);
	}

	if ((Istestpassed1 == 1u) && (Istestpassed2 == 1u)) {
		DMA_PRINTK ("\nedma3_memtomemcpytest_dma_link: EDMA Data Transfer Successfull\n");
	} else {
		DMA_PRINTK ("\nedma3_memtomemcpytest_dma_link: EDMA Data Transfer Failed\n");
	}

	return result;
}

/* 2 DMA Channels Chained, Mem-2-Mem Copy, A-SYNC Mode, INCR Mode */
int edma3_memtomemcpytest_dma_chain(int acnt, int bcnt, int ccnt, int sync_mode, int event_queue)
{
	int result = 0;
	unsigned int dma_ch1 = 0;
	unsigned int dma_ch2 = 0;
	int i;
	int count = 0;
	unsigned int Istestpassed1 = 0u;
	unsigned int Istestpassed2 = 0u;
	unsigned int numenabled = 0;
	unsigned int BRCnt = 0;
	int srcbidx = 0;
	int desbidx = 0;
	int srccidx = 0;
	int descidx = 0;
	struct edmacc_param param_set;

	/* Initalize source and destination buffers */
	for (count = 0u; count < (acnt*bcnt*ccnt); count++)
	{
		dmabufsrc1[count] = 'A' + (count % 26);
		dmabufdest1[count] = 0;

		dmabufsrc2[count] = 'A' + (count % 26);
		dmabufdest2[count] = 0;
	}

	/* Set B count reload as B count. */
	BRCnt = bcnt;

	/* Setting up the SRC/DES Index */
	srcbidx = acnt;
	desbidx = acnt;

	/* A Sync Transfer Mode */
	srccidx = acnt;
	descidx = acnt;

	result = edma_alloc_channel (EDMA_CHANNEL_ANY, callback1, NULL, event_queue);
	if (result < 0)
	{
		DMA_PRINTK ("\nedma3_memtomemcpytest_dma_chain::davinci_request_dma"
				" failed for dma_ch1, error:%d\n", result);
		return result;
	}
	dma_ch1 = result;
	edma_set_src (dma_ch1, (unsigned long)(dmaphyssrc1), INCR, W8BIT);
        edma_set_dest (dma_ch1, (unsigned long)(dmaphysdest1), INCR, W8BIT);
        edma_set_src_index (dma_ch1, srcbidx, srccidx);
        edma_set_dest_index (dma_ch1, desbidx, descidx);
        edma_set_transfer_params (dma_ch1, acnt, bcnt, ccnt, BRCnt, ASYNC);

	/* Request another DMA Channel */
	result = edma_alloc_channel (EDMA_CHANNEL_ANY, callback2, NULL, event_queue);

        if (result < 0) {
		DMA_PRINTK ("\nedma3_memtomemcpytest_dma_chain::davinci_request_dma "
				"failed for dma_ch2, error:%d\n", result);
		edma_free_channel(dma_ch1);
                return result;
        }

        dma_ch2 = result;
        edma_set_src (dma_ch2, (unsigned long)(dmaphyssrc2), INCR, W8BIT);
        edma_set_dest (dma_ch2, (unsigned long)(dmaphysdest2), INCR, W8BIT);
        edma_set_src_index (dma_ch2, srcbidx, srccidx);
        edma_set_dest_index (dma_ch2, desbidx, descidx);
        edma_set_transfer_params (dma_ch2, acnt, bcnt, ccnt, BRCnt, ASYNC);

	/* Chain both the channels */
	edma_read_slot(dma_ch1, &param_set);
	param_set.opt |= (1 << TCCHEN_SHIFT);
	param_set.opt |= EDMA_TCC(EDMA_CHAN_SLOT(dma_ch2));
	edma_write_slot(dma_ch1, &param_set);

	/*
	 * Enable the Intermediate and Final Interrupts on Channel 1.
	 * Also, Enable the Intermediate Chaining.
	 */
	edma_read_slot(dma_ch1, &param_set);
	param_set.opt |= (1 << ITCCHEN_SHIFT);
	param_set.opt |= (1 << TCINTEN_SHIFT);
	param_set.opt |= (1 << ITCINTEN_SHIFT);
	edma_write_slot(dma_ch1, &param_set);

	/* Enable the Intermediate and Final Interrupts on Channel 2 */
	edma_read_slot(dma_ch2, &param_set);
	param_set.opt |= (1 << TCINTEN_SHIFT);
	param_set.opt |= (1 << ITCINTEN_SHIFT);
	edma_write_slot(dma_ch2, &param_set);

	numenabled = bcnt * ccnt;

	for (i = 0; i < numenabled; i++) {
		irqraised2 = 0;

		/*
		 * Now enable the transfer for Master channel as many times
		 * as calculated above.
		 */
		result = edma_start(dma_ch1);
		if (result != 0) {
			DMA_PRINTK ("\nedma3_memtomemcpytest_dma_chain: "
					"davinci_start_dma failed \n");

			edma_stop(dma_ch1);

			edma_free_channel(dma_ch1);
			edma_free_channel(dma_ch2);

			return result;
		}

		/*
		 * Transfer on the master channel (ch1Id) will finish after some
		 * time.
		 * Now, because of the enabling of intermediate chaining on channel
		 * 1, after the transfer gets over, a sync event will be sent
		 * to channel 2, which will trigger the transfer on it.
		 * Also, Final and Intermediate Transfer Complete
		 * Interrupts are enabled on channel 2, so we should wait for the
		 * completion ISR on channel 2 first, before proceeding
		 * ahead.
		 */
		while (irqraised2 == 0u);

		/* Check the status of the completed transfer */
		if (irqraised2 < 0) {
			/* Some error occured, break from the FOR loop. */
			DMA_PRINTK ("edma3_test_with_chaining: "
					"Event Miss Occured!!!\n");
			break;
		}
	}

	/* Match the Source and Destination Buffers. */
	for (i = 0; i < (acnt*bcnt*ccnt); i++) {
		if (dmabufsrc1[i] != dmabufdest1[i]) {
			DMA_PRINTK ( "\nedma3_memtomemcpytest_dma_chain(1): Data write-read matching failed at = %u\n",i);
			Istestpassed1 = 0u;
			break;
		}
	}
	if (i == (acnt*bcnt*ccnt)) {
		Istestpassed1 = 1u;
	}

	for (i = 0; i < (acnt*bcnt*ccnt); i++) {
		if (dmabufsrc2[i] != dmabufdest2[i]) {
			DMA_PRINTK ( "\nedma3_memtomemcpytest_dma_chain(2): Data write-read matching failed at = %u\n",i);
			Istestpassed2 = 0u;
			break;
		}
	}
	if (i == (acnt*bcnt*ccnt)) {
		Istestpassed2 = 1u;
	}

	edma_stop(dma_ch1);
	edma_free_channel(dma_ch1);

	edma_stop(dma_ch2);
	edma_free_slot(dma_ch2);
	edma_free_channel(dma_ch2);

	if((Istestpassed1 == 1u) && (Istestpassed2 == 1u)) {
		DMA_PRINTK ( "\nedma3_memtomemcpytest_dma_chain: EDMA Data Transfer Successfull\n");
	} else {
		DMA_PRINTK ( "\nedma3_memtomemcpytest_dma_chain: EDMA Data Transfer Failed\n");
	}
	return result;
}




/**************************************************************************************************
Syntax:      	    static int test_edma_function_end()
Parameters:     	unsigned lch, u16 ch_status, void *data
Remarks:			END and Clear DMA FUNCTION
***************************************************************************************************/
static int test_edma_function_end()
{

	/*
	buf.dev.dev = pcm->card->dev;
	buf.private_data = NULL;
	buf.area = dma_alloc_writecombine(pcm->card->dev, size,&buf.addr, GFP_KERNEL);
	buf.bytes = size;
    */
	
	/*
	buf.dev.dev = test_pcm->card->dev;
	buf.private_data = NULL;
	buf.area = dma_alloc_writecombine(test_pcm->card->dev, size,&buf.addr, GFP_KERNEL);
	buf.bytes = size;
	printk("davinci_pcm: preallocate_dma_buffer:cpu_viewed_area=%p,device_viewed_addr=%p,size=%d\n", (void *) buf.area, (void *) buf.addr, size);
	*/
	
	//dma_free_coherent(test_pcm->card->dev, size, buf.area, buf.addr);
	  dma_free_writecombine(test_pcm->card->dev, size, buf.area, buf.addr);
	  buf.area=0;
	
	
	
	/*
	dma_free_coherent(NULL, MAX_DMA_TRANSFER_IN_BYTES, dmabufsrc1, dmaphyssrc1);
	dma_free_coherent(NULL, MAX_DMA_TRANSFER_IN_BYTES, dmabufdest1,dmaphysdest1);
	dma_free_coherent(NULL, MAX_DMA_TRANSFER_IN_BYTES, dmabufsrc2, dmaphyssrc2);
	dma_free_coherent(NULL, MAX_DMA_TRANSFER_IN_BYTES, dmabufdest2,dmaphysdest2);
	*/
	
	printk ("\Clear DMA AREA\n");
	
}































































/**************************************************************************************************
Syntax:      	    test_array_function()
Parameters:     	none
Remarks:			Start Testing function  for ARM processor 
Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
static void test_array_function()
{
	unsigned char         d;//1 байт
	unsigned short int    a;//2 байта
    unsigned long  int    b;//4 байт
	
    unsigned long  int mas_long[2]={0x11223344,0x55667788};
    unsigned char  mas_byte [8];
    unsigned short int array[4]={0x1122,0x3344,0x5566,0x7788}; 
    unsigned short int mas  [4];
    
    
    //!!!!!!!!!!Примечание перевёртыши происходтя при  копирование массива unsigned short int в  unsigned char тоже  самое и с 32 разрядами!!!!!!!!!!!!!!
    //Перед   копированием нужно  превращать в Little Endian массивы
    //memcpy(mas_byte, array, sizeof(array));
     memcpy(mas_byte, mas_long, sizeof(mas_long));
    //memcpy(mas, array, sizeof(array));
     printk("mas0=0x%x,mas1=0x%x,mas2=0x%x,mas3=0x%x,mas4=0x%x,mas5=0x%x,mas6=0x%x,mas7=0x%x\n\r",mas_byte[0],mas_byte[1],mas_byte[2],mas_byte[3],mas_byte[4],mas_byte[5],mas_byte[6],mas_byte[7]);
    
    //printk("mas0=0x%x,mas1=0x%x,mas2=0x%x,mas3=0x%x\n\r",mas[0],mas[1],mas[2],mas[3]);
    //array[4]={0x1122;0x3344;0x5566;0x7788};
    
    __be16  e;
    //e=0x2233;
    
    // u16 m;
    //m=0x2233;
    
    //printk("e=%d,e=0x%x\n\r",e,e);
    /*
	d=0x0099;
	b=0x11223344;
	a=0x2255;
    
	printk("d=%d,d=0x%x\n\r",d,d);
    printk("a=%d,a=0x%x\n\r",a,a);
    printk("b=%d,b=0x%x\n\r",b,b);
	*/
	
}


/**************************************************************************************************
Syntax:      	    test_sizeof_function()
Parameters:     	none
Remarks:			Start Testing function  for ARM processor 
Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
static void test_sizeof_function()
{
  
  //unsigned ->>беззнаковый  (только положительные значения)
  //signed   -> знаковый	 (есть минус)  //по  умолчанию  все signed
	
  unsigned char   a;   //1 байт
  unsigned short  b;   //2 байт
  unsigned long   c;   //4 байт
		
  signed char     d;   //1 байт
  signed short    e;   //2 байт
  signed long     f;   //4 байт
  
  loff_t          t;    //8  байт
  size_t          len;  //4  байт

  int             l;    //4 байт (32 разряда)
  unsigned short int    m;    //2 байт (16 разрядов)    0-65535
  float           k;    //4 байт
  
  
  printk(KERN_INFO "float =  %d\n\r",sizeof k);
  printk(KERN_INFO "short int =  %d\n\r",sizeof m);
  printk(KERN_INFO "int =  %d\n\r",sizeof l);
  
  printk(KERN_INFO "lott_f =  %d\n\r",sizeof t);
  printk(KERN_INFO "size_t =  %d\n\r",sizeof len);
  printk(KERN_INFO "unsigned char =  %d\n\r",sizeof a);
  printk(KERN_INFO "unsigned short= %d\n\r",sizeof b);
  printk(KERN_INFO "unsigned long= %d\n\r",sizeof c);
  printk(KERN_INFO "signed char  =  %d\n\r",sizeof d);
  printk(KERN_INFO "signed short= %d\n\r",sizeof e);
  printk(KERN_INFO "signed long =  %d\n\r",sizeof f);
			
	
}




















