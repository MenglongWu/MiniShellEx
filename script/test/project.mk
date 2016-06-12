
PRJ_VERSION = "1.0.0"
PRJ_NAME = "testbuild"


# load_lds = y
# CROSS_COMPILE = /opt/EmbedSky/crosstools_3.4.5_softfloat/gcc-3.4.5-glibc-2.3.6/arm-linux/bin/arm-linux-

# default download.elf,download.dis,download.bin
OUTPUT_ELF	= testbuild
OUTPUT_DIS	=
OUTPUT_BIN	= 
OUTPUT_SO 	= 
OUTPUT_A	= 

OUTPUT_DIR = release




INCLUDE_DIR    = -I./usr/include  \
				-I./include \
				-I./shell \
				-I/usr/local/install/include

LFLAGS    = -lreadline -lhistory -ltermcap -lxml2 -lminishell-ex
 # -lefence -lpthread
lIB_DIR   = -L/usr/local/install/lib
CFLAGS    = -g  -DAUTO_BUILD

