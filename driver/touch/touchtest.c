/**
 ******************************************************************************
 * @file	touchtest.c
 * @brief	裸机 LCD 驱动接口

	LCD 驱动测试程序，在运行该测试程序之前首先保证驱动层实现 lcd.h 的接口。
 *
 @section Platform
	-# 
 @section Library
	-# 
- 2015-08-27,Menglong Woo,MenglongWoo@163.com
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
#include "s3c2440_touch.h"

int do_tcpic(int argc,char **argv)
{ 
	struct point pt[5] = {
		{60,                 60},
		{LCD_XSIZE_TFT - 60, 60},
		{60,                 LCD_YSIZE_TFT - 60},
		{LCD_XSIZE_TFT -60 , LCD_YSIZE_TFT - 60},
		{LCD_XSIZE_TFT / 2, LCD_YSIZE_TFT / 2},
	};
	struct point tc;
// { 40,  LCD_YSIZE_TFT - 40 },
// { LCD_XSIZE_TFT / 2, LCD_YSIZE_TFT / 2 },
// { LCD_XSIZE_TFT - 40, 40 }
// { 300,  LCD_YSIZE_TFT - 40 },
// 	{ 40, 100 },
// 	{ LCD_XSIZE_TFT - 40, LCD_YSIZE_TFT -40 },

	// lcd_rectangle2(300,  LCD_YSIZE_TFT - 40,4,4, RGB(255,0,0)); 
	// lcd_rectangle2(40, 100,4,4, RGB(255,0,0));
	// lcd_rectangle2(LCD_XSIZE_TFT - 40, LCD_YSIZE_TFT -40,4,4, RGB(255,0,0)); 

	lcd_rectangle(0,  0 , LCD_XSIZE_TFT, LCD_YSIZE_TFT, RGB(0,0,0)); 		
	// for (int i = 0; i < sizeof(pt)/ sizeof(struct point); i++) {
		lcd_rectangle(pt[0].x,  pt[0].y ,4,4, RGB(255,0,0)); 		
	// }

	int k = 1;
	struct point adj[5];

	while(k < 5) {

		if (gettouch(&adj[k-1])) {
			printk("werr\n");
			for (int i = 0; i < sizeof(pt)/ sizeof(struct point); i++) {
				lcd_rectangle(pt[i].x,  pt[i].y ,4,4, RGB(0,0,0)); 		
			}
			lcd_rectangle(pt[k].x,  pt[k].y ,4,4, RGB(255,0,0)); 		
			k++;
		}
		if (0 != s_peekchar()) {
			break;
		}
	}
	return 0;
	// // lcd_rectangle2(60,  60 ,4,4, RGB(255,0,0)); 
	// // lcd_rectangle2(LCD_XSIZE_TFT - 60, 60,4,4, RGB(255,0,0));
	// // lcd_rectangle2(LCD_XSIZE_TFT - 60, LCD_YSIZE_TFT -60,4,4, RGB(255,0,0)); 



	// lcd_rectangle2(0+1 , 0+1, 3,3, RGB(255,0,0));		 // 左上角绘制点
	// lcd_rectangle2(479-3, 271-3, 3,3, RGB(255,0,0));// 右小角绘制点
	
	// rDISRC0= (volatile unsigned int)&lcd_buffer2[0][0];         // Nand flash data register
	// rDISRCC0=(0<<1) | (0<<0); //arc=AHB,src_addr=fix
	// rDIDST0=(unsigned)(lcd_buffer);
	// rDIDSTC0=(0<<1) | (0<<0); //dst=AHB,dst_addr=inc;
	// rDCON0=(1<<31)|(1<<30)|(0<<29)|(1<<28)|(1<<27)|(0<<23)|(1<<22)|(2<<20)|((480*272/4));
	// rDMASKTRIG0=(1<<1)|(1<<0);
	// while(0 != rDMASKTRIG0);


	// rDISRC0= (volatile unsigned int)&lcd_buffer2[0][0];         // Nand flash data register
	// rDISRCC0=(0<<1) | (0<<0); //arc=AHB,src_addr=fix
	// rDIDST0=(unsigned)(lcd_buffer);
	// rDIDSTC0=(0<<1) | (0<<0); //dst=AHB,dst_addr=inc;
	// rDCON0=(1<<31)|(1<<30)|(0<<29)|(1<<28)|(1<<27)|(0<<23)|(1<<22)|(2<<20)|((480*272/4));
	// rDMASKTRIG0=(1<<1)|(1<<0);
	// while(0 != rDMASKTRIG0);



	return 0;
}

W_BOOT_CMD(tcp,do_tcpic,"LCD control on/off(1/0)"); 