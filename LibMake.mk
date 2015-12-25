#
# Makefile Template for "Dynamic Library"
#
# Description:
#	Automatic load file config list "config_xxx_file_list.mk"
#	which will be complie,when use this Makefile Template,
#	you only edit the config_xxx_file_list.mk
#
#
# Author: Menglong Wu
# E-mail:DreagonWoo@163.com
#
#
# V0.1		2014-08-23
#		2 demo makefile AppMakefile.mk and LibMakefile.mk
#

#################################################################
# Demo directory struction
# | Makefile
# 	-main.c
# |-dir1
# 	-f1_1.c
# 	-f1_2.c
# |-dir1
# 	|dir11
#		-f11_1.c
# |-dir2
# 	-f2_1.c
# between "----" and "----" copy to your Makefile and edit.
#---------------------------------------------------------------
# _NAME_		= any name 
# DIR_$(_NAME_) 	= directory name
# SRCS_$(_NAME_) 	= file list in the directory
# SRCS =  $(foreach TMP, \
# 	$(SRCS_$(_NAME_)), \
# 	$(DIR_$(_NAME_))/$(TMP))
#---------------------------------------------------------------

#################################################################
include config_lib_file_list.mk







#################################################################
OBJS = 	$(patsubst %.S,%.o,\
		$(patsubst %.c,%.o,$(SRCS)))

#################################################################
CROSS_COMPILE	=/opt/EmbedSky/crosstools_3.4.5_softfloat/gcc-3.4.5-glibc-2.3.6/arm-linux/bin/arm-linux-
ARCH=arm920t
NAME_ELF	= download.elf
NAME_DIS	= download.dis
NAME_BIN	= download.bin
NAME_LIB	= lib_unname
OUTPUT_DIR	= lib
#################################################################



#################################################################
CFLAGS		=  -g -D_DEBUG_ 
LFLAGS		=
INCLUDE_DIR 	=
LIB_DIR 	=
#################################################################



ifeq ("$(OUTPUT_DIR)", "")	
	OUTPUT_DIR=debug
endif
ifeq ("$(CROSS_COMPILE)", "")
else

endif


CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy







# CC=gcc
# APP_OUTPUT=main.elf
# APP_FILE=main.c
# APP_CFLAG=-std=gnu99 -Wall -Wl,-rpath=./ -export-dynamic 
# SO_FILE=-llib
# DEBUG=Y

# ifdef DEBUG
# 	ifeq (${DEBUG},Y)
# 		CFLAGS+=-gcc
# 	else
		
# 	endif
# endif


#liblib.so:lib1.o lib3.o
#	${CC} -o liblib.so -shared -fPIC lib1.o lib3.o -L=./:./lib/:./out/ -export-dynamic

all:$(OUTPUT_DIR) $(OBJS)
	@echo create $(OUTPUT_DIR)/$(NAME_LIB).a
	@$(AR) cqs $(OUTPUT_DIR)/$(NAME_LIB).a $(OBJS)

	@echo create $(OUTPUT_DIR)/$(NAME_LIB).so
	@$(CC) -shared -fPIC $(OBJS) -o $(OUTPUT_DIR)/$(NAME_LIB).so
		
%.o:%.c
	@echo compile $^
	@$(CC) -o $@ -c $^ $(CFLAGS)
%.o:%.S
	@echo compile $@
	@echo compile $^
	@$(CC) -o $@ -c $^ $(CFLAGS)


$(OUTPUT_DIR):
	echo $(OUTPUT_DIR)
	mkdir $(OUTPUT_DIR)
clean:
	@-rm 	$(OBJS) \
		$(OUTPUT_DIR)/$(NAME_LIB).so \
		$(OUTPUT_DIR)/$(NAME_LIB).a
rmtar:$(OUTPUT_DIR)
	@-rm -rf $(OUTPUT_DIR)