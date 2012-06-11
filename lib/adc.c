/*
 * adc.c
 *  Created on: 2012. 6. 3.
 *      Author: donghee
 */

#include "global.h"
#include "adc.h"

#define Num_of_Results 8
static unsigned int results[Num_of_Results];

static uint16_t value;

void adc_init(void)
{
}

uint16_t ADC_Read(uint8_t adcx)
{
	ADC12CTL0 &= ~ENC;
    P6SEL |= 1 << adcx;                             // Enable A/D channel A1
    ADC12CTL0 = ADC12ON+SHT0_8+MSC;           // Turn on ADC12, set sampling time
    ADC12CTL1 = SHP+CONSEQ_2;                 // Use sampling timer, set mode
    ADC12IE = 1 << adcx ;                           // Enable ADC12IFG.0
    ADC12CTL0 |= ENC;                         // Enable conversions
    ADC12CTL0 |= ADC12SC;                     // Start conversion

// TODO: http://jotux.github.com/LaunchLib/docs/html/adc_8c.html#a5ac508fc2087ee41c1458b98f0f4e475
}

void ADG708_Set_Channel(uint8_t channel)
{

  uint8_t a0;
  uint8_t a1;
  uint8_t a2;

  a0 = (channel-1) & 1;
  a1 = ((channel-1)>>1) & 1;
  a2 = ((channel-1)>>2) & 1;
  IO_SET(A0, a0);
  IO_SET(A1, a1);
  IO_SET(A2, a2);
}

uint16_t ADC_Get_Voltage(uint8_t channel)
{
  ADG708_Set_Channel(channel);
  return ADC_Read(1);
}
