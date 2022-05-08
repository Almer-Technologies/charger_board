/*  Title       : port
 *  Filename    : port.h
 *  Author      : iacopo sprenger
 *  Date        : 29.04.2022
 *  Version     : 0.1
 *  Description : operating system port for atmega328p
 */

#ifndef PORT_H
#define PORT_H




/**********************
 *  INCLUDES
 **********************/

#include <stdint.h>

#include "atmega328p.h"

/**********************
 *  CONSTANTS
 **********************/


/**********************
 *  MACROS
 **********************/

#define cli() \
    asm volatile ("cli"::)


#define sei() \
    asm volatile ("sei"::)

#define reti() \
    asm volatile ("reti"::)

#define ret() \
    asm volatile ("ret"::)

#define nop() \
    asm volatile ("nop"::)





#define port_context_save(ctx) 		\
    asm volatile (			\
    	"push	r0 		\n\t"	\
    	"in	r0, __SREG__ 	\n\t"	\
    	"cli			\n\t"	\
    	"push	r0 		\n\t"	\
    	"push	r1 		\n\t"	\
    	"clr	r1 		\n\t"	\
    	"push	r2 		\n\t"	\
    	"push	r3 		\n\t"	\
    	"push	r4 		\n\t"	\
    	"push	r5 		\n\t"	\
    	"push	r6 		\n\t"	\
    	"push	r7 		\n\t"	\
    	"push	r8 		\n\t"	\
    	"push	r9 		\n\t"	\
    	"push	r10 		\n\t"	\
    	"push	r11 		\n\t"	\
    	"push	r12		\n\t"	\
    	"push	r13		\n\t"	\
    	"push	r14		\n\t"	\
    	"push	r15 		\n\t"	\
    	"push	r16		\n\t"	\
    	"push	r17 		\n\t"	\
    	"push	r18		\n\t"	\
    	"push	r19		\n\t"	\
    	"push	r20		\n\t"	\
    	"push	r21 		\n\t"	\
    	"push	r22		\n\t"	\
    	"push	r23 		\n\t"	\
    	"push	r24 		\n\t"	\
    	"push	r25		\n\t"	\
    	"push	r26		\n\t"	\
    	"push	r27		\n\t"	\
    	"push	r28		\n\t"	\
    	"push	r29 		\n\t"	\
    	"push	r30 		\n\t"	\
    	"push	r31		\n\t");	\
    asm volatile (			\
    	"in	%0, __SP_L__	\n\t"	\
    	"in	%1, __SP_H__	\n\t"	\
    	:"=r"((ctx)->spl), "=r"((ctx)->sph))


#define port_context_restore(ctx)	\
    asm volatile (  			\
    	"out    __SP_L__, %0   \n\t"	\
	"out    __SP_H__, %1   \n\t"	\
	::"r"((ctx)->spl), "r"((ctx)->sph))


