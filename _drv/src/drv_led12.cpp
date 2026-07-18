
// ****************************************************************************
//
//                           BarePi LED12 display
//
// ****************************************************************************

#include "../../includes.h"	// includes

#if USE_LED12		// 1=use BarePi LED12 display, 0=not used (drv_led12.*)

// digits (contains inverted segments of the digits and characters)
const u8 LED12_SEGDig[LED12CH_NUM] = {
	// - digits
	(u8)~(LED12_SEGA+LED12_SEGB+LED12_SEGC+LED12_SEGD+LED12_SEGE+LED12_SEGF),	// 0 LED12CH_0
	(u8)~(LED12_SEGB+LED12_SEGC),							// 1 LED12CH_1
	(u8)~(LED12_SEGA+LED12_SEGB+LED12_SEGD+LED12_SEGE+LED12_SEGG),			// 2 LED12CH_2
	(u8)~(LED12_SEGA+LED12_SEGB+LED12_SEGC+LED12_SEGD+LED12_SEGG),			// 3 LED12CH_3
	(u8)~(LED12_SEGB+LED12_SEGC+LED12_SEGF+LED12_SEGG),				// 4 LED12CH_4
	(u8)~(LED12_SEGA+LED12_SEGC+LED12_SEGD+LED12_SEGF+LED12_SEGG),			// 5 LED12CH_5
	(u8)~(LED12_SEGA+LED12_SEGC+LED12_SEGD+LED12_SEGE+LED12_SEGF+LED12_SEGG),	// 6 LED12CH_6
	(u8)~(LED12_SEGA+LED12_SEGB+LED12_SEGC),					// 7 LED12CH_7
	(u8)~(LED12_SEGA+LED12_SEGB+LED12_SEGC+LED12_SEGD+LED12_SEGE+LED12_SEGF+LED12_SEGG), // 8 LED12CH_8
	(u8)~(LED12_SEGA+LED12_SEGB+LED12_SEGC+LED12_SEGD+LED12_SEGF+LED12_SEGG),	// 9 LED12CH_9
	// - characters
	(u8)~(LED12_SEGA+LED12_SEGB+LED12_SEGC+LED12_SEGE+LED12_SEGF+LED12_SEGG),	// 10 LED12CH_A
	(u8)~(LED12_SEGC+LED12_SEGD+LED12_SEGE+LED12_SEGF+LED12_SEGG),			// 11 LED12CH_B
	(u8)~(LED12_SEGA+LED12_SEGD+LED12_SEGE+LED12_SEGF),				// 12 LED12CH_C
	(u8)~(LED12_SEGB+LED12_SEGC+LED12_SEGD+LED12_SEGE+LED12_SEGG),			// 13 LED12CH_D
	(u8)~(LED12_SEGA+LED12_SEGD+LED12_SEGE+LED12_SEGF+LED12_SEGG),			// 14 LED12CH_E
	(u8)~(LED12_SEGA+LED12_SEGE+LED12_SEGF+LED12_SEGG),				// 15 LED12CH_F
	(u8)~(LED12_SEGA+LED12_SEGC+LED12_SEGD+LED12_SEGE+LED12_SEGF),			// 16 LED12CH_G
	(u8)~(LED12_SEGB+LED12_SEGC+LED12_SEGE+LED12_SEGF+LED12_SEGG),			// 17 LED12CH_H
	(u8)~(LED12_SEGC),								// 18 LED12CH_I
	(u8)~(LED12_SEGB+LED12_SEGC+LED12_SEGD+LED12_SEGE),				// 19 LED12CH_J
	(u8)~(LED12_SEGE+LED12_SEGF+LED12_SEGG),					// 20 LED12CH_K
	(u8)~(LED12_SEGD+LED12_SEGE+LED12_SEGF),					// 21 LED12CH_L
	(u8)~(LED12_SEGA+LED12_SEGB+LED12_SEGC+LED12_SEGE+LED12_SEGF),			// 22 LED12CH_M
	(u8)~(LED12_SEGC+LED12_SEGE+LED12_SEGG),					// 23 LED12CH_N
	(u8)~(LED12_SEGC+LED12_SEGD+LED12_SEGE+LED12_SEGG),				// 24 LED12CH_O
	(u8)~(LED12_SEGA+LED12_SEGB+LED12_SEGE+LED12_SEGF+LED12_SEGG),			// 25 LED12CH_P
	(u8)~(LED12_SEGA+LED12_SEGB+LED12_SEGD+LED12_SEGE+LED12_SEGF),			// 26 LED12CH_Q
	(u8)~(LED12_SEGE+LED12_SEGG),							// 27 LED12CH_R
	(u8)~(LED12_SEGA+LED12_SEGC+LED12_SEGD+LED12_SEGF+LED12_SEGG),			// 28 LED12CH_S
	(u8)~(LED12_SEGD+LED12_SEGE+LED12_SEGF+LED12_SEGG),				// 29 LED12CH_T
	(u8)~(LED12_SEGC+LED12_SEGD+LED12_SEGE),					// 30 LED12CH_U
	(u8)~(LED12_SEGB+LED12_SEGC+LED12_SEGD+LED12_SEGE+LED12_SEGF),			// 31 LED12CH_V
	(u8)~(LED12_SEGB+LED12_SEGC+LED12_SEGD+LED12_SEGE+LED12_SEGF+LED12_SEGG),	// 32 LED12CH_W
	(u8)~(LED12_SEGC+LED12_SEGF+LED12_SEGG),					// 33 LED12CH_X
	(u8)~(LED12_SEGB+LED12_SEGC+LED12_SEGD+LED12_SEGF+LED12_SEGG),			// 34 LED12CH_Y
	(u8)~(LED12_SEGA+LED12_SEGB+LED12_SEGD+LED12_SEGE+LED12_SEGG),			// 35 LED12CH_Z
	// - special
	(u8)~(LED12_SEGD+LED12_SEGE+LED12_SEGG),					// 36 LED12CH_C2
	(u8)~(LED12_SEGA+LED12_SEGB+LED12_SEGE+LED12_SEGG),				// 37 LED12CH_QUER
	(u8)~(LED12_SEGG),								// 38 LED12CH_NEG
	(u8)~(0),									// 39 LED12CH_SPC
	(u8)~(LED12_SEGD),								// 40 LED12CH_UNDER
	// - graphics
	(u8)~(LED12_SEGA),								// 41 LED12CH_LED12_SEGA
	(u8)~(LED12_SEGB),								// 42 LED12CH_LED12_SEGB
	(u8)~(LED12_SEGC),								// 43 LED12CH_LED12_SEGC
	(u8)~(LED12_SEGD),								// 44 LED12CH_LED12_SEGD
	(u8)~(LED12_SEGE),								// 45 LED12CH_LED12_SEGE
	(u8)~(LED12_SEGF),								// 46 LED12CH_LED12_SEGF
	(u8)~(LED12_SEGG),								// 47 LED12CH_LED12_SEGG
	(u8)~(LED12_SEGP),								// 48 LED12CH_LED12_SEGP
};	

