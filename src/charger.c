#include <charger.h>
#include <os.h>
#include <hal.h>




#define CHARGER_ADDR            0x3F

#define CHARGER_TYPE_MASK       0xF0
#define CHARGER_STATUS_MASK     0xE0



static os_event_t i2c_event;

static charger_type_t charger_last_type;
static charger_type_t charger_last_status;
static uint8_t charger_using_hv = 0;

/* this will be called from ISR */
void i2c_done(void) {
	os_event_signal(&i2c_event);
}


void charger_i2c_write(uint8_t reg, uint8_t data) {
        static uint8_t _data;
        _data = data;
        hal_i2c_reg_write_it(CHARGER_ADDR, reg, &_data, 1, i2c_done);
        os_event_wait(&i2c_event);
}

uint8_t charger_i2c_read(uint8_t reg) {
        static uint8_t _data;
        hal_i2c_reg_read_it(CHARGER_ADDR, reg, &_data, 1, i2c_done);
        os_event_wait(&i2c_event);
        return _data;
}


void charger_init(void) {
        /* We initialize as TAKEN because this will only serve a signal 
	   and never as mutex */
	os_event_create(&i2c_event, OS_TAKEN);


        /* disable OVP */
        charger_i2c_write(0x06, 0b11100100);

        /* set trshld for fast charge to 2.4V and 5A max */
        charger_i2c_write(0x02, 0b00111111);

        /* 680mA precharge current - 480mA termination current */
        charger_i2c_write(0x03, 0b11111111);

        /* stop charging after 45 min - battreg voltage set to 4.6V */
        charger_i2c_write(0x05, 0b11101000);

}


charger_type_t charger_get_type(void) {
        charger_last_type = charger_i2c_read(0x11) & CHARGER_TYPE_MASK;

        if(charger_last_type == CT_HV_2A && charger_using_hv == 0) {
                /* set HV to 12.3V */
                charger_i2c_write(0x0B, 0b00010010);
                charger_using_hv = 1;
        } else if(charger_last_type != CT_HV_2A && charger_using_hv == 1) {
                /* disable HV to 5V */
                charger_i2c_write(0x0B, 0b00010000);
                charger_using_hv = 0;
        }

        return charger_last_type;
}

charger_status_t charger_get_status(void) {
        charger_last_status = charger_i2c_read(0x13) & CHARGER_STATUS_MASK;

        return charger_last_status;
}