#define port_context_create(ctx)			\
    asm volatile (  					\
    	"out    __SP_L__, %0   \n\t"/* set spl		*/\
	"out    __SP_H__, %1   \n\t"/* set sph		*/\
	"push   %2             \n\t"/* set pcl		*/\
	"push   %3             \n\t"/* set pch		*/\
	"clr    r0             \n\t"/* 			*/\
	"push   r0             \n\t"/*	r0		*/\
	"in     r0, __SREG__   \n\t"/*			*/\
	"push   r0             \n\t"/*	sreg		*/\
	"clr    r0             \n\t"/*			*/\
	"clr    r1             \n\t"/*			*/\
	"push   r0             \n\t"/*	r1		*/\
	"push   r0             \n\t"/*	r2		*/\
	"push   r0             \n\t"/*	r3		*/\
	"push   r0             \n\t"/*	r4		*/\
	"push   r0             \n\t"/*	r5		*/\
	"push   r0             \n\t"/*	r6		*/\
	"push   r0             \n\t"/*	r7		*/\
	"push   r0             \n\t"/*	r8		*/\
	"push   r0             \n\t"/*	r9		*/\
	"push   r0             \n\t"/*	r10		*/\
	"push   r0             \n\t"/*	r11		*/\
	"push   r0             \n\t"/*	r12		*/\
	"push   r0             \n\t"/*	r13		*/\
	"push   r0             \n\t"/*	r14		*/\
	"push   r0             \n\t"/*	r15		*/\
	"push   r0             \n\t"/*	r16		*/\
	"push   r0             \n\t"/*	r17		*/\
	"push   r0             \n\t"/*	r18		*/\
	"push   r0             \n\t"/*	r19		*/\
	"push   r0             \n\t"/*	r20		*/\
	"push   r0             \n\t"/*	r21		*/\
	"push   r0             \n\t"/*	r22		*/\
	"push   r0             \n\t"/*	r23		*/\
	"push   r0             \n\t"/*	r24		*/\
	"push   r0             \n\t"/*	r25		*/\
	"push   r0             \n\t"/*	r26		*/\
	"push   r0             \n\t"/*	r26		*/\
	"push   r0             \n\t"/*	r27		*/\
	"push   r0             \n\t"/*	r29		*/\
	"push   r0             \n\t"/*	r30		*/\
	"push   r0             \n\t"/*	r31		*/\
	::"r"((ctx)->spl), "r"((ctx)->sph), "r"((ctx)->pcl), "r"((ctx)->pch))

#define port_context_return()		\
    asm volatile (			\
	"pop    r31            \n\t"    \
	"pop    r30            \n\t"    \
	"pop    r29            \n\t"    \
	"pop    r28            \n\t"    \
	"pop    r27            \n\t"    \
	"pop    r26            \n\t"    \
	"pop    r25            \n\t"    \
	"pop    r24            \n\t"    \
	"pop    r23            \n\t"    \
	"pop    r22            \n\t"    \
	"pop    r21            \n\t"    \
	"pop    r20            \n\t"    \
	"pop    r19            \n\t"    \
	"pop    r18            \n\t"    \
	"pop    r17            \n\t"    \
	"pop    r16            \n\t"    \
	"pop    r15            \n\t"    \
	"pop    r14            \n\t"    \
	"pop    r13            \n\t"    \
	"pop    r12            \n\t"    \
	"pop    r11            \n\t"    \
	"pop    r10            \n\t"    \
	"pop    r9             \n\t"    \
	"pop    r8             \n\t"    \
	"pop    r7             \n\t"    \
	"pop    r6             \n\t"    \
	"pop    r5             \n\t"    \
	"pop    r4             \n\t"    \
	"pop    r3             \n\t"    \
	"pop    r2             \n\t"    \
	"pop    r1             \n\t"    \
	"pop    r0             \n\t"    \
	"out    __SREG__, r0   \n\t"	\
	"pop    r0             \n\t"	\
	"reti		       \n\t"	\
	::)



/**********************
 *  TYPEDEFS
 **********************/

typedef struct port_context {
    uint8_t spl;
    uint8_t sph;
    void * entry;
    uint8_t * stack_top;     
    uint8_t pcl;
    uint8_t pch;           
}port_context_t;


/**********************
 *  VARIABLES
 **********************/


/**********************
 *  PROTOTYPES
 **********************/

static inline void port_context_init(port_context_t* ctx, void (*entry)(void), uint8_t * stack, uint16_t stack_size) {
	ctx->stack_top = &(stack[stack_size-1]); //get last element of stack
	ctx->entry = (void*) entry;
	ctx->pcl = (uint8_t) ((uint16_t) entry);
	ctx->pch = (uint8_t) ((uint16_t) entry >> 8);
	ctx->spl = (uint8_t) ((uint16_t) ctx->stack_top);
	ctx->sph = (uint8_t) ((uint16_t) ctx->stack_top >> 8);
}


#endif /* PORT_H */

/* END */