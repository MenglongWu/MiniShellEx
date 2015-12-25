/**
 ******************************************************************************
 * @file	LR043JC211.c
 * @brief	天嵌入4.3寸 LCD 屏幕

	driver/lcd/LR043JC211.c. \n

	Screen Size    			4.3(Diagonal)  inch
	Display Format 			480 RGB x 272  Dot
	Active  Area  			95.04(H) x 53.856(V)  
	Pixel Pitch   			0.198(H) x 0.198(V)  
	Surface Treatment  		Touch panel , 10% haze 
	Pixel Confi guration  	RGB-Stripe 
	Outline Dimension    	105.5( W) x 67.2(H) x 5(D)  


- 2015-8-9,Menglong Wu,MenglongWoo@163.com
 	- 2440 LCD驱动练习，该源码采用的是16bpp，后期会将该驱动改成24bpp
 	- 本驱动参考的数据手册是 "LCD_LR043JC211_V04.pdf" 光盘里有3份LCD文档，不清楚
 	到底装的是那一款，所以驱动时序设置有点问题
 	- 本驱动参考天嵌源码 u-boot-1.1.6/drivers/lcd/embedsky_lcd.c 
 	
- 2015-8-11,Menglong Wu,MenglongWoo@163.com
	- 修改成24bpp

*/
#include "2440addr.h"
#include "stdio.h"
#include "string.h"
#include "lcd.h"
#include "board.h"

struct s3c_gpio_reg
{
	unsigned long gpbcon;
	unsigned long gpbdat;
	unsigned long gpbup;
};

struct s3c_lcd_reg
{
	unsigned long lcdcon1;
	unsigned long lcdcon2;
	unsigned long lcdcon3;
	unsigned long lcdcon4;
	unsigned long lcdcon5;
	unsigned long lcdsaddr1;
	unsigned long lcdsaddr2;
	unsigned long lcdsaddr3;
	unsigned long redlut;
	unsigned long greenlut;
	unsigned long bluelut;
	unsigned long reserved[9];
	unsigned long dithmode;
	unsigned long tpal;
	unsigned long lcdintpnd;
	unsigned long lcdsrcpnd;
	unsigned long lcdintmsk;
	unsigned long tconsel;
};

#define LEN_LCD_BUF 480*270*4
static volatile struct s3c_lcd_reg  *preg_lcd;
static volatile struct s3c_gpio_reg *preg_gpio;
// static volatile	unsigned char lcd_buffer[LEN_LCD_BUF];


// #define M5D(n)			( ( n ) & 0x1fffff )				// To get lower 21bits
// #define MVAL_USED 						(0)						//0=each frame   1=rate by MVAL
// #define MVAL							(13)
// #define HWSWP							(1)						//Half word swap control

// #define	CONFIG_EMBEDSKY_LCD_VBPD		2
// #define	CONFIG_EMBEDSKY_LCD_VFPD		4
// #define	CONFIG_EMBEDSKY_LCD_VSPW		8
// #define	CONFIG_EMBEDSKY_LCD_HBPD		10
// #define	CONFIG_EMBEDSKY_LCD_HFPD		19
// #define	CONFIG_EMBEDSKY_LCD_HSPW		30
// #define	CONFIG_EMBEDSKY_LCD_CLKVAL		4
#define LCD_XSIZE_TFT	 				(480)	
#define LCD_YSIZE_TFT				 	(272)

// #define SCR_XSIZE_TFT	 	(LCD_XSIZE_TFT)
// #define SCR_YSIZE_TFT	 	(LCD_YSIZE_TFT)

#define HOZVAL_TFT		( LCD_XSIZE_TFT - 1 )
#define LINEVAL_TFT		( LCD_YSIZE_TFT - 1 )

volatile unsigned long lcd_buffer[LCD_YSIZE_TFT][LCD_XSIZE_TFT] ;//= { 0, } ;
volatile unsigned long lcd_buffer2[LCD_YSIZE_TFT][LCD_XSIZE_TFT] ;//= { 0, } ;

volatile char vbpd = 1, vfpd = 1, vspw = 1, hbpd = 1, hfpd = 1, hspw = 1, clkval_tft = 1 ;
#define lcdbuf    (0x00001000)	//Bus width & wait status

/**
 * @brief	调试用
 * @param	null
 * @retval	null
 * @remarks	
 * @see	
 */
  
void buffer_init()
{
	unsigned long *ps,*ps2;
	ps = (unsigned long *)&lcd_buffer[0][0];
	ps2 = (unsigned long *)&lcd_buffer2[0][0];

	for (int i = 0; i < LCD_YSIZE_TFT*LCD_XSIZE_TFT ;i++) {
		*ps++  = RGB(255,255,255);
		*ps2++ = RGB(0,255,0);
	}
}
// 197x45

