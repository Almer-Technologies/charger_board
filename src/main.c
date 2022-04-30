/*  Title		: init
 *  Filename	: init.c
 *	Author		: iacopo sprenger
 *	Date		: 29.04.2022
 *	Version		: 0.1
 *	Description	: processor initialization called from asembly setup
 */

/**********************
 *	INCLUDES
 **********************/

#include "atmega328p.h"
#include "os.h"
#include "hal.h"

/**********************
 *	CONSTANTS
 **********************/


/**********************
 *	MACROS
 **********************/


/**********************
 *	TYPEDEFS
 **********************/


/**********************
 *	VARIABLES
 **********************/




/**********************
 *	PROTOTYPES
 **********************/


/**********************
 *	DECLARATIONS
 **********************/



void thread_a_entry(void) {

}

void thread_b_entry(void) {

}

int main(void) {

	DDRB = 32;
	PORTB = 32;

	static os_thread_t thread_a, thread_b;
	static uint8_t stack_a[1024];
	static uint8_t stack_b[1024];

	os_thread_createI(&thread_a, 1, thread_a_entry, stack_a, 1024);	
	os_thread_createI(&thread_b, 2, thread_b_entry, stack_b, 1024);
	
	hal_systick_init();

	sei();
	//os_system_init();

	//infinite loop lock
	for(;;) {
	}
}



/* END */

