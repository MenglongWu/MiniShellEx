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

*/
#include "2440addr.h"
#include "stdio.h"
#include "string.h"

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
static volatile	unsigned char lcd_buffer[LEN_LCD_BUF];


#define M5D(n)			( ( n ) & 0x1fffff )				// To get lower 21bits
#define MVAL_USED 						(0)						//0=each frame   1=rate by MVAL
#define MVAL							(13)
#define HWSWP							(1)						//Half word swap control

#define	CONFIG_EMBEDSKY_LCD_VBPD		2
#define	CONFIG_EMBEDSKY_LCD_VFPD		4
#define	CONFIG_EMBEDSKY_LCD_VSPW		8
#define	CONFIG_EMBEDSKY_LCD_HBPD		10
#define	CONFIG_EMBEDSKY_LCD_HFPD		19
#define	CONFIG_EMBEDSKY_LCD_HSPW		30
#define	CONFIG_EMBEDSKY_LCD_CLKVAL		4
#define LCD_XSIZE_TFT	 				(480)	
#define LCD_YSIZE_TFT				 	(272)

#define SCR_XSIZE_TFT	 	(LCD_XSIZE_TFT)
#define SCR_YSIZE_TFT	 	(LCD_YSIZE_TFT)

#define HOZVAL_TFT		( LCD_XSIZE_TFT - 1 )
#define LINEVAL_TFT		( LCD_YSIZE_TFT - 1 )

volatile unsigned short embedsky_LCD_BUFFER[SCR_YSIZE_TFT][SCR_XSIZE_TFT] ;//= { 0, } ;

volatile char vbpd = 1, vfpd = 1, vspw = 1, hbpd = 1, hfpd = 1, hspw = 1, clkval_tft = 1 ;


/**
 * @brief	调试用
 * @param	null
 * @retval	null
 * @remarks	
 * @see	
 */
  
void buffer_init()
{
	unsigned long *p;
	// p = (unsigned long *)lcd_buffer;
	for (int i = 0 ; i < LEN_LCD_BUF; i++) {
		lcd_buffer[i] = 0xff;
	}
	volatile unsigned short *ps;
	ps = &embedsky_LCD_BUFFER[0][0];

	for (int i = 0; i < SCR_YSIZE_TFT*SCR_XSIZE_TFT ;i++) {
		*ps++ = 0xffff;
	}
}
/**
 * @brief	lcd 电源开关
 * @param	0 关闭
 * @param	0 打开
 */
