/**
 ******************************************************************************
 * @file	lcdtest.c
 * @brief	裸机 LCD 驱动接口

	LCD 驱动测试程序，在运行该测试程序之前首先保证驱动层实现 lcd.h 的接口。
 *
 @section Platform
	-# 
 @section Library
	-# 
- 2015-08-12,Menglong Woo,MenglongWoo@163.com
 	- TestOrder

 * @attention
 *
 * ATTENTION
 *
 * <h2><center>&copy; COPYRIGHT </center></h2>
*/
#include "lcdconf.h"
#include "lcd.h"
#include "2440addr.h"
#include "board.h"
#include "gl_type.h"
#include "gl_ui.h"

#include "zimo_st9.h"
#include "zimo_st20.h"
#include "minishell_core.h"
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
// #define LCD_XSIZE_TFT	 				(480)	
// #define LCD_YSIZE_TFT				 	(272)
extern volatile unsigned long lcd_buffer[LCD_YSIZE_TFT][LCD_XSIZE_TFT] ;//= { 0, } ;
extern volatile unsigned long lcd_buffer2[LCD_YSIZE_TFT][LCD_XSIZE_TFT] ;//= { 0, } ;



int do_line(int argc,char **argv)
{
	
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
	return 0;

}
W_BOOT_CMD(lcdspeed,do_line,"Test LCD draw speed (all screen)");

int do_lcdorder(int argc,char **argv)
{ 
	long arr[] = {
		RGB(255,0,0),RGB(0,255,0),RGB(0,0,255),
		RGB(240,192,0),RGB(240,0,192),
		RGB(192,240,0),RGB(0,240,192),
		RGB(192,0,240),RGB(0,192,240),
	};

	lcd_rectangle2(0, 0, 480,272, RGB(255,255,255));// red

	// 验证所有RGB三色的顺序
	for (int i = 0; i < sizeof(arr)/sizeof(long); i++) {
		lcd_rectangle2(100 +i*20, 20, 20,100, arr[i]);// red	
	}



	static volatile struct s3c_lcd_reg  *preg_lcd;
	preg_lcd = (struct s3c_lcd_reg*)0x4d000000;

	lcd_rectangle2(0+1 , 0+1, 3,3, RGB(255,0,0));		 // 左上角绘制点
	lcd_rectangle2(479-3, 271-3, 3,3, RGB(255,0,0));// 右小角绘制点
	
	// preg_lcd->lcdcon1 |= (1 << 0);



	gl_ui_setlib(p_zm_ascii_st20,16,27,p_zm_step_st20);
	gl_ui_setbkmode(BACKFILL);//背景填充模式
	gl_ui_setfontcolor(RGB(114,144,217));
	gl_ui_setbkcolor(RGB(39,40,34));
	gl_ui_setpencolor(RGB(235,34,209));
	
	gl_text(10,200,"HelloWorld.",-1);
	rDISRC0= (volatile unsigned int)&lcd_buffer2[0][0];         // Nand flash data register
	rDISRCC0=(0<<1) | (0<<0); //arc=AHB,src_addr=fix
	rDIDST0=(unsigned)(lcd_buffer);
	rDIDSTC0=(0<<1) | (0<<0); //dst=AHB,dst_addr=inc;
	rDCON0=(1<<31)|(1<<30)|(0<<29)|(1<<28)|(1<<27)|(0<<23)|(1<<22)|(2<<20)|((480*272/4));
	rDMASKTRIG0=(1<<1)|(1<<0);
	while(0 != rDMASKTRIG0);


	rDISRC0= (volatile unsigned int)&lcd_buffer2[0][0];         // Nand flash data register
	rDISRCC0=(0<<1) | (0<<0); //arc=AHB,src_addr=fix
	rDIDST0=(unsigned)(lcd_buffer);
	rDIDSTC0=(0<<1) | (0<<0); //dst=AHB,dst_addr=inc;
	rDCON0=(1<<31)|(1<<30)|(0<<29)|(1<<28)|(1<<27)|(0<<23)|(1<<22)|(2<<20)|((480*272/4));
	rDMASKTRIG0=(1<<1)|(1<<0);
	while(0 != rDMASKTRIG0);

	return 0;
}
W_BOOT_CMD(lcdorder,do_lcdorder,"Test gl_ui library");


void delay(long t)
{
	for (int i = 0;i < t;i++) {
		for (int j = 0;j < 10000;j++);
	}
}
int do_testrgb(int argc,char **argv)
{
	long arr[] = {RGB(255,0,0),RGB(0,255,0),RGB(0,0,255)};
	
	for (int i = 0;i < 3;i++) {
		lcd_rectangle2(0, 0, 480,272, arr[i]);// red
		rDISRC0= (volatile unsigned int)&lcd_buffer2[0][0];         // Nand flash data register
		rDISRCC0=(0<<1) | (0<<0); //arc=AHB,src_addr=fix
		rDIDST0=(unsigned)(lcd_buffer);
		rDIDSTC0=(0<<1) | (0<<0); //dst=AHB,dst_addr=inc;
		rDCON0=(1<<31)|(1<<30)|(0<<29)|(1<<28)|(1<<27)|(0<<23)|(1<<22)|(2<<20)|((480*272/4));
		rDMASKTRIG0=(1<<1)|(1<<0);
		while(0 != rDMASKTRIG0);
		delay(500);
	}
	return 0;
}
W_BOOT_CMD(lcdrgb,do_testrgb,"Test break point");

int do_lcdtext(int argc,char **argv)
{
	if (argc == 1) {
		printk("input error\n\tlcdtext <text>\n");
		return 0;
	}
	gl_text(0,0,argv[1],-1);

	rDISRC0= (volatile unsigned int)&lcd_buffer2[0][0];         // Nand flash data register
	rDISRCC0=(0<<1) | (0<<0); //arc=AHB,src_addr=fix
	rDIDST0=(unsigned)(lcd_buffer);
	rDIDSTC0=(0<<1) | (0<<0); //dst=AHB,dst_addr=inc;
	rDCON0=(1<<31)|(1<<30)|(0<<29)|(1<<28)|(1<<27)|(0<<23)|(1<<22)|(2<<20)|((480*272/4));
	rDMASKTRIG0=(1<<1)|(1<<0);
	while(0 != rDMASKTRIG0);
	return 0;	
}
W_BOOT_CMD(lcdtext,do_lcdtext,"Test gl_ui library");



int do_lcdpower(int argc,char **argv)
{
	if (argc == 2 && argv[1][0] == '1') {
		lcd_power(1);
	}
	else if (argc == 2 && argv[1][0] == '0') {
		lcd_power(0);
	}
	return 0;
}
W_BOOT_CMD(lcdpower,do_lcdpower,"LCD control on/off(1/0)"); 

int do_lcdbl(int argc,char **argv)
{
	if (argc == 2 && argv[1][0] == '1') {
		lcd_blacklight(1);
	}
	else if (argc == 2 && argv[1][0] == '0') {
		lcd_blacklight(0);
	}
	return 0;
}
W_BOOT_CMD(lcdbl,do_lcdbl,"LCD blacklight on/off(1/0)");

