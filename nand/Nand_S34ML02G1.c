/**
 ******************************************************************************
 * @file	Nand_S34ML02G1.c
 * @brief	
 *			S3C2440下Nand Flash S34ML02G1驱动程序
 *
 *--------------------------------------------------
 * version    |    author    |    date    |    content
 * V1.0 		Menglong Wu		2010-10-12		
 * V2.0 		Menglong Wu		2010-11-9	1.write、read函数添加读写长度
 * V2.1 		Menglong Wu		2010-11-6	1.原程序的读取速度11KB/s提升至3.08MB/s
 ******************************************************************************
*/


 


#include "board_config.h"
#include "Nand_S34ML02G1.h"
#include "2440addr.h"
// #include "nand.h"
#include "def.h"





// int printk(const char *fmt, ...);
 void __low_nand_reset()
{
	NF_CE_L();
	NF_CLEAR_RB();
	NF_CMD(CMD_RESET);  
	NF_WAITRB();
	NF_CE_H();
}
 void __low_nand_init()
{
	rGPACON |= 0x3f<<17;
	rNFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4)|(0<<0);
	rNFCONT = (0<<13)|(0<<12)|(0<<10)|(0<<9)|(0<<8)|(1<<6)|(1<<5)|(1<<4)|(1<<1)|(1<<0);
	rNFSTAT = 0;
	// rNF_Reset();
	__low_nand_reset();
}

	char g_nfid1;
	char g_nfid2;
	char g_nfid3;
	char g_nfid4;
	char g_nfid5;
 char __low_nand_readid(unsigned int *part1,unsigned *part2)
{


	char	pMID;
	char	pDID;
	char	nBuff;
	char	n4thcycle;
	int	i;


	NF_nFCE_L();    
	NF_CLEAR_RB();
	NF_CMD(CMD_READID);	// read id command
	NF_ADDR(0x00);
	for ( i = 0; i < 100; i++ );

	g_nfid1 = NF_RDDATA8();
	g_nfid2 = NF_RDDATA8();
	g_nfid3 = NF_RDDATA8();
	g_nfid4 = NF_RDDATA8();
	g_nfid5 = NF_RDDATA8();
	

	pMID = g_nfid1;
	pDID = g_nfid2;

	nBuff     = NF_RDDATA8();
	n4thcycle = NF_RDDATA8();
	NF_nFCE_H();
	



	return (pDID);
}



/**
 * @brief	向Nand写入1页数据
 * @param	page_addr 写入页的起始地址
 * @param	form 待写入页的内容
 * @param	size 写入的大小
 * @retval	待完善
 * @remarks	调用底层驱动__low_nand_write_page
 * @see nand_erase
 */
char __low_nand_write_page(unsigned int page_addr,unsigned char *from,unsigned int size)
{
	U32 i;
	// char nfstatus;
	unsigned int *pfrom32 = (unsigned int *)from;//2440的NFADDR寄存器是32位的
	unsigned char *pfrom8;
	unsigned int nLen;
	// printk("write page addr :%.8x \r\n",page_addr);



	// __low_nand_reset();

	//  Enable the chip
	NF_nFCE_L();
	// NF_CLEAR_RB();

	// Issue Read command
	NF_CMD(CMD_WRITE1);

	//  Set up address
	
	NF_ADDR(page_addr & 0xff);		
	NF_ADDR((page_addr >> 8) & 0x0f);
	NF_ADDR((page_addr >> 12) & 0xff);
	NF_ADDR((page_addr >> 20) & 0xff);
	NF_ADDR((page_addr >> 28) & 0x01);
	
	
	// for(i = 0;i < (NAND_PAGE_SIZE+64) >> 2;++i) {
	// for(i = 0;i < (NAND_PAGE_SIZE) >> 2;++i) {
	// 	NF_WRDATA( *((unsigned*)pfrom32++));
	// }

	// 写入前若干4Byte
	nLen = size >> 2;
	for(i = 0;i < nLen;++i) {
		NF_WRDATA( *((unsigned*)pfrom32++));
	}

	// 写入剩下Byte
	nLen = size & 0x03;
	pfrom8 =  (unsigned char *)pfrom32;
	for(i = 0;i < nLen;++i) {
		NF_WRDATA8( *((unsigned char*)pfrom8++));
	}

	NF_CMD(CMD_WRITE2);

	
	NF_WAITRB();		// wait tR(max 12us)
	NF_CMD(CMD_STATUS);

	return (NF_RDDATA8() & 0x01);
	// nfstatus =  NF_RDDATA8();

}



//  void __low_nand_read_page(unsigned int page_addr,unsigned char *to,unsigned int size)
// {
// 	U32 i;
// 	// char nfstatus;
// 	unsigned int *pto32 = (unsigned int *)to;
// 	unsigned char *pto8;
// 	unsigned int nLen;
// 	// puts("read flash\n");

// 	// __low_nand_reset();
// 	//  Enable the chip
// 	NF_nFCE_L();
// 	NF_CLEAR_RB();

// 	// Issue Read command
// 	NF_CMD(CMD_READ);

