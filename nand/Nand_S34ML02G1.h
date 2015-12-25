/**
 ******************************************************************************
 * @file	Nand_S34ML02G1.h
 * @brief	
 *			S34ML02G1宏定义
 *
 *--------------------------------------------------
 * version    |    author    |    date    |    content
 * V1.0			Nane			20xx-xx-xx		1.xxxxx
 ******************************************************************************
 */

#ifndef _NAND_S34ML02G1_H_
#define _NAND_S34ML02G1_H_ 



#define CMD_READ			0x00	//  Read
#define CMD_READ2			0x30	//  Read3

#define CMD_ERASE1			0x60	//  Erase phase 1
#define CMD_ERASE2			0xD0	//  Erase phase 2


#define CMD_WRITE1			0x80	//  Write phase 1
#define CMD_WRITE2			0x10	//  Write phase 2

#define CMD_RESET			0xff	//  Reset
#define CMD_READID			0x90	//  ReadID
#define CMD_STATUS			0x70	//  Status read





#define NF_CMD(cmd)			{rNFCMD  = (cmd); }
#define NF_ADDR(addr)			{rNFADDR = (addr); }	
#define NF_nFCE_L()			{rNFCONT &= ~(1<<1); }
#define NF_nFCE_H()			{rNFCONT |= (1<<1); }
#define NF_RSTECC()			{rNFCONT |= (1<<4); }
#define NF_RDMECC()			(rNFMECC0 )
#define NF_RDSECC()			(rNFSECC )



#define NF_RDDATA()			(rNFDATA)
#define NF_RDDATA8()			(rNFDATA8)
#define NF_WRDATA(data)			{rNFDATA = (data); }
#define NF_WRDATA8(data)			{rNFDATA8 = (data); }



#define NF_WAITRB()				{while(!(rNFSTAT&(1<<0)));} 
#define NF_CLEAR_RB()			{rNFSTAT |= (1<<2); }
#define NF_DETECT_RB()			{while(!(rNFSTAT&(1<<2)));}
#define NF_MECC_UnLock()		{rNFCONT &= ~(1<<5); }
#define NF_MECC_Lock()			{rNFCONT |= (1<<5); }
#define NF_SECC_UnLock()		{rNFCONT &= ~(1<<6); }
#define NF_SECC_Lock()			{rNFCONT |= (1<<6); }

#define	RdNFDat8()			(rNFDATA8)	//byte access
#define	RdNFDat()			RdNFDat8()	//for 8 bit nand flash, use byte access
#define	WrNFDat8(dat)			(rNFDATA8 = (dat))	//byte access
#define	WrNFDat(dat)			WrNFDat8()	//for 8 bit nand flash, use byte access

#define pNFCONF				rNFCONF 
#define pNFCMD				rNFCMD  
#define pNFADDR				rNFADDR 
#define pNFDATA				rNFDATA 
#define pNFSTAT				rNFSTAT 
#define pNFECC				rNFECC0  

#define NF_CE_L()			NF_nFCE_L()
#define NF_CE_H()			NF_nFCE_H()
#define NF_DATA_R()			rNFDATA
#define NF_ECC()			rNFECC0
 


// HCLK=100Mhz
// #define TACLS				1
// #define TWRPH0				1
// #define TWRPH1				1

#define TACLS				0
#define TWRPH0				1
#define TWRPH1				0



#endif	


