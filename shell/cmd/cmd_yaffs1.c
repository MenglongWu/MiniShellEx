

#include "minishell_core.h"
#include "board.h"
#include "board_config.h"
#include "nandflash.h"

// #include "clib.h"


extern int nbbt_scan();
extern int nbbt_writeSector(unsigned int sector,unsigned char *buf,unsigned int count);
extern int atoi(const char *s);
extern char __low_nand_erase_block(unsigned int row_addr);





int do_wfile(int argc,char **argv)
{
	//todo:more
	puts("cmd read file only for test\n");
	return 0;
	
}

int do_rfile(int argc,char **argv)
{
	//todo:more
	puts("cmd read file only for test\n");
	return 0;
	
}

void copy_file2nand(unsigned int ramAddr,unsigned int blockAddr,unsigned int fileSize)
{
	//计算boot代码大小，确定需要格式化Block的块总数、需要写入的Page总数
	unsigned long bootSize = fileSize;		//uboot代码长度
	unsigned long blockTotal,pageTotal;

	
	blockTotal = 	(
						(bootSize + NAND_BLOCK_MASK ) & (~NAND_BLOCK_MASK)
					) 	/ NAND_BLOCK_SIZE;
	pageTotal  = 	(
						(bootSize + NAND_PAGE_MASK ) & (~NAND_PAGE_MASK)
					)	/ NAND_PAGE_SIZE;

	printk("\r\ncode size 0x%x (%dByte)\r\n",bootSize,bootSize);
	printk("require block %d require page %d\r\n",blockTotal,pageTotal);
	
	//烧录进FLASH，不带坏块检测
	char retErase;
	unsigned char *lpsrcData = (unsigned char *)ramAddr;	//SDRAM 0x30000000地址的代码
	unsigned int off_data,page_addr;

	retErase = nand_erase_block(blockAddr);
	// printk("retrEsdrase %d\r\n",retErase);
	off_data = 0;
	page_addr = blockAddr;
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

int do_mount(int argc,char **argv)
{
	yaffs_mount("/boot");
	yaffs_mkdir("/boot/mydoc", 0);
	cmd_yaffs_ls("/boot",0);
}








int do_file2nand(int argc,char **argv)
{

	int ramAddr,nandBlockAddr,fileSize;

	if(argc == 4) {
		ramAddr 		= atoi(argv[1])*16;
		nandBlockAddr   = atoi(argv[2])*16;
		fileSize   		= atoi(argv[3]);
		printk("ramAddr 0x%8.8x nandBlockAddr %8.8x fileSize %d\n",ramAddr,nandBlockAddr,fileSize);
		copy_file2nand(ramAddr,nandBlockAddr,fileSize);
	}
	else {
		printk("Usage:\n");
		printk("\tformat <RAM addr> <Block addr>\n");
		return 0;
	}
	// __w_boot_cmd_mount = __w_boot_cmd_mount;
	//__w_boot_cmd_a.fun = __w_boot_cmd_a.fun;
	return 0;

	// puts("Are you sure format flash and update[y/n]\n");

	// if('y' == (s_getchar() | 0x20))  
	// {
	// 	puts("\nCopy SDRAM 0x30000000 Nand\n");
	// 	copy_sdram2nand();	
	// 	puts("Finish\n");
	// }
	// else 
	// {
	// 	puts("Cancel\n");
	// }
}


//struct cmd_table _w_boot_cmd_name = {name};

//struct cmd_table __w_boot_cmd_name = {"name", 0, "dsssdf"};





int do_ls(int argc,char **argv)
{
	char *dirname;

	if (argc < 2 || argc > 3 || (argc == 3 && strcmp(argv[1], "-l"))) {
		printk("Bad arguments: yls [-l] dir\n");
		return -1;
	}

	dirname = argv[argc - 1];

	return cmd_yaffs_ls(dirname, (argc > 2) ? 1 : 0);

}

static int ycheck_option_valid(const char *str)
{
	return strcmp(str,"DIR") == 0 || strcmp(str,"REG") == 0;
}

int do_ycheck(int argc,char **argv)
{
	const char *dirname;
	const char *str = "ANY";

	if (argc < 2 || argc > 3 ||
	(argc == 3 && ! ycheck_option_valid(argv[2]))) {
		printk("Bad arguments: ycheck name [DIR|REG]");
		return -1;
	}

	dirname = argv[1];
	if(argc>2)
		str = argv[2];

	return cmd_yaffs_check(dirname, str);
}


int do_yrd(int argc,char **argv)
{
	char *filename;

	if (argc != 2) {
		printk("Bad arguments: yrd file_name\n");
		return -1;
	}

	filename = argv[1];

	printk("Reading file %s\n", filename);

	return cmd_yaffs_read_file(filename);
}

#ifdef _DEBUG_
int do_ywr_dbg(int argc,char **argv)
{
	char *filename;
	unsigned long  value;
	unsigned long  numValues;

	if (argc != 4) {
		printk("Bad arguments: ywr file_name value n_values\n");
		return -1;
	}

	filename = argv[1];
	value = simple_strtol(argv[2],0,16);
	numValues = simple_strtoul(argv[3], 0, 16);

	printk("Writing value (%lx) %lx times to %s... \n", value, numValues,
	       filename);

	return cmd_yaffs_write_file_dbg(filename, value, numValues);
}
#endif

int do_ywr(int argc,char **argv)
{
	char *filename;
	unsigned long  addr;
	unsigned long  nSize;

	if (argc != 4) {
		printk("Bad arguments: ywr file_name value n_values\n");
		return -1;
	}

	filename  = argv[1];
	addr      = simple_strtoul(argv[2], 0, 16);
	nSize     = simple_strtoul(argv[3], 0, 16);

	printk("Writing data addr (%lx) size %lx times to %s... \n", addr, nSize,
	       filename);

	return cmd_yaffs_write_file(filename, addr, nSize);
}

int do_mkdir(int argc,char **argv)
{
	//todo:more
	
	if(argc >= 2) {
		printk("mkdir %s\n",argv[1]);
		yaffs_mkdir(argv[1],0);	
	}
	return 0;	
}
int do_rmdir(int argc,char **argv)
{
	char *dirname;

	if (argc != 2) {
		printk("Bad arguments: yrmdir dir_name\n");
		return -1;
	}

	dirname = argv[1];
	return cmd_yaffs_rmdir(dirname);
}

int do_mv(int argc,char **argv)
{
	char *oldPath;
	char *newPath;

	if (argc != 3) {
		printk("Bad arguments: ymv old_path new_path\n");
		return -1;
	}

	oldPath = argv[1];
	newPath = argv[2];

	return cmd_yaffs_mv(newPath, oldPath);
}

int do_rm(int argc,char **argv)
{
	char *name;

	if (argc != 2) {
		printk("Bad arguments: yrm name\n");
		return -1;
	}

	name = argv[1];

	return cmd_yaffs_rm(name);
}

int do_copy(int argc,char **argv)
{
	if(argc != 3) {
		printk("param error\n");
		return 1;
	}
	cmd_yaffs_cp(argv[1],argv[2]);
	return 0;
}

W_BOOT_CMD(ls,do_ls,"");

W_BOOT_CMD(check,do_ycheck,"");
W_BOOT_CMD(mkdir,do_mkdir,"");
W_BOOT_CMD(rmdir,do_rmdir,"");
W_BOOT_CMD(rm,do_rm,"");
W_BOOT_CMD(mv,do_mv,"");

W_BOOT_CMD(rfile,do_yrd,"read file");
W_BOOT_CMD(wfile,do_ywr,"write file");
W_BOOT_CMD(wfiledbg,do_ywr_dbg,"debug only write 16Hex in file");
W_BOOT_CMD(cp,do_copy,"copy file");


W_BOOT_CMD(mount,do_mount,"dfdf");
// W_BOOT_CMD(rfile,do_rfile,"dfdf");
// W_BOOT_CMD(wfile,do_wfile,"dfdf");