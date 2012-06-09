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
#include "pca9548a.h"

char message[255] = {'1','2','3','l','o',' ','o','s','s','i',' ', '1'};


void test_tmp101(void) {
	// OBC temp sensor
    volatile uint16_t temp;

	TMP10x_Init(OBCTEMP_ADDR);
    temp = TMP10x_Read(); // C = temp / 0x10
}

void test_eeprom(void) {
	volatile unsigned int i;
    volatile unsigned char x;

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
}

void test_panneltemp()
{
    volatile uint16_t temp;
    volatile uint8_t x;

    PCA9548A_Init(0x70);
    PCA9548A_SetChannel(3);

// 	TMP10x_Init(PANELTEMP_ADDR);
//    temp = TMP10x_Read(); // C = temp / 0x10
//    temp = temp+1;
//    if ((temp/16) > 28)
//    	LED_ON();
//    else
//    	LED_OFF();
}

int main(void) {

    WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
    IO_DIRECTION(LED, OUTPUT);
    three_blink();
//    test_eeprom();
//    test_tmp101();
//
    while(1) {
    	test_panneltemp();
    }

    infinite_blink();
}
