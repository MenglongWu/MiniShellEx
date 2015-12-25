#ifndef _S3C2440_H_
#define _S3C2440_H_


struct point  //typedef 坐标点
{
   unsigned short  x;
   unsigned short  y;
};
void init_touch();
int gettouch(struct point *pt);
#endif