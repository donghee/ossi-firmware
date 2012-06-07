/*
 *
                        .__           ____
   ____   ______  ______|__|         /_   |
  /  _ \ /  ___/ /  ___/|  |  ______  |   |
 (  <_> )\___ \  \___ \ |  | /_____/  |   |
  \____//____  >/____  >|__|          |___|
            \/      \/
  
  _____ _____ ______ 
 |  _  /  __ \| ___ \
 | | | | /  \/| |_/ /
 | | | | |    | ___ \
 \ \_/ / \__/\| |_/ /
  \___/ \____/\____/ 
                     
*  Created on: 2012. 6. 3.
*
*
*/

#include <msp430.h>
#include "24lc256.h"


char message[255] = {'h','e','l','l','o',' ','o','s','s','i',' ', '1'};

void configure_clock() {
	BCSCTL1 |= RSEL0 + RSEL1 + RSEL2;
	DCOCTL = DCO0 + DCO1 + DCO2;
	BCSCTL2 |= SELM_0 + DIVM_0;
}

int main(void) {
	volatile unsigned int i;
    volatile unsigned char x;

	WDTCTL = WDTPW + WDTHOLD;		// Stop watchdog timer

	EEPROM_Init(0x50); // 1010 000 eeprom

	for(i=0; i < 12;i++ ) {
		  ascii(message[i]);
		  x= EEPROM_ByteWrite(i, message[i]);
		  EEPROM_AckPolling();
	}
	infinite_blink();

	  for(i=0; i < 12;i++ ) {
	  	  x = EEPROM_RandomRead(i);
	  	  ascii(x);
	  }
 }

