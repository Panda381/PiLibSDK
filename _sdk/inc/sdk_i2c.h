
// ****************************************************************************
//
//           I2C0, I2C1 master (BSC Broadcom Serial Controller Master)
//
// ****************************************************************************
// - 2x I2C master driver on Pi < 4, or 6x I2C master driver on Pi 4
// - I2C2 master is reserved for HDMI interface and should not be accessed by user programs
// - fast-mode: 400Kb/s
// - both 7-bit and 10-bit addressing

#ifndef _SDK_I2C_H
#define _SDK_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

// I2C controller
typedef struct {
	io32	CTRL;		// 0x00: control
				//	bit 0: "READ" packet transfer direction 0=write, 1=read
				//	bit 4-5: "CLEAR" (write-only) clear FIFO 0=no action, 1-3=clear FIFO (one shot operation)
				//		- Clearing FIFO during transfer will result abort transfer.
				//	bit 7: "ST" (write-only) 1=start new transfer (one shot operation)
				//		- If bits "CLEAR" and "ST" are set together,
				//		  FIFO is cleared before new frame is started.
				//	bit 8: "INTD" 1=generate interrupt while DONE = 1
				//	bit 9: "INTT" 1=generate interrupt while TXW = 1
				//	bit 10: "INTR" 1=generate interrupt while RXR = 1
				//	bit 15: "I2CEN" 1=enable I2C controller

	io32	STAT;		// 0x04: status (read-only, some write-1-to-clear)
				//	bit 0: "TA" 1=transfer is active, 0=controller is idle
				//	bit 1: "DONE" 1=transfer complete (write 1 to clear)
				//	bit 2: "TXW" 1=FIFO is less than 1/4 full, need write data
				//	bit 3: "RXR" 1=FIFO is 3/4 or more full, need read data
				//	bit 4: "TXD" 1=transmit FIFO has space for at least 1 byte (FIFO can accept more data), 0=transmit FIFO is full
				//	bit 5: "RXD" 1=receive FIFO contains at least 1 byte (some data from FIFO can be read), 0=receive FIFO is empty
				//	bit 6: "TXE" 1=transmit FIFO is empty, 0=transmit FIFO not empty
				//	bit 7: "RXF" 1=receive FIFO is full, 0=receive FIFO is not full
				//	bit 8: "ERR" Ack error: 1=slave has not acknowledged its address (write 1 to clear)
				//	bit 9: "CLKT" Clock Stretch Timeout: 1=Slave has held the SCL signal low
				//		(clock stretching) for longer and that specified in the I2C.CLKT register
				//		(write 1 to clear)

	io32	DLEN;		// 0x08: data length, bit 0-15 "DLEN"
				//	Writing to DLEN specifies the number of bytes to
				//	be transmitted/received. Reading from DLEN
				//	when TA = 1 or DONE = 1, returns the number
				//	of bytes still to be transmitted or received.
				//	Reading from DLEN when TA = 0 and DONE = 0,
				//	returns the last DLEN value written. DLEN can
				//	be left over multiple packets.

	io32	ADDR;		// 0x0C: slave address, bit 0-6 "ADDR"
				//   How to write to a slave using 10-bit addressing:
				//	- Assuming we are in the 'stop' state (and the FIFO is empty)
				//	1. Write the number of data bytes to written (plus one) to the DLEN register.
				//	2. Write 'XXXXXXXX' to the FIFO where 'XXXXXXXX' are the least 8 significant bits
				//	   of the 10-bit slave address.
				//	3. Write other data to be transmitted to the FIFO.
				//	4. Write '11110XX' to Slave Address Register where 'XX' are the two most significant bits
				//	   of the 10-bit address. Set CTRL.READ = 0 and CTRL.ST = 1, this will start a write transfer.
				//   How to read from a slave using 10-bit addressing:
				//	1. Write 1 to the DLEN register.
				//	2. Write 'XXXXXXXX' to the FIFO where 'XXXXXXXX' are the least 8 significant bits
				//	   of the 10-bit slave address.
				//	3. Write '11110XX' to the Slave Address Register where 'XX' are the two most significant
				//	   bits of the 10-bit address. Set CTRL.READ = 0 and CTRL.ST = 1, this will start a write transfer.
				//	4. Poll the STAT.TA bit, waiting for the transfer has started.
				//	5. Write the number of data bytes to read to the DLEN register.
				//	6. Set CTRL.READ = 1 and CTRL.ST = 1, this will send the repeat start bit, new slave address
				//	   and R/W bit (which is '1') initiating the read.

	io32	FIFO;		// 0x10: 16-byte data FIFO, bit 0-7 "DATA" (clear FIFO with "CTRL.CLEAR")

	io32	DIV;		// 0x14: clock divider, bit 0-15 "CDIV" (bit 0 is ignored and always 0)
				//	SCL = CoreClock / CDIV
				//	Where CoreClock is nominally 250 MHz. If CDIV is
				//	set to 0, the divisor is 65536. CDIV is always
				//	rounded down to an even number.
				//	The documentation and drivers incorrectly state that 0 means
				//	division by 32768 - in fact, it corresponds to division by 65536.

	io32	DEL;		// 0x18: data delay ... delay values should always be set to less than CDIV/2
				//	bit 0-15: "REDL" Rising Edge Delay (default 0x30 = 48)
				//		Number of core clock cycles to wait after the
				//		rising edge of SCL before reading the next bit of data.
				//	bit 16-31: "FEDL" Falling Edge Delay (default 0x30 = 48)
				//		Number of core clock cycles to wait after the
				//		falling edge of SCL before outputting next bit of data.

	io32	CLKT;		// 0x1C: clock stretch timeout, bit 0-15: "TOUT" Clock Stretch Timeout Value (default 0x40 = 64)
				//		Number of SCL clock cycles to wait after the
				//		rising edge of SCL before deciding that the slave
				//		is not responding.

} I2C_t;
STATIC_ASSERT(sizeof(I2C_t) == 0x20, "Incorrect I2C_t!");

