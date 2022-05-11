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

#include <hal.h>
#include <avr/interrupt.h>

/**********************
 *	CONSTANTS
 **********************/


/**********************
 *	MACROS
 **********************/

#define I2C_FREQUENCY 400000

#define SPI_FREQUENCY 1000000

#define SPI_MIN_FREQUENCY 0

#ifndef F_CPU
    #define F_CPU 16000000UL
    #warning "F_CPU not defined! Assuming 16MHz."
#endif

/* i2c freq */

#if (F_CPU/I2C_FREQUENCY - 16) / (2 * 1) >= 10 \
        && (F_CPU/I2C_FREQUENCY - 16) / (2 * 1) <= 0xFF
    #define TWI_PRESCALER 1
    #define TWPS0_VALUE 0
    #define TWPS1_VALUE 0
#elif (F_CPU/I2C_FREQUENCY - 16) / (2 * 4) >= 10 \
        && (F_CPU/I2C_FREQUENCY - 16) / (2 * 4) <= 0xFF
    #define TWI_PRESCALER 4
    #define TWPS0_VALUE 1
    #define TWPS1_VALUE 0
#elif (F_CPU/I2C_FREQUENCY - 16) / (2 * 16) >= 10 \
        && (F_CPU/I2C_FREQUENCY - 16) / (2 * 16) <= 0xFF
    #define TWI_PRESCALER 16
    #define TWPS0_VALUE 0
    #define TWPS1_VALUE 1
#elif (F_CPU/I2C_FREQUENCY - 16) / (2 * 64) >= 10 \
        && (F_CPU/I2C_FREQUENCY - 16) / (2 * 64) <= 0xFF
    #define TWI_PRESCALER 64
    #define TWPS0_VALUE 1
    #define TWPS1_VALUE 1
#else
    #error "I2C_FREQUENCY too low!"
#endif

#define TWBR_VALUE ((F_CPU/I2C_FREQUENCY - 16) / (2 * TWI_PRESCALER))


#if SPI_MIN_FREQUENCY <= F_CPU/2 && F_CPU/2 <= SPI_FREQUENCY
    #define SPI_PRESCALER 2
    #define SPR0_VALUE 0
    #define SPR1_VALUE 0
    #define SPI2X_VALUE 1
#elif SPI_MIN_FREQUENCY <= F_CPU/4 && F_CPU/4 <= SPI_FREQUENCY
    #define SPI_PRESCALER 4
    #define SPR0_VALUE 0
    #define SPR1_VALUE 0
    #define SPI2X_VALUE 0
#elif SPI_MIN_FREQUENCY <= F_CPU/8 && F_CPU/8 <= SPI_FREQUENCY
    #define SPI_PRESCALER 8
    #define SPR0_VALUE 1
    #define SPR1_VALUE 0
    #define SPI2X_VALUE 1
#elif SPI_MIN_FREQUENCY <= F_CPU/16 && F_CPU/16 <= SPI_FREQUENCY
    #define SPI_PRESCALER 16
    #define SPR0_VALUE 1
    #define SPR1_VALUE 0
    #define SPI2X_VALUE 0
#elif SPI_MIN_FREQUENCY <= F_CPU/32 && F_CPU/32 <= SPI_FREQUENCY
    #define SPI_PRESCALER 32
    #define SPR0_VALUE 0
    #define SPR1_VALUE 1
    #define SPI2X_VALUE 1
#elif SPI_MIN_FREQUENCY <= F_CPU/64 && F_CPU/64 <= SPI_FREQUENCY
    #define SPI_PRESCALER 64
    #define SPR0_VALUE 1
    #define SPR1_VALUE 1
    #define SPI2X_VALUE 1
#elif SPI_MIN_FREQUENCY <= F_CPU/128 && F_CPU/128 <= SPI_FREQUENCY
    #define SPI_PRESCALER 128
    #define SPR0_VALUE 1
    #define SPR1_VALUE 1
    #define SPI2X_VALUE 0
#else
    #error "No valid SPI_PRESCALER found!"
#endif


#define i2c_wait() 	\
        while(!(TWCR & (1<<TWINT)))

#define i2c_start() 	\
        TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN)

#define i2c_restart() 	\
        TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN)

#define i2c_address_write(address) 	\
        TWDR = ((address)<<1) & ~0x01;	\
        TWCR = (1 << TWINT) | (1 << TWEN)

#define i2c_address_read(address) 	\
        TWDR = ((address)<<1) | 0x01;	\
        TWCR = (1 << TWINT) | (1 << TWEN)

