
// ****************************************************************************
//
//                EMMC - External Mass Media Controller (SD Card)
//
// ****************************************************************************

#include "../../includes.h"	// includes

// SD Clock Frequencies (in Hz)
#define SD_CLOCK_ID         400000		// 0.4 MHz
#define SD_CLOCK_NORMAL     25000000		// 25 MHz
#define SD_CLOCK_HIGH       50000000		// 50 MHz
#define SD_CLOCK_100        100000000		// 100 MHz
#define SD_CLOCK_208        208000000		// 208 MHz

// commands - CMDTM register
#define SD_CMD_BLKCNT_EN		(1 << 1)	// Enable block counter for multiple block transfers
#define SD_CMD_AUTO_CMD_EN_CMD12	(1 << 2)	// Command sent after completion: CMD12
#define SD_CMD_AUTO_CMD_EN_CMD23	(2 << 2)	// Command sent after completion: CMD23
#define SD_CMD_DAT_DIR_CH		(1 << 4)	// Direction: from card to host
#define SD_CMD_DAT_DIR_HC		(0 << 4)	// Direction: from host to card
#define SD_CMD_MULTI_BLOCK		(1 << 5)	// Type of data transfer: multiple block
#define SD_CMD_RSPNS_TYPE_NONE		(0 << 16)	// No response
#define SD_CMD_RSPNS_TYPE_136		(1 << 16)	// 136-bits response (17 bytes): R2 (with CRC), R3,4 (no CRC)
#define SD_CMD_RSPNS_TYPE_48		(2 << 16)	// 48-bits response (6 bytes): R1, R5, R6, R7 (with CRC)
#define SD_CMD_RSPNS_TYPE_48B		(3 << 16)	// 48-bits response with busy (6 bytes): R1b, R5b (with CRC)
#define SD_CMD_CRCCHK_EN		(1 << 19)	// enable Check the responses CRC
#define SD_CMD_IXCHK_EN			(1 << 20)	// enable Check that response has same index as command
#define SD_CMD_ISDATA			(1 << 21)	// data transfer command
#define SD_CMD_TYPE_NORMAL		(0 << 22)	// type of command: normal 
#define SD_CMD_TYPE_SUSPEND		(1 << 22)	// type of command: suspend (current data transfer)
#define SD_CMD_TYPE_RESUME		(2 << 22)	// type of command: resume (last data transfer)
#define SD_CMD_TYPE_ABORT		(3 << 22)	// type of command: abort (current data transfer)
#define SD_CMD_INDEX(a)			((a) << 24)	// index of the command

#define SD_CMD_TYPE_MASK		(3 << 22)	// Mask of type of command

#define SD_CMD_RSPNS_TYPE_MASK  	(3 << 16)	// Mask of response type

#define SD_RESP_NONE        SD_CMD_RSPNS_TYPE_NONE			// no response
#define SD_RESP_R1          (SD_CMD_RSPNS_TYPE_48 | SD_CMD_CRCCHK_EN)	// R1: 48-bits response with CRC (6 bytes)
#define SD_RESP_R1b         (SD_CMD_RSPNS_TYPE_48B | SD_CMD_CRCCHK_EN)	// R1B: 48-bits response with busy and CRC (6 bytes)
#define SD_RESP_R2          (SD_CMD_RSPNS_TYPE_136 | SD_CMD_CRCCHK_EN)	// R2: 136-bits response with CRC (17 bytes)
#define SD_RESP_R3          SD_CMD_RSPNS_TYPE_48			// R3: 48-bits response, no CRC (6 bytes)
#define SD_RESP_R4          SD_CMD_RSPNS_TYPE_136			// R4: 136-bits response, no CRC (17 bytes)
#define SD_RESP_R5          (SD_CMD_RSPNS_TYPE_48 | SD_CMD_CRCCHK_EN)	// R5: 48-bits response with CRC (6 bytes)
#define SD_RESP_R5b         (SD_CMD_RSPNS_TYPE_48B | SD_CMD_CRCCHK_EN)	// R5B: 48-bits response with busy and CRC (6 bytes)
#define SD_RESP_R6          (SD_CMD_RSPNS_TYPE_48 | SD_CMD_CRCCHK_EN)	// R6: 48-bits response with CRC (6 bytes)
#define SD_RESP_R7          (SD_CMD_RSPNS_TYPE_48 | SD_CMD_CRCCHK_EN)	// R7: 48-bits response with CRC (6 bytes)

#define SD_DATA_READ        (SD_CMD_ISDATA | SD_CMD_DAT_DIR_CH)		// Read: transfer data from card to host
#define SD_DATA_WRITE       (SD_CMD_ISDATA | SD_CMD_DAT_DIR_HC)		// Write: transfer data from host to card

