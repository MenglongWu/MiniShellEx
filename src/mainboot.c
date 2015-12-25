/**
 ******************************************************************************
 * @file	mainboot.c
 * @brief	
 *			S3C2440主程序C函数入口
*/
/*--------------------------------------------------
 * version    |    author    |    date    |    content
 * V1.0			Menglong Wu		2014-11-10	
 ******************************************************************************
*/

#include "autoconfig.h"
#include "board_config.h"
#include "2440addr.h"

#include "board.h"
#include "nandflash.h"
#include "string.h"

#include "minishell_core.h"
#include "../nand/nand_bbt.h"

#ifdef CONFIG_YAFFS1
#include "yaffsfs.h"
#endif

#include "lcd.h"

#include "s3c2440_irq.h"
#include "s3c2440_touch.h"

int do_help(int argc,char **argv);
int do_lcdorder(int argc,char **argv);

void LedIocl(unsigned long val)
{
	unsigned long tmpreg;

	tmpreg = rGPBDAT;
	tmpreg |= ((1 << 5) | (1 << 6) | (1 << 7) | (1 << 8));
	val = val & 0x3;
	tmpreg &= ~((1 << (val+5)));
	rGPBDAT = tmpreg;
}

int main()
{
	// puts("\ngo.go.go1111\n");
	puts("\n**************  Menglong Wu Bootloader Run  **************\n");
	LedIocl(2);
	
#ifdef CONFIG_YAFFS1
	yaffs_StartUp();
	yaffs_mount("/boot");
#endif
			// yaffs_mkdir("/boot/abcd");
			// ListDir("/boot");
	
	

	puts("\nProject command help\n");
	do_help(0,NULL);
#ifdef CONFIG_LCD
	lcd_init();
	do_lcdorder(0,NULL);
#endif

	InitKeyInterrupt();
#ifdef CONFIG_2440_TOUCH
	init_touch();
#endif
	// irq_request(ISR_EINT0_OFT, EINT0_Handle);
	// irq_request(ISR_EINT1_OFT, EINT1_Handle);
	// irq_request(ISR_EINT2_OFT, EINT2_Handle);
	// irq_request(ISR_EINT4_7_OFT, EINT4_7_Handle);

	irq_enable();

	
	sh_enter();
	puts("Press key select\n");
	puts("\tC.Copy Bootloader from SDRAM 0x30000000 to Nand\n");
	puts("\tG.Go run\n");

	
	
	if(s_getchar() == 'c') {
	 	 puts("copy\n");

	 	 copy_sdram2nand();
	 	 puts("finish\n");
	}
	else {
		puts("go run\n");
	}
	// sh_enter();
	
	while (1) {
	}
	
	return 0;
}

