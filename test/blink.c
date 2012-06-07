#include <msp430.h>
#include <global.h>

#define LED_PORT           1
#define LED_PIN            1

#define LED_ON()           IO_SET(LED,HIGH);
#define LED_OFF()           IO_SET(LED,LOW);

void infinite_blink() {
	P1DIR |= BIT1;					// Set P1.0 to output direction
	IO_DIRECTION(LED, OUTPUT);
	for(;;) {
		volatile unsigned int i;	// volatile to prevent optimization
//	    P1OUT ^= BIT1;
	    IO_SET(LED, TOGGLE);
		i = 10000;					// SW Delay
		do i--;
		while(i != 0);
	}
}

