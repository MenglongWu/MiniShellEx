/**
 ******************************************************************************
 * @file	s3c2440_touch.c
 * @brief	s3c2440 电阻屏触屏驱动

irq/driver/touch/s3c2440_touch.c
 *
 @section Platform
	-# 
 @section Library
	-# 
- 2015-8-25,Menglong Woo,MenglongWoo@163.com
 	- brief

*/
#include "autoconfig.h"
#include "2440addr.h"
#include "board.h"
#include "s3c2440_irq.h"
#include "minishell_core.h"
#include "lcdconf.h"
#include "lcd.h"
#include "s3c2440_touch.h"

int count=0;
volatile int xdata, ydata;
volatile int x[4], y[4];

int event = 0;

typedef struct Matrix //typedef 矩阵列？？
{						
long double An,  
            Bn,     
            Cn,   
            Dn,    
            En,    
            Fn,     
            Divider ;
} Matrix ;

float A1 = -0.010052, B1 = -0.615400, C1 = 563.040955;
float A2 = 0.348466, B2 = 0.000536, C2 = -44.085407;
float K = 358141.000000;

typedef	struct POINT  //typedef 坐标点
{
   unsigned short  x;
   unsigned short  y;
}Coordinate;
Coordinate ScreenSample[3];
/* LCD上的坐标 */

Coordinate DisplaySample[3] =   {

	// { 300,  LCD_YSIZE_TFT - 40 },
	// { 40, 100 },
	// { LCD_XSIZE_TFT - 40, LCD_YSIZE_TFT -40 },

	{ 60, 60 },
	{ LCD_XSIZE_TFT - 60, 60 },
	{ LCD_XSIZE_TFT - 60, LCD_YSIZE_TFT -60 },

	// { 60,  LCD_YSIZE_TFT - 60 },
	// { LCD_XSIZE_TFT - 60, 60 },
	// { LCD_XSIZE_TFT - 60, LCD_YSIZE_TFT -60 },
	                            } ;


Matrix matrixPtr[3];

#define _Xd(c) (DisplaySample[c].x)
#define _Yd(c) (DisplaySample[c].y)
#define _X(c) (ScreenSample[c].x)
#define _Y(c) (ScreenSample[c].y)
void getDisplayPoint(Coordinate * displayPtr,
                     Coordinate * screenPtr,
                     Matrix * matrixPtr );
void getDisplayPoint2(Coordinate *m_adj, Coordinate *m_touch);
#define GATE_TC (10)



int gettouch(struct point *pt)
{
	printk("event = %d\n");
	if (event) {
		event = 0;
		pt->x = x[1];
		pt->y = y[1];
		return 1;
	}
	return 0;
}
int fliter()
{
	int avx,avy;
	int det;
	avx = (x[0] + x[1] + x[2] + x[3]) >> 2;
	avy = (y[0] + y[1] + y[2] + y[3]) >> 2;

	det = x[0] - x[1] > 0 ? x[0] - x[1] : x[1] - x[0];
	if (det > GATE_TC) {
		return 0;
	}
	det = x[2] - x[3] > 0 ? x[2] - x[3] : x[3] - x[2];
	if (det > GATE_TC) {
		return 0;
	}
	det = y[0] - y[1] > 0 ? y[0] - y[1] : y[1] - y[0];
	if (det > GATE_TC) {
		return 0;
	}
	det = y[2] - y[3] > 0 ? y[2] - y[3] : y[3] - y[2];
	if (det > GATE_TC) {
		return 0;
	}
	xdata = avx;
	ydata = avy;
	return 1;

}


static inline void start_xy_adc()
{
	rADCCON |= (1 << 0);
	rADCTSC |= (1 << 2);
}

inline void wait_touch_up()
{
	rADCTSC = 0x1d3;
}

inline void wait_touch_down()
{
	rADCTSC = 0x0d3;
}



void sub_touch_down_up(void)
{
	if (rADCDAT0 & (1 << 15)) {
		printk("touch up\n");
		wait_touch_down();
	}
	else {
		printk("touch down  ");
		// wait_touch_up();
		start_xy_adc();
	}
	
	// 先清除子中断，后清除总中断
	ClearSubPending(BIT_SUB_TC);
	
	
}

Coordinate dis,scr;

void sub_adc()
{
	x[count] = rADCDAT0 & 0x3ff;
	y[count] = rADCDAT1 & 0x3ff;
	count++;

	if (count >= 4) {
		count = 0;
		if (fliter()) {
			scr.x = x[1];
			scr.y = y[1];
			// getDisplayPoint(&dis,&scr,matrixPtr );
			getDisplayPoint2(&dis,&scr );
			
			printk("x %d y %d    dx %d dy %d  ", x[1], y[1] ,dis.x, dis.y);
			wait_touch_up();
			event = 1;

		}
		else {
			start_xy_adc();	
		}
		
	}
	else {
		start_xy_adc();	
	}
	ClearSubPending(BIT_SUB_ADC);
}