#define i2c_write(data) 	\
        TWDR = data;		\
    	TWCR = (1 << TWINT) | (1 << TWEN)

#define i2c_read_ack()		\
    	TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN)

#define i2c_read_nack()		\
    	TWCR = (1 << TWINT) | (1 << TWEN)

#define i2c_stop()		\
    	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN)

#define i2c_status()		\
    	(TWSR & 0xF8)

/**********************
 *	TYPEDEFS
 **********************/

typedef struct hal_uart {
	uint8_t tx_busy;
	uint8_t rx_busy;
	uint16_t tx_len;
	uint16_t rx_len;
	uint16_t tx_data_p;
	uint16_t rx_data_p;
	uint8_t * tx_data;
	uint8_t * rx_data;
	void (*tx_cmplt)(void);
	void (*rx_cmplt)(void);
}hal_uart_t;

typedef struct hal_spi {
	uint8_t busy;
	uint8_t * data;
	uint8_t * resp;
	uint16_t len;
	uint16_t data_p;
	void (*tfr_cplt)(void);
}hal_spi_t;

typedef enum i2c_mode {
	I2C_WR,
	I2C_RD,
	I2C_WR_REG,
	I2C_RD_REG
}i2c_mode_t;

typedef enum i2c_state {
	I2C_START,
	I2C_ADDR,
	I2C_REG,
	I2C_DATA,
	I2C_STOP
}i2c_state_t;


typedef struct hal_i2c {
	uint8_t busy;
	uint8_t address;
	uint8_t reg;
	uint8_t * data;
	uint16_t len;
	uint16_t data_p;
	i2c_state_t state;
	i2c_mode_t mode;
	void (*tfr_cplt)(void);
}hal_i2c_t;


/**********************
 *	VARIABLES
 **********************/

static hal_uart_t uart;

static hal_spi_t spi;

static hal_i2c_t i2c;

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

