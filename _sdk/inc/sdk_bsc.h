
// ****************************************************************************
//
//         BSC Broadcom Serial Controller Slave (I2C slave, SPI slave)
//
// ****************************************************************************
// - speed 400Kb/s
// - no DMA support
// - clock stretching is not supported
// - 7-bit addressing only

#ifndef _SDK_BSC_H
#define _SDK_BSC_H

// BSC Broadcom Serial Controller Slave controller (I2C / SPI Slave)
typedef struct {

	io32	DR;		// 0x00: data register
				//	bit 0-7: "DATA" data written to TX FIFO, or read from RX FIFO
				//	bit 8: (read only) "OE" 1=RX overrun error
				//		(to clear, write 0 to I2C SPI status register)
				//	bit 9: (read only) "UE" 1=TX underrun error
				//		(to clear, write 0 to I2C SPI status register)
				//	bit 16: (read only) "TXBUSY" 1=transmit is busy
				//	bit 17: (read only) "RXFE" 1=RX FIFO is empty
				//	bit 18: (read only) "TXFF" 1=TX FIFO is full
				//	bit 19: (read only) "RXFF" 1=RX FIFO is full
				//	bit 20: (read only) "TXFE" 1=TX FIFO is empty
				//	bit 21: (read only) "RXBUSY" 1=receive is busy
				//	bit 22-26: (read only) "TXFLEVEL" current level of the TX FIFO use (0-31)
				//	bit 27-31: (read only) "RXFLEVEL" current level of the RX FIFO use (0-31)

	io32	RSR;		// 0x04: status register, clear register
				//	bit 0: "OE" RX overrun error (cannot receive next byte), clear by writing 0
				//	bit 1: "UE" TX underrun error (cannot send next byte), clear by writing 0

	io32	SLV;		// 0x08: I2C slave address
				//	bit 0-6: "ADDR" I2C slave address
				//		Programmable I2C slave address
				//		Note: In case HOSTCTRLEN bit is set from the
				//		I2C SPI Control Register bit SLVADDR[0]
				//		chooses the following:
				//		0 - selects normal operation, i.e. accessing RX and TX FIFOs.
				//		1 - selects access to I2C SPI SW Status
				//		Register or I2C SPI Host Control Register

	io32	CR;		// 0x0C: control register
				//	bit 0: "EN" 1=enable I2C SPI slave
				//	bit 1: "SPI" 1=enable SPI mode
				//	bit 2: "I2C" 1=enable I2C mode
				//	bit 3: "CPHA" CPHA clock phase (SPI related)
				//	bit 4: "CPOL" SPOL clock polarity (SPI related)
				//	bit 5: "ENSTAT" ENABLE STATUS 8bit register
				//		0 = Status register disabled. Implies ordinary I2C protocol.
				//		1 = Status register enabled. When enabled the
				//		status register is transferred as a first data
				//		character on the I2C bus. Status register is
				//		transferred to the host.
				//		NOTE: The same behaviour is achieved from the
				//		Host side by using bit SLVADDR[6] of the slave address.
				//	bit 6: "ENCTRL" ENABLE CONTROL 8bit register
				//		0 = Control register disabled. Implies ordinary I2C protocol.
				//		1 = Control register enabled. When enabled the
				//		control register is received as a first data
				//		character on the I2C bus.
				//		NOTE: The same behaviour is achieved from the
				//		Host side by using bit SLVADDR[6] of the slave address.
				//	bit 7: "BRK" Break current operation, 1=Stop operation and clear the FIFOs
				//	bit 8: "TXE" 1=Transmit mode enabled
				//	bit 9: "RXE" 1=Receive mode enabled
				//	bit 10: "INV_RXF" Inverse RX status flags
				//		0 = default status flags
				//		When this bit is 0, bit 6 (RXFF - RX FIFO Full)
				//		will reset to a 0
				//		1 = inverted status flags
				//		When this bit is 0, bit 6 (RXFF - RX FIFO Empty)
				//		will reset to a 1
				//		* NOTE: INV_RX bit changes the default values
				//		of 7 bit as it is specified for I2C SPI GPU Host
				//		Status Register.
				//	bit 11: "TESTFIFO" 1=TEST FIFO enabled
				//	bit 12: "HOSTCTRLEN" 1=Host control enabled
				//		Note: HOSTCTRLEN allows Host to request
				//		GPUSTAT or HCTRL register. The same
				//		behaviour is achieved from the GPU side using
				//		ENSTAT and ENCTRL.
				//	bit 13: "INV_TXF" Inverse TX status flags
				//		0 = default status flags
				//		When this bit is 0, bit 6 (TXFE - TX FIFO Empty)
				//		will reset to a 1
				//		1 = inverted status flags
				//		When this bit is set, bit 6 (TXFE - TX FIFO Full)
				//		will reset to a 0
				//		* Note: INV_TX bit changes the default values of
				//		6 bit as it is specified for I2C SPI GPU Host
				//		Status Register.

	io32	FR;		// 0x10: flag register (read only)
				//	bit 0: "TXBUSY" 1=Transmit is busy
				//	bit 1: "RXFE" 1=RX FIFO is empty
				//	bit 2: "TXFF" 1=TX FIFO is full
				//	bit 3: "RXFF" 1=RX FIFO is full
				//	bit 4: "TXFE" 1=TX FIFO is empty
				//	bit 5: "RXBUSY" 1=Receive is busy
				//	bit 6-10: "TXFLEVEL" returns current level of the TX FIFO use
				//	bit 11-15: "RXFLEVEL" returns current level of the RX FIFO use

	io32	IFLS;		// 0x14: interrupt FIFO level select register
				//	bit 0-2: "TXIFLSEL" TX Interrupt FIFO Level Select
				//		Interrupt is triggered when:
				//		0 TX FIFO gets 1/8 full
				//		1 TX FIFO gets 1/4 full
				//		2 TX FIFO gets 1/2 full
				//		3 TX FIFO gets 3/4 full
				//		4 TX FIFO gets 7/8 full
				//		5,6,7 not used
				//	bit 3-5: "RXIFLSEL" RX Interrupt FIFO Level Select
				//		Interrupt is triggered when:
				//		0 RX FIFO gets 1/8 full
				//		1 RX FIFO gets 1/4 full
				//		2 RX FIFO gets 1/2 full
				//		3 RX FIFO gets 3/4 full
				//		4 RX FIFO gets 7/8 full
				//		5,6,7 not used

	io32	IMSC;		// 0x18: interrupt mask set/clear register (1=interrupt enable)
				//	bit 0: "RXIM" Receive interrupt mask
				//	bit 1: "TXIM" Transmit interrupt mask
				//	bit 2: "BEIM" Break error interrupt mask
				//	bit 3: "OEIM" Overrun error interrupt mask

	io32	RIS;		// 0x1C: raw interrupt status register (1=interrupt pending)
				//	bit 0: "RXRIS" Receive raw interrupt status
				//	bit 1: "TXRIS" Transmit raw interrupt status
				//	bit 2: "BERIS" Break error raw interrupt status
				//	bit 3: "OERIS" Overrun error raw interrupt status

	io32	MIS;		// 0x20: masked interrupt status register (1=interrupt pending and enabled)
				//	bit 0: "RXMIS" Receive masked interrupt status
				//	bit 1: "TXMIS" Transmit masked interrupt status
				//	bit 2: "BEMIS" Break error masked interrupt status
				//	bit 3: "OEMIS" Overrun error masked interrupt status

	io32	ICR;		// 0x24: interrupt clear register
				//	bit 0: "RXIC" Clear receive interrupt
				//	bit 1: "TXIC" Clear transmit interrupt
				//	bit 2: "BEIC" Clear break error interrupt
				//	bit 3: "OEIC" Clear overrun error interrupt

	io32	DMACR;		// 0x28: DMA control register (not supported)

	io32	TDR;		// 0x2C: FIFO test data
				//	bit 0-7: "DATA" Test data is written into the receive FIFO and
				//		read out of the transmit FIFO.
				//		Test Data Register enables data to be written into the receive
				//		FIFO and read out from the transmit FIFO for test purposes.

	io32	GPUSTAT;	// 0x30: GPU status register
				//	bit 0-3: "DATA" GPU to Host Status Register, SW controllable
				//		The GPU SW Status Register to be passed via I2C bus to a Host.
				//		NOTE: GPU SW Status Register is combined with the status bit
				//		coming from within I2C SPI Slave device. Hence, the I2C SPI
				//		GPU Host Status Register as it is seen by a
				//		Host is depicted on Table 1 14.

	io32	HCTRL;		// 0x34: host control register
				//	bit 0-7: "DATA" HCTRL Host Control Register, SW processing received via I2C bus
				//		The Host Control register is received from the host side via
				//		I2C bus. When ENCTRL - enable control register bit is set,
				//		the host control register is received as the first data
				//		character after the I2C address.

	io32	DEBUG1;		// 0x38: I2C debug register, bit 0-25: "DATA"

	io32	DEBUG2;		// 0x3C: SPI debug register, bit 0-23: "DATA"

} BSC_t;
STATIC_ASSERT(sizeof(BSC_t) == 0x40, "Incorrect BSC_t!");

// BSC controller address
#define BSC		((BSC_t*)ARM_BSC_SLAVE_BASE)

// GPIOs, Pi <= 3:
//		Pull	ALT3
// GPIO18	Low	BSCSL_SDA/MOSI
// GPIO19	Low	BSCSL_SCL/SCLK
// GPIO20	Low	BSCSL/MISO
// GPIO21	Low	BSCSL/CE_N

// Initialize I2C Slave interface
//  addr ... slave address 0..127
// Remap GPIOs before using.
void I2CSlave_Init(int addr);

// read data from I2C Slave
//  buf ... destination buffer
//  num ... number of bytes
//  tout ... timeout in [us]
// Returns number of received bytes ('result < num' means error)
// Initialize with I2CSlave_Init() before using.
int I2CSlave_Read(void* buf, int num, u32 tout);

// write data to I2C Slave
//  buf ... source buffer
//  num ... number of bytes
//  tout ... timeout in [us]
// Returns number of sent bytes ('result < num' means error)
// Initialize with I2CSlave_Init() before using.
int I2CSlave_Write(const void* buf, int num, u32 tout);

#endif // _SDK_BSC_H
