/**
 ******************************************************************************
 * @file    board.c
 * @brief    
 *        Bootloader启动代码与硬件串口相关函数\n
 *
*/
/*--------------------------------------------------
 * version    |    author    |    date    |    content
 * V1.0            Menglong Wu  2014-11-11  
 ******************************************************************************
*/
#include "autoconfig.h"

#include "board_config.h"
#include "2440addr.h"
#include "nandflash.h"
// #include <stdio.h>
#include "board.h"


/**
 * @brief    串口采用115200/8/None/1
*/

void usart_init()
{
    //GPH 2\3 上拉输入
    rGPHCON = 0xa0;
    rGPHUP  = 0x0c; 


    rUFCON0 = 0x0;   //UART channel 0 FIFO control register, FIFO disable
    
    rUMCON0 = 0x0;   //UART chaneel 0 MODEM control register, AFC disable
    
//UART0
    rULCON0 = 0x3;   //Line control register : Normal,No parity,1 stop,8 bits
     //    [10]       [9]     [8]        [7]        [6]      [5]         [4]           [3:2]        [1:0]
     // Clock Sel,  Tx Int,  Rx Int, Rx Time Out, Rx err, Loop-back, Send break,  Transmit Mode, Receive Mode
     //     0          1       0    ,     0          1        0           0     ,       01          01
     //   PCLK       Level    Pulse    Disable    Generate  Normal      Normal        Interrupt or Polling
    rUCON0  = 0x245;   // Control register
    //rUBRDIV0=( (int)(pclk/16./baud+0.5) -1 );   //Baud rate divisior register 0
//UART1
    rUBRDIV0 = 26;
    
    
    // 清除开机前串口寄存器的垃圾数据
    // RdURXH0();
    // rUTRSTAT0 = 6;
    
}

/**
 * @brief    以阻塞模式，串口输出一个字符
*/

// int _IO_putc(int data,FILE *p)
int s_putchar(int data)
{
    if(data == '\n') {
        while(!(rUTRSTAT0 & 0x2));   //Wait until THR is empty.    
        WrUTXH0('\r');
    }
	while(!(rUTRSTAT0 & 0x2));   //Wait until THR is empty.
	return WrUTXH0(data);
}



/**
 * @brief    以阻塞模式，串口获取一个字符
*/
// int _IO_getc(FILE *p)
// int getchar(void)
int s_getchar(void)
{
	while(!(rUTRSTAT0 & 0x1)); //Receive data ready
	return RdURXH0();
}

/**
 * @brief    检测串口是否有外部输入，有则返回输入字符，无则返回0
*/
int s_peekchar(void)
{
    if(rUTRSTAT0 & 0x1) {
        return RdURXH0();
    }
    return 0;
}

/**
 * @brief    串口输出以字符串，以0结尾
*/
int puts(const char *s)
{
    while(*s) {
        s_putchar(*s++);
    }
    return 0;
}


#ifdef CONFIG_2440_IRQ
#include "s3c2440_irq.h"

extern void (*isr_handle_array[50])(void);
int do_irq(int argc,char **argv);
#endif

/**
 * @brief    所有中断进入点
 */
void EINT_Handle()
{
#ifdef CONFIG_2440_IRQ

    
    unsigned long oft = rINTOFFSET;
    // do_irq(0, 0);
    isr_handle_array[oft](); 
#endif
}


