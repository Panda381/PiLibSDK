
// ****************************************************************************
//
//                     Auxiliaries - UART1, SPI1, SPI2
//
// ****************************************************************************
// Note - AUX timing is derived from the VideoCore system clock, which may change during operation.
// To ensure a stable frequency, use parameters "force_turbo=1" and "core_freq=..." in config.txt.
// SPI2 is present in Pi 4 and higher chips, but it is not mapped to any GPIO pins and is therefore unusable.

/*
UART1 (Mini-UART):
	Stop bit is not checked - it is ignored, only wait some time for next bit.
	If enabled, it will immediately start receiving data, expecially if RX line
	is low (= start bit). Immediately after a reset, the baudrate register
	will be zero and the system clock will be 250 MHz. So only 2.5 us
	suffice to fill the receive FIFO. The result will be that if the UART
	is enabled without changing the default configuration, the FIFO will be
	full and overflowing in no time flat.
	GPIO pins should be set up first before enabling the UART.
    features:
	- 7 or 8 bit
	- 1 start and 1 stop bit
	- no parity
	- break generation
	- 8-level FIFO or receive and tramsmit
	- SW controlled RTS, SW readable CTS
	- auto flow control with programmable FIFO level
	- 16550 like registers
	- Baudrate derived from system clock (use force_turbo and core_freq in config.txt)
		8-times oversampling, Baudrate = CoreClock / (8*(baudrate_divider+1)),
		at 250 MHz: Baudrate = 476 Baud ... 31.25 MBaud
	- No framing error detection
	- No receive time-out interrupt
	- No DCD, DSR, DTR or RI signals

    Usual baudrates with system clock 250 MHz:

	req.Baud	divider		real Baud	error %
	-------------------------------------------------------
			65535		476.8
	477		65513		477		-0,001%
	600		52082		600		0,001%
	1200		26041		1200		-0,001%
	1800		17360		1800		0,001%
	2400		13020		2400		-0,001%
	3600		8680		3600		-0,005%
	4800		6509		4800		0,006%
	5787		5399		5787		0,001%
	7200		4339		7200		0,006%
	9600		3254		9601		0,006%
	10400		3004		10399		-0,006%
	14400		2169		14401		0,006%
	19200		1627		19195		-0,024%
	28800		1084		28802		0,006%
	38400		813		38391		-0,024%
	56000		557		56004		0,006%
	57600		542		57551		-0,086%
	115200		270		115314		0,099%
	128000		243		128074		0,058%
	134400		232		134120		-0,208%
	161280		193		161082		-0,122%
	201600		154		201613		0,006%
	230400		135		229779		-0,269%
	256000		121		256148		0,058%
	268800		115		269397		0,222%
	403200		77		400641		-0,635%
	460800		67		459559		-0,269%
	614400		50		612745		-0,269%
	806400		38		801282		-0,635%
	921600		33		919118		-0,269%
	1228800		24		1250000		1,725%
	2457600		12		2403846		-2,187%
	3000000		9		3125000		4,167%
	6000000		4		6250000		4,167%
			3		7812500
			2		10416667
			1		15625000
			0		31250000
*/

/*
SPI1, SPI2:
SPI2 is present in Pi 4 and higher chips, but it is not mapped to any GPIO pins and is therefore unusable.
	- speed up to SPI clock 125 MHz
	- single-beat bit length 1 - 32 bits
	- signle-beat variabe bit length 1 - 24 bits
	- multi-beat infinite bit length
	- 3 independent chip selects per master
	- 4 entries 32-bit wide transmit and receive FIFOs
	- data out on rising or falling clock edge
	- clock inversion (idle high or idle low)
	- shift in/out MS or LS bit first
	- SPI speed = CoreClock / (2*(divider+1)) ... range 30.5 kHz to 125MHz (on core clock 250 MHz)
*/

#ifndef _SDK_AUX_H
#define _SDK_AUX_H

// UART1 RTS level control 1..4 empty spaces left to de-assert RTS output signal
#define UART1_RTSLEVEL_1	2	// 1 empty space left
#define UART1_RTSLEVEL_2	1	// 2 empty spaces left
#define UART1_RTSLEVEL_3	0	// 3 empty spaces left
#define UART1_RTSLEVEL_4	3	// 4 empty spaces left

// SPI1/SPI2 extra data out hold time in system clocks
#define SPI12_HOLD_NO		0	// no additional clocks
#define SPI12_HOLD_1		1	// 1 additional clock
#define SPI12_HOLD_4		2	// 4 additional clocks
#define SPI12_HOLD_7		3	// 7 additional clocks

// SPI1/SPI2 pattern output CSx bits
#define SPI12_CS0		B0	// output on CS0 pin
#define SPI12_CS1		B1	// output on CS0 pin
#define SPI12_CS2		B2	// output on CS0 pin

