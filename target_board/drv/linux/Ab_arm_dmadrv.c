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


#include <linux/delay.h>  // mdelay ,msleep,
#include <linux/timer.h>  // timer 
#include <linux/ktime.h>  // hardware timer (ktime)


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
#include "include/Ab_arm_fifodrv.h"
#include "edma.h"


/*SOUND MUSIC PARAMETERS */
#define SAMPLE_RATE			0.125	//8000 Hz
#define PCM_SAMPLE_SIZE		16

#define A_LAW_SAMPLE_SIZE	8
#define U_LAW_SAMPLE_SIZE	8



int array_current_smeschenie_dma=0;

/*****************************************************************************/
/*	GLOBAL TIMER STRUCTURES     					     				 	 */
/*****************************************************************************/
struct timer_list timer1_read;          //default timer   
static struct hrtimer hr_timer;           //high resolution timer 
///////////////TIMER TEST FUNCTION//////////
static inline ktime_t ktime_now(void);






/*****************************************************************************/
/*	GLOBAL STATIC DEFENITION						     				 			 */
/*****************************************************************************/

static void enqueue_dma();
static void dma_request();
static void dma_prepare();
static void dma_trigger();
static inline unsigned char* get_data_array();

//EDMA GLOBAL Sitara Parameters
int dma_channel=0;
int dma_slot=0;
const char *playback_stream_name="Playback";
struct snd_dma_buffer             *dmab_slic=   NULL;
struct snd_pcm 			          *l_pcm=       NULL;            //pcm codec structure
struct snd_pcm_substream 		  *l_rsubstream=NULL;            //substream Codec structure
struct snd_soc_dai 	              *l_cpu_dai=   NULL;            //mcasp codec interface
struct snd_soc_dai 	              *l_codec_dai= NULL;            //hardware_codec TLV_aic interface



/*****************************************************************************/
/* GLOBAL EXTERN DEFENITION													     */
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
Syntax:      	    static inline ktime_t ktime_now(void)
Parameters:     	void 
Remarks:			timer functions 
***************************************************************************************************/
static inline ktime_t ktime_now(void)
{
struct timespec ts;
ktime_get_ts(&ts);
//printk(".%.9ld[ns]",ts.tv_nsec);          //nano second 
  printk("%lld[s]-",(long long)ts.tv_sec);  //seconds
//return timespec_to_ktime(ts);
}




