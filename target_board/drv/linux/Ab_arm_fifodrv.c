/**********************************************************************************************************************
*                                        (c) COPYRIGHT by LO ZNIIS.                                             *
*                                            All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : Ab_arm_fifodrv.c
*
* Description : Sitara 3352 linux driver kernel module for FIFO Packet BUFFER
*
* Author      : Konstantin Shiluaev..
*
******************************************************************************/
#define GFP_MASK               GFP_KERNEL
//#define FIFO_PACKET_SIZE_BYTE  1514          
//#define FIFO_PACKET_NUM        32
//#define FIFO_PACKET_SIZE_BYTE  1460          
//#define FIFO_PACKET_NUM        50

/***Голосовые Пакеты***/
//#define FIFO_PACKET_SIZE_BYTE  4000          
//#define FIFO_PACKET_NUM        16

#define FIFO_PACKET_SIZE_BYTE  4380          
#define FIFO_PACKET_NUM        4


/*****************************************************************************/
/*	INCLUDES							     */
/*****************************************************************************/
#include "include/Ab_arm_fifodrv.h"



/*****************************************************************************/
/*	GLOBAL CONFIGURATION						     */
/*****************************************************************************/

struct mpcfifo *rtp_input_fifo_stream1;
struct mpcfifo *mpcfifo_init(unsigned int obj_size,unsigned int obj_num , gfp_t gfp_mask, spinlock_t *lock);

/*****************************************************************************/
/*	PRIVATE FUNCTION PROTOTYPES					     */
/*****************************************************************************/
static unsigned int mpcfifo_put(struct mpcfifo *rbd_p,const unsigned char *buf);  //static local get  function
static void mpcfifo_get(struct mpcfifo *rbd_p, void *obj);                        //static local set  functions
static bool mpcfifo_get_buf_ready(struct mpcfifo *rbd_p);                         //static local ready function







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
	//Очищаем очередь перед использованием
	spinlock_t my_lock_rtp_input_fifo_stream1;
	memset(&rtp_input_fifo_stream1 , 0x0000, sizeof(rtp_input_fifo_stream1));
	
    //INIT FIFO DEIRECTION0 PUT to direction 0
	rtp_input_fifo_stream1=mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock_rtp_input_fifo_stream1);
    if(!rtp_input_fifo_stream1) {printk("???Transmit FIFO DIRECTION_0 Initialization Failed???\n\r");}	
    printk("+FIFO_1_TRANSMIT_INIT_OK+\n\r");
	
}

/*****************************************************************************
Syntax:      	    struct mpcfifo *mpcfifo_init(unsigned int obj_size,unsigned int obj_num , gfp_t gfp_mask, spinlock_t *loc
Remarks:			This Function Init FIFO buffers 
Return Value:	    Returns 1 on success and negative value on failure.
 				Value		 									Description
				--------------------------------------------------------------
				= 1												Success
				=-1												Failure
*******************************************************************************/
struct mpcfifo *mpcfifo_init(unsigned int obj_size,unsigned int obj_num , gfp_t gfp_mask, spinlock_t *lock)
{
	struct mpcfifo *ret;

    /////////////////Initialize structure mpcfifo buffer	
	ret = kmalloc(sizeof(struct mpcfifo), GFP_MASK);
	if (!ret)
	{	
		return(NULL);
	}
	
	//Конец очереди(tail) смотрим на  0
	ret->tail=0;
	
	//Длинна очереди начиная от   0<->4  сколько элементов получаеться
	ret->N=obj_num; //потом разберусь
	
	//Голова очереди(head) 4
	 // ret->head=ret->N;
	ret->head=0;//ret->N
	
	
	ret->cur_put_packet_size=0;
	ret->cur_get_packet_size=0;

	ret->lock = lock;
	ret->fifo_pusto=0;
	ret->fifo_zapolneno=0;
    //те настройки которые относяться к голосовому блоку передачи данных
	ret->voice_block_size           =4380;        //Размер равен 4000 байт минимального блока  на  проигрыш
	ret->all_num_of_voice_blocks    =0;    	      //количество пришедших голосовых блоков по 4000
	ret->ostatok_ot_voice_block_size=0;			  //если не равномерно склеиваються блоки                          
	

	printk("+mpcfifo_init_alocate_memory_for_structure+\n\r");	
	return (ret);
	
}