// Mini-UART structure
typedef struct
{
	io32	IO;		// 0x00: Mini-UART I/O data
				//  if DLAB bit is 0:
				//	bit 0..7: received/written byte
				//  if DLAB bit is 1:
				//	bit 0..7: low byte of baudrate divider

	io32	IER;		// 0x04: Mini-UART interrupt enable
				//  if DLAB bit is 0:
				//	bit 0: 1=enable transmit interrupt when transmit FIFO is empty
				//	bit 1: 1=enable receive interrupt when receive FIFO holds at least 1 byte
				//  if DLAB bit is 1:
				//	bit 0..7: high byte of baudrate divider

	io32	IIR;		// 0x08: Mini-UART interrupt status
				//	bit 0: 0=interrupt is pending (read-only)
				//	bit 1,2: read: 0=no interrupt, 1=trasmit empty, 2=receive valid 3=(not possible)
				//		write '1' to bit 1 to clear receive FIFO
				//		write '1' to bit 2 to clear transmit FIFO

	io32	LCR;		// 0x0C: Mini-UART line control
				//	bit 0,1: data byte 00=7 bits, 11=8 bits
				//	bit 6: Break, 1=UART1_TX line is pulled low continuously
				//		If held for at least 12 bits times that will indicate a break condition.
				//	bit 7: DLAB, access to baudrate register 1=using MU_IO and MU_IER, 0=using MU_BAUD
				//		If must be 0 during operation.

	io32	MCR;		// 0x10: Mini-UART modem control
				//	bit 1: write 1=RTS line is low, 0=RTS line is high
				//		RTS line is output to control remote transmitter by receiver.
				//		This bit is ignored if the RTS is used for auto-flow control.

	io32	LSR;		// 0x14: (read only) Mini-UART line status
				//	bit 0: 1=receive FIFO holds at least 1 byte
				//	bit 1: 1=receiver FIFO overrun (read this register to clear it)
				//		- to read without clear, use STAT register
				//	bit 5: 1=transmit FIFO can accept at least 1 byte (not full)
				//	bit 6: 1=transmit FIFO is empty and transmitter is idle (finished shifting out the last bit)
	
	io32	MSR;		// 0x18: (read only) Mini-UART modem status
				//	bit 4: read 1=CTS line is low, 0=CTS line is high
				//		CTS line is input to control transmitter by remoter receiver.

	io32	SCRATCH;	// 0x1C: Mini-UART scratch
				//	bit 0..7: single byte storage, one whole byte extra on top, provided by SDC

	io32	CTRL;		// 0x20: Mini-UART extra control
				//	bit 0: 1=mini-UART receiver enabled
				//		when disabling receiver: disable, wait until receiver idle, then change baudrate
				//		If this bit is clear no new symbols will be accepted by the receiver.
				//		Any symbols in progress of reception will be finished.
				//	bit 1: 1=mini-UART transmitter enabled
				//		If this bit is clear no new symbols will be sent by the transmitter.
				//		Any symbols in progress of transmission will be finished.
				//	bit 2: 1=RTS auto-flow controlled by receiver, 0=RTS controlled by MU_MCR (RTS is output)
				//	bit 3: 1=CTS auto-flow controls transmitter, 0=CTS ignored by transmitter (CTS is input)
				//	bit 4..5: receiver control level RTS (de-assert: RTS line will be set to DISABLE state)
				//		0 = de-assert RTS if 3 empty spaces left
				//		1 = de-assert RTS if 2 empty spaces left
				//		2 = de-assert RTS if 1 empty spaces left
				//		3 = de-assert RTS if 4 empty spaces left
				//	bit 6: RTS flow assert level (=ENABLE) is 1=low, 0=high
				//	bit 7: CTS flow assert level (=ENABLE) is 1=low, 0=high

	io32	STAT;		// 0x24: Mini-UART extra status (read only)
				//	bit 0: 1=mini-UART receive FIFO contains at least 1 byte
				//	bit 1: 1=mini-UART transmitter FIFO can accept at least 1 more byte (= not full)
				//	bit 2: 1=receiver is idle (useful if the receiver is disabled, before change baudrate)
				//	bit 3: 1=transmitter is idle (set for short time if transmit FIFO contains data)
				//	bit 4: 1=receiver FIFO overrun (read LSR register to clear)
				//	bit 5: 1=transmit FIFO is full (= inverse of bit 1)
				//	bit 6: status of UART1_RTS line
				//	bit 7: status of UART1_CTS line
				//	bit 8: 1=transmitter FIFO is empty (can accept 8 bytes)
				//	bit 9: 1=transmitter is idle and transmit FIFO is empty (= bit 3 AND bit 8 = "Transmitter Done")
				//	bit 16..19: number of bytes in receive FIFO 0..8
				//	bit 24..27: number of bytes in transmit FIFO 0..8

	io32	BAUD;		// 0x28: Mini-UART baudrate divider
				//	bit 0..15: baudrate divider (or alternative access using DLAB=1)
				//		baudrate = CoreClock / ( 8 * (baudrate_divider + 1) )
				//		with 250 MHz is baudrate in range 477 Baud .. 31.25 MBaud
} AuxUART_t;
STATIC_ASSERT(sizeof(AuxUART_t) == 0x2C, "Incorrect AuxUART_t!");

