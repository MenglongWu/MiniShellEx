/**
 ******************************************************************************
 * @file	start.S
 * @brief	
 *			S3C2440启动代码

 	主要完成功能：
 			1.初始化SDRAM\n
 			2.设置各模式下的栈地址\n
 			3.拷贝Nand代码到SDRAM\n
 			4.跳转到SDRAM里的C代码\n
*/
/*--------------------------------------------------
 * version    |    author    |    date    |    content
 * V1.0			Menglong Wu		2014-09-03	1.目标板TQ2440
 * V1.0.1		Menglong Wu		2014-11-10	
 ******************************************************************************
*/



#include "arm920t.h"

#define SDRAM_BASE 0x30000000


.text
.global _start
_start:
	b reset
	ldr pc, _undifined_instruction
	ldr pc, _software_interrupt
	ldr pc, _prefetch_abort
	ldr pc, _data_abort
	ldr pc, _not_used
	ldr pc, _irq
	ldr pc, _fiq

_undifined_instruction: 	.word undifined_instruction 
_software_interrupt: 		.word software_interrupt
_prefetch_abort: 			.word prefetch_abort
_data_abort: 				.word data_abort
_not_used: 					.word not_used
_irq: 						.word irq
_fiq: 						.word fiq

undifined_instruction:
	nop

software_interrupt:
	nop
	
prefetch_abort:
	nop
	
data_abort:
	nop

not_used:
	nop

irq:
    sub lr, lr, #4                  @ 计算返回地址
    stmdb   sp!,    { r0-r12,lr }   @ 保存使用到的寄存器
                                    @ 注意，此时的sp是中断模式的sp
                                    @ 初始值是上面设置的3072
	#bl error_buzzer
     
    ldr lr, =int_return             @ 设置调用ISR即EINT_Handle函数后的返回地址  
    ldr pc, =EINT_Handle            @ 调用中断服务函数，在interrupt.c中
int_return:
    ldmia   sp!,    { r0-r12,pc }^  @ 中断返回, ^表示将spsr的值复制到cpsr
 	
fiq:	
	nop


reset:
	bl boot_led0
	
	bl disable_watchdog
	bl disable_interrupt
	bl init_clock
	bl init_sdram
	# bl copy_steppingstone_to_sdram
# 	# bl	disable_mmu
	
	bl	clear_bss
	
 	bl init_stack
 	bl boot_led						@ 显示LED
 	bl usart_init					@ 初始化串口
 	bl copy_nand2sdram				@ 拷贝

 	
	
# 	ldr	pc, _main
# _main:
# 	.word main
 	ldr	pc, =main                @ 调用C程序中的main函数
 	# bl main
 	
halt_loop:
	b       halt_loop



.globl _bss_start
_bss_start:
	.word bss_start

.globl _bss_end
_bss_end:
	.word bss_end

clear_bss:
	ldr	r0, _bss_start
	ldr	r1, _bss_end
	mov 	r2, #0x00000000

clbss_l:str	r2, [r0]
	add	r0, r0, #4
	cmp	r0, r1
	ble	clbss_l
	mov	pc, lr






disable_watchdog:
	ldr     r0, =0x53000000     @ WATCHDOG寄存器地址
	mov     r1, #0x0                     
	str   	r1, [r0]              @ 写入0，禁止WATCHDOG，否则CPU会不断重启
	mov	pc, lr
init_stack:
	#todo set arm mode
	# @ldr     sp, =1024*4	@ 设置堆栈，注意：不能大于4k, 因为现在可用的内存只有4K
				# @ nand flash中的代码在复位后会移到内部ram中，此ram只有4K


	# mrs	r0, cpsr
	# bic	r0, r0, #0x1f
	# orr	r0, r0, #0xdb
	# msr	cpsr_c, r0                @UndefMode		
	# ldr	sp,=3296@UndefStack		@UndefStack=0x33FF_5C00

	# mrs	r0, cpsr
	# bic	r0, r0, #0x1f
	# orr	r0, r0, #0xd7
	# msr	cpsr_c, r0                @AbortMode
	# ldr	sp,=3496@AbortStack		@AbortStack=0x33FF_6000

	mrs	r0, cpsr
	bic	r0, r0, #0x1f
	orr	r0, r0, #0xd2
	msr	cpsr_c, r0                @IRQMode
	ldr	sp,=0x33FF7000  @IRQStack		@IRQStack=0x33FF_7000
	

	# mrs	r0, cpsr
	# bic	r0, r0, #0x1f
	# orr	r0, r0, #0xd1
	# msr	cpsr_c, r0                @FIQMode
	# ldr	sp,=3896@FIQStack		@FIQStack=0x33FF_8000

	mrs	r0, cpsr
	bic	r0, r0, #0x1f
	orr	r0, r0, #0xd3
	msr	cpsr_c, r0                @SVCMode
	ldr	sp,=0x34000000   @SVCStack		@SVCStack=0x33FF_5800

 	mov	pc, lr


