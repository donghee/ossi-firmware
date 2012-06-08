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
#include "morse.h"
#include "config.h"
#include "24lc256.h"

char message[255] = {'1','2','3','l','o',' ','o','s','s','i',' ', '1'};

int main(void) {
	volatile unsigned int i;
    volatile unsigned char x;
    volatile uint16_t temp;
    WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer

    EEPROM_Init(EEPROM_ADDR);

	for(i=0; i < 1;i++ ) {
		  ascii(message[i]);
		  EEPROM_ByteWrite(i, message[i]);
		  EEPROM_AckPolling();
	}

	for(i=0; i < 1;i++ ) {
		  x=EEPROM_RandomRead(i);
		  ascii(x);
	}

    TMP10x_Init(OBCTEMP_ADDR);
    temp = TMP10x_Read(); // C = temp / 0x10
    infinite_blink();
 }