// SPI structure
typedef struct
{
	io32	CTRL0;		// 0x00: SPI control register 0
				//	bit 0..5: shift length, number of bits to shift (only if not variable shift mode)
				//	bit 6: start out data with 0=LS (low bit), 1=MS (most significiant bit = bit 31 or 23)
				//	bit 7: 'idle' clock line is 0=low, 1=high
				//	bit 8: data is clocked OUT on 0=falling, 1=rising edge of the clock
				//	bit 9: clear FIFO, 1=receive and transmit FIFOs are held in reset and flushed, 0=normal operation
				//	bit 10: data is clocked IN on 0=falling, 1=rising edge of the clock
				//	bit 11: 1=enable SPI, 0=disable SPI but FIFO can still be written or read
				//	bit 12..13: extra data out hold time in system clocks 0=no, 1=1 clk (4 ns), 2=4 clk (16 ns), 3=7 clk (28 ns)
				//		- holds MOSI data additional more time after clock edge
				//	bit 14: variable width 1=shift length and data from TX fifo, 0=shift length from bits 0..5 of this register
				//			if variable data length from FIFO: bit 0..23=data, bit 24..28=shift length 0..24
				//	bit 15: variable CS pattern taken from 1=TX fifo (bit 14 must be set too), 0=from bits 17..19 of this register
				//			if variable data length from FIFO: bit 0..23=data, bit 29..31=CS0..CS2 pattern
				//	bit 16: 1=post-input mode (input data are taken 1 bit later)
				//	bit 17: pattern output on CS0 pin
				//	bit 18: pattern output on CS1 pin
				//	bit 19: pattern output on CS2 pin
				//	bit 20..31: SPI clock divider 0..4095, speed = CoreClock/(2*(divider+1))
				//			with 250 MHz is range 30517 Hz .. 125 MHz

	io32	CTRL1;		// 0x04: SPI control register 1
				//	bit 0: 1=receiver shift register is not cleared between transactions
				//		- it enables concatenate data in FIFO entries (you'll get 0x0081 0x8146 instead of 0x0081 0x0046)
				//		0=clear receiver shift register before each transaction
				//	bit 1: start IN data with 0=LS (low bit), 1=MS (most significiant bit)
				//	bit 6: 1=interrupt line is high when SPI is idle (no transmission, CS is high)
				//	bit 7: 1=interrupt line is high when transmit FIFO is empty (can send next data)
				//	bit 8..10: 0..7 additional clock cycles when CS goes high after end of transmission

	io32	STAT;		// 0x08: (read only) SPI status
				//	bit 0..5: number of bits still to be processed 0..32 (starts with 'shift length' and counts down)
				//	bit 6: 1=module is busy transferring data
				//	bit 7: 1=receiver FIFO is empty
				//	bit 8: 1=receiver FIFO is full
				//	bit 9: 1=transmit FIFO is empty
				//	bit 10: 1=transmit FIFO is full
				//	bit 16..19: number of entries 0..4 in receive FIFO
				//	bit 24..27: number of entries 0..4 in transmit FIFO

	io32	PEEK;		// 0x0C: SPI peek (read only), read will show top received entry, but not delete it from FIFO
	
	io32	res[4];		// 0x10 (16 = 0x10): ... reserved

	io32	IO[4];		// 0x20 (16 = 0x10): last SPI data, CS end (all 4 registers are the same - you can use any of them),
				//	send data with de-assert CS (= last entry), receive data (same as TXHOLD)

	io32	TXHOLD[4];	// 0x30 (16 = 0x10): middle SPI data, CS hold (all 4 registers are the same - you can use any of them),
				//	send data without de-assert CS (= middle entry), receive data (same as IO)
} AuxSPI_t;
STATIC_ASSERT(sizeof(AuxSPI_t) == 0x40, "Incorrect AuxSPI_t!");

// AUX structure
typedef struct
{
	io32	IRQ;		// 0x00: (read only) Auxiliary interrupt status
				//	bit 0: 1=mini-UART has interrupt pending
				//	bit 1: 1=SPI1 has interrupt pending
				//	bit 2: 1=SPI2 has interrupt pending

	io32	ENABLE;		// 0x04: Auxiliary enables (if disabled, peripheral registers are unaccessbile)
				//	bit 0: 1=mini-UART enabled (if disabled, all mini-UART registers are disabled)
				//	   If enabled, it will immediately start receiving data, expecially if RX line is low = start bit.
				//	   GPIO pins should be set up first before enabling the UART.
				//	bit 1: 1=SPI1 enabled (if disabled, all SPI1 registers are disabled)
				//	bit 2: 2=SPI2 enabled (if disabled, all SPI2 registers are disabled)

	io32	res1[14];	// 0x08 (56 = 0x38): ... reserved

	AuxUART_t uart;		// 0x40 (44 = 0x2C): UART1 (Mini-UART)

	io32	res2[5];	// 0x6C (20 = 0x14): ... reserved

	union {
	        AuxSPI_t spi[2];	// 0x80 (2*64 = 2*0x40): SPI1, SPI2
		struct {
		        AuxSPI_t spi1;	// 0x80 (64 = 0x40): SPI1
		        AuxSPI_t spi2;	// 0xC0 (64 = 0x40): SPI2
	// SPI2 is present in Pi 4 and higher chips, but it is not mapped to any GPIO pins and is therefore unusable.
		};
	};

} AUX_t;
STATIC_ASSERT(sizeof(AUX_t) == 0x100, "Incorrect AUX_t!");

// AUX peripheral
#define AUX		((AUX_t*)ARM_AUX_BASE)

// ==== UART1
// Before enable UART1, setup GPIO pins with GPIO_Func(gpio, GPIO_FUNC_AF5). Use GPIOs:
//  TXD1: GPIO14, GPIO32, GPIO40
//  RXD1: GPIO15, GPIO33, GPIO41
//  CTS1: GPIO16, GPIO30, GPIO43
//  RTS1: GPIO17, GPIO31, GPIO42

// UART1 enable/disable/check
// - If enabled, it will immediately start receiving data, expecially if RX line is low (= start bit is active).
// - If disabled, all mini-UART registers are disabled and unaccessbile.
// - GPIO pins should be set up first before enabling the UART.
// - DLAB bit should be clear to 0 for many functions.
INLINE void UART1_Enable(void) { AUX->ENABLE |= B0; }
INLINE void UART1_Disable(void) { AUX->ENABLE &= ~B0; }
INLINE Bool UART1_IsEnabled(void) { return (AUX->ENABLE & B0) != 0; }

