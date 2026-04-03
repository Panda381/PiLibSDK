
// ****************************************************************************
//
//                              SD host driver
//
// ****************************************************************************

#ifndef _SDK_SDHOST_H
#define _SDK_SDHOST_H

#define SECTOR_SIZE		512		// sector size
#define SDDATA_FIFO_WORDS	16		// size of data FIFO in number of words
#define SDCDIV_MAX_CDIV		0x7ff		// SDCDIV max. clock divider

// SD Host controller
typedef struct {

	io32	SDCMD;		// 0x00: Command to SD card
				//	bit 0..5: "CMD" command index (SDCMD_CMD_MASK = 0x3F)
				//	bit 6: "READ_CMD" read command (SDCMD_READ_CMD = 0x40)
				//	bit 7: "WRITE_CMD" write command (SDCMD_WRITE_CMD = 0x80)
				//	bit 9: "LONG_RESPONSE" (SDCMD_LONG_RESPONSE = 0x200)
				//	bit 10: "NO_RESPONSE" (SDCMD_NO_RESPONSE = 0x400)
				//	bit 11: "BUSYWAIT" (SDCMD_BUSYWAIT = 0x800)
				//	bit 14: "FAIL_FLAG" fail flag (SDCMD_FAIL_FLAG = 0x4000)
				//	bit 15: "NEW_FLAG" new command flag - cleared if command completed (SDCMD_NEW_FLAG = 0x8000)

	io32	SDARG;		// 0x04: Argument to SD card

	io32	SDTOUT;		// 0x08: Start value for timeout counter

	io32	SDCDIV;		// 0x0C: Start value for clock divider (max. 0x7FF) (SDCDIV_MAX_CDIV = 0x7ff)
				//	clock = system_clock (250 MHz) / (SDCDIV + 2)

	io32	SDRSP0;		// 0x10: (read-only) SD card response (bits 0..31)

	io32	SDRSP1;		// 0x14: (read-only) SD card response (bits 32..63)

	io32	SDRSP2;		// 0x18: (read-only) SD card response (bits 64..95)

	io32	SDRSP3;		// 0x1C: (read-only) SD card response (bits 96..127)

	io32	SDHSTS;		// 0x20: (read-only, to clear write "1") SD host status and interrupt flags
				//	bit 0: "DATA_FLAG" (SDHSTS_DATA_FLAG = 0x01)
				//	bit 3: "FIFO_ERROR" (SDHSTS_FIFO_ERROR = 0x08)
				//	bit 4: "CRC7_ERROR" (SDHSTS_CRC7_ERROR = 0x10)
				//	bit 5: "CRC16_ERROR" (SDHSTS_CRC16_ERROR = 0x20)
				//	bit 6: "CMD_TIME_OUT" (SDHSTS_CMD_TIME_OUT = 0x40)
				//	bit 7: "REW_TIME_OUT" (SDHSTS_REW_TIME_OUT = 0x80)
				//	bit 8: "SDIO_IRPT" (SDHSTS_SDIO_IRPT = 0x100)
				//	bit 9: "BLOCK_IRPT" (SDHSTS_BLOCK_IRPT = 0x200)
				//	bit 10: "BUSY_IRPT" (SDHSTS_BUSY_IRPT = 0x400)

//#define SDHSTS_TRANSFER_ERROR_MASK	(SDHSTS_CRC7_ERROR | SDHSTS_CRC16_ERROR | SDHSTS_REW_TIME_OUT | SDHSTS_FIFO_ERROR)
//#define SDHSTS_ERROR_MASK		(SDHSTS_CMD_TIME_OUT | SDHSTS_TRANSFER_ERROR_MASK)

	io32	res[3];		// 0x24 (12 = 0x0C): ... reserved

	io32	SDVDD;		// 0x30: SD card power control
				//	0=power OFF, 1=power ON

	io32	SDEDM;		// 0x34: Emergency Debug Mode and FIFO status
				//	bit 0..3: FSM mode 0..15: (SDEDM_FSM_MASK = 0xf)
				//		0 = "FSM_IDENTMODE" (SDEDM_FSM_IDENTMODE = 0x0)
				//		1 = "FSM_DATAMODE" (SDEDM_FSM_DATAMODE = 0x1)
				//		2 = "FSM_READDATA" (SDEDM_FSM_READDATA = 0x2)
				//		3 = "FSM_WRITEDATA" (SDEDM_FSM_WRITEDATA = 0x3)
				//		4 = "FSM_READWAIT" (SDEDM_FSM_READWAIT = 0x4)
				//		5 = "FSM_READCRC" (SDEDM_FSM_READCRC = 0x5)
				//		6 = "FSM_WRITECRC" (SDEDM_FSM_WRITECRC = 0x6)
				//		7 = "FSM_WRITEWAIT1" (SDEDM_FSM_WRITEWAIT1 = 0x7)
				//		8 = "FSM_POWERDOWN" (SDEDM_FSM_POWERDOWN = 0x8)
				//		9 = "FSM_POWERUP" (SDEDM_FSM_POWERUP = 0x9)
				//		10 = "FSM_WRITESTART1" (SDEDM_FSM_WRITESTART1 = 0xa)
				//		11 = "FSM_WRITESTART2" (SDEDM_FSM_WRITESTART2 = 0xb)
				//		12 = "FSM_GENPULSES" (SDEDM_FSM_GENPULSES = 0xc)
				//		13 = "FSM_WRITEWAIT2" (SDEDM_FSM_WRITEWAIT2 = 0xd)
				//		15 = "FSM_STARTPOWDOWN" (SDEDM_FSM_STARTPOWDOWN = 0xf)
				//	bit 4-8: number of words in FIFO
				//	bit 9-13: "WRITE_THRESHOLD" write threshold 0..31 (SDEDM_WRITE_THRESHOLD_SHIFT = 9)
				//	bit 14-18: "READ_THRESHOLD" read threshold 0..31 (SDEDM_READ_THRESHOLD_SHIFT = 14)
				//	bit 19: "FORCE_DATA_MODE" (SDEDM_FORCE_DATA_MODE = (1<<19))
				//	bit 20: "CLOCK_PULSE" (SDEDM_CLOCK_PULSE = (1<<20))
				//	bit 21: "BYPASS" (SDEDM_BYPASS = (1<<21))

	io32	SDHCFG;		// 0x38: Host configuration
				//	bit 0: "REL_CMD_LINE" (SDHCFG_REL_CMD_LINE = (1<<0))
				//	bit 1: "WIDE_INT_BUS" enable internal 4-wire bus (SDHCFG_WIDE_INT_BUS = (1<<1))
				//	bit 2: "WIDE_EXT_BUS" enable external 4-wire bus ... do not use on Pi (SDHCFG_WIDE_EXT_BUS = (1<<2))
				//	bit 3: "SLOW_CARD" Disable high-speed mode - disable clever clock switching (SDHCFG_SLOW_CARD = (1<<3))
				//	bit 4: "DATA_IRPT_EN" data interrupt enable (SDHCFG_DATA_IRPT_EN = (1<<4))
				//	bit 5: "SDIO_IRPT_EN" SDIO interrupt enable (SDHCFG_SDIO_IRPT_EN = (1<<5))
				//	bit 8: "BLOCK_IRPT_EN" block interrupt enable (SDHCFG_BLOCK_IRPT_EN = (1<<8))
				//	bit 10: "BUSY_IRPT_EN" busy interrupt enable (SDHCFG_BUSY_IRPT_EN = (1<<10))

	io32	SDHBCT;		// 0x3C: Host byte count (debug), block size

	io32	SDDATA;		// 0x40: FIFO data to/from SD card (16 FIFO words)

	io32	res2[3];	// 0x44 (12 = 0x0C): ... reserved

	io32	SDHBLC;		// 0x50: Host block count (SDIO/SDHC)

} SDHost_t;
STATIC_ASSERT(sizeof(SDHost_t) == 0x54, "Incorrect SDHost_t!");

