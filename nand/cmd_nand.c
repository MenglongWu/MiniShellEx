/**
 ******************************************************************************
 * @file	cmd_nand.c
 * @brief	
 *		NandFlash添加的MiniShell命令
 *
 *--------------------------------------------------
 * version    |    author    |    date    |    content
 * V1.0 		Menglong Wu		2014-12-09	
 ******************************************************************************
*/
#include "minishell_core.h"
#include "board.h"
#include "linux/string.h"
#include "board_config.h"
#include "nandflash.h"
// #include "yaffsfs.h"


// int do_mkdir(int argc,char **argv)
// {
// 	//char strout[256];
// 	//printf("输入目录路径");
// 	//scanf("%256s",strout);
// 	if(argc >= 2) {
// 		printk("mkdir %s\n",argv[1]);
// 		yaffs_mkdir(argv[1],0);	
// 	}
// 	return 0;
// }

void PrintByte(unsigned char *data,unsigned int len)
{
	for(int i = 0;i < len;++i) {
		if(i % 16 == 0) {
			if(i % 512 == 0) {
				puts("\n");
			}
			printk("\n%8.8x:  ",i);
		}
		else if(i % 8 == 0) {
			puts(" ");
		}
		
		printk("%2.2x ",*data++);
	}
	puts("\n");
}
/**
 * @brief	列出本Nand的信息
 * @param\n NULL
 * @retval\n	NULL
 * @remarks	
 */

int nand_inf()
{
	puts("Nand flash device information:\n");
	printk("\tPage   size 		%dB+%dB\n",NAND_PAGE_SIZE,NAND_SECTOR_SIZE);
	printk("\tBlock  size 		%dKB\n",NAND_BLOCK_SIZE/1024);
	printk("\tDevice size 		%dMB\n",(NAND_BLOCK_SIZE*NAND_BLOCK_COUNT)/(1024*1024));
	printk("\tPage  pre block  	%d\n",PAGE_PER_BLOCK);
	printk("\tBlock pre device 	%d\n",NAND_BLOCK_COUNT);
	
	return 0;

}
/**
 * @brief	minishell 命令
 * @param nand w、nand r、nand e等，具体内容在控制台输入shell
 * @retval\n	NULL
 * @remarks	
 */
int do_nand(int argc,char **argv)
{
	unsigned int var1 = 0,var2 = 0,var3 = 0,var4 = 0,var5 = 0;
	
	if(argc <= 1) {
		puts("Usage:\n");
		puts("\tnand [i] print nand information\n");
		puts("\tnand [e] [start block] <end block (start block+1)> erase block\n");
		printk("\tnand [w] [block] [page] [off] [addr] write nand\n",NAND_PAGE_SIZE);
		printk("\tnand [r] [block] [page] [off] [addr] read nand\n",NAND_PAGE_SIZE);
		puts("\tnand [c] check bad block\n");
		return 0;
	}

	if(argc >= 3) {
		var1 = atoi(argv[2]);
	}
	if(argc >= 4) {
		var2 = atoi(argv[3]);
	}
	if(argc >= 5) {
		var3 = atoi(argv[4]);
	}
	if(argc >= 6) {
		var4 = simple_strtol(argv[5],0,16);
	}
	if(argc >= 7) {
		var5 = atoi(argv[6]);
	}

	if(argv[1][0] == 'i') {
		nand_inf();
	}
	else if(argv[1][0] == 'e' && argc >= 3) {
		var2 = (var2 && var2 < NAND_BLOCK_COUNT) ? var2 : var1;
		printk("Are you sure Erase block %d~%d [y/n]\n",var1,var2);
		if('y' == (s_getchar() | 0x20))  
		{
			for(int i = var1;i <= var2;++i) {
				nand_erase(i);
				printk("\tErase block %d\n",i);
			}
			puts("\n");
		}
		else 
		{
			puts("Cancel\n");
		}
	}
	
	else if(argv[1][0] == 'w' && argc >= 5) {
		printk("block %d page %d off %d data 0x%8.8x\n",var1,var2,var3,var4);
		var5 = (var5 && var5 < NAND_PAGE_SIZE)? var5 : NAND_PAGE_SIZE;
		nand_write(var1,var2,var3,(unsigned char*)var4,var5);
	}
	else if(argv[1][0] == 'r' && argc >= 5) {
		var5 = (var5 && var5 < NAND_PAGE_SIZE)? var5 : NAND_PAGE_SIZE;
		nand_read(var1,var2,var3,(unsigned char*)var4,var5);
		PrintByte((unsigned char*)var4,var5);
		
	}
	else if(argv[1][0] == 'p' && argc >= 3) {
		var1 = simple_strtol(argv[2],0,16);
		PrintByte((unsigned char*)var1,NAND_PAGE_SIZE);
		
	}
	__low_nand_write_page(
			(var1 * NAND_BLOCK_ADDR) + (var2 * NAND_PAGE_ADDR) + var3,
			(unsigned char*)var4,
			var5);
	// nand_read(0,0,0,0x30200000,NAND_PAGE_SIZE);
	
	// // __low_nand_read_page(
	// // 		0,
	// // 		(unsigned char*)0x30200000,
	// // 		NAND_PAGE_SIZE);
	// PrintByte(0x30200000,NAND_PAGE_SIZE);
	return 0;
}

