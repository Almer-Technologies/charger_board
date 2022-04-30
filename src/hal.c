/*  Title		: HAL
 *  Filename	: hal.c
 *	Author		: iacopo sprenger
 *	Date		: 30.04.2022
 *	Version		: 0.1
 *	Description	: Hardware abstraction layer
 */

/**********************
 *	INCLUDES
 **********************/

#include "hal.h"

/**********************
 *	CONSTANTS
 **********************/


/**********************
 *	MACROS
 **********************/


/**********************
 *	TYPEDEFS
 **********************/


/**********************
 *	VARIABLES
 **********************/


/**********************
 *	PROTOTYPES
 **********************/


/**********************
 *	DECLARATIONS
 **********************/



void hal_systick_init(void) {

    TCCR0A = 0b10; //ctc mode

    OCR0A = 249; //250-1
    //--> this gives us an interrupt freq of 1ms


    //using timer0 with prescaler /8
    //also start timer
    TCCR0B = 0b011<<CSO;
    
    TCNT0 = 0;
    

    TIMSK0 = 1<<OCIExA; //enable compare A interrupt
}



/* END */