/**
 * @brief	lcd 背光电源开关
 * @param	0 关闭
 * @param	0 打开
 */
void lcd_blacklight(unsigned int v)
{
	if (v) {
		rGPGDAT |= (1 << 4);// power on LCD电源	
	}
	else {
		rGPGDAT &= ~(1 << 4);// power down LCD电源	
	}
}

/**
 * @brief	lcd 控制器开关
 * @param	0 关闭
 * @param	0 打开
 */
void lcd_power(unsigned int v)
{
	if (v) {
		rLCDCON1 |= (1 << 0);
		rLCDCON5 |= (1 << 3);
	}
	else {
		rLCDCON1 &= ~(1 << 0);
		rLCDCON5 &= ~(1 << 3);
	}
}

/**
 GPG	4
 		LCD_POWER
 
 GPC	15		14		13		12		11		10		9		8
 		VD7		VD6		VD5		VD4		VD3		VD2		VD1		VD0

		7				6				5				4	
	 	LCD_LPCREVB		LCD_LPCREV		LCD_LPCOE		VM

	 	3				2				1				0
	 	VFRAME			VLINE			VCLK			LEND

 GPD	15			14			13			12			
 		VD23		VD22		VD21		VD20		
 		11			10			9			8
 		VD19		VD18		VD17		VD16
 		7			6			5			4
 		VD15		VD14		VD13		VD12
 		3			2			1			0
 		VD11		VD10		VD9		VD8
 
 */
void lcd_port_init()
{
	rGPCCON = 0xaaaaaaaa;// 启用LCD功能
	rGPCUP  = 0xffffffff;// 禁止内部上啦

	rGPDCON = 0xaaaaaaaa;// 启用LCD功能
	rGPDUP  = 0xffffffff;// 禁止内部上啦

	rGPGCON &= ~(3 << 8);// GPG4 output
	rGPGCON |=  (1 << 8);
	lcd_blacklight(0);
}


void readlcdreg()
{
	preg_lcd = (struct s3c_lcd_reg*)0x4d000000;
	printk("read reg\n");
	printk("lcdcon1   	%x\n",preg_lcd->lcdcon1);
	printk("lcdcon2   	%x\n",preg_lcd->lcdcon2);
	printk("lcdcon3   	%x\n",preg_lcd->lcdcon3);
	printk("lcdcon4   	%x\n",preg_lcd->lcdcon4);
	printk("lcdcon5   	%x\n",preg_lcd->lcdcon5);
	printk("lcdsaddr1   %x\n",preg_lcd->lcdsaddr1);
	printk("lcdsaddr2   %x\n",preg_lcd->lcdsaddr2);
	printk("lcdsaddr3  	%x\n",preg_lcd->lcdsaddr3);
	printk("redlut   	%x\n",preg_lcd->redlut);
	printk("greenlut   	%x\n",preg_lcd->greenlut);
	printk("bluelut   	%x\n",preg_lcd->bluelut);
	printk("reserved   	%x\n",preg_lcd->reserved);
	printk("dithmode   	%x\n",preg_lcd->dithmode);
	printk("tpal   		%x\n",preg_lcd->tpal);
	printk("lcdintpnd   %x\n",preg_lcd->lcdintpnd);
	printk("lcdsrcpnd   %x\n",preg_lcd->lcdsrcpnd);
	printk("lcdintmsk   %x\n",preg_lcd->lcdintmsk);
	printk("tconsel   	%x\n",preg_lcd->tconsel);
}
// void embedsky_lcd_PowerEnable(int invpwren , int pwren);


