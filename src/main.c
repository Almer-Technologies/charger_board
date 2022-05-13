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

#define BARO_ADDR	0x78

#define BARO_ID	0x0D


#define IMU_ADDR	0x68

#define IMU_ID		0x75
#define IMU_MAGIC	0x68

/**********************
 *	MACROS
 **********************/

#define PACK_U16(dH, dL) 	((dH)<<8)|((dL)<<0)


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
	int16_t acc[3];
	int16_t gyro[3];
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
void sensor_thread_entry(void) {
	/* thread setup */

	//try to connect to BMP

	os_delay(500);

	static uint8_t data_id;


	while (1) {

		hal_i2c_reg_read(0x68, 0x75, &data_id, 1);
		if(data_id == 0x58) {
			//baro detected
			break;
		}
		os_delay(100);
		break;
	}

	hal_print("baro detected\n\r");


	//try to connect to IMU
	while (1) {

		hal_i2c_reg_read(IMU_ADDR, IMU_ID, &data_id, 1);
		if(data_id == IMU_MAGIC) {
			//baro detected
			break;
		}
		os_delay(100);
	}
	hal_print("imu detected\n\r");

	uint8_t pmanage_data;
	pmanage_data = 0b00000000;

	hal_i2c_reg_write(IMU_ADDR, 0x6B, &pmanage_data, 1);

	os_delay(100);

	hal_i2c_reg_write(IMU_ADDR, 0x6C, &pmanage_data, 1);

	os_delay(100);


	hal_systick_t last_wake = hal_systick_get();
	/* thread mainloop */
	for(;;) {
		hal_print("imu loop\n\r");
		uint8_t sensor_data[14];

		hal_i2c_reg_read_it(IMU_ADDR, 0x3B, &sensor_data, 14, NULL);

		//decode imu data

		sp.acc[X] = PACK_U16(sensor_data[0], sensor_data[1]);
		sp.acc[Y] = PACK_U16(sensor_data[2], sensor_data[3]);
		sp.acc[Z] = PACK_U16(sensor_data[4], sensor_data[5]);

		sp.gyro[X] = PACK_U16(sensor_data[8], sensor_data[9]);
		sp.gyro[Y] = PACK_U16(sensor_data[10], sensor_data[11]);
		sp.gyro[Z] = PACK_U16(sensor_data[12], sensor_data[13]);

		static uint8_t report_data[64];
		static uint16_t str_len;

		str_len = sprintf(report_data, "data: %d, %d, %d, %d, %d, %d\n\r", sp.acc[X], sp.acc[Y], sp.acc[Z], sp.gyro[X], sp.gyro[Y], sp.gyro[Z]);

		hal_uart_send_it(report_data, str_len, NULL);

		os_delay_windowed(&last_wake, 500);
	}
}

/**
 * Radio feedback thread
 * Uses SPI to communicate with the radio
 * as fast as possible send last data
 */
void radio_thread_entry(void) {
	/* thread setup */


	/* thread mainloop */
	for(;;) {

		uint8_t resp[2];
		uint8_t data[2];
		data[0] = (1<<7)|0x42;
		data[1] = 0;

		hal_spi_transfer(data, resp, 2);

		os_delay(500);

	}
}

/**
 * Global control thread
 * Determines rocket state and triggers flight events
 * use sensor data to estimate state
 */
void control_thread_entry(void) {
	/* thread setup */


	/* thread mainloop */
	for(;;) {

	}
}

/**
 * EEPROM Logging Thread
 * Writes flight events to eeprom
 */

void logger_thread_entry(void) {
	/* thread setup */


	/* thread mainloop */
	for(;;) {

	}
}

int main(void) {


	
	hal_systick_init();
	hal_uart_init();
	hal_i2c_init();
	hal_spi_init(1, 1, 0);
	os_system_init();



	/* hal led initialization */
	hal_gpio_init_out(GPIOD, GPIO_PIN2|GPIO_PIN3|GPIO_PIN4);

	/* threads definitions */
	static uint8_t stack_a[256];
	static os_thread_t thread_a = {
		.name = "thread_a"
	};

	static uint8_t stack_b[256];
	static os_thread_t thread_b = {
		.name = "thread_b"
	};

	static uint8_t sensor_stack[256];
	static os_thread_t sensor_thread = {
		.name = "sensor  "
	};

	static uint8_t radio_stack[256];
	static os_thread_t radio_thread = {
		.name = "radio   "
	};

	static uint8_t control_stack[256];
	static os_thread_t control_thread = {
		.name = "control "
	};

	static uint8_t logger_stack[256];
	static os_thread_t logger_thread = {
		.name = "logger  "
	};






	/* threads creation */
	os_thread_createI(&sensor_thread, 3, sensor_thread_entry, sensor_stack, 256);
	os_thread_createI(&radio_thread, 1, radio_thread_entry, radio_stack, 256);

	os_event_create(&event_a, OS_TAKEN);


	//execution is handed over to the scheduler
	os_system_start();



	for(;;) {

	}

	os_system_panic("end");
}



/* END */
