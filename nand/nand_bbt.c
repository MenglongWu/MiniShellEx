/**
 ******************************************************************************
 * @file	nand_bbt.c
 * @brief	
 *		Nand坏块检测，数据结构struct nand_bbt记录每个块的使用情况
 		0 Bit：unuse
 		1 Bit：0：坏块；1 好块
		2 Bit：0：Sector内有数据；1：扇区内无数据
		3 Bit：0：存在ECC错误；1：无ECC错误。nand_write_page/nand_read_page执行后返回ecc结果
		4 Bit：0：删除Sector内数据；1：保留Sector内数据。nbbt_safeErase执行时检查此位
		5 Bit：0：暗示3Bit肯定为0，但此次nand_write_page/nand_read_page执行后ecc正确，则设置3Bit为1
			1：无意义
		6 Bit：unuse
		7 Bit：unuse
 *
 *--------------------------------------------------
 * version    |    author    |    date    |    content
 * V1.0			Menglong		2014-11-14	1.nbbt_safeErase、nbbt_writeSector、nbbt_readSector
 *		nbbt_init、nbbt_scan、nbbt_badOnUse、_nbbt_loadBBT
 ******************************************************************************
*/
#include "board_config.h"
#include "nandflash.h"
#include "board.h"
#include "2440addr.h"
#include "def.h"


#ifdef _DEBUG_
#define printdbg printk
#else
#define printdbg
#endif
// struct __attribute__((packed)) nand_bbt

//不采用段定义的对齐方式，采用不对其
struct nand_bbt
{
	unsigned char flag;
}__attribute__((packed));


#define NBBT_F_BIT0 1			//该bbt是否有意义
#define NBBT_F_BIT1 2			//是否属于坏块
#define NBBT_F_BIT2 4			//Sector是否有数据
#define NBBT_F_BIT3 8			//ECC是否曾经出现过错误
#define NBBT_F_BIT4 0x10		//sector内的数据需删除
#define NBBT_F_BIT5 0x20		//怀疑坏块标志
#define NBBT_F_BIT6 0x40		
#define NBBT_F_BIT7 0x80		
#define NBBT_F_ALL  0xff			


struct nand_bbt nbbt[NAND_SECTOR_COUNT];
static unsigned char s_isInit = 0;

	

static void _nbbt_loadBBT(unsigned int sector);
int nbbt_init()
{
	printk("\nnbbt_init()\n");
	for(int i = 0;i < NAND_SECTOR_COUNT;++i) {
		nbbt[i].flag = 0;
	}
	s_isInit = 1;
	return 0;
}


/**
 * @brief	打印Block所有扇区的nand_bbt信息
 * @param	sector 当前Block内任意sector序号
 * @retval\n	NULL
 * @remarks	
 */

void _nbbt_printBBT(unsigned int sector)
{
	if(!s_isInit) {
		return;
	}
	struct nand_bbt *pbbt;
	unsigned int start,end;
	start = BLOCK_FIRST_SECT(sector);
	end  = BLOCK_LAST_SECT(sector);
	pbbt        = &nbbt[start];
	printk("\nsector first %d last %d\n",start,end);

	if(nbbt[sector].flag == 0) 
	{
		_nbbt_loadBBT(sector);
	}
	while(start < end) {
		printk("\t%d~%d:",start,start+15);
		for(int i = 0;i < 16;++i) {
			printk(" %2.2x",pbbt->flag);
			start++;
			pbbt++;	
		}

		printk("\n");
	}
}


/**
 * @brief	从Nand的Space内载入当前Block所有nand_bbt信息
 * @param	sector 当前Block内任意sector序号
 * @retval\n	NULL
 * @remarks	
 */

static void _nbbt_loadBBT(unsigned int sector)
{
	struct nand_bbt *pbbt;
	unsigned int start,end,firstSector;

	printk("\n_nbbt_loadBBT()\n",sector);

	// Step 1.读取本Block的所有Space
	//start 是第一个page的space地址
	//end是下一个Block第一个page的space地址
	firstSector = BLOCK_FIRST_SECT(sector);
	pbbt        = &nbbt[firstSector];
	start       = SECT_STROE_ADDR( BLOCK_FIRST_SECT(sector) )+NAND_PAGE_SIZE;
	end         = start + NAND_BLOCK_ADDR;
	
	while(start < end) {
		nand_read_page(
						start,
						(unsigned char*)(pbbt),
						SECTOR_PER_PAGE);

		pbbt += SECTOR_PER_PAGE;
		start += NAND_PAGE_ADDR;
	}
}

/**
 * @brief	写描述符到space空间
 * @param	sector 扇区序号
 * @retval\n	NULL
 * @remarks	
 */