// UART1 enable/disable receiver
//  when disabling receiver: disable, wait until receiver idle, then change baudrate
//  If receiver is disabled no new symbols will be accepted by the receiver.
//  Any symbols in progress of reception will be finished.
INLINE void UART1_RxEnable(void) { AUX->uart.CTRL |= B0; }
INLINE void UART1_RxDisable(void) { AUX->uart.CTRL &= ~B0; }

// UART1 enable/disable transmitter
//  If transmitter is disabled no new symbols will be sent by the transmitter.
//  Any symbols in progress of transmission will be finished.
INLINE void UART1_TxEnable(void) { AUX->uart.CTRL |= B1; }
INLINE void UART1_TxDisable(void) { AUX->uart.CTRL &= ~B1; }

// set/get UART1 baudrate divider 0..0xFFFF (UART1 must be enabled first, with DLAB = 0)
//  baudrate = CoreClock / ( 8 * (baudrate_divider + 1) )
//  with 250 MHz is baudrate in range 477 Baud .. 31.25 MBaud
//  To ensure a stable frequency, use parameters "force_turbo=1" and "core_freq=..." in config.txt.
INLINE void UART1_SetBaudDiv(u32 div) { AUX->uart.BAUD = div; }
INLINE u32 UART1_GetBaudDiv(void) { return AUX->uart.BAUD & 0xffff; }

// convert baudrate in Bauds (typically 476 Baud .. 31.25 MBaud) to baudrate divider (0..0xFFFF)
u32 UART1_BaudToDiv(u32 baud);

// convert baudrate divider (0..0xFFFF) to baudrate in Bauds (typically 476 Baud .. 31.25 MBaud)
u32 UART1_DivToBaud(u32 div);

// Calculate baudrate error in percent, as the difference between the desired and actual baudrate speed.
double UART1_BaudErr(u32 baud);

// set UART1 baudrate 476..31250000 Baud (or another range with CoreClock other than 250 MHz)
// UART1 must be enabled first, with DLAB = 0.
//  To ensure a stable frequency, use parameters "force_turbo=1" and "core_freq=..." in config.txt.
INLINE void UART1_SetBaud(u32 baud) { UART1_SetBaudDiv(UART1_BaudToDiv(baud)); }

// get UART1 current baudrate in Bauds (UART1 must be enabled first, with DLAB = 0)
INLINE u32 UART1_GetBaud(void) { return UART1_DivToBaud(UART1_GetBaudDiv()); }

// write byte to UART1 data without waiting
INLINE void UART1_Write(int ch) { AUX->uart.IO = ch; }

// read byte from UART1 data without waiting
INLINE u8 UART1_Read(void) { return (u8)AUX->uart.IO; }

// enable/disable UART1 transmit interrupt when transmit FIFO is empty
INLINE void UART_TxIntEnable(void) { AUX->uart.IER |= B0; }
INLINE void UART_TxIntDisable(void) { AUX->uart.IER &= ~B0; }

// enable/disable UART1 receive interrupt when receive FIFO holds at least 1 byte
INLINE void UART_RxIntEnable(void) { AUX->uart.IER |= B1; }
INLINE void UART_RxIntDisable(void) { AUX->uart.IER &= ~B1; }

// Check if UART1 interrupt is pending
INLINE Bool UART1_IntPending(void) { return (AUX->IRQ & B0) != 0; }

// check if UART1 transmit interrupt is pending (if transmit FIFO is empty)
INLINE Bool UART1_TxIntPending(void) { return (AUX->uart.IIR & (B0|B1)) == (B0|B1); }

// check if UART1 receive interrupt is pending (if receive FIFO holds data)
INLINE Bool UART1_RxIntPending(void) { return (AUX->uart.IIR & (B0|B2)) == (B0|B2); }

// clear UART1 receive FIFO
INLINE void UART1_RxFlush(void) { AUX->uart.IIR = B1; }

// clear UART1 transmit FIFO
INLINE void UART1_TxFlush(void) { AUX->uart.IIR = B2; }

// clear UART1 both FIFOs
INLINE void UART1_Flush(void) { AUX->uart.IIR = B1|B2; }

// select 7-bit or 8-bit data size
INLINE void UART1_7bit(void) { AUX->uart.LCR &= ~(B0|B1); }
INLINE void UART1_8bit(void) { AUX->uart.LCR |= B0|B1; }

// activate/deactivate BREAK signal (= hold TX line low for at least 12 bits)
INLINE void UART1_BreakOn(void) { AUX->uart.LCR |= B6; }
INLINE void UART1_BreakOff(void) { AUX->uart.LCR &= ~B6; }

// set UART1 RTS output to LOW or HIGH
INLINE void UART1_RTSLow(void) { AUX->uart.MCR |= B1; }
INLINE void UART1_RTSHigh(void) { AUX->uart.MCR &= ~B1; }

// get UART1 CTS input LOW (=0) or HIGH (=1)
INLINE int UART1_GetCTS(void) { return (AUX->uart.MSR &= B4) == 0; }

// enable/disable RTS auto-flow control
//  - if RTS auto-flow is enabled, RTS is controlled by receiver
//  - if RTS auto-flow is disabled, RTS can be controlled by software
INLINE void UART1_RTSAutoEnable(void) { AUX->uart.CTRL |= B2; }
INLINE void UART1_RTSAutoDisable(void) { AUX->uart.CTRL &= ~B2; }

