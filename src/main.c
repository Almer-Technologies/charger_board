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

#include <rf95.h>

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
#define BARO_MEAS	0xF4
#define BARO_CTRL	0xF5
#define BARO_DATA	0xF7
#define BARO_RESET	0xE0


#define IMU_ADDR	0x68

#define IMU_ID		0x75
#define IMU_MAGIC	0x68
#define IMU_DATA	0x3B

#define IMU_RANGE	16


#define RADIO_PREAMBLE 	8
#define RADIO_FREQ		868.0
#define RADIO_POWER		12


#define A -0.0065
#define R 287.0
#define G0 9.80665

#define CALIBRATION_LEN	4

#define STR_LEN	64

#define LED1_GPIO 	GPIOC
#define LED1_PIN	GPIO_PIN1
#define LED2_GPIO 	GPIOC
#define LED2_PIN	GPIO_PIN2
#define LED3_GPIO 	GPIOC
#define LED3_PIN	GPIO_PIN3
#define LED4_GPIO 	GPIOD
#define LED4_PIN	GPIO_PIN6
#define LED5_GPIO 	GPIOD
#define LED5_PIN	GPIO_PIN5

#define PYRO_GPIO 	GPIOD
#define PYRO_PIN	GPIO_PIN4


/* ROCKET STATES BEHAVIOURS */

#define POWERED_ACT_THRESHOLD	6000

#define POWERED_STAY_THRESHOLD	2000

#define	APOGEE_MIN				2000
#define APOGEE_DOWN				500
#define APOGEE_TIME				3500
#define APOGEE_COUNT			5
#define APOGEE_BCK_TIMER		5500 //ms

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
	ROCKET_ARMED,
	ROCKET_POWERED,
	ROCKET_COAST,
	ROCKET_APOGEE,
	ROCKET_DESCENT,
	ROCKET_TOUCHDOWN
}rocket_state_t;


