
// ****************************************************************************
//
//                              SD host driver
//
// ****************************************************************************

#include "../../includes.h"	// includes

// SD card type
u8 SD_Type = SD_TYPE_NONE;

// current required clock
u32 SD_CurrentReqClock = SD_CLOCK_INIT;

// current real clock
u32 SD_CurrentClock = SD_CLOCK_INIT;

// current hardware time-out in [ms]
int SD_CurrentTimeOut = 500;

// Card RCA
u32 SD_CardRCA;

// media size in number of 512-byte sectors
u32 SD_MediaSize;

// SD card type name
const char SD_TypeName0[] = "-no-";
const char SD_TypeName1[] = "MMC";
const char SD_TypeName2[] = "SDv1";
const char SD_TypeName3[] = "SDv2";
const char SD_TypeName4[] = "SDHC";
const char* const SD_TypeName[SD_TYPE_NUM] = { SD_TypeName0, SD_TypeName1, SD_TypeName2, SD_TypeName3, SD_TypeName4 };

// SD commands
#define CMD0_IDLE		(0|B10)		// GO_IDLE_STATE (go to idle state), no response ... timeout 5-10ms
#define CMD1_MMCOP		1		// SEND_OP_COND (MMC), initiate initialization process, response R1
#define CMD2_SEND_CID		(2|B9)		// ALL_SEND_CID, get CID, response R2
#define CMD3_SEND_RELATIVE_ADDR	3		// SEND_RELATIVE_ADDR, response R6
//#define CMD5_SET_OP_COND	5		// IO_SET_OP_COND, query voltage range required by SDIO card, response R4
#define CMD7_SELECT_CARD	(7|B11)		// SELECT_CARD/DESELECT_CARD, response R1B
#define CMD8_IF			8		// SEND_IF_COND, only SDC V2, check voltage range, response R7 ... timeout 5-10ms
#define CMD9_CSD		(9|B9)		// SEND_CSD, read CSD register, response R2
//#define CMD10_CID		10		// SEND_CID, read CID register, response R2
#define CMD12_STOP		(12|B11)	// STOP_TRANSMISSION, stop to read data, response R1B ABORT
//#define ACMD13_STATE		13		// SD_STATUS (SDC), response R1
#define CMD13_STATUS		13		// SEND_STATUS, response R1
#define CMD16_SETLEN		16		// SET_BLOCKLEN (arg=block length), change R/W block size, response R1 ... timeout 5-10ms
#define CMD17_READ1		(17|B6)		// READ_SINGLE_BLOCK (arg=data address), response R1 ... timeout 50-100ms
//#define CMD18_READMUL		18		// READ_MULTIPLE_BLOCK (arg=data address), response R1
//#define CMD23_SETCNT		23		// SET_BLOCK_COUNT (MMC)
//#define ACMD23_ERASE		23		// SET_WR_BLK_ERASE_COUNT (SDC), for only SDC, response R1
#define CMD24_WRITE1		(24|B7)		// WRITE_SINGLE_BLOCK (arg=data address), write a block, response R1 ... timeout 50-100ms
//#define CMD25_WRITEMUL	25		// WRITE_MULTIPLE_BLOCK (arg=data address), write multiple blocks, response R1
//#define CMD27_PROGCSD		27		// PROGRAM_CSD, response R1
//#define CMD28_SETWP		28		// SET_WRITE_PROT (arg=data address), set write protection, response R1b
//#define CMD29_CLRWP		29		// CLR_WRITE_PROT (arg=data address), clear write protection, response R1b
//#define CMD30_SNDRWP		30		// SEND_WRITE_PROT (arg=data address), send write protection, response R1
//#define CMD32_ERASE_BEG	32		// ERASE_ER_BLK_START, TAG_SECTOR_START (arg=data address), response R1
//#define CMD33_ERASE_END	33		// ERASE_ER_BLK_END, TAG_SECTOR_END (arg=data address), response R1
//#define CMD34_UNTAG		34		// UNTAG_SECTOR (arg=data address), response R1
//#define CMD35_GRP_BEG		35		// TAG_ERASE_GROUP_START (arg=data address), response R1
//#define CMD36_GRP_END		36		// TAG_ERASE_GROUP_END (arg=data address), response R1
//#define CMD37_GRP_UNTAG	37		// UNTAG_ERASE_GROUP (arg=data address), response R1
//#define CMD38_ERASE		38		// ERASE (arg=stuff bits)
#define ACMD41_SDCOP		41		// APP_SEND_OP_COND (only SDC, arg=OCR contents), initiate initialization process, response R1 ... timeout 50-100ms, but up to 500ms whole operation
//#define CMD42_UNLOCK		42		// LOCK_UNLOCK (arg=stuff bits), response R1b
#define CMD55_APP		55		// APP_CMD (arg=stuff bits), leading command of ACMD<n> commads, response R1
//#define CMD58_OCR		58		// READ_OCR (arg=stuff bits), read OCR, response R3 ... timeout 5-10ms
//#define CMD59_CRC		59		// CRC_ON_OFF (arg=crc option), enable CRC, response R1

