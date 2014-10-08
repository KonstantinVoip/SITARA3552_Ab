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
unsigned short voice_buf_get_data_in_rtp_stream1 (unsigned char *in_buf , int *in_size);
unsigned short voice_buf_set_data_in_rtp_stream1 (const unsigned char *in_buf ,const int in_size);

/*Функции  которые смотрят из TDM потоков здесь я раздеди потомучто нам нужно из пакетов получить сплошной массив данных подсунуть их в DMA*/
bool voice_buf_get_data_in_tdm_stream1 (unsigned char *in_buf , int *in_size);
void voice_buf_set_data_in_tdm_stream1 (const unsigned char *in_buf ,const int in_size);



/*****************************************************************************/
/*			                 STRUCTURES										 */
/******************************************************************************/

#endif /* ABARMFIFODRV_H */

