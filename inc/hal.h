/*  Title       : HAL
 *  Filename    : hal.h
 *  Author      : iacopo sprenger
 *  Date        : 30.04.2022
 *  Version     : 0.1
 *  Description : Hardware abstraction layer
 */


#ifndef HAL_H
#define HAL_H




/**********************
 *  INCLUDES
 **********************/

#include <stdint.h>

#include "atmega328p.h"
#include "port.h"

/**********************
 *  CONSTANTS
 **********************/

#define HAL_GPIO_OUT    1
#define HAL_GPIO_IN     0





/**********************
 *  MACROS
 **********************/

/* hal gpio */

#define hal_gpio_init_out(port, pins) \
    _IO_BYTE((port)+GPIO_DDRx) |= (pins)

#define hal_gpio_init_in(port, pins) \
    _IO_BYTE((port)+GPIO_DDRx) &= ~(pins)

#define hal_gpio_init_in_pup(port, pins) \
    _IO_BYTE((port)+GPIO_DDRx) &= ~(pins); \
    _IO_BYTE((port)+GPIO_PORTx) |= (pins)

#define hal_gpio_get(port, pin) \
    _IO_BYTE((port)+GPIO_PINx) & (pin) ? 1 : 0

#define hal_gpio_set(port, pin) \
    _IO_BYTE((port)+GPIO_PORTx) |= (pin)

#define hal_gpio_clr(port, pin) \
    _IO_BYTE((port)+GPIO_PORTx) &= ~(pin)

#define hal_gpio_tgl(port, pin) \
    _IO_BYTE((port)+GPIO_PORTx) ^= (pin)


/* hal_utils */

#define hal_print(string) \
    hal_uart_send(string, sizeof(string));

#define hal_print_it(string) \
    hal_uart_send_it(string, sizeof(string), NULL);


/* hal sleep */

#define hal_sleep_enter()   \
    SMCR = 1;               \
    asm volatile (          \
        "sleep" )           

#define hal_sleep_disable() \
    SMCR = 0

/**********************
 *  TYPEDEFS
 **********************/

typedef enum hal_error {
	HAL_SUCCESS,
	HAL_ERROR,
	HAL_BUSY
}hal_error_t;


typedef uint32_t hal_systick_t;

typedef enum i2c_dir {
    HAL_I2C_READ = 0x0,
    HAL_I2C_WRITE = 0x1,
} i2c_dir_t;



/**********************
 *  VARIABLES
 **********************/


/**********************
 *  PROTOTYPES
 **********************/

/* hal delay */
void hal_delay(uint32_t delay_ms);

/* hal uart */
void hal_uart_init(void);
void hal_uart_send_char(uint8_t data);
void hal_uart_send(uint8_t * data, uint16_t len);
void hal_uart_send_it(uint8_t * data, uint16_t len, void (*tx_cmplt)(void));
uint8_t hal_uart_recv_char(void);
void hal_uart_recv(uint8_t * data, uint16_t len);
void hal_uart_recv_it(uint8_t * data, uint16_t len, void (*rx_cmplt)(void));

/* hal pwm */
typedef enum hal_led_brightness {
    LED_OFF,
    LED_LOW,
    LED_HIGH,
    LED_ON,
}hal_led_brightness_t;

void hal_led_init(void);
uint8_t hal_led_attach(uint8_t * port, uint8_t pin);
void hal_led_set_brightness(uint8_t channel, hal_led_brightness_t step);


/* hal i2c */
void hal_i2c_init(void);
void hal_i2c_write(uint8_t address, uint8_t * data, uint16_t len);
void hal_i2c_read(uint8_t address, uint8_t * data, uint16_t len);
void hal_i2c_reg_write(uint8_t address, uint8_t reg, uint8_t * data, uint16_t len);
uint8_t hal_i2c_reg_read(uint8_t address, uint8_t reg, uint8_t * data, uint16_t len);
void hal_i2c_write_it(uint8_t address, uint8_t * data, uint16_t len, void (*tfr_cplt)(void));
void hal_i2c_read_it(uint8_t address, uint8_t * data, uint16_t len, void (*tfr_cplt)(void));
void hal_i2c_reg_write_it(uint8_t address, uint8_t reg, uint8_t * data, uint16_t len, void (*tfr_cplt)(void));
void hal_i2c_reg_read_it(uint8_t address, uint8_t reg, uint8_t * data, uint16_t len, void (*tfr_cplt)(void));

/* hal spi */
void hal_spi_init(uint8_t cpol, uint8_t cpha, uint8_t lsb_first);
void hal_spi_transfer(uint8_t * data, uint8_t * resp, uint16_t len);
void hal_spi_transfer_it(uint8_t * data, uint8_t * resp, uint16_t len, void (*tfr_cplt)(void));
void hal_spi_reg_write(uint8_t addr, uint8_t * data, uint16_t len);
void hal_spi_reg_read(uint8_t addr, uint8_t * data, uint16_t len);
void hal_spi_reg_write_it(uint8_t addr, uint8_t * data, uint16_t len, void (*tfr_cplt)(void));
void hal_spi_reg_read_it(uint8_t addr, uint8_t * data, uint16_t len, void (*tfr_cplt)(void));

/* hal systick */
void hal_systick_init(void);
hal_systick_t hal_systick_get(void);
hal_systick_t hal_systick_getI(void);
void hal_systick_inc(void);



#endif /* HAL_H */

/* END */
