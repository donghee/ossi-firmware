#include <msp430.h>
#include <global.h>

#define LED_PORT           1
#define LED_PIN            1

void infinite_blink() {
	P1DIR |= BIT1;					// Set P1.0 to output direction
	IO_DIRECTION(LED, OUTPUT);
	for(;;) {
		volatile unsigned int i;	// volatile to prevent optimization
	    IO_SET(LED,TOGGLE);
		i = 10000;					// SW Delay
		do i--;
		while(i != 0);
	}
}

