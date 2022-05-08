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
    _IO_BYTE((port)+GPIO_DDRx) |= (pins); \
    nop()

#define hal_gpio_init_in(port, pins) \
    _IO_BYTE((port)+GPIO_DDRx) &= ~(pins); \
    nop()

#define hal_gpio_init_in_pup(port, pins) \
    _IO_BYTE((port)+GPIO_DDRx) &= ~(pins); \
    _IO_BYTE((port)+GPIO_PORTx) |= (pins); \
    nop()

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


typedef uint32_t hal_systick_t;



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

/* hal systick */
void hal_systick_init();
hal_systick_t hal_systick_get(void);
hal_systick_t hal_systick_getI(void);
void hal_systick_inc(void);



#endif /* HAL_H */

/* END */
