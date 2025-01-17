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

#define SPI_FREQUENCY 500000

#define SPI_MIN_FREQUENCY 0

#ifndef F_CPU
    #define F_CPU 8000000UL
    #warning "F_CPU not defined! Assuming 8MHz."
#endif


/* i2c freq */

#if (F_CPU/I2C_FREQUENCY - 16) / (1) >= 0 \
        && (F_CPU/I2C_FREQUENCY - 16) / (1) <= 0xFF
    #define TWI_PRESCALER 1
    #define TWPS0_VALUE 0
    #define TWPS1_VALUE 0
#elif (F_CPU/I2C_FREQUENCY - 16) / (4) >= 0 \
        && (F_CPU/I2C_FREQUENCY - 16) / (4) <= 0xFF
    #define TWI_PRESCALER 4
    #define TWPS0_VALUE 1
    #define TWPS1_VALUE 0
#elif (F_CPU/I2C_FREQUENCY - 16) / (16) >= 0 \
        && (F_CPU/I2C_FREQUENCY - 16) / (16) <= 0xFF
    #define TWI_PRESCALER 16
    #define TWPS0_VALUE 0
    #define TWPS1_VALUE 1
#elif (F_CPU/I2C_FREQUENCY - 16) / (64) >= 0 \
        && (F_CPU/I2C_FREQUENCY - 16) / (64) <= 0xFF
    #define TWI_PRESCALER 64
    #define TWPS0_VALUE 1
    #define TWPS1_VALUE 1
#else
    #error "I2C_FREQUENCY too high!"
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

#define i2c_start(it) 	\
        TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | ((it) << TWIE)

#define i2c_restart(it) 	\
        TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | ((it) << TWIE)

#define i2c_address_write(address, it) 	\
        TWDR = ((address)<<1) & ~0x01;	\
        TWCR = (1 << TWINT) | (1 << TWEN) | ((it) << TWIE)

#define i2c_address_read(address, it) 	\
        TWDR = ((address)<<1) | 0x01;	\
        TWCR = (1 << TWINT) | (1 << TWEN) | ((it) << TWIE)

#define i2c_write(data, it) 	\
        TWDR = data;		\
    	TWCR = (1 << TWINT) | (1 << TWEN) | ((it) << TWIE)

#define i2c_read_ack(it)		\
    	TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | ((it) << TWIE)

#define i2c_read_nack(it)		\
    	TWCR = (1 << TWINT) | (1 << TWEN) | ((it) << TWIE)

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
	uint8_t reg;
	uint8_t use_reg;
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
	void (*isr_mode)(uint8_t);
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

