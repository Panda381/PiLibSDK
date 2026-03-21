
// ****************************************************************************
//
//           I2C0, I2C1 master (BSC Broadcom Serial Controller Master)
//
// ****************************************************************************

#include "../../includes.h"	// includes

// set clock in Hz (3814 Hz - 125 MHz)
void I2C_SetClock(int i2c, int clk)
{
	int div = (CoreClock + clk/2) / clk; // get divider
	div &= ~1;			// round down to even number
	if (div < 2) div = 2;		// minimal value
	if (div > 65536) div = 65536;	// maximal value
	I2C_SetDiv(i2c, div);
}

// get clock in Hz (returns 3814 Hz - 125 MHz)
int I2C_GetClock(int i2c)
{
	int div = I2C_GetDiv(i2c) & ~1;
	int f = (CoreClock + div/2) / div;
	return f;
}

// read data from I2C
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  buf ... destination buffer
//  num ... number of bytes 1..65535
// Returns number of received bytes ('result < num' means error)
// Remap GPIOs before using.
int I2C_Read(int i2c, int addr, void* buf, int num)
{
	// destination buffer
	u8* d = (u8*)buf;

	// setup transfer
	I2C_t* a = I2C(i2c);	// peripheral address
	a->CTRL = 0;		// disable I2C
	a->ADDR = addr;		// setup slave address
	a->STAT = B1|B8|B9;	// clear DONE, ERR and CLKT
	a->CTRL = B4|B5;	// clear FIFO
	a->DLEN = num;		// number of bytes
	a->CTRL = B0|B7|B15;	// start transfer in READ direction

	// transfer data
	while (num > 0)
	{
		// get STAT register
		u32 stat = a->STAT;

		// error "ERR" or "CLKT"
		if ((stat & (B8|B9)) != 0) break;

		// transfer complete "DONE"
		if ((stat & B1) != 0)
		{
			// no data in FIFO? It looks like an error
			if ((stat & B5) == 0)  // "RXD"
			{
				// short time for signal settling
				WaitUs(5);

				// some data are missing, break
				stat = a->STAT;
				if ((stat & B5) == 0) break; // "RXD"
			}
		}

		// FIFO contains received byte
		if ((stat & B5) != 0)  // "RXD"
		{
			*d++ = a->FIFO;
			num--;
		}
	}

	// clear state
	a->STAT = B1|B8|B9;	// clear DONE, ERR and CLKT

	// return number of bytes
	return d - (u8*)buf;
}

// write data to I2C
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  buf ... source buffer
//  num ... number of bytes 1..65535
// Returns number of sent bytes ('result < num' means error)
// Remap GPIOs before using.
int I2C_Write(int i2c, int addr, const void* buf, int num)
{
	// source buffer
	const u8* s = (const u8*)buf;

	// setup transfer
	I2C_t* a = I2C(i2c);	// peripheral address
	a->CTRL = 0;		// disable I2C
	a->ADDR = addr;		// setup slave address
	a->STAT = B1|B8|B9;	// clear DONE, ERR and CLKT
	a->CTRL = B4|B5;	// clear FIFO
	a->DLEN = num;		// number of bytes

	// prepare FIFO
	int i = num;
	if (i > I2C_FIFO_SIZE) i = I2C_FIFO_SIZE;
	for (; i > 0; i--)
	{
		a->FIFO = *s++;
		num--;
	}

	// start transfer
	a->CTRL = B7|B15;	// start transfer in WRITE direction

	// transfer data
	while (True)
	{
		// get STAT register
		u32 stat = a->STAT;

		// transfer complete "DONE", or error "ERR", or error "CLKT"
		if ((stat & (B1|B8|B9)) != 0) break;

		// FIFO can accept next byte
		if ((num > 0) && ((stat & B4) != 0))  // "TXD"
		{
			a->FIFO = *s++;
			num--;
		}
	}

	// clear state
	a->STAT = B1|B8|B9;	// clear DONE, ERR and CLKT

	// return number of bytes
	return s - (const u8*)buf;
}
