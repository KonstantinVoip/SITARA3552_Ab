/**********************************************************************************************************************
*                                        (c) COPYRIGHT by LO ZNIIS.                                             *
*                                            All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : Ab_arm_codec.c
*
* Description : Sitara 3352 linux driver kernel module fo ARM abonent Codec G.711 pcm A-LAW or MU-law  or PCM
*
* Author      : Konstantin Shiluaev.
*
******************************************************************************/



/*****************************************************************************/
/*	INCLUDES							     */
/*****************************************************************************/
#include "include/Ab_arm_codec.h"
#include "include/Ab_arm_TestBuf.h"

/*****************************************************************************/
/*	GLOBAL CONFIGURATION						     */
/*****************************************************************************/
#define	BIAS		(0x84)		/* Bias for linear code. */
#define CLIP         8159


/*****************************************************************************/
/*	GLOBAL CONFIGURATION						     */
/*****************************************************************************/
short out_aPCM[16024];



/*****************************************************************************/
/*	PRIVATE STATIC FUNCTION PROTOTYPES	FOR CODECs G.711				     */
/*****************************************************************************/
static int search(int val, int * table, int	size);	
static int linear2alaw(int	pcm_val) ;
static int alaw2linear(int	a_val);	
static int ulaw2linear( int u_val);
static int linear2ulaw(int pcm_val);



int  Init_audio_codecs ()
{
    printk("START_AUDIO_CODEC_TEST\n\r");
	int i=0;
	//int l_i=200;
 
/*	
	for(i=0;i<8012;i++)
	{	
	  out_aPCM[i]=ulaw2linear(test_sinus_g711_uLAW[i]);
	}
  
*/  
	//Вроде Похоже на правду Декодер G.711 работает у нас    
	
	
	/*
	for(i=0;i<=l_i;i++)
	{
			// printk("{%d|0x%x}-",i,stereo_voice_buffer[i]); 
			 printk("{%d|0x%x}-",i,out_aPCM[i]);

    }
	*/
		
}
short g711_ulaw_decoder(short in_size,short *output_pcm_buf,const unsigned char *input_ulaw_buffer)
//short g711_ulaw_decoder(const unsigned char *input_ulaw_buffer,short *output_pcm_buf  ,short in_size)
{
    short l_in_size_byte=0;
    short i=0;
    
    //Нечего Декодировать нам

    
    
    if(in_size==0)
    {
     printk("NO DATA to  G711_ULAW_DECODE_to_PCM\n\r"); 
     return 0;
    }
    
    l_in_size_byte=in_size;
    printk("+g711_ulaw_decoder/in_size=%d+\n\r",l_in_size_byte);
    
    /*   
	for(i=0;i<l_in_size_byte;i++)
	{	
		output_pcm_buf[i]=ulaw2linear(input_ulaw_buffer[i]);
	}
*/	
	
	
return l_in_size_byte;
	
}
















/*****************************************************************************
Syntax:      	    int linear2alaw(int	pcm_val)  		 
Return Value:	    Returns 1 on success and negative value on failure.
 					Value		 									Description
					--------------------------------------------------------------
					= 1												Success
					=-1												Failure
*******************************************************************************/
static int  search(int	val,int *table,int size)
{
	int		i;		// changed from "short" *drago

	for (i = 0; i < size; i++) 
	{
		if (val <= *table++)
			return (i);
	}
	return (size);
}



/*****************************************************************************
Syntax:      	    int linear2alaw(int	pcm_val)  
Remarks:			 
Return Value:	    Returns 1 on success and negative value on failure.
 					Value		 									Description
					--------------------------------------------------------------
					= 1												Success
					=-1												Failure
*******************************************************************************/
static int linear2alaw(int	pcm_val)            // 2's complement (16-bit range) 
                                        // changed from "short" *drago
{
	int		mask;	// changed from "short" *drago
	int		seg;	// changed from "short" *drago
	int		aval;

	pcm_val = pcm_val >> 3;

	if (pcm_val >= 0) 
	{
		mask = 0xD5;		// sign (7th) bit = 1
	} 
	else 
	{
		mask = 0x55;		// sign bit = 0 
		pcm_val = -pcm_val - 1;
	}

	// Convert the scaled magnitude to segment number.
	seg = search(pcm_val, seg_aend, 8);

	// Combine the sign, segment, and quantization bits. 

	if (seg >= 8)		// out of range, return maximum value.
		return (0x7F ^ mask);
	else {
		aval = seg << SEG_SHIFT;
		if (seg < 2)
			aval |= (pcm_val >> 1) & QUANT_MASK;
		else
			aval |= (pcm_val >> seg) & QUANT_MASK;
		return (aval ^ mask);
	}
}

/*****************************************************************************
Syntax:      	alaw2linear() - Convert an A-law value to 16-bit linear PCM    
Remarks:			
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				--------------------------------------------------------------
				= 1												Success
				=-1												Failure
*******************************************************************************/
static int alaw2linear(int	a_val)		
{
	int		t;      // changed from "short" *drago 
	int		seg;    // changed from "short" *drago

	a_val ^= 0x55;

	t = (a_val & QUANT_MASK) << 4;
	seg = ((unsigned)a_val & SEG_MASK) >> SEG_SHIFT;
	switch (seg) {
	case 0:
		t += 8;
		break;
	case 1:
		t += 0x108;
		break;
	default:
		t += 0x108;
		t <<= seg - 1;
	}
	return ((a_val & SIGN_BIT) ? t : -t);
}


/**************************************************************************************************
Syntax:      	    int linear2ulaw( int	pcm_val) 	 
Return Value:	    Returns 1 on success and negative value on failure.
 				    Value		 									Description
				   -------------------------------------------------------------------------------------
				   = 1												Success
				   =-1												Failure
***************************************************************************************************/
static int linear2ulaw(int pcm_val)	// 2's complement (16-bit range) 
{
	int		mask;
	int		seg;
	int		uval;

	// Get the sign and the magnitude of the value. 
	pcm_val = pcm_val >> 2;
	if (pcm_val < 0) {
		pcm_val = -pcm_val;
		mask = 0x7F;
	} else {
		mask = 0xFF;
	}
        if ( pcm_val > CLIP ) pcm_val = CLIP;		// clip the magnitude 
	pcm_val += (BIAS >> 2);

	// Convert the scaled magnitude to segment number. 
	seg = search(pcm_val, seg_uend, 8);

	
	 // Combine the sign, segment, quantization bits;
	 // and complement the code word.
	 //
	if (seg >= 8)		// out of range, return maximum value. 
		return (0x7F ^ mask);
	else {
		uval = (seg << 4) | ((pcm_val >> (seg + 1)) & 0xF);
		return (uval ^ mask);
	}

}

/**************************************************************************************************
Syntax:      	    ulaw2linear() - Convert a u-law value to 16-bit linear PCM 
Return Value:	    Returns 1 on success and negative value on failure.
 				    Value		 									Description
				   -------------------------------------------------------------------------------------
				   = 1												Success
				   =-1												Failure
***************************************************************************************************/
static int ulaw2linear( int u_val)
{
	int t;
	// Complement to obtain normal u-law value. 
	u_val = ~u_val;
	// Extract and bias the quantization bits. Then
	// shift up by the segment number and subtract out the bias.
	t = ((u_val & QUANT_MASK) << 3) + BIAS;
	t <<= (u_val & SEG_MASK) >> SEG_SHIFT;
	return ((u_val & SIGN_BIT) ? (BIAS - t) : (t - BIAS));
}