static int times = 0;
void sub_adc_adjtc()
{
	
	x[count] = rADCDAT0 & 0x3ff;
	y[count] = rADCDAT1 & 0x3ff;
	count++;

	if (count >= 4) {
		count = 0;
		if (fliter()) {
			printk("times %d x %d y %d  ", times, x[1], y[1]);
			wait_touch_up();
			ScreenSample[times].x = x[1];
			ScreenSample[times].y = y[1];
			times++;
			event = 1;
			if (times >= 3) {
				times = 0;
			}
		}
		else {
			start_xy_adc();	
		}
		
	}
	else {
		start_xy_adc();	
	}
	ClearSubPending(BIT_SUB_ADC);
}
void int_adc(void)
{
	if (rSUBSRCPND & BIT_SUB_TC) {
		sub_touch_down_up();
	}
	else {
		sub_adc();	
	}

	ClearPending(BIT_ADC);
}

void int_adc_adjtc(void)
{
	if (rSUBSRCPND & BIT_SUB_TC) {
		sub_touch_down_up();
	}
	else {
		sub_adc_adjtc();	
	}

	ClearPending(BIT_ADC);
}

void init_touch()
{
	

	rADCCON=(1<<14)+(50<<6);   //ADCPRS En, ADCPRS Value

	rADCDLY=(-1);
	printk("\nTouch Screen test\n");

	rADCTSC=0x0d3;  //Wfait,XP_PU,XP_Dis,XM_Dis,YP_Dis,YM_En

	// pISR_ADC = (int)AdcTsAuto;

	// rINTMSK=~BIT_ADC;       //ADC Touch Screen Mask bit clear
	// rINTSUBMSK=~(BIT_SUB_TC);
	
	EnableIrq(BIT_ADC);
	EnableSubIrq(BIT_SUB_TC);
	EnableSubIrq(BIT_SUB_ADC);

	printk("\nPress any key to quit!\n");
	printk("\nStylus Down, please...... \n");

	
	irq_request(ISR_ADC_OFT, int_adc);
	// irq_request(BIT_SUB_TC, touch_down_up);
	printk("Touch Screen Test is Finished!!!\n");
}




void getDisplayPoint(Coordinate * displayPtr,
                     Coordinate * screenPtr,
                     Matrix * matrixPtr )
{
  

  
    /* XD = AX+BY+C */        //触摸屏与显示屏之间的转换公式，待研究。。
    displayPtr->x = ( (matrixPtr->An * screenPtr->x) + 
                      (matrixPtr->Bn * screenPtr->y) + 
                       matrixPtr->Cn 
                    ) / matrixPtr->Divider ;
	/* YD = DX+EY+F */        
    displayPtr->y = ( (matrixPtr->Dn * screenPtr->x) + 
                      (matrixPtr->En * screenPtr->y) + 
                       matrixPtr->Fn 
                    ) / matrixPtr->Divider ;

} 
void getDisplayPoint2(Coordinate *m_adj, Coordinate *m_touch)
{
		m_adj->x = A1 * m_touch->x + B1 * m_touch->y + C1;
		m_adj->y = A2 * m_touch->x + B2 * m_touch->y + C2;
}