int nbbt_saveBBT(unsigned int sector)
{
	struct nand_bbt *pbbt;
	unsigned int start;

	// printk("\nsave sector flag\n",sector);

	// Step 1.读取本Block的所有Space
	pbbt        = &nbbt[sector];
	start       = SECT_SPACE_ADDR( sector );
	
	// printk("nnt addr %8.8x\n",start);
	nand_write_page(
					start,
					(unsigned char*)(pbbt),
					SECTOR_PER_PAGE);
	return 0;
}
/**
 * @brief	格式化方式坏块检测
 * @param\n
 * @retval\n	NULL
 * @remarks	
 */

int nbbt_scan()
{
	if(!s_isInit) {
		return 1;
	}
	unsigned int start,end;
	unsigned char buf[NAND_PAGE_SIZE];
	unsigned char bufzero[NAND_PAGE_SIZE];

	unsigned char bad[NAND_SECTOR_COUNT];
	unsigned int page_addr,block_addr;
	unsigned int sector;

	
	
	for(int i = 0;i < 12;++i) {
		printk("sector 0x%8.8x\n",SECT_STROE_ADDR(i));
	}
	
	printk("\nErase device\n");
	printk("read reg rGPACON %8.8x\n",rGPACON);
	// for(int i = 0;i < NAND_SECTOR_COUNT;++i) {
	// 	bad[i] = 0;
	// }


	start = 1*NAND_BLOCK_ADDR;
	end   = 200*NAND_BLOCK_ADDR;//(NAND_BLOCK_COUNT-1) * NAND_BLOCK_ADDR;

	// printk("NAND_DIVECE_SIZE = %d\nNAND_SECTOR_SIZE = %d\nNAND_BLOCK_ADDR = %d\nNAND_SECTOR_COUNT = %d\n",
	// 	NAND_DIVECE_SIZE,NAND_SECTOR_SIZE,NAND_BLOCK_ADDR,NAND_SECTOR_COUNT);
	// end   = 20 * NAND_BLOCK_ADDR;
	printk("scan 0x%8.8x ~ 0x%8.8x\n",start,end);
	block_addr = start;
	while(block_addr < end) {
		printk("Erase 0x%8.8x\n",block_addr);
		nand_erase_block(block_addr);
		block_addr += NAND_BLOCK_ADDR;
	}

	//写1校验
	printk("check read\n");
	printk("\n");
	page_addr = start;
	sector = 0;
	while(page_addr < end) {
		
		// printk("Read 0x%8.8x\n",page_addr);
		nand_read_page(
						page_addr,
						(unsigned char*)(buf),
						NAND_PAGE_SIZE);
		
		page_addr += NAND_PAGE_ADDR;
		// for(int i = 0;i < NAND_PAGE_SIZE;++i) {
		// 	if(buf[i] != 0xff) {
		// 		printk("\nErr erase 0x%8.8x 0x%8.8x",page_addr,i);
		// 		bad[sector] = 1;
		// 	}
		// }

	}
	printk("Read finish \n");
	return 0;
	//写0校验
	for(int i = 0;i < NAND_PAGE_SIZE;i++) {
		bufzero[i] = 0;
	}

	printk("\nWrite device\n");
	page_addr = start;
	sector = 0;
	while(page_addr < end) {
		// printk("\rWrite 0x%8.8x",page_addr);
		nand_write_page(
						page_addr,
						(unsigned char*)(bufzero),
						NAND_PAGE_SIZE);

		nand_read_page(
						page_addr,
						(unsigned char*)(buf),
						NAND_PAGE_SIZE);

		page_addr += NAND_PAGE_ADDR;
		for(int i = 0;i < NAND_PAGE_SIZE;++i) {
			if(buf[i] != 0) {
				printk("\nErr read 0x%8.8x 0x%8.8x",page_addr,i);
				bad[sector] = 1;
			}
		}
		sector++;
	}

	printk("\nList sector\n");
	for(int i = 0;i < NAND_SECTOR_COUNT;++i) {
		if(bad[i] == 1) {
			printk("bad block %x\n",i);
		}
	}
	return 0;
}

/**
 * @brief	在使用过程中形成坏块
 * @param\n
 * @retval\n	NULL
 * @remarks	
 */

int nbbt_badOnUse()
{
	if(!s_isInit) {
		return 1;
	}
	return 0;
}


/**
 * @brief	检查space保存有效数据的page，擦除block，拷贝回有效的block
 * @param\n
 * @retval\n	NULL
 * @remarks	
 */

