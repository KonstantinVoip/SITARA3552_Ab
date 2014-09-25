/**********************************************************************************************************************
*                                        (c) COPYRIGHT by LO ZNIIS 2014                                             *
*                                            All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : Ab_arm_maindrv.c
*
* Description : Sitara 3352 linux Main driver kernel module init Start 
*
* Author      : Konstantin Shiluaev..
*
******************************************************************************/

/*-----------------------------------------------------------------------------
GENERAL NOTES
-----------------------------------------------------------------------------*/


/*****************************************************************************/
/*	COMMON CONFIGURATION						     */
/*****************************************************************************/

////

/*****************************************************************************/
/* Linux INCLUDES							     */
/*****************************************************************************/
#include <linux/module.h> // Needed by all modules
#include <linux/kernel.h> // Needed for KERN_ALERT
#include <linux/init.h>   // Needed for the macros
#include <linux/delay.h>  // mdelay ,msleep,
#include <linux/timer.h>  // timer 
#include <linux/ktime.h>  // hardware timer (ktime)


/*****************************************************************************/
/*NetFilter Linux INCLUDES							     */
/*****************************************************************************/
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/if_ether.h>
#include <linux/socket.h>
#include <linux/ioctl.h>
#include <linux/if.h>
#include <linux/in.h>
#include <linux/if_packet.h>
#include <linux/icmp.h>
#include <linux/udp.h>
#include <linux/if_arp.h>
#include <linux/etherdevice.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>


/*****************************************************************************/
/*Local INCLUDES							     */
/*****************************************************************************/
#include "include/Ab_arm_maindrv.h"
#include "include/Ab_arm_test_drv_file.h"
#include "include/Ab_arm_ethernetdrv.h"
#include "include/Ab_arm_slicdrv.h"
#include "include/Ab_arm_dmadrv.h"
#include "include/Ab_arm_fifodrv.h"
#include "include/Ab_arm_codec.h"

//#define ENABLE_APLAY_WAV_FILE  1

//#define LINUX_APLAY_TEST       1
#define KERNEL_MUDULE_TEST     1



/*****************************************************************************/
/*Global Define Defenition static function 							    				 */
/*****************************************************************************/
static int Init_Net_Filter_HooK_IP();
static int Init_Net_Filter_Hook_ARP();


/*****************************************************************************/
/*Global Structure Defenition 							    				 */
/*****************************************************************************/
//// Структура для регистрации функции перехватчика входящих ip пакетов
struct nf_hook_ops bundle;
struct nf_hook_ops arp_bundle;

/**********************IP AND UDP DEFENITION******************/
//IP PRoTOCOL VALUE
#define ICMP 0x1
#define IPv4 0x4
#define TCP  0x6
#define UDP  0x11

#define BROADCAST_FRAME     0xFFFF
#define ETHERNET_HEADER_LEGTH  14 //14 bait  (112  bit)
#define IPv4_HEADER_LENGTH     20 //20 bait  (160  bit)
#define UDP_HEADER_LENGTH      8  //8 bait   (64   bit) 
#define RTP_HEADER_LENGTH      12 //12 bait  (96   bit)




//Порт назначения UDP 
int udp_dest_port_LE=0x1027;  //ПОРТ 10000 туда кидаю  данные ffmpeg 


#define RTP_SOURCE_TRAFFIC_LE   0x0182A8C0   //192.168.130.1  Little  Endian
#define RTP_SOURCE_TRAFFIC_BE   0xC0A88201   //192.168.130.1  Big     Endian         
#define SITARA_cpsw0_IP_ADDR    0xC0A9827C  //192.168.130.124
#define SITARA_cpsw1_IP_ADDR    0x0A000001  //10.0.0.1

/**************************************************************************************************
Syntax:      	    unsigned int Hook_Func_ARp
Parameters:     	
Remarks:			Process recieve ARP frame 
Return Value:	    0  =>  Success  ,-EINVAL => Failure
***************************************************************************************************/
unsigned int Hook_Func_ARP(uint hooknum,
                  struct sk_buff *skb,
                  const struct net_device *in,
                  const struct net_device *out,
                  int (*okfn)(struct sk_buff *))