#define SD_CMD_RESERVED(a)  0xffffffff			// command not used

const u32 EMMC_SDCommands[64] =
{
	SD_CMD_INDEX(0),
	SD_CMD_RESERVED(1),
	SD_CMD_INDEX(2) | SD_RESP_R2,
	SD_CMD_INDEX(3) | SD_RESP_R6,
	SD_CMD_INDEX(4),
	SD_CMD_INDEX(5) | SD_RESP_R4,
	SD_CMD_INDEX(6) | SD_RESP_R1 | SD_DATA_READ,
	SD_CMD_INDEX(7) | SD_RESP_R1b,
	SD_CMD_INDEX(8) | SD_RESP_R7,
	SD_CMD_INDEX(9) | SD_RESP_R2,
	SD_CMD_INDEX(10) | SD_RESP_R2,
	SD_CMD_INDEX(11) | SD_RESP_R1,
	SD_CMD_INDEX(12) | SD_RESP_R1b | SD_CMD_TYPE_ABORT,
	SD_CMD_INDEX(13) | SD_RESP_R1,
	SD_CMD_RESERVED(14),
	SD_CMD_INDEX(15),
	SD_CMD_INDEX(16) | SD_RESP_R1,
	SD_CMD_INDEX(17) | SD_RESP_R1 | SD_DATA_READ,
	SD_CMD_INDEX(18) | SD_RESP_R1 | SD_DATA_READ | SD_CMD_MULTI_BLOCK | SD_CMD_BLKCNT_EN | SD_CMD_AUTO_CMD_EN_CMD12,
	SD_CMD_INDEX(19) | SD_RESP_R1 | SD_DATA_READ,
	SD_CMD_INDEX(20) | SD_RESP_R1b,
	SD_CMD_RESERVED(21),
	SD_CMD_RESERVED(22),
	SD_CMD_INDEX(23) | SD_RESP_R1,
	SD_CMD_INDEX(24) | SD_RESP_R1 | SD_DATA_WRITE,
	SD_CMD_INDEX(25) | SD_RESP_R1 | SD_DATA_WRITE | SD_CMD_MULTI_BLOCK | SD_CMD_BLKCNT_EN | SD_CMD_AUTO_CMD_EN_CMD12,
	SD_CMD_RESERVED(26),
	SD_CMD_INDEX(27) | SD_RESP_R1 | SD_DATA_WRITE,
	SD_CMD_INDEX(28) | SD_RESP_R1b,
	SD_CMD_INDEX(29) | SD_RESP_R1b,
	SD_CMD_INDEX(30) | SD_RESP_R1 | SD_DATA_READ,
	SD_CMD_RESERVED(31),
	SD_CMD_INDEX(32) | SD_RESP_R1,
	SD_CMD_INDEX(33) | SD_RESP_R1,
	SD_CMD_RESERVED(34),
	SD_CMD_RESERVED(35),
	SD_CMD_RESERVED(36),
	SD_CMD_RESERVED(37),
	SD_CMD_INDEX(38) | SD_RESP_R1b,
	SD_CMD_RESERVED(39),
	SD_CMD_RESERVED(40),
	SD_CMD_RESERVED(41),
	SD_CMD_RESERVED(42) | SD_RESP_R1,
	SD_CMD_RESERVED(43),
	SD_CMD_RESERVED(44),
	SD_CMD_RESERVED(45),
	SD_CMD_RESERVED(46),
	SD_CMD_RESERVED(47),
	SD_CMD_RESERVED(48),
	SD_CMD_RESERVED(49),
	SD_CMD_RESERVED(50),
	SD_CMD_RESERVED(51),
	SD_CMD_RESERVED(52),
	SD_CMD_RESERVED(53),
	SD_CMD_RESERVED(54),
	SD_CMD_INDEX(55) | SD_RESP_R1,
	SD_CMD_INDEX(56) | SD_RESP_R1 | SD_CMD_ISDATA,
	SD_CMD_RESERVED(57),
	SD_CMD_RESERVED(58),
	SD_CMD_RESERVED(59),
	SD_CMD_RESERVED(60),
	SD_CMD_RESERVED(61),
	SD_CMD_RESERVED(62),
	SD_CMD_RESERVED(63)
};

