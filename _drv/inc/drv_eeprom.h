
// ****************************************************************************
//
//                     EEPROM driver on I2C interface
//
// ****************************************************************************
// Supported EEPROMs (speed 100-400 kHz, write 5-10 ms max, 16-bit adressing):
//	Atmel AT24C32, 4 KB, page 32 bytes, address range 0x50-0x57, config magic 'BPiC'
//	Atmel AT24C64, 8 KB, page 32 bytes, address range 0x50-0x57, config magic 'BPiD'
//	Atmel AT24C128, 16 KB, page 64 bytes, address range 0x50-0x53, config magic 'BPiE'
//	Atmel AT24C256, 32 KB, page 64 bytes, address range 0x50-0x53, config magic 'BPiF'
//	Microchip 24LC512, 64 KB, page 128 bytes, address range 0x50-0x57, config magic 'BPiG'

#if USE_EEPROM		// 1=use EEPROM driver, 0=not used (drv_eeprom.*)

#ifndef _DRV_EEPROM_H
#define _DRV_EEPROM_H

#define EEPROM_PAGESIZEMAX	128	// max. size of the page

// EEPROM model
enum {
	EEPROM_MODEL_4K = 0,	// AT24C32, 4 KB, page 32 bytes, config magic 'BPiC'
	EEPROM_MODEL_8K,	// AT24C64, 8 KB, page 32 bytes, config magic 'BPiD'
	EEPROM_MODEL_16K,	// AT24C128, 16 KB, page 64 bytes, config magic 'BPiE'
	EEPROM_MODEL_32K,	// AT24C256, 32 KB, page 64 bytes, config magic 'BPiF'
	EEPROM_MODEL_64K,	// 24LC512, 64 KB, page 128 bytes, config magic 'BPiG'

	EEPROM_MODEL_NUM
};

// EEPROM model descriptor
typedef struct {
	int	sizebits;	// total byte size in bits (12=4K, 13=8K, 14=16K, 15=32K, 16=64K)
	int	pagebits;	// page byte size in bits (5=32B, 6=64B, 7=128B)
} sEEPROMmodel;

// EEPROM device
class cEEPROM
{
public:
	int 	i2c;		// I2C bus index (0 or 1)
	int	addr;		// I2C base address (0x50-0x57)
	int	speed;		// transfer speed in Hz (usually 100000 to 400000)

	int	sizebits;	// total size in bits (12=4K, 13=8K, 14=16K, 15=32K, 16=64K)
	int	pagebits;	// page size in bits (5=32B, 6=64B, 7=128B)

	int	size;		// total size in bytes
	int	pagesize;	// page size in bytes
	int	pagenum;	// number of pages

	u32	seed;		// seed of random number generator

	// page buffer
	u8	pagebuf[EEPROM_PAGESIZEMAX];

	// generate random number byte
	u8	RandByte();

	// initialize custom EEPROM
	//  i2c ... I2C bus index (0 or 1)
	//  addr ... I2C address (0x50-0x57)
	//  sizebits ... total size in bits (12=4K, 13=8K, 14=16K, 15=32K, 16=64K)
	//  pagebits ... page size in bits (5=32B, 6=64B, 7=128B)
	//  speed ... transfer speed in Hz (usually 100000 to 400000, default I2C_DEF_SPEED=100000)
	void InitSet(int i2c, int addr, int sizebits, int pagebits, int speed = I2C_DEF_SPEED);

	// initialize EEPROM model
	//  i2c ... I2C bus index (0 or 1)
	//  addr ... I2C address (0x50-0x57)
	//  model ... EEPROM model EEPROM_MODEL_*
	//  speed ... transfer speed in Hz (usually 100000 to 400000, default I2C_DEF_SPEED=100000)
	void Init(int i2c, int addr, int model, int speed = I2C_DEF_SPEED);

	// detect EEPROM model from header of the configuration EEPROM and initialize it (returns False on error or if cannot detect EEPROM model)
	//  i2c ... I2C bus index (0 or 1)
	//  addr ... I2C address (0x50-0x57)
	//  speed ... transfer speed in Hz (usually 100000 to 400000, default I2C_DEF_SPEED=100000)
	Bool Detect(int i2c, int addr, int speed = I2C_DEF_SPEED);

