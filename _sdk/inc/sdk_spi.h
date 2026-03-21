
// ****************************************************************************
//
//                                  SPI0 Master
//
// ****************************************************************************
// - 3 wire serial protocol Serial Peripheral Interface (SPI) or Synchronous Serial Protocol (SSP).
// - 2 wire version of SPI using single wire as bidirectional data wire
// - Implements a LoSSI Master (Low Speed Serial Interface)
// - Provides support for polled, interrupt or DMA operation.
// - 16-word write FIFO, 16-word read FIFO

// Polled transfer:
//	- set CS, CPOL, CPHA, and set TA = 1
//	- poll TXD writing bytes to FIFO, RXD reading bytes from FIFO until all data written
//	- poll DONE until goes to 1
//	- set TA = 0

// Interrupt transfer:
//	- set INTR and INTD
//	- set CS, CPOL, CPHA, and set TA = 1, this will trigger interrupt with DONE = 1
//	On interrupt:
//		- if DONE is set and data to write (= first interrupt) write up to 16 bytes to FIFO
//		- if DONE is set and no more data, set TA = 0, read trailing data from FIFO until RXD is 0
//		- if RXR is set read 12 bytes from FIFO and if more data to write, write up to 12 bytes to FIFO

// DMA transfer:
//	- 2 DMA are required, each DMA channel must be set to perform 32-bit trasfers
//	- enable DMA DREQ by setting DMAEN bit and ADCS if required
//	- program two DMA control blocks, one for each DMA controller
//	- DMA channel 1 control block should have its PER_MAP set to 'x' and should be set to
//		write 'transfer length + 1' word to FIFO. Data should comprise:
//			- a word with transfer length in bytes in top sixteeen bits
//			  and control register settings [7:0] in bottom 8 bits (TA=1, CS, CPOL, CPHA)
//			- 'transfer length' number in words of data to send
//	- DMA channel 2 control block should have its PER_MAP set to 'y' and should be set to
//		read 'transfer length' words from FIFO
//	- point each D?A channel as its CB and set its ACTIVE bit to 1
//	- on receipt of an interrupts from DMA channel 2, the transfer is complete

#ifndef _SDK_SPI_H
#define _SDK_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

