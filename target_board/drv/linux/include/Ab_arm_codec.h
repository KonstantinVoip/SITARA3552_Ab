/**********************************************************************************************************************
*                                        (c) COPYRIGHT by LO ZNIIS.                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : Ab_arm_codec.h
*
* Description : definitions and declarations for ARM FIFO Voice Codec G.711 A-law and Mu-law   or PCM .
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
#ifndef ABARMCODECDRV_H
#define ABARMCODECDRV_H


#include <linux/kernel.h>
#include <linux/module.h>


/*****************************************************************************/
/*	                 EXTERN FUNCTIONS AND DATA TYPES						 */
/******************************************************************************/
int  Init_audio_codecs (); //Init and Clear FIFO buffer
int  Clear_audio_codec(); //Clear FIFO buffer

/*CODER  and  Encode FUnctionS  TO DO Features NEXT */
//short g711_alaw_encoder(const unsigned char *input_buf,short *output_buffer,size);
//short g711_alaw_decoder(const unsigned char *input_buf,short *output_buffer,short size);


short g711_ulaw_encoder(const unsigned char *input_pcm_buf    ,short *output_ulaw_buffer,short in_size);
//short g711_ulaw_decoder(const unsigned char *input_ulaw_buffer,short *output_pcm_buf    ,short in_size);

short g711_ulaw_decoder(short in_size,short *output_ulaw_buffer,const unsigned char *input_ulaw_buffer);


/*****************************************************************************/
/*	                 EXTERN FUNCTIONS AND DATA TYPES						 */
/******************************************************************************/
#define	SIGN_BIT	(0x80)		/* Sign bit for a A-law byte. */
#define	QUANT_MASK	(0xf)		/* Quantization field mask. */
#define	NSEGS		(8)		    /* Number of A-law segments. */
#define	SEG_SHIFT	(4)		    /* Left shift for segment number. */
#define	SEG_MASK	(0x70)		/* Segment field mask. */



/*****************************************************************************/
/*			                 STRUCTURES										 */
/******************************************************************************/
static int seg_aend[8] = {0x1F, 0x3F, 0x7F, 0xFF,0x1FF, 0x3FF, 0x7FF, 0xFFF};
static int seg_uend[8] = {0x3F, 0x7F, 0xFF, 0x1FF,0x3FF, 0x7FF, 0xFFF, 0x1FFF};

/* u- to A-law conversions */
/* copy from CCITT G.711 specifications */
static unsigned char u2a[128] = {			       
	1,	1,	2,	2,	3,	3,	4,	4,
	5,	5,	6,	6,	7,	7,	8,	8,
	9,	10,	11,	12,	13,	14,	15,	16,
	17,	18,	19,	20,	21,	22,	23,	24,
	25,	27,	29,	31,	33,	34,	35,	36,
	37,	38,	39,	40,	41,	42,	43,	44,
	46,	48,	49,	50,	51,	52,	53,	54,
	55,	56,	57,	58,	59,	60,	61,	62,
	64,	65,	66,	67,	68,	69,	70,	71,
	72,	73,	74,	75,	76,	77,	78,	79,
/* corrected:
	81,	82,	83,	84,	85,	86,	87,	88, 
   should be: */
	80,	82,	83,	84,	85,	86,	87,	88,
	89,	90,	91,	92,	93,	94,	95,	96,
	97,	98,	99,	100,	101,	102,	103,	104,
	105,	106,	107,	108,	109,	110,	111,	112,
	113,	114,	115,	116,	117,	118,	119,	120,
	121,	122,	123,	124,	125,	126,	127,	128};

/* A- to u-law conversions */
static unsigned char a2u[128] = {			  
	1,	3,	5,	7,	9,	11,	13,	15,
	16,	17,	18,	19,	20,	21,	22,	23,
	24,	25,	26,	27,	28,	29,	30,	31,
	32,	32,	33,	33,	34,	34,	35,	35,
	36,	37,	38,	39,	40,	41,	42,	43,
	44,	45,	46,	47,	48,	48,	49,	49,
	50,	51,	52,	53,	54,	55,	56,	57,
	58,	59,	60,	61,	62,	63,	64,	64,
	65,	66,	67,	68,	69,	70,	71,	72,
/* corrected:
	73,	74,	75,	76,	77,	78,	79,	79,
   should be: */
	73,	74,	75,	76,	77,	78,	79,	80,

	80,	81,	82,	83,	84,	85,	86,	87,
	88,	89,	90,	91,	92,	93,	94,	95,
	96,	97,	98,	99,	100,	101,	102,	103,
	104,	105,	106,	107,	108,	109,	110,	111,
	112,	113,	114,	115,	116,	117,	118,	119,
	120,	121,	122,	123,	124,	125,	126,	127};



#endif /* ABARMFIFODRV_H */