//////////////////////////////////STATIC FIFO BUFFER FUNCTION///////////////////////////////////////////
/**************************************************************************************************
Syntax:      	    bool mpcfifo_get_buf_ready(struct mpcfifo *rbd_p)	 
Return Value:	    Returns 1 on success and negative value on failure.
 				    Value		 									Description
				   -------------------------------------------------------------------------------------
				   = 1												Success
				   =-1												Failure
***************************************************************************************************/
static bool mpcfifo_get_buf_ready(struct mpcfifo *rbd_p)
{
	signed int l_tail=0;
	//проверяем условие что нормальный указатель в памяти не хлам
	if(!rbd_p)
	{return 0;}

	//rbd_p->tail;
    //Буфер пуст взять нечего назад ноль видимо 
	//printk("l_tail=%d>>rbd_p->head=%d\n\r",l_tail,rbd_p->head);
	
	//printk("++++rbd_p->tail=%d|rbd_p->head=%d++++\n\r",rbd_p->tail,rbd_p->head);
	
	
	/*Очередь не заполнена*/
	if (rbd_p->tail -rbd_p->head ==0)
	{
		 //mdelay(500);
	   // printk("mpcfifo_get_buf_ready/FIFO_EMPTY_rbd_p->tail=%d,rbd_p->head=%d,\n\r",rbd_p->tail,rbd_p->head);
		return 0;
	}
	//schetchic1=rbd_p->head;//-rbd_p->tail;
	
	/*
	
	if (rbd_p->tail -rbd_p->head ==FIFO_PACKET_NUM)
	{
		 //mdelay(500);
	     printk("mpcfifo_get_buf_ready/FIFO_FULL=%d\n\r",FIFO_PACKET_NUM);
	     rbd_p->head=0;
	     rbd_p->tail=0;
	     return 0;
	}
     */
	
	/*
	if(rbd_p->head-rbd_p->tail==FIFO_PACKET_NUM)
	{
		//mdelay(500);
	 // printk("????mpcfifo_get_buf_ready/bd_p->head-l_tail==FIFO_PACKET_NUM????\n\r");
	  return 0;
	}
	*/
  //printk("mpcfifo_get_buf_ready/return_1\n\r");
 
 
  return 1;
}