// convert ASCII character to LED12CH_* character (returns LED12CH_NOCHAR if character is not valid)
u8 LED12_AsciiToLedCh(u8 ch)
{
	// space
	if (ch == ' ') return LED12CH_SPC;

	// minus
	if (ch == '-') return LED12CH_NEG;

	// ?
	if (ch == '?') return LED12CH_QUER;

	// _
	if (ch == '_') return LED12CH_UNDER;

	// digit
	if ((ch >= '0') && (ch <= '9')) return ch - '0' + LED12CH_0;

	// letter
	if ((ch >= 'A') && (ch <= 'Z')) return ch - 'A' + LED12CH_A;
	if ((ch >= 'a') && (ch <= 'z')) return ch - 'a' + LED12CH_A;

	// simple dot
	if ((ch == '.') || (ch == ',')) return LED12CH_SEGP;

	// invalid character
	return LED12CH_NOCHAR;
}

// initialize
//  i2c ... I2C bus index (0 or 1)
//  addr ... I2C address (default LED12_ADDR=0x32)
//  speed ... transfer speed in Hz (usually 100000 to 400000, default I2C_DEF_SPEED=100000)
void cLED12::Init(int i2c /* = LED12_I2C */, int addr /* = LED12_ADDR */, int speed /* = LED12_SPEED */)
{
	this->i2c = i2c;
	this->addr = addr;
	this->speed = speed;
}

// export ASCII text with length to print buffer
void cLED12::TextLenBuf(const char* text, int len)
{
	// clear buffer with spaces
	memset(this->buf, 0xff, LED12_DIG_NUM);

	// convert text
	u8 ch;
	int i = 0;
	for (; len > 0; len--)
	{
		// convert character
		ch = LED12_AsciiToLedCh(*text++);

		// valid character
		if (ch != LED12CH_NOCHAR)
		{
			// dot - only add segment DP to last character
			if ((ch == LED12CH_SEGP) && (i > 0))
				this->buf[i-1] &= ~LED12_SEGP;

			// add character
			else
			{
				if (i == LED12_DIG_NUM) break;
				this->buf[i++] = LED12_SEGDig[ch];
			}
		}
	}
}

// export ASCIIZ text to print buffer
void cLED12::TextBuf(const char* text)
{
	this->TextLenBuf(text, StrLen(text));
}

// send print buffer to the display (returns False on error)
Bool cLED12::Update()
{
	// write registers with 16-bit addressing (returns False on error)
	//  i2c ... I2C peripheral 0..1
	//  addr ... slave address 0..127
	//  inx ... 16-bit index of first register
	//  reg ... pointer to array of registers
	//  num ... number of registers
	//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
	return I2Cbus_WriteReg16(this->i2c, this->addr, LED12_RAM_SEG, this->buf, LED12_DIG_NUM, this->speed);
}

#endif // USE_LED12