// enable/disable CTS auto-flow control
//  - if CTS audo-flow is enabled, CTS controls transmitter
//  - if CTS auto-flow is disabled, CTS is ignored by transmitter
INLINE void UART1_CTSAutoEnable(void) { AUX->uart.CTRL |= B3; }
INLINE void UART1_CTSAutoDisable(void) { AUX->uart.CTRL &= ~B3; }

// set level of RTS auto-flow to UART1_RTSLEVEL_*
INLINE void UART1_RTSAutoLevel(int level) { AUX->uart.CTRL = (AUX->uart.CTRL & ~(B4|B5)) | (level << 4); }

// set RTS assert level polarity to LOW or HIGH
INLINE void UART1_RTSAssertLow(void) { AUX->uart.CTRL |= B6; }
INLINE void UART1_RTSAssertHigh(void) { AUX->uart.CTRL &= ~B6; }

// set CTS assert level polarity to LOW or HIGH
INLINE void UART1_CTSAssertLow(void) { AUX->uart.CTRL |= B7; }
INLINE void UART1_CTSAssertHigh(void) { AUX->uart.CTRL &= ~B7; }

// check if UART1 receive FIFO contains at least 1 byte (= not empty)
INLINE Bool UART1_RxIsReady(void) { return (AUX->uart.STAT & B0) != 0; }

// check UART1 receiver FIFO overrun (to clear, use UART1_RxOverClr())
INLINE Bool UART1_RxIsOver(void) { return (AUX->uart.STAT & B4) != 0; }

// clear receiver FIFO overrun error flag
INLINE void UART1_RxOverClr(void) { (void)AUX->uart.LSR; }

// check if UART1 receiver is idle (not receiving data)
// - useful if the receiver is disabled, before change baudrate
INLINE Bool UART1_RxIsIdle(void) { return (AUX->uart.STAT & B2) != 0; }

// check if UART1 transmitter FIFO can accept next byte (= not full)
INLINE Bool UART1_TxIsReady(void) { return (AUX->uart.STAT & B1) != 0; }

// check if UART1 transmitter FIFO is full (= inverse of UART1_TxIsReady())
INLINE Bool UART1_TxIsFull(void) { return (AUX->uart.STAT & B5) != 0; }

// check if UART1 transmitter FIFO is empty (= can accept 8 bytes)
INLINE Bool UART1_TxIsEmpty(void) { return (AUX->uart.STAT & B8) != 0; }

// check if UART1 transmitter is idle and transmit FIFO is empty
INLINE Bool UART1_TxIsIdle(void) { return (AUX->uart.STAT & B9) != 0; }

// get number of entries in receive FIFO 0..8
INLINE int UART1_RxEntry(void) { return (AUX->uart.STAT >> 16) & 0x0f; }

// get number of entries in transmit FIFO 0..8
INLINE int UART1_TxEntry(void) { return (AUX->uart.STAT >> 24) & 0x0f; }

// initialize UART1 with baudrate 476..31250000 Baud, 8 bits, 1 start, 1 stop, no parity, no flow control, no interrupt
//  GPIOs must be activated too - use GPIO_Func(gpio, GPIO_FUNC_AF5).
void UART1_Init(u32 baud);

// terminate UART1 (GPIOs must be deactivated too)
void UART1_Term();

// receive UART1 character with waiting
u8 UART1_RecvChar(void);

// send UART1 character with waiting
void UART1_SendChar(u8 ch);

// send UART1 data buffer with length
void UART1_SendBuf(const u8* buf, int len);

// send UART1 ASCIIZ text
void UART1_SendText(const char* text);

#if USE_STREAM	// use Data stream (lib_stream.c, lib_stream.h)

// formatted print string to UART1, with argument list (returns number of characters, without terminating 0)
u32 UART1_PrintArg(const char* fmt, va_list args);

// formatted print string to UART1, with variadic arguments (returns number of characters, without terminating 0)
NOINLINE u32 UART1_Print(const char* fmt, ...);

#endif // USE_STREAM

// ==== SPI1, SPI2
// SPI2 is present in Pi 4 and higher chips, but it is not mapped to any GPIO pins and is therefore unusable.

// Before enable SPI1/SPI2, setup GPIO pins with GPIO_Func(gpio, GPIO_FUNC_AF4). Use GPIOs:
// SPI1_CE2_N	GPIO16
// SPI1_CE1_N	GPIO17
// SPI1_CE0_N	GPIO18
// SPI1_MISO	GPIO19
// SPI1_MOSI	GPIO20
// SPI1_SCLK	GPIO21

// Only RASPPI < 4 (Pi 4 and Pi 5 does not support SPI2):
// SPI2_MISO	GPIO40
// SPI2_MOSI	GPIO41
// SPI2_SCLK	GPIO42
// SPI2_CE0_N	GPIO43
// SPI2_CE1_N	GPIO44
// SPI2_CE2_N	GPIO45

// SPI1/SPI2 enable/disable/check
// - If disabled, all SPI registers are disabled and unaccessbile.
// - GPIO pins should be set up first before enabling the SPI.
INLINE void SPI1_Enable(void) { AUX->ENABLE |= B1; }
INLINE void SPI2_Enable(void) { AUX->ENABLE |= B2; }

INLINE void SPI1_Disable(void) { AUX->ENABLE &= ~B1; }
INLINE void SPI2_Disable(void) { AUX->ENABLE &= ~B2; }

