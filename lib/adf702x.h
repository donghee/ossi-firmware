/*
 * adf702x.h
 *
 *  Created on: 2012. 5. 5.
 *      Author: donghee
 */

#ifndef RF_H_
#define RF_H_

#include  <msp430x16x.h>

#define uint32_t unsigned long
//#define uint16_t unsigned short
#define uint8_t unsigned char

void adf7020_1_init(void);
void adf7020_1_sendStart(void);
void adf7020_1_ook(uint8_t is_pa_on);

void adf7021n_init(void);
unsigned char adf7021n_get_mode(void);
void adf7021n_sendStart(void);
void adf7021n_recvStart(void);



#define IDLE 0
#define TX 1
#define RX 2

#define RF_MAX 64
#define PREAMBLE_BYTE           0xAA

#define VALID_PREAMBLE_BYTE_1   0x55
#define VALID_PREAMBLE_BYTE_2   0xAA

#define SYNC_WORD1 0xD3                // First byte of sync word
#define SYNC_WORD2 0x91                // Second byte of sync word

#define HEADER_SIZE     4       // 4 bytes header

// Macros for turning on and off the LEDs (for testing only)
#define FALSE 0
#define TRUE (!FALSE)

#define ON  TRUE
#define OFF FALSE

#define adf702x_data       (adf702x_buf + 3)




#endif /* RF_H_ */
