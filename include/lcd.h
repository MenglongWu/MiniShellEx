/**
 ******************************************************************************
 * @file	lcd.h
 * @brief	裸机 LCD 驱动接口

定义LCD裸机驱动接口，平台关于lcd的配置可以写在lcdconf.h中，必须实现:\n
lcd_init，lcd_power，lcd_blacklight
LCD实际显存为lcd_buffer，对于采用双显存使能DMA的驱动，还需要一块玉lcd_buffer相同
大小的显存lcd_buffer2


 *
 @section Platform
	-# 
 @section Library
	-# 
- 20xx-xx-xx,author,email
 	- brief

 * @attention
 *
 * ATTENTION
 *
 * <h2><center>&copy; COPYRIGHT </center></h2>
*/


#ifndef _LCD_H_
#define _LCD_H_
#include "lcdconf.h"

#define RGB8(r,g,b)		// todo
#define RGB565(r,g,b)		((unsigned short) ( ((r) >> 3) << 11 ) | ( ((g) >> 2) << 5) | ( ((b) >> 3) << 0) )
#define RGB24(r,g,b)		 	((unsigned long) ( (r) << 16 )        | ( (g) << 8)        | ( (b) << 0) )


// Define RGB macro
#if CONFIG_LCD_BPP == 24
#define RGB(r,g,b)			RGB24((r),(g),(b))

#elif CONFIG_LCD_BPP == 16
#define RGB(r,g,b)			RGB565((r),(g),(b))

#elif CONFIG_LCD_BPP == 8
#define RGB(r,g,b)			RGB8((r),(g),(b))

#else
#warning "CONFIG_LCD_BPP only with 8/16/24 check out lcdconf.h"
#define RGB(r,g,b) RGB565((r),(g),(b))
#endif



extern volatile unsigned long lcd_buffer[LCD_YSIZE_TFT][LCD_XSIZE_TFT] ;//= { 0, } ;
extern volatile unsigned long lcd_buffer2[LCD_YSIZE_TFT][LCD_XSIZE_TFT] ;//= { 0, } ;

void lcd_init();
void lcd_power(unsigned int v);
void lcd_blacklight(unsigned int v);
void lcd_rectangle( int x0,int y0,int width,int height, unsigned long c);
void lcd_rectangle2( int x0,int y0,int width,int height, unsigned long c);

void TestRGB();
void TestOrder();
#endif
