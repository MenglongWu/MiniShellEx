/**
 ******************************************************************************
 * @file	s3c2440_irq.c
 * @brief	s3c2440 中断

irq/driver/s3c2440_irq.c
 *
 @section Platform
	-# 
 @section Library
	-# 
- 2015-8-25,Menglong Woo,MenglongWoo@163.com
 	- brief
*/


#include "2440addr.h"
#include "2440lib.h"
#include "s3c2440_irq.h"
#include "board.h"
#include "minishell_core.h"



void (*isr_handle_array[50])(void);


void *irq_request(int index, void (*pfun)(void))
{
	void (*pold)(void);

	printk("init %d\n", index);
	if (index  >= 50 ) {
		return (void*)NULL;
	}
	
	pold = isr_handle_array[index];
	isr_handle_array[index] = pfun;
	
	return isr_handle_array[index];
}

void EINT0_Handle()
{
	printk("EINT0\n");
	// delay(200);
	rGPBDAT |= (0xf<<5);   // 所有LED熄灭
	rGPBDAT &= ~(1<<5);      // LED1点亮
	ClearPending(BIT_EINT0);
}

void EINT1_Handle()
{
	
	printk("EINT1 \n");
	// delay(200);
	rGPBDAT |= (0xf<<5);   // 所有LED熄灭
	rGPBDAT &= ~(1<<6);      // LED2点亮
	ClearPending(BIT_EINT1);

}

void EINT2_Handle()
{
	
	printk("EINT2\n");
	// delay(200);
	rGPBDAT |= (0xf<<5);   // 所有LED熄灭
	rGPBDAT &= ~(1<<7);      // LED3点亮
	ClearPending(BIT_EINT2);

}

void EINT4_7_Handle()
{
	
	printk("EINT4-7\n");
	// delay(200);
	
	
	rGPBDAT |= (0xf<<5);   // 所有LED熄灭
	rGPBDAT &= ~(1<<8);      // LED4点亮                
	
	rEINTPEND = (1<<4);   
	ClearPending(BIT_EINT4_7);	
}


void InitKeyInterrupt()
{
	unsigned long tmpreg;
	//GPF:0\1\2\4为输入口
	tmpreg = rGPFCON;
	tmpreg &= ~((1 << 0) | (1 << 2) | (1 << 4) | (1 << 8));
	tmpreg |=  ((1 << 1) | (1 << 3) | (1 << 5) | (1 << 9));
	rGPFCON = tmpreg;


	//GPF:0\1\2\4使能弱上拉
	tmpreg = rGPFUP;
	tmpreg &= ~((1 << 0) | (1 << 1) | (1 << 2) | (1 << 4));
	rGPFUP = tmpreg;

	//EINT0\1\2\4 下降沿触发方式
	tmpreg = rEXTINT0;
	tmpreg &= ~((7 << 0) | (7 << 4) | (7 << 8) | (8 << 16));
	tmpreg |= ((2 << 0) | (2 << 4) | (2 << 8) | (2 << 16));
	rEXTINT0 = tmpreg;


	//清除中断
	rSRCPND = 0x17;
	rINTPND =0x17;

	tmpreg = rEINTPEND;
	tmpreg |= ((1<<4));
	rEINTPEND = tmpreg;

	//rINTMSK = ~0x17;//
	EnableEINTIrq(BIT_EINT4_7);
	EnableIrq(BIT_EINT0 | BIT_EINT1 | BIT_EINT2 | BIT_EINT4_7);
}


int do_irq(int argc,char **argv)
{
	printk("irq\n");
	printk("\trSRCPND			%8.8x\n", rSRCPND);
	printk("\trINTMOD			%8.8x\n", rINTMOD);
	printk("\trINTMSK			%8.8x\n", rINTMSK);
	printk("\trPRIORITY			%8.8x\n", rPRIORITY);
	printk("\trINTPND			%8.8x\n", rINTPND);
	printk("\trINTOFFSET		%8.8x\n", rINTOFFSET);
	printk("\trSUBSRCPND		%8.8x\n", rSUBSRCPND);
	printk("\trINTSUBMSK		%8.8x\n", rINTSUBMSK);
	return 0;
}
W_BOOT_CMD(irq,do_irq,"Show irq register");