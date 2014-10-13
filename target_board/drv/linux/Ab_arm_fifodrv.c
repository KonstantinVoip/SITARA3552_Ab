/**********************************************************************************************************************
*                                        (c) COPYRIGHT by LO ZNIIS.                                             *
*                                            All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : Ab_arm_fifodrv.c
*
* Description : Sitara 3352 linux driver kernel module for FIFO Packet BUFFER  input
*             : В данном случаее  нужна байтовая очередь на входе и выходе нужно кормить кодек данными с определённой
*             : скоростью для g.711 m-law 30 мс 
*
* Author      : Konstantin Shiluaev..
*
******************************************************************************/

/*****************************************************************************/
/*	INCLUDES							     */
/*****************************************************************************/
#include <linux/mutex.h>
#include <linux/kfifo.h>
#include <linux/proc_fs.h>


#include "include/Ab_arm_fifodrv.h"






//#include <asm/page.h>


/*****************************************************************************/
/*	GLOBAL CONFIGURATION	DEFINE for BUFFER 					     		*/
/*****************************************************************************/
#define GFP_MASK               GFP_KERNEL
/* fifo size in elements (bytes) */
#define FIFO_SIZE	/*32*/ /*4096*/ 8192
/* lock for procfs read access */
static DEFINE_MUTEX(read_lock);
/* lock for procfs write access */
static DEFINE_MUTEX(write_lock);


/*
 * define DYNAMIC in this example for a dynamically allocated fifo.
 *
 * Otherwise the fifo storage will be a part of the fifo structure.
 */
//#if 0
#define DYNAMIC
//#endif

#ifdef DYNAMIC
static struct kfifo test;
#else
static DECLARE_KFIFO(test, unsigned char, FIFO_SIZE);
#endif





/*****************************************************************************/
/*	PRIVATE FUNCTION PROTOTYPES					     */
/*****************************************************************************/




/*****************************************************************************
Syntax:      	    Init_FIFO_voice_rtp_buf ();
Remarks:			This Function Init FIFO buffers 
Return Value:	    Returns 1 on success and negative value on failure.
 					Value		 									Description
					--------------------------------------------------------------
					= 1												Success
					=-1												Failure
*******************************************************************************/
void Init_FIFO_voice_rtp_buf ()
{	
 #ifdef DYNAMIC
	int ret;
	ret = kfifo_alloc(&test, FIFO_SIZE, GFP_KERNEL);
	if (ret) 
	{
	printk("?error kfifo_alloc?\n");
	return ret;
	}
	#else
	INIT_KFIFO(test);
#endif

	
	/*
	if (testfunc() < 0) 
	{
	#ifdef DYNAMIC
	kfifo_free(&test);
	#endif
    */
return -EIO;
}




////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////SET DATA PACKET TO FIFO BUFFER EXTERN FUNCTION/////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
/*****************************************************************************
Syntax:      	    void voice_buf_set_data_in_rtp_stream1 
Remarks:			set data on input RTP packet to FIFO  buffer
*******************************************************************************/
unsigned short voice_buf_set_data_in_rtp_stream1 (const unsigned char *in_buf ,const int in_size)
{
unsigned short i=0;
	 

    if(kfifo_is_full(&test)==1)
    {
    printk("++++FIFO is FULL bytes_clear_data+++++\n\r");
    return 0;
    }
    else
    {	
      
    	//put values into the fifo 
    	for (i = 0; i != in_size; i++)
    	{
		kfifo_put(&test, &in_buf[i]);
	  //printk("%x|",ret);
    	}
    return i;
    }
	
}

////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////GET DATA PACKET FROM FIFO BUFFER _EXTERN FUNCTION for ///////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
/*****************************************************************************
Syntax:      	    void voice_buf_set_data_in_rtp_stream1 
Remarks:			set data on input RTP packet to FIFO  buffer
*******************************************************************************/
unsigned short voice_buf_get_data_in_rtp_stream1 (unsigned char *in_buf , int *in_size)
{
unsigned short j=0;
//signed short empty;
signed short state=0;



//	empty=kfifo_is_empty(&test);
//	printk(KERN_INFO "emty=%d\n\r",empty);


     if(kfifo_is_empty(&test)==1)
     {
    	 //printk("++++FIFO is EMPTY++++\n\r");
    	 return 0;//FIFO is EMPTY
     }
     else
     {	 

    	 //get values into the fifo берём 4000 и если и сможем 
    	   for (j = 0; j != in_size; j++)
    	   {
    	       state=kfifo_get(&test, &in_buf[j]);
    	       if(state==0)
    	       {
    	    	   return j;
    	       }
    	     		 //Добавлю ещё кусок работы по подсчёту раземера колчества  байтов которые мы полуаем	 
    	 	         //printk("{%d|%d}",state,j);
    	 	}
    	     	 
     return j;
     }
	
}





/*****************************************************************************
Syntax:      	    Clear_FIFO_voice_rtp_buf()
Remarks:			This Function Clear FIFO buffer for fifo_put_to_tdm0_dir 
Return Value:	    Returns 1 on success and negative value on failure.
 				Value		 									Description
				--------------------------------------------------------------
				= 1												Success
				=-1												Failure
*******************************************************************************/
void Clear_FIFO_voice_rtp_buf()
{
  kfifo_free(&test);
  printk("+free_fifo_buffer_OK+\n\r");	
}

















