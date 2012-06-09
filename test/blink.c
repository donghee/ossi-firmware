#include <msp430.h>
#include <global.h>

#define LED_PORT           1
#define LED_PIN            1

void _blink() {
	volatile unsigned int i;	// volatile to prevent optimization
    IO_SET(LED,TOGGLE);
	i = 10000;					// SW Delay
	do i--;
	while(i != 0);
}

void infinite_blink() {
	IO_DIRECTION(LED, OUTPUT);
	for(;;) {
		_blink();
	}
}

void three_blink() {
	volatile uint8_t i;
	IO_DIRECTION(LED, OUTPUT);
	for(i=0;i<7;i++) {
		_blink();
	}
}
