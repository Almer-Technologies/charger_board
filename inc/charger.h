#ifndef CHARGER_H
#define CHARGER_H

#include <stdint.h>


typedef enum charger_type {
	CT_NONE		= 0x00,
	CT_USB_SDP	= 0x10,
	CT_USB_DCP_2A	= 0x20,
	CT_USB_CDP_1A5	= 0x30,
	CT_DIV1_1A	= 0x40,
	CT_DIV2_2A1	= 0x50,
	CT_DIV3_2A4	= 0x60,
	CT_DIV4_2A	= 0x70,
	CT_UNKNOWN	= 0x80,
	CT_HV_2A	= 0x90,
	CT_DIV5_3A	= 0xA0,
}charger_type_t;

typedef enum charger_status {
	CS_NONE		= 0x00,
	CS_TRICKLE	= 0x20,
	CS_PRE		= 0x40,
	CS_FAST		= 0x60,
	CS_CONST	= 0x80,
	CS_DONE		= 0xA0,
}charger_status_t;



void charger_i2c_write(uint8_t reg, uint8_t data);

void charger_i2c_read(uint8_t reg, uint8_t data);


void charger_init(void);



#endif /* CHARGER_H */