void hal_i2c_write_isr(uint8_t status);
void hal_i2c_read_isr(uint8_t status);
void hal_i2c_reg_write_isr(uint8_t status);
void hal_i2c_reg_read_isr(uint8_t status);


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

	hal_gpio_init_in(GPIOD, GPIO_PIN0);
	hal_gpio_init_out(GPIOD, GPIO_PIN1);

	uart.tx_busy = 0;
	uart.rx_busy = 0;

	uint16_t ubrr = 103; //9600 baud

	UBRR0L = (uint8_t) ubrr;
	UBRR0H = (uint8_t) ubrr>>8;

	UCSR0A = (1<<U2X0);


	//enable rx and/or tx
	UCSR0B = (1<<TXEN0) | (1<<RXEN0);

	//set frame format 8 data 1 stop no parity
	UCSR0C = (0b11<<UCSZ00);
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

	i2c.isr_mode = NULL;

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

	i2c_start(0);

	i2c_wait();

	if(i2c_status() != TW_START) {
		i2c.busy = 0;
		return;
	}

	i2c_address_write(i2c.address, 0);

	i2c_wait();

	if(i2c_status() != TW_MT_SLAW_ACK) {
		i2c_stop();
		i2c.busy = 0;
		return;
	}


	while(i2c.data_p < i2c.len) {
		i2c_write(i2c.data[i2c.data_p++], 0);
		i2c_wait();

		if(i2c_status() != TW_MT_DATAW_ACK) {
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

	i2c_start(0);

	i2c_wait();

	if(i2c_status() != TW_START) {
		i2c.busy = 0;
		return;
	}

	i2c_address_read(i2c.address, 0);

	i2c_wait();

	if(i2c_status() != TW_MT_SLAR_ACK) {
		i2c_stop();
		i2c.busy = 0;
		return;
	}

	while(i2c.data_p < i2c.len-1) {
		i2c_read_ack(0);
		i2c_wait();
		if(i2c_status() != TW_MT_DATAR_ACK) {
			i2c_stop();
			i2c.busy = 0;
			return;
		}
		i2c.data[i2c.data_p++] = TWDR;
	}
	//last byte without ack
	i2c_read_nack(0);
	i2c_wait();
	if(i2c_status() != TW_MT_DATAR_NACK) {
		i2c_stop();
		i2c.busy = 0;
		return;
	}
	i2c.data[i2c.data_p++] = TWDR;

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

	i2c_start(0);

	i2c_wait();

	if(i2c_status() != TW_START) {
		i2c.busy = 0;
		return;
	}

	i2c_address_write(i2c.address, 0);

	i2c_wait();

	if(i2c_status() != TW_MT_SLAW_ACK) {
		i2c_stop();
		i2c.busy = 0;
		return;
	}

	i2c_write(i2c.reg, 0);

	i2c_wait();

	if(i2c_status() != TW_MT_DATAW_ACK) {
		i2c_stop();
		i2c.busy = 0;
		return;
	}

	while(i2c.data_p < i2c.len) {
		i2c_write(i2c.data[i2c.data_p++], 0);
		i2c_wait();

		if(i2c_status() != TW_MT_DATAW_ACK) {
			i2c_stop();
			i2c.busy = 0;
			return;
		}
	}

	i2c_stop();

	i2c.busy = 0;
}

uint8_t hal_i2c_reg_read(uint8_t address, uint8_t reg, uint8_t * data, uint16_t len) {
	if(i2c.busy) {
		return 1;
	}

	i2c.busy = 1;

	i2c.data = data;
	i2c.data_p = 0;
	i2c.len = len;
	i2c.address = address;
	i2c.reg = reg;

	i2c_start(0);

	i2c_wait();

	if(i2c_status() != TW_START) {
		i2c.busy = 0;
		return 1;
	}

	i2c_address_write(i2c.address, 0);

	i2c_wait();

	if(i2c_status() != TW_MT_SLAW_ACK) {
		i2c_stop();
		i2c.busy = 0;
		return 1;
	}

	i2c_write(i2c.reg, 0);

	i2c_wait();

	if(i2c_status() != TW_MT_DATAW_ACK) {
		i2c_stop();
		i2c.busy = 0;
		return 1;
	}

	i2c_restart(0);

	i2c_wait();

	if(i2c_status() != TW_RSTART) {
		i2c_stop();
		i2c.busy = 0;
		return 1;
	}

	i2c_address_read(i2c.address, 0);

	i2c_wait();

	if(i2c_status() != TW_MT_SLAR_ACK) {
		i2c_stop();
		i2c.busy = 0;
		return 1;
	}

	while(i2c.data_p < i2c.len-1) {
		i2c_read_ack(0);
		i2c_wait();
		if(i2c_status() != TW_MT_DATAR_ACK) {
			i2c_stop();
			i2c.busy = 0;
			return 1;
		}
		i2c.data[i2c.data_p++] = TWDR;
	}
	//last byte without ack
	i2c_read_nack(0);
	i2c_wait();
	if(i2c_status() != TW_MT_DATAR_NACK) {
		i2c_stop();
		i2c.busy = 0;
		return 1;
	}
	i2c.data[i2c.data_p++] = TWDR;


	i2c_stop();

	i2c.busy = 0;
	return 0;
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
	i2c.isr_mode = hal_i2c_write_isr;

	i2c_start(1);

	
}

void hal_i2c_write_isr(uint8_t status) {
	switch(status) {
	case TW_START:
	case TW_RSTART:
		i2c_address_write(i2c.address, 1);
		break;
	case TW_MT_SLAW_ACK:
	case TW_MT_DATAW_ACK:
		if(i2c.data_p >= i2c.len) {
			i2c_stop();
			i2c.busy = 0;
			if(i2c.tfr_cplt) {
				i2c.tfr_cplt();
			}
			return;
		}
		i2c_write(i2c.data[i2c.data_p++], 1);
		break;
	case TW_MT_SLAW_NACK:
	case TW_MT_DATAW_NACK:
	case TW_LOST:
		//maybe retry?
	default:
		i2c_stop();
		i2c.busy=0;
		break;
	}
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
	i2c.isr_mode = hal_i2c_read_isr;

	i2c_start(1);
}

void hal_i2c_read_isr(uint8_t status) {
	switch(status) {
	case TW_START:
	case TW_RSTART:
		i2c_address_read(i2c.address, 1);
		break;
	case TW_MT_SLAR_ACK:
		//request data
		i2c_read_ack(1);
		break;
	case TW_MT_DATAR_ACK:
		//get new data
		i2c.data[i2c.data_p++] = TWDR;
		//if enough, stop
		if(i2c.data_p < i2c.len-1) {
			//request new data
			i2c_read_ack(1);
		} else {
			//request new data last
			i2c_read_nack(1);
		}
		break;
	case TW_MT_DATAR_NACK:
		//last data
		i2c.data[i2c.data_p++] = TWDR;
		i2c_stop();
		i2c.busy = 0;
		if(i2c.tfr_cplt) {
			i2c.tfr_cplt();
		}
		break;
	case TW_MT_SLAR_NACK:
	case TW_LOST:
		//maybe retry?
	default:
		i2c_stop();
		i2c.busy=0;
		break;
	}
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

	i2c.isr_mode = hal_i2c_reg_write_isr;
	i2c_start(1);

}

void hal_i2c_reg_write_isr(uint8_t status) {
	switch(status) {
	case TW_START:
	case TW_RSTART:
		i2c_address_write(i2c.address, 1);
		break;
	case TW_MT_SLAW_ACK:
		//write register
		i2c_write(i2c.reg, 1);
		break;
	case TW_MT_DATAW_ACK:
		//if enough, stop
		if(i2c.data_p >= i2c.len) {
			i2c_stop();
			i2c.busy = 0;
			if(i2c.tfr_cplt) {
				i2c.tfr_cplt();
			}
			return;
		}
		//write data
		i2c_write(i2c.data[i2c.data_p++], 1);
		break;
	case TW_MT_SLAR_NACK:
	case TW_MT_DATAR_NACK:
	case TW_LOST:
		//maybe retry?
	default:
		i2c_stop();
		i2c.busy=0;
		break;
	}
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

	i2c.isr_mode = hal_i2c_reg_read_isr;

	i2c_start(1);

}

void hal_i2c_reg_read_isr(uint8_t status) {
	switch(status) {
	case TW_START:
		i2c_address_write(i2c.address, 1);
		break;
	case TW_MT_SLAW_ACK:
		//write register
		i2c_write(i2c.reg, 1);
		break;
	case TW_MT_DATAW_ACK:
		i2c_restart(1);
		break;
	case TW_RSTART:
		i2c_address_read(i2c.address, 1);
		break;
	case TW_MT_SLAR_ACK:
		//request data
		if(i2c.data_p < i2c.len-1) {
			//request new data
			i2c_read_ack(1);
		} else {
			//request new data last
			i2c_read_nack(1);
		}
		break;
	case TW_MT_DATAR_ACK:
		//get new data
		i2c.data[i2c.data_p++] = TWDR;
		//if enough, stop
		if(i2c.data_p < i2c.len-1) {
			//request new data
			i2c_read_ack(1);
		} else {
			//request new data last
			i2c_read_nack(1);
		}
		break;
	case TW_MT_DATAR_NACK:
		//last data
		i2c.data[i2c.data_p++] = TWDR;
		i2c_stop();
		i2c.busy = 0;
		if(i2c.tfr_cplt) {
			i2c.tfr_cplt();
		}
		break;
	case TW_MT_SLAW_NACK:
	case TW_MT_DATAW_NACK:
	case TW_MT_SLAR_NACK:
	case TW_LOST:
		//maybe retry?
	default:
		i2c_stop();
		i2c.busy=0;
		break;
	}
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
	cli();
	hal_gpio_clr(GPIOB, GPIO_PIN2); //clear chip select
	sei();
	while(spi.data_p < spi.len) {
		SPDR = spi.data[spi.data_p];

		while(!(SPSR & (1<<SPIF)));

		spi.resp[spi.data_p++] = SPDR;
	}

	cli();
	hal_gpio_set(GPIOB, GPIO_PIN2); //set chip select
	sei();
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

void hal_spi_reg_write(uint8_t addr, uint8_t * data, uint16_t len) {

	if(spi.busy) {
		return;
	}

	spi.busy = 1;
	spi.data = data;
	spi.resp = NULL;
	spi.reg = addr;
	spi.len = len;
	spi.data_p = 0;
	cli();
	hal_gpio_clr(GPIOB, GPIO_PIN2); //clear chip select
	sei();

	SPDR = spi.reg;
	while(!(SPSR & (1<<SPIF)));

	while(spi.data_p < spi.len) {
		SPDR = spi.data[spi.data_p];

		while(!(SPSR & (1<<SPIF)));

		spi.resp[spi.data_p++] = SPDR;
	}

	cli();
	hal_gpio_set(GPIOB, GPIO_PIN2); //set chip select
	sei();
	spi.busy = 0;
}

void hal_spi_reg_read(uint8_t addr, uint8_t * data, uint16_t len) {

	if(spi.busy) {
		return;
	}

	spi.busy = 1;
	spi.resp = data;
	spi.data = NULL;
	spi.reg = addr;
	spi.len = len;
	spi.data_p = 0;
	cli();
	hal_gpio_clr(GPIOB, GPIO_PIN2); //clear chip select
	sei();

	SPDR = spi.reg;
	while(!(SPSR & (1<<SPIF)));

	while(spi.data_p < spi.len) {
		SPDR = 0;

		while(!(SPSR & (1<<SPIF)));

		spi.resp[spi.data_p++] = SPDR;
	}

	cli();
	hal_gpio_set(GPIOB, GPIO_PIN2); //set chip select
	sei();

	spi.busy = 0;
}

void hal_spi_reg_write_it(uint8_t addr, uint8_t * data, uint16_t len, void (*tfr_cplt)(void)) {

	if(spi.busy) {
		return;
	}
	spi.busy = 1;
	spi.data = data;
	spi.resp = NULL;
	spi.len = len;
	spi.data_p = 0;
	spi.reg = addr;

	hal_gpio_clr(GPIOB, GPIO_PIN2); //clear chip select

	spi.tfr_cplt = tfr_cplt;

	SPDR = spi.reg;

	SPCR |= (1<<SPIE);

}

void hal_spi_reg_read_it(uint8_t addr, uint8_t * data, uint16_t len, void (*tfr_cplt)(void)) {

	if(spi.busy) {
		return;
	}
	spi.busy = 1;
	spi.data = NULL;
	spi.resp = data;
	spi.len = len;
	spi.data_p = 0;
	spi.reg = addr;

	hal_gpio_clr(GPIOB, GPIO_PIN2); //clear chip select

	spi.tfr_cplt = tfr_cplt;

	SPDR = spi.reg;

	SPCR |= (1<<SPIE);

}




/* hal pwm */


#define MAX_LEDS 16

static struct  {
	uint8_t * port;
	uint8_t pin;
	hal_led_brightness_t step;
} hal_led_data[MAX_LEDS] = {0};

static uint8_t hal_led_count;

void hal_led_init(void) {

	hal_led_count = 0;

	TCCR1A = 0b10; //ctc mode

	ICR1 = 1499; //max

	OCR1A = 499; //dim

	OCR1B = 999; //bright

	//using timer0 with prescaler /1
	//also start timer
	TCCR1B = 0b001<<CSO;

	TCNT1 = 0;

	TIMSK1 = (1<<OCIExB) | (1<<OCIExA) | (1<<TOIE1); //enable cmp A+B and ovf interrupt

}

uint8_t hal_led_attach(uint8_t * port, uint8_t pin) {
	if(hal_led_count < MAX_LEDS) {
		hal_gpio_init_out(port, pin);
		hal_gpio_set(port, pin);
		hal_led_data[hal_led_count].port = port;
		hal_led_data[hal_led_count].pin = pin;
		hal_led_data[hal_led_count].step = LED_OFF;
		hal_led_count++;
		return hal_led_count;
	} else {
		return 0;
	}
}
void hal_led_set_brightness(uint8_t channel, hal_led_brightness_t step) {
	if(channel <= hal_led_count && channel > 0) {
		hal_led_data[channel-1].step = step;
	}
}


ISR(TIMER1_COMPA_vect) {
	for(uint8_t i = 0; i < hal_led_count; i++) {
		if(hal_led_data[i].step == LED_HIGH) {
			hal_gpio_set(hal_led_data[i].port, hal_led_data[i].pin);
		}
	}
}

ISR(TIMER1_COMPB_vect) {
	for(uint8_t i = 0; i < hal_led_count; i++) {
		if(hal_led_data[i].step == LED_LOW) {
			hal_gpio_set(hal_led_data[i].port, hal_led_data[i].pin);
		}
	}
}

ISR(TIMER1_OVF_vect) {
	for(uint8_t i = 0; i < hal_led_count; i++) {
		if(hal_led_data[i].step == LED_OFF) {
			hal_gpio_set(hal_led_data[i].port, hal_led_data[i].pin);
		} else {
			hal_gpio_clr(hal_led_data[i].port, hal_led_data[i].pin);
		}
	}
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

	if(i2c.isr_mode) {
		i2c.isr_mode(status);
	}

}

/* spi */

ISR(SPI_STC_vect) {
	if(spi.resp) {
		spi.resp[spi.data_p++] = SPDR;
	} else {
		spi.data_p++;
	}
	if(spi.data_p >= spi.len) {
		SPCR &= ~(1<<SPIE);
		hal_gpio_set(GPIOB, GPIO_PIN2); //set chip select
		spi.busy = 0;
		if(spi.tfr_cplt) {
			spi.tfr_cplt();
		}
	} else {
		if(spi.data) {
			SPDR = spi.data[spi.data_p];
		} else {
			SPDR = 0;
		}
	}
	
}

/* END */

