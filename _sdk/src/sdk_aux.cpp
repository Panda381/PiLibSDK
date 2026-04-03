
// ****************************************************************************
//
//                     Auxiliaries - UART1, SPI1, SPI2
//
// ****************************************************************************

#include "../../includes.h"	// includes

// Convert baudrate in Bauds (typically 476 Baud .. 31.25 MBaud) to baudrate divider (0..0xFFFF)
u32 UART1_BaudToDiv(u32 baud)
{
	// divide by zero
	if (baud == 0) baud = 1;

	// get register value
	u32 div = (u32)(((u64)CoreClock + baud*4) / (baud*8));

	// limitations
	if (div > 0) div--;
	if (div > 0xffff) div = 0xffff;
	return div;
}

// Convert baudrate divider (0..0xFFFF) to baudrate in Bauds (typically 476 Baud .. 31.25 MBaud)
u32 UART1_DivToBaud(u32 div)
{
	// divider
	div++;

	// ger Baud value
	u32 baud = (u32)(((u64)CoreClock + div*4) / (div*8));
	return baud;
}

// Calculate baudrate error in percent, as the difference between the desired and actual baudrate speed.
double UART1_BaudErr(u32 baud)
{
	// divide by zero
	if (baud == 0) return 0;

	// convert required baudrate in Bauds to baudrate divider
	u32 div = UART1_BaudToDiv(baud);

	// convert back baudrate divider to baudrate in Bauds
	u32 baud_real = UART1_DivToBaud(div);

	// get error in %
	double err = ((double)baud_real - (double)baud) / (double)baud * 100.0;
	return err;
}

// initialize UART1 with baudrate 476..31250000 Baud, 8 bits, 1 start, 1 stop, no parity, no flow control, no interrupt
//  GPIOs must be activated too - use GPIO_Func(gpio, GPIO_FUNC_AF5).
void UART1_Init(u32 baud)
{
	// terminate UART1
	UART1_Term();

	// enable UART1
	UART1_Enable();

	// temporary disable all, reset FIFOs
	AUX->uart.CTRL = 0;	// disable receiver and transmitter
	AUX->uart.LCR = B0|B1;	// clear DLAB bit, set 8 bits
	AUX->uart.IER = 0;	// disable interrupts
	AUX->uart.IIR = B1|B2;	// clear FIFOs

	// setup baudrate generator
	UART1_SetBaud(baud);

	// setup control register: receiver enabled, transmitter enabled, no flow control
	AUX->uart.CTRL = B0|B1;

	// reset overrun flag
	(void)AUX->uart.LSR;
}

// terminate UART1 (GPIOs must be deactivated too)
void UART1_Term()
{
	// only if UART1 is enabled (otherwise inaccessible)
	if (UART1_IsEnabled())
	{
		// disable receiver and transmitter
		AUX->uart.CTRL = 0;

		// disable DLAB mode
		AUX->uart.LCR = B0|B1;

		// disable interrupts
		AUX->uart.IER = 0;

		// clear FIFOs
		UART1_Flush();

		// reset overrun flag
		UART1_RxOverClr();

		// disable UART1
		UART1_Disable();
	}
}

// receive UART1 character with waiting
u8 UART1_RecvChar(void)
{
	// wait 
	while (!UART1_RxIsReady()) {}

	// receive
	return UART1_Read();
}

// send UART1 character with waiting
void UART1_SendChar(u8 ch)
{
	// wait
	while (!UART1_TxIsReady()) {}

	// send
	UART1_Write((u32)ch);
}

// send UART1 data buffer with length
void UART1_SendBuf(const u8* buf, int len)
{
	for (; len > 0; len--) UART1_SendChar(*buf++);
}

// send UART1 ASCIIZ text
void UART1_SendText(const char* text)
{
	char ch;
	while ((ch = *text++) != 0) UART1_SendChar(ch);
}

#if USE_STREAM	// use Data stream (lib_stream.c, lib_stream.h)

// callback - write data to drawing console
u32 StreamWriteUart1Print(sStream* str, const void* buf, u32 num)
{
	UART1_SendBuf((const u8*)buf, num);
	return num;
}

