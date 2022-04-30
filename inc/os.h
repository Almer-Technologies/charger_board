/*  Title       : os
 *  Filename    : os.h
 *  Author      : iacopo sprenger
 *  Date        : 29.04.2022
 *  Version     : 0.1
 *  Description : operating system definitions
 */

#ifndef OS_H
#define OS_H



/**********************
 *  INCLUDES
 **********************/

#include <stdint.h>

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

typedef uint8_t os_priority_t;

typedef struct os_thread os_thread_t;


/**********************
 *  VARIABLES
 **********************/


/**********************
 *  PROTOTYPES
 **********************/

void os_system_init(void);


#endif /* OS_H */

/* END */