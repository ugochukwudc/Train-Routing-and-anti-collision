/*
 * Process.c
 * Support functions
*/
#include <stdio.h>
#include "process.h"

unsigned long get_PSP(void)
{
/* Returns contents of PSP (current process stack */
__asm("	mrs r0, psp");
__asm("	bx lr");
return 0;			/***** Not executed -- shuts compiler up */
			/***** If used, will clobber 'r0' */
}

unsigned long get_MSP(void)
{
/* Returns contents of MSP (main stack) */
__asm("	mrs r0, msp");
__asm("	bx lr");
return(0);
}

void set_PSP(volatile unsigned long ProcessStack)
{
/* set PSP to ProcessStack */
__asm("	msr	psp, r0");
}

void set_MSP(volatile unsigned long MainStack)
{
/* Set MSP to MainStack */
__asm("	msr	msp, r0");
}

void set_LR(volatile unsigned long LR_Value)
{
/* Set LR to LR_Value */
__asm(" 	mov r14,r0");
printf("LR value is set as %lx",LR_Value);
__asm("		bx lr");
}

void  save_registers()
{
/* Save r4..r11 on process stack */
__asm("	mrs r0,psp");
__asm("	stmdb r0!,{r4-r11}");	/* Store multiple, decrement before */
__asm("	msr	psp,r0");
}

void  restore_registers()
{
/* Restore r4..r11 from stack to CPU */
__asm("	mrs	r0,psp");
__asm("	ldmia r0!,{r4-r11}");	/* Load multiple, increment after */
__asm("	msr	psp,r0");
}

unsigned long get_SP()
{
__asm(" 	mov r0,SP"); /**** Leading space required -- for label ****/
__asm("		bx lr");
return 0;
}

void set_SP(volatile unsigned long data)
{
__asm(" 	mov SP,r0");
}

void assignR7(volatile unsigned long data)
{
/* Assign 'data' to R7; since the first argument is R0, this is
 * simply a MOV for R0 to R7
 */
 __asm("	mov r7,r0");

}
