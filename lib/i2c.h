/*
 * i2c.h
 *
 *  Created on: 2012. 5. 24.
 *      Author: donghee
 */

#ifndef I2C_H_
#define I2C_H_

#include  <msp430x16x.h>

void InitI2C();
void I2CWriteInit();
void I2CReadInit();

extern unsigned char I2CBuffer[3];
extern int PtrTransmit;


#endif /* I2C_H_ */
