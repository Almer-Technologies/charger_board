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

#include <atmega328p.h>
#include <os.h>
#include <hal.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>





void  feedback_thread_entry(void) {


	for(;;) {

		hal_gpio_clr(GPIOB, GPIO_PIN1);

		os_delay(500);

		hal_gpio_set(GPIOB, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2);

		os_delay(500);

		hal_gpio_clr(GPIOB, GPIO_PIN0);

		os_delay(500);

		hal_gpio_set(GPIOB, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2);

		os_delay(500);

		hal_gpio_clr(GPIOB, GPIO_PIN2);

		os_delay(500);

		hal_gpio_set(GPIOB, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2);

		os_delay(500);

		hal_gpio_clr(GPIOB, GPIO_PIN1|GPIO_PIN2);

		os_delay(500);

		hal_gpio_set(GPIOB, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2);

		os_delay(500);

		hal_gpio_clr(GPIOB, GPIO_PIN0|GPIO_PIN1);

		os_delay(500);

		hal_gpio_set(GPIOB, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2);

		os_delay(500);

		hal_gpio_clr(GPIOB, GPIO_PIN0|GPIO_PIN1);

		os_delay(500);

		hal_gpio_set(GPIOB, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2);

		os_delay(500);

		hal_gpio_clr(GPIOB, GPIO_PIN0|GPIO_PIN2);

		os_delay(500);

		hal_gpio_set(GPIOB, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2);

		os_delay(500);

		hal_gpio_clr(GPIOB, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2);

		os_delay(500);

		hal_gpio_set(GPIOB, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2);

		os_delay(500);
	}

}


#define CHARGER_ADDRESS		0x3F


void  control_thread_entry(void) {

	/* Disable OVP on input */

	hal


	for(;;) {

		os_delay(1000);
		
	}
}

void  logger_thread_entry(void) {
	for(;;) {
		os_delay(1000);
	}
}


int main(void) {

	/* hal led initialization */

	hal_systick_init();
	hal_i2c_init();
	os_system_init();




	/* setup feedback leds */
	hal_gpio_init_out(GPIOB, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2);

	hal_gpio_set(GPIOB, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2);

	/* threads definitions */

	static uint8_t feedback_stack[256];
	static os_thread_t feedback_thread = {
		.name = "feedback"
	};

	static uint8_t control_stack[200];
	static os_thread_t control_thread = {
		.name = "control "
	};

	static uint8_t logger_stack[256];
	static os_thread_t logger_thread = {
		.name = "logger  "
	};






	/* threads creation */
	os_thread_createI(&feedback_thread, 5, feedback_thread_entry, feedback_stack, sizeof(feedback_stack));
	os_thread_createI(&control_thread, 3, control_thread_entry, control_stack, sizeof(control_stack));
	os_thread_createI(&logger_thread, 1, logger_thread_entry, logger_stack, sizeof(logger_stack));




	//execution is handed over to the scheduler
	os_system_start();



	for(;;) {

	}

	os_system_panic("end");
}



/* END */