INLINE Bool SPI1_IsEnabled(void) { return (AUX->ENABLE & B1) != 0; }
INLINE Bool SPI2_IsEnabled(void) { return (AUX->ENABLE & B2) != 0; }

// Check if SPI1/SPI2 interrupt is pending
INLINE Bool SPI1_IntPending(void) { return (AUX->IRQ & B1) != 0; }
INLINE Bool SPI2_IntPending(void) { return (AUX->IRQ & B2) != 0; }

// set/get SPI1/SPI2 clock divider 0..0x0FFF (SPI1/SPI2 must be enabled first)
//  speed = CoreClock / ( 2 * (divider + 1) )
//  with 250 MHz is speed in range 30517 Hz .. 125 MHz
//  To ensure a stable frequency, use parameters "force_turbo=1" and "core_freq=..." in config.txt.
INLINE void SPI1_SetDiv(u32 div) { AUX->spi1.CTRL0 = (AUX->spi1.CTRL0 & ~(0xfff<<20)) | (div<<20); }
INLINE void SPI2_SetDiv(u32 div) { AUX->spi2.CTRL0 = (AUX->spi2.CTRL0 & ~(0xfff<<20)) | (div<<20); }

INLINE u32 SPI1_GetDiv(void) { return (AUX->spi1.CTRL0 >> 20) & 0xfff; }
INLINE u32 SPI2_GetDiv(void) { return (AUX->spi2.CTRL0 >> 20) & 0xfff; }

// convert SPI1/SPI2 speed in Hz (typically 30517 Hz .. 125 MHz) to clock divider (0..0x0FFF)
u32 SPI_SpeedToDiv(u32 speed);

// convert SPI1/SPI2 clock divider (0..0x0FFF) to speed in Hz (typically 30517 Hz .. 125 MHz)
u32 SPI_DivToSpeed(u32 div);

// Calculate SPI1/SPI2 speed error in percent, as the difference between the desired and actual speed.
double SPI_SpeedErr(u32 speed);

// set SPI1/SPI2 speed 30517..125000000 Hz (or another range with CoreClock other than 250 MHz)
// SPI1/SPI2 must be enabled first.
//  To ensure a stable frequency, use parameters "force_turbo=1" and "core_freq=..." in config.txt.
INLINE void SPI1_SetSpeed(u32 speed) { SPI1_SetDiv(SPI_SpeedToDiv(speed)); }
INLINE void SPI2_SetSpeed(u32 speed) { SPI2_SetDiv(SPI_SpeedToDiv(speed)); }

// get SPI1/SPI2 current speed in Hz (SPI1/SPI2 must be enabled first)
INLINE u32 SPI1_GetSpeed(void) { return SPI_DivToSpeed(SPI1_GetDiv()); }
INLINE u32 SPI2_GetSpeed(void) { return SPI_DivToSpeed(SPI2_GetDiv()); }

// convert SPI1/SPI2 speed in Hz (typically 30517 Hz .. 125 MHz) to clock divider (0..0x0FFF)
u32 SPI_SpeedToDiv(u32 speed);

// convert SPI1/SPI2 clock divider (0..0x0FFF) to speed in Hz (typically 30517 Hz .. 125 MHz)
u32 SPI_DivToSpeed(u32 div);

// Calculate SPI1/SPI2 speed error in percent, as the difference between the desired and actual speed.
double SPI_SpeedErr(u32 speed);

// set word length 1..32, number of bits to shift (only in fixed shift mode SPIx_FixLen())
INLINE void SPI1_SetLen(int len) { AUX->spi1.CTRL0 = (AUX->spi1.CTRL0 & ~0x3f) | len; }
INLINE void SPI2_SetLen(int len) { AUX->spi2.CTRL0 = (AUX->spi2.CTRL0 & ~0x3f) | len; }

// set transmitter shift direction - start with MSB (most significant bit) or with LSB (least significant bit)
INLINE void SPI1_OutMSB(void) { AUX->spi1.CTRL0 |= B6; }
INLINE void SPI2_OutMSB(void) { AUX->spi2.CTRL0 |= B6; }

INLINE void SPI1_OutLSB(void) { AUX->spi1.CTRL0 &= ~B6; }
INLINE void SPI2_OutLSB(void) { AUX->spi2.CTRL0 &= ~B6; }

// set receiver shift direction - start with MSB (most significant bit) or with LSB (least significant bit)
INLINE void SPI1_InMSB(void) { AUX->spi1.CTRL1 |= B1; }
INLINE void SPI2_InMSB(void) { AUX->spi2.CTRL1 |= B1; }

INLINE void SPI1_InLSB(void) { AUX->spi1.CTRL1 &= ~B1; }
INLINE void SPI2_InLSB(void) { AUX->spi2.CTRL1 &= ~B1; }

// set idle clock line to LOW or HIGH
INLINE void SPI1_IdleLow(void) { AUX->spi1.CTRL0 &= ~B7; }
INLINE void SPI2_IdleLow(void) { AUX->spi2.CTRL0 &= ~B7; }

INLINE void SPI1_IdleHigh(void) { AUX->spi1.CTRL0 |= B7; }
INLINE void SPI2_IdleHigh(void) { AUX->spi2.CTRL0 |= B7; }

// set data to be clocked OUT on Falling or Rising edge of the clock
INLINE void SPI1_OutFall(void) { AUX->spi1.CTRL0 &= ~B8; }
INLINE void SPI2_OutFall(void) { AUX->spi2.CTRL0 &= ~B8; }

