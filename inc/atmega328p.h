/*  Title       : atmega328p
 *  Filename    : template.h
 *  Author      : atmega328p sprenger
 *  Date        : 29.04.2022
 *  Version     : 0.1
 *  Description : definitions for atmega328p
 */

#ifndef ATMEGA328P_H
#define ATMEGA328P_H



/**********************
 *  INCLUDES
 **********************/

#include <stdint.h>

/**********************
 *  TYPES
 **********************/

#define _MMIO_BYTE(mem_addr) (*(volatile uint8_t *)(mem_addr))

/**********************
 * DEFINITIONS
 **********************/

// GPIOB
#define PINB    _MMIO_BYTE(0x23)
#define DDRB    _MMIO_BYTE(0x24)
#define PORTB   _MMIO_BYTE(0x25)

// GPIOC
#define PINC    _MMIO_BYTE(0x26)
#define DDRC    _MMIO_BYTE(0x27)
#define PORTC   _MMIO_BYTE(0x28)

// GPIOD
#define PIND    _MMIO_BYTE(0x29)
#define DDRD    _MMIO_BYTE(0x2A)
#define PORTD   _MMIO_BYTE(0x2B)

//
#define TIFR0   _MMIO_BYTE(0x35)
#define TIFR1   _MMIO_BYTE(0x36)
#define TIFR2   _MMIO_BYTE(0x37)

//
#define PCIFR   _MMIO_BYTE(0x3B)
#define EIFR    _MMIO_BYTE(0x3C)

#define EIMSK   _MMIO_BYTE(0x3D)
#define GPIOR0  _MMIO_BYTE(0x3E)




#define MCUCR   _MMIO_BYTE(0x55)


/**********************
 *  BIT FIELDS
 **********************/





#endif /* ATMEGA328P_H */

/* END */