/*  Title		: system
 *  Filename	: os_sys.c
 *	Author		: iacopo sprenger
 *	Date		: 29.04.2022
 *	Version		: 0.1
 *	Description	: os system setup
 */

/**********************
 *	INCLUDES
 **********************/

#include "os.h"

/**********************
 *	CONSTANTS
 **********************/


/**********************
 *	MACROS
 **********************/

typedef enum os_thread_state{
    OS_RUNNING,
    OS_READY,
    OS_SUSPENDED
}os_thread_state_t;


/**********************
 *	TYPEDEFS
 **********************/

typedef struct os_scheduler {

}os_scheduler_t;

struct os_thread {
    os_thread_t * next;
    os_priority_t priority;
    port_context_t context;
    os_thread_state_t state;
};


/**********************
 *	VARIABLES
 **********************/


/**********************
 *	PROTOTYPES
 **********************/


/**********************
 *	DECLARATIONS
 **********************/
void os_create_thread() {

}


//start system
void os_system_init(void) {


}

/* END */