static inline void lcd_power(unsigned int v)
{
	if (v) {
		rGPGDAT |= (1 << 4);// power on LCD电源	
	}
	else {
		rGPGDAT &= ~(1 << 4);// power down LCD电源	
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
	lcd_power(0);
}
char *getenv(char *s)
{
	return 0;
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
void embedsky_lcd_PowerEnable(int invpwren , int pwren);


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
	// bit[4:1]		0b1100	:	16 bpp for TFT
	// bit[0]		0 		:	Disable the video output and the LCD control signal
	// preg_lcd->lcdcon1 = (4 << 8) | (3 << 5) | (0xd << 1) | (0 << 0);
	preg_lcd->lcdcon1 = (4 << 8) | (3 << 5) | (0xC << 1) | (0 << 0);
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
	preg_lcd->lcdcon3 = (10 << 19) | (479 << 8) | (10);// 极限值
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
	preg_lcd->lcdcon5 = ( 1 << 11) | ( 0 << 10 ) | ( 1 << 9 ) | ( 1 << 8 ) | 
						( 0 << 7 ) | ( 0 << 6 ) | ( 1 << 3 ) | ( 0 << 1 ) | ( HWSWP ) ;


	// 显存地址
	preg_lcd->lcdsaddr1 = ( ( ( unsigned long ) embedsky_LCD_BUFFER >> 22 ) << 21 ) | M5D ( ( unsigned long ) embedsky_LCD_BUFFER >> 1 ) ;
	preg_lcd->lcdsaddr1 = ( ((unsigned long)embedsky_LCD_BUFFER) >> 1);
	// 错误写法，为什么错误不知道
	preg_lcd->lcdsaddr2 =( (  (unsigned long)((char*)embedsky_LCD_BUFFER +   SCR_XSIZE_TFT * LCD_YSIZE_TFT*2  ) >> 1) +1) & 0x1fffff;
	// 正确写法
	preg_lcd->lcdsaddr2 =( (  (unsigned long)((char*)embedsky_LCD_BUFFER +   (SCR_XSIZE_TFT) * LCD_YSIZE_TFT*2  ) >> 1) ) & 0x1fffff;
	// bit[21:11]	0	:	OFFSIZE 不使用滚动模式，没有偏移量
	// bit[10:1]	480	:	行点数 * bpp / Half ward = 480 * 16 / 16
	preg_lcd->lcdsaddr3 = (0<< 11) | (480 * 16 / 16);

	preg_lcd->lcdintmsk |= (3) ;
	preg_lcd->lcdintmsk = 0 ;
	preg_lcd->tconsel &= ( ~7 ) ;
	// 设置调色板
	// 24bpp 没有调色板
	preg_lcd->tpal = 0;



	// 使能LCD 背光
	lcd_power(1);
	// 启动LCD 本身
	preg_lcd->lcdcon1 |= 0x01;

	// 使能LCD 控制器
	preg_lcd->lcdcon5 |= (1 << 3);
	// embedsky_lcd_PowerEnable(0,1);

	// preg_lcd->lcdintmsk = 0xce0;
	readlcdreg();
}

void lcd_exit()
{

}



void embedsky_lcd_ClearScr_Rectangle( int x0,int y0,int width,int height, unsigned long c)
{
	unsigned int x,y ;
		
	for( y = y0 ; y < (y0 + height) ; y++ )
	{
		for( x = x0 ; x < (width + x0) ; x++ )
		{
			embedsky_LCD_BUFFER[y][x] = c;
		}
	}
}



/**************************************************************
TFT LCD 电源控制引脚使能
**************************************************************/
void embedsky_lcd_PowerEnable(int invpwren , int pwren)
{
	struct s3c_gpio_reg *gpio = (struct s3c_gpio_reg*)0x56000060;
	struct s3c_lcd_reg *lcd =(struct s3c_lcd_reg*)0x4d000000;
										//GPG4 is setted as LCD_PWREN
	gpio->gpbup = gpio -> gpbup & (( ~( 1 << 4) ) | ( 1 << 4) );		// Pull-up disable
	gpio -> gpbcon = gpio->gpbcon & (( ~( 3 << 8) ) | ( 3 << 8) );		//GPG4=LCD_PWREN
	gpio -> gpbdat = gpio -> gpbdat | (1 << 4 ) ;
										//invpwren=pwren;
										//Enable LCD POWER ENABLE Function
	lcd -> lcdcon5 = lcd -> lcdcon5 & (( ~( 1 << 3 ) ) | ( pwren << 3 ) );	// PWREN
	lcd -> lcdcon5 = lcd -> lcdcon5 & (( ~( 1 << 5 ) ) | ( invpwren << 5 ) );	// INVPWREN
}

/**************************************************************
LCD视频和控制信号输出或者停止，1开启视频输出
**************************************************************/
void embedsky_lcd_EnvidOnOff(int onoff)
{
	struct s3c_lcd_reg *lcd =(struct s3c_lcd_reg*)0x4d000000;

	if( onoff == 1 )
		lcd -> lcdcon1 |= 1 ;						// ENVID=ON
	else
		lcd -> lcdcon1 = lcd -> lcdcon1 & 0x3fffe ;			// ENVID Off
}

/**************************************************************
LCD初始化程序(在board/EmbedSky/目录下的boot_init.c文件的60行左右,调用的)
**************************************************************/
void embedsky_lcd_Init(void)
{
	return ;
// 	struct s3c_gpio_reg *gpio = (struct s3c_gpio_reg*)0x56000060;
// 	struct s3c_lcd_reg *lcd =(struct s3c_lcd_reg*)0x4d000000;

// 	char *s_lcd;

// 	lcd -> lcdsaddr1 = ( ( ( unsigned long ) embedsky_LCD_BUFFER >> 22 ) << 21 ) | M5D ( ( unsigned long ) embedsky_LCD_BUFFER >> 1 ) ;
// 	lcd -> lcdsaddr2 = M5D( ( ( unsigned long) embedsky_LCD_BUFFER + ( SCR_XSIZE_TFT * LCD_YSIZE_TFT * 2 ) ) >> 1 ) ;
// 	lcd -> lcdsaddr3 = ( ( ( SCR_XSIZE_TFT - LCD_XSIZE_TFT ) / 1 ) << 11 ) | ( LCD_XSIZE_TFT /1 ) ;

// 	vbpd = CONFIG_EMBEDSKY_LCD_VBPD;

// 	vfpd = CONFIG_EMBEDSKY_LCD_VFPD;

// 	vspw = CONFIG_EMBEDSKY_LCD_VSPW;

// 	hbpd = CONFIG_EMBEDSKY_LCD_HBPD;

// 	hfpd = CONFIG_EMBEDSKY_LCD_HFPD;

// 	hspw = CONFIG_EMBEDSKY_LCD_HSPW;

// 	clkval_tft = CONFIG_EMBEDSKY_LCD_CLKVAL;

// 	// embedsky_lcd_ClearScr( 0x0 ) ;
// 	lcd_port_init();
// 	// gpio -> GPCUP  = 0xffffffff ;
// 	// gpio -> GPCCON = 0xaaaaaaaa ;						//Initialize VD[0:7]	 
	 
// 	// gpio -> GPDUP  = 0xffffffff ;
// 	// gpio -> GPDCON = 0xaaaaaaaa ;						//Initialize VD[15:8]

// 	lcd -> lcdcon1 = ( clkval_tft << 8 ) | ( MVAL_USED << 7 ) | (3 << 5 ) | ( 12 << 1 ) | 0 ;
// 										// TFT LCD panel,16bpp TFT,ENVID=off
// 	lcd -> lcdcon2 = ( vbpd << 24 ) | ( LINEVAL_TFT << 14 ) | ( vfpd << 6 ) | ( vspw ) ;
// 	lcd -> lcdcon3 = ( hbpd << 19 ) | ( HOZVAL_TFT << 8 ) | ( hfpd ) ;
// 	lcd -> lcdcon4 = ( MVAL << 8 ) | ( hspw ) ;
// #if( LCD_TFT == VGA1024768 )
// 	lcd -> lcdcon5 = ( 1 << 11) | ( HWSWP ) ;
// #else
// 	lcd -> lcdcon5 = ( 1 << 11) | ( 0 << 10 ) | ( 1 << 9 ) | ( 1 << 8 ) | ( 0 << 7 ) | ( 0 << 6 ) | ( 1 << 3 ) | ( 0 << 1 ) | ( HWSWP ) ;
// #endif

// // 	lcd -> LCDINTMSK |= (3) ;						// MASK LCD Sub Interrupt
// // #if( LCD_TFT == VGA1024768 )
// // 	lcd -> LPCSEL &= 0xf82;
// // #else
// // 	lcd -> LPCSEL &= ( ~7 ) ;						// Disable LPC3480
// // #endif
// 	lcd -> tpal = 0x0 ;							// Disable Temp Palette


// 	embedsky_lcd_PowerEnable( 0, 1 ) ;
}

#include "minishell_core.h"

#define RGB(r,g,b)	(r) << 11 | (g) << 
int do_dr(int argc,char **argv)
{
	if (argc == 2 || argv[1][0] == '1') {
		// preg_lcd->lcdsaddr2 =( (  (unsigned long)((char*)embedsky_LCD_BUFFER +   (SCR_XSIZE_TFT) * LCD_YSIZE_TFT*2  ) >> 1) ) & 0x1fffff;
		// preg_lcd->lcdsaddr3 = (0<< 11) | (480 * 16 / 16);
	}
	else {
		// preg_lcd->lcdsaddr3 = (32<< 11) | (480 * 16 / 16);
	}
}
W_BOOT_CMD(dr,do_dr,"");