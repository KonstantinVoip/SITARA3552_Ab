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


/**************************************************************************************************
Syntax:      	    int get_rtp_prepare_packet_handler(unsigned char *packet,int packet_len)
Parameters:     	none
Remarks:			Init  MAC level for ARM Sitara AM3352 Processor.
Return Value:	    1  =>  Success  ,0 => Failure
***************************************************************************************************/
int get_rtp_prepare_packet_handler(unsigned char *packet,int packet_len)
{
  static int packet_count =0;
  int packet_size=0;	
  int rtp_payload_size=0;
  int all_data_size_byte=0;
  unsigned char data_packet_array[1514];
  int i=0;
  
 
       rtp_payload_size=packet_len-14-20-8-12;
   
    // if(packet_count==0)
    // {
    	 
       //printk("Len=%d,rtp_payload_size_byte=%d\n\r",packet_len,rtp_payload_size);
	  
       //Получаем голосовые  данные из  нашего RTP пакета  
       memcpy(&data_packet_array, &packet[14+20+8+12], rtp_payload_size);
       //Нужно всё это теперь  упаковть в большой накопительный буфер FIFO на 64000 байт складируем Пакеты.
       voice_buf_set_data_in_rtp_stream1 (data_packet_array ,rtp_payload_size);
       
       
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
























