/**********************************************************************************************************************
*                                        (c) COPYRIGHT by LONIIS.                                             *
*                                            All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : Ab_arm_test_drv_file.c
*
* Description : Testin Files for ARM Sitara AM3352 processor
*
* Author      : Konstantin Shiluaev..
*
******************************************************************************/


#include <linux/module.h> // Needed by all modules
#include <linux/kernel.h> // Needed for KERN_ALERT






/*****************************************************************************/
/*Local INCLUDES							     */
/*****************************************************************************/
#include "include/Ab_arm_test_drv_file.h"



/*****************************************************************************/
/*	Extern function Defenition						                 		*/
/*****************************************************************************/



/*****************************************************************************/
/*	Static Local function Defenition						                 */
/*****************************************************************************/
static void test_sizeof_function();
static void test_array_function();






/*****************************************************************************/
/*	COMMON CONFIGURATION						     */
/*****************************************************************************/






/**************************************************************************************************
Syntax:      	    void Start_Test_Sitara_arm_func()
Parameters:     	none
Remarks:			Start Testing function  for ARM processor 
Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
void Start_Test_Sitara_arm_func()
{
	
	printk("START_TEST_FUNCTION\n\r");
	
	
   //Nabor Testov for ARM Sitara processor	
	//TEST 1
	//test_sizeof_function();
	//TEST 2
	test_array_function();
	
	
	
	//TEST 3
	//TEST 4
	//TEST 5 
	
	
	
}



/**************************************************************************************************
Syntax:      	    test_array_function()
Parameters:     	none
Remarks:			Start Testing function  for ARM processor 
Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
static void test_array_function()
{
	unsigned char         d;//1 байт
	unsigned short int    a;//2 байта
    unsigned long  int    b;//4 байт
	
    unsigned long  int mas_long[2]={0x11223344,0x55667788};
    unsigned char  mas_byte [8];
    unsigned short int array[4]={0x1122,0x3344,0x5566,0x7788}; 
    unsigned short int mas  [4];
    
    
    //!!!!!!!!!!Примечание перевёртыши происходтя при  копирование массива unsigned short int в  unsigned char тоже  самое и с 32 разрядами!!!!!!!!!!!!!!
    //Перед   копированием нужно  превращать в Little Endian массивы
    //memcpy(mas_byte, array, sizeof(array));
     memcpy(mas_byte, mas_long, sizeof(mas_long));
    //memcpy(mas, array, sizeof(array));
     printk("mas0=0x%x,mas1=0x%x,mas2=0x%x,mas3=0x%x,mas4=0x%x,mas5=0x%x,mas6=0x%x,mas7=0x%x\n\r",mas_byte[0],mas_byte[1],mas_byte[2],mas_byte[3],mas_byte[4],mas_byte[5],mas_byte[6],mas_byte[7]);
    
    //printk("mas0=0x%x,mas1=0x%x,mas2=0x%x,mas3=0x%x\n\r",mas[0],mas[1],mas[2],mas[3]);
    //array[4]={0x1122;0x3344;0x5566;0x7788};
    
    __be16  e;
    //e=0x2233;
    
    // u16 m;
    //m=0x2233;
    
    //printk("e=%d,e=0x%x\n\r",e,e);
    /*
	d=0x0099;
	b=0x11223344;
	a=0x2255;
    
	printk("d=%d,d=0x%x\n\r",d,d);
    printk("a=%d,a=0x%x\n\r",a,a);
    printk("b=%d,b=0x%x\n\r",b,b);
	*/
	
}


/**************************************************************************************************
Syntax:      	    test_sizeof_function()
Parameters:     	none
Remarks:			Start Testing function  for ARM processor 
Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
static void test_sizeof_function()
{
  
  //unsigned ->>беззнаковый  (только положительные значения)
  //signed   -> знаковый	 (есть минус)  //по  умолчанию  все signed
	
  unsigned char   a;   //1 байт
  unsigned short  b;   //2 байт
  unsigned long   c;   //4 байт
		
  signed char     d;   //1 байт
  signed short    e;   //2 байт
  signed long     f;   //4 байт
  
  loff_t          t;    //8  байт
  size_t          len;  //4  байт

  int             l;    //4 байт (32 разряда)
  unsigned short int    m;    //2 байт (16 разрядов)    0-65535
  float           k;    //4 байт
  
  
  printk(KERN_INFO "float =  %d\n\r",sizeof k);
  printk(KERN_INFO "short int =  %d\n\r",sizeof m);
  printk(KERN_INFO "int =  %d\n\r",sizeof l);
  
  printk(KERN_INFO "lott_f =  %d\n\r",sizeof t);
  printk(KERN_INFO "size_t =  %d\n\r",sizeof len);
  printk(KERN_INFO "unsigned char =  %d\n\r",sizeof a);
  printk(KERN_INFO "unsigned short= %d\n\r",sizeof b);
  printk(KERN_INFO "unsigned long= %d\n\r",sizeof c);
  printk(KERN_INFO "signed char  =  %d\n\r",sizeof d);
  printk(KERN_INFO "signed short= %d\n\r",sizeof e);
  printk(KERN_INFO "signed long =  %d\n\r",sizeof f);
			
	
}




















