/**********************************************************************************************************************
*                                        (c) COPYRIGHT by LO ZNIIS.                                             *
*                                            All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : Ab_arm_itdmdrv.c
*
* Description : Sitara 3352 linux driver kernel module for CREATE RTP packet for send to Eternet
*
* Author      : Konstantin Shiluaev..
*
******************************************************************************/

/*********************************************************************
*******************RTP_PAYLOAD_TYPE_OF_SERVICE************************
/*********************************************************************/
/*RTP PAcket Audio PAYLOAD_TYPE*/
#define PAYLOAD_TYPE_PCMU                  0         //G.711  Mu-LAW   (codec)  -1 channel  8000
#define PAYLOAD_TYPE_DynamicRTP_Type96     96        //PCM Signed 16bit  Little Endian Format  1-channel


#define SAMPLE_RATE			0.125	//8000 Hz
#define SEND_INTERVAL		20		//20 milisecs
#define RTP_TIME_STAMP		SEND_INTERVAL/SAMPLE_RATE
#define RTP_SEQUENCE_NUM 	0
#define RTP_SOURCE_ID		5678
#define RTP_PAYLOAD_TYPE	RTP_PCMA_PT
#define RTP_INC_TIME_STAMP	RTP_TIME_STAMP
/*****************************************************************************/
/*	COMMON CONFIGURATION						     */
/*****************************************************************************/
#include <linux/module.h>
#include <linux/device.h>
#include <linux/slab.h>
////

/*****************************************************************************/
/*	INCLUDES							     */
/*****************************************************************************/
#include "include/Ab_arm_itdmdrv.h"
#include "include/Ab_arm_ethernetdrv.h"
#include "include/Ab_arm_fifodrv.h"
#include "include/Ab_arm_codec.h"

/**************************************************************************************************
Syntax:      	    int get_rtp_prepare_packet_handler(unsigned char *packet,int packet_len)
Parameters:     	none
Remarks:			Init  MAC level for ARM Sitara AM3352 Processor.
Return Value:	    1  =>  Success  ,0 => Failure
***************************************************************************************************/
int get_rtp_prepare_packet_handler(unsigned char *packet,int packet_len)
{
  static int packet_count =0;
  unsigned short num_voice_counting_in_g711_packet=0;
  
  int packet_size=0;	
  unsigned short    rtp_payload_size=0;
  unsigned short    rtp_payload_type=0;
  //short *out;
  unsigned char *out;
  int all_data_size_byte=0;
  unsigned char data_packet_array[1514];
  unsigned char out_pcm_array[2920];
  int i=0;
  unsigned short l_i=64;
  unsigned short decode_pcm_payload_size=0;
       
  
  
  
  
  
  rtp_payload_size=packet_len-14-20-8-12;      
  //Теперь нужно  получить Payload тип правильно
  memcpy(&rtp_payload_type,packet+43,1);
 //printk("++++++rtp_payload_type=%d++++\n\r",rtp_payload_type);
       
       
       
       switch(rtp_payload_type)
       {
       case PAYLOAD_TYPE_PCMU:
       
       decode_pcm_payload_size = (rtp_payload_size*2); 
       printk("PAYLOAD_TYPE_G711_PCMU,payload_byte=%d,decode_byte_size=%d\n\r",rtp_payload_size,decode_pcm_payload_size);
       //нужно отправлять буфер на декодирование G.711 декодером очень быстро при этом 

       //Размер данных умножаем на два так как после раскодирования у нас получаеться в два раз больше массив
       /* */
       //memcpy(&data_packet_array, &packet[14+20+8+12], rtp_payload_size);
       //out=g711_ulaw_decoder(rtp_payload_size,data_packet_array);
       //memcpy(out_pcm_array,out,decode_pcm_payload_size);
       
       
       /*Нужно проверить что получилось после декодера */
       
       
       //voice_buf_set_data_in_rtp_stream1 (out_pcm_array ,double_size);
         
         
       
       
       
       
       /*Перед итем как класть этии данные в FIFO буфер  нуже отправить и на вход декодера чтобы получить PCM отсчёты */
       //Эта часть у нас работает просто отправляем иданные в FIFO буфер!!!!
       /*
       memcpy(&data_packet_array, &packet[14+20+8+12], rtp_payload_size);
       voice_buf_set_data_in_rtp_stream1 (data_packet_array ,rtp_payload_size);
       */
       break;	   
    	   
       //Ecли входной сигнал  у нас PCM   S16 Little Endian
       case PAYLOAD_TYPE_DynamicRTP_Type96:
       printk("PAYLOAD_TYPE_DynamicRTP_Type96\n\r");
       //Нужно всё это теперь Кидаем в RTP буфер.
       //Получаем голосовые  данные из  нашего RTP пакета  
       memcpy(&data_packet_array, &packet[14+20+8+12], rtp_payload_size);
       voice_buf_set_data_in_rtp_stream1 (data_packet_array ,rtp_payload_size);
       break;
       
       default:
       printk("??PAYLOAD_TYPE_UNICNOWN??\n\r");   
       break;	   
       }
       
       
       
       
    // if(packet_count==0)
    // {
    	 
       //printk("Len=%d,rtp_payload_size_byte=%d\n\r",packet_len,rtp_payload_size);
	  
/*       

       if(PAYLOAD_TYPE==PAYLOAD_TYPE_PCMU) 
       {
    	   //отправляем на декодирование  G.711 decoder
    	   
    	   
       }
       //Ecли входной сигнал  у нас PCM   S16 Little Endian
       if(PAYLOAD_TYPE==PAYLOAD_TYPE_DynamicRTP_Type96)
       {
    	  //отправляем  на проигрыш сразу или буфер в наш FIFO  
    	  
    	   
       }
*/       

       
       
       
       
       //Получаем голосовые  данные из  нашего RTP пакета  
       //memcpy(&data_packet_array, &packet[14+20+8+12], rtp_payload_size);
       
       
       
       
       //Нужно всё это теперь  упаковть в большой накопительный буфер FIFO на 64000 байт складируем Пакеты.
       //voice_buf_set_data_in_rtp_stream1 (data_packet_array ,rtp_payload_size);
       
       
       //Первые 100 Элементов голосовых данных RTP пакета  //Вроде правильно
       /*
	   for(i=0;i<100;i++)
	   {	  
	   printk("0x%x|",data_packet_array[i]);
	   }*/
	   
       //Последние 100 Элементов голосовых данных         //Вроде правильно
       /*
       for(i=0;i<100;i++)
       {	  
       printk("0x%x|",data_packet_array[(rtp_payload_size-100)+i]);
       }
       */
    	  
     //}
	   //Нужно всё это теперь  упаковть в большой накопительный буфер FIFO на 64000 байт.
       
       
       
       
	 packet_count++;
    	 
return 1;

}




/**************************************************************************************************
Syntax:      	    void rtpSend()
Parameters:     	none
Remarks:			Init  MAC level for ARM Sitara AM3352 Processor.
Return Value:	    1  =>  Success  ,0 => Failure
***************************************************************************************************/
void rtpSend()
{
	
	
}



/**************************************************************************************************
Syntax:      	    int get_rtp_prepare_packet_handler(unsigned char *packet,int packet_len)
Parameters:     	none
Remarks:			Init  MAC level for ARM Sitara AM3352 Processor.
Return Value:	    1  =>  Success  ,0 => Failure
***************************************************************************************************/













/*****************************************************************************/
/*	PUBLIC FUNCTION DEFINITIONS					     */
/*****************************************************************************/
























