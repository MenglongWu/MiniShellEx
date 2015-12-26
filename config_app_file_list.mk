# CONFIG_YAFFS1 = y
# CONFIG_WML_BOOTLOADER = y
# CONFIG_MINISHELL_CORE = y
# CONFIG_LCD = y

# 最小系统代码
SRCS-$(CONFIG_WML_BOOTLOADER) = src/start.S src/copyboot.c  src/board.c   src/mainboot.c 
SRCS-$(CONFIG_WML_BOOTLOADER) += nand/Nand_S34ML02G1.c

# C库
SRCS-y += clib/ctype.c
SRCS-y += clib/ubootlibc.c
SRCS-y += clib/string.c

# MiniShell控制台
SRCS-$(CONFIG_MINISHELL_CORE) += shell/minishell_core.c






# SRCS-y += nand/nand_bbt.c
SRCS-y += nand/cmd_nand.c


# yaffs
# SRCS-y += fs/yaffs1/exp.c
SRCS-$(CONFIG_YAFFS1) += fs/yaffs1/cmd_glue.c
SRCS-$(CONFIG_YAFFS1) += fs/yaffs1/mem/mem.c
SRCS-$(CONFIG_YAFFS1) += fs/yaffs1/nand_flash/nand_flash.c
SRCS-$(CONFIG_YAFFS1) += fs/yaffs1/yaffscfg.c  
SRCS-$(CONFIG_YAFFS1) += fs/yaffs1/yaffs_ecc.c  
SRCS-$(CONFIG_YAFFS1) += fs/yaffs1/yaffs_flashif.c  
SRCS-$(CONFIG_YAFFS1) += fs/yaffs1/yaffsfs.c  
SRCS-$(CONFIG_YAFFS1) += fs/yaffs1/yaffs_guts.c
# # 文件系统
SRCS-$(CONFIG_YAFFS1) += shell/cmd/cmd_yaffs1.c



# LCD 驱动
SRCS-$(CONFIG_LCD) += driver/lcd/s3c2440_lcd.c \
# SRCS-$(CONFIG_LCD) += driver/lcd/LR043JC211.c
SRCS-$(CONFIG_LCD) += driver/lcd/lcdtest.c
# SRCS-$(CONFIG_LCD) += driver/lcd/LR043JC211_16bpp_uboot.c

# SRCS-$(CONFIG_LCD) += driver/lcd/wml_lcd.o
# SRCS-$(CONFIG_LCD) += driver/lcd/lcd.c




# glui lib
SRCS-$(CONFIG_LCD) += driver/glui/gl_ui.c
SRCS-$(CONFIG_LCD) += driver/glui/zimo_st9.c
SRCS-$(CONFIG_LCD) += driver/glui/zimo_st20.c


#IRQ
SRCS-$(CONFIG_2440_IRQ) += driver/s3c2440_irq.c

#TOUCH depond CONFIG_2440_IRQ
SRCS-$(CONFIG_2440_TOUCH) += driver/touch/s3c2440_touch.c
SRCS-$(CONFIG_TOUCH_TEST) += driver/touch/touchtest.c

# IIC
SRCS-$(CONFIG_2440_IIC) += driver/iic/s3c2440_iic.c
SRCS-$(CONFIG_AT24C02) += driver/iic/at24c02.c

# CAMIF
ifeq ("$(CONFIG_USE_TQ)","y")
	SRCS-$(CONFIG_2440_CAMIF) += driver/tq_camif/s3c2440_camif.c
	SRCS-$(CONFIG_2440_CAMIF) += driver/tq_camif/camproset.c
	SRCS-$(CONFIG_OV9650) += driver/tq_camif/ov9650.c
endif




SRCS-y = src/main.c
SRCS-y += shell/minishell_core.c
SRCS-y += shell/minishell_core_ex.c