// R1 response (1 byte; R1b equals R1, with additional busy bytes):
//#define R1_IDLE_STATE		B0	// in idle state
//#define R1_ERASE_RESET	B1	// erase reset
#define R1_ILL_COMMAND		B2	// illegal command error
//#define R1_COM_CRC_ERR	B3	// CRC error
//#define R1_ERASE_ERR		B4	// erase sequence error
//#define R1_ADDR_ERR		B5	// address error
//#define R1_PARAM_ERR		B6	// parameter error

#define SDHSTS_CLEAR	0x000007F8	// word to clear SDHSTS error register
#define SDHSTS_ERRMASK	(B3|B4|B5|B6|B7) // SDHSTS error mask: FIFO error, CRC7 error, CRC16 error, CMD time out, REW time out

#define SD_CMD_TIMEOUT		10		// command time-out in [ms] ... but SDHost uses default timeout about 330 clock cycles
#define SD_READ_TIMEOUT		200		// CMD17 read time-out in [ms]
#define SD_WRITE_TIMEOUT	400		// CMD24 write time-out in [ms]
#define SD_INIT_TIMEOUT		500		// ACMD41 init time-out in [ms]

// set hardware time-out in [ms]
void SD_SetTimeout(int ms)
{
	// save timeout
	SD_CurrentTimeOut = ms;

	// set hardware timeout
	SDHost->SDTOUT = (u32)(((u64)ms * SD_CurrentClock + 500) / 1000);
}

// break current command, clear state (called after time-out)
void SD_Break(void)
{
	// prepare interface
	SDHost_t* sd = SDHost;

	// clear command - break current command
	sd->SDCMD = 0;

	// clear error flags
	sd->SDHSTS = SDHSTS_CLEAR;

	// clear data length
	sd->SDHBCT = 0;
	sd->SDHBLC = 0;

	// clear FIFO (only if reading)
	int i;
	for (i = 16; i > 0; i--) (void)sd->SDDATA;	

	// reset FSM
	for (i = 1000; i > 0; i--) if ((sd->SDEDM & 0x0f) == 0) break;

	// clear command argument
	sd->SDARG = 0;

	// short delay
	WaitUs(30);
}

// set SD Host clock in Hz (recommended - use SD_CLOCK_*)
void SD_SetClock(u32 clock)
{
	// check if already set
	if (clock == SD_CurrentReqClock) return;
	SD_CurrentReqClock = clock;

	// setup clock
	s32 div = (CoreClock + clock/2) / clock;
	div -= 2;
	if (div < 0) div = 0;
	if (div > SDCDIV_MAX_CDIV) div = SDCDIV_MAX_CDIV;
	SDHost->SDCDIV = div;

	// real clock
	div += 2;
	clock = (CoreClock + div/2) / div;
	SD_CurrentClock = clock;

	// set timeout
	SD_SetTimeout(SD_CurrentTimeOut);

	// sort delay
	WaitUs(30);
}

// stop data transmission due time-out error, clear state
void SD_StopTrans(void)
{
	// set time-out
	SD_SetTimeout(300);

	// prepare interface
	SDHost_t* sd = SDHost;

	// send STOP command (B15 = "NEW_FLAG")
	sd->SDCMD = CMD12_STOP | B15;

	// get start time in [us]
	u32 start = Time();

	// wait for end of command
	while ((sd->SDCMD & B15) != 0)	// SDCMD_NEW_FLAG - command not completed
	{
		// timeout
		if ((u32)(Time() - start) >= (u32)(200*1000)) break;
	}

	// clear state
	SD_Break();
}

