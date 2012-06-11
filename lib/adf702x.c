#include  "adf702x.h"
#include "global.h"

////////// TEST BOARD
//#define TXCLK_PORT 1
//#define TXCLK_PIN 2
//
//#define DATA_PORT 5
//#define DATA_PIN 0
//
//#define SCLK_PORT 5
//#define SCLK_PIN 1
//
//#define SREAD_PORT 5
//#define SREAD_PIN 2
//
//#define SDATA_PORT 5
//#define SDATA_PIN 3
//
//#define SLE_PORT 5
//#define SLE_PIN 4
//
//#define CE_PORT 5
//#define CE_PIN 5
//
//#define PAON_PORT 2
//#define PAON_PIN 7

////////// REAL BOARD
#define TXCLK_PORT 2
#define TXCLK_PIN 0

#define DATA_PORT 2
#define DATA_PIN 1

#define SCLK_PORT 2
#define SCLK_PIN 2

#define SREAD_PORT 2
#define SREAD_PIN 3

#define SDATA_PORT 2
#define SDATA_PIN 4

#define SLE_PORT 2
#define SLE_PIN 5

#define CE_PORT 2
#define CE_PIN 6

#define PAON_PORT 2
#define PAON_PIN 7

// 7021-n: 433MHz 1200kbps
static const uint32_t adf7021_regs[] = {
	0x095FF380, //r0
	0x00575011, //r1
	0x0077F082, //r2
	0x37160123, //r3
	0x80293814, //r4
	0x00003155, //r5
};

// crystal 11.05920MHz
//// 7020-1: 288.5/2 = 144.25 MHz
//static const uint32_t adf7020_1_regs[] = {
//	0x10d0b1f0, //r0
//	0x0002b011, //r1
//	0x800f00d2, //r2
//};

// crystal 19.2MHz
// PFD 6.4MHz
// 7020-1: 288.5/2 = 144.25 MHz
// vco bias 0.625mA
static const uint32_t adf7020_1_regs[] = {
	0x1168a000, //r0
	0x0002b031, //r1
	0x800f00d2, //r2
};


//#define PA_ON (63 << 9)  //-0.25 dbm = -16 + 0.45 * 35
#define PA_ON (10 << 9)  //-0.25 dbm = -16 + 0.45 * 35


unsigned char ShiftReg;

#define bitSet(value, bit) ((value) |= (0x01 << (bit)))
#define bitClear(value, bit) ((value) &= ~(0x01 << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#define setShiftRegLSB(bitValue) (bitWrite(ShiftReg, 0, bitValue ))

char preamble_count;
unsigned char preamble_found;


volatile uint8_t adf702x_buf[RF_MAX] =  {0xAA, 0xAA, 0xAA, 0xAA, SYNC_WORD1, SYNC_WORD2, 0xAA, 0xAA, 0xAA, '1', '2', '3', '4', '5', '6', 0xAA, 0xAA, 0XAA};
volatile uint8_t adf702x_rx_buf[RF_MAX];

static const uint32_t adf7021_tx_reg = 0x01600850;
static unsigned char mode;

int bits_step =7;
int bytes_step = 0;

void spi_write(unsigned char _register)
{
	volatile int i;

	for(i = 7; i >= 0; i--)
	{
//		P5OUT &= ~BIT1;                            // SCLK is low _
		IO_SET(SCLK, LOW);
		if(_register & (1<<i)) {
//			P5OUT |= BIT3;                         // SDATA is high
			IO_SET(SDATA, HIGH);
		}
		else {
//			P5OUT &= ~BIT3;                        // SDATA is low
			IO_SET(SDATA, LOW);
		}
		__delay_cycles(5);
//		P5OUT |= BIT1;                             // SCLK is high _|
		IO_SET(SCLK, HIGH);
		__delay_cycles(15);
	}
//	P5OUT &= ~BIT1;                                // SCLK is low _|x|
	IO_SET(SCLK, LOW);
}

void adf702x_write(uint32_t registers)
{
	volatile int i;
	unsigned char _register;

	for(i=0;i<sizeof(registers);i++)
	{
		_register = (registers >> 24-(i*8)) & 0xff;
		spi_write(_register);
	}
    // SLE
//	P5OUT |= BIT4;                            // SLE high: SLE reset
	IO_SET(SLE, HIGH);
	__delay_cycles(10);
	IO_SET(SDATA, LOW);                       // SDATA low
//	P5OUT &= ~BIT4;                           // low
	IO_SET(SLE, LOW);

	__delay_cycles(30);

}


void adf7020_1_init()
{
//	P5DIR |= BIT0 + BIT1 + BIT3 + BIT4 + BIT5;
	IO_DIRECTION(DATA,OUTPUT);
	IO_DIRECTION(SCLK,OUTPUT);
	IO_DIRECTION(SREAD,OUTPUT);
	IO_DIRECTION(SDATA,OUTPUT);
	IO_DIRECTION(SLE,OUTPUT);
	IO_DIRECTION(CE,OUTPUT);
//	P5OUT &= ~BIT0;

	// SCLK and SDATA pin must be LOW from start.
	IO_SET(DATA, LOW);
	IO_SET(SCLK, LOW);
	IO_SET(SDATA, LOW);
	IO_SET(SLE, LOW);

}