int nbbt_safeErase(unsigned int sector)
{

	unsigned char blockData[NAND_PAGE_SIZE*PAGE_PER_BLOCK];
	struct nand_bbt *pbbt;
	unsigned int addr,start,end;//,firstSector;
	unsigned int off;
	
	printk("\nnbbt_safeErase() sector %d \n",sector);
	// Step 1.读取本Block的所有Space
	//start 是第一个page的space地址
	//end是下一个Block第一个page的space地址
	

	// Step 2.遍历Block中所有Sector flag，拷贝有用的数据，待擦除Block后恢复
	//firstSector = BLOCK_FIRST_SECT(sector);
	// start       = SECT_STROE_ADDR( BLOCK_FIRST_SECT(sector) )+NAND_PAGE_SIZE;
	start    = BLOCK_FIRST_SECT(sector);
	end      = start + SECTOR_PER_PAGE * PAGE_PER_BLOCK;
	pbbt     = &nbbt[BLOCK_FIRST_SECT(sector)];
	off      = 0;
	// for(int i = 0;i < SECTOR_PER_PAGE * PAGE_PER_BLOCK;++i) {
	while(start < end) {
		// start       = SECT_STROE_ADDR(firstSector);
		addr       = SECT_STROE_ADDR(start);

		// 下列判断排列顺序依据出现概率而定
		//1Bit=1\2Bit=0\4Bit=1 或 5Bit=0
		if(     ((pbbt->flag & 
				(NBBT_F_BIT1 | NBBT_F_BIT2 | NBBT_F_BIT4)) == 
				(NBBT_F_BIT1 | NBBT_F_BIT4)) 							||

				(pbbt->flag & NBBT_F_BIT5) == 0) {

			printdbg("\tBackup sect %d(Block %d sect %d)\n",start,start >> 8,start & 0xff);
			nand_read_page(
							addr,
							(unsigned char*)(blockData+off),
							NAND_SECTOR_SIZE);
			pbbt->flag = (~NBBT_F_BIT2);
			
		}
		//4Bit=0	改成1Bit保留，其余全1
		else if( (pbbt->flag & NBBT_F_BIT4) == 0) {
			printk("\tdelete sect %d 4Bit=0\n",start);
			// pbbt->flag = pbbt->flag | (~NBBT_F_BIT1);
			pbbt->flag = NBBT_F_ALL;
		}
		// 3Bit=0	改成3Bit=1，其余全1
		else if( (pbbt->flag & NBBT_F_BIT3) == 0) {
			printk("\tsect %d 3Bit=0\n",start);
			pbbt->flag = (~NBBT_F_BIT3);
			
		}
		// 1Bit=0
		else if( (pbbt->flag & NBBT_F_BIT1) == 0) {
			printk("\tsect %d 1Bit=0\n",start);
			pbbt->flag = (~NBBT_F_BIT1);
		}
		else {
			pbbt->flag = NBBT_F_ALL;
		}
		
		off += NAND_SPACE_SIZE;
		pbbt++;
		// firstSector++;
		start++;
	}

	
	//Step 3.删除该Block
	nand_erase_block(SECT_STROE_ADDR( BLOCK_FIRST_SECT(sector) ));



_Err_ECC:
	//Step 4.1.回写数据区域
	// firstSector = BLOCK_FIRST_SECT(sector);
	start    = BLOCK_FIRST_SECT(sector);
	pbbt        = &nbbt[BLOCK_FIRST_SECT(sector)];
	off         = 0;
	end      = start + SECTOR_PER_PAGE * PAGE_PER_BLOCK;

	// for(int i = 0;i < SECTOR_PER_PAGE * PAGE_PER_BLOCK;++i) {
	while(start < end) {
		// start       = SECT_STROE_ADDR(firstSector);
		addr       = SECT_STROE_ADDR(start);
		if(  (pbbt->flag &  NBBT_F_BIT2 ) == 0) {
			printk("\tRewrite sect %d(Block %d sect %d)\n",start,start >> 8,start & 0xff);
			// TODO: 写入出现错误返回_Err_ECC
			nand_write_page(
							addr,
							(unsigned char*)(blockData+off),
							NAND_SECTOR_SIZE);
		}
		nbbt_saveBBT(start);
		off += NAND_SPACE_SIZE;
		pbbt++;
		start++;
	}


	// //Step 4.2 回写Space
	// firstSector = BLOCK_FIRST_SECT(sector);
	// pbbt        = &nbbt[firstSector];
	// start       = SECT_STROE_ADDR( BLOCK_FIRST_SECT(sector) )+NAND_PAGE_SIZE;
	// end         = start + NAND_BLOCK_ADDR;
	// while(start < end) {
	// 	// TODO: 写入出现错误返回_Err_ECC
	// 	nand_write_page(
	// 					start,
	// 					(unsigned char*)(pbbt),
	// 					SECTOR_PER_PAGE);

	// 	nbbt_saveBBT(firstSector);
	// 	firstSector++;
	// 	pbbt += SECTOR_PER_PAGE;
	// 	start += NAND_PAGE_ADDR;
		
	// }

	printdbg("end nbbt_safeErase()\n\n");
	return 0;
}



