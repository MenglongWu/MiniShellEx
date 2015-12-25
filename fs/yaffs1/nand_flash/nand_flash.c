/**********************************************************************

Author:			Andy.Wu
Date:			2006.6.8

Description:	this file is the driver of nand_flash. 


***********************************************************************/
#include "board_config.h"
#include "nandflash.h"

extern void __low_nand_init();
extern char __low_nand_write_page(unsigned int page_addr,unsigned char *from,unsigned int size);
extern void __low_nand_read_page(unsigned int page_addr,unsigned char *to,unsigned int size);
extern char __low_nand_erase_block(unsigned int row_addr);



#ifdef RAMFS
unsigned char ramfs[RAM_SIZE];
#else
unsigned char ramfs[1024];
#endif
//#define RAMFS_BASE (0x30200000 + RAM_BLOCK_ADDR)
#define RAMFS_BASE (ramfs)

int ramfs_init()
{
#ifdef RAMFS
	unsigned char *pdata = (unsigned char *)RAMFS_BASE;
	unsigned int len = sizeof(ramfs);//(512+512)*1024*4;//4MB
	while(len) {
		*pdata = 0xff;
		pdata++;
		len--;
	}
#endif
	return 0;
}

// char ramfs_write(unsigned int page_addr,unsigned char *from,unsigned int size)
// {
// 	unsigned char *pdata = (unsigned char *)(RAMFS_BASE + page_addr);
// 	unsigned int len = size;
// 
// 	while(len) {
// 		*pdata++ = *from++;
// 		len--;
// 	}
// 	return 512;
// }

char ramfs_write512(unsigned int block,unsigned int page,unsigned int sector,unsigned char *from)
{
	//unsigned char *pdata = (unsigned char *)(RAMFS_BASE + block * RAM_BLOCK_ADDR+page*RAM_PAGE_ADDR);
	//unsigned char *pdata = (unsigned char *)(RAMFS_BASE + block * RAM_BLOCK_ADDR+page*(RAM_PAGE_ADDR+16));
	unsigned char *pdata = (unsigned char *)(
		RAMFS_BASE + 
		block * RAM_BLOCK_ADDR+
		page * RAM_PAGE_ADDR);
	unsigned int len = 512;

	// printk("-w5 %8.8x\n",pdata - RAMFS_BASE);
	while(len) {
		*pdata++ = *from++;
		len--;
	}
	// PrintByte(from,512);
	return 0;
}


char ramfs_writespace(unsigned int block,unsigned int page,unsigned int sector,unsigned char *from)
{
	//unsigned char *pdata = (unsigned char *)(RAMFS_BASE + block * RAM_BLOCK_ADDR+page*RAM_PAGE_ADDR+512);
	unsigned char *pdata = (unsigned char *)(
		RAMFS_BASE + 
		block * RAM_BLOCK_ADDR+
		page * RAM_PAGE_ADDR+
		RAM_SECTOR_ADDR);
	unsigned int len = 16;

	// printk("-ws %8.8x\n",pdata - RAMFS_BASE);
	while(len) {
		*pdata++ = *from++;
		len--;
	}
	// PrintByte(from,16);
	return 0;
}

void ramfs_read512(unsigned int block,unsigned int page,unsigned int sector,unsigned char *to)
{
	unsigned char *pdata = (unsigned char *)(
		RAMFS_BASE + 
		block * RAM_BLOCK_ADDR+
		page * RAM_PAGE_ADDR);

	unsigned int len = 512;

	// printk("-r5 %8.8x\n",pdata - RAMFS_BASE);
	while(len) {
		//*pdata = *pdata;
		*to++ = *pdata++;
		len--;
	}
}

void ramfs_readspace(unsigned int block,unsigned int page,unsigned int sector,unsigned char *to)
{
	unsigned char *pdata = (unsigned char *)(
		RAMFS_BASE + 
		block * RAM_BLOCK_ADDR+
		page * RAM_PAGE_ADDR+
		RAM_SECTOR_ADDR);
	unsigned int len = 16;

	// printk("-rs %8.8x\n",pdata - RAMFS_BASE);
	while(len) {
		//*pdata = *pdata;
		*to++ = *pdata++;
		len--;
	}
}
char ramfs_erase(unsigned int row_addr)
{
	unsigned char *pdata = (unsigned char *)(RAMFS_BASE + row_addr);
	unsigned int len = (512+16)*4;//page per block

	while(len) {
		*pdata++ = 0xff;
		len--;
	}
	return 0;
}









char nand_write512(unsigned int block,unsigned int page,unsigned int sector,unsigned char *from)
{

	//__low_nand_write_page(addr, from, len);
	nand_write(block,page,sector * (512 + 16),from,512);

	return 0;
}


