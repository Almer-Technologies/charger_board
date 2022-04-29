
#include "atmega328p.h"

int main(void) {

	DDRB = 1<<5;
	PORTB = 1<<5;

	for(;;) {

	}
}
