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
#include "port.h"

/**********************
 *	CONSTANTS
 **********************/


/**********************
 *	MACROS
 **********************/

#define for_each_thread(head, node) \
	for(os_thread_t * node = head; node->next != 0; node = node->next)


/**********************
 *	TYPEDEFS
 **********************/

typedef struct os_scheduler {
    os_thread_t * head;
    os_thread_t * running;
}os_scheduler_t;




/**********************
 *	VARIABLES
 **********************/

static os_scheduler_t scheduler;

/**********************
 *	PROTOTYPES
 **********************/

void os_scheduler_add_thread(	os_scheduler_t * sch,
				os_thread_t * thd);

void os_scheduler_add_threadI(	os_scheduler_t * sch,
				os_thread_t * thd);

/**********************
 *	DECLARATIONS
 **********************/

/* os_thread */

void os_thread_create(	os_thread_t * thd,
			os_priority_t prio, 
			void (*entry)(void), 
			uint8_t * stack, 
			uint16_t stack_size) {
	cli();

	thd->priority = prio;
	thd->state = OS_DISABLED;
	thd->next = 0;
	port_context_init(&thd->context, entry, stack, stack_size);
	os_scheduler_add_thread(&scheduler, thd);

	thd->state = OS_READY; //start thread


	sei();
}

void os_thread_createI(	os_thread_t * thd,
			os_priority_t prio, 
			void (*entry)(void), 
			uint8_t * stack, 
			uint16_t stack_size) {

	thd->priority = prio;
	thd->state = OS_DISABLED;
	thd->next = 0;
	port_context_init(&thd->context, entry, stack, stack_size);
	os_scheduler_add_threadI(&scheduler, thd);

	thd->state = OS_READY; //start thread
}


/* os_scheduler */

/**
 *  Add thread in the sorted linked list of threads
 **/ 
void os_scheduler_add_thread(	os_scheduler_t * sch,
				os_thread_t * thd) {
	cli();
	os_thread_t * node;
	for( node = sch->head; node->next != 0; node = node->next) {
		if(node->priority < thd->priority) {
			thd->next = node;
			node = thd;
			sei();
			return;
		}
	} 
	//case where the thread is last
	node->next = thd;
	sei();
}

void os_scheduler_add_threadI(	os_scheduler_t * sch,
				os_thread_t * thd) {
	os_thread_t * node;
	for( node = sch->head; node->next != 0; node = node->next) {
		if(node->priority < thd->priority) {
			thd->next = node;
			node = thd;
			return;
		}
	} 
	//case where the thread is last
	node->next = thd;
}

os_thread_t * os_scheduler_runI(os_scheduler_t * sch) {
	os_thread_t * node;
	for( node = sch->head; (node->next != 0) || (node->priority > sch->running->priority); node = node->next) {
		if(node->state == OS_READY) {
			sch->running->state = OS_READY;
			node->state = OS_RUNNING;
			sch->running = node;
			return sch->running;
		}
	}
}

//init system
void os_system_init(void) {

	//setup scheduler

	//setup idle task


}

//give control to scheduler
void os_system_start(void) {

	//start scheduler

	//enable interrupts


}

/* END */