char nand_writespace(unsigned int block,unsigned int page,unsigned int sector,unsigned char *from)
{
	//unsigned char *pdata = (unsigned char *)(RAMFS_BASE + block * RAM_BLOCK_ADDR+page*RAM_PAGE_ADDR+512);
	//__low_nand_write_page(addr, from, len);
	nand_write(block,page,sector * (512 + 16)+512,from,16);
	// while(len) {
	// 	*pdata++ = *from++;
	// 	len--;
	// }
	return 0;
}

void nand_read512(unsigned int block,unsigned int page,unsigned int sector,unsigned char *to)
{
	// printk("r 512 addr %8.8x\n",addr);
	//__low_nand_read_page(addr, to, len);
	nand_read(block,page,sector * (512 + 16),to,512);

}

void nand_readspace(unsigned int block,unsigned int page,unsigned int sector,unsigned char *to)
{

	// printk("r spare addr %8.8x\n",addr);
	//__low_nand_read_page(addr, to, len);
	nand_read(block,page,sector * (512 + 16)+512,to,16);
	// while(len) {
	// 	//*pdata = *pdata;
	// 	*to++ = *pdata++;
	// 	len--;
	// }
}
//char nand_erase(unsigned int row_addr)
//{
	// unsigned char *pdata = (unsigned char *)(RAMFS_BASE + row_addr);
	// unsigned int len = (512+16)*4;//page per block

	// while(len) {
	// 	*pdata++ = 0xff;
	// 	len--;
	// }
	//return 0;
//}

#define CHUNK_STROE_ADDR(chunk)  ( (NAND_PAGE_ADDR+16)* chunk)
#define CHUNK_SPACE_ADDR(chunk)  ( (NAND_PAGE_ADDR+16)* chunk + 512)
/*************************************************
Initialize the  hardware of nandflash
*************************************************/
int nand_flash_hwr_init(void)
{
	// printk("sizeof(ramfs) %d %8.8x\n",sizeof(ramfs),ramfs);
	__low_nand_init();
	ramfs_init();
	return 0;
}



/************************************************
Erase a block on nandflash;
block:	the number of block;

*************************************************/
int nand_flash_erase_block(int block)
{
	// __low_nand_erase_block(NAND_BLOCK_ADDR*block);
	//ramfs_erase(RAMFS_BASE + RAM_BLOCK_ADDR*block);
	ramfs_erase((unsigned int)(RAMFS_BASE + RAM_BLOCK_ADDR*block));
	return 0;
}

/************************************************
Write data to chunkInNAND;
chunkInNAND:
data:	the first 512bytes
spare:	the next 16bytes
*************************************************/
int nand_flash_program_buf(int chunkInNAND, const unsigned char *data, const unsigned char *spare)
{


	int block,page,sector;
	unsigned int pdata ;

	block = chunkInNAND / (RAM_PAGE_PER_BLOCK*RAM_CHUNK_PER_PAGE);
	page = (chunkInNAND % (RAM_PAGE_PER_BLOCK*RAM_CHUNK_PER_PAGE)) / RAM_CHUNK_PER_PAGE;
	sector = chunkInNAND % (RAM_CHUNK_PER_PAGE);

	// printk("nand_flash_program_buf()");
	// printk("\twrite chunkid %d block %d page %d sector %d d %8.8x s %8.8x\n",chunkInNAND,block,page,sector,data,spare);
	if(data) {
		// ramfs_write512(block,page,sector,(unsigned char*)data);
		nand_write512(block,page,sector,(unsigned char*)data);
	}
	if(spare) {
		// ramfs_writespace(block,page,sector,(unsigned char*)spare);
		nand_writespace(block,page,sector,(unsigned char*)spare);
	}
	return 0;
}



/************************************************
Write data to chunkInNAND;
chunkInNAND:
data:	the first 512 bytes
spare:	the next 16 bytes
*************************************************/
int nand_flash_read_buf(int chunkInNAND, unsigned char *data,unsigned char *spare)
{
	int block,page,sector;
	unsigned int pdata;


	block = chunkInNAND / (RAM_PAGE_PER_BLOCK*RAM_CHUNK_PER_PAGE);
	page = (chunkInNAND % (RAM_PAGE_PER_BLOCK*RAM_CHUNK_PER_PAGE)) / RAM_CHUNK_PER_PAGE;
	sector = chunkInNAND % (RAM_CHUNK_PER_PAGE);

	
	// printk("nand_flash_read_buf()\n");
	
	// printk("\tread chunkid %d block %d page %d sector %d d %8.8x s %8.8x\n",chunkInNAND,block,page,sector,data,spare);
	if(data) {
		// ramfs_read512(block,page,sector,data);
		nand_read512(block,page,sector,data);
	}
	if(spare) {
		// ramfs_readspace(block,page,sector,spare);
		nand_readspace(block,page,sector,spare);
	}


	return 0;
}


