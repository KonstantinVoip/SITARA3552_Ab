/**********************************************************************************************************************
*                                        (c) COPYRIGHT by LO ZNIIS.                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : Ab_arm_fifodrv.h
*
* Description : definitions and declarations for ARM FIFO DRV driver kernel module 
*
* Author      : Konstantin Shiluaev
*
******************************************************************************
******************************************************************************
*
* Module's Description Record:
* ============================
*
* $ProjectRevision: 0.0.1 $
* $Source: 
* $State: Debug$
* $Revision: 0.0.1 $ $Name: $
* $Date: 2014/09/21 10:40:51 $
* $Locker: $
*
* Module's revision history:
* ==========================
* --------- $Log: mpcdrv.h $
* --------- Initial revision
******************************************************************************/

/*-----------------------------------------------------------------------------
 GENERAL NOTES
-----------------------------------------------------------------------------*/
#ifndef ABARMFIFODRV_H
#define ABARMFIFODRV_H



#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/log2.h>


/*****************************************************************************/
/*	                 EXTERN FUNCTIONS AND DATA TYPES						 */
/******************************************************************************/
void Init_FIFO_voice_rtp_buf (); //Init and Clear FIFO buffer
void Clear_FIFO_voice_rtp_buf(); //Clear FIFO buffer


/*Функции для входных пакетов FIFO из сети получаем и накапливаем массив*/
bool voice_buf_get_data_in_rtp_stream1 (unsigned char *in_buf , int *in_size);
void voice_buf_set_data_in_rtp_stream1 (const unsigned char *in_buf ,const int in_size);

/*Функции  которые смотрят из TDM потоков здесь я раздеди потомучто нам нужно из пакетов получить сплошной массив данных подсунуть их в DMA*/
bool voice_buf_get_data_in_tdm_stream1 (unsigned char *in_buf , int *in_size);
void voice_buf_set_data_in_tdm_stream1 (const unsigned char *in_buf ,const int in_size);



/*****************************************************************************/
/*			                 STRUCTURES										 */
/******************************************************************************/
typedef struct description_packet
{
 int size;	
 int rtp_payload_size;
 unsigned char data[1460];
 
}DATA_lbc;

struct mpcfifo {
	DATA_lbc      q[50];
	unsigned int  head;
	unsigned int  tail;
    int           N;
    int           cur_put_packet_size;
    int           cur_get_packet_size;
    int           fifo_pusto;
    int           fifo_zapolneno;
    int           all_rtp_paload_data_size; 
    
    spinlock_t    *lock;
};












#endif /* ABARMFIFODRV_H */