typedef struct rocket_params {
	int16_t acc[3];
	int16_t gyro[3];
	int16_t acc0[3];
	int16_t gyro0[3];
	uint32_t acc_norm;
	uint32_t press;
	uint32_t press0;
	int32_t alt;
	int32_t alt_max;
	int32_t temp;
	int32_t temp0;
	float altf;
	float accf[3];
	float gyrof[3];
	float acc_normf;
	rocket_state_t state;
	int32_t timer;
	uint16_t counter;
	uint16_t bkp_timer;

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
static os_event_t event_uart;
static os_event_t event_spi;


static uint8_t lora_packet[128];


/**********************
 *	PROTOTYPES
 **********************/

void rocket_idle(void);
void rocket_idle_start(void);

void rocket_armed(void);
void rocket_armed_start(void);

void rocket_powered(void);
void rocket_powered_start(void);

void rocket_coast(void);
void rocket_coast_start(void);

void rocket_apogee(void);
void rocket_apogee_start(void);

void rocket_descent(void);
void rocket_descent_start(void);

void rocket_touchdown(void);
void rocket_touchdown_start(void);


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


float baro_get_altitude(uint32_t p0, int32_t t0, uint32_t p) {
	float alt;

	alt = ((t0+21575.0)/100.0)/A*(pow(((float)p)/((float)p0), -A*R/G0) - 1);

	return (alt);
}




void sensor_i2c_done(void) {
	os_event_signal(&event_i2c);
}

#define IMU_CONVERT_ACC(acc) \
	(((uint32_t)(acc))*1000)>>11

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

	data = 0b00011100; //filter

	hal_i2c_reg_write(BARO_ADDR, BARO_CTRL, &data, 1);

	hal_delay(100);

	data = 0b00110111; //t_ovr=1 p_ovr=1 mode=normal

	hal_i2c_reg_write(BARO_ADDR, BARO_MEAS, &data, 1);


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

	data = 0b00000000; //power management

	hal_i2c_reg_write(IMU_ADDR, 0x6B, &data, 1);

	os_delay(100); //power management

	hal_i2c_reg_write(IMU_ADDR, 0x6C, &data, 1);

	os_delay(100);

	data = 0b00011000; //config (full scale)

	hal_i2c_reg_write(IMU_ADDR, 0x1C, &data, 1);

	os_delay(100);

	sp.alt_max = 0;


	hal_systick_t last_wake = hal_systick_get();
	/* thread mainloop */
	for(;;) {

		/* IMU READ */

		static uint8_t imu_data[14];

		hal_i2c_reg_read_it(IMU_ADDR, IMU_DATA, &imu_data, 14, sensor_i2c_done);

		os_event_wait(&event_i2c);

		sp.acc[X] = IMU_CONVERT_ACC(PACK_I16(imu_data[0], imu_data[1]));
		sp.acc[Y] = IMU_CONVERT_ACC(PACK_I16(imu_data[2], imu_data[3]));
		sp.acc[Z] = IMU_CONVERT_ACC(PACK_I16(imu_data[4], imu_data[5]));

		sp.gyro[X] = PACK_I16(imu_data[8], imu_data[9]);
		sp.gyro[Y] = PACK_I16(imu_data[10], imu_data[11]);
		sp.gyro[Z] = PACK_I16(imu_data[12], imu_data[13]);

		/* IMU CALIBRATION */
		if(calibration == CALIBRATION_LEN) {//calibration start
			sp.acc0[X] = sp.acc[X]/CALIBRATION_LEN;
			sp.acc0[Y] = (sp.acc[Y]-1000)/CALIBRATION_LEN; //1g in y direction
			sp.acc0[Z] = sp.acc[Z]/CALIBRATION_LEN;
			sp.gyro0[X] = sp.gyro[X]/CALIBRATION_LEN;
			sp.gyro0[Y] = sp.gyro[Y]/CALIBRATION_LEN;
			sp.gyro0[Z] = sp.gyro[Z]/CALIBRATION_LEN;
		} else if(calibration) {
			sp.acc0[X] += sp.acc[X]/CALIBRATION_LEN;
			sp.acc0[Y] += (sp.acc[Y]-1000)/CALIBRATION_LEN;
			sp.acc0[Z] += sp.acc[Z]/CALIBRATION_LEN;
			sp.gyro0[X] += sp.gyro[X]/CALIBRATION_LEN;
			sp.gyro0[Y] += sp.gyro[Y]/CALIBRATION_LEN;
			sp.gyro0[Z] += sp.gyro[Z]/CALIBRATION_LEN;
			if(calibration == 1){
			}
		} else{
			sp.acc[X] -= sp.acc0[X];
			sp.acc[Y] -= sp.acc0[Y];
			sp.acc[Z] -= sp.acc0[Z];

			sp.gyro[X] -= sp.gyro0[X];
			sp.gyro[Y] -= sp.gyro0[Y];
			sp.gyro[Z] -= sp.gyro0[Z];
		}

		cli();
		sp.acc_norm = sqrt((uint32_t)sp.acc[X]*(uint32_t)sp.acc[X] +
							  (uint32_t)sp.acc[Y]*(uint32_t)sp.acc[Y] +
							  (uint32_t)sp.acc[Z]*(uint32_t)sp.acc[Z]);
		sei();


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
			sp.altf = baro_get_altitude(sp.press0, sp.temp0, sp.press);
			sp.alt = (int32_t)(sp.altf*100.0);
		}

		if(sp.alt_max < sp.alt) {
			sp.alt_max = sp.alt;
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

		os_delay_windowed(&last_wake, 100);
	}
}

void sensor_spi_done(void) {
	os_event_signal(&event_spi);
}


#define RADIO_WR_ADDR(addr) (addr) |  (1<<7)
#define RADIO_RD_ADDR(addr) (addr) & ~(1<<7)

//Add TX led

