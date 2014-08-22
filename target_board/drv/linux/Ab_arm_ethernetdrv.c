/**********************************************************************************************************************
*                                        (c) COPYRIGHT by LONIIS.                                             *
*                                            All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : Ab_arm_ethernetdrv.c
*
* Description : Sitara 3352 linux driver for ARM Ethernet device
*
* Author      : Konstantin Shiluaev..
*
*******************************************************************************/

/*-----------------------------------------------------------------------------
GENERAL NOTES
1.This Module function Initialization ethernet structure for MAC  level
2.Initialize function  for PHY transivers (in  future)
3.Transmit packet to Ethernet
-----------------------------------------------------------------------------*/


/*****************************************************************************/
/* Linux INCLUDES							     */
/*****************************************************************************/
#include <linux/kernel.h>
#include <linux/string.h>    //memset  function

///INET//
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/if_arp.h>
#include <linux/if_vlan.h>
#include <linux/in.h>
#include <linux/ip.h>



#include <linux/cpsw.h>
/*****************************************************************************/
/* Local INCLUDES							     */
/*****************************************************************************/
#include "include/Ab_arm_ethernetdrv.h"
#include "include/Ab_arm_TestBuf.h"


/*****************************************************************************/
/*Global Static Function  Defenition 							    		 */
/*****************************************************************************/
static struct net_device *cpsw_get_device_by_name(const char *ifname);


/*****************************************************************************/
/*Global Structure Defenition 							    				 */
/*****************************************************************************/
struct net_device *cpsw0_dev,*cpsw1_dev;





/**************************************************************************************************
Syntax:      	    bool Init_Arm_CPSW_MAC_Ethernet()
Parameters:     	none
Remarks:			Init  MAC level for ARM Sitara AM3352 Processor.
Return Value:	    1  =>  Success  ,0 => Failure
***************************************************************************************************/
bool Init_Arm_CPSW_MAC_Ethernet()
{
 
	int i=0;
	
	printk("+Init_Arm_CPSW_MAC_Ethernet()+\n\r");	
	const char *ifname0="eth0";
	const char *ifname1="eth1";
	
	memset(&cpsw0_dev, 0x0000, sizeof(cpsw0_dev));  
	memset(&cpsw1_dev, 0x0000, sizeof(cpsw1_dev)); 
	
	
	cpsw0_dev=cpsw_get_device_by_name(ifname0);
	if(!cpsw0_dev){printk("No Device Found %s\n\r",ifname0);}

	cpsw1_dev=cpsw_get_device_by_name(ifname1);
	if(!cpsw1_dev){printk("No Device Found %s\n\r",ifname1);}

	
	//Conflict IP_ADDR_in  seti?
	
	
	
	/*
	for(i=0;i<=10;i++)
	{
	printk("packet_to_send=%d\n\r",i);
	send_packet_buf_to_cpsw0(pack_mas,1514);
	}*/
	
	
   
return 1;	
}



/**************************************************************************************************
Syntax:      	    bool Init_Arm_PHY_Transiver()
Parameters:     	none
Remarks:			Init  PHY Transiver.
Return Value:	    1  =>  Success  ,0 => Failure
***************************************************************************************************/
bool Init_Arm_PHY_Transiver()
{
	
	printk("+Init_Arm_PHY_Transiver()+\n\r");		
	
	
return 1;	
}