INLINE void SPI1_OutRise(void) { AUX->spi1.CTRL0 |= B8; }
INLINE void SPI2_OutRise(void) { AUX->spi2.CTRL0 |= B8; }

// set data to be clocked IN on Falling or Rising edge of the clock
INLINE void SPI1_InFall(void) { AUX->spi1.CTRL0 &= ~B10; }
INLINE void SPI2_InFall(void) { AUX->spi2.CTRL0 &= ~B10; }

INLINE void SPI1_InRise(void) { AUX->spi1.CTRL0 |= B10; }
INLINE void SPI2_InRise(void) { AUX->spi2.CTRL0 |= B10; }

// Enable/Disable clearing both FIFO (must be disabled during normal operation)
INLINE void SPI1_FlushEnable(void) { AUX->spi1.CTRL0 |= B9; }
INLINE void SPI2_FlushEnable(void) { AUX->spi2.CTRL0 |= B9; }

INLINE void SPI1_FlushDisable(void) { AUX->spi1.CTRL0 &= ~B9; }
INLINE void SPI2_FlushDisable(void) { AUX->spi2.CTRL0 &= ~B9; }

INLINE void SPI1_Flush(void) { SPI1_FlushEnable(); SPI1_FlushDisable(); }
INLINE void SPI2_Flush(void) { SPI2_FlushEnable(); SPI2_FlushDisable(); }

// Soft Enable/Disable SPI1/SPI2 - FIFO can still be written or read if disable
INLINE void SPI1_SoftEnable(void) { AUX->spi1.CTRL0 |= B11; }
INLINE void SPI2_SoftEnable(void) { AUX->spi2.CTRL0 |= B11; }

INLINE void SPI1_SoftDisable(void) { AUX->spi1.CTRL0 &= ~B11; }
INLINE void SPI2_SoftDisable(void) { AUX->spi2.CTRL0 &= ~B11; }

// Set extra data out hold time in system clocks SPI12_HOLD_*
//  Holds MOSI data additional more time after clock edge.
INLINE void SPI1_SetHold(int hold) { AUX->spi1.CTRL0 = (AUX->spi1.CTRL0 & ~(3<<12)) | (hold<<12); }
INLINE void SPI2_SetHold(int hold) { AUX->spi2.CTRL0 = (AUX->spi2.CTRL0 & ~(3<<12)) | (hold<<12); }

// Set variable or fixed length of transmitted data
//  - On fixed length - length 1..32 is set by the function SPIx_SetLen().
//  - On variable length - length 1..24 is set in bits 24..28 of transmitted data
INLINE void SPI1_VarLen(void) { AUX->spi1.CTRL0 |= B14; }
INLINE void SPI2_VarLen(void) { AUX->spi2.CTRL0 |= B14; }

INLINE void SPI1_FixLen(void) { AUX->spi1.CTRL0 &= ~B14; }
INLINE void SPI2_FixLen(void) { AUX->spi2.CTRL0 &= ~B14; }

// Set variable or fixed CS pattern of transmitted data
//  - On fixed pattern - CSx pins are set with functions SPIx_CSx()
//  - On variable pattern - CSx pins are set in bits 29..31 of transmitted data (variable length must be enabled too)
INLINE void SPI1_VarCS(void) { AUX->spi1.CTRL0 |= B15; }
INLINE void SPI2_VarCS(void) { AUX->spi2.CTRL0 |= B15; }

INLINE void SPI1_FixCS(void) { AUX->spi1.CTRL0 &= ~B15; }
INLINE void SPI2_FixCS(void) { AUX->spi2.CTRL0 &= ~B15; }

// Enable/Disable post-input mode (input data are taken 1 bit later)
INLINE void SPI1_PostInEnable(void) { AUX->spi1.CTRL0 |= B16; }
INLINE void SPI2_PostInEnable(void) { AUX->spi2.CTRL0 |= B16; }

INLINE void SPI1_PostInDisable(void) { AUX->spi1.CTRL0 &= ~B16; }
INLINE void SPI2_PostInDisable(void) { AUX->spi2.CTRL0 &= ~B16; }

// Select CSx pattern output - combination of SPI12_CS* flags (only in fixed mode SPIx_FixCS())
INLINE void SPI1_CS(int cs) { AUX->spi1.CTRL0 = (AUX->spi1.CTRL0 & ~(7<<17)) | (cs<<17); }
INLINE void SPI2_CS(int cs) { AUX->spi2.CTRL0 = (AUX->spi2.CTRL0 & ~(7<<17)) | (cs<<17); }

// Enable/Disable clear receiver shift register between transactions
//  If clear is disabled, it will concatenate data in FIFO entries - you'll get 0x0081 0x8146 instead of 0x0081 0x0046.
INLINE void SPI1_ClearEnable(void) { AUX->spi1.CTRL1 &= ~B0; }
INLINE void SPI2_ClearEnable(void) { AUX->spi2.CTRL1 &= ~B0; }

INLINE void SPI1_ClearDisable(void) { AUX->spi1.CTRL1 |= B0; }
INLINE void SPI2_ClearDisable(void) { AUX->spi2.CTRL1 |= B0; }

// Enable/Disable interrupt when all data are sent and transmit shift register is empty (= all done)
INLINE void SPI1_DoneIntEnable(void) { AUX->spi1.CTRL1 |= B6; }
INLINE void SPI2_DoneIntEnable(void) { AUX->spi2.CTRL1 |= B6; }

