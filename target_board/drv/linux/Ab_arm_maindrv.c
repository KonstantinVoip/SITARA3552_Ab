/**********************************************************************************************************************
*                                        (c) COPYRIGHT by LONIIS.                                             *
*                                            All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : Ab_arm_maindrv.c
*
* Description : Sitara 3352 linux driver kernel module init 
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

    /* Указатель на структуру заголовка протокола eth в пакете */
	struct ethhdr *eth;
    /* Указатель на структуру заголовка протокола ip в пакете */
	struct iphdr *ip;
	/*Указатель на UDP заголовок*/
	//struct udphdr *udph;
		/*указатель на icmp сообщение*/
	//struct icmphdr *icmp;
	
	
	//Фильтрация 2 го уровня по ETH заголовку
	eth=(struct ethhdr *)skb_mac_header(skb);
	 //Фильтрация 3 го уровня по IP
	ip = (struct iphdr *)skb_network_header(skb);

	//printk("+++++++++++++IP++++++++++=%x\n\r",(uint)ip->saddr);
	//Фильтр для пакетов от НМС3 и к НМС3  нужн подумать как ручками не прописывать может лучше сделать порт 18000;
	
		
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
       bundle.hooknum = NF_INET_PRE_ROUTING;
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
	

   /*Initialization NEt_FILTER Kernel PAcket Recieve and Transmit*/



    ret=Init_Net_Filter_HooK_IP();
    ret=Init_Net_Filter_Hook_ARP();  	
    

    ret=Init_Arm_CPSW_MAC_Ethernet();    //Init ARM CPSW  Ethernet  Interface Driver
    if(ret==0)
    {  	
    printk("?Error Init Ethernet Module?\n\r");	
    }
    

    
    
    
    
    /*Start Testing Function for ARM  processor*/
    //Start_Test_Sitara_arm_func();
    
    
    
    
    //ret=Init_Arm_McASP_interface();
    //ret=Init_Arm_AIC3106_low_level_codec_i2c();
   
    
    printk("!!!Ab_arm_init_module_I-tdm() Start_OK++!!!\n");
    
    
   
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
    printk("Ab_arm_exit_module() I-TDM called\n");
    nf_unregister_hook(&bundle);      
    nf_unregister_hook(&arp_bundle);
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






