void radio_send(uint8_t * data, uint8_t len) {
	hal_gpio_set(LED5_GPIO, LED5_PIN);
	uint8_t spi_data = 0;

	//set mode IDLE
	spi_data = RH_RF95_MODE_STDBY | RH_RF95_LONG_RANGE_MODE;
	hal_spi_reg_write_it(RADIO_WR_ADDR(RH_RF95_REG_01_OP_MODE), &spi_data, 1, sensor_spi_done);
	os_event_wait(&event_spi);

	//set fifo addr=0
	spi_data = 0; //fifo address
	hal_spi_reg_write_it(RADIO_WR_ADDR(RH_RF95_REG_0D_FIFO_ADDR_PTR), &spi_data, 1, sensor_spi_done);
	os_event_wait(&event_spi);

	//set Radiohead compatible headers
	spi_data = 0xff; //tx header to
	hal_spi_reg_write_it(RADIO_WR_ADDR(RH_RF95_REG_00_FIFO), &spi_data, 1, sensor_spi_done);
	os_event_wait(&event_spi);

	spi_data = 0; //tx header from
	hal_spi_reg_write_it(RADIO_WR_ADDR(RH_RF95_REG_00_FIFO), &spi_data, 1, sensor_spi_done);
	os_event_wait(&event_spi);

	spi_data = 0; //tx header id
	hal_spi_reg_write_it(RADIO_WR_ADDR(RH_RF95_REG_00_FIFO), &spi_data, 1, sensor_spi_done);
	os_event_wait(&event_spi);

	spi_data = 0; //tx header flags
	hal_spi_reg_write_it(RADIO_WR_ADDR(RH_RF95_REG_00_FIFO), &spi_data, 1, sensor_spi_done);
	os_event_wait(&event_spi);

	hal_spi_reg_write_it(RADIO_WR_ADDR(RH_RF95_REG_00_FIFO), data, len, sensor_spi_done);
	os_event_wait(&event_spi);

	spi_data = len + RH_RF95_HEADER_LEN;
	hal_spi_reg_write_it(RADIO_WR_ADDR(RH_RF95_REG_22_PAYLOAD_LENGTH), &spi_data, 1, sensor_spi_done);
	os_event_wait(&event_spi);

	//set mode TX
	spi_data = RH_RF95_MODE_TX | RH_RF95_LONG_RANGE_MODE;
	hal_spi_reg_write_it(RADIO_WR_ADDR(RH_RF95_REG_01_OP_MODE), &spi_data, 1, sensor_spi_done);
	os_event_wait(&event_spi);

	spi_data = 0x40;
	hal_spi_reg_write_it(RADIO_WR_ADDR(RH_RF95_REG_40_DIO_MAPPING1), &spi_data, 1, sensor_spi_done);
	os_event_wait(&event_spi);

	hal_gpio_clr(LED5_GPIO, LED5_PIN);
}

/**
 * Radio feedback thread
 * Uses SPI to communicate with the radio
 * as fast as possible send last data
 */
