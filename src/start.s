;Author: Iacopo Sprenger

;startup assembly code for atmega328p


; some variables
.equ RAMEND_LOW,  0xff
.equ RAMEND_HIGH, 0x10

.equ SPH, 0x3E
.equ SPL, 0x3D


.section .text
.global main

.org 0x0000	; Reset vector
	rjmp	reset
.org 0x0002	; External Interrupt Request 0

.org 0x0004	; External Interrupt Request 1

.org 0x0006	; Pin Change Interrupt Request 0

.org 0x0008	; Pin Change Interrupt Request 0

.org 0x000a	; Pin Change Interrupt Request 1

.org 0x000c	; Watchdog Time-out Interrupt

.org 0x000e	; Timer/Counter2 Compare Match A

.org 0x0010	; Timer/Counter2 Compare Match A

.org 0x0012	; Timer/Counter2 Overflow

.org 0x0014	; Timer/Counter1 Capture Event

.org 0x0016	; Timer/Counter1 Compare Match A

.org 0x0018	; Timer/Counter1 Compare Match B

.org 0x001a	; Timer/Counter1 Overflow

.org 0x001c	; TimerCounter0 Compare Match A

.org 0x001e	; TimerCounter0 Compare Match B

.org 0x0020	; Timer/Couner0 Overflow

.org 0x0022	; SPI Serial Transfer Complete

.org 0x0024	; USART Rx Complete

.org 0x0026	; USART, Data Register Empty

.org 0x0028	; USART Tx Complete

.org 0x002a	; ADC Conversion Complete

.org 0x002c	; EEPROM Ready

.org 0x002e	; Analog Comparator

.org 0x0030	; Two-wire Serial Interface

.org 0x0032	; Store Program Memory Read


.org 0x0034

reset:
	; Init stack pointer	
	ldi	r16, RAMEND_LOW
	out	SPL, r16
	ldi	r16, RAMEND_HIGH
	out	SPH, r16		
	jmp 	main



