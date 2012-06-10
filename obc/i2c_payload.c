#include "i2c.h"
#include "config.h"

void I2C_PAYLOAD_Init(uint8_t slave_address)
{
	 _EINT();
	InitI2C(slave_address);
}

void I2C_PAYLOAD_Write(uint8_t value)
{
  uint8_t control_register;
//  while (I2CDCTL&I2CBUSY); // I2C module busy
  while (I2CDCTL&I2CBB); // I2C bus busy. from TI forum http://goo.gl/Gsxsw http://www.ti.com/lit/er/slaz018d/slaz018d.pdf


  I2CBuffer[0] = value;
  PtrTransmit = 0;

  I2CWriteInit();
  I2CNDAT = 1;
  I2CTCTL |= I2CSTT+I2CSTP;   // start and stop condition generation
}