// extern int getchar(void);
int do_copy_sdram2nand(int argc,char **argv)
{
	puts("Are you sure format flash and update[y/n]\n");
	if('y' == (s_getchar() | 0x20))  
	{
		puts("\nCopy SDRAM 0x30000000 Nand\n");
		copy_sdram2nand();	
		puts("Finish\n");
	}
	else 
	{
		puts("Cancel\n");
	}
	return 0;
}


#ifdef _DEBUG_

int do_nand_readspeed_dbg(int argc,char **argv)
{
	unsigned char buffer[2048] = {0};
	unsigned int times = 1;
	unsigned int pageSize = 512;
	

	if(argc < 3) {
		printk("param error\n");
		return 1;
	}
	times    = atoi(argv[1]);
	pageSize = atoi(argv[2]);
	if(pageSize > 2048) {
		pageSize = 2048;
	}
	// for(int i = 0;i < 2048;++i) {
	// 	buffer[i] = 0x55;
	// }
	for(int i = 0;i < times;++i) {
		// __low_nand_read_page_dma(0,buffer,pageSize);		
		__low_nand_read_page(0,buffer,pageSize);		
	}
	
	

	PrintByte(buffer,pageSize);
	// PrintByte(0x30000000,512);
	return 0;
}

W_BOOT_CMD(rspeed_dbg,do_nand_readspeed_dbg,"test nand read speed");



int do_nand_writespeed_dbg(int argc,char **argv)
{
	unsigned char buffer[2048];
	unsigned int times = 1;
	unsigned int pageSize = 512;
	


	if(argc < 3) {
		printk("param error\n");
		return 1;
	}
	for(int i = 0;i < 2048;++i) {
		buffer[i] = 0xff;
	}
	times    = atoi(argv[1]);
	pageSize = atoi(argv[2]);
	if(pageSize > 2048) {
		pageSize = 2048;
	}

	for(int i = 0;i < times;++i) {
		__low_nand_read_page(NAND_BLOCK_ADDR * 2047,buffer,pageSize);		
	}
	
	

	// PrintByte(buffer,512);
	// PrintByte(0x30000000,512);
	return 0;
}
W_BOOT_CMD(wspeed_dbg,do_nand_writespeed_dbg,"test nand read speed");
#endif

W_BOOT_CMD(copyboot,do_copy_sdram2nand,"copy boot to Nand start address");


W_BOOT_CMD(nand,do_nand,"nand option");
// W_BOOT_CMD(mkdir,do_mkdir,"");

