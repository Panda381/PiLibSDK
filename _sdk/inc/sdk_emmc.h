
// ****************************************************************************
//
//                EMMC - External Mass Media Controller (SD Card)
//
// ****************************************************************************
// Arasan EMMC/SDHCI
// Interface uses its own clock clk_emmc - should be 50 to 100 MHz.
// Sampling clock for the response and data from the card can be delayed up to 14ns.
// This interface is shared with WLAN and SD card. To enable parallel access, 
//   SDHOST driver or EMMC2 must be used.

#ifndef _SDK_EMMC_H
#define _SDK_EMMC_H

#ifdef __cplusplus
extern "C" {
#endif

//#define SD_BLOCK_SIZE		512		// sector size

// EMMC controller
typedef struct {
	io32	ARG2;		// 0x00: Argument to be issued with ACMD23

	io32	BLKSIZECNT;	// 0x04: Block size and count
				//	bit 0..9: "BLKSIZE" block size in bytes
				//	bit 16..31: "BLKCNT" number of blocks to be transferred (decreased after transfer)
				// This register must not be accessed or modified while any
				// data transfer between card and host is ongoing.
				// EMMC module restricts the maximum block size to the size of
				// the internal data FIFO which is 1k bytes.

	io32	ARG1;		// 0x08: Argument for all commands except ACMD23.
				// Must be set before command is issued.

	io32	CMDTM;		// 0x0C: Command and transfer mode
				//	bit 1: "TM_BLKCNT_EN" 1=Enable block counter for multiple block transfers
				//	bit 2-3: "TM_AUTO_CMD_EN" Select command to be send after completion of data transfer (BLKCNT=0):
				//		0 = no command
				//		1 = command CMD12
				//		2 = command CMD23
				//		3 = reserved
				//	bit 4: "TM_DAT_DIR" Direction of data transfer: 0=from host to card, 1=from card to host
				//	bit 5: "TM_MULTI_BLOCK" Type of data transfer: 0=single block, 1=multiple block
				//	bit 16-17: "CMD_RSPNS_TYPE" Type of expected response from card:
				//		0 = no response
				//		1 = 136 bits response (17 bytes)
				//		2 = 48 bits response (6 bytes)
				//		3 = 48 bits response using busy (6 bytes)
				//	bit 19: "CMD_CRCCHK_EN" 1=enable Check the responses CRC
				//	bit 20: "CMD_IXCHK_EN" 1=enable Check that response has same index as command
				//	bit 21: "CMD_ISDATA" Command involves data transfer:
				//		0 = no data transfer command
				//		1 = data transfer command
				//	bit 22-23: "CMD_TYPE" Type of command to be issued to the card:
				//		0 = normal
				//		1 = suspend (the current data transfer)
				//		2 = resume (the last data transfer)
				//		3 = abort (the current data transfer)
				//	bit 24-29: "CMD_INDEX" Index of the command to be issued to the card


	io32	RESP0;		// 0x10: Response bits 0..31 (valid if no new command was issued)
				// In case of commands CMD2 and CMD10 it contains CID[31:0]
				// and in case of command CMD9 it contains CSD[31:0].

	io32	RESP1;		// 0x14: Response bits 32..63 (valid if no new command was issued)
				// In case of commands CMD2 and CMD10 it contains CID[63:32]
				// and in case of command CMD9 it contains CSD[63:32].

	io32	RESP2;		// 0x18: Response bits 64..95 (valid if no new command was issued)
				// In case of commands CMD2 and CMD10 it contains CID[95:64]
				// and in case of command CMD9 it contains CSD[95:64].

	io32	RESP3;		// 0x1C: Response bits 96..127 (valid if no new command was issued)
				// In case of commands CMD2 and CMD10 it contains CID[127:96]
				// and in case of command CMD9 it contains CSD[127:96].

	io32	DATA;		// 0x20: Data - transfer data to/from the card

	io32	STATUS;		// 0x24: Status
				// This register contains information intended for debugging. Its values
				// change automatically according to the hardware. As it involves
				// resynchronisation between different clock domains it changes only
				// after some latency and it is easy sample the values too early.
				// Therefore it is not recommended to use this register for polling.
				// Instead use the INTERRUPT register which implements a handshake
				// mechanism which makes it impossible to miss a change when polling.
				//	bit 0: "CMD_INHIBIT" 1=Command line still used by previous command
				//	bit 1: "DAT_INHIBIT" 1=Data lines still used by previous data transfer
				//	bit 2: "DAT_ACTIVE" 1=At least one data line is active
				//	bit 8: "WRITE_TRANSFER" 1=New data can be written to EMMC
				//	bit 9: "READ_TRANSFER" 1=New data can be read from EMMC
				//	bit 20-23: "DAT_LEVEL0" Value of data lines DAT3 to DAT0
				//	bit 24: "CMD_LEVEL" Value of command line CMD
				//	bit 25-28: "DAT_LEVEL1" Value of data lines DAT7 to DAT4

	io32	CONTROL0;	// 0x28: Host configuration bits
				//	bit 1: "HCTL_DWIDTH" 1=enable using 4 data lines
				//	bit 2: "HCTL_HS_EN" 1=enable high speed mode
				//		(i.e. DAT and CMD lines change on the rising CLK edge)
				//	bit 5: "HCTL_8BIT" 1=enable using 8 data lines
				//	bit 16: "GAP_STOP" Stop the current transaction at the
				//		next block gap: 1=stop, 0=ignore
				//	bit 17: "GAP_RESTART" Restart a transaction which was
				//		stopped using the GAP_STOP bit: 1=restart, 0=ignore
				//	bit 18: "READWAIT_EN" 1=enable using DAT2 read-wait protocol
				//		for SDIO cards supporting this
				//	bit 19: "GAP_IEN" 1=enable SDIO interrupt at block gap
				//		(only valid if the HCTL_DWIDTH bit is set)
				//	bit 20: "SPI_MODE" 1=enable SPI mode
				//	bit 21: "BOOT_EN" 1=start boot mode access, 0=stop boot mode access
				//	bit 22: "ALT_BOOT_EN" 1=Enable alternate boot mode access

	io32	CONTROL1;	// 0x2C: Host configuration bits
				//	bit 0: "CLK_INTLEN" 1=Clock enable for internal EMMC clocks for power saving
				//	bit 1: "CLK_STABLE" (read-only) 1=SD clock stable
				//		CLK_STABLE seems contrary to its name only to indicate that there
				//		was a rising edge on the clk_emmc input but not that the frequency
				//		of this clock is actually stable.
				//	bit 2: "CLK_EN" 1=SD clock enable
				//	bit 5: "CLK_GENSEL" Mode of clock generation: 0=divided, 1=programmable
				//	bit 6-7: "CLK_FREQ_MS2" SD clock base divider MSBs (bits 8..9)
				//	bit 8-15: "CLK_FREQ8" SD clock base divider LSBs (bits 0..7)
				//	bit 16-19: "DATA_TOUNIT" Data timeout unit exponent:
				//			x=TMCLK*2^(x+13), or 15=disabled
				//	bit 24: "SRST_HC" 1=Enable reset the complete host circuit
				//	bit 25: "SRST_CMD" 1=Enable reset the command handling circuit
				//	bit 26: "SRST_DATA" 1=Enable reset the data handling circuit

	io32	INTERRUPT;	// 0x30: Interrupt flags (flags can be disabled using IRPT_MASK register)
				//	bit 0: "CMD_DONE" 1=Command has finished
				//	bit 1: "DATA_DONE" 1=Data transfer has finished
				//	bit 2: "BLOCK_GAP" 1=Data transfer has stopped at block gap
				//	bit 4: "WRITE_RDY" 1=Data can be written to DATA register
				//	bit 5: "READ_RDY" 1=DATA register contains data to be read
				//	bit 8: "CARD" 1=Card made interrupt request
				//	bit 12: "RETUNE" 1=Clock retune request was made
				//	bit 13: "BOOTACK" 1=Boot acknowledge has been received
				//	bit 14: "ENDBOOT" 1=Boot operation has terminated
				//	bit 15: "ERR" 1=An error has occured
				//		ERR is a generic flag and is set if any of the enabled error flags is set.
				//	bit 16: "CTO_ERR" 1=Timeout on command line
				//	bit 17: "CCRC_ERR" 1=Command CRC error
				//	bit 18: "CEND_ERR" 1=End bit on command line not 1
				//	bit 19: "CBAD_ERR" 1=Incorrect command index in response
				//	bit 20: "DTO_ERR" 1=Timeout on data line
				//	bit 21: "DCRC_ERR" 1=Data CRC error
				//	bit 22: "DEND_ERR" 1=End bit on data line not 1
				//	bit 24: "ACMD_ERR" 1=Auto command error

	io32	IRPT_MASK;	// 0x34: Interrupt flag enable
				//	bit 0: "CMD_DONE" 1=Set flag if command has finished
				//	bit 1: "DATA_DONE" 1=Set flag if data transfer has finished
				//	bit 2: "BLOCK_GAP" 1=Set flag if data transfer has stopped at block gap
				//	bit 4: "WRITE_RDY" 1=Set flag if data can be written to DATA register
				//	bit 5: "READ_RDY" 1=Set flag if DATA register contains data to be read
				//	bit 8: "CARD" 1=Set flag if card made interrupt request
				//	bit 12: "RETUNE" 1=Set flag if clock retune request was made
				//	bit 13: "BOOTACK" 1=Set flag if boot acknowledge has been received
				//	bit 14: "ENDBOOT" 1=Set flag if boot operation has terminated
				//	bit 16: "CTO_ERR" 1=Set flag if timeout on command line
				//	bit 17: "CCRC_ERR" 1=Set flag if command CRC error
				//	bit 18: "CEND_ERR" 1=Set flag if end bit on command line not 1
				//	bit 19: "CBAD_ERR" 1=Set flag if incorrect command index in response
				//	bit 20: "DTO_ERR" 1=Set flag if timeout on data line
				//	bit 21: "DCRC_ERR" 1=Set flag if data CRC error
				//	bit 22: "DEND_ERR" 1=Set flag if end bit on data line not 1
				//	bit 24: "ACMD_ERR" 1=Set flag if auto command error

	io32	IRPT_EN;	// 0x38: Interrupt generation enable
				//	bit 0: "CMD_DONE" 1=Generate interrupt if command has finished
				//	bit 1: "DATA_DONE" 1=Generate interrupt if data transfer has finished
				//	bit 2: "BLOCK_GAP" 1=Generate interrupt if data transfer has stopped at block gap
				//	bit 4: "WRITE_RDY" 1=Generate interrupt if data can be written to DATA register
				//	bit 5: "READ_RDY" 1=Generate interrupt if DATA register contains data to be read
				//	bit 8: "CARD" 1=Generate interrupt if card made interrupt request
				//	bit 12: "RETUNE" 1=Generate interrupt if clock retune request was made
				//	bit 13: "BOOTACK" 1=Generate interrupt if boot acknowledge has been received
				//	bit 14: "ENDBOOT" 1=Generate interrupt if boot operation has terminated
				//	bit 16: "CTO_ERR" 1=Generate interrupt if timeout on command line
				//	bit 17: "CCRC_ERR" 1=Generate interrupt if command CRC error
				//	bit 18: "CEND_ERR" 1=Generate interrupt if end bit on command line not 1
				//	bit 19: "CBAD_ERR" 1=Generate interrupt if incorrect command index in response
				//	bit 20: "DTO_ERR" 1=Generate interrupt if timeout on data line
				//	bit 21: "DCRC_ERR" 1=Generate interrupt if data CRC error
				//	bit 22: "DEND_ERR" 1=Generate interrupt if end bit on data line not 1
				//	bit 24: "ACMD_ERR" 1=Generate interrupt if auto command error

	io32	CONTROL2;	// 0x3C: Host configuration bits
				//	bit 0: "ACNOX_ERR" (read-only) 1=Auto command not executed due to an error
				//	bit 1: "ACTO_ERR" (read-only) 1=Timeout occurred during auto command execution
				//	bit 2: "ACCRC_ERR" (read-only) 1=Command CRC error occurred during auto command execution
				//	bit 3: "ACEND_ERR" (read-only) 1=End bit is not 1 during auto command execution
				//	bit 4: "ACBAD_ERR" (read-only) 1=Command index error occurred during auto command execution
				//	bit 7: "NOTC12_ERR" (read-only) 1=Error occurred during auto command CMD12 execution
				//	bit 16-18: "UHSMODE" Select the speed mode of the SD card:
				//			0 = SDR12
				//			1 = SDR25
				//			2 = SDR50
				//			3 = SDR104
				//			4 = DDR50
				//	bit 22: "TUNEON" Start tuning the SD clock: 1=tuning, 0=not tuned or tuning complete
				//	bit 23: "TUNED" 1=Tuned clock is used for sampling data

	io32	CAPABIL0;	// 0x40: Capabilities 0

	io32	CAPABIL1;	// 0x44: Capabilities 1

	io32	res[2];		// 0x48 (8): ... reserved

	io32	FORCE_IRPT;	// 0x50: Force interrupt event (fake interrupt events for debugging)
				//	bit 0: "CMD_DONE" 1=Raise interrupt of command has finished
				//	bit 1: "DATA_DONE" 1=Raise interrupt of data transfer has finished
				//	bit 2: "BLOCK_GAP" 1=Raise interrupt of data transfer has stopped at block gap
				//	bit 4: "WRITE_RDY" 1=Raise interrupt of data can be written to DATA register
				//	bit 5: "READ_RDY" 1=Raise interrupt of DATA register contains data to be read
				//	bit 8: "CARD" 1=Raise interrupt of card made interrupt request
				//	bit 12: "RETUNE" 1=Raise interrupt of clock retune request was made
				//	bit 13: "BOOTACK" 1=Raise interrupt of boot acknowledge has been received
				//	bit 14: "ENDBOOT" 1=Raise interrupt of boot operation has terminated
				//	bit 16: "CTO_ERR" 1=Raise interrupt of timeout on command line
				//	bit 17: "CCRC_ERR" 1=Raise interrupt of command CRC error
				//	bit 18: "CEND_ERR" 1=Raise interrupt of end bit on command line not 1
				//	bit 19: "CBAD_ERR" 1=Raise interrupt of incorrect command index in response
				//	bit 20: "DTO_ERR" 1=Raise interrupt of timeout on data line
				//	bit 21: "DCRC_ERR" 1=Raise interrupt of data CRC error
				//	bit 22: "DEND_ERR" 1=Raise interrupt of end bit on data line not 1
				//	bit 24: "ACMD_ERR" 1=Raise interrupt of auto command error

	io32	res2[7];	// 0x54 (28 = 0x1C): ... reserved

	io32	BOOT_TIMEOUT;	// 0x70: Timeout in boot mode - Number of card clock cycles after which a
				//	timeout during boot mode is flagged.

	io32	DBG_SEL;	// 0x74: Debug bus configuration
				//	bit 0: "SELECT" Submodules accessed by debug bus:
				//		0 = receiver and fifo_ctrl
				//		1 = others

	io32	res3[2];	// 0x78 (8): ... reserved

	io32	EXRDFIFO_CFG;	// 0x80: Extension FIFO configuration
				//	bit 0-2: "RD_THRSH" Read threshold in 32 bits words (0..7)
				// This register allows fine tuning the dma_req generation for paced DMA transfers when
				// reading from the card. If the extension data FIFO contains less than RD_THRSH 32
				// bits words dma_req becomes inactive until the card has filled the extension data FIFO
				// above threshold. This compensates the DMA latency.
				// When writing data to the card the extension data FIFO feeds into the EMMC module s
				// FIFO and no fine tuning is required Therefore the RD_THRSH value is in this case ignored.

	io32	EXRDFIFO_EN;	// 0x84: Extension FIFO enable
				//	bit 0: "ENABLE" 1=Enable the extension FIFO

	io32	TUNE_STEP;	// 0x88: Delay per card clock tuning step
				//	bit 0-2: "DELAY" Sampling clock delay per step:
				//		0 = 200ps typically
				//		1 = 400ps typically
				//		2 = 400ps typically
				//		3 = 600ps typically
				//		4 = 700ps typically
				//		5 = 900ps typically
				//		6 = 900ps typically
				//		7 = 1100ps typically
				// This register is used to delay the card clock when sampling the
				// returning data and command response from the card.
				// DELAY determines by how much the sampling clock is delayed per step.

	io32	TUNE_STEPS_STD;	// 0x8C: Card clock tuning steps for SDR
				//	bit 0-5: "STEPS" Number of steps (0 to 40)
				// This register is used to delay the card clock when sampling the returning data and
				// command response from the card. It determines by how many steps the sampling
				// clock is delayed in SDR mode.

	io32	TUNE_STEPS_DDR;	// 0x90: Card clock tuning steps for DDR
				//	bit 0-5: "STEPS" Number of steps (0 to 40)
				// This register is used to delay the card clock when sampling the returning data and
				// command response from the card. It determines by how many steps the sampling
				// clock is delayed in DDR mode.

	io32	res4[23];	// 0x94 (92 = 0x5C): ... reserved

	io32	SPI_INT_SPT;	// 0xF0: SPI interrupt support
				//	bit 0-7: "SELECT" 1=Interrupt independent of card select line
				// This register controls whether assertion of interrupts in SPI mode is possible
				// independent of the card select line.

	io32	res5[2];	// 0xF4 (8): ... reserved

	io32	SLOTISR_VER;	// 0xFC: Slot interrupt status and version
				//	bit 0-7: "SLOT_STATUS" Logical OR of interrupt and wakeup signal for each slot
				//	bit 16-23: "SDVERSION" Host Controller specification version
				//	bit 24-31: "VENDOR" Vendor Version Number

} EMMC_t;
STATIC_ASSERT(sizeof(EMMC_t) == 0x100, "Incorrect EMMC_t!");

#if RASPPI==4
#define EMMC	((EMMC_t*)ARM_EMMC2_BASE)
#else
#define EMMC	((EMMC_t*)ARM_EMMC_BASE)
#endif


#ifdef __cplusplus
}
#endif

#endif // _SDK_EMMC_H