void setCalibrationMatrix( Coordinate * displayPtr,	  //注意形参，两组坐标，一组矩阵值
                          Coordinate * screenPtr,
                          Matrix * matrixPtr)
{
	matrixPtr->Divider = ((screenPtr[0].x - screenPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
	                    ((screenPtr[1].x - screenPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;

	printk("%d %d %d\n",
		((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)),
		((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)),
		((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
		((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) );


	matrixPtr->An = ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
	            ((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;	
	/*	  matrixPtr->An =15340;		*/
	/* B＝((X0－X2) (XD1－XD2)－(XD0－XD2) (X1－X2))／K	*/
	matrixPtr->Bn =((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].x - displayPtr[2].x)) - 
	            ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].x - screenPtr[2].x)) ;	  
	/*	 matrixPtr->Bn =-573300;	*/
	/* C＝(Y0(X2XD1－X1XD2)+Y1(X0XD2－X2XD0)+Y2(X1XD0－X0XD1))／K */
	matrixPtr->Cn =(screenPtr[2].x * displayPtr[1].x - screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y +
	            (screenPtr[0].x * displayPtr[2].x - screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y +
	            (screenPtr[1].x * displayPtr[0].x - screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y ; 
	/*	matrixPtr->Cn = 242160440;	*/			 
	/* D＝((YD0－YD2) (Y1－Y2)－(YD1－YD2) (Y0－Y2))／K	*/
	matrixPtr->Dn = ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].y - screenPtr[2].y)) - 
	            ((displayPtr[1].y - displayPtr[2].y) * (screenPtr[0].y - screenPtr[2].y)) ;	 	
	/*	matrixPtr->Dn =-410850;	   */
	/* E＝((X0－X2) (YD1－YD2)－(YD0－YD2) (X1－X2))／K	*/
	matrixPtr->En = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].y - displayPtr[2].y)) - 
	            ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].x - screenPtr[2].x)) ;	  
	/*	matrixPtr->En =	3630;	 */
	/* F＝(Y0(X2YD1－X1YD2)+Y1(X0YD2－X2YD0)+Y2(X1YD0－X0YD1))／K */
	matrixPtr->Fn = (screenPtr[2].x * displayPtr[1].y - screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y +
	            (screenPtr[0].x * displayPtr[2].y - screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y +
	            (screenPtr[1].x * displayPtr[0].y - screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y ;   

	            printk("\n%d %d\n%d %d\n%d %d\n", 
	            	screenPtr[0].x,screenPtr[0].y,
	            	screenPtr[1].x,screenPtr[1].y,
	            	screenPtr[2].x,screenPtr[2].y);
	            printk("\n%d %d\n%d %d\n%d %d\n", 
	            	displayPtr[0].x,displayPtr[0].y,
	            	displayPtr[1].x,displayPtr[1].y,
	            	displayPtr[2].x,displayPtr[2].y);
	            printk("\n%d %d %d %d %d %d %d",
	            	(int)matrixPtr->An,
	            	(int)matrixPtr->Bn,
	            	(int)matrixPtr->Cn,
	            	(int)matrixPtr->Dn,
	            	(int)matrixPtr->En,
	            	(int)matrixPtr->Fn,
	            	(int)matrixPtr->Divider);
}

void adj()
{
	// 3 点校准做算法
	K = (_X(0) - _X(2)) * (_Y(1) - _Y(2)) - (_X(1) - _X(2)) * (_Y(0) - _Y(2));

	A1 = ( (_Xd(0) - _Xd(2)) * (_Y(1) - _Y(2)) - (_Xd(1) - _Xd(2)) * (_Y(0) - _Y(2)) ) / K;
	B1 = ( (_X(0) - _X(2)) * (_Xd(1) - _Xd(2)) - (_Xd(0) - _Xd(2)) * (_X(1) - _X(2)) ) / K;
	C1 = ( _Y(0) * (_X(2) * _Xd(1) - _X(1) * _Xd(2)) +
	    _Y(1) * (_X(0) * _Xd(2) - _X(2) * _Xd(0)) +
	    _Y(2) * (_X(1) * _Xd(0) - _X(0) * _Xd(1))) / K;

	A2 = ( (_Yd(0) - _Yd(2)) * (_Y(1) - _Y(2)) - (_Yd(1) - _Yd(2)) * (_Y(0) - _Y(2)) ) / K;
	B2 = ( (_X(0) - _X(2)) * (_Yd(1) - _Yd(2)) - (_Yd(0) - _Yd(2)) * (_X(1) - _X(2)) ) / K;
	C2 = ( _Y(0) * (_X(2) * _Yd(1) - _X(1) * _Yd(2)) +
	    _Y(1) * (_X(0) * _Yd(2) - _X(2) * _Yd(0)) +
	    _Y(2) * (_X(1) * _Yd(0) - _X(0) * _Yd(1))) / K;

	printk("\n[%d %d %d %d %d %d %d ]\n", (
		(int)(A1*1000), (int)(B1*1000), (int)(C1*1000), 
		(int)(A2*1000), (int)(B2*1000), (int)(C2*1000), 
		(int)(K*1000)));
}

int do_tcpic(int argc,char **argv);
int do_adjtc_start(int argc,char **argv)
{
	times = 0;
	irq_request(ISR_ADC_OFT, int_adc_adjtc);
#ifdef CONFIG_TOUCH_TEST
	do_tcpic(0,0);
#endif
	
	
	return 0;
}

W_BOOT_CMD(tcs,do_adjtc_start,"LCD blacklight on/off(1/0)");

int do_adjtc_end(int argc,char **argv)
{
	setCalibrationMatrix(DisplaySample, ScreenSample,matrixPtr);
	adj();
	irq_request(ISR_ADC_OFT, int_adc);
	return 0;
}

W_BOOT_CMD(tce,do_adjtc_end,"LCD blacklight on/off(1/0)");

int do_tc_draw(int argc,char **argv)
{
	int lasty, lastx;	
	struct point pt;
	while(1) {
		if (0 != s_peekchar()) {
			break;
		}
		if (event) {
			lcd_rectangle(lastx,lasty,20,20,RGB(0,0,0));
			lcd_rectangle(dis.x,dis.y,20,20,RGB(255,0,0));
			printk("[%d %d ]\n",dis.x, dis.y);

			lastx = dis.x;
			lasty = dis.y;
			event = 0;
		}
		
	}
	return 0;
}

W_BOOT_CMD(tcd, do_tc_draw,"LCD blacklight on/off(1/0)");