/**************************************************************************************************
Syntax:      	    static unsigned int mpcfifo_put(struct mpcfifo *rbd_p, void *obj)		 
Return Value:	    Returns 1 on success and negative value on failure.
 				    Value		 									Description
				   -------------------------------------------------------------------------------------
				   = 1												Success
				   =-1												Failure
***************************************************************************************************/
static unsigned int mpcfifo_put(struct mpcfifo *rbd_p,const unsigned char *buf)
{
	u16 i;
	DATA_lbc  ps;
	//static all_data_size=0;
	static int counts_nakopitel_bytes_to_voice_block=0;
	static int counts_nakopitel_bytes_kotorie_popali=0;  
	static int packet_count=0;
	static int schetchik_vhogdenia=0;
	static int local_count_i=0;
	static int prosto_esche_odin_packet_count=0;
	static int block_count=0;
	
	
	//Нет указателя на FIFO выходим из очереди
	if(!rbd_p){return 0;}
	
	prosto_esche_odin_packet_count=prosto_esche_odin_packet_count+1;
	
	
	//Складируем здесь данные до получения 4000
	counts_nakopitel_bytes_to_voice_block=counts_nakopitel_bytes_to_voice_block+rbd_p->cur_put_packet_size;
	packet_count=packet_count+1;
   
	
	if(prosto_esche_odin_packet_count==50)
    {	
	//printk("packet=%d,cur_data_size=%d,all_data_aize=%d\n\r",prosto_esche_odin_packet_count,rbd_p->cur_put_packet_size,counts_nakopitel_bytes_to_voice_block);
	prosto_esche_odin_packet_count=0;
    }
	
	
	//Заполняем три первых пакета
	if(packet_count<=3)
	{
		
		//Заполняем блок данными пока очень грубо будет много теряться данных
		for(i=0;i<rbd_p->cur_put_packet_size;i++)
		{
			    ps.data[i+schetchik_vhogdenia]=buf[i];   
			//  ps.data[i+counts_nakopitel_bytes_kotorie_popali]=buf[i];
			    local_count_i++;
    
		}
	 // printk("schtchik_massiva=%d\n\r",schetchik_vhogdenia);
	  schetchik_vhogdenia=local_count_i;
	  
	}
	
	

	//кладём три пакета
	if(packet_count==3)
	{
		
		// printk("PUT_TO_FIFO_BLOCK_tail++=%d\n\r",rbd_p->tail);
		//Пишу размер блока сюда равен около 4000
		  ps.size=counts_nakopitel_bytes_to_voice_block;//counts_nakopitel_bytes_to_voice_block;
		  //увелтчтваем очередь на еденицу
		  
		  /*Очередь достгла своего  пика обнуляем её*/
		  if(rbd_p->tail==4)
		  {
			 // printk("TAIL=0\n\r");
			  rbd_p->tail=0;  
		  }
		  
		  
		  rbd_p->q[rbd_p->tail++]=ps;
		  block_count=block_count+1;
		  
		  
		  
		  //printk("FIFO_BLOCK_PUT=%d,BLOCK__DATA_SIZE=%d|num_of_packet_in_BLOCK=%d,all_packet_in=%d\n\r",block_count,counts_nakopitel_bytes_to_voice_block,packet_count,prosto_esche_odin_packet_count);
		  
		  
		  
		  //mpcfifo_get_buf_ready(rbd_p);
		  
		  //mpcfifo_get_buf_ready(rbd_p);
		  
		  //rbd_p->tail=rbd_p->tail % rbd_p->N; //глубина очереди 2 блока.обнуляемуказатель на начало очереди
		  //printk("_PUT:rbd_p->tail=%d,rbd_p->N=%d,voice_block=%d\n\r",rbd_p->tail,rbd_p->N,rbd_p->all_num_of_voice_blocks);
		  
		  
	      //rbd_p->all_num_of_voice_blocks++;
	    //обнуляем счётчик байт на ноль 
	      counts_nakopitel_bytes_to_voice_block=0;
	      counts_nakopitel_bytes_kotorie_popali=0;
	      packet_count=0; 	    
	      local_count_i=0;
	      schetchik_vhogdenia=0;    
	  return 1;
	}
	
	//Обоаботка остатка пакетов
	
	
	
	

	
	//Естественно нужна защита от одновременного доступа к даннымю потом сделаю! пока так.
	//Размер текущего пакета который в очередь  кладу.
	//ps.size=rbd_p->cur_put_packet_size;
	//all_data_size=all_data_size+rbd_p->cur_put_packet_size;
	//rbd_p->all_rtp_paload_data_size=all_data_size;
	/*
	ret->voice_block_size           =4000;        //Размер равен 4000 байт минимального блока  на  проигрыш
	ret->all_num_of_voice_blocks    =0;    	      //количество пришедших голосовых блоков по 4000
	ret->ostatok_ot_voice_block_size=0;			  //если не равномерно склеиваються блоки  
	*/	
	/*Нужно обнулять и переставлять на следующий элемент только когда накопиться 4000 байт из нескольких пакетов */
	//rbd_p->q[rbd_p->tail++]=ps;
	//rbd_p->tail=rbd_p->N -rbd_p->head;
	//rbd_p->tail=rbd_p->tail %rbd_p->N; //глубина очереди 16 блоков.
	//printk("mpcfifo_put/data_size_byte=%d,count=%d\n\r",rbd_p->all_rtp_paload_data_size,count);
	//count=count+1;
	//return 1;
	
}
/**************************************************************************************************
Syntax:      	    static unsigned int mpcfifo_get(struct mpcfifo *rbd_p, void *obj		             
Return Value:	    Returns 1 on success and negative value on failure.
 				    Value		 									Description
				   -------------------------------------------------------------------------------------
				   = 1												Success
				   =-1												Failure
***************************************************************************************************/
static void mpcfifo_get(struct mpcfifo *rbd_p, void *obj)
{
	u16 i;
	DATA_lbc local;
	
	//printk("mpcfifo_get/rbd_p->all_rtp_paload_data_size=%d\n\r",rbd_p->all_rtp_paload_data_size);
	//printk("rbd_p->all_rtp_paload_data_size=%d\n\r",rbd_p->all_rtp_paload_data_size);
	
	
	//printk("GET:head++=%d,\n\r",rbd_p->head);
	
	//тут видимо ничего не меняем
	 if(rbd_p->head==4)
	 {
	 //  printk("TAIL=0\n\r");
	   rbd_p->head=0;  
	  }
	
	
	//rbd_p->head =rbd_p->head %rbd_p->N;//Обнуляем голову очерди
	
	local.size = rbd_p->q[rbd_p->head].size;
	rbd_p->cur_get_packet_size=rbd_p->q[rbd_p->head].size;
	
	
	//printk("mpcfifo_get/local.size=%d,rbd_p->head=%d\n\r",local.size,rbd_p->head);
	
	
    	
	for(i=0;i<local.size;i++)
	{
		  *(((u8 *)obj) + i) = rbd_p->q[rbd_p->head].data[i];
	}
	
	
	//Уаеличиваем указатель на еденицу
	rbd_p->head++;
    
	
	
   
	//mdelay(500);
	//printk("++mpc_fifo_get_ok++\n\r");
   /*	
   printk("+FIFO_Dir0_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r", local.data[0], local.data[1], local.data[2], local.data[3], local.data[4], local.data[5]);
   printk("+FIFO_Dir0_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r", local.data[rbd_p->cur_get_packet_size-6], local.data[rbd_p->cur_get_packet_size-5], local.data[rbd_p->cur_get_packet_size-4], local.data[rbd_p->cur_get_packet_size-3], local.data[rbd_p->cur_get_packet_size-2], local.data[rbd_p->cur_get_packet_size-1]);
   */	
}

