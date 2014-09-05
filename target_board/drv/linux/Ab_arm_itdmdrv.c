/**********************************************************************************************************************
*                                        (c) COPYRIGHT by LO ZNIIS.                                             *
*                                            All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : Ab_arm_itdmdrv.c
*
* Description : Sitara 3352 linux driver kernel module for ITDM-Driver
*
* Author      : Konstantin Shiluaev..
*
******************************************************************************




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
  unsigned char data_packet_array[1514];
  int i=0;
  
 
     rtp_payload_size=packet_len-14-20-8-12;
   
     if(packet_count==0)
     {
    	 
       printk("Len=%d,rtp_payload_size_byte=%d\n\r",packet_len,rtp_payload_size);
	  
       //Получаем голосовые  данные из  нашего RTP пакета  
       memcpy(&data_packet_array, &packet[14+20+8+12], rtp_payload_size);
        	    
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
    	  
     }
	
	 packet_count++;
    	 
return 1;

}























/*****************************************************************************/
/*	PUBLIC FUNCTION DEFINITIONS					     */
/*****************************************************************************/
























