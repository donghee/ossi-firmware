/*
 * i2c.h
 *
 *  Created on: 2012. 5. 24.
 *      Author: donghee
 */

#ifndef I2C_H_
#define I2C_H_

#include  <msp430x16x.h>

void I2C_master_init2();
void I2C_slave_init2(unsigned char address);
void I2C_write_init2();
void I2C_read_init2();

extern unsigned char I2CBuffer2[3];
extern int PtrTransmit2;
extern int PtrReceive2;

#endif /* I2C_H_ */
