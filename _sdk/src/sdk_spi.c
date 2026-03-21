
// ****************************************************************************
//
//                                  SPI0 Master
//
// ****************************************************************************

#include "../../includes.h"	// includes

// set clock in Hz (3814 Hz - 125 MHz)
void SPI0_SetClock(int clk)
{
	int div = (CoreClock + clk/2) / clk; // get divider
	div &= ~1;			// round down to even number
	if (div < 2) div = 2;		// minimal value
	if (div > 65536) div = 65536;	// maximal value
	SPI0_SetDiv(div);
}

// get clock in Hz (returns 3814 Hz - 125 MHz)
int SPI0_GetClock(void)
{
	int div = SPI0_GetDiv() & ~1;
	int f = (CoreClock + div/2) / div;
	return f;
}

// initialize SPI0
//   speed ... transfer speed in Hz (typically 3814 Hz - 125 MHz)
//   cpha ... clock phase 0 or 1, first SCLK transition at 0=middle, 1=beginning of data bit
//   cpol ... clock polarity 0 or 1, rest state of clock 0=low, 1=high
// GPIO pins should be set up first before enabling the SPI.
void SPI0_Init(u32 speed, int cpha, int cpol)
{
	// set clock
	SPI0_SetClock(speed);

	// setup SPI, flush FIFO
	SPI_t* spi = SPI0;
	spi->CS = (cpha << 2) | (cpol << 3) | B4 | B5;
}

// write/read SPI0 data (must be initialized with SPI0_Init())
//  cs ... chip select index 0..1
//  wbuf ... write buffer (to send; NULL=not used, send default value 0xff)
//  rbuf ... read buffer (to receive; NULL=not used)
//  num ... number of bytes
void SPI0_WriteRead(int cs, const u8* wbuf, u8* rbuf, int num)
{
	u32 data;

	// start transfer (select CS, get CPHA, CPOL, flush FIFO, set TA)
	SPI_t* spi = SPI0;
	spi->CS = cs | (spi->CS & (B2|B3)) | B4 | B5 | B7;
	
	// prepare counters
	int wnum = num;
	int rnum = num;

	// transfer loop
	while ((wnum > 0) || (wnum > 0))
	{
		// write enabled
		if ((wnum > 0) && ((spi->CS & B18) != 0)) // flag TXD
		{
			// get data to send
			data = 0xff;
			if (wbuf != NULL) data = *wbuf++;
			
			// send byte
			spi->FIFO = data;
			wnum--;
		}

		// read enable
		if ((rnum > 0) && ((spi->CS & B17) != 0)) // flag RXD
		{
			// receive byte
			data = spi->FIFO;
			rnum--;

			// write byte to buffer
			if (rbuf != NULL) *rbuf++ = (u8)data;
		}
	}

	// flush rest of received data
	while ((spi->CS & B16) == 0) // flag DONE
	{
		while ((spi->CS & B17) != 0) // flag RXD
		{
			(void)spi->FIFO;
		}
	}

	// stop transfer (clear TA flag)
	spi->CS &= ~B7;
}
