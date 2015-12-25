/*
* File: gl_ui.c
* Brief:��ʾ����ͼӦ�ýӿ�
*	Date			Author			Note
	2012-10-17		MenglongWu		�����ӿ�ʵ�֣������ٵ���
*/
#include "gl_type.h"
#include "gl_ui_config.h"
#include "gl_ui.h"

//�ڲ�ȫ�ֱ���
static void (*_setpoint)(unsigned short x, unsigned short y,uint32_t  color);
static unsigned short _fcolor = 0x0000;		//������ɫ
static unsigned short _bkcolor = 0xffff;	//������ɫ
//static unsigned short _alpha = 100;		//͸����
static unsigned char  _bkmode = TRANSPARENT;//�������ģʽ
static unsigned short _pencolor = 0x0000;	//����ɫ
static unsigned short _brushcolor = 0x0000;		//��ˢ��ɫ
unsigned char *_zm_ascii[128] = {0};		//�ֿ�
static struct gl_ui_arraylib _zm_lib;		//������

/*
* Function: ���û�㺯
* Parameters:
	�ص���������Ҫ�ײ�Ӳ���ӿڷ��ϻص������Ĳ������ݹ���
	������ǰд�ĺ����ӿڲ�һ���ǰ��մ˹����д�ģ�
	��ʹ�ú�GL_UI_OLD_DEV��ʱ�򣬾���GL_SETPOINT����ص�����
* Return:
* Remarks:
*/
void gl_ui_hook(
	void (*setpoint)(unsigned short x, unsigned short y,uint32_t  color))
{
	_setpoint = setpoint;
}
/*
* Function: ���ñ������ģʽ
* Parameters:
	bkmode������TRANSPARENT��BACKFILL
* Return:��һ�ε����ģʽ
* Remarks:
*/
uint32_t gl_ui_setbkmode(uint32_t bkmode)
{
	unsigned short old = bkmode;
	_bkmode = bkmode;
	return old;
}

/*
* Function: ����������ɫ
* Parameters:
	�����豸ѡ��RGB()��RGB16()
* Return:��һ�ε�����ֵ
* Remarks:
*/
uint32_t gl_ui_setfontcolor(uint32_t  color)
{
	unsigned short old = _fcolor;
	_fcolor = color;
	return old;
}
/*
* Function: ���ñ���ɫ
* Parameters:
	�����豸ѡ��RGB()��RGB16()
* Return:��һ�ε�����ֵ
* Remarks:
*/
uint32_t gl_ui_setbkcolor(uint32_t  color)
{
	unsigned short old = _bkcolor;
	_bkcolor = color;
	return old;
}

/*
* Function: ���û�����ɫ
* Parameters:
	�����豸ѡ��RGB()��RGB16()
* Return:��һ�ε�����ֵ
* Remarks:
*/
uint32_t gl_ui_setpencolor(uint32_t  color)
{
	unsigned short old = _pencolor;
	_pencolor = color;
	return old;
}

/*
* Function: ���û�ˢ��ɫ
* Parameters:
	�����豸ѡ��RGB()��RGB16()
* Return:��һ�ε�����ֵ
* Remarks:
*/
uint32_t gl_ui_setbrushcolor(uint32_t  color)
{
	unsigned short old = _bkcolor;
	_brushcolor = color;
	return old;
}

/*
* Function: ����������ɫ
* Parameters:
	lib:������Ӧ��p_zm_ascii_xxx
	w,h:������Ӧ��ZM_xxx_W,ZM_xxx_H
	lib:������Ӧ��p_zm_step_xxx
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
* Function: ������㺯��
* Parameters:
	x,y:���Ƶ�λ��
	color:�����ɫ������ֻ�кڰ׵���ʾ������ɫֻ����-1����ɫ����0����ɫ��
	��Ӧ��gl_ui_setfontcol,gl_ui_setbkcolor�Ⱥ�����colorҲ���������趨
* Return:
* Remarks:
	Ӧ�ò���Զ����ֱ�ӷ��ʴ˺������ײ����
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
* Function: ����ˮƽ��
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
* Function: ���ƴ�ֱ��
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
* Function: ���ƾ����
* Parameters:
* Return:
* Remarks:
	�����ֻ����0��1��1��ʹ��_pencolor���ƣ�����ɫ�ø���͸�������������
	��ʹ�ñ�����ˢ���
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
* Function: ���ascii����
* Parameters:
* Return:
* Remarks:
	�ڵ��ô˺���ǰ�����ȵ���gl_ui_setlib�����ֿ⡣
	Ϊ�˾������洢����ascii�ֿ�Ҳ����ÿ����д��zimo_xxx.c��ģ�
	��������Լ�ɾ��zimo_xxx.c�����ݣ����巽�����ṩ���ֲ�
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
* Function: ����һ�������ɫ�ľ���
* Parameters:
* Return:
* Remarks:
	����ɫ��_pencolor���ɫ��_brushcolor
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
* Function: ����һ�����ľ���
* Parameters:
* Return:
* Remarks:
	����ɫ��_pencolor
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