/**************************************************************************************************
Syntax:      	    void send_packet_buf_to_cpsw0(u16 buf[758],u16 len)
Parameters:     	none
Remarks:			Init  MAC level for ARM Sitara AM3352 Processor.
Return Value:	    1  =>  Success  ,0 => Failure
***************************************************************************************************/
void send_packet_buf_to_cpsw0(u16 buf[758],u16 len)
{
	struct cpsw_priv *priv = netdev_priv(cpsw0_dev);
	
	
	struct sk_buff *skb=NULL;
	__u8 *eth;
	__be16 protocol = htons(ETH_P_IP);
	netdev_tx_t (*xmit)(struct sk_buff *, struct net_device *)= cpsw0_dev->netdev_ops->ndo_start_xmit;	
	u16 ret;
	skb = netdev_alloc_skb(cpsw0_dev, len);
	if (!skb) 
	{
	 cpsw0_dev->stats.rx_dropped++;
	 //priv->extra_stats.rx_skbmissing++;
	 printk("?cpsw0_dev_cannot allocate Ethernt SKB buffer?\n\r");
	 return;
	}
	
	//copy received packet to skb buffer 
	memcpy(skb->data, buf, len);
	//Reserve for ethernet and IP header 
	eth = (__u8 *) skb_push(skb, 14);
	memcpy(eth, buf, 12);
	*(__be16 *) & eth[12] = protocol;
	
	skb_put(skb, len);
	// Tell the skb what kind of packet this is 
	skb->protocol = eth_type_trans(skb, cpsw0_dev);
	  //////////////////Create and Fill packet to Send///////////////
    ret = (*xmit)(skb, cpsw0_dev);
	
    /* Значения ret возвращаемые функцией
	 * #define NETDEV_TX_OK 0           driver took care of packet 
     * #define NETDEV_TX_BUSY 1         driver tx path was busy
     * #define NETDEV_TX_LOCKED -1      driver tx lock was already taken
	*/
	if(ret==0)
	{
		  
	}
	else
	{
		  printk("?cpsw0_dev_transmit_failed,ret=%d\n\r?\n\r",ret);
	}
    
	if (NET_RX_DROP == ret) 
    {
	//priv->stats.tx_dropped++;
    } 
	
	else
	{
    // Increment the number of packets 
    cpsw0_dev->stats.tx_packets++;cpsw0_dev->stats.tx_bytes += len;
	}
    
    

}


/**************************************************************************************************
Syntax:      	    void send_packet_buf_to_cpsw1(u16 buf[758],u16 len)
Parameters:     	massive ,lenght  in byte
Remarks:			Init  MAC level for ARM Sitara AM3352 Processor.
Return Value:	    1  =>  Success  ,0 => Failure
***************************************************************************************************/
void send_packet_buf_to_cpsw1(u16 buf[758],u16 len)
{

	struct cpsw_priv *priv = netdev_priv(cpsw1_dev);
	
	
	struct sk_buff *skb=NULL;
	__u8 *eth;
	__be16 protocol = htons(ETH_P_IP);
	netdev_tx_t (*xmit)(struct sk_buff *, struct net_device *)= cpsw1_dev->netdev_ops->ndo_start_xmit;	
	u16 ret;
	skb = netdev_alloc_skb(cpsw1_dev, len);
	if (!skb) 
	{
	 //cpsw0_dev->stats.rx_dropped++;
	 //priv->extra_stats.rx_skbmissing++;
	 printk("?cpsw1_dev_cannot allocate Ethernt SKB buffer?\n\r");
	 return;
	}
	
	//copy received packet to skb buffer 
	memcpy(skb->data, buf, len);
	//Reserve for ethernet and IP header 
	eth = (__u8 *) skb_push(skb, 14);
	memcpy(eth, buf, 12);
	*(__be16 *) & eth[12] = protocol;
	
	skb_put(skb, len);
	// Tell the skb what kind of packet this is 
	skb->protocol = eth_type_trans(skb, cpsw1_dev);
	  //////////////////Create and Fill packet to Send///////////////
    ret = (*xmit)(skb, cpsw1_dev);
    /* Значения ret возвращаемые функцией
	 * #define NETDEV_TX_OK 0           driver took care of packet 
     * #define NETDEV_TX_BUSY 1         driver tx path was busy
     * #define NETDEV_TX_LOCKED -1      driver tx lock was already taken
	*/
	if(ret==0){}
	else{printk("?cpsw1_dev_transmit_failed,ret=%d\n\r?\n\r",ret);}
    
	if (NET_RX_DROP == ret) 
    {
	//priv->stats.tx_dropped++;
    } 
	
	else{// Increment the number of packets 
    cpsw1_dev->stats.tx_packets++;cpsw1_dev->stats.tx_bytes += len;
	}
	
}





/**************************************************************************************************
Syntax:      	    static struct net_device *cpsw_get_device_by_name(const char *ifname)
Parameters:     	name for ethernet device ifname
Remarks:			Init  MAC level for ARM Sitara AM3352 Processor.
Return Value:	    1  =>  Success  ,0 => Failure
***************************************************************************************************/
static struct net_device *cpsw_get_device_by_name(const char *ifname)
{
	char b[IFNAMSIZ+5];
	int i = 0;	
	for (i = 0; ifname[i] != '@'; i++) 
	{
		if (i == IFNAMSIZ)
			break;
		b[i] = ifname[i];
	}
	b[i] = 0;
	return dev_get_by_name(&init_net, b);	

}










