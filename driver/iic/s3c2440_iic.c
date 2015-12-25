/**
 ******************************************************************************
 * @file	s3c2440_iic.c
 * @brief	s3c2440 iic 裸机驱动

以TQ2440为平台，编写驱动
 *
- 2015-11-14,MenglongWoo,MenglongWoo@aliyun.com
 	- brief

*/

#include "2440addr.h"
#include "s3c2440_irq.h"
#include "minishell_core.h"
#include "board.h"
// #include "stdio.h"

// #include "stdio.h"
#include "string.h"
// #include "lcd.h"
#include "board.h"

volatile int flag;
static volatile int iic_ints;


#define WRDATA      (1)
#define POLLACK     (2)
#define RDDATA      (3)
#define SETRDADDR   (4)


#define IICBUFSIZE 0x20
static unsigned char _iicData[IICBUFSIZE];
static volatile int _iicDataCount;
static volatile int _iicStatus;
static volatile int _iicMode;
static int _iicPt;

static void delay(int time)
{
	int i,j;
	for(i=0; i<time;i++)
		for(j=0;j<1000;j++)
			;
}

// __irq void IIC_ISR(void)
void iic_isr(void)
{
		flag = 0;
		iic_ints++;
		// rSRCPND = (1<<27);
		// rINTPND = (1<<27);
		ClearPending(BIT_IIC);

			unsigned long iicSt,i;
		    
			rSRCPND = BIT_IIC;          //Clear pending bit
			rINTPND = BIT_IIC;
			iicSt   = rIICSTAT; 
		    
			if(iicSt & 0x8){}           //When bus arbitration is failed.
			if(iicSt & 0x4){}           //When a slave address is matched with IICADD
			if(iicSt & 0x2){}           //When a slave address is 0000000b
			if(iicSt & 0x1){}           //When ACK isn't received

			switch(_iicMode)
			{
				case POLLACK:
					_iicStatus = iicSt;
					break;

				case RDDATA:
					if((_iicDataCount--)==0)
					{
						_iicData[_iicPt++] = rIICDS;
		            
						rIICSTAT = 0x90;                 //Stop MasRx condition 
						rIICCON  = 0xaf;                 //Resumes IIC operation.
						delay(1);                        //Wait until stop condtion is in effect.
		                                                //Too long time... 
		                                                //The pending bit will not be set after issuing stop condition.
						break;    
					}      
					_iicData[_iicPt++] = rIICDS;         //The last data has to be read with no ack.

					if((_iicDataCount)==0)
						rIICCON = 0x2f;                  //Resumes IIC operation with NOACK.  
					else 
						rIICCON = 0xaf;                  //Resumes IIC operation with ACK
					break;

				case WRDATA:
					if((_iicDataCount--)==0)
					{
						rIICSTAT = 0xd0;                //Stop MasTx condition 
						rIICCON  = 0xaf;                //Resumes IIC operation.
						delay(1);                       //Wait until stop condtion is in effect.
						//The pending bit will not be set after issuing stop condition.
						break;    
					}
					rIICDS = _iicData[_iicPt++];        //_iicData[0] has dummy.
					for(i=0;i<10;i++);                  //for setup time until rising edge of IICSCL
		              
					rIICCON = 0xaf;                     //resumes IIC operation.
					break;

				case SETRDADDR:
		 			if((_iicDataCount--)==0)
					break;                          //IIC operation is stopped because of IICCON[4]    
					rIICDS = _iicData[_iicPt++];
					for(i=0;i<10;i++);                  //For setup time until rising edge of IICSCL
					rIICCON = 0xaf;                     //Resumes IIC operation.
					break;

				default:
					break;      
			}
}


/*
	IICSCL	GPE14
	IICSDA	GPE15
*/
void iic_port_init(void)
{
	unsigned long tmpreg;

	tmpreg = rGPECON;
	tmpreg &= ~( (3 << 28) | (3 << 30) );
	tmpreg |=  ( (2 << 28) | (2 << 30) );
	rGPECON = tmpreg;

	rGPEUP |=  ( (1 << 14) | (1 << 13) );
}

void iic_control_init()
{

}

