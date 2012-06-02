#include  "adf702x.h"

// 7021-n: 433MHz 1200kbps
static const uint32_t adf7021_regs[] = {
	0x095FF380, //r0
	0x00575011, //r1
	0x0077F082, //r2
	0x37160123, //r3
	0x80293814, //r4
	0x00003155, //r5
};

// 7020-1: 288.5/2 = 144.25 MHz
static const uint32_t adf7020_1_regs[] = {
	0x10d0b1f0, //r0
	0x0003b011, //r1
	0x800f00d2, //r2
};

//#define PA_ON 0x5a << 8
#define PA_ON 0x48 << 8  //0dbm

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
		P5OUT &= ~BIT1;                            // SCLK is low _
		if(_register & (1<<i))
			P5OUT |= BIT3;                         // SDATA is high
		else
			P5OUT &= ~BIT3;                        // SDATA is low
		__delay_cycles(5);
		P5OUT |= BIT1;                             // SCLK is high _|
		__delay_cycles(15);
	}
	P5OUT &= ~BIT1;                                // SCLK is low _|x|
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
	P5OUT |= BIT4;                            // high
	__delay_cycles(10);
	P5OUT &= ~BIT4;

	__delay_cycles(30);

}


void adf7020_1_init()
{
	P5DIR |= BIT0 + BIT1 + BIT3 + BIT4 + BIT5;
	P5OUT &= ~BIT0;
}


void adf7020_1_tx()
{
	P5OUT |= BIT5;                            // CE is HIGH
	P5DIR |= BIT0;

	adf702x_write(adf7020_1_regs[0]);
	adf702x_write(adf7020_1_regs[1]);
	adf702x_write(adf7020_1_regs[2]);

	mode = TX;
}

void adf7020_1_poweramp(uint8_t is_pa_on)
{
	P5OUT &= ~BIT0;
	if (is_pa_on==1)
		adf702x_write(adf7020_1_regs[2]|PA_ON);
	else
		adf702x_write(adf7020_1_regs[2]);
}


void adf7020_1_sendStart()
{
	adf7020_1_tx();
	adf7020_1_poweramp(1); // power on
}

void adf7020_1_ook(uint8_t is_high)
{
	if (is_high == 1)
		P5OUT |= BIT0;
	else
		P5OUT &= ~BIT0;
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
