SRCS += mem/mem.c

SRCS += nand_flash/nand_flash.c

SRCS += yaffs/exp.c
SRCS += yaffs/yaffscfg.c  
SRCS += yaffs/yaffs_ecc.c  
SRCS += yaffs/yaffs_flashif.c  
SRCS += yaffs/yaffsfs.c  
SRCS += yaffs/yaffs_guts.c


INCLUDE = -I./mem -I./nand_flash -I./yaffs

all:
	arm-linux-gcc $(SRCS) $(INCLUDE)
clean:
	-rm mem/*.o
	-rm nand_flash/*.o
	-rm yaffs/*.o