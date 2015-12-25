/*
* File: gl_ui.c
* Brief:显示器绘图应用接口
*	Date			Author			Note
	2012-10-17		MenglongWu		基本接口实现，后期再调整
*/
#include "gl_type.h"
#include "gl_ui_config.h"
#include "gl_ui.h"

//内部全局变量
static void (*_setpoint)(unsigned short x, unsigned short y,uint32_t  color);
static unsigned short _fcolor = 0x0000;		//字体颜色
static unsigned short _bkcolor = 0xffff;	//背景颜色
//static unsigned short _alpha = 100;		//透明度
static unsigned char  _bkmode = TRANSPARENT;//背景填充模式
static unsigned short _pencolor = 0x0000;	//笔颜色
static unsigned short _brushcolor = 0x0000;		//画刷颜色
unsigned char *_zm_ascii[128] = {0};		//字库
static struct gl_ui_arraylib _zm_lib;		//作废了

/*
* Function: 设置绘点函
* Parameters:
	回调函数，需要底层硬件接口符合回调函数的参数传递规则。
	由于以前写的函数接口不一定是按照此规则编写的，
	当使用宏GL_UI_OLD_DEV的时候，就用GL_SETPOINT代替回调函数
* Return:
* Remarks:
*/
void gl_ui_hook(
	void (*setpoint)(unsigned short x, unsigned short y,uint32_t  color))
{
	_setpoint = setpoint;
}
/*
* Function: 设置背景填充模式
* Parameters:
	bkmode可以是TRANSPARENT或BACKFILL
* Return:上一次的填充模式
* Remarks:
*/
uint32_t gl_ui_setbkmode(uint32_t bkmode)
{
	unsigned short old = bkmode;
	_bkmode = bkmode;
	return old;
}

/*
* Function: 设置文字颜色
* Parameters:
	根据设备选择RGB()或RGB16()
* Return:上一次的设置值
* Remarks:
*/
uint32_t gl_ui_setfontcolor(uint32_t  color)
{
	unsigned short old = _fcolor;
	_fcolor = color;
	return old;
}
/*
* Function: 设置背景色
* Parameters:
	根据设备选择RGB()或RGB16()
* Return:上一次的设置值
* Remarks:
*/
uint32_t gl_ui_setbkcolor(uint32_t  color)
{
	unsigned short old = _bkcolor;
	_bkcolor = color;
	return old;
}

/*
* Function: 设置画笔颜色
* Parameters:
	根据设备选择RGB()或RGB16()
* Return:上一次的设置值
* Remarks:
*/
uint32_t gl_ui_setpencolor(uint32_t  color)
{
	unsigned short old = _pencolor;
	_pencolor = color;
	return old;
}

/*
* Function: 设置画刷颜色
* Parameters:
	根据设备选择RGB()或RGB16()
* Return:上一次的设置值
* Remarks:
*/
uint32_t gl_ui_setbrushcolor(uint32_t  color)
{
	unsigned short old = _bkcolor;
	_brushcolor = color;
	return old;
}

/*
* Function: 设置文字颜色
* Parameters:
	lib:查阅相应的p_zm_ascii_xxx
	w,h:查阅相应的ZM_xxx_W,ZM_xxx_H
	lib:查阅相应的p_zm_step_xxx
* Return:
* Remarks:
*/
unsigned char gl_ui_setlib(unsigned char *lib[128],uint16_t w,uint16_t h,uint8_t *step)
//struct gl_ui_arraylib gl_ui_setlib(struct gl_ui_arraylib *lib)
{
// 	unsigned char old,*news;// = p_zm_ascii
// 	unsigned short i;
// 	for(i = 0;i < 128;++i)
// 		//_zm_ascii[i] = p_zm_ascii[i];
// 		_zm_ascii[i] = lib[i];
// 	return old;
	//struct gl_ui_arraylib old;
	//struct gl_ui_arraylib old;
	uint16_t i;
	for(i = 0;i < 128;++i) {
		_zm_lib.lib[i] = lib[i];
		_zm_lib.step[i] = *step++;
	}
	
	_zm_lib.w = w;
	_zm_lib.h = h;
	
	
	return 0;
	
}
/*
* Function: 基本绘点函数
* Parameters:
	x,y:绘制的位置
	color:绘点颜色，对于只有黑白的显示器，颜色只能是-1（白色）和0（黑色）
	相应的gl_ui_setfontcol,gl_ui_setbkcolor等函数的color也是这样的设定
* Return:
* Remarks:
	应用层永远不会直接访问此函数，底层调用
*/
void gl_setpoint(unsigned short x, unsigned short y,uint32_t  color)
{
#ifdef GL_UI_OLD_DEV
	GL_SETPOINT(x,y,color);
#else
	_setpoint(x,y,color);
#endif
}
/*
* Function: 绘制水平线
* Parameters:
* Return:
* Remarks:
*/
void gl_horizon_line(uint16_t x1,uint16_t x2,uint16_t y)
{
	uint16_t i;
	for(i=x1; i<=x2; i++)
		gl_setpoint(i,y,_pencolor);
}