{
//printk("+++++++++++++ARP+++++++++++++\n\r");
return NF_ACCEPT;	
}



/**************************************************************************************************
Syntax:      	    unsigned int Hook_Func
Parameters:     	
Remarks:			Process recieve IP frame 
Return Value:	    0  =>  Success  ,-EINVAL => Failure
***************************************************************************************************/
unsigned int Hook_Func(uint hooknum,
                  struct sk_buff *skb,
                  const struct net_device *in,
                  const struct net_device *out,
                  int (*okfn)(struct sk_buff *))
{

     int my_l_dest_udp_port=0;
	 static int rtp_count=0;
	 int i=0;
	
	
	 /* Указатель на структуру заголовка протокола eth в пакете */
	struct ethhdr *eth;
    /* Указатель на структуру заголовка протокола ip в пакете */
	struct iphdr *ip;

	//Фильтрация 2 го уровня по ETH заголовку
	eth=(struct ethhdr *)skb_mac_header(skb);
	 //Фильтрация 3 го уровня по IP
	ip = (struct iphdr *)skb_network_header(skb);

	//printk("++HOOK_IP|ip_saadr=0%x|ip_daadr=0x%x++++++\n\r",(uint)ip->saddr,(uint)ip->daddr);

	 /*Пустой UDP пакет отбрасываю*/
	 if((skb->mac_len+skb->len)==42) 
	 {
	 printk("bad input packet size =%d\n\r",skb->mac_len+skb->len);
	 return NF_DROP;
	 } 
	
	 //Фильтр пакетов к моему IP пока так сделаю;
	 //Источник RTP пакетов ко мне для обработки на порт назначения UDP  = 10000  тогда обрабатываю
	 if (((uint)ip->saddr==RTP_SOURCE_TRAFFIC_LE))
	 {
		  memcpy(&my_l_dest_udp_port,(skb->mac_header)+ETHERNET_HEADER_LEGTH+22,2);
		  if(my_l_dest_udp_port==udp_dest_port_LE)
		  {	 
			    //Пакет пришёл мне обрабатываю отправляю его дальше на обработку функция отправки пакета на обработку
			      get_rtp_prepare_packet_handler(skb->mac_header,(uint)skb->mac_len+(uint)skb->len);
			    //printk("udp_dest=0x%x\n\r",my_l_dest_udp_port);
			    //printk("++RTP_PACKET_SOURCE_OK++_num=%d|packet_size_bytes=%d\n\r",rtp_count++,(uint)skb->mac_len+(uint)skb->len);  			  
			    rtp_count++;
			    
		   return NF_DROP;   //сбрасываю пакет он никому больше не нужен кроме меня
		  }	  
		
	 }
	
return NF_ACCEPT;	
}	





/**************************************************************************************************
Syntax:      	    static void Init_Net_Filter_HooK_IP()
Parameters:     	
Remarks:			Process recieve ARP frame 
Return Value:	    0  =>  Success  ,-EINVAL => Failure
***************************************************************************************************/
static int Init_Net_Filter_HooK_IP()
{
   int status=0; 
	/* Заполняем структуру для регистрации hook функции */
    /* Указываем имя функции, которая будет обрабатывать пакеты */
       bundle.hook = Hook_Func;
    /* Устанавливаем указатель на модуль, создавший hook */
       bundle.owner = THIS_MODULE;
    /* Указываем семейство протоколов */
       bundle.pf = NFPROTO_IPV4;         
    /* Указываем, в каком месте будет срабатывать функция */
       bundle.hooknum =NF_INET_PRE_ROUTING;
    /* Выставляем самый высокий приоритет для функции */
       bundle.priority = NF_IP_PRI_FIRST;
    /* Регистрируем */
       status=nf_register_hook(&bundle);  
		
return 1;
}


