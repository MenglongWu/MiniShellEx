#ifndef _GL_UI_H_
#define _GL_UI_H_
#include "gl_type.h"

#define TRANSPARENT 0	//±³¾°Í¸Ã÷
#define BACKFILL 1		//ÓÃ±³¾°Ìî³ä

// #define RGB(r,g,b) ((uint32_t)(r << 16 | g << 8 | b)
// #define RGB16(r,g,b) ((uint16_t)((r & 0xF8)<< 8 | (g & 0xFC)<< 3 | (b & 0xF8) >> 3))
struct gl_ui_arraylib
{
	uint16_t w;
	uint16_t h;
	uint16_t step[128];
	uint8_t *lib[128];
	
	//uint8_t *(*lib[128]);
};
void gl_ui_hook(
	void (*setpoint)(unsigned short x, unsigned short y,uint32_t color));
void gl_setpoint(unsigned short x, unsigned short y,uint32_t color);
uint32_t gl_ui_setbkmode(uint32_t bkmode);
uint32_t gl_ui_setfontcolor(uint32_t color);
uint32_t gl_ui_setbkcolor(uint32_t color);
uint32_t gl_ui_setpencolor(uint32_t  color);
uint32_t gl_ui_setbrushcolor(uint32_t  color);
unsigned char gl_ui_setlib(unsigned char *lib[128],uint16_t w,uint16_t h,uint8_t *step);
//unsigned char gl_ui_setlib(unsigned char *lib[128],uint16_t w,uint16_t h,uint16_t step);
//unsigned char gl_ui_setlib(unsigned char *(*lib[128]),uint16_t w,uint16_t h);
//unsigned char gl_ui_setlib(unsigned char *lib[128]);
//struct gl_ui_arraylib gl_ui_setlib(struct gl_ui_arraylib *lib);

void gl_horizon_line(uint16_t x1,uint16_t x2,uint16_t y);//,uint16_t color);
void gl_vertical_line(uint16_t x,uint16_t y1,uint16_t y2);//,uint16_t color);
void gl_picture(uint8_t *buf,uint16_t px,uint16_t py,uint16_t w,uint16_t h);
void gl_text(uint16_t x,uint16_t y,uint8_t *str,uint16_t num);//,uint16_t color,uint16_t bkcolor);
void gl_rect(
	uint16_t x,uint16_t y,
	uint16_t w,uint16_t h);//,
	//uint16_t linecolor);
void gl_fill_rect(
	uint16_t x,uint16_t y,
	uint16_t w,uint16_t h);//,
	//uint16_t linecolor,uint16_t fillcolor);
#endif

