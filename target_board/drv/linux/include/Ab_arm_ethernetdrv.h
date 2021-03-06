/**********************************************************************************************************************
*                                        (c) COPYRIGHT by LONIIS.                                             *
*                                        All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : Ab_arm_ethernetdrv.h
*
* Description : definitions and declarations ARM  Ethernet MODULE
*
* Author      : Konstantin Shiluaev
*
******************************************************************************
******************************************************************************/

/*-----------------------------------------------------------------------------
 GENERAL NOTES
-----------------------------------------------------------------------------*/
#ifndef ABARMETHERNETDRV_H
#define ABARMETHERNETDRV_H

#include <linux/kernel.h> // Needed for KERN_ALERT
/*****************************************************************************/
/*	PUBLIC GLOBALS							   							     */
/*****************************************************************************/
//Start Ethernet Initialization
bool Init_Arm_CPSW_MAC_Ethernet();
bool Init_Arm_PHY_Transiver();

//Start Transmit  Function
void send_packet_buf_to_cpsw0(u16 buf[758],u16 len);
void send_packet_buf_to_cpsw1(u16 buf[758],u16 len);
//GET packet form  ethernet  Device
int get_rtp_prepare_packet_handler(unsigned char *packet,int packet_len);








/*****************************************************************************/
/*	COMMON CONFIGURATION						     */
/*****************************************************************************/



/*****************************************************************************/
/*	PUBLIC MACROS							     */
/*****************************************************************************/



/*****************************************************************************/
/*	PUBLIC DATA TYPES						     */
/*****************************************************************************/


/*****************************************************************************/
/*	PUBLIC FUNCTION PROTOTYPES					     */
/*****************************************************************************/


#endif /* MPCDRVMAIN_H */

