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

#include <os.h>
#include <port.h>
#include <hal.h>

#include <avr/interrupt.h>

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

static volatile os_scheduler_t scheduler;

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

void os_thread_print(os_thread_t * thd);

os_thread_t * os_scheduler_get_ready(os_scheduler_t * sch);

void os_system_switch(os_thread_t * old, os_thread_t * new);

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
		os_thread_print(node);
	}	
}


void os_thread_print(os_thread_t * thd) {
	hal_print(thd->name);
	hal_uart_send_char(' ');
	hal_uart_send_char('0'+thd->priority);
	hal_uart_send_char(' ');
	switch(thd->state) {
		case OS_RUNNING:
			hal_uart_send_char('X');
			break;
		case OS_READY:
			hal_uart_send_char('R');
			break;
		case OS_SUSPENDED:
			hal_uart_send_char('S');
			break;
		case OS_WAITING:
			hal_uart_send_char('W');
			break;
		case OS_DISABLED:
			hal_uart_send_char('D');
			break;
	}
	hal_uart_send_char('\n');
	hal_uart_send_char('\r');
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
		//hal_print("idle\n\r");
		//hal_gpio_tgl(GPIOD, GPIO_PIN4);
		hal_sleep_enter();
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
#if DEBUG == VERBOSE
	hal_print("starting system\n\r");
#endif

	scheduler.running = os_scheduler_get_ready(&scheduler);


	scheduler.running->state = OS_RUNNING;
	scheduler.running->existing = 1;

	//hal_print("running thread: ");
	//os_thread_print(scheduler.running);
#if DEBUG == VERBOSE
	hal_print("first state:\n\r");
	os_thread_list();
#endif
	port_context_create(&(scheduler.running->context));
	port_context_return();
}

void os_system_panic(const uint8_t * msg) {
	cli();
	hal_uart_send("PANIC  ", 7);
	hal_uart_send(msg, sizeof(msg));
	hal_uart_send("\n\r", 2);
	while(1) {	

	}
}

//reschedule system assuming no threads are running
void os_system_reschedule(void) {
#if DEBUG == VERBOSE
	hal_print("reschedule requested\n\r");
	os_thread_list();
#endif
	scheduler.running = os_scheduler_get_ready(&scheduler);

	scheduler.running->state = OS_RUNNING;

#if DEBUG == VERBOSE
	hal_print("new states \n\r");
	os_thread_list();
#endif
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
				scheduler.running->state = OS_READY;
				os_system_reschedule();
			}
		}
	}
}

void os_delay(hal_systick_t delay) {
	port_context_save(&(scheduler.running->context));
	scheduler.running->state = OS_SUSPENDED;
	scheduler.running->suspended_timer = delay;
	os_system_reschedule();
#if DEBUG == VERBOSE
	hal_print("resched for delay: \n\r");
	os_thread_list();
#endif
	if(!(scheduler.running->existing)) {
		scheduler.running->existing = 1;
		port_context_create(&scheduler.running->context);
	} else {
		port_context_restore(&scheduler.running->context);
	}
	port_context_return();
}

void os_delay_windowed(hal_systick_t * last_wake, hal_systick_t delay) {
	hal_systick_t time = hal_systick_get();
	if(delay > (time - *last_wake)) {
		os_delay(delay - (time - *last_wake));
	} else {
		os_delay(1);
	}
	*last_wake = hal_systick_get();
}

ISR(TIMER0_COMPA_vect, ISR_NAKED) {
	port_context_save(&(scheduler.running->context));
	hal_sleep_disable();
	hal_systick_inc();
	os_delay_compute();

	if(!(scheduler.running->existing)) {
		scheduler.running->existing = 1;
		port_context_create(&scheduler.running->context);
	} else {
		port_context_restore(&scheduler.running->context);
	}
	port_context_return();
}



/* os_event */



void os_event_create(os_event_t * event, os_event_state_t state) {
	event->state = state;
	event->owner = NULL;
}

// puts the thread into waiting for event state
void os_event_wait(os_event_t * event) {
	port_context_save(&(scheduler.running->context));

	scheduler.running->state = OS_WAITING;
	scheduler.running->waiting_event = event;

	os_system_reschedule();

	if(!(scheduler.running->existing)) {
		scheduler.running->existing = 1;
		port_context_create(&scheduler.running->context);
	} else {
		port_context_restore(&scheduler.running->context);
	}
	port_context_return();
}

// puts all the waiting threads in ready state
void os_event_signal(os_event_t * event) {
	port_context_save(&(scheduler.running->context));

	scheduler.running->state = OS_READY;
	os_thread_t * node;
	for( node = scheduler.head; node != NULL; node = node->next) {
		if(node->state == OS_WAITING && node->waiting_event == event) {
			node->state = OS_READY;
		}
	}

	os_system_reschedule();

	if(!(scheduler.running->existing)) {
		scheduler.running->existing = 1;
		port_context_create(&scheduler.running->context);
	} else {
		port_context_restore(&scheduler.running->context);
	}
	port_context_return();
}

// if the event is free, set it to taken
// if the event is taken, wait for it to be free
void os_event_take(os_event_t * event) {


	if(event->state == OS_TAKEN) {
		os_event_wait(event);
	} else {
		event->state = OS_TAKEN;
		event->owner = scheduler.running;
	}

}

// set the event to free if owned, otherwise do nothing
// put all the waiting threads to ready state
void os_event_release(os_event_t * event) {
	if(event->owner == scheduler.running) {
		event->state = OS_FREE;
		os_event_signal(event);
	}	
}



/* END */