void radio_thread_entry(void) {
	/* thread setup */

	static uint8_t report_data[STR_LEN];
	static uint16_t str_len;
	uint8_t data;

	os_delay(500);
	os_event_create(&event_spi, OS_TAKEN);

	hal_spi_reg_read_it(RADIO_RD_ADDR(RH_RF95_REG_42_VERSION), &data, 1, sensor_spi_done);
	os_event_wait(&event_spi);

	str_len = snprintf(report_data, STR_LEN, "radio version: %x\n\r", data);

	hal_uart_send(report_data, str_len);

	/* configure radio */
	//set sleep mode
	data = RH_RF95_MODE_SLEEP | RH_RF95_LONG_RANGE_MODE;
	hal_spi_reg_write_it(RADIO_WR_ADDR(RH_RF95_REG_01_OP_MODE), &data, 1, sensor_spi_done);
	os_event_wait(&event_spi);
	os_delay(10);

	data = 0;

	hal_spi_reg_read_it(RADIO_RD_ADDR(RH_RF95_REG_01_OP_MODE), &data, 1, sensor_spi_done);
	os_event_wait(&event_spi);

	if(data != (RH_RF95_MODE_SLEEP | RH_RF95_LONG_RANGE_MODE)) {
		while(1) {
			//hal_print("ERROR\n\r");
		}
	}

    // Set up FIFO
    // We configure so that we can use the entire 256 byte FIFO for either receive
    // or transmit, but not both at the same time
	data = 0;
	hal_spi_reg_write_it(RADIO_WR_ADDR(RH_RF95_REG_0E_FIFO_TX_BASE_ADDR), &data, 1, sensor_spi_done);
	os_event_wait(&event_spi);

	data = 0;
	hal_spi_reg_write_it(RADIO_WR_ADDR(RH_RF95_REG_0F_FIFO_RX_BASE_ADDR), &data, 1, sensor_spi_done);
	os_event_wait(&event_spi);

	data = RH_RF95_MODE_STDBY | RH_RF95_LONG_RANGE_MODE;
	hal_spi_reg_write_it(RADIO_WR_ADDR(RH_RF95_REG_01_OP_MODE), &data, 1, sensor_spi_done);
	os_event_wait(&event_spi);

	//modem configuration:
	data = 0x92;
	hal_spi_reg_write_it(RADIO_WR_ADDR(RH_RF95_REG_1D_MODEM_CONFIG1), &data, 1, sensor_spi_done);
	os_event_wait(&event_spi);

	data = 0x74;
	hal_spi_reg_write_it(RADIO_WR_ADDR(RH_RF95_REG_1E_MODEM_CONFIG2), &data, 1, sensor_spi_done);
	os_event_wait(&event_spi);

	data = 0x04;
	hal_spi_reg_write_it(RADIO_WR_ADDR(RH_RF95_REG_26_MODEM_CONFIG3), &data, 1, sensor_spi_done);
	os_event_wait(&event_spi);

	//configure preamble
	data = RADIO_PREAMBLE >> 8;
	hal_spi_reg_write_it(RH_RF95_REG_20_PREAMBLE_MSB, &data, 1, sensor_spi_done);
	os_event_wait(&event_spi);

	data = RADIO_PREAMBLE & 0xFF;
	hal_spi_reg_write_it(RH_RF95_REG_21_PREAMBLE_LSB, &data, 1, sensor_spi_done);
	os_event_wait(&event_spi);

	//configure frequency
	uint32_t frf = (RADIO_FREQ * 1000000.0) / RH_RF95_FSTEP;
	data = (frf >> 16) & 0xff;
	hal_spi_reg_write_it(RADIO_WR_ADDR(RH_RF95_REG_06_FRF_MSB), &data, 1, sensor_spi_done);
	os_event_wait(&event_spi);

	data = (frf >> 8) & 0xff;
	hal_spi_reg_write_it(RADIO_WR_ADDR(RH_RF95_REG_07_FRF_MID), &data, 1, sensor_spi_done);
	os_event_wait(&event_spi);

	data = frf & 0xff;
	hal_spi_reg_write_it(RADIO_WR_ADDR(RH_RF95_REG_08_FRF_LSB), &data, 1, sensor_spi_done);
	os_event_wait(&event_spi);


	//configure Tx Power
#if RADIO_POWER > 20
	data = RH_RF95_PA_DAC_ENABLE;
	hal_spi_reg_write_it(RADIO_WR_ADDR(RH_RF95_REG_4D_PA_DAC), &data, 1, sensor_spi_done);
	os_event_wait(&event_spi);

	data = RH_RF95_PA_SELECT | (RADIO_POWER-5);
	hal_spi_reg_write_it(RADIO_WR_ADDR(RH_RF95_REG_09_PA_CONFIG), &data, 1, sensor_spi_done);
	os_event_wait(&event_spi);
#else
	data = RH_RF95_PA_DAC_DISABLE;
	hal_spi_reg_write_it(RADIO_WR_ADDR(RH_RF95_REG_4D_PA_DAC), &data, 1, sensor_spi_done);
	os_event_wait(&event_spi);

	data = RH_RF95_PA_SELECT | (RADIO_POWER-2);
	hal_spi_reg_write_it(RADIO_WR_ADDR(RH_RF95_REG_09_PA_CONFIG), &data, 1, sensor_spi_done);
	os_event_wait(&event_spi);
#endif

	//wait for first measurements
	os_delay(500);

	/* thread mainloop */
	for(;;) {


		//build GRUND compatible LoRa packet

		static float temp_var_f;
		static uint32_t temp_var_u32;
		static int32_t temp_var_i32;

		//timestamp
		temp_var_u32 = hal_systick_get();
		memcpy(lora_packet, &temp_var_u32, 4); //timestamp


		//accelerometer
		cli();
		memcpy(lora_packet+4, &(sp.accf[X]), 4);
		sei();
		cli();
		memcpy(lora_packet+8, &(sp.accf[Y]), 4);
		sei();
		cli();
		memcpy(lora_packet+12, &(sp.accf[Z]), 4);
		sei();
		//gyroscope
		cli();
		memcpy(lora_packet+16, &(sp.gyrof[X]), 4);
		sei();
		cli();
		memcpy(lora_packet+20, &(sp.gyrof[Y]), 4);
		sei();
		cli();
		memcpy(lora_packet+24, &(sp.gyrof[Z]), 4);
		sei();

		memset(lora_packet+28, 0, 4); //reserved mpu??

		memset(lora_packet+32, 0, 4); //reserved bmp??

		//pressure
		cli();
		memcpy(lora_packet+36, &(sp.press), 4);
		sei();

		//altitude
		cli();
		memcpy(lora_packet+40, &(sp.altf), 4);
		sei();

		//battery levels + status
		memset(lora_packet+44, 0, 12);

		//status
		memcpy(lora_packet+60, &(sp.state), 1);

		//gps
		memset(lora_packet+61, 0, 24);



		//send packet through radio

		radio_send(lora_packet, sizeof(lora_packet));



		os_delay(500);

	}
}