// formatted print string to UART1, with argument list (returns number of characters, without terminating 0)
u32 UART1_PrintArg(const char* fmt, va_list args)
{
	// write and read stream
	sStream wstr, rstr;

	// initialize stream to read from
	StreamReadBufInit(&rstr, fmt, StrLen(fmt));

	// initialize stream to write to
	Stream0Init(&wstr); // initialize nul stream
	wstr.write = StreamWriteUart1Print; // write callback
	
	// print string
	return StreamPrintArg(&wstr, &rstr, args);
}

// formatted print string to UART1, with variadic arguments (returns number of characters, without terminating 0)
NOINLINE u32 UART1_Print(const char* fmt, ...)
{
	u32 n;
	va_list args;
	va_start(args, fmt);
	n = UART1_PrintArg(fmt, args);
	va_end(args);
	return n;
}

#endif // USE_STREAM

// convert SPI1/SPI2 speed in Hz (typically 30517 Hz .. 125 MHz) to clock divider (0..0x0FFF)
u32 SPI_SpeedToDiv(u32 speed)
{
	// divide by zero
	if (speed == 0) speed = 1;

	// get register value
	u32 div = (u32)(((u64)CoreClock + speed) / (speed*2));

	// limitations
	if (div > 0) div--;
	if (div > 0xfff) div = 0xfff;
	return div;
}

// convert SPI1/SPI2 clock divider (0..0x0FFF) to speed in Hz (typically 30517 Hz .. 125 MHz)
u32 SPI_DivToSpeed(u32 div)
{
	// divider
	div++;

	// ger speed value
	u32 speed = (u32)(((u64)CoreClock + div) / (div*2));
	return speed;
}

// Calculate SPI1/SPI2 speed error in percent, as the difference between the desired and actual speed.
double SPI_SpeedErr(u32 speed)
{
	// divide by zero
	if (speed == 0) return 0;

	// convert required speed in Hz to clock divider
	u32 div = SPI_SpeedToDiv(speed);

	// convert back clock divider to sped in Hz
	u32 speed_real = SPI_DivToSpeed(div);

	// get error in %
	double err = ((double)speed_real - (double)speed) / (double)speed * 100.0;
	return err;
}

// initialize SPI1 as master, use 8-bit data, use fixed CS
//   speed ...... transfer speed in Hz (typically 30517..125000000 Hz)
//   msb ........ start with: True=MSB (most significant bit), False=LSB (least significant bit)
//   idlehigh ... idle clock line is : True=high, False=low
//   outrise .... OUT data are clocked on: True=rising edge, False=falling edge
//   inrise ..... IN data are clocked on: True=rising edge, False=falling edge
// GPIO pins should be set up first before enabling the SPI.
void SPI1_Init(u32 speed, Bool msb, Bool idlehigh, Bool outrise, Bool inrise)
{
	// terminate SPI
	SPI1_Term();

	// SPI enable
	SPI1_Enable();

	// reset controls
	AUX->spi1.CTRL1 = 0;		// disable interrupts
	AUX->spi1.CTRL0 = B9|8;		// reset FIFO (must be enabled later), select 8-bit data of fixed length

	// set MSB/LSB mode
	if (msb)
	{
		SPI1_OutMSB();
		SPI1_InMSB();
	}

	// set idle clock
	if (idlehigh) SPI1_IdleHigh();

	// OUT edge
	if (outrise) SPI1_OutRise();

	// IN edge
	if (inrise) SPI1_InRise();

	// setup clock divider
	SPI1_SetSpeed(speed);

	// enable FIFO
	SPI1_FlushDisable();

	// soft enable
	SPI1_SoftEnable();
}

// initialize SPI2 as master, use 8-bit data, use fixed CS
//   speed ...... transfer speed in Hz (typically 30517..125000000 Hz)
//   msb ........ start with: True=MSB (most significant bit), False=LSB (least significant bit)
//   idlehigh ... idle clock line is : True=high, False=low
//   outrise .... OUT data are clocked on: True=rising edge, False=falling edge
//   inrise ..... IN data are clocked on: True=rising edge, False=falling edge
// GPIO pins should be set up first before enabling the SPI.
void SPI2_Init(u32 speed, Bool msb, Bool idlehigh, Bool outrise, Bool inrise)
{
	// terminate SPI
	SPI2_Term();

	// SPI enable
	SPI2_Enable();

	// reset controls
	AUX->spi2.CTRL1 = 0;		// disable interrupts
	AUX->spi2.CTRL0 = B9|8;		// reset FIFO (must be enabled later), select 8-bit data of fixed length

	// set MSB/LSB mode
	if (msb)
	{
		SPI2_OutMSB();
		SPI2_InMSB();
	}

	// set idle clock
	if (idlehigh) SPI2_IdleHigh();

	// OUT edge
	if (outrise) SPI2_OutRise();

	// IN edge
	if (inrise) SPI2_InRise();

	// setup clock divider
	SPI2_SetSpeed(speed);

	// enable FIFO
	SPI2_FlushDisable();

	// soft enable
	SPI2_SoftEnable();
}

