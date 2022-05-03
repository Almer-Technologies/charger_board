/*  Title		: HAL
 *  Filename		: hal.c
 *	Author		: iacopo sprenger
 *	Date		: 30.04.2022
 *	Version		: 0.1
 *	Description	: Hardware abstraction layer
 */

/**********************
 *	INCLUDES
 **********************/

#include "hal.h"
#include <avr/interrupt.h>

/**********************
 *	CONSTANTS
 **********************/


/**********************
 *	MACROS
 **********************/

#define hal_systick_inc() \
	system_tick++


/**********************
 *	TYPEDEFS
 **********************/

typedef struct hal_uart {
	uint8_t tx_busy:1;
	uint8_t rx_busy:1;
	uint16_t tx_len;
	uint16_t rx_len;
	uint16_t tx_data_p;
	uint16_t rx_data_p;
	uint8_t * tx_data;
	uint8_t * rx_data;
	void (*tx_cmplt)(void);
	void (*rx_cmplt)(void);
}hal_uart_t;


/**********************
 *	VARIABLES
 **********************/

static hal_uart_t uart;

static hal_systick_t system_tick;


/**********************
 *	PROTOTYPES
 **********************/


/**********************
 *	DECLARATIONS
 **********************/


/* hal delay */

void hal_delay(uint32_t delay_ms) {
	hal_systick_t time = hal_systick_get();

	while((hal_systick_get()) < (delay_ms+time));
}



/* hal uart */

void hal_uart_init(void) {

	uart.tx_busy = 0;
	uart.rx_busy = 0;

	uint16_t ubrr = 8; //115200 baud

	UBRR0L = (uint8_t) ubrr;
	UBRR0H = (uint8_t) ubrr>>8;


	//enable rx and/or tx
	UCSR0B = (1<<TXENx) | (1<<RXENx);

	//set frame format 8 data 1 stop no parity
	UCSR0C = (0b11<<UCSZx0);
}


void hal_uart_send_char(uint8_t data) {
	while (!(UCSR0A & (1<<UDREx))); //wait prev data to be transmitted
	UDR0 = data;
}

void hal_uart_send(uint8_t * data, uint16_t len) {
	if(uart.tx_busy) {
		return;
	}
	uart.tx_busy = 1;
	uart.tx_len = len;
	uart.tx_data = data;
	uart.tx_data_p = 0;
	while(uart.tx_data_p < uart.tx_len) {
		while (!(UCSR0A & (1<<UDREx))); //wait prev data to be transmitted
		UDR0 = (uint8_t) (uart.tx_data[uart.tx_data_p++]);
	}
	while (!(UCSR0A & (1<<TXCx))); //wait for end of data transmission
	UCSR0A |= (1<<TXCx);
	uart.tx_busy = 0;
}

void hal_uart_send_it(uint8_t * data, uint16_t len, void (*tx_cmplt)(void)) {
	if(uart.tx_busy) {
		return;
	}
	uart.tx_busy = 1;
	uart.tx_len = len;
	uart.tx_data = data;
	uart.tx_data_p = 0;
	uart.tx_cmplt = tx_cmplt;
	//send first bit
	UDR0 = (uint8_t) (uart.tx_data[uart.tx_data_p++]);
	//enable interrupt
	UCSR0B |= 1<<UDRIEx; //enable DRE interrupt
}

uint8_t hal_uart_recv_char(void) {
	while (!(UCSR0A & (1<<RXCx))); //wait for char to arrive
	return UDR0;
}

void hal_uart_recv(uint8_t * data, uint16_t len) {
	if(uart.rx_busy) {
		return;
	}
	uart.rx_busy = 1;
	uart.rx_len = len;
	uart.rx_data = data;
	uart.rx_data_p = 0;
	while(uart.rx_data_p < uart.rx_len) {
		while(!(UCSR0A & (1<<RXCx))); //wait for char to arrive
		uart.rx_data[uart.rx_data_p++] = UDR0;
	}
	uart.rx_busy = 0;
}

void hal_uart_recv_it(uint8_t * data, uint16_t len, void (*rx_cmplt)(void)) {
	if(uart.rx_busy) {
		return;
	}
	uart.rx_busy = 1;
	uart.rx_len = len;
	uart.rx_data = data;
	uart.rx_data_p = 0;
	uart.rx_cmplt = rx_cmplt;

	//enable uart rx interrupt
	UCSR0B |= 1<<RXCIEx; 
}


/* hal systick */

void hal_systick_init(void) {

	system_tick = 0;

	TCCR0A = 0b10; //ctc mode

	OCR0A = 249; //250-1
	//--> this gives us an interrupt freq of 1ms

	//using timer0 with prescaler /8
	//also start timer
	TCCR0B = 0b011<<CSO;

	TCNT0 = 0;

	TIMSK0 = 1<<OCIExA; //enable compare A interrupt
}



/**
 * 	returns system time in tick (usually ms)
 **/

hal_systick_t hal_systick_get(void) {
	//guards as 32bit assignement is not atomic
	cli();
	hal_systick_t shadow = system_tick;
	sei();
	return shadow;
}




/**********************
 *	INTERRUPTS
 **********************/


ISR(TIMER0_COMPA_vect) {
	hal_systick_inc();
}


ISR(USART_UDRE_vect) {
	//ready to send next byte
	if((uart.tx_data_p < uart.tx_len)) {
		UDR0 = (uint8_t) (uart.tx_data[uart.tx_data_p++]);
		
	} else {

		//disable interrupt for uart DRE
		UCSR0B &= ~(1<<UDRIEx);
		//enable TXC interrupt
		UCSR0A |= (1<<TXCx);
		UCSR0B |= 1<<TXCIEx;
	}
}

ISR(USART_TX_vect) {
	//transmission complete
	UCSR0A |= (1<<TXCx);
	UCSR0B &= ~(1<<TXCIEx);
	uart.tx_busy = 0;
	if(uart.tx_cmplt) {
		uart.tx_cmplt();
	}
}

ISR(USART_RX_vect) {
	//transmission complete
	uart.rx_data[uart.rx_data_p++] = UDR0;
	if(uart.rx_data_p >= uart.rx_len) {
		UCSR0B &= ~(1<<RXCIEx);
		uart.rx_busy = 0;
		if(uart.rx_cmplt) {
			uart.rx_cmplt();
		}
	}
	
}

/* END */

