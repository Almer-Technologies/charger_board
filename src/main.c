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
#include <math.h>

/**********************
 *	CONSTANTS
 **********************/

#define X 0
#define Y 1
#define Z 2

#define BARO_ADDR	0x76

#define BARO_ID		0xD0
#define BARO_MAGIC	0x58
#define BARO_TRIM	0x88
#define BARO_CTRL_MEAS	0xF4
#define BARO_CTRL	0xF5
#define BARO_DATA	0xF7
#define BARO_RESET	0xE0


#define IMU_ADDR	0x68

#define IMU_ID		0x75
#define IMU_MAGIC	0x68
#define IMU_DATA	0x3B


#define A -0.0065
#define R 287.0
#define G0 9.80665

#define CALIBRATION_LEN	4

/**********************
 *	MACROS
 **********************/

#define PACK_U16(dH, dL) 	((uint16_t)(dH)<<8)|((uint16_t)(dL)<<0)
#define PACK_I16(dH, dL) 	((int16_t)(dH)<<8)|((int16_t)(dL)<<0)


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
	int16_t acc0[3];
	int16_t gyro0[3];
	uint32_t press;
	uint32_t press0;
	int32_t alt;
	int32_t temp;
	int32_t temp0;
	rocket_state_t state;

}rocket_params_t;


/**********************
 *	VARIABLES
 **********************/



static rocket_params_t sp;

/* baro trim parameters */
static uint16_t dig_T1;
static int16_t 	dig_T2;
static int16_t 	dig_T3;
static uint16_t dig_P1;
static int16_t  dig_P2;
static int16_t  dig_P3;
static int16_t  dig_P4;
static int16_t  dig_P5;
static int16_t  dig_P6;
static int16_t  dig_P7;
static int16_t  dig_P8;
static int16_t  dig_P9;

static int32_t t_fine;

static os_event_t event_i2c;


/**********************
 *	PROTOTYPES
 **********************/


/**********************
 *	DECLARATIONS
 **********************/



int32_t baro_compensate_temp(int32_t temp) {
	int32_t var1, var2, T;
	var1 = ((((temp>>3) - ((int32_t)dig_T1<<1))) * ((int32_t)dig_T2)) >> 11;
	var2 = (((((temp>>4) - ((int32_t)dig_T1)) * ((temp>>4) - ((int32_t)dig_T1)))>>12) * ((int32_t)dig_T3))>>14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}

