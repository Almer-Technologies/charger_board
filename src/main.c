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

/**********************
 *	CONSTANTS
 **********************/

#define X 0
#define Y 1
#define Z 2

/**********************
 *	MACROS
 **********************/


/**********************
 *	TYPEDEFS
 **********************/

typedef enum rocket_state {
	ROCKET_IDLE,
	ROCKET_PRESSURIZED,
	ROCKET_POWERED,
	ROCKET_COAST,
	ROCKET_APOGEE,
	ROCKET_TOUCHDOWN
}rocket_state_t;


typedef struct rocket_params {
	uint16_t acc[3];
	uint16_t gyro[3];
	uint16_t baro;
	rocket_state_t state;

}rocket_params_t;


/**********************
 *	VARIABLES
 **********************/

static os_event_t event_a;


static rocket_params_t sp;

/**********************
 *	PROTOTYPES
 **********************/


/**********************
 *	DECLARATIONS
 **********************/

/**
 * Sensor acquisition thread
 * Uses i2c to communicate with sensors
 * about 50Hz -> record max acc, vel and alt
 */
void sensor_thread(void) {
	/* thread setup */


	/* thread mainloop */
	for(;;) {

	}
}

/**
 * Radio feedback thread
 * Uses SPI to communicate with the radio
 * as fast as possible send last data
 */
void radio_thread(void) {
	/* thread setup */


	/* thread mainloop */
	for(;;) {

	}
}

/**
 * Global control thread
 * Determines rocket state and triggers flight events
 * use sensor data to estimate state
 */
void control_thread(void) {
	/* thread setup */


	/* thread mainloop */
	for(;;) {

	}
}

/**
 * EEPROM Logging Thread
 * Writes flight events to eeprom
 */

void logging_thread(void) {
	/* thread setup */


	/* thread mainloop */
	for(;;) {

	}
}


void thread_a_entry(void) {
	static hal_systick_t last_wake;
	static const uint8_t spi_data[] = {0x55, 0xFE};
	static uint8_t spi_resp[2];
	last_wake = hal_systick_get();
	for(;;) {
		
		cli();
		hal_gpio_tgl(GPIOD, GPIO_PIN2);
		sei();

		hal_print("thread a\n\r");
		hal_spi_transfer_it(spi_data, spi_resp, 2, NULL);
		hal_delay(200);
		os_event_signal(&event_a);

		os_delay_windowed(&last_wake, 1000);

	}
}

void thread_b_entry(void) {
	static const uint8_t i2c_data[] = {0xF5, 0xEF};
	for(;;) {
		//os_event_wait(&event_a);
		cli();
		hal_gpio_tgl(GPIOD, GPIO_PIN3);
		sei();
		hal_print("thread b\n\r");
		hal_i2c_read(0x21, i2c_data, 2);
		uint8_t data = 0xff;
		//hal_i2c_transfer(0x31, 0, &data, 1);
		os_delay(1500);
	}
}

int main(void) {


	
	hal_systick_init();
	hal_uart_init();
	hal_i2c_init();
	hal_spi_init(1, 1, 0);
	os_system_init();



	/* hal initialization */
	hal_gpio_init_out(GPIOD, GPIO_PIN2|GPIO_PIN3|GPIO_PIN4);

	/* threads definitions */
	static os_thread_t thread_a = {
		.name = "thread_a"
	};
	static os_thread_t thread_b = {
		.name = "thread_b"
	};


	static uint8_t stack_a[256];
	static uint8_t stack_b[256];


	/* threads creation */
	os_thread_createI(&thread_a, 2, thread_a_entry, stack_a, 256);	
	os_thread_createI(&thread_b, 1, thread_b_entry, stack_b, 256);

	os_event_create(&event_a, OS_TAKEN);


	//execution is handed over to the scheduler
	os_system_start();



	for(;;) {

	}

	os_system_panic("end");
}



/* END */
