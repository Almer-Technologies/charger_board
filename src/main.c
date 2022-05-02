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

	static os_thread_t thread_a, thread_b;
	static uint8_t stack_a[512];
	static uint8_t stack_b[512];

	//os_thread_createI(&thread_a, 1, thread_a_entry, stack_a, 1024);	
	//os_thread_createI(&thread_b, 2, thread_b_entry, stack_b, 1024);
	
	hal_systick_init();
	hal_uart_init();

	hal_gpio_init_out(GPIOD, PIN2|PIN3);
	hal_gpio_init_out(GPIOB, PIN5);

	hal_gpio_clr(GPIOB, PIN5);

	sei();
	//os_system_init();

	//infinite loop lock
	for(;;) {

		static const uint8_t  message[] = "hello\n\r"; 
		static const uint16_t message_len = 7; 
		uint8_t data = (uint8_t) (hal_systick_get()>>8)+'0';
		hal_gpio_set(GPIOD, PIN2);
		hal_uart_send(message, message_len);
		hal_uart_send_char(data);
		hal_gpio_clr(GPIOD, PIN2);

		hal_gpio_set(GPIOD, PIN3);
		hal_delay(100);
		hal_gpio_clr(GPIOD, PIN3);

	}
}



/* END */