// 	//  Set up address
// 	NF_ADDR(page_addr & 0xff);		
// 	NF_ADDR((page_addr >> 8) & 0x0f);
// 	NF_ADDR((page_addr >> 12) & 0xff);
// 	NF_ADDR((page_addr >> 20) & 0xff);
// 	NF_ADDR((page_addr >> 28) & 0x01);
// 	NF_CMD(CMD_READ2);


// 	NF_WAITRB();		// wait tR(max 12us)

	
// 	// for (i = 0; i < (NAND_PAGE_SIZE+64) >> 2;++i) {
// 	// for (i = 0; i < (NAND_PAGE_SIZE) >> 2;++i) {
// 	// 	*pto32++ = NF_RDDATA();
// 	// }
 
//  	// 读取若干4Byte
// 	nLen = size >> 2;
// 	for(i = 0;i < nLen;++i) {
// 		*pto32++ = NF_RDDATA();
// 	}
// 	// 读取剩下Byte
// 	nLen = size & 0x03;
// 	pto8 =  (unsigned char *)pto32;
// 	for(i = 0;i < nLen;++i) {
// 		*pto8++ = NF_RDDATA8();
// 	}
// 	NF_nFCE_H();
// }


 //void __low_nand_read_page_dma(unsigned int page_addr,unsigned char *to,unsigned int size)
void __low_nand_read_page(unsigned int page_addr,unsigned char *to,unsigned int size)
{
	U32 i;



	// char nfstatus;
	unsigned int *pto32 = (unsigned int *)to;
	unsigned char *pto8;
	unsigned int nLen;
	// puts("read flash\n");

	// __low_nand_reset();
	//  Enable the chip
	NF_nFCE_L();
	NF_CLEAR_RB();

	// Issue Read command
	NF_CMD(CMD_READ);

	//  Set up address
	NF_ADDR(page_addr & 0xff);		
	NF_ADDR((page_addr >> 8) & 0x0f);
	NF_ADDR((page_addr >> 12) & 0xff);
	NF_ADDR((page_addr >> 20) & 0xff);
	NF_ADDR((page_addr >> 28) & 0x01);
	NF_CMD(CMD_READ2);


	NF_WAITRB();		// wait tR(max 12us)

	
	
	//rDISRC0=(unsigned long)0x4E000010;
	if( (size & 0x0f) == 0 && (size > 16) ) {
		rDISRC0=NFDATA;         // Nand flash data register
		rDISRCC0=(0<<1) | (1<<0); //arc=AHB,src_addr=fix
		rDIDST0=(unsigned)(to);
		rDIDSTC0=(0<<1) | (0<<0); //dst=AHB,dst_addr=inc;
		rDCON0=(1<<31)|(1<<30)|(0<<29)|(1<<28)|(1<<27)|(0<<23)|(1<<22)|(2<<20)|((size/4/4));
		// printk("%d \n",((size/4/4)-1));
		// rDCON0=(1<<31)|(1<<30)|(0<<29)|(0<<28)|(1<<27)|(0<<23)|(1<<22)|(0<<20)|(size);
						//Handshake,AHB,interrupt,(4-burst),whole,S/W,no_autoreload,word,count=128;
						// DMA on and start.
		rDMASKTRIG0=(1<<1)|(1<<0);

		while(0 != rDMASKTRIG0){
			// printk("%x %x\n",rDCSRC0,rDCDST0);        // Wait until Dma transfer is done.
		}
		// pto8 = to + size -(size & 0x0f);

		// for(int i = 0;i < size & 0x0f;++i) {
		// 	*pto8++ = NF_RDDATA8();
		// }
	}
	else {
		// 读取若干4Byte
		nLen = size >> 2;
		for(i = 0;i < nLen;++i) {
			*pto32++ = NF_RDDATA();
		}
		// 读取剩下Byte
		nLen = size & 0x03;
		pto8 =  (unsigned char *)pto32;
		for(i = 0;i < nLen;++i) {
			*pto8++ = NF_RDDATA8();
		}
	}


	
	NF_nFCE_H();
}





char __low_nand_erase_block(unsigned int row_addr)
{
	NF_nFCE_L();
	NF_CLEAR_RB();
	NF_CMD(CMD_ERASE1);
	NF_ADDR((row_addr >> 12) & 0xff);
	NF_ADDR((row_addr >> 20) & 0xff);
	NF_ADDR((row_addr >> 28) & 0x01);
	NF_CMD(CMD_ERASE2);
	NF_WAITRB();		

	NF_CMD(CMD_STATUS);
	return (NF_RDDATA8() & 0x01);

}



//  inline char nand_write_page(unsigned int page_addr,unsigned char *from,unsigned int size)
// {
// 	return __low_nand_write_page(page_addr,from,size);
// }

//  inline void nand_read_page(unsigned int page_addr,unsigned char *to,unsigned int size)
// {
// 	__low_nand_read_page(page_addr,to,size);
// }
//  inline char nand_erase_block(unsigned int row_addr)
// {
// 	return __low_nand_erase_block(row_addr);
// }


// inline void nand_init()
// {
// 	__low_nand_init();
// }

// inline void nand_init()
// {
// 	__low_nand_init();
// }

// static void __low_nand_check_bad()
// {

// }
// static void __low_nand_jumpover_bad()
// {

// }

// void __low_nand_format(unsigned int from,unsigned int to)
// {

// }

