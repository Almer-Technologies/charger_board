/*  Title		: init
 *  Filename	: init.c
 *	Author		: iacopo sprenger
 *	Date		: 29.04.2022
 *	Version		: 0.1
 *	Description	: processor initialization called from asembly setup
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

int main(void) {

	DDRB = 32;
	PORTB = 32;

	//os_system_init();

	//infinite loop lock
	for(;;) {

	}
}



/* END */

