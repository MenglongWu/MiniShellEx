OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS {
	. = 0x30000000;	
	. = ALIGN(4);
	.text :
	{
		src/start.o (.text)
		src/copyboot.o (.text)
		nand/Nand_S34ML02G1.o (.text)
		src/board.o (.text)
		*(.text)
	}

	. = ALIGN(4);
	.data : 
	{
	*(.data)
	}

	. = ALIGN(4);
	.rodata : 
	{
	*(.rodata)
	}



	. = ALIGN(4);
	__u_boot_cmd_start = .;
	.u_boot_cmd : { *(.u_boot_cmd) }
	__u_boot_cmd_end = .;


	. = ALIGN(4);
	__w_boot_cmd_start = .;
	.w_boot_cmd : { *(.w_boot_cmd) }
	__w_boot_cmd_end = .;


	. = ALIGN(4);
	bss_start = .;
	.bss : 
	{
	*(.bss) 
	}
	bss_end = .;
}