//#define USE_UINT64
#ifdef USE_UINT64
uint32_t baro_compensate_press(int32_t press) {
	int64_t var1, var2, p;
	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)dig_P6;
	var2 = var2 + ((var1*(int64_t)dig_P5)<<17);
	var2 = var2 + (((int64_t)dig_P4)<<35);
	var1 = ((var1 * var1 * (int64_t)dig_P3)>>8) + ((var1 * (int64_t)dig_P2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)dig_P1)>>33;
	if (var1 == 0) {
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576-press;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((int64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((int64_t)dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7)<<4);
	return (uint32_t)p;
}
#else
uint32_t baro_compensate_press(int32_t press) {
	int32_t var1, var2;
	uint32_t p;
	var1 = (((int32_t)t_fine)>>1) - (int32_t)64000;
	var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((int32_t)dig_P6);
	var2 = var2 + ((var1*((int32_t)dig_P5))<<1);
	var2 = (var2>>2)+(((int32_t)dig_P4)<<16);
	var1 = (((dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((int32_t)dig_P2) * var1)>>1))>>18;
	var1 =((((32768+var1))*((int32_t)dig_P1))>>15);
	if (var1 == 0)
	{
	return 0; // avoid exception caused by division by zero
	}
	p = (((uint32_t)(((int32_t)1048576)-press)-(var2>>12)))*3125;
	if (p < 0x80000000)
	{
	p = (p << 1) / ((uint32_t)var1);
	}
	else
	{
	p = (p / (uint32_t)var1) * 2;
	}
	var1 = (((int32_t)dig_P9) * ((int32_t)(((p>>3) * (p>>3))>>13)))>>12;
	var2 = (((int32_t)(p>>2)) * ((int32_t)dig_P8))>>13;
	p = (uint32_t)((int32_t)p + ((var1 + var2 + dig_P7) >> 4));
	return p;
}
#endif


int32_t baro_get_altitude(uint32_t p0, int32_t t0, uint32_t p) {
	float alt;

	alt = ((t0+21575.0)/100.0)/A*(pow(((float)p)/((float)p0), -A*R/G0) - 1);

	return (int32_t) (alt*100);
}




void sensor_i2c_done(void) {
	os_event_signal(&event_i2c);
}

/**
 * Sensor acquisition thread
 * Uses i2c to communicate with sensors
 * about 50Hz -> record max acc, vel and alt
 */
void sensor_thread_entry(void) {
	/* thread setup */

	/* i2c event */

	os_event_create(&event_i2c, OS_TAKEN);

	os_delay(100);

	static uint8_t data_id;
	static uint8_t data;
	static uint8_t calibration;
	calibration = CALIBRATION_LEN;

	/* BARO detect */

	uint8_t attempts = 5;


	while (attempts) {

		hal_i2c_reg_read(BARO_ADDR, BARO_ID, &data_id, 1);
		if(data_id == BARO_MAGIC) {
			//baro detected
			hal_print("baro detected\n\r");
			break;
		}
		os_delay(100);
		attempts--;
	}

	data = 0xB6;

	hal_i2c_reg_write(BARO_ADDR, BARO_RESET, &data, 1);

	os_delay(100);

	/* BARO configuration */

	static uint8_t baro_trim[26];
	hal_i2c_reg_read(BARO_ADDR, BARO_TRIM, baro_trim, 26);

	dig_T1 = PACK_U16(baro_trim[1], baro_trim[0]);
	dig_T2 = PACK_I16(baro_trim[3], baro_trim[2]);
	dig_T3 = PACK_I16(baro_trim[5], baro_trim[4]);
	dig_P1 = PACK_U16(baro_trim[7], baro_trim[6]);
	dig_P2 = PACK_I16(baro_trim[9], baro_trim[8]);
	dig_P3 = PACK_I16(baro_trim[11], baro_trim[10]);
	dig_P4 = PACK_I16(baro_trim[13], baro_trim[12]);
	dig_P5 = PACK_I16(baro_trim[15], baro_trim[14]);
	dig_P6 = PACK_I16(baro_trim[17], baro_trim[16]);
	dig_P7 = PACK_I16(baro_trim[19], baro_trim[18]);
	dig_P8 = PACK_I16(baro_trim[21], baro_trim[20]);
	dig_P9 = PACK_I16(baro_trim[23], baro_trim[22]);

	hal_delay(100);

	data = 0b00001000; //filter

	hal_i2c_reg_write(BARO_ADDR, BARO_CTRL, &data, 1);

	hal_delay(100);

	data = 0b00110111; //t_ovr=1 p_ovr=1 mode=normal

	hal_i2c_reg_write(BARO_ADDR, BARO_CTRL_MEAS, &data, 1);


	attempts = 5;

	/* IMU detect */
	while (attempts) {

		hal_i2c_reg_read(IMU_ADDR, IMU_ID, &data_id, 1);
		if(data_id == IMU_MAGIC) {
			//baro detected
			hal_print("imu detected\n\r");
			break;
		}
		os_delay(100);
		attempts = 10;
	}

	/* IMU configuration */

	data = 0b00000000;

	hal_i2c_reg_write(IMU_ADDR, 0x6B, &data, 1);

	os_delay(100);

	hal_i2c_reg_write(IMU_ADDR, 0x6C, &data, 1);

	os_delay(100);

	data = 0b00011000;

	hal_i2c_reg_write(IMU_ADDR, 0x1C, &data, 1);

	os_delay(100);



	hal_systick_t last_wake = hal_systick_get();
	/* thread mainloop */
	for(;;) {

		/* IMU READ */

		static uint8_t imu_data[14];

		hal_i2c_reg_read_it(IMU_ADDR, IMU_DATA, &imu_data, 14, sensor_i2c_done);

		os_event_wait(&event_i2c);

		sp.acc[X] = PACK_I16(imu_data[0], imu_data[1]);
		sp.acc[Y] = PACK_I16(imu_data[2], imu_data[3]);
		sp.acc[Z] = PACK_I16(imu_data[4], imu_data[5]);

		sp.gyro[X] = PACK_I16(imu_data[8], imu_data[9]);
		sp.gyro[Y] = PACK_I16(imu_data[10], imu_data[11]);
		sp.gyro[Z] = PACK_I16(imu_data[12], imu_data[13]);

		/* IMU CALIBRATION */
//		if(calibration == CALIBRATION_LEN) {//calibration start
//			sp.acc0[X] = sp.acc[X]/CALIBRATION_LEN;
//			sp.acc0[Y] = sp.acc[Y]/CALIBRATION_LEN;
//			sp.acc0[Z] = sp.acc[Z]/CALIBRATION_LEN;
//			sp.gyro0[X] = sp.gyro[X]/CALIBRATION_LEN;
//			sp.gyro0[Y] = sp.gyro[Y]/CALIBRATION_LEN;
//			sp.gyro0[Z] = sp.gyro[Z]/CALIBRATION_LEN;
//		} else if(calibration) {
//			sp.acc0[X] += sp.acc[X]/CALIBRATION_LEN;
//			sp.acc0[Y] += sp.acc[Y]/CALIBRATION_LEN;
//			sp.acc0[Z] += sp.acc[Z]/CALIBRATION_LEN;
//			sp.gyro0[X] += sp.gyro[X]/CALIBRATION_LEN;
//			sp.gyro0[Y] += sp.gyro[Y]/CALIBRATION_LEN;
//			sp.gyro0[Z] += sp.gyro[Z]/CALIBRATION_LEN;
//			if(calibration == 1){
//			}
//		}


		/* BARO READ */

		static uint8_t baro_data[6];

		hal_i2c_reg_read_it(BARO_ADDR, BARO_DATA, &baro_data, 6, sensor_i2c_done);

		os_event_wait(&event_i2c);

		int32_t press = ((int32_t)baro_data[0]<<12)|((int32_t)baro_data[1]<<4)|((int32_t)baro_data[2]<<0);
		int32_t temp = ((int32_t)baro_data[3]<<12)|((int32_t)baro_data[4]<<4)|((int32_t)baro_data[5]<<0);

		sp.temp = baro_compensate_temp(temp);
		sp.press = baro_compensate_press(press);


		/* BARO CALIBRATION */
		if(calibration == CALIBRATION_LEN) {//calibration start
			sp.press0 = sp.press/CALIBRATION_LEN;
			sp.temp0 = sp.temp/CALIBRATION_LEN;
			sp.alt = 0;
			calibration--;
		} else if(calibration) {
			sp.press0 += sp.press/CALIBRATION_LEN;
			sp.temp0 += sp.temp/CALIBRATION_LEN;
			sp.alt = 0;
			calibration--;
		}else {
			sp.alt = baro_get_altitude(sp.press0, sp.temp0, sp.press);
		}

/*
		static uint8_t report_data[128];
		static uint16_t str_len;


		str_len = sprintf(report_data, "data: %hd, %hd, %hd | %hd, %hd, %hd \n\r",
				sp.acc[X], sp.acc[Y], sp.acc[Z], sp.gyro[X], sp.gyro[Y], sp.gyro[Z]);

		hal_uart_send(report_data, str_len);
*/

/*
		str_len = snprintf(report_data, 128, "data: %ld  ", sp.press0);

		hal_uart_send(report_data, str_len);

		str_len = snprintf(report_data, 128, " %ld ", sp.alt);

		hal_uart_send(report_data, str_len);

		str_len = snprintf(report_data, 128, " %ld\n\r", sp.press);

		hal_uart_send(report_data, str_len);

*/

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


		switch(sp.state) {
		case ROCKET_IDLE:

		case ROCKET_PRESSURIZED:

		case ROCKET_POWERED:

		case ROCKET_COAST:

		case ROCKET_APOGEE:

		case ROCKET_TOUCHDOWN:

		default:
			break;
		}

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




	//execution is handed over to the scheduler
	os_system_start();



	for(;;) {

	}

	os_system_panic("end");
}



/* END */