void hal_sleep_idle(void) {
	
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

/* hal i2c */
void hal_i2c_init(void) {

	hal_gpio_init_in(GPIOC, GPIO_PIN4|GPIO_PIN5); //set gpio to input for high Z
	hal_gpio_set(GPIOC, GPIO_PIN4|GPIO_PIN5); //internal pull-ups for i2c


	i2c.busy = 0;

	i2c.tfr_cplt = NULL;

	TWBR = TWBR_VALUE;

    	TWSR = (TWPS1_VALUE << TWPS1) | (TWPS0_VALUE << TWPS0);
    
    	TWCR = (1 << TWEN);

}

void hal_i2c_write(uint8_t address, uint8_t * data, uint16_t len) {
	if(i2c.busy) {
		return;
	}

	i2c.busy = 1;

	i2c.data = data;
	i2c.data_p = 0;
	i2c.len = len;
	i2c.address = address;

	i2c_start();

	i2c_wait();

	if(i2c_status() != TW_START) {
		i2c.busy = 0;
		return;
	}

	i2c_address_write(i2c.address);

	i2c_wait();

	if(i2c_status() != TW_MT_SLA_ACK) {
		i2c_stop();
		i2c.busy = 0;
		return;
	}


	while(i2c.data_p < i2c.len) {
		i2c_write(i2c.data[i2c.data_p++]);
		i2c_wait();

		if(i2c_status() != TW_MT_DATA_ACK) {
			i2c_stop();
			i2c.busy = 0;
			return;
		}
	}

	i2c_stop();

	i2c.busy = 0;
}

void hal_i2c_read(uint8_t address, uint8_t * data, uint16_t len) {
	if(i2c.busy) {
		return;
	}

	i2c.busy = 1;

	i2c.data = data;
	i2c.data_p = 0;
	i2c.len = len;
	i2c.address = address;

	i2c_start();

	i2c_wait();

	if(i2c_status() != TW_START) {
		i2c.busy = 0;
		return;
	}

	i2c_address_read(i2c.address);

	i2c_wait();

	if(i2c_status() != TW_MT_SLA_ACK) {
		i2c_stop();
		i2c.busy = 0;
		return;
	}

	while(i2c.data_p < i2c.len) {
		i2c_read_ack();
		i2c_wait();
		i2c.data[i2c.data_p++] = TWDR;
	}

	i2c_stop();

	i2c.busy = 0;
}

void hal_i2c_reg_write(uint8_t address, uint8_t reg, uint8_t * data, uint16_t len) {
	if(i2c.busy) {
		return;
	}

	i2c.busy = 1;

	i2c.data = data;
	i2c.data_p = 0;
	i2c.len = len;
	i2c.address = address;
	i2c.reg = reg;

	i2c_start();

	i2c_wait();

	if(i2c_status() != TW_START) {
		i2c.busy = 0;
		return;
	}

	i2c_address_write(i2c.address);

	i2c_wait();

	if(i2c_status() != TW_MT_SLA_ACK) {
		i2c_stop();
		i2c.busy = 0;
		return;
	}

	i2c_write(i2c.reg);

	i2c_wait();

	if(i2c_status() != TW_MT_DATA_ACK) {
		i2c_stop();
		i2c.busy = 0;
		return;
	}

	while(i2c.data_p < i2c.len) {
		i2c_write(i2c.data[i2c.data_p++]);
		i2c_wait();

		if(i2c_status() != TW_MT_DATA_ACK) {
			i2c_stop();
			i2c.busy = 0;
			return;
		}
	}

	i2c_stop();

	i2c.busy = 0;
}

void hal_i2c_reg_read(uint8_t address, uint8_t reg, uint8_t * data, uint16_t len) {
	if(i2c.busy) {
		return;
	}

	i2c.busy = 1;

	i2c.data = data;
	i2c.data_p = 0;
	i2c.len = len;
	i2c.address = address;
	i2c.reg = reg;

	i2c_start();

	i2c_wait();

	if(i2c_status() != TW_START) {
		i2c.busy = 0;
		return;
	}

	i2c_address_write(i2c.address);

	i2c_wait();

	if(i2c_status() != TW_MT_SLA_ACK) {
		i2c_stop();
		i2c.busy = 0;
		return;
	}

	i2c_write(i2c.reg);

	i2c_wait();

	if(i2c_status() != TW_MT_DATA_ACK) {
		i2c_stop();
		i2c.busy = 0;
		return;
	}

	i2c_restart();

	i2c_address_read(i2c.address);

	i2c_wait();

	if(i2c_status() != TW_MT_SLA_ACK) {
		i2c_stop();
		i2c.busy = 0;
		return;
	}

	while(i2c.data_p < i2c.len) {
		i2c_read_ack();
		i2c_wait();
		i2c.data[i2c.data_p++] = TWDR;
	}

	i2c_stop();

	i2c.busy = 0;
}

void hal_i2c_write_it(uint8_t address, uint8_t * data, uint16_t len, void (*tfr_cplt)(void)) {
	if(i2c.busy) {
		return;
	}

	i2c.busy = 1;

	i2c.data = data;
	i2c.data_p = 0;
	i2c.len = len;
	i2c.address = address;
	i2c.tfr_cplt = tfr_cplt;
	i2c.mode = I2C_WR;

	i2c.state = I2C_START;

	i2c_start();

	TWCR |= (1<<TWIE); //enable tw interrupt

	
}
void hal_i2c_read_it(uint8_t address, uint8_t * data, uint16_t len, void (*tfr_cplt)(void)) {
	if(i2c.busy) {
		return;
	}

	i2c.busy = 1;

	i2c.data = data;
	i2c.data_p = 0;
	i2c.len = len;
	i2c.address = address;
	i2c.tfr_cplt = tfr_cplt;
	i2c.mode = I2C_RD;

	i2c.state = I2C_START;

	i2c_start();

	TWCR |= (1<<TWIE); //enable tw interrupt

	


}

void hal_i2c_reg_write_it(uint8_t address, uint8_t reg, uint8_t * data, uint16_t len, void (*tfr_cplt)(void)) {
	if(i2c.busy) {
		return;
	}

	i2c.busy = 1;

	i2c.data = data;
	i2c.data_p = 0;
	i2c.len = len;
	i2c.address = address;
	i2c.reg = reg;
	i2c.tfr_cplt = tfr_cplt;
	i2c.mode = I2C_WR_REG;

	i2c.state = I2C_START;

	i2c_start();

	TWCR |= (1<<TWIE); //enable tw interrupt
	
}

void hal_i2c_reg_read_it(uint8_t address, uint8_t reg, uint8_t * data, uint16_t len, void (*tfr_cplt)(void)) {
	if(i2c.busy) {
		return;
	}

	i2c.busy = 1;

	i2c.data = data;
	i2c.data_p = 0;
	i2c.len = len;
	i2c.address = address;
	i2c.reg = reg;
	i2c.tfr_cplt = tfr_cplt;
	i2c.mode = I2C_RD_REG;

	i2c.state = I2C_START;

	i2c_start();

	TWCR |= (1<<TWIE); //enable tw interrupt

}



/* hal spi */
void hal_spi_init(uint8_t cpol, uint8_t cpha, uint8_t lsb_first) {

	hal_gpio_init_out(GPIOB, GPIO_PIN2|GPIO_PIN3|GPIO_PIN5);
	hal_gpio_init_in(GPIOB, GPIO_PIN4);

	hal_gpio_set(GPIOB, GPIO_PIN2); //set chip select


	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(SPR1_VALUE<<SPR1)|(SPR0_VALUE<<SPR0);
	SPCR |= ((cpol&0b1)<<CPOL)|((cpha&0b1)<<CPHA)|((lsb_first&0b1)<<DORD);

	SPSR = (SPI2X_VALUE<<SPI2X);

	spi.busy = 0;

	spi.tfr_cplt = NULL;

}

void hal_spi_transfer(uint8_t * data, uint8_t * resp, uint16_t len) {

	if(spi.busy) {
		return;
	}

	spi.busy = 1;
	spi.data = data;
	spi.resp = resp;
	spi.len = len;
	spi.data_p = 0;
	hal_gpio_clr(GPIOB, GPIO_PIN2); //clear chip select

	while(spi.data_p < spi.len) {
		SPDR = spi.data[spi.data_p];

		while(!(SPSR & (1<<SPIF)));

		spi.resp[spi.data_p++] = SPDR;
	}

	hal_gpio_set(GPIOB, GPIO_PIN2); //set chip select

	spi.busy = 0;
}

void hal_spi_transfer_it(uint8_t * data, uint8_t * resp, uint16_t len, void (*tfr_cplt)(void)) {

	if(spi.busy) {
		return;
	}
	spi.busy = 1;
	spi.data = data;
	spi.resp = resp;
	spi.len = len;
	spi.data_p = 0;

	hal_gpio_clr(GPIOB, GPIO_PIN2); //clear chip select

	spi.tfr_cplt = tfr_cplt;

	SPDR = spi.data[spi.data_p];

	SPCR |= (1<<SPIE);

}



/* hal systick */

void hal_systick_init() {

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

hal_systick_t hal_systick_getI(void) {
	return system_tick;
}

void hal_systick_inc(void) {
	system_tick++;
}



/**********************
 *	INTERRUPTS
 **********************/



/* uart */


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


/* i2c */

ISR(TWI_vect) {
	uint8_t status = i2c_status();

	switch(status) {
	case TW_START:
	case TW_RSTART:
		//send device address
		switch(i2c.mode) {
		case I2C_RD:
			i2c_address_read(i2c.address);
			break;
		case I2C_RD_REG:
			if(i2c.state == I2C_REG) {
				i2c_address_write(i2c.address);
			}else{
				i2c_address_read(i2c.address);
			}
			break;
		case I2C_WR:
		case I2C_WR_REG:
			i2c_address_write(i2c.address);
			break;
		}
		break;
	case TW_MT_SLA_ACK:
		switch(i2c.mode) {
		case I2C_RD:
			//read data (wait for wata ?)
			i2c_read_ack();
			break;
		case I2C_RD_REG:
			//read data or write reg
			if(i2c.state == I2C_REG) {
				i2c_write(i2c.reg);
			}else{
				//wait for data??--> arriving in the next cycle
				i2c_read_ack();
			}
			break;
		case I2C_WR:
			i2c_write(i2c.data[i2c.data_p++]);
			break;
		case I2C_WR_REG:
			i2c_write(i2c.reg);
			break;
		}

		break;
	case TW_MT_DATA_ACK:
		switch(i2c.mode) {
		case I2C_RD:
		case I2C_RD_REG:
			i2c_read_ack();
			break;
		case I2C_WR:
		case I2C_WR_REG:
			i2c_write(i2c.data[i2c.data_p++]);
			break;
		}
		break;
	case TW_MT_SLA_NACK:
	case TW_MT_DATA_NACK:
		//error
		break;
	case TW_LOST:
		//arbitration lost, try again?
		//error
		break;
	default://handle an error
		break;
	}

	//check if finished


}

/* spi */

ISR(SPI_STC_vect) {
	spi.resp[spi.data_p++] = SPDR;
	if(spi.data_p >= spi.len) {
		SPCR &= ~(1<<SPIE);
		hal_gpio_set(GPIOB, GPIO_PIN2); //set chip select
		spi.busy = 0;
		if(spi.tfr_cplt) {
			spi.tfr_cplt();
		}
	} else {
		SPDR = spi.data[spi.data_p];
	}
	
}

/* END */

