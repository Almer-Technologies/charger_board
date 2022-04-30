/*  Title		: interrupts
 *  Filename	: interrupts.c
 *	Author		: iacopo sprenger
 *	Date		: 30.04.2022
 *	Version		: 0.1
 *	Description	: interrupt service routines
 */

/**********************
 *	INCLUDES
 **********************/

#include "atmega328p.h"
#include "os.h"

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


void __attribute__((signal)) timer_compa_int(void) {
    DDRB = 32;
    PORTB ^= 32;

}



/* END */

