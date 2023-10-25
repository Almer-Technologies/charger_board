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
#include <charger.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


static charger_status_t status;
static charger_type_t type;



void  control_thread_entry(void) {

	charger_init();


	for(;;) {
		type = charger_get_type();
		os_delay(500);
		status = charger_get_status();
		os_delay(500);
	}
}


static uint8_t led_r, led_g, led_b;

typedef enum led_color {
	LED_BLACK,
	LED_RED,
	LED_GREEN,
	LED_BLUE,
	LED_YELLOW,
	LED_PURPLE,
	LED_CYAN,
	LED_WHITE
}led_color_t;

void led_init_rgb(void) {
	led_r = hal_led_attach((uint8_t*)GPIOB, GPIO_PIN0);
	led_b = hal_led_attach((uint8_t*)GPIOB, GPIO_PIN1);
	led_g = hal_led_attach((uint8_t*)GPIOB, GPIO_PIN2);
}

void led_set_color(led_color_t color) {
	switch (color){
	case LED_BLACK:
		hal_led_set_brightness(led_r, LED_OFF);
		hal_led_set_brightness(led_g, LED_OFF);
		hal_led_set_brightness(led_b, LED_OFF);
		break;
	case LED_RED:
		hal_led_set_brightness(led_r, LED_ON);
		hal_led_set_brightness(led_g, LED_OFF);
		hal_led_set_brightness(led_b, LED_OFF);
		break;
	case LED_GREEN:
		hal_led_set_brightness(led_r, LED_OFF);
		hal_led_set_brightness(led_g, LED_ON);
		hal_led_set_brightness(led_b, LED_OFF);
		break;
	case LED_BLUE:
		hal_led_set_brightness(led_r, LED_OFF);
		hal_led_set_brightness(led_g, LED_OFF);
		hal_led_set_brightness(led_b, LED_ON);
		break;
	case LED_YELLOW:
		hal_led_set_brightness(led_r, LED_ON);
		hal_led_set_brightness(led_g, LED_ON);
		hal_led_set_brightness(led_b, LED_OFF);
		break;
	case LED_PURPLE:
		hal_led_set_brightness(led_r, LED_ON);
		hal_led_set_brightness(led_g, LED_OFF);
		hal_led_set_brightness(led_b, LED_ON);
		break;
	case LED_CYAN:
		hal_led_set_brightness(led_r, LED_OFF);
		hal_led_set_brightness(led_g, LED_ON);
		hal_led_set_brightness(led_b, LED_ON);
		break;
	case LED_WHITE:
		hal_led_set_brightness(led_r, LED_ON);
		hal_led_set_brightness(led_g, LED_ON);
		hal_led_set_brightness(led_b, LED_ON);
		break;
	}
}

void  feedback_thread_entry(void) {

	hal_print("Charger Board\n");
	hal_print("Almer Technologies\n");
	hal_print("Iacopo Sprenger\n");
	hal_print("Jan Vrkoslav\n");

	

	/* setup feedback leds */
	led_init_rgb();

	os_delay(500);

	

	for(;;) {
		hal_gpio_clr(GPIOB, GPIO_PIN1);
		hal_print("Charger Type: ");
		os_delay(50);
		switch(type) {
		case CT_NONE:
			hal_print_it("None (500mA)\n");
			break;
		case CT_USB_SDP:
			hal_print_it("USB SDP (?)\n");
			break;
		case CT_USB_DCP_2A:
			hal_print_it("USB DCP (2A)\n");
			break;
		case CT_USB_CDP_1A5:
			hal_print_it("USB CDP (1.5A)\n");
			break;
		case CT_DIV1_1A:
			hal_print_it("Divider 1 (1A)\n");
			break;
		case CT_DIV2_2A1:
			hal_print_it("Divider 2 (2.1A)\n");
			break;
		case CT_DIV3_2A4:
			hal_print_it("Divider 3 (2.4A)\n");
			break;
		case CT_DIV4_2A:
			hal_print_it("Divider 4 (2A)\n");
			break;
		case CT_UNKNOWN:
			hal_print_it("Unknown (500mA)\n");
			break;
		case CT_HV_2A:
			hal_print_it("High Voltage (2A)\n");
			break;
		case CT_DIV5_3A:
			hal_print_it("Divider 5 (3A)\n");
			break;
		}

		hal_gpio_set(GPIOB, GPIO_PIN1);

		os_delay(100);
		hal_print_it("Charger Status: ");
		os_delay(50);
		switch(status) {
		case CS_NONE:
			hal_print_it("None\n");
			led_set_color(LED_OFF);
			break;
		case CS_TRICKLE:
			hal_print_it("Trickle charge\n");
			led_set_color(LED_RED);
			break;
		case CS_PRE:
			hal_print_it("Pre charge\n");
			led_set_color(LED_YELLOW);
			break;
		case CS_FAST:
			hal_print_it("Fast charge\n");
			led_set_color(LED_WHITE);
			break;
		case CS_CONST:
			hal_print_it("Constant charge\n");
			led_set_color(LED_CYAN);
			break;
		case CS_DONE:
			hal_print_it("Done!\n");
			led_set_color(LED_GREEN);
			break;
		}
		os_delay(800);
	}
}


int main(void) {

	/* hal led initialization */

	hal_systick_init();
	hal_uart_init();
	hal_i2c_init();
	hal_led_init();
	os_system_init();






	/* threads definitions */

	static uint8_t feedback_stack[256];
	static os_thread_t feedback_thread = {
		.name = "feedback"
	};

	static uint8_t control_stack[256];
	static os_thread_t control_thread = {
		.name = "control "
	};







	/* threads creation */
	os_thread_createI(&feedback_thread, 3, feedback_thread_entry, feedback_stack, sizeof(feedback_stack));
	os_thread_createI(&control_thread, 2, control_thread_entry, control_stack, sizeof(control_stack));




	//execution is handed over to the scheduler
	os_system_start();



	for(;;) {

	}

	os_system_panic("end");
}



/* END */
