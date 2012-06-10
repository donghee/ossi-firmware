/*
 *
                        .__           ____
   ____   ______  ______|__|         /_   |
  /  _ \ /  ___/ /  ___/|  |  ______  |   |
 (  <_> )\___ \  \___ \ |  | /_____/  |   |
  \____//____  >/____  >|__|          |___|
            \/      \/
 
 ______           _                 _ 
 | ___ \         | |               | |
 | |_/ /_ _ _   _| | ___   __ _  __| |
 |  __/ _` | | | | |/ _ \ / _` |/ _` |
 | | | (_| | |_| | | (_) | (_| | (_| |
 \_|  \__,_|\__, |_|\___/ \__,_|\__,_|
             __/ |                    
            |___/                     

*  Created on: 2012. 6. 3.
*
*
*/

#include <msp430.h>				
#include "morse.h"

char RXData = 0;

int I2C_Slave_Init(uint16_t address)
{
	  P3SEL |= 0x0A;                            // Select I2C pins
	  U0CTL |= I2C + SYNC;                      // Recommended init procedure
	  U0CTL &= ~I2CEN;                          // Recommended init procedure
	  I2CTCTL |= I2CSSEL1;                      // SMCLK
	  I2COA = 0x0048;                           // Own Address is 048h
	  I2CIE = RXRDYIE;                          // Enable RXRDYIFG interrupt
	  U0CTL |= I2CEN;                           // Enable I2C
}

int main(void) {
	volatile unsigned int i;
	WDTCTL = WDTPW + WDTHOLD;		// Stop watchdog timer
	P1DIR |= BIT1;					// Set P1.0 to output direction
	_EINT();
	I2C_Slave_Init(0x0048);
//	 _BIS_SR(LPM0_bits + GIE);                 // Enter LPM0 w/ interrupt

	while(1)
	{
//		IO_SET(LED,TOGGLE);
//		__delay_cycles(100000);
	}
}


// Common ISR for I2C Module
#pragma vector=USART0TX_VECTOR
__interrupt void I2C_ISR(void)
{
 switch( I2CIV )
 {
   case  2: break;                          // Arbitration lost
   case  4: break;                          // No Acknowledge
   case  6: break;                          // Own Address
   case  8: break;                          // Register Access Ready
   case 10:                                 // Receive Ready
      RXData = I2CDRB;                      // RX data
      if (RXData > 0) {
    	  LED_ON();
      }else {
    	  LED_OFF();
      }
     _BIC_SR_IRQ(CPUOFF);                   // Clear LPM0
     break;
   case 12:  break;                         // Transmit Ready
   case 14: break;                          // General Call
   case 16: break;                          // Start Condition
 }
}
