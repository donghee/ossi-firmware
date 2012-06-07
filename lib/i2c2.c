#include "i2c.h"

unsigned char I2CBuffer2[3];
int PtrTransmit2;
int PtrReceive2;

void I2C_master_init2(void)
{
	  P3SEL |= 0x0A;                            // Select I2C pins
	  U0CTL |= I2C + SYNC;                      // Switch UI2CSART0 to I2C mode
	  U0CTL &= ~I2CEN;                          // Recommended init procedure
	  I2CTCTL = I2CTRX+I2CSSEL_2;   // byte mode, repeat mode, clock source = SMCLK,
	                                // transmit mode
	  I2CSA = 0x50;         // define Slave Address
	  U0CTL |= I2CEN;                           // Enable I2C
}

void I2C_slave_init2(unsigned char address)
{
	  P3SEL |= 0x0A;                            // Select I2C pins
	  U0CTL |= I2C + SYNC;                      // Switch UI2CSART0 to I2C mode
	  U0CTL &= ~I2CEN;                          // Recommended init procedure
	  I2CTCTL |= I2CSSEL1;                      // SMCLK
	  //I2COA = 0x0048;                           // Own Address is 048h
	  I2COA = address;
	  U0CTL |= I2CEN;                           // Enable I2C
}


void I2C_write_init2(void)
// Description:
//   Initialization of the I2C Module for Write operation.
{
  U0CTL |= MST;           // define Master Mode
  I2CTCTL |= I2CTRX;      // I2CTRX=1 => Transmit Mode (R/W bit = 0)
  I2CIFG &= ~TXRDYIFG;
  I2CIE = TXRDYIE;        // enable Transmit ready interrupt
}

/*---------------------------------------------------------------------------*/
void I2C_read_init2(void)
// Description:
//   Initialization of the I2C Module for Read operation.
{
  I2CTCTL &= ~I2CTRX;     // I2CTRX=0 => Receive Mode (R/W bit = 1)
  I2CIE = RXRDYIE;        // enable Receive ready interrupt
  PtrReceive2 = 0;
}

////// I2C Interrupt Vector (I2CIV) handler
//#pragma vector=USART0TX_VECTOR
//__interrupt void USART0 (void)
//{
//  switch( I2CIV )
//  {
//  case 10:
//  {
//      I2CBuffer2[PtrReceive2]=I2CDRB;   // store received data in buffer
//      PtrReceive2++;
//      if (PtrReceive2 > 1)
//      {
//    	  P6OUT ^= BIT0;
//    	  PtrReceive2 = 0;
//      }
//      break;
//  }
//   case 12:
//   {
//    I2CDRB = I2CBuffer2[PtrTransmit2];
//    PtrTransmit2 = PtrTransmit2-1;
//    if (PtrTransmit2 <0)
//    {
//    	I2CIE &= ~TXRDYIE;        // disable interrupts
//    }
//    break;
//    }
//  }
//}
