/*  Title       : os
 *  Filename    : os.h
 *  Author      : iacopo sprenger
 *  Date        : 29.04.2022
 *  Version     : 0.1
 *  Description : operating system definitions
 */

#ifndef OS_H
#define OS_H



/**********************
 *  INCLUDES
 **********************/

#include <stdint.h>

#include "port.h"

/**********************
 *  CONSTANTS
 **********************/


/**********************
 *  MACROS
 **********************/


/**********************
 *  TYPEDEFS
 **********************/

typedef enum os_thread_state{
    OS_RUNNING,
    OS_READY,
    OS_SUSPENDED,
    OS_DISABLED
}os_thread_state_t;


/**
 * higher threads have higher priority
 **/
typedef uint8_t os_priority_t;

typedef struct os_thread os_thread_t;

struct os_thread {
    os_thread_t * next;
    os_priority_t priority;
    port_context_t context;
    os_thread_state_t state;
};

/**********************
 *  VARIABLES
 **********************/


/**********************
 *  PROTOTYPES
 **********************/

void os_system_init(void);



/* os_thread */
void os_thread_create(	os_thread_t * thd,
			os_priority_t prio, 
			void (*entry)(void), 
			uint8_t * stack, 
			uint16_t stack_size);

void os_thread_createI( os_thread_t * thd,
			os_priority_t prio, 
			void (*entry)(void), 
			uint8_t * stack, 
			uint16_t stack_size);


#endif /* OS_H */

/* END */