/**
 * @brief	写扇区
 * @param	sector 写入扇区序号
 * @param	buf 返回缓存
 * @param	count 连续写入扇区数
 * @retval	0：成功；其他标示错误
 * @remarks	nbbt_writeSector/nbbt_readSector不对sector/count的值做检测，如果
 sector=0xffffffff count=2将发生溢出
 */

int nbbt_writeSector(unsigned int sector,unsigned char *buf,unsigned int count)
{
	if(!s_isInit) {
		return 1;
	}
	unsigned int addr,start,end;
	unsigned int off;
	struct nand_bbt *pbbt;

	printdbg("\nbbt_writeSector() sector %d buf %8.8x count %d\n",sector,buf,count);
	// Step 1.检查描述符是否有效
	if(nbbt[sector].flag == 0) 
	{
		printk("load bbt\n");
		_nbbt_loadBBT(sector);
	}

	// Step 2.检查要写入的块sector是否已经存在数据，存在则删除
	unsigned int bneedElase = 0;

	start = sector;
	end   = sector + count;
	pbbt  = &nbbt[start];
	while(start < end) {
		if( (	pbbt->flag &
					(NBBT_F_BIT1 | NBBT_F_BIT2) ) == 
					(NBBT_F_BIT1) ) {
			printk("\tHave data:1Bit=1 2Bit=0 sect %d\n",start);
			pbbt->flag = ~(NBBT_F_BIT4);
			bneedElase = 1;
		}
		start++;
		pbbt++;
	}//end while
	if(bneedElase == 1) {
		nbbt_safeErase(start);

		//todo 均衡磨损、仅标记擦除但不再写入
		//return 1;
	}
	
	

	// Step 3.写入buf数据
	end = sector + count;
	off = 0;
	start = sector;
	pbbt = &nbbt[start];
	printk("\tstart %d end %d\n",start,end);
	while(start < end) {
		// 1Bit=1无坏块、2Bit=1无数据
		if(		(pbbt->flag & 
				(NBBT_F_BIT1 | NBBT_F_BIT2) )   ==    
				(NBBT_F_BIT1 | NBBT_F_BIT2) ) {

			addr = SECT_STROE_ADDR(start);
			printk("\twrite sect %d addr 0x%8.8x\n",start,addr);
			nand_write_page(
							addr,
							(unsigned char*)(buf+off),
							NAND_SECTOR_SIZE);

			
			if(1) {
				pbbt->flag = (~NBBT_F_BIT2);	
			}
			// todo:_ErrECC
			else {
				pbbt->flag = ~(NBBT_F_BIT2|NBBT_F_BIT5);
				nbbt_badOnUse(start);
			}
			nbbt_saveBBT(start);
		}
		// 1Bit=1无坏块、2Bit=0有数据
		else if( (	pbbt->flag &
					(NBBT_F_BIT1 | NBBT_F_BIT2) ) == 
					(NBBT_F_BIT1) ) {
			printk("\t[[[[warring]]]]]]] %d\n",start);
			pbbt->flag = ~(NBBT_F_BIT4);
			// nbbt_safeErase(start);

		}
		// 1Bit=0坏块
		else {
			printk("\tBad:1Bit=0\n");
			return 1;
		}

		off += NAND_SECTOR_SIZE;
		start++;

		pbbt++;
	}//end while

	printdbg("end nbbt_writeSector()\n\n");
	return 0;
}


/**
 * @brief	读扇区
 * @param	sector 写入扇区序号
 * @param	buf 返回缓存
 * @param	count 连续写入扇区数。
 * @retval	0：成功；其他标示错误
 * @remarks	nbbt_writeSector/nbbt_readSector不对sector/count的值做检测，如果
 sector=0xffffffff count=2将发生溢出
 */
int nbbt_readSector(unsigned int sector,unsigned char *buf,unsigned int count)
{
	if(!s_isInit) {
		return 1;
	}
	unsigned int addr,start,end;
	unsigned int off;
	struct nand_bbt *pbbt;

	printk("\nbbt_readSector() sector %d buf %8.8x count %d\n",sector,buf,count);
	start = sector;
	end = sector + count;
	off = 0;
	pbbt = &nbbt[start];
	while(start < end) {

		addr = SECT_STROE_ADDR(start);
		// printk("read sect %d addr 0x%8.8x\n",start,addr);
		nand_read_page(
						addr,
						(unsigned char*)(buf+off),
						NAND_SECTOR_SIZE);
		off += NAND_SECTOR_SIZE;
		start++;
		pbbt->flag = (~NBBT_F_BIT2);
		pbbt++;
	}
	printdbg("\n");
	return 0;
}

