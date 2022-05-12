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


#define INFO	0
#define VERBOSE 1



#define DEBUG INFO


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


#define GPIO_PINx	(0)
#define GPIO_DDRx	(1)
#define GPIO_PORTx	(2)

#define GPIO_PIN0		(1<<0)
#define GPIO_PIN1		(1<<1)
#define GPIO_PIN2		(1<<2)
#define GPIO_PIN3		(1<<3)
#define GPIO_PIN4		(1<<4)
#define GPIO_PIN5		(1<<5)
#define GPIO_PIN6		(1<<6)
#define GPIO_PIN7		(1<<7)

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

#define OSCCAL	_MMIO_BYTE(0x66)
#define PCICR	_MMIO_BYTE(0x68)
#define EICRA	_MMIO_BYTE(0x69)

#define PCMSK0	_MMIO_BYTE(0x6B)
#define PCMSK1	_MMIO_BYTE(0x6C)
#define PCMSK2	_MMIO_BYTE(0x6D)


#define TIMSK0  _MMIO_BYTE(0x6E)
#define TIMSK1	_MMIO_BYTE(0x6F)
#define TIMSK2	_MMIO_BYTE(0x70)


#define ADCL	_MMIO_BYTE(0x78)
#define ADCH	_MMIO_BYTE(0x79)
#define ADCSRA	_MMIO_BYTE(0x7A)
#define ADCSRB	_MMIO_BYTE(0x7B)
#define ADMUX	_MMIO_BYTE(0x7C)
#define DIDR0	_MMIO_BYTE(0x7E)
#define DIDR1	_MMIO_BYTE(0x7F)


#define TCCR1A	_MMIO_BYTE(0x80)
#define TCCR1B	_MMIO_BYTE(0x81)
#define TCCR1C	_MMIO_BYTE(0x82)
#define TCNT1L	_MMIO_BYTE(0x84)
#define TCNT1H	_MMIO_BYTE(0x85)
#define ICR1L	_MMIO_BYTE(0x86)
#define ICR1H	_MMIO_BYTE(0x87)
#define OCR1AL	_MMIO_BYTE(0x88)
#define OCR1AH	_MMIO_BYTE(0x89)
#define OCR1BL	_MMIO_BYTE(0x8A)
#define OCR1BH	_MMIO_BYTE(0x8B)

#define TCCR2A	_MMIO_BYTE(0xB0)
#define TCCR2B	_MMIO_BYTE(0xB1)
#define TCNT2	_MMIO_BYTE(0xB2)
#define OCR2A	_MMIO_BYTE(0xB3)
#define OCR2B	_MMIO_BYTE(0xB4)

#define ASSR	_MMIO_BYTE(0xB6)

/* i2c */
#define TWBR	_MMIO_BYTE(0xB8)
#define TWSR	_MMIO_BYTE(0xB9)
#define TWAR	_MMIO_BYTE(0xBA)
#define TWDR	_MMIO_BYTE(0xBB)
#define TWCR	_MMIO_BYTE(0xBC)
#define TWAMR	_MMIO_BYTE(0xBD)

/* uart */
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

/* spi */
#define SPR0	0U
#define SPR1	1U
#define CPHA	2U
#define CPOL	3U
#define MSTR	4U
#define DORD	5U
#define SPE	    6U
#define SPIE	7U

#define SPI2X	0U
#define WCOL	6U
#define SPIF	7U


/* i2c */
#define TWIE	0U
#define TWEN	2U
#define TWWC	3U
#define TWSTO	4U
#define TWSTA	5U
#define TWEA	6U
#define TWINT	7U

#define	TWPS0	0U
#define TWPS1	1U
#define TWS3	3U
#define TWS4	4U
#define TWS5	5U
#define TWS6	6U
#define TWS7	7U

#define TW_START		    0x08
#define TW_RSTART		    0x10
#define TW_MT_SLAW_ACK	    0x18
#define TW_MT_SLAW_NACK	    0x20
#define TW_MT_DATAW_ACK	    0x28
#define TW_MT_DATAW_NACK	0x30
#define TW_LOST		        0x38
#define TW_MT_SLAR_ACK		0x40
#define TW_MT_SLAR_NACK		0x48
#define TW_MT_DATAR_ACK		0x50
#define TW_MT_DATAR_NACK	0x58







/* SMCR */

#define SMCR_SE	0U

#endif /* ATMEGA328P_H */

/* END */
