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

#define nop() \
    asm volatile ("nop"::)






/**********************
 *  TYPEDEFS
 **********************/

typedef struct port_context {
    uint8_t spl;
    uint8_t sph;
    void * entry;
    uint8_t * stack_top;                 
}port_context_t;


/**********************
 *  VARIABLES
 **********************/


/**********************
 *  PROTOTYPES
 **********************/

/**
 * everything is stored on the stack
 * except the stack pointer
 */

static inline void port_context_store(port_context_t* ctx) {
     // Save context
     asm volatile ( "push   r0             \n\t"    // r0
		    "in     r0, __SREG__   \n\t"
		    "push   r0             \n\t"    // SREG
		    "push   r1             \n\t"    // r1
		    "clr    r1             \n\t"    // GCC expects r1 to be null
		    "push   r2             \n\t"    // r2
		    "push   r3             \n\t"    // r3
		    "push   r4             \n\t"    // r4
		    "push   r5             \n\t"    // r5
		    "push   r6             \n\t"    // r6
		    "push   r7             \n\t"    // r7
		    "push   r8             \n\t"    // r8
		    "push   r9             \n\t"    // r9
		    "push   r10            \n\t"    // r10
		    "push   r11            \n\t"    // r11
		    "push   r12            \n\t"    // r12
		    "push   r13            \n\t"    // r13
		    "push   r14            \n\t"    // r14
		    "push   r15            \n\t"    // r15
		    "push   r16            \n\t"    // r16
		    "push   r17            \n\t"    // r17
		    "push   r18            \n\t"    // r18
		    "push   r19            \n\t"    // r19
		    "push   r20            \n\t"    // r20
		    "push   r21            \n\t"    // r21
		    "push   r22            \n\t"    // r22
		    "push   r23            \n\t"    // r23
		    "push   r24            \n\t"    // r24
		    "push   r25            \n\t"    // r25
		    "push   r26            \n\t"    // r26
		    "push   r27            \n\t"    // r27
		    "push   r28            \n\t"    // r28
		    "push   r29            \n\t"    // r29
		    "push   r30            \n\t"    // r30
		    "push   r31            \n\t"    // r31
		    "in     %0, __SP_L__   \n\t"    // Save current Stack Pointer
		    "in     %1, __SP_H__   \n\t"
		    :"=r"(ctx->spl), "=r"(ctx->sph));


}

static inline void port_context_restore(port_context_t* ctx) {
    //restore stack
    asm volatile (  "out    __SP_L__, %0   \n\t"
		    "out    __SP_H__, %1   \n\t"
		    "pop    r31            \n\t"    // r31
		    "pop    r30            \n\t"    // r30
		    "pop    r29            \n\t"    // r29
		    "pop    r28            \n\t"    // r28
		    "pop    r27            \n\t"    // r27
		    "pop    r26            \n\t"    // r26
		    "pop    r25            \n\t"    // r25
		    "pop    r24            \n\t"    // r24
		    "pop    r23            \n\t"    // r23
		    "pop    r22            \n\t"    // r22
		    "pop    r21            \n\t"    // r21
		    "pop    r20            \n\t"    // r20
		    "pop    r19            \n\t"    // r19
		    "pop    r18            \n\t"    // r18
		    "pop    r17            \n\t"    // r17
		    "pop    r16            \n\t"    // r16
		    "pop    r15            \n\t"    // r15
		    "pop    r14            \n\t"    // r14
		    "pop    r13            \n\t"    // r13
		    "pop    r12            \n\t"    // r12
		    "pop    r11            \n\t"    // r11
		    "pop    r10            \n\t"    // r10
		    "pop    r9             \n\t"    // r9
		    "pop    r8             \n\t"    // r8
		    "pop    r7             \n\t"    // r7
		    "pop    r6             \n\t"    // r6
		    "pop    r5             \n\t"    // r5
		    "pop    r4             \n\t"    // r4
		    "pop    r3             \n\t"    // r3
		    "pop    r2             \n\t"    // r2
		    "pop    r1             \n\t"    // r1
		    "pop    r0             \n\t"    // SREG
		    "out    __SREG__, r0   \n\t"
		    "pop    r0             \n\t"    // r0
		    ::"r"(ctx->spl), "r"(ctx->sph));
}

static inline void port_context_create(port_context_t* ctx) {

    //create context
    volatile uint8_t pcl = (uint8_t) ctx->entry;
    volatile uint8_t pch = (uint8_t) ctx->entry<<8;

    ctx->spl = (uint8_t) ctx->stack_top;
    ctx->sph = (uint8_t) ctx->stack_top<<8;



    asm volatile (  "out    __SP_L__, %0   \n\t"
		    "out    __SP_H__, %1   \n\t"
		    "push   %2             \n\t"    // push PC_L first
		    "push   %3             \n\t"    // push PC_H then
		    "clr    r0             \n\t"
		    "push   r0             \n\t"    // r0
		    "in     r0, __SREG__   \n\t"
		    "push   r0             \n\t"    // SREG
		    "clr    r0             \n\t"
		    "clr    r1             \n\t"    // GCC expects r1 to be null
		    "push   r0             \n\t"    // r1
		    "push   r0             \n\t"    // r2
		    "push   r0             \n\t"    // r3
		    "push   r0             \n\t"    // r4
		    "push   r0             \n\t"    // r5
		    "push   r0             \n\t"    // r6
		    "push   r0             \n\t"    // r7
		    "push   r0             \n\t"    // r8
		    "push   r0             \n\t"    // r9
		    "push   r0             \n\t"    // r10
		    "push   r0             \n\t"    // r11
		    "push   r0             \n\t"    // r12
		    "push   r0             \n\t"    // r13
		    "push   r0             \n\t"    // r14
		    "push   r0             \n\t"    // r15
		    "push   r0             \n\t"    // r16
		    "push   r0             \n\t"    // r17
		    "push   r0             \n\t"    // r18
		    "push   r0             \n\t"    // r19
		    "push   r0             \n\t"    // r20
		    "push   r0             \n\t"    // r21
		    "push   r0             \n\t"    // r22
		    "push   r0             \n\t"    // r23
		    "push   r0             \n\t"    // r24
		    "push   r0             \n\t"    // r25
		    "push   r0             \n\t"    // r26
		    "push   r0             \n\t"    // r27
		    "push   r0             \n\t"    // r28
		    "push   r0             \n\t"    // r29
		    "push   r0             \n\t"    // r30
		    "push   r0             \n\t"    // r31
		    ::"r"(ctx->spl), "r"(ctx->sph), "r"(pcl), "r"(pch));
}



static inline void port_context_init(port_context_t* ctx, void (*entry)(void), uint8_t * stack, uint16_t stack_size) {
	ctx->stack_top = &stack[stack_size-1]; //get last element of stack
	ctx->entry = (void*) entry;
}


#endif /* PORT_H */

/* END */