# include $(OUTPUT)

CROSS_COMPILE	=

NOWTIME=$(shell date "+%Y-%m-%d_%H:%M:%S")
 


#################################################################
# INCLUDE_DIR	- Where will be search *.h file
# LFLAGS		- Linking option
# LIB_DIR		- Where will be search *.so/*.a file
#-Wl,-rpath=./:./lib/

#when app.elf run will select *.so/a from $(PATH) -> ./ -> ./lib/
INCLUDE_DIR	= -I../include -I../osnet -I../shell -I../ -I../src -I./usr/include/readline -I./
LFLAGS		= -lreadline -lpthread -lhistory -lncurses -lsqlite3 -lmd5
LIB_DIR 	= 
CFLAGS     += -DBUILD_DATE=\"$(NOWTIME)\"
ifeq ("$(ARCH)", "x86")
	INCLUDE_DIR	+= 
	LFLAGS		+= -ltermcap  
	# -lefence 
	LIB_DIR 	+= -L/usr/local/install/lib
	CFLAGS		+= -DTARGET_X86
endif

ifeq ("$(ARCH)", "armv7")
	INCLUDE_DIR	+= -I/usr/4412/install/include
	LFLAGS		+= 
	LIB_DIR 	+= -L/usr/4412/install/lib
	CFLAGS		+= -DTARGET_ARMV7
endif


ifeq ("$(ARCH)", "arm920t")
	INCLUDE_DIR	+= -I/usr/arm920t/install/include
	LFLAGS		+= 
	LIB_DIR 	+= -L/usr/arm920t/install/lib
	CFLAGS		+= -DTARGET_ARM920T
endif


ifeq ("$(ARCH)", "i586")
	INCLUDE_DIR	+= -I/usr/win32/install/include
	LFLAGS		+= 
	LIB_DIR 	+= -L/usr/win32/install/lib
	CFLAGS		+= 
endif
#################################################################


#################################################################
OBJS = 	$(patsubst %.S,%.o,\
		$(patsubst %.cpp,%.o,\
		$(patsubst %.c,%.o,$(SRCS))))


# ifeq ("$(OUTPUT_DIR)", "")
# 	OUTPUT_DIR=debug
# endif

ifeq ("$(CROSS_COMPILE)", "")
else
endif


GCC_G++ = g++
CC 	= $(CROSS_COMPILE)$(GCC_G++)
LD 	= $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy

#################################################################
# CFLAGS		- Compile general option
# CC_FLAGS		- Compile only for *.c file option
# CS_FLAGS		- Compile only for *.S file option
CFLAGS		+= -g  	 -Wall -static -rdynamic -D_UNUSE_QT_ -fshort-wchar 
ifeq ("$(GCC_G++)","gcc") # 只有gcc编译器才使用该选项，g++无此选项
	CC_FLAGS    = -std=gnu99
else
	CC_FLAGS    = 
endif
CS_FLAGS    = 


CC_FLAGS   += $(CFLAGS)
CS_FLAGS   += $(CFLAGS)




all:$(ARCH)
#################################################################
# which ARCH

tool:$(OUTPUT_DIR) ./tools/wd.o	
	@$(CC) -o ./tools/randmac.elf ./tools/randmac.c $(LIB_DIR) $(LFLAGS)
	@$(CC) -o ./src/sq.elf ./src/sqlite_app.c $(LIB_DIR) $(LFLAGS) $(INCLUDE_DIR)
	@$(CC) -o ./tools/udiskupdate.elf ./tools/udiskupdate.c 
	@$(CC) -o ./tools/keyboard.elf ./tools/keyboard.c 
	# ./tools/keymap.c  ./tools/map_uk.c

	@$(CC) -shared -fPIC protocol/MD5.cpp -o release/libmd5.so
	@# @$(CC) -o ./tools/demo.elf ./tools/udiskupdate.c -g
	@# @$(CC) -o ./src/sq.elf ./src/sqlite_app.c $(LIB_DIR) $(LFLAGS) $(INCLUDE_DIR)
	@# @$(CC) -o ./tools/demo.elf ./tools/tui.c -lmenu -lncurses
	@$(CC) -o ./tools/demo.elf ./tools/wd.c


x86:$(OUTPUT_DIR) $(OBJS)
	echo ddddddddabcd
	@echo create $(OUTPUT_DIR)/$(NAME_ELF)
	@$(CC) -o $(OUTPUT_DIR)/$(NAME_ELF) $(OBJS) $(LIB_DIR) $(LFLAGS)

	@# @echo create $(OUTPUT_DIR)/$(NAME_ELF)
	@# @$(LD) -Tx86.lds $(OBJS) -o $(OUTPUT_DIR)/$(NAME_ELF) $(LFLAGS) $(LIB_DIR)  
	
	@# @-echo create $(OUTPUT_DIR)/$(NAME_BIN)
	@# @-$(OBJCOPY) -O binary -S $(OUTPUT_DIR)/$(NAME_ELF) $(OUTPUT_DIR)/$(NAME_BIN)


	@echo create $(OUTPUT_DIR)/$(NAME_DIS)
	@$(OBJDUMP) -S $(OUTPUT_DIR)/$(NAME_ELF) > $(OUTPUT_DIR)/$(NAME_DIS)
	