	// read data from EEPROM to buffer (returns False on error)
	//  off ... offset in the EEPROM
	//  buf ... pointer to buffer
	//  num ... number of bytes to read
	Bool Read(int off, void* buf, int num);

	// write data to EEPROM from buffer (returns False on error)
	//  off ... offset in the EEPROM
	//  buf ... pointer to buffer
	//  num ... number of bytes to write
	Bool Write(int off, const void* buf, int num);

	// generate randomised pattern in buffer
	//  buf ... pointer to buffer
	//  num ... number of bytes to check
	//  seed ... seed of the random generator
	void GenPattern(void* buf, int num, u32 seed = 0x12345678);

	// write randomised pattern to the EEPROM (returns False on error)
	//  off ... offset in the EEPROM
	//  num ... number of bytes to write
	//  seed ... seed of the random generator
	Bool WritePattern(int off, int num, u32 seed = 0x12345678);

	// check randomised pattern - read from EEPROM (returns offset of difference, or 'num' if OK, or -1 if error)
	//  off ... offset in the EEPROM
	//  num ... number of bytes to check
	//  seed ... seed of the random generator
	int CheckPattern(int off, int num, u32 seed = 0x12345678);

	// verify randomised pattern in buffer (returns offset of difference, or 'num' if OK)
	//  buf ... pointer to buffer
	//  num ... number of bytes to check
	//  seed ... seed of the random generator
	int VerifyPattern(const void* buf, int num, u32 seed = 0x12345678);

	// clear EEPROM with constant byte (returns False on error)
	//  off ... offset in the EEPROM
	//  num ... number of bytes to write
	//  data ... data patern 0x00..0xFF (default 0xFF)
	Bool Clear(int off, int num, int data = 0xff);

	// format new EEPROM config storage (destroys EEPROM content; returns False on error)
	// - The entire EEPROM is first initialized to the values 0xff.
	// - The beginning of the EEPROM contains the 3-character magic string "BPi" (= Bare Pi).
	// - At offset 3 is character indicating size in bits ('7'=128B,...'9'=512B,'A'=1KB,...'G'=64KB).
	// - Each configuration entry begins with a 16-bit ID, a 16-bit count of data bytes, and the data itself.
	// - The list of configuration items ends with the 16-bit value 0xffff.
	// - ID values 0x0001 - 0x00FF are reserved for BarePi library.
	Bool Format();

	// Load configuration entry from EEPROM (returns False on error or if the item was not found)
	//  id ... ID of the entry
	//	- value 0xFFFF is reserved, marks end of list (EEPROM is initialized to values 0xff)
	//	- values 0x0001 - 0x00FF are reserved for BarePi library
	//	- values 0x0100 - 0xFFFE are free to use
	//  len ... length of the data of the entry, without entries 'id' and 'len'
	//  buf ... pointer to data of the entry, without entries 'id' and 'len' (buffer size = 'len')
	// - Both the 'id' and the 'len' of the item must match - this allows the use of
	//   multiple versions of the same 'id' with different sizes (newer version of the entry).
	// - If entry was not found, the contents of the buffer will not be changed.
	//   This can be used to prepare default values. Or initialize it after error result.
	Bool LoadCfg(int id, int len, void* buf);

	// Save configuration entry to EEPROM (returns False on error)
	//  id ... ID of the entry
	//	- value 0xFFFF is reserved, marks end of list (EEPROM is initialized to values 0xff)
	//	- values 0x0001 - 0x00FF are reserved for BarePi library
	//	- values 0x0100 - 0xFFFE are free to use
	//  len ... length of the data of the entry, without entries 'id' and 'len'
	//  buf ... pointer to data of the entry, without entries 'id' and 'len' (buffer size = 'len')
	Bool SaveCfg(int id, int len, void* buf);
};

#endif // _DRV_EEPROM_H

#endif // USE_EEPROM
