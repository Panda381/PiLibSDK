
// ****************************************************************************
//
//                             PCM I2S audio
//
// ****************************************************************************

#ifndef _SDK_PCM_H
#define _SDK_PCM_H

// PCM I2S structure
typedef struct
{
	io32	CS;		// 0x00: control and status
				//	bit 0: "EN" 1=enable PCM interface
				//	bit 1: "RXON" 1=enable reception
				//	bit 2: "TXON" 1=start transmission
				//	bit 3: (write only) "TXCLR" write 1 to clear TX FIFO,
				//		 takes 2 PCM clocks (self clearing bit)
				//	bit 4: (write only) "RXCLR" write 1 to clear RX FIFO, 
				//		 takes 2 PCM clocks (self clearing bit)
				//	bit 5-6: "TXTHR" TX FIFO threshold at which point the TXW flag is set
				//		00 = set when the TX FIFO is empty
				//		01 = set when the TX FIFO is less than full
				//		10 = set when the TX FIFO is less than full
				//		11 = set when the TX FIFO is full but for one sample
				//	bit 7-8: "RXTHR" RX FIFO threshold at which point the RXR flag is set
				//		00 = set when we have a single sample in the RX FIFO
				//		01 = set when the RX FIFO is at least full
				//		10 = set when the RX FIFO is at least
				//		11 = set when the RX FIFO is full
				//	bit 9: "DMAEN" 1=DMA DREQ enable
				//		Generates a TX DMA DREQ requests whenever the TX FIFO
				//		level is lower than TXREQ or generates a RX DMA DREQ
				//		when the RX FIFO level is higher than RXREQ.
				//	bit 13: (read only) "TXSYNC" 1=FIFO is in sync,
				//		0=FIFO is out of sync. The amount of data left
				//		in the FIFO is not a multiple of that required for a
				//		frame. This takes into account if we are halfway
				//		through the frame.
				//	bit 14: (read only) "RXSYNC" 1=FIFO is in sync,
				//		0 = FIFO is out of sync. The amount of data left
				//		in the FIFO is not a multiple of that required for a
				//		frame. This takes into account if we are halfway
				//		through the frame.
				//	bit 15: (read only, write 1 to clear) "TXERR" 1=TX FIFO error
				//	bit 16: (read only, write 1 to clear) "RXERR" 1=RTX FIFO error
				//	bit 17: (read only) "TXW" 0=TX FIFO is at least TXTHR full.
				//		1=TX FIFO is less then TXTHR full.
				//	bit 18: (read only) "RXR" 0=RX FIFO is less than RXTHR full.
				//		1=RX FIFO is RXTHR or more full.
				//	bit 19: (read only) "TXD" 1=TX FIFO has space for at least 1 sample
				//	bit 20: (read only) "RXD" 1=RX FIFO contains at least 1 sample
				//	bit 21: (read only) "TXE" 1=TX FIFO is empty
				//	bit 22: (read only) "RXF" 1=RX FIFO is full
				//	bit 23: "RXSEX" 1=Sign extend the RX data. When set, the MSB
				//		of the received data channel (as set by the
				//		CHxWID parameter) is repeated in all the higher
				//		data bits up to the full 32 bit data width.
				//	bit 24: "SYNC" PCM Clock sync helper.
				//		This bit provides a software synchronisation
				//		mechanism to allow the software to detect when
				//		2 PCM clocks have occurred. It takes 2 PCM
				//		clocks before the value written to this bit will be
				//		echoed back in the read value.
				//	bit 25: "STBY" RAM Standby
				//		This bit is used to control the PCM Rams
				//		standby mode. By default this bit is 0 causing
				//		RAMs to start initially in standby mode. Rams
				//		should be released from standby prior to any
				//		transmit/receive operation. Allow for at least 4
				//		PCM clock cycles to take effect. This may or
				//		may not be implemented, depending upon the
				//		RAM libraries being used.

	io32	FIFO;		// 0x04: FIFO data

	io32	MODE;		// 0x08: mode (cannot be changed whilst PCM is running)
				//	bit 0-9: "FSLEN" Frame Sync Length
				//		Sets the frame sync length to (FSLEN) clocks.
				//		This is only used when FSM == 0.
				//		PCM_FS will remain permanently active if
				//		FSLEN >= FLEN.
				//		0 = frame sync pulse is off.
				//		1 = frame sync pulse is 1 clock wide. etc
				//	bit 10-19: "FLEN" Frame length
				//		Sets the frame length to (FLEN+1) clocks.
				//		Used only when FSM == 0.
				//		1 = frame length of 2 clocks.
				//		2 = frame length of 3 clocks. etc
				//	bit 20: "FSI" Frame Sync Invert (invert frame sync signal)
				//		0 = In master mode, FS is normally low and goes
				//		high to indicate frame sync. In slave mode, the
				//		frame starts with the clock where FS is a 1 after
				//		being a 0.
				//		1 = In master mode, FS is normally high and
				//		goes low to indicate frame sync. In slave mode,
				//		the frame starts with the clock where FS is a 0
				//		after being a 1.
				//	bit 21: "FSM" Frame Sync Mode
				//		0 = Master mode. The PCM_FS is an output and
				//		we generate the frame sync.
				//		1 = Slave mode. The PCM_FS is an input and
				//		we lock onto the incoming frame sync signal.
				//	bit 22: "CLKI" Clock Invert (invert PCM_CLK signal)
				//		0 = Outputs change on rising edge of clock,
				//		inputs are sampled on falling edge.
				//		1 = Outputs change on falling edge of clock,
				//		inputs are sampled on rising edge.
				//	bit 23: "CLKM" Clock mode
				//		0 = Master mode. The PCM CLK is an output
				//		and drives at the MCLK rate.
				//		1 = Slave mode. The PCM CLK is an input.
				//	bit 24: "FTXP" Transmit Frame Packed Mode
				//		0 = Each TX FIFO word is written into a single channel.
				//		1 = Each TX FIFO word is split into 2 16 bit
				//		words and used to fill both data channels in the
				//		same frame. The maximum channel size is 16 bits.
				//		The LS half of the word is used in the first
				//		channel of the frame.
				//	bit 25: "FRXP" Receive Frame Packed Mode
				//		0 = The data from each channel is written into the RX FIFO.
				//		1 = The data from both RX channels is merged
				//		(1st channel is in the LS half) and then written to
				//		the RX FIFO as a single 2x16 bit packed mode word.
				//		First received channel in the frame goes into the
				//		LS half word. If the received data is larger than
				//		16 bits, the upper bits are truncated. The
				//		maximum channel size is 16 bits.
				//	bit 26: "PDME" PDM Input mode enable
				//		0 = Disable PDM (classic PCM input).
				//		1 = Enable PDM input filter.
				//		Enable CIC filter on input pin for PDM inputs. In
				//		order to receive data RXON must also be set.
				//	bit 27: "PDMN" PDM decimation factor (N)
				//		0 = Decimation factor 16.
				//		1 = Decimation factor 32.
				//		Sets the decimation factor of the CIC decimation filter.
				//	bit 28: "CLK_DIS" Clock disable
				//		1 = Disable the PCM Clock.
				//		This cleanly disables the PCM clock. This
				//		enables glitch free clock switching between an
				//		internal and an uncontrollable external clock.
				//		The PCM clock can be disabled, and then the
				//		clock source switched, and then the clock reenabled.
				//		0 = Enable the PCM clock.

	io32	RXC;		// 0x0C: receive configuration
				//	bit 0-3: "CH2WID" Channel 2 width: 0=8 bits wide, 1=9 bits wide
				//		This sets the width of channel 2 in bit clocks.
				//		This field has been extended with the CH2WEX
				//		bit giving a total width of (CH2WEX* 16) +
				//		CH2WID + 8. The Maximum supported width is 32 bits.
				//	bit 4-13: "CH2POS" Channel 2 position
				//		This sets the bit clock at which the first bit (MS
				//		bit) of channel 2 data occurs in the frame.
				//		0 indicates the first clock of frame.
				//	bit 14: "CH2EN" 1=Channel 2 enabled, 0=Channel 2 disabled and no
				//		data is received from channel 2 and written to the RX FIFO.
				//	bit 15: "CH2WEX" Channel 2 Width Extension Bit
				//		This is the MSB of the channel 2 width
				//		(CH2WID). It allows widths greater than 24 bits
				//		to be programmed and is added here to keep
				//		backwards compatibility with older versions of the PCM
				//	bit 16-19: "CH1WID" Channel 1 width: 0=8 bits wide, 1=9 bits wide
				//		This sets the width of channel 1 in bit clocks.
				//		This field has been extended with the CH1WEX
				//		bit giving a total width of (CH1WEX* 16) +
				//		CH1WID + 8. The Maximum supported width is 32 bits.
				//	bit 20-29: "CH1POS" Channel 1 position
				//		This sets the bit clock at which the first bit (MS
				//		bit) of channel 1 data occurs in the frame.
				//		0 indicates the first clock of frame.
				//	bit 30: "CH1EN" 1=Channel 1 enabled, 0=Channel 1 disabled and no
				//		data is received from channel 1 and written to the RX FIFO.
				//	bit 31: "CH1WEX" Channel 1 Width Extension Bit
				//		This is the MSB of the channel 1 width
				//		(CH1WID). It allows widths greater than 24 bits
				//		to be programmed and is added here to keep
				//		backwards compatibility with older versions of the PCM

	io32	TXC;		// 0x10: transmit configuration
				//	bit 0-3: "CH2WID" Channel 2 width: 0=8 bits wide, 1=9 bits wide
				//		This sets the width of channel 2 in bit clocks.
				//		This field has been extended with the CH2WEX
				//		bit giving a total width of (CH2WEX* 16) +
				//		CH2WID + 8. The Maximum supported width is 32 bits.
				//	bit 4-13: "CH2POS" Channel 2 position
				//		This sets the bit clock at which the first bit (MS
				//		bit) of channel 2 data occurs in the frame.
				//		0 indicates the first clock of frame.
				//	bit 14: "CH2EN" 1=Channel 2 enabled, 0=Channel 2 disabled and no
				//		data is taken from the TX FIFO and transmitted on channel 2.
				//	bit 15: "CH2WEX" Channel 2 Width Extension Bit
				//		This is the MSB of the channel 2 width
				//		(CH2WID). It allows widths greater than 24 bits
				//		to be programmed and is added here to keep
				//		backwards compatibility with older versions of the PCM
				//	bit 16-19: "CH1WID" Channel 1 width: 0=8 bits wide, 1=9 bits wide
				//		This sets the width of channel 1 in bit clocks.
				//		This field has been extended with the CH1WEX
				//		bit giving a total width of (CH1WEX* 16) +
				//		CH1WID + 8. The Maximum supported width is 32 bits.
				//	bit 20-29: "CH1POS" Channel 1 position
				//		This sets the bit clock at which the first bit (MS
				//		bit) of channel 1 data occurs in the frame.
				//		0 indicates the first clock of frame.
				//	bit 30: "CH1EN" 1=Channel 1 enabled, 0=Channel 1 disabled and no
				//		data is taken from the TX FIFO and transmitted on channel 1.
				//	bit 31: "CH1WEX" Channel 1 Width Extension Bit
				//		This is the MSB of the channel 1 width
				//		(CH1WID). It allows widths greater than 24 bits
				//		to be programmed and is added here to keep
				//		backwards compatibility with older versions of the PCM

	io32	DREQ;		// 0x14: DMA request
				//	bit 0-6: "RX" RX Request Level (default 0x20)
				//		This sets the RX FIFO DREQ level. When the
				//		level is above this the PCM will assert its DMA
				//		DREQ signal to request that some more data is
				//		read out of the RX FIFO.
				//	bit 8-14: "TX" TX Request Level (default 0x30)
				//		This sets the TX FIFO DREQ level. When the
				//		level is below this the PCM will assert its DMA
				//		DREQ signal to request more data is written to the TX FIFO.
				//	bit 16-22: "RX_PANIC" RX Panic Level (default 0x30)
				//		This sets the RX FIFO Panic level. When the
				//		level is above this the PCM will assert its RX
				//		DMA Panic signal.
				//	bit 24-30: "TX_PANIC" TX Panic Level (default 0x10)
				//		This sets the TX FIFO Panic level. When the
				//		level is below this the PCM will assert its TX
				//		DMA Panic signal.

	io32	INTEN;		// 0x18: interrupt enables
				//	bit 0: "TXW" 1=TX Write Interrupt Enable
				//		Setting this bit enables interrupts from PCM
				//		block when TX FIFO level is less than the
				//		specified TXTHR level.
				//	bit 1: "RXR" 1=RX Read Interrupt Enable
				//		Setting this bit enables interrupts from PCM
				//		block when RX FIFO level is greater than or
				//		equal to the specified RXTHR level.
				//	bit 2: "TXERR" 1=TX Error Interrupt
				//		Setting this bit enables interrupts from PCM
				//		block when TX FIFO error occurs.
				//	bit 3: "RXERR" 1=RX Error Interrupt
				//		Setting this bit enables interrupts from PCM
				//		block when RX FIFO error occurs.

	io32	INTSTC;		// 0x1C: interrupt status and clear (write 1 to clear)
				//	bit 0: "TXW" 1=interrupt occurred on TX Write.
				//	bit 1: "RXR" 1=interrupt occurred on RX Read.
				//	bit 2: "TXERR" 1=interrupt occurred on TX FIFO Error.
				//	bit 3: "RXERR" 1=interrupt occurred on RX FIFO Error.

	io32	GRAY;		// 0x20: gray mode control
				//	bit 0: "EN" 1=Enable GRAY mode
				//		Setting this bit will put the PCM into GRAY
				//		mode. In gray mode the data is received on the
				//		data in and the frame sync pins. The data is
				//		expected to be in data/strobe format.
				//	bit 1: "CLR" 1=Clear the GRAY Mode Logic
				//		This Bit will reset all the GRAY mode logic, and
				//		flush the RX buffer. It is not self clearing.
				//	bit 2: "FLUSH" 1=Flush the RX Buffer into the RX FIFO
				//		This forces the RX Buffer to do an early write.
				//		This is necessary if we have reached the end of
				//		the message and we have bits left in the RX
				//		Buffer. Flushing will write these bits as a single
				//		32 bit word, starting at bit zero. Empty bits will be
				//		packed with zeros. The number of bits written
				//		will be recorded in the FLUSHED Field.
				//		This bit is written as a 1 to initiate a flush. It will
				//		read back as a zero until the flush operation has
				//		completed (as the PCM Clock may be very slow).
				//	bit 4-9: (read only) "RXLEVEL" The Current fill level of the RX Buffer
				//		This indicates how many GRAY coded bits have
				//		been received. When 32 bits are received, they
				//		are written out into the RXFIFO.
				//	bit 10-15: (read only) "FLUSHED" The Number of bits that were
				//		flushed into the RXFIFO
				//		This indicates how many bits were valid when
				//		the flush operation was performed. The valid bits
				//		are from bit 0 upwards. Non-valid bits are set to zero.
				//	bit 16-21: (read only) "RXFIFOLEVEL" The Current level of the RXFIFO
				//		This indicates how many words are currently in the RXFIFO.

} PCM_t;
STATIC_ASSERT(sizeof(PCM_t) == 0x24, "Incorrect PCM_t!");

#define PCM	((PCM_t*)ARM_PCM_BASE)

#endif // _SDK_PCM_H