#define I2C_FIFO_SIZE	16	// FIFO size

// I2C controller address
#define I2C0		((I2C_t*)ARM_I2C0_BASE)
#define I2C1		((I2C_t*)ARM_I2C1_BASE)
#define I2C(inx)	(((inx) == 0) ? I2C0 : I2C1)	// address of 0=I2C0 or 1=I2C1

#if RASPPI >= 4
#define I2C3		((I2C_t*)ARM_I2C3_BASE)
#define I2C4		((I2C_t*)ARM_I2C4_BASE)
#define I2C5		((I2C_t*)ARM_I2C5_BASE)
#define I2C6		((I2C_t*)ARM_I2C6_BASE)
#endif

// GPIOs
//	Pull	ALT0	usual function
//GPIO0	High	SDA0	I2C0 SDA, ID EEPROM HAT
//GPIO1	High	SCL0	I2C0 SCL, ID EEPROM HAT
//GPIO2	High	SDA1	I2C1 SDA
//GPIO3	High	SCL1	I2C1 SCL

// set clock divider 1..65537 (result frequency SCL = CoreClock / div)
//  - CoreClock frequency is usually 250 MHz.
//  - div is always rounded down to an even number (bit 0 is ignored)
//  - Values 0 and 1 mean division by 65536.
//  The documentation and drivers incorrectly state that 0 means
//  division by 32768 - in fact, it corresponds to division by 65536.
INLINE void I2C0_SetDiv(int div) { I2C0->DIV = div; }
INLINE void I2C1_SetDiv(int div) { I2C1->DIV = div; }
INLINE void I2C_SetDiv(int i2c, int div) { I2C(i2c)->DIV = div; }

// get clock divider 1..65537 (bit 0 it stored, but ignored and rounded down to even number)
INLINE int I2C0_GetDiv(void) { int div = I2C0->DIV & 0xffff; if (div <= 1) div += 65536; return div; }
INLINE int I2C1_GetDiv(void) { int div = I2C1->DIV & 0xffff; if (div <= 1) div += 65536; return div; }
INLINE int I2C_GetDiv(int i2c) { int div = I2C(i2c)->DIV & 0xffff; if (div <= 1) div += 65536; return div; }

// set clock in Hz (3814 Hz - 125 MHz)
void I2C_SetClock(int i2c, int clk);
INLINE void I2C0_SetClock(int clk) { I2C_SetClock(0, clk); }
INLINE void I2C1_SetClock(int clk) { I2C_SetClock(1, clk); }

// get clock in Hz (returns 3814 Hz - 125 MHz)
int I2C_GetClock(int i2c);
INLINE int I2C0_GetClock(void) { return I2C_GetClock(0); }
INLINE int I2C1_GetClock(void) { return I2C_GetClock(1); }

// set slave address 0..127
INLINE void I2C0_SetAddr(int addr) { I2C0->ADDR = addr; }
INLINE void I2C1_SetAddr(int addr) { I2C0->ADDR = addr; }
INLINE void I2C_SetAddr(int i2c, int addr) { I2C(i2c)->ADDR = addr; }

// read data from I2C
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  buf ... destination buffer
//  num ... number of bytes 1..65535
// Returns number of received bytes ('result < num' means error)
// Remap GPIOs before using.
int I2C_Read(int i2c, int addr, void* buf, int num);

// write data to I2C
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  buf ... source buffer
//  num ... number of bytes 1..65535
// Returns number of sent bytes ('result < num' means error)
// Remap GPIOs before using.
int I2C_Write(int i2c, int addr, const void* buf, int num);

#ifdef __cplusplus
}
#endif

#endif // _SDK_I2C_H
