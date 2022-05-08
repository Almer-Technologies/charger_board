/*  Title		: main
 *  Filename	: main.c
 *	Author		: iacopo sprenger
 *	Date		: 29.04.2022
 *	Version		: 0.1
 *	Description	: main program
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
	static hal_systick_t last_wake;
	last_wake = hal_systick_get();
	for(;;) {
		
		cli();
		hal_gpio_tgl(GPIOD, GPIO_PIN2);
		sei();

		hal_delay(200);
		//hal_print("thread a\n\r");
		//hal_systick_t time = hal_systick_get();
		os_delay_windowed(&last_wake, 1000);
	}
}

void thread_b_entry(void) {
	for(;;) {
		cli();
		hal_gpio_tgl(GPIOD, GPIO_PIN3);
		sei();
		//hal_print("thread b\n\r");
		os_delay(1500);
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

	//hal_print("hal initialized!\n\r")

	os_system_init();

	static os_thread_t thread_a = {
		.name = "thread_a"
	};
	static os_thread_t thread_b = {
		.name = "thread_b"
	};
	static uint8_t stack_a[256];
	static uint8_t stack_b[256];



	os_thread_createI(&thread_a, 2, thread_a_entry, stack_a, 256);	
	os_thread_createI(&thread_b, 1, thread_b_entry, stack_b, 256);


	hal_gpio_init_out(GPIOD, GPIO_PIN2|GPIO_PIN3|GPIO_PIN4);
	hal_gpio_init_out(GPIOB, GPIO_PIN5);

	hal_gpio_clr(GPIOB, GPIO_PIN5);

	os_system_start();


	for(;;) {

	}

	os_system_panic("end");
}



/* END */