// SPI
typedef struct {

	io32	CS;		// 0x00: control and status
				//	bit 0-1: "CS" chip select
				//		0 = chip select 0
				//		1 = chip select 1
				//		2 = chip select 2
				//		3 = reserved
				//	bit 2: "CPHA" clock phase
				//		0 = First SCLK transition at middle of data bit
				//		1 = First SCLK transition at beginning of data bit
				//	bit 3: "CPOL" clock polarity
				//		0 = Rest state of clock = low
				//		1 = Rest state of clock = high
				//	bit 4: "CLEAR-TX" 1=TX FIFO clear, one shot operation
				//	bit 5: "CLEAR-RX" 1=RX FIFO clear, one shot operation
				//		If CLEAR and TA are both set in the same
				//		operation, the FIFOs are cleared before the
				//		new frame is started. 
				//	bit 6: "CSPOL" chip select polarity
				//		0 = Chip select lines are active low
				//		1 = Chip select lines are active high
				//	bit 7: "TA" transfer active
				//		0 = Transfer not active./CS lines are all high
				//		(assuming CSPOL = 0). RXR and DONE are 0.
				//		Writes to SPIFIFO write data into bits -0 of
				//		SPICS allowing DMA data blocks to set mode
				//		before sending data.
				//		1 = Transfer active. /CS lines are set according
				//		to CS bits and CSPOL. Writes to SPIFIFO write
				//		data to TX FIFO.TA is cleared by a
				//		dma_frame_end pulse from the DMA controller.
				//	bit 8: "DMAEN" 1=DMA enable, Peripheral generates data requests.
				//		These will be taken in four-byte words until the SPIDLEN
				//		has been reached.
				//	bit 9: "INTD" 1=generate interrupt on transfer complete DONE = 1
				//	bit 10: "INTR" 1=generate interrupt while RXR = 1 (RX FIFO needs reading)
				//	bit 11: "ADCS" Automatically Deassert Chip Select
				//		0 = Don t automatically deassert chip select at
				//		the end of a DMA transfer chip select is
				//		manually controlled by software.
				//		1 = Automatically deassert chip select at the end
				//		of a DMA transfer (as determined by SPIDLEN)
				//	bit 12: "REN" 1=Read enable if using bidirectional mode
				//	bit 13: "LEN" LoSSI enable
				//		0 = The serial interface will behave as an SPI master.
				//		1 = The serial interface will behave as a LoSSI master.
				//	bit 14: "LMONO" (unused)
				//	bit 15: "TE_EN" (unused)
				//	bit 16: (read only) "DONE" 1=Transfer is complete. Cleared by writing
				//		more data to the TX FIFO or setting TA to 0.
				//	bit 17: (read only) "RXD" 1=RX FIFO contains at least 1 byte
				//	bit 18: (read only) "TXD" 1=TX FIFO has space for at least 1 byte
				//	bit 19: (read only) "RXR" RX FIFO needs reading (3/4 full)
				//		0 = RX FIFO is less than 3/4full (or not active TA = 0).
				//		1 = RX FIFO is 3/4 or more full. Cleared by reading sufficient
				//		data from the RX FIFO or setting TA to 0.
				//	bit 20: (read only) "RXF" 1=RX FIFO is full
				//	bit 21: "CSPOL0" chip select 0 polarity 0=low, 1=high
				//	bit 22: "CSPOL1" chip select 1 polarity 0=low, 1=high
				//	bit 23: "CSPOL2" chip select 2 polarity 0=low, 1=high
				//	bit 24: "DMA_LEN" Enable DMA mode in Lossi mode
				//	bit 25: "LEN_LONG" Enable Long data word in Lossi mode if DMA_LEN is set
				//		0 = writing to the FIFO will write a single byte
				//		1 = writing to the FIFO will write a 32 bit word

	io32	FIFO;		// 0x04: TX and RX FIFOs bit 0-31 "DATA"
				//		DMA Mode:
				//		If TA is clear, the first 32-bit write to this register
				//		will control SPIDLEN and SPICS. Subsequent
				//		reads and writes will be taken as four-byte data
				//		words to be read/written to the FIFOs
				//		Poll/Interrupt Mode (DMAEN clear, TA set)
				//		Writes to the register write bytes to TX FIFO.
				//		Reads from register read bytes from the RX FIFO

	io32	CLK;		// 0x08: clock divider, bit 0-15 "CDIV" (bit 0 is ignored): SCLK = CoreClock / CDIV
				//		Divider 0 to 65535 - rounded down to even numer, 0 means 65536

	io32	DLEN;		// 0x0C: data length, bit 0-15 "LEN": number of bytes to transfer (only for DMA mode)

	io32	LTOH;		// 0x10: LOSSI mode TOH, bit 0-3 "TOH": Output Hold delay in APB clocks.
				//		A value of 0 causes a 1 clock delay.

	io32	DC;		// 0x14: DMA DREQ controls
				//	bit 0-7: "TDREQ" DMA Write Request Threshold (default 0x20)
				//		Generate a DREQ signal to the TX DMA engine
				//		whenever the TX FIFO level is less than or equal
				//		to this amount.
				//	bit 8-15: "TPANIC" DMA Write Panic Threshold (default 0x10)
				//		Generate the Panic signal to the TX DMA engine
				//		whenever the TX FIFO level is less than or equal
				//		to this amount.
				//	bit 16-23: "RDREQ" DMA Read Request Threshold (default 0x20)
				//		Generate A DREQ to the RX DMA engine
				//		whenever the RX FIFO level is greater than this
				//		amount, (RX DREQ is also generated if the
				//		transfer has finished but the RXFIFO isn't empty).
				//	bit 24-31: "RPANIC" DMA Read Panic Threshold (default 0x30)
				//		Generate the Panic signal to the RX DMA
				//		engine whenever the RX FIFO level is greater
				//		than this amount.

} SPI_t;
STATIC_ASSERT(sizeof(SPI_t) == 0x18, "Incorrect SPI_t!");

// SPI0 interface
#define SPI0 ((SPI_t*)ARM_SPI0_BASE)

// GPIO mapping:
//		Pull	ALT0
// GPIO7	High	SPI0_CE1_N
// GPIO8	High	SPI0_CE0_N
// GPIO9	Low	SPI0_MISO
// GPIO10	Low	SPI0_MOSI
// GPIO11	Low	SPI0_SCLK

// set clock divider 1..65537 (result frequency SCLK = CoreClock / div)
//  - CoreClock frequency is usually 250 MHz.
//  - div is always rounded down to an even number (bit 0 is ignored)
//  - Values 0 and 1 mean division by 65536.
INLINE void SPI0_SetDiv(int div) { SPI0->CLK = div; }

// get clock divider 1..65537 (bit 0 it stored, but ignored and rounded down to even number)
INLINE int SPI0_GetDiv(void) { int div = SPI0->CLK & 0xffff; if (div <= 1) div += 65536; return div; }

// set clock in Hz (3814 Hz - 125 MHz)
void SPI0_SetClock(int clk);

// get clock in Hz (returns 3814 Hz - 125 MHz)
int SPI0_GetClock(void);

// initialize SPI0
//   speed ... transfer speed in Hz (typically 3814 Hz - 125 MHz)
//   cpha ... clock phase 0 or 1, first SCLK transition at 0=middle, 1=beginning of data bit
//   cpol ... clock polarity 0 or 1, rest state of clock 0=low, 1=high
// GPIO pins should be set up first before enabling the SPI.
void SPI0_Init(u32 speed, int cpha, int cpol);

// write/read SPI0 data (must be initialized with SPI0_Init())
//  cs ... chip select index 0..1
//  wbuf ... write buffer (to send; NULL=not used, send default value 0xff)
//  rbuf ... read buffer (to receive; NULL=not used)
//  num ... number of bytes
void SPI0_WriteRead(int cs, const u8* wbuf, u8* rbuf, int num);

#ifdef __cplusplus
}
#endif

#endif // _SDK_SPI_H
