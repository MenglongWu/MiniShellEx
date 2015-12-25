#ifndef _S3C2440_IRQ_H_
#define _S3C2440_IRQ_H_


void *irq_request(int index, void (*pfun)(void));
void InitKeyInterrupt();

void EINT0_Handle();
void EINT1_Handle();
void EINT2_Handle();
void EINT4_7_Handle();




#define irq_enable() \
{ \
	__asm__ volatile ( "msr	cpsr_c, #0x53;"); \
}

 


#define irq_disable() \
{					 \
	__asm__ volatile ( "msr	cpsr_c, #0xd3;"); \
}

#define fiq_enable() \
{					 \
	__asm__ volatile ("msr cpsr_c, #0x53;"); \
}

#define fiq_disable() \
{					 \
	__asm__ volatile ("msr cpsr_c, #0xD3;"); \
}

#endif