void rocket_idle(void) {
	rocket_armed_start();
}

void rocket_idle_start(void) {
	sp.state = ROCKET_IDLE;
	cli();
	hal_gpio_clr(GPIOC, GPIO_PIN1|GPIO_PIN2|GPIO_PIN3);
	hal_gpio_clr(GPIOD, GPIO_PIN6|GPIO_PIN5);
	sei();

}

void rocket_armed(void) {
	if(sp.acc_norm > POWERED_ACT_THRESHOLD) {
		rocket_powered_start();
	}
}

void rocket_armed_start(void) {
	sp.state = ROCKET_ARMED;
	cli();
	hal_gpio_set(LED4_GPIO, LED4_PIN);
	sei();

}

void rocket_powered(void) {
	if(sp.acc_norm < POWERED_STAY_THRESHOLD) {
		rocket_coast_start();
	}
}

void rocket_powered_start(void) {
	sp.state = ROCKET_POWERED;
	sp.bkp_timer = APOGEE_BCK_TIMER;
	cli();
	hal_gpio_set(LED1_GPIO, LED1_PIN);
	sei();
}

void rocket_coast(void) {
	if(sp.alt > APOGEE_MIN) {
		if(sp.alt < (sp.alt_max - APOGEE_DOWN)) {
			sp.counter++;
			if(sp.counter > APOGEE_COUNT) {
				rocket_apogee_start();
			}
		} else {
			sp.counter = 0;
		}
	} else {
		sp.counter = 0;
	}
}

void rocket_coast_start(void) {
	sp.state = ROCKET_COAST;
	cli();
	hal_gpio_set(LED2_GPIO, LED2_PIN);
	sei();
}

void rocket_apogee(void) {
	if(sp.timer <= 0) {
		sp.timer = 0;
		cli();
		hal_gpio_clr(PYRO_GPIO, PYRO_PIN);
		hal_gpio_clr(LED3_GPIO, LED3_PIN);
		sei();
		rocket_descent_start();
	}
}

void rocket_apogee_start(void) {
	sp.state = ROCKET_APOGEE;
	cli();
	hal_gpio_set(LED3_GPIO, LED3_PIN);
	hal_gpio_set(PYRO_GPIO, PYRO_PIN);
	sei();
	sp.timer = APOGEE_TIME;
}

void rocket_descent(void) {
	if((sp.alt<0?-sp.alt:sp.alt) < APOGEE_DOWN){
		rocket_touchdown_start();
	}
}

void rocket_descent_start(void) {
	sp.state = ROCKET_DESCENT;
}

void rocket_touchdown(void) {

}

void rocket_touchdown_start(void) {
	sp.state = ROCKET_TOUCHDOWN;
	cli();
	hal_gpio_set(LED5_GPIO, LED5_PIN);
	sei();
}




/**
 * Global control thread
 * Determines rocket state and triggers flight events
 * use sensor data to estimate state
 */
void control_thread_entry(void) {
	/* thread setup */

	rocket_idle_start();

	hal_systick_t last_wake = hal_systick_get();

	/* thread mainloop */
	for(;;) {

		if(sp.timer) {
			sp.timer -= 100;
		}
		if(sp.bkp_timer) {
			sp.bkp_timer -= 100;
			if(sp.bkp_timer <= 0) { //passage a zero
				rocket_apogee_start();
			}
		}



		switch(sp.state) {
		case ROCKET_IDLE:
			rocket_idle();
			break;
		case ROCKET_ARMED:
			rocket_armed();
			break;
		case ROCKET_POWERED:
			rocket_powered();
			break;
		case ROCKET_COAST:
			rocket_coast();
			break;
		case ROCKET_APOGEE:
			rocket_apogee();
			break;
		case ROCKET_DESCENT:
			rocket_descent();
			break;
		case ROCKET_TOUCHDOWN:
			rocket_touchdown();
			break;
		default:
			break;
		}

		os_delay_windowed(&last_wake, 100);

	}
}

