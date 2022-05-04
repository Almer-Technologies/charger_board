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
	for(;;) {
		hal_print("thread a\n\r");
	}
}

void thread_b_entry(void) {
	for(;;) {
		hal_print("thread b\n\r");
	}
}

void cb(void) {
	static const uint8_t  done[] = "done\n\r"; 
	static const uint16_t done_len = sizeof(done);
	hal_uart_send_it(done, done_len, NULL);
}


int main(void) {


	
	hal_systick_init();
	hal_uart_init();

	hal_print("hal initialized!\n\r")

	os_system_init();

	static os_thread_t thread_a = {
		.name = "thread_a"
	};
	static os_thread_t thread_b = {
		.name = "thread_b"
	};
	static uint8_t stack_a[256];
	static uint8_t stack_b[256];



	os_thread_createI(&thread_a, 3, thread_a_entry, stack_a, 256);	
	os_thread_createI(&thread_b, 4, thread_b_entry, stack_b, 256);

	os_thread_list();

	hal_gpio_init_out(GPIOD, PIN2|PIN3);
	hal_gpio_init_out(GPIOB, PIN5);

	hal_gpio_clr(GPIOB, PIN5);

	os_system_start();



	//infinite loop lock
	for(;;) {

		static const uint8_t  message[] = "hello\n\r"; 
		static const uint16_t message_len = sizeof(message);
		uint8_t msg[7];
		hal_uart_recv_it(msg, 7, cb);
		hal_gpio_set(GPIOD, PIN2);
		//hal_uart_send(message, message_len);
		//hal_uart_send(msg, 7);
		hal_gpio_clr(GPIOD, PIN2);

		hal_delay(500);

	}
}



/* END */