// send command with argument to SD card ... need to complete with SD_FinishCmd()
//  cmd ... command, with SDCMD flags
//  arg ... argument
//  ms ... operation total timeout in [ms]
// SDHost uses hardware time-out for command.
void SD_SendCmd(int cmd, u32 arg, int ms)
{
	// prepare interface
	SDHost_t* sd = SDHost;

	// clear error flags
	sd->SDHSTS = SDHSTS_CLEAR;

	// set hardware timeout
	SD_SetTimeout(ms);

	// prepare data size and count to 1 sector
	if ((cmd == CMD17_READ1) || (cmd == CMD24_WRITE1))
	{
		sd->SDHBCT = SECTOR_SIZE;	// block size
		sd->SDHBLC = 1;			// 1 sector
	}

	// set argument
	sd->SDARG = arg;

	// set command with "NEW_FLAG"
	cb();
	sd->SDCMD = cmd | B15;
	WaitUs(20);
}

// wait for end of command (returns False on error)
Bool SD_WaitCmd(void)
{
	// get start time in [us]
	u32 start = Time();

	// prepare interface
	SDHost_t* sd = SDHost;

	// wait for end of command
	while ((sd->SDCMD & B15) != 0)	// SDCMD_NEW_FLAG - command not completed
	{
		// timeout
		if ((u32)(Time() - start) >= (u32)(SD_CMD_TIMEOUT*1000))
		{
			// break current command (called after time-out)
			SD_Break();
			return False;
		}
	}

	// settling cleanup time - minimum 40 clock cycles
	// After writing, a longer time is required - at least 20us.
	WaitUs(30);
	return True;
}

// finish current command (returns False on error)
Bool SD_FinishCmd(int cmd)
{
	// wait for end of the command
	if (!SD_WaitCmd()) return False;

	// prepare interface
	SDHost_t* sd = SDHost;

	// check result
	u32 sdcmd = sd->SDCMD;
	if ((sdcmd & B14) != 0)	// SDCMD_FAIL_FLAG
	{
		// get error flags
		u32 sdhsts = sd->SDHSTS;

		// break current command, clear state
		SD_Break();

		// ignore CRC7 error with command CMD1
		if (((sdhsts & B4) == 0) || (cmd != CMD1_MMCOP))
		{
			// kick FSM out of its wait
			u32 edm = sd->SDEDM;
			int fsm = edm & 0x0f;	// get current FSM state
			if ((fsm == 4) || (fsm == 10)) // FSM_READWAIT or FSM_WRITESTART1
			{
				sd->SDEDM = edm | B19;	// FORCE_DATA_MODE
			}
			return False;
		}
	}
	WaitUs(5);
	return True;
}

// execute simple command and finish (returns False on error)
Bool SD_Cmd(int cmd, u32 arg)
{
	// send command
	SD_SendCmd(cmd, arg, SD_CMD_TIMEOUT);

	// finish command
	return SD_FinishCmd(cmd);
}

// SD read 1 sector (returns False on error)
//  sector ... sector number
//  buffer ... pointer to destination buffer of size 512 bytes, must be u32 aligned
Bool SD_ReadSect(u32 sector, void* buffer)
{
	int len, n;
	u32 edm;

	// check if card is connected
	if (SD_Type == SD_TYPE_NONE) return False;

	// set read speed
	SD_SetClock(SD_CLOCK_READ);

	// convert sector number to offset
	if (SD_Type != SD_TYPE_SDHC) sector *= SECTOR_SIZE;

	// send command to read single block
	SD_SendCmd(CMD17_READ1, sector, SD_READ_TIMEOUT);

	// get start time in [us]
	u32 start = Time();

	// loop data
	u32* buf = (u32*)buffer;
	len = SECTOR_SIZE/4;	// sector size in words	
	SDHost_t* sd = SDHost;
	while (len > 0)
	{
		// get number of words in FIFO
		edm = sd->SDEDM;
		n = (edm >> 4) & 0x1f;	// number of words in FIFO
		
		// no data, check errors and time-out
		if (n == 0)
		{
			// check current state
			n = edm & 0x0f;		// get FSM state
			if (	(n != 2) &&	// read data
				(n != 4) &&	// read data and wait
				(n != 5))	// read CRC
			{
				// check error code
				if ((sd->SDHSTS & SDHSTS_ERRMASK) != 0)
				{
					// break current command (called after time-out)
					SD_Break();
					return False;
				}
			}

			// time-out
			if ((u32)(Time() - start) >= (u32)(SD_READ_TIMEOUT*1000))
			{
				// stop data transmission due time-out error, clear state
				SD_StopTrans();
				return False;
			}
		}
		else
		{
			// count data
			if (n > len) n = len;
			len -= n;

			// read words from the FIFO
			while (n > 0)
			{
				// read 1 word
				*buf++ = sd->SDDATA;
				n--;
			}
		}
	}

	// check error code
	if ((sd->SDHSTS & SDHSTS_ERRMASK) != 0)
	{
		// break current command (called after time-out)
		SD_Break();
		return False;
	}

	// finish command
	return SD_FinishCmd(CMD17_READ1);
}