const u32 EMMC_SDACommands[64] =
{
	SD_CMD_RESERVED(0),
	SD_CMD_RESERVED(1),
	SD_CMD_RESERVED(2),
	SD_CMD_RESERVED(3),
	SD_CMD_RESERVED(4),
	SD_CMD_RESERVED(5),
	SD_CMD_INDEX(6) | SD_RESP_R1,
	SD_CMD_RESERVED(7),
	SD_CMD_RESERVED(8),
	SD_CMD_RESERVED(9),
	SD_CMD_RESERVED(10),
	SD_CMD_RESERVED(11),
	SD_CMD_RESERVED(12),
	SD_CMD_INDEX(13) | SD_RESP_R1,
	SD_CMD_RESERVED(14),
	SD_CMD_RESERVED(15),
	SD_CMD_RESERVED(16),
	SD_CMD_RESERVED(17),
	SD_CMD_RESERVED(18),
	SD_CMD_RESERVED(19),
	SD_CMD_RESERVED(20),
	SD_CMD_RESERVED(21),
	SD_CMD_INDEX(22) | SD_RESP_R1 | SD_DATA_READ,
	SD_CMD_INDEX(23) | SD_RESP_R1,
	SD_CMD_RESERVED(24),
	SD_CMD_RESERVED(25),
	SD_CMD_RESERVED(26),
	SD_CMD_RESERVED(27),
	SD_CMD_RESERVED(28),
	SD_CMD_RESERVED(29),
	SD_CMD_RESERVED(30),
	SD_CMD_RESERVED(31),
	SD_CMD_RESERVED(32),
	SD_CMD_RESERVED(33),
	SD_CMD_RESERVED(34),
	SD_CMD_RESERVED(35),
	SD_CMD_RESERVED(36),
	SD_CMD_RESERVED(37),
	SD_CMD_RESERVED(38),
	SD_CMD_RESERVED(39),
	SD_CMD_RESERVED(40),
	SD_CMD_INDEX(41) | SD_RESP_R3,
	SD_CMD_INDEX(42) | SD_RESP_R1,
	SD_CMD_RESERVED(43),
	SD_CMD_RESERVED(44),
	SD_CMD_RESERVED(45),
	SD_CMD_RESERVED(46),
	SD_CMD_RESERVED(47),
	SD_CMD_RESERVED(48),
	SD_CMD_RESERVED(49),
	SD_CMD_RESERVED(50),
	SD_CMD_INDEX(51) | SD_RESP_R1 | SD_DATA_READ,
	SD_CMD_RESERVED(52),
	SD_CMD_RESERVED(53),
	SD_CMD_RESERVED(54),
	SD_CMD_RESERVED(55),
	SD_CMD_RESERVED(56),
	SD_CMD_RESERVED(57),
	SD_CMD_RESERVED(58),
	SD_CMD_RESERVED(59),
	SD_CMD_RESERVED(60),
	SD_CMD_RESERVED(61),
	SD_CMD_RESERVED(62),
	SD_CMD_RESERVED(63)
};

// Command indices
#define GO_IDLE_STATE		0
#define SEND_OP_COND		1
#define ALL_SEND_CID		2
#define SEND_RELATIVE_ADDR	3
#define SET_DSR			4
#define IO_SET_OP_COND		5
#define SWITCH_FUNC		6
//#define SELECT_CARD             7
//#define DESELECT_CARD           7
#define SELECT_DESELECT_CARD	7
#define SEND_IF_COND		8
#define SEND_CSD		9
#define SEND_CID		10
#define VOLTAGE_SWITCH		11
#define STOP_TRANSMISSION	12
#define SEND_STATUS		13
#define GO_INACTIVE_STATE	15
#define SET_BLOCKLEN		16
#define READ_SINGLE_BLOCK	17
#define READ_MULTIPLE_BLOCK	18
#define SEND_TUNING_BLOCK	19
#define SPEED_CLASS_CONTROL	20
#define SET_BLOCK_COUNT		23
#define WRITE_BLOCK		24
#define WRITE_MULTIPLE_BLOCK	25
#define PROGRAM_CSD		27
#define SET_WRITE_PROT		28
#define CLR_WRITE_PROT		29
#define SEND_WRITE_PROT		30
#define ERASE_WR_BLK_START	32
#define ERASE_WR_BLK_END	33
#define ERASE			38
#define LOCK_UNLOCK		42
#define APP_CMD			55
#define GEN_CMD			56

// SD card version
#define SD_VER_UNKNOWN      0	// unknown
#define SD_VER_1            1	// 1.0
#define SD_VER_1_1          2	// 1.1
#define SD_VER_2            3	// 2.0
#define SD_VER_3            4	// 3.0
#define SD_VER_4            5	// 4.0

const char* EMMC_SDVersions[6] =
{
	"NONE",
	"v1.0",
	"v1.1",
	"v2.0",
	"v3.0",
	"v4.0",
};

/*
// Initialize EMMC SD card interface
void EMMC_Init(void)
{



}
*/
