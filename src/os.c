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
#include "hal.h"

/**********************
 *	CONSTANTS
 **********************/

#define IDLE_STACK_SIZE	64


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

static os_thread_t idle_thread = {
	.name="idle_thd"
};

static uint8_t idle_stack[IDLE_STACK_SIZE];


/**********************
 *	PROTOTYPES
 **********************/

void os_scheduler_add_thread(	os_scheduler_t * sch,
				os_thread_t * thd);

void os_scheduler_add_threadI(	os_scheduler_t * sch,
				os_thread_t * thd);

os_thread_t * os_scheduler_get_ready(os_scheduler_t * sch);

void os_system_switch(os_thread_t * old, os_thread_t * new);

void os_system_panic(const uint8_t * msg);

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
	thd->existing = 0;
	thd->next = NULL;
	port_context_init(&(thd->context), entry, stack, stack_size);
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
	thd->existing = 0;
	thd->next = NULL;
	port_context_init(&(thd->context), entry, stack, stack_size);
	os_scheduler_add_threadI(&scheduler, thd);

	thd->state = OS_READY; //start thread
}


void os_thread_list(void) {
	hal_print("threads list\n\r");
	os_thread_t * node;
	for( node = scheduler.head; node != NULL; node = node->next) {
		hal_print(node->name);
		hal_print(" ");
		hal_uart_send_char('0'+node->priority);
		hal_print("\r\n");
	}	
}



/* os_scheduler */

/**
 *  Add thread in the sorted linked list of threads
 **/ 
void os_scheduler_add_thread(	os_scheduler_t * sch,
				os_thread_t * thd) {
	cli();
	os_thread_t ** node;
	for( node = &(sch->head); (*node) != NULL; node = &((*node)->next)) {
		if((*node)->priority < thd->priority) {
			thd->next = (*node);
			(*node) = thd;
			return;
		}
	} 
	//case where the thread is last
	os_system_panic("no idle thread");
	sei();
}

void os_scheduler_add_threadI(	os_scheduler_t * sch,
				os_thread_t * thd) {
	os_thread_t ** node;
	for( node = &(sch->head); (*node) != NULL; node = &((*node)->next)) {
		if((*node)->priority < thd->priority) {
			thd->next = (*node);
			(*node) = thd;
			return;
		}
	} 
	//case where the thread is last
	os_system_panic("no idle thread");
}

//returns thread with highest priority that is ready.
os_thread_t * os_scheduler_get_ready(os_scheduler_t * sch) {
	os_thread_t * node;
	for( node = sch->head; node != NULL; node = node->next) {
		if(node->state == OS_READY) {
			return node;
		}
	}
	//this should return the idle thread
	os_system_panic("idle thread not ready!");
}






/* os system */

void os_system_idle(void) {
	for(;;) {
		//should enter idle
		hal_print("idle\n\r");
	}
}

void os_system_init(void) {

	//setup idle thread

	idle_thread.priority = 0;
	idle_thread.next = NULL;
	idle_thread.state = OS_READY;
	idle_thread.existing = 0;

	port_context_init(&(idle_thread.context), os_system_idle, idle_stack, IDLE_STACK_SIZE);



	//setup scheduler

	scheduler.head = &idle_thread;
	scheduler.running = &idle_thread;

}

//give control to scheduler
void os_system_start(void) {


	

	scheduler.running = os_scheduler_get_ready(&scheduler);

	scheduler.running->state = OS_RUNNING;
	scheduler.running->existing = 1;
	
	port_context_create(&(scheduler.running->context));
	port_context_return();

	os_system_panic("outside");
}

void os_system_panic(const uint8_t * msg) {
	cli();
	while(1) {
		hal_uart_send("PANIC  ", 7);
		hal_uart_send(msg, sizeof(msg));
		hal_uart_send("\n\r", 2);

	}
}

//reschedule system
void os_system_reschedule(void) {

	os_thread_t * old = scheduler.running;
	//run scheduler
	os_thread_t * new = os_scheduler_get_ready(&scheduler);

	if(new == scheduler.running) {
		return;
	} else {
		os_system_switch(old, new);
	}
}

//switch context
void os_system_switch(os_thread_t * old, os_thread_t * new) {
	port_context_save(&old->context);
	if(!(new->existing)) {
		new->existing = 1;
		port_context_create(&new->context);
	} else {
		port_context_restore(&new->context);
	}
	port_context_return();
}


/* os_delay */

//compute new delay time and set task to ready if timeout
void os_delay_compute(void){
	os_thread_t * node;
	for( node = scheduler.head; node != NULL; node = node->next) {
		if(node->state == OS_SUSPENDED) {
			node->suspended_timer--; //remove 1ms from suspended timer
			if(node->suspended_timer == 0) {
				node->state = OS_READY;
			}
		}
	}
}

void os_delay(hal_systick_t delay) {
	scheduler.running->state = OS_SUSPENDED;
	scheduler.running->suspended_timer = delay;
	os_system_reschedule();
}

void os_delay_windowed(hal_systick_t * last_wake, hal_systick_t delay) {
	hal_systick_t time = hal_systick_getI();
	scheduler.running->state = OS_SUSPENDED;
	scheduler.running->suspended_timer = *last_wake - time + delay;
	*last_wake = time;
	os_system_reschedule();
}



/* os_event */

void os_event_take(os_event_t event) {
	return;
}

void os_event_give(os_event_t event) {
	return;
}


/* END */