/*
* Function: 绘制垂直线
* Parameters:
* Return:
* Remarks:
*/
void gl_vertical_line(uint16_t x,uint16_t y1,uint16_t y2)
{
	uint16_t i;
	for(i=y1; i<=y2; i++)
		gl_setpoint(x,i,_pencolor);
}

/*
* Function: 绘制矩阵点
* Parameters:
* Return:
* Remarks:
	矩阵点只能是0和1，1处使用_pencolor绘制，背景色用根据透明度情况不绘制
	或使用背景画刷填充
*/
void gl_picture(uint8_t *buf,uint16_t px,uint16_t py,uint16_t w,uint16_t h)
{
	uint16_t x,y;
	uint8_t *tbuf = buf;
	w = w + px;
	h = h + py;
	if(_bkmode == TRANSPARENT) {
		for(y = py;y < h;++y) {
			for(x = px;x < w;) {	
				if(*tbuf & 0x80) gl_setpoint(x,y, _pencolor);
				x++;
				if(*tbuf & 0x40) gl_setpoint(x,y, _pencolor);
				x++;
				if(*tbuf & 0x20) gl_setpoint(x,y, _pencolor);
				x++;
				if(*tbuf & 0x10) gl_setpoint(x,y, _pencolor);
				x++;
				if(*tbuf & 0x08) gl_setpoint(x,y, _pencolor);
				x++;
				if(*tbuf & 0x04) gl_setpoint(x,y, _pencolor);
				x++;
				if(*tbuf & 0x02) gl_setpoint(x,y, _pencolor);
				x++;
				if(*tbuf & 0x01) gl_setpoint(x,y, _pencolor);
				x++;
				tbuf++;
			}
		}
	}
	else {
		for(y = py;y < h;++y) {
			for(x = px;x < w;) {	
				gl_setpoint(x++,y,*tbuf & 0x80 ? _pencolor : _bkcolor);
				gl_setpoint(x++,y,*tbuf & 0x40 ? _pencolor : _bkcolor);
				gl_setpoint(x++,y,*tbuf & 0x20 ? _pencolor : _bkcolor);
				gl_setpoint(x++,y,*tbuf & 0x10 ? _pencolor : _bkcolor);
				gl_setpoint(x++,y,*tbuf & 0x08 ? _pencolor : _bkcolor);
				gl_setpoint(x++,y,*tbuf & 0x04 ? _pencolor : _bkcolor);
				gl_setpoint(x++,y,*tbuf & 0x02 ? _pencolor : _bkcolor);
				gl_setpoint(x++,y,*tbuf & 0x01 ? _pencolor : _bkcolor);
				tbuf++;
			}
		}
	}
}

/*
* Function: 输出ascii文字
* Parameters:
* Return:
* Remarks:
	在掉用此函数前必须先调用gl_ui_setlib设置字库。
	为了精简代码存储量，ascii字库也不是每个都写在zimo_xxx.c里的，
	根据情况自己删减zimo_xxx.c的内容，具体方法看提供的手册
*/
void gl_text(uint16_t x,uint16_t y,uint8_t *str,uint16_t num)
{
	uint16_t i = 0;
	uint8_t *tstr = str;
	uint32_t old;
	
	i = 0;
	old = gl_ui_setpencolor(_fcolor);
	while(*tstr && i++ < num) {
		//gl_picture((uint8_t*)p_zm_ascii[*tstr++],x,y,ZM_WIDTH,ZM_HIGH);
		//gl_picture(_zm_ascii[*tstr++],x,y,ZM_WIDTH,ZM_HIGH);
		gl_picture(_zm_lib.lib[*tstr],x,y,_zm_lib.w,_zm_lib.h);
		x += _zm_lib.step[*tstr];
		tstr++;
	}
	gl_ui_setpencolor(old );
}

/*
* Function: 绘制一个有填充色的矩形
* Parameters:
* Return:
* Remarks:
	线条色是_pencolor填充色是_brushcolor
*/
void gl_fill_rect(
	uint16_t x,uint16_t y,
	uint16_t w,uint16_t h)
{
	int tx,ty;
	
	gl_horizon_line (x    ,x+w-1,y    );
	gl_horizon_line (x    ,x+w-1,y+h-1);
	gl_vertical_line(x    ,y+1  ,y+h-2);
	gl_vertical_line(x+w-1,y+1  ,y+h-2);
	
	
	for(ty = y+1;ty < y+h-1;ty++) {
		for(tx = x+1;tx < x+w-1;tx++) {
			gl_setpoint(tx    ,ty,_brushcolor);		
		}
	}
}
/*
* Function: 绘制一个空心矩形
* Parameters:
* Return:
* Remarks:
	线条色是_pencolor
*/
void gl_rect(
	uint16_t x,uint16_t y,
	uint16_t w,uint16_t h)
{
	gl_horizon_line (x    ,x+w-1,y    );
	gl_horizon_line (x    ,x+w-1,y+h-1);
	gl_vertical_line(x    ,y+1  ,y+h-2);
	gl_vertical_line(x+w-1,y+1  ,y+h-2);
}



