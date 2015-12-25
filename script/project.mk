
PRJ_VERSION = "1.0.0"
PRJ_NAME = "Hello-KuGo_Menglong"


load_lds = y
CROSS_COMPILE = /opt/EmbedSky/crosstools_3.4.5_softfloat/gcc-3.4.5-glibc-2.3.6/arm-linux/bin/arm-linux-

# default download.elf,download.dis,download.bin
OUTPUT_ELF	=
OUTPUT_DIS	=
OUTPUT_BIN	= 
OUTPUT_SO 	= 
OUTPUT_A	= 

OUTPUT_DIR = release




INCLUDE_DIR    = -I./include \
				 -I./fat/src \
				 -I./shell \
				 -I./fs/yaffs1 \
				 -I./fs/yaffs1/mem \
				 -I./fs/yaffs1/nand_flash \
				 -I./driver/glui

LFLAGS    = -static -lgcc
LIB_DIR    = -L./ -L/opt/EmbedSky/crosstools_3.4.5_softfloat/gcc-3.4.5-glibc-2.3.6/arm-linux/lib/gcc/arm-linux/3.4.5
CFLAGS    = -D_DEBUG_ -Wall -static -O3 -g

