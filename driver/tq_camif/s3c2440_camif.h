/******************************************************************************

 Filename:	camif.h

 Descriptions		
 - header file of camif.c

 ******************************************************************************/

 
#ifndef __CAMIF_H__
#define __CAMIF_H__

#ifdef __cplusplus
extern "C" {
#endif

// #include "camdef.h"

void CalculateBurstSize(unsigned long dstHSize,unsigned long *mainBurstSize,unsigned long *RemainedBurstSize);
void CalculatePrescalerRatioShift(unsigned long srcWidth, unsigned long dstWidth, unsigned long *ratio,unsigned long *shift);

void CamPviewIsr(void);
void CamCodecIsr(void);
void CamIsr(void);


void CamPortSet(void);
void CamPortReturn(void);
void CamPreviewIntUnmask(void);
void CamCodecIntUnmask(void);
void CamPreviewIntMask(void);
void CamCodecIntMask(void);
void CamReset(void);
void CamModuleReset(void);
void SetCAMClockDivider(int divn);

void CamCaptureStart(unsigned long mode);
void CamCaptureStop(void);
void _CamPviewStopHw(void);
void _CamCodecStopHw(void);


void Test_CamPreview(void);
void Test_CamCodec(void);
void Test_YCbCr_to_RGB(void);
void Camera_Test(void);


#ifdef __cplusplus
}
#endif

#endif /*__CAMIF_H__*/