// SD write 1 sector (returns False on error)
//  sector ... sector number
//  buffer ... pointer to source buffer of size 512 bytes, must be u32 aligned
Bool SD_WriteSect(u32 sector, const void* buffer)
{
	int len, n;
	u32 edm;

	// check if card is connected
	if (SD_Type == SD_TYPE_NONE) return False;

	// set write speed
	SD_SetClock(SD_CLOCK_WRITE);

	// convert sector number to offset
	if (SD_Type != SD_TYPE_SDHC) sector *= SECTOR_SIZE;

	// send command to write single block
	SD_SendCmd(CMD24_WRITE1, sector, SD_WRITE_TIMEOUT);

	// get start time in [us]
	u32 start = Time();

	// loop data
	const u32* buf = (const u32*)buffer;
	len = SECTOR_SIZE/4;	// sector size in words	
	SDHost_t* sd = SDHost;
	while (len > 0)
	{
		// get free space in FIFO in number of words
		edm = sd->SDEDM;
		n = SDDATA_FIFO_WORDS - ((edm >> 4) & 0x1f);	// number of free words in FIFO
		
		// no free space, check errors and time-out
		if (n == 0)
		{
			// check current state
			n = edm & 0x0f;	// get FSM state
			if (	(n != 3) &&	// write data
				(n != 10) &&	// write start 1
				(n != 11))	// write start 2
			{
				// check error code
				if ((sd->SDHSTS & SDHSTS_ERRMASK) != 0)
				{
					// break current command (called after time-out)
					SD_Break();
					return False;
				}
			}

			// time-out
			if ((u32)(Time() - start) >= (u32)(SD_WRITE_TIMEOUT*1000))
			{
				// stop data transmission due time-out error, clear state
				SD_StopTrans();
				return False;
			}
		}
		else
		{
			// count data
			if (n > len) n = len;
			len -= n;

			// write words to the FIFO
			while (n > 0)
			{
				// write 1 word
				sd->SDDATA = *buf++;
				n--;
			}
		}
	}

	// check error code
	if ((sd->SDHSTS & SDHSTS_ERRMASK) != 0)
	{
		// break current command (called after time-out)
		SD_Break();
		return False;
	}

	// finish command
	if (!SD_FinishCmd(CMD24_WRITE1)) return False;

	// A short delay may be required before the card enters the Programming state.
	WaitUs(20);

	// Waiting for the SD card to finish writing
	while (True)
	{
		// get status
		if (!SD_Cmd(CMD13_STATUS, SD_CardRCA << 16)) return False; // error, SD card was removed

		// check R1 status:
		//	bit 8: 1=ready for next data
		//	bit 9-12:	4=transfer (ready for next transfer) <<< check
		//			5=sending data
		//			6=receiving data
		//			7=programming
		//			9=data
		// programming: returns 0x00000E00
		// ready: returns 0x00000900
		if (((sd->SDRSP0 >> 8) & 0x1f) == 9) break;
	}
	WaitUs(5);
	return True;
}

// SD Host reset
// - takes 20 ms
void SD_Reset(void)
{
	// set power card OFF
	SD_PowerDisable();

	// prepare interface
	SDHost_t* sd = SDHost;

	// reset registers
	sd->SDCMD = 0;
	sd->SDARG = 0;
	sd->SDTOUT = 0x00f00000;	
	sd->SDCDIV = 0;
	sd->SDHSTS = SDHSTS_CLEAR;	// write '1' to clear
	sd->SDHCFG = 0;
	sd->SDHBCT = 0;
	sd->SDHBLC = 0;

	// Limit fifo usage due to silicon bug
	u32 t = sd->SDEDM;
	t &= ~((0x1f << 9) | (0x1f << 14));	// clear thresholds
	t |= (4 << 9) | (4 << 14);		// set thresholds to 4
	sd->SDEDM = t;

	// delay 10ms
	WaitMs(10);

	// set power card ON
	SD_PowerEnable();

	// delay 10ms
	WaitMs(10);

	// host configuration
	sd->SDHCFG = B1 | B3;	// interrupts disable, internal 4-wire bus, disable clock switching

	// set slow clock
	SD_CurrentReqClock = 0;
	SD_SetClock(SD_CLOCK_INIT);
}