// terminate SPI1
void SPI1_Term(void)
{
	// only if SPI is enabled (otherwise inaccessible)
	if (SPI1_IsEnabled())
	{
		// reset controls
		AUX->spi1.CTRL1 = 0;	// disable interrupts
		AUX->spi1.CTRL0 = B9;	// reset FIFO

		// SPI disable
		SPI1_Disable();
	}
}

// terminate SPI2
void SPI2_Term(void)
{
	// only if SP2 is enabled (otherwise inaccessible)
	if (SPI2_IsEnabled())
	{
		// reset controls
		AUX->spi2.CTRL1 = 0;	// disable interrupts
		AUX->spi2.CTRL0 = B9;	// reset FIFO

		// SPI disable
		SPI2_Disable();
	}
}

// write/read SPI1 data (must be initialized with SPI1_Init())
//  cs ... chip select index 0..2
//  wbuf ... write buffer (to send; NULL=not used, send default value 0xff)
//  rbuf ... read buffer (to receive; NULL=not used)
//  num ... number of bytes
void SPI1_WriteRead(int cs, const u8* wbuf, u8* rbuf, int num)
{
	u32 data;

	// prepare counters
	int wnum = num;
	int rnum = num;

	// select CS
	SPI1_CS(1<<cs);

	// flush buffers
	SPI1_Flush();

	// transfer loop
	while ((wnum > 0) || (wnum > 0))
	{
		// write enabled
		if ((wnum > 0) && !SPI1_TxIsFull())
		{
			// get data to send
			data = 0xff;
			if (wbuf != NULL) data = *wbuf++;
			
			// send byte
			wnum--;
			if (wnum == 0)
				SPI1_WriteLast(data); // send last byte
			else
				SPI1_Write(data); // send middle byte
		}

		// read enable
		// also workaround for cases where the flag indicating the arrival
		// of the last byte is lost after the transfer is completed
		if ((rnum > 0) && (!SPI1_RxIsEmpty() || !SPI1_IsBusy()))
		{
			// receive byte
			data = SPI1_Read();
			rnum--;

			// write byte to buffer
			if (rbuf != NULL) *rbuf++ = (u8)data;
		}
	}
}

// write/read SPI2 data (must be initialized with SPI2_Init())
//  cs ... chip select index 0..2
//  wbuf ... write buffer (to send; NULL=not used, send default value 0xff)
//  rbuf ... read buffer (to receive; NULL=not used)
//  num ... number of bytes
void SPI2_WriteRead(int cs, const u8* wbuf, u8* rbuf, int num)
{
	u32 data;

	// prepare counters
	int wnum = num;
	int rnum = num;

	// select CS
	SPI2_CS(1<<cs);

	// flush buffers
	SPI2_Flush();

	// transfer loop
	while ((wnum > 0) || (wnum > 0))
	{
		// write enabled
		if ((wnum > 0) && !SPI2_TxIsFull())
		{
			// get data to send
			data = 0xff;
			if (wbuf != NULL) data = *wbuf++;
			
			// send byte
			wnum--;
			if (wnum == 0)
				SPI2_WriteLast(data); // send last byte
			else
				SPI2_Write(data); // send middle byte
		}

		// read enable
		// also workaround for cases where the flag indicating the arrival
		// of the last byte is lost after the transfer is completed
		if ((rnum > 0) && (!SPI2_RxIsEmpty() || !SPI2_IsBusy()))
		{
			// receive byte
			data = SPI2_Read();
			rnum--;

			// write byte to buffer
			if (rbuf != NULL) *rbuf++ = (u8)data;
		}
	}
}