void IIC_Init(void)
{
	/*	将IO口配置为SDA和SCL	*/
	// rGPECON &= ~(0xf<<28);
	// rGPECON |= (2<<28) | (2<<30);
	/*	禁止上拉	*/
	// rGPEUP |= (1<<14) | (1<<13);
	iic_port_init();

	/*	使能应答信号，设置IIC时钟为fPCLK的512分频，使能中断	*/
	rIICCON = (1<<7) | (1<<6) | (1<<5);

	// TODO 中断
	// pISR_IIC = (unsigned int)IIC_ISR;
	
	/*	使能串行输出	*/
	rIICSTAT |= (1<<4);
	
	/*	使能中断	*/
	rINTMSK &= ~(1<<27);
	rSRCPND = (1<<27);
	rINTPND = (1<<27);



	rCLKCON |= 1<<16;
	
	rGPEUP  |= 0xc000;                  //Pull-up disable
	rGPECON &= ~0xf0000000;
	rGPECON |= 0xa0000000;              //GPE15:IICSDA , GPE14:IICSCL 
	
	//Enable ACK, Prescaler IICCLK=PCLK/16, Enable interrupt, Transmit clock value Tx clock=IICCLK/16
	// If PCLK 50.7MHz, IICCLK = 3.17MHz, Tx Clock = 0.198MHz
	//rIICCON = (1<<7) | (1<<6) | (1<<5) | (0);	//IICCLK=PCLK/512=97K
	//just read one byte, don't enable ack!
	rIICCON = (0<<7) | (1<<6) | (1<<5) | (0);	//IICCLK=PCLK/512=97K
	
	rIICSTAT = 0x10;                    //IIC bus data output enable(Rx/Tx)
	rIICADD  = 0x10;                    //2410 slave address = [7:1]


	EnableIrq(BIT_IIC);
	// EnableSubIrq(BIT_SUB_TC);
	// EnableSubIrq(BIT_SUB_ADC);
	irq_request(ISR_IIC_OFT, iic_isr);
}



static int iic_wr(unsigned char data, unsigned char cnt)
{
	int i, c;
	
	rIICCON &= ~0x10;		//Resume IIC operation to send
	
	if(cnt)
		rIICDS = data;
	
	i = 3;	//3ms
	//while(!(rIICCON&0x10)&&i--) delay(1);
	c = iic_ints;
	while(iic_ints==c&&i--) delay(1);
	//rIICCON &= ~0x10;
	
	if(!i)
		return -1;

	return (rIICSTAT&1)?-1:0;
}

static int iic_rd(unsigned char *pBuf)
{
	int i, c;
	
	rIICCON &= ~0x10;		//Resumes IIC operation to receive
	i = 3;	//3ms
	//while(!(rIICCON&0x10)&&i--) delay(1);
	c = iic_ints;
	while(iic_ints==c&i--) delay(1);
	
	if(i) {
		*pBuf = rIICDS;
		return 0;
	}
	
	return -1;
}



//*************************[ Wr24C080 ]****************************
void Wr24C080(unsigned long slvAddr,unsigned long addr,unsigned char data)
{
	_iicMode      = WRDATA;
	_iicPt        = 0;
	_iicData[0]   = (unsigned char)addr;
	_iicData[1]   = data;
	_iicDataCount = 2;
    
	rIICDS   = slvAddr;                 //0xa0
	rIICSTAT = 0xf0;                    //MasTx,Start
	//Clearing the pending bit isn't needed because the pending bit has been cleared.
    
	while(_iicDataCount!=-1);

	_iicMode = POLLACK;

	while(1)
	{
		rIICDS     = slvAddr;
		_iicStatus = 0x100;
		rIICSTAT   = 0xf0;              //MasTx,Start
		rIICCON    = 0xaf;              //Resumes IIC operation. 
           
		while(_iicStatus==0x100);
           
		if(!(_iicStatus&0x1))
			break;                      //When ACK is received
	}
	rIICSTAT = 0xd0;                    //Stop MasTx condition 
	rIICCON  = 0xaf;                    //Resumes IIC operation. 
	delay(1);                           //Wait until stop condtion is in effect.
	//Write is completed.
}
        
