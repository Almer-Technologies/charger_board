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

#define _IO_BYTE(mem_addr) 	(*(volatile uint8_t *)(mem_addr))
#define _MMIO_BYTE(mem_addr) 	(*(volatile uint8_t *)(mem_addr))

#define NULL (void*) 0

/**********************
 * DEFINITIONS
 **********************/



//memory mapped general purpose registers


#define GPIO_PINx	0U
#define GPIO_DDRx	1U
#define GPIO_PORTx	2U

#define PIN0		1<<0U
#define PIN1		1<<1U
#define PIN2		1<<2U
#define PIN3		1<<3U
#define PIN4		1<<4U
#define PIN5		1<<5U
#define PIN6		1<<6U
#define PIN7		1<<7U

/* GPIOB */
#define GPIOB	0x23
#define PINB    _IO_BYTE(0x23)
#define DDRB    _IO_BYTE(0x24)
#define PORTB   _IO_BYTE(0x25)

/* GPIOC */
#define GPIOC	0x26
#define PINC    _IO_BYTE(0x26)
#define DDRC    _IO_BYTE(0x27)
#define PORTC   _IO_BYTE(0x28)

/* GPIOD */
#define GPIOD	0x29
#define PIND    _IO_BYTE(0x29)
#define DDRD    _IO_BYTE(0x2A)
#define PORTD   _IO_BYTE(0x2B)



/* timer interrupt flags */
#define TIFR0   _IO_BYTE(0x35)
#define TIFR1   _IO_BYTE(0x36)
#define TIFR2   _IO_BYTE(0x37)

//
#define PCIFR   _IO_BYTE(0x3B)
#define EIFR    _IO_BYTE(0x3C)

#define EIMSK   _IO_BYTE(0x3D)
#define GPIOR0  _IO_BYTE(0x3E)

#define EECR    _IO_BYTE(0x3F)
#define EEDR    _IO_BYTE(0x40)
#define EEARL   _IO_BYTE(0x41)
#define EEARH   _IO_BYTE(0x42)
    
#define GTCCR   _IO_BYTE(0x43)
    
/* timer0 */
#define TCCR0A  _IO_BYTE(0x44)
#define TCCR0B  _IO_BYTE(0x45)
#define TCNT0   _IO_BYTE(0x46)
#define OCR0A   _IO_BYTE(0x47)
#define OCR0B   _IO_BYTE(0x48)
    
#define GPIOR1  _IO_BYTE(0x4A)
#define GPIOR2  _IO_BYTE(0x4B)
    
#define SPCR    _IO_BYTE(0x4C)
#define SPSR    _IO_BYTE(0x4D)
#define SPDR    _IO_BYTE(0x4E)

#define ACSR    _IO_BYTE(0x50)

/* mcu control */
#define SMCR    _IO_BYTE(0x53)
#define MCUCR   _IO_BYTE(0x55)


#define SPMCSR  _IO_BYTE(0x57)

#define SPL     _IO_BYTE(0x5D)
#define SPH     _IO_BYTE(0x5E)
#define SREG    _IO_BYTE(0x5F)

#define WDTCSR  _MMIO_BYTE(0x60)
#define CLKPR   _MMIO_BYTE(0x61)



#define PRR     _MMIO_BYTE(0x64)


#define TIMSK0  _MMIO_BYTE(0x6E)




/* usart */
#define UCSR0A	_MMIO_BYTE(0xC0)
#define UCSR0B  _MMIO_BYTE(0xC1)
#define UCSR0C  _MMIO_BYTE(0xC2)
#define UBRR0L  _MMIO_BYTE(0xC4)
#define UBRR0H  _MMIO_BYTE(0xC5)
#define UDR0    _MMIO_BYTE(0xC6)











/**********************
 *  BIT FIELDS
 **********************/

/* timers */
#define CSO	0U

#define TOIEx	0U
#define OCIExA	1U
#define OCIExB	2U

#define TOIEx	0U
#define OCIExA	1U
#define OCIExB	2U

/* uart */
#define MPCMx	0U
#define U2Xx	1U
#define UPEx	2U
#define DORx	3U
#define FEx	4U
#define UDREx	5U
#define TXCx	6U
#define RXCx	7U

#define TXB8x	0U
#define RXB8x	1U
#define UCSZx2	2U
#define TXENx	3U
#define RXENx	4U
#define UDRIEx	5U
#define TXCIEx	6U
#define RXCIEx	7U

#define UCPOLx  0U
#define UCSZx0  1U
#define UCSZx1  2U
#define USBSx   3U
#define UPMx0   4U
#define UPMx1   5U
#define UMSELx0 6U
#define UMSELx1 7U

#endif /* ATMEGA328P_H */

/* END */