void lcd_init()
{
	buffer_init();
	// GPIO 配置成 LCD功能
	readlcdreg();
	lcd_port_init();	
	// return ;
	
	// Step 2 根据LCD手册配置LCD控制器，
	// 如下所有设置基于HCLK = 100M 的前提
	preg_lcd = (struct s3c_lcd_reg*)0x4d000000;

	// Step 2.1 设置 LCD 控制器输出时钟VCLK、色彩深度 X bpp
	// bit[17:8]	TFT: VCLK = HCLK / [(CLKVAL+1) x 2]    (CLKVAL >= 0)
	// 				芯片工作在 HCLK = 100M
	// 				LCD 手册   VCLK = 15M(66.7ns) 这里选用10M(100ns)便于计算
	// 				CLKVAL = 4
	// bit[6:5]		0b11 	:	TFT LCD
	// bit[4:1]		0b1100	:	24 bpp for TFT
	// bit[0]		0 		:	Disable the video output and the LCD control signal
	preg_lcd->lcdcon1 = (4 << 8) | (3 << 5) | (0xD << 1) | (0 << 0);
	// preg_lcd->lcdcon1 =0x36c0479;

	// Step 2.2 垂直同步参数 VBPD、LINEVAL、VFPD、VSPW
	// bit[31:24]	1	:		VBPD	Tvb = 2	垂直同步信号后多长时间，多少个无效行同步信号，LCD 手册 Tvb
	// bit[24:14]	271	:		LINEVAL	= 272 - 1，LCD 手册 480 RGB x 272  Dot
	// bit[13:6]	1	:		VFPD	Tvf = 2		垂直同步信号结束前多长时间，多少个无效行同步信号，LCD 手册 Tvf
	// bit[5:0]		1	:		VSPW	Tvp = 2		垂直同步信号脉宽占用几个时钟周期，LCD 手册 Tvp
	// preg_lcd->lcdcon2 = (2 << 24) | (271 << 14) | (4 << 6) | (2 << 0);
	preg_lcd->lcdcon2 = (2 << 24) | (271 << 14) | (4 << 6) | (8 << 0);

	// LCD 手册 Note2: Thf+ Thp+ Thb >44
	// bit[25:19]	10	:		HBPD = 10;	Thb = 11	LCD 手册 Thb
	// bit[18:8]	479	:		480 - 1		LCD 手册 480 RGB x 272  Dot
	// bit[7:0]		10 	:		HFPD = 10;	Thf = 11	LCD 手册 Thf
	preg_lcd->lcdcon3 = (10 << 19) | ((LCD_XSIZE_TFT - 1) << 8) | (10);// 极限值
	// bit[7:0]		20	:		HSPW = 20;	Thp = 21	水平同步脉冲宽度，LCD 手册 Thp
	preg_lcd->lcdcon4 = (13 << 8) | (30);


	// 信号极性
	// bit[12] 		0	24bpp 才需要设置，0 = LSB valid 1 = MSB Valid
	// bit[11]			16bpp 才需要设置，
	// bit[10]		0 	VCLK 极性，不反转
	// bit[9] 		1	VLINE/HSYNC，反转
	// bit[8] 		1	VFRAME/VSYNC，反转
	// bit[7] 		0	VM 数据，不反转（低电平表示0，高电平表示1）
	// bit[6] 		0	VEN 数据使能，不反转
	// bit[3] 		0	POWER_EN 输出低电平
	// bit[1]		0	内存结构采用D[23:0]  详情查看 2440手册 “24BPP Display”
	// bit[0]		0	内存结构采用D[23:0]  详情查看 2440手册 “24BPP Display”
	// preg_lcd->lcdcon5 = (0 << 0) | (0 << 10) | (1 << 9) | (1 << 8);
	preg_lcd->lcdcon5 = ( 0 << 12) | ( 0 << 11) | ( 0 << 10 ) | ( 1 << 9 ) | ( 1 << 8 ) | 
						( 0 << 7 ) | ( 0 << 6 ) | ( 1 << 3 ) | ( 0 << 1 ) | ( 0 << 0 ) ;

	// 显存地址
	// preg_lcd->lcdsaddr1 = ( ( ( unsigned long ) lcd_buffer >> 22 ) << 21 ) | M5D ( ( unsigned long ) lcd_buffer >> 1 ) ;
	preg_lcd->lcdsaddr1 = ( ((unsigned long)lcd_buffer) >> 1);
	// 错误写法，为什么错误不知道
	preg_lcd->lcdsaddr2 =( (  (unsigned long)((char*)lcd_buffer +   LCD_XSIZE_TFT * LCD_YSIZE_TFT*4  ) >> 1) +1) & 0x1fffff;
	// 正确写法
	preg_lcd->lcdsaddr2 =( (  (unsigned long)((char*)lcd_buffer +   LCD_XSIZE_TFT * LCD_YSIZE_TFT*4  ) >> 1) ) & 0x1fffff;
	// bit[21:11]	0	:	OFFSIZE 不使用滚动模式，没有偏移量
	// bit[10:1]	480	:	行点数 * bpp / Half ward = 480 * 16 / 16
	preg_lcd->lcdsaddr3 = (0<< 11) | (LCD_XSIZE_TFT * 32 / 16);

	preg_lcd->lcdintmsk = 0 ;
	preg_lcd->tconsel &= ( ~7 ) ;
	// 设置调色板
	// 24bpp 没有调色板
	preg_lcd->tpal = 0;



	// 使能LCD 背光
	lcd_blacklight(1);
	// 启动LCD 本身
	
	
	// 使能LCD 控制器
	preg_lcd->lcdcon1 |= 0x01;

	readlcdreg();
}

// 如下是测试代码，不需要刻意屏蔽
void lcd_rectangle( int x0,int y0,int width,int height, unsigned long c)
{
	register unsigned int x,y ;
		
	for( y = y0 ; y < (y0 + height) ; y++ )
	{
		for( x = x0 ; x < (width + x0) ; x++ )
		{
			lcd_buffer[y][x] = c;
		}
	}
}