/**************************************************************************************************
Syntax:      	    static void Init_Net_Filter_Hook_ARP()
Parameters:     	
Remarks:			Process recieve ARP frame 
Return Value:	    0  =>  Success  ,-EINVAL => Failure
***************************************************************************************************/
static int Init_Net_Filter_Hook_ARP()
{
 int status=0;
	
	////////////////////////////ARP_HOOK_FUNCTION/////////// 
    arp_bundle.hook =    Hook_Func_ARP;
    arp_bundle.owner=    THIS_MODULE;
    arp_bundle.pf   =    NFPROTO_ARP;
    arp_bundle.hooknum = NF_INET_PRE_ROUTING;
    arp_bundle.priority =NF_IP_PRI_FIRST;
    status=nf_register_hook(&arp_bundle);
	
return 1;
}




/**************************************************************************************************
Syntax:      	    int Ab_arm_init_module(void)
Parameters:     	none
Remarks:			load the kernel module driver - invoked by insmod. 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
int Ab_arm_init_module(void)
{
bool ret=0;
     

			
#ifdef KERNEL_MUDULE_TEST

            /*Initialization NEt_FILTER Kernel PAcket Recieve and Transmit*/
			ret=Init_Net_Filter_HooK_IP();
			ret=Init_Net_Filter_Hook_ARP();  	
			ret=Init_Arm_CPSW_MAC_Ethernet();    //Init ARM CPSW  Ethernet  Interface Driver
			if(ret==0){printk("?Error Init Ethernet Module?\n\r");}
			/*Initialize FIFO RTP BUFFER */
			Init_FIFO_voice_rtp_buf ();
    ///////////////////////////////////////////////////////////////////////////////////////////////////
            /*Init Low Level Hardware  Appart functon*/
            Init_Arm_AIC3106_low_level_codec_i2c();
            Init_Arm_McASP_interface();      
            /*Initialize AUdio Codecs for ARM processor */
            Init_audio_codecs ();
            /*Init and Start EDMA  Sitara Interface*/
            Init_Arm_EDMA_interface();   
#endif
          
          
          
 
#ifdef     LINUX_APLAY_TEST       
           /*Init Low Level Hardware  Appart functon*/
           Init_Arm_AIC3106_low_level_codec_i2c();
           Init_Arm_McASP_interface();      
           /*Initialize AUdio Codecs for ARM processor */
           Init_audio_codecs (); 
 
#endif                 
          
   
/*Start Testing FUnctions */       
//ret= Start_Test_Sitara_arm_func();
          
          
return 0;
}









/**************************************************************************************************
Syntax:      	    Ab_arm_cleanup_module(void)
Parameters:     	none
Remarks:			unload the kernel module driver - invoked by rmmod

Return Value:	    none

***************************************************************************************************/
void Ab_arm_cleanup_module(void)
{	
//  printk("Ab_arm_exit_module() I-TDM called\n");

    //Ethernet Clear block	
	
#ifdef KERNEL_MUDULE_TEST
	   nf_unregister_hook(&bundle);      
       nf_unregister_hook(&arp_bundle); 
	
       Clear_Arm_EDMA_interface();
       Clear_FIFO_voice_rtp_buf();
#endif	
	

       
#ifdef   LINUX_APLAY_TEST 
       
       
#endif       
       
       
       
       
printk("!OK_CLEAR ALL DATA !\n\r") ;
    
}

/*****************************************************************************/
/*	PUBLIC FUNCTION DEFINITIONS					     */
/*****************************************************************************/


/*****************************************************************************/
/*	MODULE DESCRIPTION				     */
/*****************************************************************************/
module_init(Ab_arm_init_module);
module_exit(Ab_arm_cleanup_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Konstantin Shiluaev <k_sjiluaev@supertel.spb.su>");
MODULE_DESCRIPTION("Test ARM SITARA 3352 I-TDM driver");






















