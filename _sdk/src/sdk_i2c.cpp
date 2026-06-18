
// ****************************************************************************
//
//           I2C0, I2C1 master (BSC Broadcom Serial Controller Master)
//
// ****************************************************************************

#include "../../includes.h"	// includes

// flag - I2C was initialized
Bool I2C_IsInit[I2C_NUM] = {
	False, False,
#if RASPPI >= 4
	False, False, False, False, False,
#endif
};

// last used I2C speed i Hz
volatile int I2C_LastSpeed[I2C_NUM] = {
	I2C_DEF_SPEED, I2C_DEF_SPEED,
#if RASPPI >= 4
	I2C_DEF_SPEED, I2C_DEF_SPEED, I2C_DEF_SPEED, I2C_DEF_SPEED, I2C_DEF_SPEED,
#endif
};

// set clock in Hz (3814 Hz - 125 MHz)
void I2C_SetClock(int i2c, int clk)
{
	I2C_LastSpeed[i2c] = clk;
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

// initialize I2C and setup GPIOs (uses GPIO0/1 or GPIO2/3)
//  clk ... set clock in Hz (3814 Hz - 125 MHz; usually 100000 to 400000, default I2C_DEF_SPEED = 100000)
void I2C_Init(int i2c, int clk /* = I2C_DEF_SPEED */)
{
//	Pull	ALT0		ALT1		ALT2		ALT3		ALT4		ALT5		usual function
//GPIO0	High	SDA0		SA5		-		-		-		-		I2C0 SDA, ID EEPROM HAT
//GPIO1	High	SCL0		SA4		-		-		-		-		I2C0 SCL, ID EEPROM HAT
//GPIO2	High	SDA1		SA3		-		-		-		-		I2C1 SDA
//GPIO3	High	SCL1		SA2		-		-		-		-		I2C1 SCL

	// setup GPIO
	if (i2c == 0)
	{
		// I2C0: GPIO0 SDA0, GPIO1 SCL0
		GPIO_Func(0, GPIO_FUNC_AF0);
		GPIO_Func(1, GPIO_FUNC_AF0);
		GPIO_Pull(0, GPIO_PULL_UP);
		GPIO_Pull(1, GPIO_PULL_UP);
	}
	else
	{
		// I2C1: GPIO2 SDA1, GPIO3 SCL1
		GPIO_Func(2, GPIO_FUNC_AF0);
		GPIO_Func(3, GPIO_FUNC_AF0);
		GPIO_Pull(2, GPIO_PULL_UP);
		GPIO_Pull(3, GPIO_PULL_UP);
	}

	// clear flags
	I2C_t* a = I2C(i2c);	// peripheral address
	a->CTRL = B4|B5|B15;	// I2C enable, clear FIFO (must be enabled to enable access to the registers)
	a->STAT = B1|B8|B9;	// clear DONE, ERR and CLKT
	a->DEL = (48 << 16) | 48;

	// setup clock
	I2C_SetClock(i2c, clk);	// setup clock

	// setup data delay to 300ns at CoreClock=250MHz (values are in core clocks)
	a->DEL = (75 << 16) | 75;

	// setup CLKT to 50 ms at 400 kHz (values are in I2C clocks)
	a->CLKT = 20000;

	// wait some time to stabilize signals on the bus - only on first initialize
	if (!I2C_IsInit[i2c]) WaitMs(1);

	// flag - I2C was initialized
	I2C_IsInit[i2c] = True;
}

// terminate I2C
void I2C_Term(int i2c)
{
	// flag - I2C was not initialized
	if (!I2C_IsInit[i2c]) return; // not initialized
	I2C_IsInit[i2c] = False;

#if USE_I2CBUS		// 1=use I2C bus driver, 0=not used (drv_i2cbus.*)
	// terminate I2C bus driver
	I2Cbus_Term(i2c);
#endif // USE_I2CBUS

	// disable interrupts
	I2C_t* a = I2C(i2c);	// peripheral address
	a->CTRL = B4|B5|B15;	// clear FIFO
	a->STAT = B1|B8|B9;	// clear DONE, ERR and CLKT
	a->CTRL = 0;		// disable I2C

	// terminate GPIO
	if (i2c == 0)
	{
		// I2C0: GPIO0 SDA0, GPIO1 SCL0
		GPIO_Func(0, GPIO_FUNC_IN);
		GPIO_Func(1, GPIO_FUNC_IN);
		GPIO_Pull(0, GPIO_PULL_OFF);
		GPIO_Pull(1, GPIO_PULL_OFF);
	}
	else
	{
		// I2C1: GPIO2 SDA1, GPIO3 SCL1
		GPIO_Func(2, GPIO_FUNC_IN);
		GPIO_Func(3, GPIO_FUNC_IN);
		GPIO_Pull(2, GPIO_PULL_OFF);
		GPIO_Pull(3, GPIO_PULL_OFF);
	}
}

// auto-initialize I2C, if not initialized
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
// If it has already been initialized, it only updates the transfer rate.
void I2C_AutoInit(int i2c, int speed /* = 0 */)
{
	// use last speed
	if (speed == 0) speed = I2C_LastSpeed[i2c];

	// initialize, if not initialized yet
	if (!I2C_IsInit[i2c])
		I2C_Init(i2c, speed);
	else
	{
		// already initialized, only update transfer speed
		if (speed != I2C_LastSpeed[i2c])
		{
			// set clock in Hz (3814 Hz - 125 MHz)
			I2C_SetClock(i2c, speed);

			// wait at least 1 CLK to update pre-divider
			WaitUs(10);
		}
	}
}

// read data from I2C (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  data ... data buffer, part 1 (can be NULL if len = 0)
//  len ... number of bytes 0..65535, part 1
//  data2 ... data buffer, part 2 (can be NULL if len2 = 0)
//  len2 ... number of bytes 0..65535, part 2
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
// Do not check device presence reading 0 bytes, it does not work.
// I2C is auto-initialized, if not initialized yet.
Bool I2C_Read(int i2c, int addr, void* data, int len, void* data2 /* = NULL */, int len2 /* = 0 */, int speed /* = 0 */)
{
#if USE_I2CBUS		// 1=use I2C bus driver, 0=not used (drv_i2cbus.*)
	// redirection to I2C bus driver
	if (I2Cbus_IsUse[i2c]) return I2Cbus_Read(i2c, addr, data, len, data2, len2, speed);
#endif

	// auto-initialize I2C, if not initialized
	I2C_AutoInit(i2c, speed);

	// destination buffer
	u8* d = (u8*)data;
	u8* d2 = (u8*)data2;

	// prepare peripheral address
	I2C_t* a = I2C(i2c);	// peripheral address

	// wait until the bus is free (time-out 100us)
	u32 start = Time();
	while ((a->STAT & B0) != 0)
	{
		if ((u32)(Time() - start) >= 100) return False;
	}

	// clear old transfer and FIFO
	a->CTRL = B4|B5|B15;	// clear FIFO
	a->STAT = B1|B8|B9;	// clear DONE, ERR and CLKT

	// setup transfer
	a->ADDR = addr;		// setup slave address
	a->DLEN = len+len2;	// number of bytes

	// start transfer
	a->CTRL = B0|B7|B15;	// start transfer in READ direction

	// transfer data (time-out 55ms per character)
	while ((u32)(Time() - start) < 55000)
	{
		// get STAT register
		u32 stat = a->STAT;

		// transfer error ERR or CLKT
		if ((stat & (B8|B9)) != 0)
		{
			a->STAT = B1|B8|B9; // clear DONE, ERR and CLKT flag
			return False;
		}

		// FIFO contains received byte
		if ((len+len2 > 0) && ((stat & B5) != 0))  // "RXD"
		{
			if (len > 0)
			{
				*d++ = a->FIFO;
				len--;
			}
			else
			{
				*d2++ = a->FIFO;
				len2--;
			}
			start = Time();	// restart timeout
		}

		// transfer complete "DONE"
		if ((stat & B1) != 0)
		{
			// short time for signal settling
			WaitUs(5);

			// read rest of data from FIFO
			while ((len+len2 > 0) && ((a->STAT & B5) != 0)) // "RXD"
			{
				if (len > 0)
				{
					*d++ = a->FIFO;
					len--;
				}
				else
				{
					*d2++ = a->FIFO;
					len2--;
				}
			}

			// clear DONE, ERR and CLKT flag
			stat = a->STAT;
			a->STAT = B1|B8|B9;

			// check data length
			return (len+len2 == 0) && ((stat & (B8|B9)) == 0);
		}
	}

	// timeout error
	return False;
}

// write data to I2C (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  data ... data buffer, part 1 (can be NULL if len = 0)
//  len ... number of bytes 0..65535, part 1
//  data2 ... data buffer, part 2 (can be NULL if len2 = 0)
//  len2 ... number of bytes 0..65535, part 2
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
// The presence of a device can be tested by writing a data length of 0.
// I2C is auto-initialized, if not initialized yet.
Bool I2C_Write(int i2c, int addr, const void* data, int len, const void* data2 /* = NULL */, int len2 /* = 0 */, int speed /* = 0 */)
{
#if USE_I2CBUS		// 1=use I2C bus driver, 0=not used (drv_i2cbus.*)
	// redirection to I2C bus driver
	if (I2Cbus_IsUse[i2c]) return I2Cbus_Write(i2c, addr, data, len, data2, len2, speed);
#endif

	// auto-initialize I2C, if not initialized
	I2C_AutoInit(i2c, speed);

	// source buffer
	const u8* s = (const u8*)data;
	const u8* s2 = (const u8*)data2;

	// prepare peripheral address
	I2C_t* a = I2C(i2c);	// peripheral address

	// wait until the bus is free (time-out 100us)
	u32 start = Time();
	while ((a->STAT & B0) != 0)
	{
		if ((u32)(Time() - start) >= 100) return False;
	}

	// clear old transfer and FIFO
	a->CTRL = B4|B5|B15;	// clear FIFO
	a->STAT = B1|B8|B9;	// clear DONE, ERR and CLKT

	// setup transfer
	a->ADDR = addr;		// setup slave address
	a->DLEN = len+len2;	// number of bytes

	// prepare FIFO
	int i;
	for (i = I2C_FIFO_SIZE; i > 0; i--)
	{
		if (len > 0)
		{
			a->FIFO = *s++;
			len--;
		}
		else if (len2 > 0)
		{
			a->FIFO = *s2++;
			len2--;
		}
		else
			break;
	}

	// start transfer
	a->CTRL = B7|B15;	// start transfer in WRITE direction

	// transfer data (time-out 55ms per character)
	while ((u32)(Time() - start) < 55000)
	{
		// get STAT register
		u32 stat = a->STAT;

		// transfer error ERR or CLKT
		if ((stat & (B8|B9)) != 0)
		{
			a->STAT = B1|B8|B9; // clear DONE, ERR and CLKT flag
			return False;
		}

		// FIFO can accept next byte
		if (((stat & B4) != 0) && (len+len2 > 0)) // "TXD"
		{
			if (len > 0)
			{
				a->FIFO = *s++;
				len--;
			}
			else
			{
				a->FIFO = *s2++;
				len2--;
			}
			start = Time();	// restart timeout
		}

		// transfer complete "DONE"
		if ((stat & B1) != 0)
		{
			a->STAT = B1|B8|B9; // clear DONE, ERR and CLKT flag
			return (len+len2 == 0) && ((stat & (B8|B9)) == 0);
		}
	}

	// timeout error
	return False;
}

// check presence of the I2C device (to scan devices on the bus; returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
// I2C is auto-initialized, if not initialized yet.
Bool I2C_Check(int i2c, int addr, int speed /* = I2C_DEF_SPEED */)
{
	return I2C_Write(i2c, addr, NULL, 0, NULL, 0, speed);
}

// read registers with 8-bit addressing (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  inx ... 8-bit index of first register
//  reg ... pointer to array of registers
//  num ... number of registers
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
Bool I2C_ReadReg8(int i2c, int addr, int inx, u8* reg, int num /* = 1 */, int speed /* = 0 */)
{
	// write address
	u8 b[1];
	b[0] = (u8)inx;
	if (!I2C_Write(i2c, addr, b, 1, NULL, 0, speed)) return False;

	// read data
	return I2C_Read(i2c, addr, reg, num, NULL, 0, speed);
}

// read registers with 16-bit addressing (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  inx ... 16-bit index of first register
//  reg ... pointer to array of registers
//  num ... number of registers
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
Bool I2C_ReadReg16(int i2c, int addr, int inx, u8* reg, int num /* = 1 */, int speed /* = 0 */)
{
	// write address
	u8 b[2];
	b[0] = (u8)(inx >> 8);
	b[1] = (u8)inx;
	if (!I2C_Write(i2c, addr, b, 2, NULL, 0, speed)) return False;

	// read data
	return I2C_Read(i2c, addr, reg, num, NULL, 0, speed);
}

// read registers with 24-bit addressing (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  inx ... 24-bit index of first register
//  reg ... pointer to array of registers
//  num ... number of registers
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
Bool I2C_ReadReg24(int i2c, int addr, int inx, u8* reg, int num /* = 1 */, int speed /* = 0 */)
{
	// write address
	u8 b[3];
	b[0] = (u8)(inx >> 16);
	b[1] = (u8)(inx >> 8);
	b[2] = (u8)inx;
	if (!I2C_Write(i2c, addr, b, 3, NULL, 0, speed)) return False;

	// read data
	return I2C_Read(i2c, addr, reg, num, NULL, 0, speed);
}

// read registers with 32-bit addressing (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  inx ... 32-bit index of first register
//  reg ... pointer to array of registers
//  num ... number of registers
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
Bool I2C_ReadReg32(int i2c, int addr, int inx, u8* reg, int num /* = 1 */, int speed /* = 0 */)
{
	// write address
	u8 b[4];
	b[0] = (u8)(inx >> 24);
	b[1] = (u8)(inx >> 16);
	b[2] = (u8)(inx >> 8);
	b[3] = (u8)inx;
	if (!I2C_Write(i2c, addr, b, 4, NULL, 0, speed)) return False;

	// read data
	return I2C_Read(i2c, addr, reg, num, NULL, 0, speed);
}

// write registers with 8-bit addressing (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  inx ... 8-bit index of first register
//  reg ... pointer to array of registers
//  num ... number of registers
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
Bool I2C_WriteReg8(int i2c, int addr, int inx, const u8* reg, int num /* = 1 */, int speed /* = 0 */)
{
	u8 b[1];
	b[0] = (u8)inx;
	return I2C_Write(i2c, addr, b, 1, reg, num, speed);
}

// write registers with 16-bit addressing (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  inx ... 16-bit index of first register
//  reg ... pointer to array of registers
//  num ... number of registers
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
Bool I2C_WriteReg16(int i2c, int addr, int inx, const u8* reg, int num /* = 1 */, int speed /* = 0 */)
{
	u8 b[2];
	b[0] = (u8)(inx >> 8);
	b[1] = (u8)inx;
	return I2C_Write(i2c, addr, b, 2, reg, num, speed);
}

// write registers with 24-bit addressing (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  inx ... 24-bit index of first register
//  reg ... pointer to array of registers
//  num ... number of registers
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
Bool I2C_WriteReg24(int i2c, int addr, int inx, const u8* reg, int num /* = 1 */, int speed /* = 0 */)
{
	u8 b[3];
	b[0] = (u8)(inx >> 16);
	b[1] = (u8)(inx >> 8);
	b[2] = (u8)inx;
	return I2C_Write(i2c, addr, b, 3, reg, num, speed);
}

// write registers with 32-bit addressing (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  inx ... 32-bit index of first register
//  reg ... pointer to array of registers
//  num ... number of registers
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
Bool I2C_WriteReg32(int i2c, int addr, int inx, const u8* reg, int num /* = 1 */, int speed /* = 0 */)
{
	u8 b[4];
	b[0] = (u8)(inx >> 24);
	b[1] = (u8)(inx >> 16);
	b[2] = (u8)(inx >> 8);
	b[3] = (u8)inx;
	return I2C_Write(i2c, addr, b, 4, reg, num, speed);
}
