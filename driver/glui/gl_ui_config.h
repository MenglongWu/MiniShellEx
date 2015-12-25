/*
* File: gl_ui_config.h
* Brief:移植配置文件，只需要修改两个地方
	1、使用到的字库
	2、相关设备的画点函数用GL_SETPOINT宏替换
* 	Date			Author			Note
	2012.10.14		MenglongWu		手持式稳定光源移植
*/
#ifndef _GL_UI_CONFIG_H_
#define _GL_UI_CONFIG_H_

//选择需要的字库
//#include "lcd\\zimo_yh16.h"//微软雅黑16
#include "zimo_st9.h"//宋体9
#include "zimo_st20.h"//宋体20
//#include "lcd\\led19264.h"//底层硬件接口
//使用到的显示设备
// #include "glcd.h"
#include "lcd.h"


//显示设备接口
#define GL_UI_OLD_DEV//为了兼容以前编写的老设备，接口不一样
static inline void setpixel(int x,int y,int c)
{
	lcd_buffer2[y][x] = c;
}
#define GL_SETPOINT(x,y,color) setpixel(x,y,color)//LED_Set_Point(x,y)//手持稳定光源显示器画点函数

////////////////////////////////////////////////////

#endif