//**********************[ Rd24C080 ] ***********************************
void Rd24C080(unsigned long slvAddr,unsigned long addr,unsigned char *data)
{
	_iicMode      = SETRDADDR;
	_iicPt        = 0;
	_iicData[0]   = (unsigned char)addr;
	_iicDataCount = 1;

	rIICDS   = slvAddr;
	rIICSTAT = 0xf0;                    //MasTx,Start  
	//Clearing the pending bit isn't needed because the pending bit has been cleared.
	while(_iicDataCount!=-1);

	_iicMode      = RDDATA;
	_iicPt        = 0;
	_iicDataCount = 1;
    
	rIICDS        = slvAddr;
	rIICSTAT      = 0xb0;               //MasRx,Start
	rIICCON       = 0xaf;               //Resumes IIC operation.   
	while(_iicDataCount!=-1);

	*data = _iicData[1];
}

void wr2402(unsigned char devAddr,unsigned char addr, unsigned char *data, int len)
{
	int i;
	
	/*	发送从设备地址(寻址)	*/
	flag = 1;
	rIICDS = devAddr;	
	rIICCON &= ~(1<<4);	//清除中断，注意，这句话要放在写IICDS后面
	rIICSTAT = 0xf0;	//设置为主发送模式
	// while(flag==1)		//等待从设备应答
	// 	delay(100);
	
	/*	发送所写的内存地址	*/	
	flag = 1;
	rIICDS = addr;
	rIICCON &= ~(1<<4);
	// while(flag == 1);
	// 	delay(100);
		
	/*	发送所需要写入的数据	*/
	for(i=0; i<len; i++)
	{
		flag = 1;
		rIICDS = data[i];
		rIICCON &= ~(1<<4);
		// while(flag == 1);
		// 	delay(100);
	}
	
	/*	发送停止信号	*/
	rIICSTAT = 0xd0;
	/*	恢复操作	*/
	rIICCON = 0xe0;
	
	// delay(100);
}

void rd2402(unsigned char devAddr,unsigned char addr,unsigned char *data, int len)
{
	int i;
	
	/*	发送从设备地址	*/
	flag = 1;
	rIICDS = devAddr;
	rIICCON &= ~(1<<4);
	rIICSTAT = 0xf0;
	while(flag == 1);
		delay(100);
	
	/*	发送所需要读的内存地址	*/
	flag = 1;
	rIICDS = addr;
	rIICCON &= ~(1<<4);
	while(flag == 1);
		delay(100);
	
	/*	设置主设备接收模式	*/
	flag = 1;
	rIICDS = devAddr;
	rIICCON &= ~(1<<4);
	rIICSTAT = 0xB0;
	while(flag == 1);
		delay(100);

	/*	读取的是设备地址	*/
	flag = 1;
	i = rIICDS;
	rIICCON &= ~(1<<4);
	while(flag==1)
		delay(100);
	
	/*	读数据	*/
	for(i=0; i<len; i++)
	{
		flag = 1;
		/*	最后一个数据不发送ACK	*/
		if (i==len-1)
		{
			rIICCON &= ~(1<<7);
		}
		data[i] = rIICDS;
		rIICCON &= ~(1<<4);
		while(flag==1)
			delay(100);
	}
	
	rIICSTAT = 0x90;
 	rIICCON = 0xe0;
	
 	delay(100);
}


int iic_24cxx(int argc,char **argv)
{
	unsigned char data[256];
	int i;
	
	printk("\nIIC test: write 16 byte to 24c02 page 0,1. 8 Byte/Page\n");
	IIC_Init();
	
	
	for(i=0; i<16; i++)
		data[i] = i;
	// while(1) {
	// 	wr2402(0xa0,0,data,8);
	// }
	// write_eeprom(0xa0,data,8);
	// write_eeprom(0xa0,data+8,8);	//这里使用的是AT24C02，每页只有8B，所以每次最多只能写8B,多了会回滚
	for(i=0;i<256;i++)
		Wr24C080(0xA0,(unsigned char)i,i);
	for(i=0; i<16; i++)
		data[i] = 3;

	for(i=0;i<256;i++)
		Rd24C080(0xA1,(unsigned char)i,&(data[i])); 

	// read_eeprom(0xa0,data,16);		//读可以不受页大小的影响

	printk("Read date from 24c02 16 Byte\n");
	for (i = 0; i < 16;i++) {
		printk("%2.2x ", data[i]);
	}
	printk("\n");
	printk("Test Finish\n");
	return 0;
}
W_BOOT_CMD(iic24,iic_24cxx,"iic test ");

