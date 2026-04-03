
// ****************************************************************************
//
//         BSC Broadcom Serial Controller Slave (I2C slave, SPI slave)
//
// ****************************************************************************

#include "../../includes.h"	// includes

// Initialize I2C Slave interface
//  addr ... slave address 0..127
// Remap GPIOs before using.
void I2CSlave_Init(int addr)
{
	BSC_t* bsc = BSC;

	// setup I2C slave address
	bsc->SLV = addr;

	// disable interrupts
	bsc->IMSC = 0;

	// enable I2C mode, enable BSC
	bsc->CR = B0 | B2;
}

// read data from I2C Slave
//  buf ... destination buffer
//  num ... number of bytes
//  tout ... timeout in [us]
// Returns number of received bytes ('result < num' means error)
// Initialize with I2CSlave_Init() before using.
int I2CSlave_Read(void* buf, int num, u32 tout)
{
	BSC_t* bsc = BSC;

	// destination buffer
	u8* d = (u8*)buf;

	// clear status error register
	bsc->RSR = 0;

	// start transfer - enable receive mode
	bsc->CR |= B9; // "RXE" receive enable flag

	// start time
	u32 start = Time();

	// transfer data
	while (num > 0)
	{
		// time out error
		if ((u32)(Time() - start) >= tout) break;

		// overrun error
		if ((bsc->RSR & B0) != 0) break;	// "OE" overrun flag

		// receive data
		while (((bsc->FR & B1) == 0) && (num > 0)) // "RXFE" RX FIFO is empty flag
		{
			// read byte
			*d++ = bsc->DR & 0xff;

			// decrement counter
			num--;
		}
	}

	// wait for transfer to stop
	while ((bsc->FR & B5) != 0)	// "RXBUSY" Receive is busy flag
	{
		// overrun error
		if ((bsc->RSR & B0) != 0) break;	// "OE" overrun flag
	}

	// stop transfer
	bsc->CR &= ~B9; // "RXE" receive enable flag

	// return number of bytes
	return d - (u8*)buf;
}

// write data to I2C Slave
//  buf ... source buffer
//  num ... number of bytes
//  tout ... timeout in [us]
// Returns number of sent bytes ('result < num' means error)
// Initialize with I2CSlave_Init() before using.
int I2CSlave_Write(const void* buf, int num, u32 tout)
{
	BSC_t* bsc = BSC;

	// source buffer
	const u8* s = (const u8*)buf;

	// clear status error register
	bsc->RSR = 0;

	// start transfer - enable trasmit mode
	bsc->CR |= B8; // "TXE" transmit enable flag

	// start time
	u32 start = Time();

	// transfer data
	while (num > 0)
	{
		// time out error
		if ((u32)(Time() - start) >= tout) break;

		// underrun error
		if ((bsc->RSR & B1) != 0) break;	// "UE" underrun flag

		// send data
		while (((bsc->FR & B2) == 0) && (num > 0)) // "TXFF" TX FIFO is full flag
		{
			// write byte
			bsc->DR = *s++;

			// decrement counter
			num--;
		}
	}

	// wait for transfer to stop
	while ((bsc->FR & B5) != 0)	// "RXBUSY" Receive is busy flag
	{
		// underrun error
		if ((bsc->RSR & B1) != 0) break;	// "UE" underrun flag
	}

	// stop transfer
	bsc->CR &= ~B8; // "TXE" transmit enable flag

	// return number of bytes
	return s - (const u8*)buf;
}