void adf7020_1_tx()
{
//	P5OUT |= BIT5;                            // CE is HIGH
	IO_SET(CE, HIGH);
//	P5DIR |= BIT0;
	IO_DIRECTION(DATA,OUTPUT);
	__delay_cycles(1000);

	adf702x_write(adf7020_1_regs[1]); // once again for stable operation.
	__delay_cycles(4000);
	adf702x_write(adf7020_1_regs[0]);
	__delay_cycles(400);
	adf702x_write(adf7020_1_regs[2]);

	mode = TX;
}

void adf7020_1_poweramp(uint8_t is_pa_on)
{
//	P5OUT &= ~BIT0;
	IO_SET(DATA, LOW);

	if (is_pa_on==1) {
		adf702x_write(adf7020_1_regs[2]|PA_ON);
//		IO_SET(PAON, HIGH);
	}
	else {
		adf702x_write(adf7020_1_regs[2]);
//		IO_SET(PAON, LOW);
	}
}


void adf7020_1_sendStart()
{
	adf7020_1_tx();
	__delay_cycles(1000);
	adf7020_1_poweramp(1); // power on
	__delay_cycles(1000);
}

void adf7020_1_ook(uint8_t is_high)
{
	if (is_high == 1) {
//		P5OUT |= BIT0;
		IO_SET(DATA, HIGH);
	}
	else {
//		P5OUT &= ~BIT0;
		IO_SET(DATA, LOW);
	}
	mode = TX;
}

void adf7021n_rx()
{
	P5OUT |= BIT5;                            // CE is HIGH
    P5DIR &= ~BIT0;

	// fill adf702x_rx_buf

 	// r4 for 1200 bps
	adf702x_write(adf7021_regs[1]);
	adf702x_write(adf7021_regs[3]);
	adf702x_write(adf7021_regs[5]);
	adf702x_write(adf7021_regs[0]);
	adf702x_write(adf7021_regs[4]);

    //mode = RX;
    mode = IDLE;
}

void adf7021n_tx()
{
	P5OUT |= BIT5;                            // CE is HIGH
	P5DIR |= BIT0;

	// fill adf702x_tx_buf

	adf702x_write(adf7021_regs[1]);
	adf702x_write(adf7021_regs[3]);
	adf702x_write(adf7021_tx_reg);
	adf702x_write(adf7021_regs[2]);
	adf702x_write(adf7021_regs[4]);

	mode = TX;
}

unsigned char adf7021n_get_mode()
{
	return mode;
}

void adf7021n_enable_data_interrupt()
{
	  P1OUT = BIT2; //pull up
	  P1IE |= BIT2; // interrupt enable
	  P1IES |= BIT2; // interrupt hi/lo falling edge
	  //P1IES &= ~BIT2; // interrupt lo/hi edge
	  P1IFG &= ~BIT2; // P1.2 IFG cleared just in case
}


void adf7021n_recvStart()
{
	adf7021n_enable_data_interrupt();
	adf7021n_rx();
}

void adf7021n_sendStart()
{
	adf7021n_enable_data_interrupt();
	adf7021n_tx();
}

void adf7021n_init()
{
	P5DIR |= BIT0 + BIT1 + BIT3 + BIT4 + BIT5;

	// SCLK and SDATA pin must be LOW from start.
	IO_SET(DATA, LOW);
	IO_SET(SCLK, LOW);
	IO_SET(SDATA, LOW);
	IO_SET(SLE, LOW);

	P5OUT |= BIT5;                            // CE is HIGH
}


// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void adf7021n_Data_TxRx_handler(void)
{

	switch (mode) {
		case TX:
			if(adf702x_buf[bytes_step] & (1<<bits_step)) {
				P5OUT |= BIT0;
			} else {
				P5OUT &= ~BIT0;
			}

			bits_step--;

			if (bits_step < 0){bits_step = 7;bytes_step++;};
			if (bytes_step >= 15){bytes_step = 0;bits_step=7;};
			break;
		case RX:
			ShiftReg = ShiftReg << 1;
			setShiftRegLSB(P5IN & BIT0);
		    bits_step++;

		    if (bits_step == 8) {
		    	bits_step = 0;
		    	switch(bytes_step){
		    		case 0:
		    			if (ShiftReg != SYNC_WORD2) {
		    				mode = IDLE;
		    			}
		    			break;
		    		case 1:
		    			// header 0
		    			break;
		    		case 2:
		    			// header 1
		    			break;
		    		case 3:
		    			// byte size
		    			break;
		    		default:
		    			P6OUT ^= BIT0;
		    			adf702x_rx_buf[bytes_step-4] = ShiftReg;
		    	}
		        if(bytes_step > 4+6){ // 6 is data length
		          mode = IDLE;
		          // CE LOW?
		        }
		    	bytes_step++;
		    }
		    break;

		case IDLE:
			ShiftReg=ShiftReg<<1;
			setShiftRegLSB(P5IN & BIT0);

			if (preamble_found == 1) {
				if (ShiftReg == SYNC_WORD1) {
					bits_step=0;
					bytes_step=0;
					preamble_count=0;
					preamble_found=0;
					mode = RX;
				}
				break;
			}

			if ((ShiftReg == VALID_PREAMBLE_BYTE_1) || (ShiftReg == VALID_PREAMBLE_BYTE_2)) {
				preamble_count++;
			} else {
				preamble_count = 0;
				preamble_found = 0; //false
			}

			if (preamble_count >= 16) {
				preamble_found = 1; //false
			}
			break;
	}
	P1IFG &= ~BIT2; // P1.2 IFG cleared
}
