/*
* File: gl_ui_config.h
* Brief:��ֲ�����ļ���ֻ��Ҫ�޸������ط�
	1��ʹ�õ����ֿ�
	2������豸�Ļ��㺯����GL_SETPOINT���滻
* 	Date			Author			Note
	2012.10.14		MenglongWu		�ֳ�ʽ�ȶ���Դ��ֲ
*/
#ifndef _GL_UI_CONFIG_H_
#define _GL_UI_CONFIG_H_

//ѡ����Ҫ���ֿ�
//#include "lcd\\zimo_yh16.h"//΢���ź�16
#include "zimo_st9.h"//����9
#include "zimo_st20.h"//����20
//#include "lcd\\led19264.h"//�ײ�Ӳ���ӿ�
//ʹ�õ�����ʾ�豸
// #include "glcd.h"
#include "lcd.h"


//��ʾ�豸�ӿ�
#define GL_UI_OLD_DEV//Ϊ�˼�����ǰ��д�����豸���ӿڲ�һ��
static inline void setpixel(int x,int y,int c)
{
	lcd_buffer2[y][x] = c;
}
#define GL_SETPOINT(x,y,color) setpixel(x,y,color)//LED_Set_Point(x,y)//�ֳ��ȶ���Դ��ʾ�����㺯��

////////////////////////////////////////////////////

#endif