void lcd_rectangle2( int x0,int y0,int width,int height, unsigned long c)
{
	unsigned int x,y;
	for( y = y0 ; y < (y0 + height) ; y++ )
	{
		for( x = x0 ; x < (width + x0) ; x++ )
		{
			
			lcd_buffer2[y][x] = c;
		}
	}
}




#include "minishell_core.h"


int do_dr(int argc,char **argv)
{
	
	if (argc == 2 && argv[1][0] == '1') {
		// printk("dr....\n");
		// lcd_rectangle2(0, 0, 480,270, 0x00ff00ff);
	}
	else {
		printk("dr....22\n");
		lcd_rectangle2(0, 0, 480,270, 0x00ff0000);// red
		rDISRC0= (volatile unsigned int)&lcd_buffer2[0][0];         // Nand flash data register
		rDISRCC0=(0<<1) | (0<<0); //arc=AHB,src_addr=fix
		rDIDST0=(unsigned)(lcd_buffer);
		rDIDSTC0=(0<<1) | (0<<0); //dst=AHB,dst_addr=inc;
		rDCON0=(1<<31)|(1<<30)|(0<<29)|(1<<28)|(1<<27)|(0<<23)|(1<<22)|(2<<20)|((480*272/4));
		rDMASKTRIG0=(1<<1)|(1<<0);
		while(0 != rDMASKTRIG0);
	}

	if (argc == 2 && argv[1][0] == '1') {
		for (int i = 0; i < 272; i++) {
			int k[4];
			k[0] =  RGB(0xaa,0xaa,0xaa);
			k[1] =  RGB(0xaa,0xaa,0xaa);
			k[2] =  RGB(0xaa,0xaa,0xaa);
			k[3] =  RGB(0xaa,0xaa,0xaa);

			

			rDISRC0= (volatile unsigned int)&k[0];          // Nand flash data register
			rDISRCC0=(0<<1) | (1<<0); 						//arc=AHB,src_addr=fix
			rDIDST0=(unsigned)(lcd_buffer2);
			rDIDSTC0=(0<<1) | (0<<0); 						//dst=AHB,dst_addr=inc;
			rDCON0=(1<<31)|(1<<30)|(0<<29)|(1<<28)|(1<<27)|(0<<23)|(1<<22)|(2<<20)|(480*272/4);
			rDMASKTRIG0=(1<<1)|(1<<0);
			while(0 != rDMASKTRIG0);

			static int f = 0;
			f++;
			
			if (f == 272) {
				f = 0;
			}
			k[0] =  RGB(0xff,0,0);
			k[1] =  RGB(0xff,0,0);
			k[2] =  RGB(0xff,0,0);
			k[3] =  RGB(0xff,0,0);

			rDISRC0= (volatile unsigned int)&k[0];          // Nand flash data register
			rDISRCC0=(0<<1) | (1<<0); 						//arc=AHB,src_addr=fix
			rDIDST0=(unsigned)((char*)lcd_buffer2+480*f*4);
			rDIDSTC0=(0<<1) | (0<<0); 						//dst=AHB,dst_addr=inc;
			rDCON0=(1<<31)|(1<<30)|(0<<29)|(1<<28)|(1<<27)|(0<<23)|(1<<22)|(2<<20)|(480*1/4);
			rDMASKTRIG0=(1<<1)|(1<<0);
			while(0 != rDMASKTRIG0);

			//lcd_rectangle2(20+f, 20, 100,100,0xff0000);

			rDISRC0= (volatile unsigned int)&lcd_buffer2[0][0];         // Nand flash data register
			rDISRCC0=(0<<1) | (0<<0); //arc=AHB,src_addr=fix
			rDIDST0=(unsigned)(lcd_buffer);
			rDIDSTC0=(0<<1) | (0<<0); //dst=AHB,dst_addr=inc;
			rDCON0=(1<<31)|(1<<30)|(0<<29)|(1<<28)|(1<<27)|(0<<23)|(1<<22)|(2<<20)|((480*272/4));
			rDMASKTRIG0=(1<<1)|(1<<0);
			while(0 != rDMASKTRIG0);
		}
	}


}





W_BOOT_CMD(dr,do_dr,""); 

int do_lcdpower(int argc,char **argv)
{
	if (argc == 2 && argv[1][0] == '1') {
		lcd_power(1);
	}
	else if (argc == 2 && argv[1][0] == '0') {
		lcd_power(0);
	}
}
W_BOOT_CMD(lcdpower,do_lcdpower,""); 

int do_lcdbl(int argc,char **argv)
{
	if (argc == 2 && argv[1][0] == '1') {
		lcd_blacklight(1);
	}
	else if (argc == 2 && argv[1][0] == '0') {
		lcd_blacklight(0);
	}
}
W_BOOT_CMD(lcdbl,do_lcdbl,"");

