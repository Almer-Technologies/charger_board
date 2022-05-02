;Author: Iacopo Sprenger

;startup assembly code for atmega328p


; some variables
.equ RAMEND_LOW,  0xff
.equ RAMEND_HIGH, 0x08

.equ EECR, 	0x3f
.equ SPH, 	0x3E
.equ SPL, 	0x3D


.section .text

.global main
.global timer0_cmpa_int
.global usart_dre_int

.org 0x0000	; Reset vector
	jmp	reset
.org 0x0004	; External Interrupt Request 0
	jmp	int_guard
.org 0x0008	; External Interrupt Request 1
	jmp	int_guard
.org 0x000c	; Pin Change Interrupt Request 0
	jmp	int_guard
.org 0x0010	; Pin Change Interrupt Request 0
	jmp	int_guard
.org 0x0014	; Pin Change Interrupt Request 1
	jmp	int_guard
.org 0x0018	; Watchdog Time-out Interrupt
	jmp	int_guard
.org 0x001c	; Timer/Counter2 Compare Match A
	jmp	int_guard
.org 0x0020	; Timer/Counter2 Compare Match A
	jmp	int_guard
.org 0x0024	; Timer/Counter2 Overflow
	jmp	int_guard
.org 0x0028	; Timer/Counter1 Capture Event
	jmp	int_guard
.org 0x002c	; Timer/Counter1 Compare Match A
	jmp	int_guard
.org 0x0030	; Timer/Counter1 Compare Match B
	jmp	int_guard
.org 0x0034	; Timer/Counter1 Overflow
	jmp	int_guard
.org 0x0038	; TimerCounter0 Compare Match A
	jmp	timer0_compa_int
.org 0x003c	; TimerCounter0 Compare Match B
	jmp	int_guard
.org 0x0040	; Timer/Couner0 Overflow
	jmp	int_guard	
.org 0x0044	; SPI Serial Transfer Complete
	jmp	int_guard
.org 0x0048	; USART Rx Complete
	jmp	int_guard
.org 0x004c	; USART, Data Register Empty
	jmp	usart_dre_int
.org 0x0050	; USART Tx Complete
	jmp	int_guard
.org 0x0054	; ADC Conversion Complete
	jmp	int_guard
.org 0x0058	; EEPROM Ready
	jmp	int_guard
.org 0x005c	; Analog Comparator
	jmp	int_guard
.org 0x0060	; Two-wire Serial Interface
	jmp	int_guard
.org 0x0064	; Store Program Memory Read
	jmp	int_guard

.org 0x0080

reset:
	; Init stack pointer	
	eor 	r1, r1
	out 	EECR, r1
	ldi	r16, RAMEND_LOW
	out	SPL, r16
	ldi	r16, RAMEND_HIGH
	out	SPH, r16		
	call 	main
	jmp 	exit


int_guard:
	jmp		0x00;




exit:
	cli
	jmp		.-2