// SD Host (SD Card) initialize (called from SysInit)
// - takes 20 ms
void SD_Init(void)
{
	int i;

	// Connect GPIO34-39 EMMC to Wifi
	//  GPIO34	WiFi SD1
	//  GPIO35	WiFi SD1
	//  GPIO36	WiFi SD1
	//  GPIO37	WiFi SD1
	//  GPIO38	WiFi SD1
	//  GPIO39	WiFi SD1
	for (i = 34; i <= 39; i++) GPIO_Func(i, GPIO_FUNC_AF3);

	// Connect GPIO48-53 SD Host to SD Card
	//  GPIO48	SD card CLK
	//  GPIO49	SD card CMD
	//  GPIO50	SD card DAT0
	//  GPIO51	SD card DAT1
	//  GPIO52	SD card DAT2
	//  GPIO53	SD card DAT3
	for (i = 48; i <= 53; i++)
	{
		GPIO_Func(i, GPIO_FUNC_AF0);
		GPIO_Pull(i, (i == 0) ? GPIO_PULL_OFF : GPIO_PULL_UP);
	}

	// reset
	SD_Reset();
}

// fast check if card is inserted (returns False if SD card is not inserted)
Bool SD_CardDetect(void)
{
	// offline state
	if (SD_Type == SD_TYPE_NONE)
	{
		// cleat state
		SD_Break();

		// set slow clock
		SD_SetClock(SD_CLOCK_INIT);

		// go to idle state (CMD8 works only in IDLE state)
		if (!SD_Cmd(CMD0_IDLE, 0)) return False;

		// check CMD8 (v2), CMD55 (v1) or CMD1 (MMC)
		return SD_Cmd(CMD8_IF, 0x000001AA) || SD_Cmd(CMD55_APP, 0) || SD_Cmd(CMD1_MMCOP, 0);
	}
	else
	{
		// check CMD13
		return SD_Cmd(CMD13_STATUS, SD_CardRCA << 16);
	}
}

// disconnect SD card
void SD_Disconnect(void)
{
	// invalidate disk type
	SD_Type = SD_TYPE_NONE;
}