////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////SET DATA PACKET TO FIFO BUFFER/////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
/*****************************************************************************
Syntax:      	    void voice_buf_set_data_in_rtp_stream1 
Remarks:			set data on input RTP packet to FIFO  buffer
*******************************************************************************/
void voice_buf_set_data_in_rtp_stream1 (const unsigned char *in_buf ,const int in_size)
{
int status=0;
int static set_iteration_dir1=0;  
unsigned long flags;
	
    //u8 dop_nechet_packet =0;    

   /*
   if(set_iteration_dir1==400)
   {	   
   printk(">>>>>>>>>>>>>>voice_buf_set_data_in_rtp_stream1|iter=%d<<<<<<<<<<<<<<<<\n\r",set_iteration_dir1);	 
   }*/


   //set packet size to fifo buffer
	/*
	if((in_size)%2==1)
	{ 
		printk("+odd_packet=%d+\n\r",in_size); 
	    //пропускаю только пакеты в заголовке ethernet type =0x0800 ARP имеет 0x0806 ETH_P_ARP
		dop_nechet_packet=1;
	}
	in_size = in_size+dop_nechet_packet;
	*/      
	spin_lock_irqsave(rtp_input_fifo_stream1->lock,flags);     
    
	//comment kosta for hex 01.12.13
    //fifo_put_to_tdm0_dir->cur_put_packet_size=in_size/2;
    // printk(">>>>>>>>>>>>>>nbuf_set_datapacket_dir0|iter=%d|size=%d<<<<<<<<<<<<<<<<\n\r",set_iteration_dir1,in_size);	
	rtp_input_fifo_stream1->cur_put_packet_size=in_size;
//Set to the FIFO buffer
	status=mpcfifo_put(rtp_input_fifo_stream1, in_buf);  
//mpcfifo_print(fifo_tdm0_dir_read, 0);
   
	spin_unlock_irqrestore(rtp_input_fifo_stream1->lock,flags);

set_iteration_dir1++;	
}

////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////GET DATA PACKET FROM FIFO BUFFER///////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
/*****************************************************************************
Syntax:      	    void voice_buf_set_data_in_rtp_stream1 
Remarks:			set data on input RTP packet to FIFO  buffer
*******************************************************************************/
bool voice_buf_get_data_in_rtp_stream1 (unsigned char *in_buf , int *in_size)
{
int  static get_iteration_dir1=0;
unsigned long flags;
	 //Буфер готов	
	 if(mpcfifo_get_buf_ready(rtp_input_fifo_stream1)==1)
	 {
		 //printk(">>>>>>>>>>>>nbuf_get_datapacket_dir1|iter=%d<<<<<<<<<<<<<<<<\n\r",get_iteration_dir1);
		 spin_lock_irqsave(rtp_input_fifo_stream1->lock,flags);
		
		 //берём из буфера пакет с данными 
		 mpcfifo_get(rtp_input_fifo_stream1, in_buf);
		 //берём размер пакета с данными
		 *in_size=rtp_input_fifo_stream1->cur_get_packet_size;
		  
		 spin_unlock_irqrestore(rtp_input_fifo_stream1->lock,flags);
		 
		 /* 
		 printk("+FIFO_Dir1_rfirst   |0x%02x|0x%02x|0x%02x|0x%04x|0x%02x|0x%02x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
		 printk("+FIFO_Dir1_rlast    |0x%02x|0x%02x|0x%02x|0x%04x|0x%02x|0x%04x|+\n\r",in_buf[in_size-6],in_buf[in_size-5],in_buf[in_size-4],in_buf[in_size-3],in_buf[in_size-2],in_buf[in_size-1]);
		 */
		#ifdef DEBUG_GET_FIFO_SEND_TO_ETHERNET 
		   p2020_get_recieve_virttsec_packet_buf(out_buf_dir1,packet_size_in_byte);//send to eternet	 
		#endif
		 
		get_iteration_dir1++;
		return 1;
	 }
	 else
	 {
		 memset(&in_buf,0x0000,sizeof(in_buf));
		 *in_size=0x0000;
		 //printk(">>>nbuf_get_datapacket_dir1|biffer_ne_gotov<<<\n\r");
		 return 0 ;
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
  kfree(rtp_input_fifo_stream1);
  printk("+free_fifo_buffer_OK+\n\r");	
}

















