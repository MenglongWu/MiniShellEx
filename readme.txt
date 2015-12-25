/**
 ******************************************************************************
 * @file	readme.txt
 * @brief	lcd/readme.txt

s3c2440 LCD 性能测试程序
 * 
- 2015-8-9,Menglong Woo,MenglongWoo@163.com
	- 基于LR043JC211 LCD的驱动

*/


"****************"
<文件包含>
"------------------------------------------------------------------------------"
	src/start.S
	src/copyboot.c
	src/board.c
	src/mainboot.c
	nand/Nand_S34ML02G1.c
	clib/ctype.c
	clib/ubootlibc.c
	clib/string.c
	shell/minishell_core.c		// 控制台shell
	nand/cmd_nand.c
	driver/lcd/s3c2440_lcd.c	// s3c2440 LCD 驱动
	driver/lcd/lcdtest.c		// lcd测试代码
	driver/glui/gl_ui.c			// gl_ui 图形简易库
	driver/glui/zimo_st9.c		// 宋天9号字模，供gl_ui用
	driver/glui/zimo_st20.c		// 宋天20号字模，供gl_ui用


"****************"
<工程配置>
"------------------------------------------------------------------------------"
	1 .配置s3c2440 的驱动在 driver/lcdconf.h，配置s3c2440 lcd控制器的参数
	2. lcd.h是一个简易的lcd中间层驱动，应用层简单调用lcd.h里的接口即可操作lcd，屏蔽底层驱动
	3. gl_ui 简易图形库，主要用于在lcd上打印Ascii调试信息，也包括其他lcd驱动，
	gl_ui的移植只要在gl_ui_config.h里实现setpixel函数（绘制一个像素点）

"****************"
<运行过程>
"------------------------------------------------------------------------------"
**************  Menglong Wu Bootloader Run  **************

read s3c2440 LCD register
        lcdcon1    3ac047b
        lcdcon2    243c108
        lcdcon3    51df0a
        lcdcon4    d1e
        lcdcon5    14308
        lcdsaddr1  180066fe
        lcdsaddr2  462fe
        lcdsaddr3  3c0
        redlut     0
        greenlut   0
        bluelut    0
        reserved   4d00002c
        dithmode   0
        tpal       0
        lcdintpnd  3
        lcdsrcpnd  3
        lcdintmsk  0
        tconsel    ffffff


Project command help
  hostname      set hostname
  help          shell help
  null          shell help
  nand          nand option
  copyboot      copy boot to Nand start address
  wspeed_dbg    test nand read speed
  rspeed_dbg    test nand read speed
  lcdbl         LCD blacklight on/off(1/0)
  lcdpower      LCD control on/off(1/0)
  lcdtext       Test gl_ui library
  lcdrgb        Test break point
  lcdorder      Test gl_ui library
  lcdspeed      Test LCD draw speed (all screen)

运行时首先打印 S3C2440 lcd 控制器的寄存器值。
lcdbl ~ lcdspeed 分别是测试：
	lcdbl		lcd背光开关
	lcdpower	lcd控制器使能
	lcdtext		输出宋体英文字母 lcdtext abcd 屏幕输出abcd
	lcdrgb		测试坏块
	lcdorder	测试lcd rgb三色顺序是否正确，输出一条彩带
	lcdspeed	测试lcd 刷新速率，全屏清屏，绘制一条直线，直到绘制屏幕低端，初略计算lcd刷新速率和DMA控制速率