armv7:$(OUTPUT_DIR) $(OBJS)
	@echo create $(OUTPUT_DIR)/$(NAME_ELF)
	@$(CC) -o $(OUTPUT_DIR)/$(NAME_ELF) $(OBJS) $(LIB_DIR) $(LFLAGS)

	
	@# @echo create $(OUTPUT_DIR)/$(NAME_ELF)
	@# @$(LD) -Tx86.lds $(OBJS) -o $(OUTPUT_DIR)/$(NAME_ELF) $(LFLAGS) $(LIB_DIR)  
	
	@# @-echo create $(OUTPUT_DIR)/$(NAME_BIN)
	@# @-$(OBJCOPY) -O binary -S $(OUTPUT_DIR)/$(NAME_ELF) $(OUTPUT_DIR)/$(NAME_BIN)


	@echo create $(OUTPUT_DIR)/$(NAME_DIS)
	@$(OBJDUMP) -S $(OUTPUT_DIR)/$(NAME_ELF) > $(OUTPUT_DIR)/$(NAME_DIS)

i586:$(OUTPUT_DIR) $(OBJS)
	@echo create $(OUTPUT_DIR)/$(NAME_ELF)
	@$(CC) -o $(OUTPUT_DIR)/$(NAME_ELF) $(OBJS) $(LIB_DIR) $(LFLAGS)

	@# @echo create $(OUTPUT_DIR)/$(NAME_ELF)
	@# @$(LD) -Tx86.lds $(OBJS) -o $(OUTPUT_DIR)/$(NAME_ELF) $(LFLAGS) $(LIB_DIR)  
	
	@# @-echo create $(OUTPUT_DIR)/$(NAME_BIN)
	@# @-$(OBJCOPY) -O binary -S $(OUTPUT_DIR)/$(NAME_ELF) $(OUTPUT_DIR)/$(NAME_BIN)


	@echo create $(OUTPUT_DIR)/$(NAME_DIS)
	@$(OBJDUMP) -S $(OUTPUT_DIR)/$(NAME_ELF) > $(OUTPUT_DIR)/$(NAME_DIS)


arm920t:$(OUTPUT_DIR) $(OBJS)
	@echo create $(OUTPUT_DIR)/$(NAME_ELF)
	@$(CC) -o $(OUTPUT_DIR)/$(NAME_ELF) $(OBJS) $(LIB_DIR) $(LFLAGS)

	@# @echo create $(OUTPUT_DIR)/$(NAME_ELF)
	@# @$(LD) -Tx86.lds $(OBJS) -o $(OUTPUT_DIR)/$(NAME_ELF) $(LFLAGS) $(LIB_DIR)  
	
	@# @-echo create $(OUTPUT_DIR)/$(NAME_BIN)
	@# @-$(OBJCOPY) -O binary -S $(OUTPUT_DIR)/$(NAME_ELF) $(OUTPUT_DIR)/$(NAME_BIN)


	@echo create $(OUTPUT_DIR)/$(NAME_DIS)
	@$(OBJDUMP) -S $(OUTPUT_DIR)/$(NAME_ELF) > $(OUTPUT_DIR)/$(NAME_DIS)

arm9220t:$(OUTPUT_DIR) $(OBJS)

	@echo create $(OUTPUT_DIR)/$(NAME_ELF)
	@$(LD) -Tboot.lds $(OBJS) -o $(OUTPUT_DIR)/$(NAME_ELF) $(LFLAGS) $(LIB_DIR)  
	
	@echo create $(OUTPUT_DIR)/$(NAME_BIN)
	@$(OBJCOPY) -O binary -S $(OUTPUT_DIR)/$(NAME_ELF) $(OUTPUT_DIR)/$(NAME_BIN)


	@echo create $(OUTPUT_DIR)/$(NAME_DIS)
	@$(OBJDUMP) -S $(OUTPUT_DIR)/$(NAME_ELF) > $(OUTPUT_DIR)/$(NAME_DIS)
#################################################################
%.o:%.c
	@echo compile $^
	@$(CC) -o $@ -c $^ $(CC_FLAGS) $(INCLUDE_DIR) 
%.o:%.cpp
	@echo compile $^
	@$(CC) -o $@ -c $^ $(CC_FLAGS) $(INCLUDE_DIR) 
%.o:%.S
	@echo compile $^
	@$(CC) -o $@ -c $^ $(CS_FLAGS) $(INCLUDE_DIR)

$(OUTPUT_DIR):
	mkdir $(OUTPUT_DIR)
clean:
	@-rm 	$(OBJS) \
		$(OUTPUT_DIR)/$(NAME_DIS) \
		$(OUTPUT_DIR)/$(NAME_ELF) \

		core
		# $(OUTPUT_DIR)/$(NAME_BIN)
toolclean:
	@-rm  tools/*.elf                \
		  tools/*.o                

		  
rmoutput:$(OUTPUT_DIR)
	@-rm -rf $(OUTPUT_DIR)
rmdb:
	@-rm /etc/tmsxx.db
sqlite3:
	sqlite3 /etc/tmsxx.db
run:
	./$(OUTPUT_DIR)/$(NAME_ELF)
copy:
	cp ./$(OUTPUT_DIR)/$(NAME_ELF) /usr/armdebug/tms4412.elf
gdb:
	gdb ./$(OUTPUT_DIR)/$(NAME_ELF)
gdb-core:
	gdb ./$(OUTPUT_DIR)/$(NAME_ELF) core
