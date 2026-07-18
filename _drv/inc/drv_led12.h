
// ****************************************************************************
//
//                           BarePi LED12 display
//
// ****************************************************************************

#if USE_LED12		// 1=use BarePi LED12 display, 0=not used (drv_led12.*)

#ifndef _DRV_LED12_H
#define _DRV_LED12_H

#ifndef LED12_I2C
#define LED12_I2C		0	// LED12 I2C bus
#endif

#ifndef LED12_ADDR
#define LED12_ADDR		0x32	// LED12 I2C address
#endif

#ifndef LED12_SPEED
#define LED12_SPEED		100000	// LED12 I2C speed
#endif

#define LED12_RAM_SEG	14	// segments register index
#define LED12_DIG_NUM	12	// number of digits

#if (LED12_I2C != 1) && (!USE_I2CBUS || !USE_I2C0BUS)
#error "LED12 requires I2C0 bus driver"
#endif

#if (LED12_I2C != 0) && (!USE_I2CBUS || !USE_I2C1BUS)
#error "LED12 requires I2C1 bus driver"
#endif

// display segments (the segment must be set to 0 to light up)
//  +--SA--+
//  |      |
// SF     SB
//  |      |
//  +--SG--+
//  |      |
// SE     SC
//  |      |
//  +--SD--+  SP

#define LED12_SEGA	B0	// PD0
#define LED12_SEGB	B1	// PD1
#define LED12_SEGC	B2	// PD2
#define LED12_SEGD	B3	// PD3
#define LED12_SEGE	B4	// PD4
#define LED12_SEGF	B5	// PD5
#define LED12_SEGG	B6	// PD6
#define LED12_SEGP	B7	// PD7

// digits and characters
// - digits
#define LED12CH_0	0	// "0"
#define LED12CH_1	1	// "1"
#define LED12CH_2	2	// "2"
#define LED12CH_3	3	// "3"
#define LED12CH_4	4	// "4"
#define LED12CH_5	5	// "5"
#define LED12CH_6	6	// "6"
#define LED12CH_7	7	// "7"
#define LED12CH_8	8	// "8"
#define LED12CH_9	9	// "9"
// - characters
#define LED12CH_A	10	// "A"
#define LED12CH_B	11	// "b"
#define LED12CH_C	12	// "C"
#define LED12CH_D	13	// "d"
#define LED12CH_E	14	// "E"
#define LED12CH_F	15	// "F"
#define LED12CH_G	16	// "G"
#define LED12CH_H	17	// "H"
#define	LED12CH_I	18	// "i"
#define	LED12CH_J	19	// "J"
#define	LED12CH_K	20	// "K"
#define	LED12CH_L	21	// "L"
#define	LED12CH_M	22	// "M"
#define	LED12CH_N	23	// "n"
#define	LED12CH_O	24	// "o"
#define	LED12CH_P	25	// "P"
#define	LED12CH_Q	26	// "Q"
#define	LED12CH_R	27	// "r"
#define	LED12CH_S	28	// "S"
#define	LED12CH_T	29	// "t"
#define	LED12CH_U	30	// "u"
#define	LED12CH_V	31	// "V"
#define	LED12CH_W	32	// "W"
#define	LED12CH_X	33	// "X"
#define	LED12CH_Y	34	// "Y"
#define	LED12CH_Z	35	// "Z"
// - special
#define LED12CH_C2	36	// "c"
#define LED12CH_QUER	37	// ?
#define LED12CH_NEG	38	// "-" minus
#define LED12CH_SPC	39	// space
#define LED12CH_UNDER	40	// "_"
// - graphics
#define LED12CH_SEGA	41	// only segment A
#define LED12CH_SEGB	42	// only segment B
#define LED12CH_SEGC	43	// only segment C
#define LED12CH_SEGD	44	// only segment D
#define LED12CH_SEGE	45	// only segment E
#define LED12CH_SEGF	46	// only segment F
#define LED12CH_SEGG	47	// only segment G
#define LED12CH_SEGP	48	// only segment P

#define LED12CH_NUM	49	// number od characters

#define LED12CH_MASK	0x3f	// mask single character, without dot

#define LED12CH_NOCHAR	0x3f	// invalid character

//#define LED12CH_DOT	B6	// DOT flag - do OR this flag with another symbol to display dot

// digits (contains inverted segments of the digits and characters)
extern const u8 LED12_SEGDig[LED12CH_NUM];

// convert ASCII character to LED12CH_* character (returns LED12CH_NOCHAR if character is not valid)
u8 LED12_AsciiToLedCh(u8 ch);

// LED12 device
class cLED12
{
public:
	int	i2c;		// I2C bus index (0 or 1)
	int	addr;		// I2C address (default 0x32)
	int 	speed;		// transfer speed in Hz (usually 100000 to 400000)
	u8	buf[LED12_DIG_NUM]; // print buffer

	// initialize
	//  i2c ... I2C bus index (0 or 1)
	//  addr ... I2C address (default LED12_ADDR=0x32)
	//  speed ... transfer speed in Hz (usually 100000 to 400000)
	void Init(int i2c = LED12_I2C, int addr = LED12_ADDR, int speed = LED12_SPEED);

	// export ASCII text with length to print buffer
	void TextLenBuf(const char* text, int len);

	// export ASCIIZ text to print buffer
	void TextBuf(const char* text);

	// send print buffer to the display (returns False on error)
	Bool Update();
};

#endif // _DRV_LED12_H

#endif // USE_LED12
