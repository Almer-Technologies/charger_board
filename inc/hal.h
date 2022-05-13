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

/* hal systick */
void hal_systick_init(void);
hal_systick_t hal_systick_get(void);
hal_systick_t hal_systick_getI(void);
void hal_systick_inc(void);



#endif /* HAL_H */

/* END */