/**
 * EEPROM Logging Thread
 * Writes flight events to eeprom
 */

void logger_uart_done(void) {
	os_event_signal(&event_uart);
}

void logger_thread_entry(void) {
	/* thread setup */



	os_event_create(&event_uart, OS_TAKEN);

	os_delay(500);


	hal_systick_t last_wake = hal_systick_get();

	/* thread mainloop */
	for(;;) {

		static uint8_t report_data[STR_LEN];
		static uint16_t str_len;

		str_len = snprintf(report_data, STR_LEN, "=== DATA ===\n\racc: %d %d %d (%lu)\n\r", sp.acc[X], sp.acc[Y], sp.acc[Z], sp.acc_norm);

		hal_uart_send_it(report_data, str_len, logger_uart_done);

		os_event_wait(&event_uart);
/*
		str_len = snprintf(report_data, STR_LEN, "gyro: %d %d %d\n\r", sp.gyro[X], sp.gyro[Y], sp.timer);

		hal_uart_send_it(report_data, str_len, logger_uart_done);

		os_event_wait(&event_uart);
*/
		str_len = snprintf(report_data, STR_LEN, "press: %ld\n\r", sp.press);

		hal_uart_send_it(report_data, str_len, logger_uart_done);

		os_event_wait(&event_uart);

		str_len = snprintf(report_data, STR_LEN, "alt: %d cm\n\r", sp.alt);

		hal_uart_send_it(report_data, str_len, logger_uart_done);

		os_event_wait(&event_uart);
		switch(sp.state) {
		case ROCKET_IDLE:
			str_len = snprintf(report_data, STR_LEN, "state: IDLE\n\r");
			break;
		case ROCKET_ARMED:
			str_len = snprintf(report_data, STR_LEN, "state: ARMED\n\r");
			break;
		case ROCKET_POWERED:
			str_len = snprintf(report_data, STR_LEN, "state: POWERED\n\r");
			break;
		case ROCKET_COAST:
			str_len = snprintf(report_data, STR_LEN, "state: COAST\n\r");
			break;
		case ROCKET_APOGEE:
			str_len = snprintf(report_data, STR_LEN, "state: APOGEE\n\r");
			break;
		case ROCKET_DESCENT:
			str_len = snprintf(report_data, STR_LEN, "state: DESCENT\n\r");
			break;
		case ROCKET_TOUCHDOWN:
			str_len = snprintf(report_data, STR_LEN, "state: TOUHDOWN\n\r");
			break;
		default:
			str_len = snprintf(report_data, STR_LEN, "state: ???\n\r");
			break;
		}


		hal_uart_send_it(report_data, str_len, logger_uart_done);

		os_event_wait(&event_uart);

		os_delay_windowed(&last_wake, 500);

	}
}

int main(void) {

	/* hal led initialization */

	hal_systick_init();
	hal_uart_init();
	hal_i2c_init();
	hal_spi_init(0, 1, 0);
	os_system_init();




	/* setup feedback leds */

	hal_gpio_init_out(GPIOC, GPIO_PIN1|GPIO_PIN2|GPIO_PIN3);
	hal_gpio_init_out(GPIOD, GPIO_PIN6|GPIO_PIN5|GPIO_PIN4);

	hal_gpio_clr(GPIOC, GPIO_PIN1|GPIO_PIN2|GPIO_PIN3);
	hal_gpio_clr(GPIOD, GPIO_PIN6|GPIO_PIN5);

	/* threads definitions */

	static uint8_t sensor_stack[256];
	static os_thread_t sensor_thread = {
		.name = "sensor  "
	};

	static uint8_t radio_stack[256];
	static os_thread_t radio_thread = {
		.name = "radio   "
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
	os_thread_createI(&sensor_thread, 5, sensor_thread_entry, sensor_stack, sizeof(sensor_stack));
	//os_thread_createI(&radio_thread, 4, radio_thread_entry, radio_stack, sizeof(radio_stack));
	os_thread_createI(&control_thread, 3, control_thread_entry, control_stack, sizeof(control_stack));
	os_thread_createI(&logger_thread, 1, logger_thread_entry, logger_stack, sizeof(logger_stack));




	//execution is handed over to the scheduler
	os_system_start();



	for(;;) {

	}

	os_system_panic("end");
}



/* END */