/**************************************************************************************************
Syntax:      	    static void callback1(unsigned lch, u16 ch_status, void *data)
Parameters:     	unsigned lch, u16 ch_status, void *data
Remarks:			Start Testing EDMA Callback1 function
***************************************************************************************************/
static inline unsigned char* get_data_array()
{
	 unsigned char  in_buf_rtp_dir1[4380];//размер блока приблизительный
	 int  in_size_rtp_dir1=0;
	 //Пакеты
	 int period_size=4000;
	 //int array_current_smeschenie_dma=0;
	 int array_current_smeschenie_cpu=0;
	 
	 
	 
	 static int start=0;
	 static int prosto_count=0;
	 
	 
	 //unsigned char  all_out_buf[8000];
	 static int num_of_pcaket   =0;
	 static int data_array_size =0;
	 
	 
	 
	 int l_i=200; //Количество элементов Массива
	 static int count =0;
	 int i;
	 static unsigned int mcasp_count_voice_iter_dma=0;
	 static unsigned int mcasp_count_voice_iter_cpu=0;
	 
	 
	 static unsigned int period_count_voice_iter_dma=0;
        
	 
	 unsigned int dma_src,dma_area;
	 dma_area=dmab_slic->area;
	 dma_src=dmab_slic->addr;
	 
	
	 

	 mcasp_count_voice_iter_dma=mcasp_count_voice_iter_dma+1;
	 mcasp_count_voice_iter_cpu=mcasp_count_voice_iter_cpu+1;
	 
	 
	 //Обработка ситуации первого старта нужно записать в НОЛЬ
	 if(start==0)
	 {
		 printk("_FIRST_Start_\n\r");
		 mcasp_count_voice_iter_dma=0; 
		 mcasp_count_voice_iter_cpu=0;
		 period_count_voice_iter_dma=0;
		 start++;
		// memcpy(0xffd50000,&rawData[0],64000); 
	 }
	
	 
	 if(mcasp_count_voice_iter_dma==16)
	 {
		  mcasp_count_voice_iter_dma=0;
	      period_count_voice_iter_dma=64000+period_count_voice_iter_dma;
	                            
	 }  //обнуляем счётчик начинаем заново
      
	
	 
	 
	 
	 if(mcasp_count_voice_iter_cpu==4)
	 {
		 mcasp_count_voice_iter_cpu=0;
		// memcpy(0xffd50000+array_current_smeschenie_dma,&test_8000_stereo[(array_current_smeschenie_cpu)+0],period_size); 
		                               
	 }  //обнуляем счётчик начинаем заново
	 
	 
	 
	 
	 array_current_smeschenie_dma=mcasp_count_voice_iter_dma*period_size;
	 array_current_smeschenie_cpu=mcasp_count_voice_iter_cpu*period_size;
	
	 
	// printk("array_smeschenie_dma=%d|array_smeschenie_cpu=%d\n\r",array_current_smeschenie_dma,array_current_smeschenie_cpu);
	 
	 
	 
	 //printk("mcasp=%d|DMA_array_smeschenie=%d|period=%d\n\r",mcasp_count_voice_iter_dma,array_current_smeschenie,period_count,ktime_now());
	 
	
	  //memcpy(0xffd50000+array_current_smeschenie_dma,&test_8000_stereo[(array_current_smeschenie_cpu)+0],period_size); 
	 
	 
	 // memcpy(0xffd50000+array_current_smeschenie_dma,&test_sinus_32000HZ[(array_current_smeschenie_dma)+0],period_size);
	 
	 
	 //Функция для работы  со  статическим массивом
	 //memcpy(0xffd50000+array_current_smeschenie,&test_sinus_440HZ[(mcasp_count*period_size)+period_count],period_size);
	 
	 
	
	 
	 
	 
#if 0	 
	 if(voice_buf_get_data_in_rtp_stream1 (&in_buf_rtp_dir1 ,&in_size_rtp_dir1)==1)
	 {	
	 
	     /*
		 prosto_count=prosto_count+1;
		 
		 /
		 if(prosto_count==6)
		 {
			 printk("-----------PROSTO_KOUNT--------\n\r");
		 }
		  */
		 
		      
		  //printk("in_size_rtp_dir1=%d|packet_count=%d\n\r",in_size_rtp_dir1,num_of_pcaket++);
		   memcpy(0xffd50000+array_current_smeschenie,in_buf_rtp_dir1,period_size);
	   
	     //memcpy(0xffd50000+array_current_smeschenie,&stereo_voice_buffer[(mcasp_count*period_size)+period_count],period_size);  
	     	
		 /*
		 for(i=0;i<=l_i;i++)
		 {
				// printk("{%d|0x%x}-",i,stereo_voice_buffer[i]); 
				 printk("{%d|0x%x}-",i,in_buf_rtp_dir1[i]);
			     count++;
		 }*/
	   
		 
	 }
	 else
	{
		 
		   //printk("NO_DATA_IN_FIFO_BUFFER\n\r");
		 
	 }
	 
#endif	 
  	 
    	 
	 //Распечатаем отладочный массив данных посмотрим что здесь у нас делаеться
//return &in_buf_rtp_dir1[0];	
//return &stereo_voice_buffer_64[0]; //Возврвщаем локальный буфер для проверки
//return 0; //Возврвщаем локальный буфер для проверки
}


/**************************************************************************************************
Syntax:      	    void timer1_routine(unsigned long data)
Parameters:     	void data
Remarks:			timer functions 
***************************************************************************************************/
void timer1_routine(unsigned long data)
{   

   //printk("+MOD_TIMER+\n\r");	

   get_data_array();
   mod_timer(&timer1_read, jiffies + msecs_to_jiffies(125)); // restarting timer  через  100 милисекунд  если я не ошибаюсь
  //mod_timer(&timer1_read, jiffies + msecs_to_jiffies(1250));
}




