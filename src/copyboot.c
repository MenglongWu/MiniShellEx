#include "board_config.h"
#include "nandflash.h"




#include "board_config.h"
#include "2440addr.h"
// #include "stdio.h"
#include "board.h"
#include "nandflash.h"
#include "string.h"

#include "minishell_core.h"


// int printk(const char *fmt, ...);

// extern void PrintNandFlash(unsigned char *data,unsigned int size);

//bss_start 在boot.lds里定义
extern unsigned long bss_start;
void copy_sdram2nand()
{
	//计算boot代码大小，确定需要格式化Block的块总数、需要写入的Page总数
	unsigned long bootSize = (unsigned long)(&bss_start) - 0x30000000;		//uboot代码长度
	unsigned long blockTotal,pageTotal;

	
	blockTotal = 	(
						(bootSize + NAND_BLOCK_MASK ) & (~NAND_BLOCK_MASK)
					) 	/ NAND_BLOCK_SIZE;
	pageTotal  = 	(
						(bootSize + NAND_PAGE_MASK ) & (~NAND_PAGE_MASK)
					)	/ NAND_PAGE_SIZE;

	// printk("\r\ncode size 0x%x (%dByte)\r\n",bootSize,bootSize);
	// printk("require block %d require page %d\r\n",blockTotal,pageTotal);
	
	//烧录进FLASH，不带坏块检测
	char retErase;
	unsigned char *lpsrcData = (unsigned char *)0x30000000;	//SDRAM 0x30000000地址的代码
	unsigned int off_data,page_addr;

	retErase = nand_erase_block(0);
	// printk("retrEsdrase %d\r\n",retErase);
	off_data = 0;
	page_addr = 0;
	while(off_data < bootSize) 
	{
		nand_write_page(
					page_addr,
					(unsigned char*)(lpsrcData + off_data) ,
					NAND_PAGE_SIZE);
		// nand_write_page(
		// 			page_addr,
		// 			(unsigned char*)(lpsrcData + off_data));

		// printk("colum 0x%8.8x write data off %d page_addr %d\r\n",off_data,off_data,page_addr);
		// PrintNandFlash((unsigned char*)(lpsrcData + off_data),16);
		off_data  += NAND_PAGE_SIZE;
		page_addr += NAND_PAGE_ADDR;
	}
}




extern int _where_lr();
extern int boot_led3();
extern int boot_led();
void copy_nand2sdram()
{
	unsigned long bootSize = (unsigned long)(&bss_start) - 0x30000000;		//uboot代码长度
	unsigned long blockTotal,pageTotal;

	
	blockTotal = 	(
						(bootSize + NAND_BLOCK_MASK ) & (~NAND_BLOCK_MASK)
					) 	/ NAND_BLOCK_SIZE;
	pageTotal  = 	(
						(bootSize + NAND_PAGE_MASK ) & (~NAND_PAGE_MASK)
					)	/ NAND_PAGE_SIZE;
	


	//烧录进FLASH，不带坏块检测
	unsigned char *lpdstData = (unsigned char *)0x30000000;	//SDRAM 0x30000000地址的代码
	unsigned int off_data,page_addr;
	int pcPoint;

	//查看PC指针位置，如果指针在0~4096表示在SRAM中运行，拷贝Nand到SDRAM
	//否则表示代码在SDRAM中，跳过Nand拷贝
	pcPoint = _where_lr();
	// printf("%x",pcPoint);
	nand_init();
	// printf("--------\n");
	if(pcPoint < 4096) 
	{
		//TODO:根据电路板移植显示正在拷贝Nand到SDRAM
		boot_led3();

		
		off_data = 0;
		page_addr = 0;

		while(off_data < bootSize) 
		{
			
			nand_read_page(
						page_addr,
						(unsigned char*)(lpdstData + off_data),
						NAND_PAGE_SIZE);
			// nand_read_page(
			// 			page_addr,
			// 			(unsigned char*)(lpdstData + off_data));

			// printk("colum 0x%8.8x write data off %d page_addr %d\r\n",off_data,off_data,page_addr);
			// PrintNandFlash((unsigned char*)(lpsrcData + off_data),16);
			off_data  += NAND_PAGE_SIZE;
			page_addr += NAND_PAGE_ADDR;
		}
	}

}

// extern int getchar(void);
extern int putchar(int);
void copy_nand2sdram_ex()
{
	unsigned long bootSize = (unsigned long)(&bss_start) - 0x30000000;		//uboot代码长度
	unsigned long blockTotal,pageTotal;

	
	blockTotal = 	(
						(bootSize + NAND_BLOCK_MASK ) & (~NAND_BLOCK_MASK)
					) 	/ NAND_BLOCK_SIZE;
	pageTotal  = 	(
						(bootSize + NAND_PAGE_MASK ) & (~NAND_PAGE_MASK)
					)	/ NAND_PAGE_SIZE;
	


	//烧录进FLASH，不带坏块检测
	unsigned char *lpdstData = (unsigned char *)0x30100000;	//SDRAM 0x30000000地址的代码
	unsigned int off_data,page_addr;
	int pcPoint;

	//查看PC指针位置，如果指针在0~4096表示在SRAM中运行，拷贝Nand到SDRAM
	//否则表示代码在SDRAM中，跳过Nand拷贝
	pcPoint = _where_lr();
	// printf("%x",pcPoint);
	nand_init();
	// printf("--------\n");
	// if(pcPoint < 4096) 
	{

		// printf("asdfsdfd\n");
		off_data = 0;
		page_addr = 0;

		while(off_data < bootSize) 
		{

			nand_read_page(
						page_addr,
						(unsigned char*)(lpdstData + off_data),
						NAND_PAGE_SIZE);
			// nand_read_page(
			// 			page_addr,
			// 			(unsigned char*)(lpdstData + off_data));
			boot_led();

			// printk("colum 0x%8.8x write data off %d page_addr %d\r\n",off_data,off_data,page_addr);
			// PrintNandFlash((unsigned char*)(lpsrcData + off_data),16);
			off_data  += NAND_PAGE_SIZE;
			page_addr += NAND_PAGE_ADDR;
		}
	}

}

