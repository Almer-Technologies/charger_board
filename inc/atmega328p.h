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


//memory mapped general purpose registers

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

#define EECR    _MMIO_BYTE(0x3F)
#define EEDR    _MMIO_BYTE(0x40)
#define EEARL   _MMIO_BYTE(0x41)
#define EEARH   _MMIO_BYTE(0x42)
    
#define GTCCR   _MMIO_BYTE(0x43)
    
//timer0    
#define TCCR0A  _MMIO_BYTE(0x44)
#define TCCR0B  _MMIO_BYTE(0x45)
#define TCNT0   _MMIO_BYTE(0x46)
#define OCR0A   _MMIO_BYTE(0x47)
#define OCR0B   _MMIO_BYTE(0x48)
    
#define GPIOR1  _MMIO_BYTE(0x4A)
#define GPIOR2  _MMIO_BYTE(0x4B)
    
#define SPCR    _MMIO_BYTE(0x4C)
#define SPSR    _MMIO_BYTE(0x4D)
#define SPDR    _MMIO_BYTE(0x4E)

#define ACSR    _MMIO_BYTE(0x50)

//sleep modes register
#define SMCR    _MMIO_BYTE(0x53)
#define MCUCR   _MMIO_BYTE(0x55)


#define SPMCSR  _MMIO_BYTE(0x57)

#define SPL     _MMIO_BYTE(0x5D)
#define SPH     _MMIO_BYTE(0x5E)
#define SREG    _MMIO_BYTE(0x5F)

#define WDTCSR  _MMIO_BYTE(0x60)
#define CLKPR   _MMIO_BYTE(0x61)

#define PRR     _MMIO_BYTE(0x64)


/**********************
 *  BIT FIELDS
 **********************/





#endif /* ATMEGA328P_H */

/* END */