// connect SD card (returns False on error)
Bool SD_Connect(void)
{
	int i;
	u32 res;

	// unknown card type
	SD_Type = SD_TYPE_NONE;

	// cleat state
	SD_Break();

	// set slow clock
	SD_SetClock(SD_CLOCK_INIT);

	// prepare interface
	SDHost_t* sd = SDHost;

	// go to IDLE state
	if (!SD_Cmd(CMD0_IDLE, 0)) return False;

	// check SD card presence: CMD8 (v2), CMD55 (v1) or CMD1 (MMC)
	if (!SD_Cmd(CMD8_IF, 0x000001AA) && !SD_Cmd(CMD55_APP, 0) && !SD_Cmd(CMD1_MMCOP, 0)) return False;

	// we have at least MMC card
	SD_Type = SD_TYPE_MMC;

	// check SD v2 card, switch to 3.3V ... support 2.7-3.6V ?
	res = sd->SDRSP0;
	if ((sd->SDRSP0 & 0xFFF) == 0x1AA)
		SD_Type = SD_TYPE_SD2;

	// check SD v1 card
	else
	{
		// is command ACMD41 supported?
		SD_Cmd(CMD55_APP, 0);
		if (SD_Cmd(ACMD41_SDCOP, 0x00FF8000) &&
			(((sd->SDRSP0 >> 24) & R1_ILL_COMMAND) == 0))
			SD_Type = SD_TYPE_SD1;
	}

	// get start time in [us]
	u32 start = Time();

	// wait for initialization (max. 500ms) - go from IDLE state to READY state
	while (True)
	{
		// time-out
		if ((u32)(Time() - start) >= (u32)(SD_INIT_TIMEOUT*1000))
		{
			// invalidate disk type
			SD_Type = SD_TYPE_NONE;
			return False;
		}

		// MMC card
		if (SD_Type == SD_TYPE_MMC)
		{
			if (!SD_Cmd(CMD1_MMCOP, 0))
			{
				SD_Type = SD_TYPE_NONE;
				return False;
			}
		}

		// SD v1 card
		else if (SD_Type == SD_TYPE_SD1)
		{
			SD_Cmd(CMD55_APP, 0);
			if (!SD_Cmd(ACMD41_SDCOP, 0x00FF8000))
			{
				SD_Type = SD_TYPE_NONE;
				return False;
			}
		}

		// SD v2 card
		//  ACMD41 argument:
		//	bit 31: 1=ready, 0=busy (only response)
		//	bit 30: HCS (high capacity support)
		//	bit 28: XPC (SDXC maximum performance) ... requires 150 mA power
		//	bit 24: S18R (1.8V request)
		//	bit 15-23: OCR voltage window 2.7-3.6V
		else
		{
			SD_Cmd(CMD55_APP, 0);
			if (!SD_Cmd(ACMD41_SDCOP, 0x40FF8000))
			{
				SD_Type = SD_TYPE_NONE;
				return False;
			}
		}

		// check result - "READY" flag
		res = sd->SDRSP0;
		if ((res & B31) != 0)	// bit 31: 0=busy, 1=ready
		{
			// check SDHC card type
			if ((SD_Type == SD_TYPE_SD2) && ((res & B30) != 0)) // bit 30: 0=SDSC (standard capacity), 1=SDHC/SDXC (high capacity)
			{
				SD_Type = SD_TYPE_SDHC;
			}
			break;
		}

		// short delay
		WaitMs(5);
	}

	// set clock to high speed
	SD_SetClock(SD_CLOCK_WRITE);

	// send CMD2 to get card CID, and to go from READY state to IDENT state
	if (!SD_Cmd(CMD2_SEND_CID, 0))
	{
		SD_Type = SD_TYPE_NONE;
		return False;
	}

	// send CMD3 to get card RCA
	if (!SD_Cmd(CMD3_SEND_RELATIVE_ADDR, 0))
	{
		SD_Type = SD_TYPE_NONE;
		return False;
	}

	// get card RCA
	SD_CardRCA = sd->SDRSP0 >> 16;

	// send CMD9 to read CSD
	if (!SD_Cmd(CMD9_CSD, SD_CardRCA << 16))
	{
		SD_Type = SD_TYPE_NONE;
		return False;
	}

	// read response
	u32 csd1 = sd->SDRSP1;	// bits 32..63
	u32 csd2 = sd->SDRSP2;	// bits 64..95
	u32 csd3 = sd->SDRSP3;	// bits 96..127

	// get CSD version
	int ver_csd = csd3 >> 30;

	// SDHC/SDXC (CSD ver 2.00)
	if (ver_csd == 1)
	{
		u32 c_size = ((csd2 & 0x3f) << 16) | (csd1 >> 16);	// max. index of the cluster (1 cluster = 1 sector 512 B)
		SD_MediaSize = (c_size + 1) << 10;
	}
	// SDSC (CSD ver 1.xx or MMC ver 3)
	else
	{
		u32 read_bl_len = (csd2 >> 16) & 0x0f;			// order of size of the read block (= sector, usualy 9) (length of the read block = 1 << read_bl_len)
		u32 c_size = ((csd2 & 0x3ff) << 2) | (csd1 >> 30);	// max. index of the cluster (= media size in number of clusters - 1)
		u32 c_size_mult = (csd1 >> 15) & 7;			// order of size of the cluster - 2 (in number of the read blocks)
		SD_MediaSize = (c_size + 1) << (c_size_mult + 2 + read_bl_len - 9);
	}

	// send CMD7 to select card - go from IDENT state to TRANSFER state (skip STANDBY state)
	if (!SD_Cmd(CMD7_SELECT_CARD, SD_CardRCA << 16))
	{
		SD_Type = SD_TYPE_NONE;
		return False;
	}

	// set sector length to standard 512 bytes
	if (SD_Type != SD_TYPE_SDHC)
	{
		if (!SD_Cmd(CMD16_SETLEN, SECTOR_SIZE))
		{
			SD_Type = SD_TYPE_NONE;
			return False;
		}
	}

	return True;
}