/**************************************************************************************************
Syntax:      	    static void callback1(unsigned lch, u16 ch_status, void *data)
Parameters:     	unsigned lch, u16 ch_status, void *data
Remarks:			Start Testing EDMA Callback1 function
***************************************************************************************************/
static void callback1(unsigned lch, u16 ch_status, void *data)
{
    static int irq_count=0;
	//printk("+dma_irq=%d|\n\r",irq_count++,ktime_now());

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

  
    size_t i_size_dma_block_data=0xfa00; //16000    //64000
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

	if (snd_dma_alloc_pages(0x2,l_pcm->card->dev,i_size_dma_block_data, dmab_slic) < 0) 
	{
	 	printk("?ERROR ALLOCATE snd_dma_alloc_pages?\n\r");
	 	kfree(dmab_slic);
	    return -ENOMEM;
	 }

	 printk("preallocate_dma_buffer:cpu_viewed_area=0x%x,device_viewed_addr=0x%x,size=%d\n", (void *) dmab_slic->area, (void *)dmab_slic->addr, i_size_dma_block_data);
     ////////////////////PREPARE   FUNCTIONS////////////////////////////	
	 dma_prepare();
	 
	 
	 //printk("stream_name='%s'\n\r",l_codec_dai->driver->playback.stream_name);
	 snd_soc_dapm_stream_event(l_pcm->private_data,playback_stream_name,SND_SOC_DAPM_STREAM_START);
	 snd_soc_dai_digital_mute(l_codec_dai, 0);
	 dma_trigger();
	 davinci_mcasp_trigger(l_rsubstream,SNDRV_PCM_TRIGGER_START,l_cpu_dai); 
             
	 
	 //TIMER _INIT
     //Timer1
      init_timer(&timer1_read);
      timer1_read.function = timer1_routine;
      timer1_read.data = 1;
      timer1_read.expires = jiffies + msecs_to_jiffies(125);//Старт таймера через 125[мс]
     
      
      memcpy(0xffd50000,&rawData[0],64000);
      add_timer(&timer1_read);  //Starting the timer1 
	  
	 
	 
	 

	return 1;
}



/*****************************************************************************/
/*	PUBLIC FUNCTION DEFINITIONS					     */
/*****************************************************************************/
static void enqueue_dma()
{
	dma_addr_t dma_pos;
	dma_addr_t src, dst;
	static unsigned int mcasp_count=0;
	static unsigned int period_count=0;
	static unsigned int prtd_period=0;
	
	
	unsigned int period_size;
	unsigned int dma_offset;
	unsigned int data_type;
	unsigned short acnt;
	unsigned int fifo_level;
	unsigned short src_bidx, dst_bidx;
	unsigned short src_cidx, dst_cidx;
	unsigned int count; 
	unsigned int dma_src,dma_area;
	
	dma_area=dmab_slic->area;
	dma_src=dmab_slic->addr;
	
	
   //printk("prtd_period=%d,mcasp_count=%d\n\r",prtd_period,mcasp_count);


        
 
      period_size = 0xfa0;                  	        //snd_pcm_lib_period_bytes(substream);
	
    
    
      dma_offset  = array_current_smeschenie_dma;          //prtd->period * period_size;
	  dma_pos     = dma_src+dma_offset;               //runtime->dma_addr+ dma_offset;
	
	
	fifo_level  = 0x20;			                    //prtd->params->fifo_level;
     
	
	
	
	//printk("DMA_POS=0x%x\n\r",dma_offset);
	
	
	
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
     
     //Механизм работает но слышны небольшие сбои щелчки при передаче данных(перезаписи массива).
     //Нужно сделть  механизм в реальном времени нам пригрывет файлы наверно это вообще не должно быть здесь
     //быстро подсовывать данные сюда пока они не закончились
     
     //  memcpy(0xffd50000,&rawData[0],64000); 
     //memcpy(0xffd50000,get_data_array(),64000);
      
	 src = dma_pos;
	 dst = 0x46400000;
	 src_bidx = data_type;
	 dst_bidx = 0;
	 src_cidx = data_type * fifo_level;
	 dst_cidx = 0;


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
	 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	 /*
	if(mcasp_count==15)
	{
	//printk("!!!!!RESET_PERIOD_BUFFER=%d!!!!\n\r",prtd_period);
	mcasp_count=0;
	prtd_period++;
    }*/
	 
}



/*****************************************************************************/
/*	PUBLIC FUNCTION DEFINITIONS					     */
/*****************************************************************************/
static void dma_request()
{
	struct edmacc_param edma_par;	
	//Работает кусок pcm/dma-request выделяет нужные каналы.		
	dma_channel = edma_alloc_channel (0xa, callback1, NULL, 0x2);
		
	printk("Ab_Arm_dma_Cnannel=%d|",dma_channel);
		
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
	
	
	//memcpy(0xffd50000,get_data_array(),64000);
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
  	
	//STOP MCASP_DEVICE
	 del_timer_sync(&timer1_read);              /* Deleting the timer */      
	 davinci_mcasp_trigger(l_rsubstream,SNDRV_PCM_TRIGGER_STOP,l_cpu_dai);  
	 snd_soc_dapm_stream_event(l_pcm->private_data,playback_stream_name,SND_SOC_DAPM_STREAM_STOP);
	 snd_dma_free_pages(dmab_slic);
     edma_free_channel(dma_channel);
     edma_free_slot(dma_slot);

     
     return 1;
}












