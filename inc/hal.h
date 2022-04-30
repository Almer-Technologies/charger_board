/*  Title       : HAL
 *  Filename    : hal.h
 *  Author      : iacopo sprenger
 *  Date        : 30.04.2022
 *  Version     : 0.1
 *  Description : Hardware abstraction layer
 */


#ifndef HAL_H
#define HAL_H




/**********************
 *  INCLUDES
 **********************/

#include <stdint.h>

#include "atmega328p.h"
#include "port.h"

/**********************
 *  CONSTANTS
 **********************/


/**********************
 *  MACROS
 **********************/


/**********************
 *  TYPEDEFS
 **********************/

typedef struct hal_timer hal_timer_t;


/**********************
 *  VARIABLES
 **********************/


/**********************
 *  PROTOTYPES
 **********************/

void hal_systick_init(void);



#endif /* HAL_H */

/* END */