#define SDHost	((SDHost_t*)ARM_SDHOST_BASE)

// SD card type
#define SD_TYPE_NONE		0	// unknown type
#define SD_TYPE_MMC		1	// MMC
#define SD_TYPE_SD1		2	// SD v1
#define SD_TYPE_SD2		3 	// SD v2
#define SD_TYPE_SDHC		4 	// SDHC, block device

#define SD_TYPE_NUM		5

// SD Clock Frequencies (in Hz)
#define SD_CLOCK_INIT		400000		// 0.4 MHz ... init (initialization phase of the SD card requires 400 kHz)
#define SD_CLOCK_NORMAL		25000000	// 25 MHz ... normal speed

#ifndef SD_CLOCK_READ
#define SD_CLOCK_READ		SD_CLOCK_NORMAL	// read speed
#endif
#ifndef SD_CLOCK_WRITE
#define SD_CLOCK_WRITE		SD_CLOCK_NORMAL	// write speed
#endif

// SD card type SD_TYPE_*
extern u8 SD_Type;

// Card RCA
extern u32 SD_CardRCA;

// media size in number of 512-byte sectors
extern u32 SD_MediaSize;

// SD card type name
extern const char* const SD_TypeName[SD_TYPE_NUM];

// Power card enable/disable
INLINE void SD_PowerEnable(void) { SDHost->SDVDD = 1; }
INLINE void SD_PowerDisable(void) { SDHost->SDVDD = 0; }

// set hardware time-out in [ms]
void SD_SetTimeout(int ms);

// break current command, clear state (called after time-out)
void SD_Break(void);

// set SD Host clock in Hz (recommended - use SD_CLOCK_*)
void SD_SetClock(u32 clock);

// stop data transmission due time-out error, clear state
void SD_StopTrans(void);

// send command with argument to SD card ... need to complete with SD_FinishCmd()
//  cmd ... command, with SDCMD flags
//  arg ... argument
//  ms ... operation total timeout in [ms]
// SDHost uses hardware time-out for command.
void SD_SendCmd(int cmd, u32 arg, int ms);

// wait for end of command (returns False on error)
Bool SD_WaitCmd(void);

// finish current command (returns False on error)
Bool SD_FinishCmd(int cmd);

// execute simple command and finish (returns False on error)
Bool SD_Cmd(int cmd, u32 arg);

// SD read 1 sector (returns False on error)
//  sector ... sector number
//  buffer ... pointer to destination buffer of size 512 bytes, must be u32 aligned
Bool SD_ReadSect(u32 sector, void* buffer);

// SD write 1 sector (returns False on error)
//  sector ... sector number
//  buffer ... pointer to source buffer of size 512 bytes, must be u32 aligned
Bool SD_WriteSect(u32 sector, const void* buffer);

// SD Host reset
// - takes 20 ms
void SD_Reset(void);

// SD Host (SD Card) initialize (called from SysInit)
// - takes 20 ms
void SD_Init(void);

// fast check if card is inserted (returns False if SD card is not inserted)
Bool SD_CardDetect(void);

// disconnect SD card
void SD_Disconnect(void);

// connect SD card (returns False on error)
Bool SD_Connect(void);

#endif // _SDK_SDHOST_H