disable_interrupt:
	msr cpsr_c, #0xd3
	mov pc, lr

# rMPLLCON
# Mdiv
# m = M + 8,	p = P + 2
# FCLK = MPLL = (2* m * Fin) / (p * 2^s)
#        UPLL = (   m * Fin) / (p * 2^s)
# bit[19:12]	MDIV
# bit[9:4]		PDIV
# bit[1:0]		SDIV
# Fin = 12MHz

#define MPLL_405MHZ ((127<<12)|(2<<4)|(1<<0))		

# rCLKDIVN
# bit[3]	0	DIVN_UPLL 	:	UCLK select register(UCLK must be 48MHz for USB)
# -								0: UCLK = UPLL clock	1: UCLK = UPLL clock / 2
# 								Set to 0, when UPLL clock is set as 48MHz
# 								Set to 1. when UPLL clock is set as 96MHz.
# bit[2:1]	2	HDIV		:	HCLK = FCLK/4 when CAMDIVN[9] = 0
# bit[0]	1	PDIV		:	PCLK = HCLK/2
# ----> 
# 		FCLK = 405MHz, HCLK = 101.25MHz, PCLK = 50.625

init_clock:
	ldr r0, =rCLKDIVN
	mov r1, #0x5
	str r1, [r0]

	ldr r0, =rMPLLCON
	ldr r1, =MPLL_405MHZ
	str r1, [r0]
	

	
	
	mcr p15,0,r0,c1,c0,0
	orr r0,r0,#0xc0000000
	mcr p15,0,r0,c1,c0,0
	
	mov pc, lr
	
# #define mem_contrl 0x48000000
init_sdram:
	ldr r0, =rBWSCON 
	add r3, r0, #4*13
	adrl r1, mem_data

0:
	ldr r2, [r1], #4
	str r2, [r0], #4
	cmp r0, r3
	bne 0b
	mov pc, lr




# copy_steppingstone_to_sdram:
# 	mov r1, #0
# 	ldr r2,=SDRAM_BASE
# 	mov r3, #4*1024
# 1:
# 	ldr r4, [r1],#4
# 	str r4, [r2],#4
# 	cmp r1, r3
# 	bne 1b
# 	mov pc, lr

mem_data:
	.long 0x2211d120 
	.long 0x00000700
	.long 0x00000700
	.long 0x00000700
	.long 0x00001f4c
	.long 0x00000700
	.long 0x00000700
	.long 0x00018005
	.long 0x00018005
	.long 0x00ae03f4
	.long 0x00000032
	.long 0x00000030
	.long 0x00000030

# # mem_data:
# # 	.long 0x22000000 
# # 	.long 0x00000700
# # 	.long 0x00000700
# # 	.long 0x00000700
# # 	.long 0x00000700
# # 	.long 0x00000700
# # 	.long 0x00000700
# # 	.long 0x00018001
# # 	.long 0x00018001
# # 	.long 0x008c04f5
# # 	.long 0x000000b1
# # 	.long 0x00000030
# # 	.long 0x00000030

.global boot_led3
boot_led3:
	ldr	r0,=0x56000010
	ldr	r1,=0x15401
	str 	r1,[r0]

	ldr	r0,=0x56000014
	ldr	r1,=0x700
	str 	r1,[r0]

	mov	pc, lr





boot_led0:
	ldr	r0,=0x56000010
	ldr	r1,=0x15401
	str 	r1,[r0]

	ldr	r0,=0x56000014
	ldr	r1,=0x7F0
	str 	r1,[r0]

	mov	pc, lr

.global boot_led
boot_led:
	ldr	r0,=0x56000010
	ldr	r1,=0x15401
	str 	r1,[r0]

	ldr	r0,=0x56000014
	ldr	r1,=0x6F0
	str 	r1,[r0]

	mov	pc, lr

error_buzzer:
	ldr	r0,=0x56000010
	ldr	r1,=0x15401
	str 	r1,[r0]

	ldr	r0,=0x56000014
	ldr	r1,=0x1
	str 	r1,[r0]
	
	mov	pc, lr





.global _where_lr

_where_lr:
	mov r0, lr
	mov pc, lr