INLINE void SPI1_DoneIntDisable(void) { AUX->spi1.CTRL1 &= ~B6; }
INLINE void SPI2_DoneIntDisable(void) { AUX->spi2.CTRL1 &= ~B6; }

// Enable/Disable interrupt when transmit FIFO is empty (= Tx empty)
INLINE void SPI1_TxEmptyIntEnable(void) { AUX->spi1.CTRL1 |= B7; }
INLINE void SPI2_TxEmptyIntEnable(void) { AUX->spi2.CTRL1 |= B7; }

INLINE void SPI1_TxEmptyIntDisable(void) { AUX->spi1.CTRL1 &= ~B7; }
INLINE void SPI2_TxEmptyIntDisable(void) { AUX->spi2.CTRL1 &= ~B7; }

// Set additional clock cycles 0..7 when CS goes high after end of transmission
INLINE void SPI1_CSClk(int clk) { AUX->spi1.CTRL1 = (AUX->spi1.CTRL1 & ~(7<<8)) | (clk<<7); }
INLINE void SPI2_CSClk(int clk) { AUX->spi2.CTRL1 = (AUX->spi2.CTRL1 & ~(7<<8)) | (clk<<7); }

// get number of remaining bits to be sent 0..32
INLINE int SPI1_Remain(void) { return AUX->spi1.STAT & 0x3f; }
INLINE int SPI2_Remain(void) { return AUX->spi2.STAT & 0x3f; }

// check if busy transferring data
INLINE Bool SPI1_IsBusy(void) { return (AUX->spi1.STAT & B6) != 0; }
INLINE Bool SPI2_IsBusy(void) { return (AUX->spi2.STAT & B6) != 0; }

// check if receiver FIFO is empty
INLINE Bool SPI1_RxIsEmpty(void) { return (AUX->spi1.STAT & B7) != 0; }
INLINE Bool SPI2_RxIsEmpty(void) { return (AUX->spi2.STAT & B7) != 0; }

// check if receiver FIFO is full
INLINE Bool SPI1_RxIsFull(void) { return (AUX->spi1.STAT & B8) != 0; }
INLINE Bool SPI2_RxIsFull(void) { return (AUX->spi2.STAT & B8) != 0; }

// check if transmit FIFO is empty
INLINE Bool SPI1_TxIsEmpty(void) { return (AUX->spi1.STAT & B9) != 0; }
INLINE Bool SPI2_TxIsEmpty(void) { return (AUX->spi2.STAT & B9) != 0; }

// check if transmit FIFO is full
INLINE Bool SPI1_TxIsFull(void) { return (AUX->spi1.STAT & B10) != 0; }
INLINE Bool SPI2_TxIsFull(void) { return (AUX->spi2.STAT & B10) != 0; }

// get number of entries 0..4 in receiver FIFO
INLINE int SPI1_RxEntry(void) { return (AUX->spi1.STAT >> 16) & 0x0f; }
INLINE int SPI2_RxEntry(void) { return (AUX->spi2.STAT >> 16) & 0x0f; }

// get number of entries 0..4 in transmit FIFO
INLINE int SPI1_TxEntry(void) { return (AUX->spi1.STAT >> 24) & 0x0f; }
INLINE int SPI2_TxEntry(void) { return (AUX->spi2.STAT >> 24) & 0x0f; }

// get received data without deleting from FIFO
INLINE u32 SPI1_Peek(void) { return AUX->spi1.PEEK; }
INLINE u32 SPI2_Peek(void) { return AUX->spi2.PEEK; }

// read data
INLINE u32 SPI1_Read(void) { return AUX->spi1.IO[0]; }
INLINE u32 SPI2_Read(void) { return AUX->spi2.IO[0]; }

// send middle data, CS continue to hold
INLINE void SPI1_Write(u32 data) { AUX->spi1.TXHOLD[0] = data; }
INLINE void SPI2_Write(u32 data) { AUX->spi2.TXHOLD[0] = data; }

// send last data, CS will de-assert
INLINE void SPI1_WriteLast(u32 data) { AUX->spi1.IO[0] = data; }
INLINE void SPI2_WriteLast(u32 data) { AUX->spi2.IO[0] = data; }

// initialize SPI2 as master, use 8-bit data, use fixed CS
//   speed ...... transfer speed in Hz (typically 30517..125000000 Hz)
//   msb ........ start with: True=MSB (most significant bit), False=LSB (least significant bit)
//   idlehigh ... idle clock line is : True=high, False=low
//   outrise .... OUT data are clocked on: True=rising edge, False=falling edge
//   inrise ..... IN data are clocked on: True=rising edge, False=falling edge
// GPIO pins should be set up first before enabling the SPI.
void SPI1_Init(u32 speed, Bool msb, Bool idlehigh, Bool outrise, Bool inrise);
void SPI2_Init(u32 speed, Bool msb, Bool idlehigh, Bool outrise, Bool inrise);

// terminate SPI1/SPI2
void SPI1_Term(void);
void SPI2_Term(void);

// write/read SPI1 data (must be initialized with SPI1_Init())
//  cs ... chip select index 0..2
//  wbuf ... write buffer (to send; NULL=not used, send default value 0xff)
//  rbuf ... read buffer (to receive; NULL=not used)
//  num ... number of bytes
void SPI1_WriteRead(int cs, const u8* wbuf, u8* rbuf, int num);
void SPI2_WriteRead(int cs, const u8* wbuf, u8* rbuf, int num);

#endif // _SDK_AUX_H
