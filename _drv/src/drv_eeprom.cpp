
// ****************************************************************************
//
//                     EEPROM driver on I2C interface
//
// ****************************************************************************

#include "../../includes.h"	// includes

#if USE_EEPROM		// 1=use EEPROM driver, 0=not used (drv_eeprom.*)

// EEPROM models
const sEEPROMmodel EEPROMmodelList[EEPROM_MODEL_NUM] = {
	// sizebits  pagebits
	{	12,	5, },	// EEPROM_MODEL_4K,	// AT24C32, 4 KB, page 32 bytes
	{	13,	5, },	// EEPROM_MODEL_8K,	// AT24C64, 8 KB, page 32 bytes
	{	14,	6, },	// EEPROM_MODEL_16K,	// AT24C128, 16 KB, page 64 bytes
	{	15,	6, },	// EEPROM_MODEL_32K,	// AT24C256, 32 KB, page 64 bytes
	{	16,	7, },	// EEPROM_MODEL_64K,	// 24LC512, 64 KB, page 128 bytes
};

// generate random number byte
u8 cEEPROM::RandByte()
{
	u32 k = this->seed*214013 + 2531011;
	this->seed = k;
	return (u8)(k >> 24);
}

// initialize custom EEPROM
//  i2c ... I2C bus index (0 or 1)
//  addr ... I2C address (0x50-0x57)
//  sizebits ... total size in bits (12=4K, 13=8K, 14=16K, 15=32K, 16=64K)
//  pagebits ... page size in bits (5=32B, 6=64B, 7=128B)
//  speed ... transfer speed in Hz (usually 100000 to 400000, default I2C_DEF_SPEED=100000)
void cEEPROM::InitSet(int i2c, int addr, int sizebits, int pagebits, int speed /* = I2C_DEF_SPEED */)
{
	this->i2c = i2c;			// I2C bus index (0 or 1)
	this->addr = addr;			// I2C base address (0x50-0x57)
	this->speed = speed;			// transfer speed in Hz (usually 100000 to 400000)

	this->sizebits = sizebits;		// total size in bits (12=4K, 13=8K, 14=16K, 15=32K, 16=64K)
	this->pagebits = pagebits;		// page size in bits (5=32B, 6=64B, 7=128B)

	this->size = 1 << sizebits;		// total size in bytes
	this->pagesize = 1 << pagebits;		// page size in bytes
	this->pagenum = 1 << (sizebits - pagebits); // number of pages
}

// initialize
//  i2c ... I2C bus index (0 or 1)
//  addr ... I2C address (0x50-0x57)
//  model ... EEPROM model EEPROM_MODEL_*
//  speed ... transfer speed in Hz (usually 100000 to 400000, default I2C_DEF_SPEED=100000)
void cEEPROM::Init(int i2c, int addr, int model, int speed /* = I2C_DEF_SPEED */)
{
	const sEEPROMmodel* m = &EEPROMmodelList[model];
	this->InitSet(i2c, addr, m->sizebits, m->pagebits, speed);
}

// detect EEPROM model from header of the configuration EEPROM and initialize it (returns False on error or if cannot detect EEPROM model)
//  i2c ... I2C bus index (0 or 1)
//  addr ... I2C address (0x50-0x57)
//  speed ... transfer speed in Hz (usually 100000 to 400000, default I2C_DEF_SPEED=100000)
Bool cEEPROM::Detect(int i2c, int addr, int speed /* = I2C_DEF_SPEED */)
{
	// emergency init
	this->Init(i2c, addr, EEPROM_MODEL_64K, speed);

	// read EEPROM config header
	u8 buf[4];
	if (!I2Cbus_ReadReg16(i2c, addr, 0, buf, 4, speed)) return False;

	// check EEPROM config magic 'BPi'
	if ((buf[0] != 'B') || (buf[1] != 'P') || (buf[2] != 'i')) return False;

	// prepare EEPROM model (only models 4..64KB are supported - magic 'B' to 'G')
	if ((buf[3] < 'C') || (buf[3] > 'G')) return False;
	int model = buf[3] - 'C';
	model += EEPROM_MODEL_4K;

	// initialize EEPROM
	this->Init(i2c, addr, model, speed);
	return True;
}

// read data from EEPROM to buffer (returns False on error)
//  off ... offset in the EEPROM
//  buf ... pointer to buffer
//  num ... number of bytes to read
Bool cEEPROM::Read(int off, void* buf, int num)
{
	u8* d = (u8*)buf;
	while (num > 0)
	{
		// number of bytes in one transaction
		int n = this->pagesize - (off & (this->pagesize-1));
		if (num < n) n = num;

		// read data from I2C
		if (!I2Cbus_ReadReg16(this->i2c, this->addr, off, d, n, this->speed)) return False;

		// shift address
		d += n;
		num -= n;
		off += n;
	}
	return True;
}

// write data to EEPROM from buffer (returns False on error)
//  off ... offset in the EEPROM
//  buf ... pointer to buffer
//  num ... number of bytes to write
Bool cEEPROM::Write(int off, const void* buf, int num)
{
	const u8* s = (const u8*)buf;
	while (num > 0)
	{
		// number of bytes in one transaction
		int n = this->pagesize - (off & (this->pagesize-1));
		if (num < n) n = num;

		// write address and data
		if (!I2Cbus_WriteReg16(this->i2c, this->addr, off, s, n, this->speed)) return False;

		// wait for write (time-out 20ms)
		u32 start = Time();
		while (True)
		{
			if (I2Cbus_Check(this->i2c, this->addr, this->speed)) break;
			if ((u32)(Time() - start) > 20000) return False;
		}

		// shift address
		s += n;
		num -= n;
		off += n;
	}
	return True;
}

// generate randomised pattern in buffer
//  buf ... pointer to buffer
//  num ... number of bytes to check
//  seed ... seed of the random generator
void cEEPROM::GenPattern(void* buf, int num, u32 seed /* = 0x12345678 */)
{
	this->seed = seed;	// seed of random number generator
	u8* s = (u8*)buf;
	int i;
	for (i = 0; i < num; i++) s[i] = this->RandByte();
}

// write randomised pattern to the EEPROM (returns False on error)
//  off ... offset in the EEPROM
//  num ... number of bytes to write
//  seed ... seed of the random generator
Bool cEEPROM::WritePattern(int off, int num, u32 seed /* = 0x12345678 */)
{
	// setup random number generator
	this->seed = seed;	// seed of random number generator
	while (num > 0)
	{
		// number of bytes in one transaction
		int n = this->pagesize - (off & (this->pagesize-1));
		if (num < n) n = num;

		// generate random data
		int i;
		for (i = 0; i < n; i++) this->pagebuf[i] = this->RandByte();

		// write address and data
		if (!I2Cbus_WriteReg16(this->i2c, this->addr, off, this->pagebuf, n, this->speed)) return False;

		// wait for write (time-out 20ms)
		u32 start = Time();
		while (True)
		{
			if (I2Cbus_Check(this->i2c, this->addr, this->speed)) break;
			if ((u32)(Time() - start) > 20000) return False;
		} 

		// shift address
		num -= n;
		off += n;
	}
	return True;
}

// check randomised pattern - read from EEPROM (returns offset of difference, or 'num' if OK, or -1 if error)
//  off ... offset in the EEPROM
//  num ... number of bytes to check
//  seed ... seed of the random generator
int cEEPROM::CheckPattern(int off, int num, u32 seed /* = 0x12345678 */)
{
	this->seed = seed;	// seed of random number generator
	int inx = 0;
	int i;
	while (num > 0)
	{
		// number of bytes in one transaction
		int n = this->pagesize - (off & (this->pagesize-1));
		if (num < n) n = num;

		// read data from I2C
		if (!I2Cbus_ReadReg16(this->i2c, this->addr, off, this->pagebuf, n, this->speed)) return -1;

		// verify data
		for (i = 0; i < n; i++)
		{
			// check one byte
			if (this->RandByte() != this->pagebuf[i]) return inx;
			inx++;
		}

		// shift address
		num -= n;
		off += n;
	}
	return inx;
}

// verify randomised pattern in buffer (returns offset of difference, or 'num' if OK)
//  buf ... pointer to buffer
//  num ... number of bytes to check
//  seed ... seed of the random generator
int cEEPROM::VerifyPattern(const void* buf, int num, u32 seed /* = 0x12345678 */)
{
	this->seed = seed;	// seed of random number generator
	const u8* s = (const u8*)buf;
	int i;
	for (i = 0; i < num; i++)
	{
		if (this->RandByte() != s[i]) return i;
	}
	return num;
}

// clear EEPROM with constant byte (returns False on error)
//  off ... offset in the EEPROM
//  num ... number of bytes to write
//  data ... data patern 0x00..0xFF (default 0xFF)
Bool cEEPROM::Clear(int off, int num, int data /* = 0xff */)
{
	// prepare data buffer
	memset(&this->pagebuf[0], data, this->pagesize);
	while (num > 0)
	{
		// number of bytes in one transaction
		int n = this->pagesize - (off & (this->pagesize-1));
		if (num < n) n = num;

		// write address and data
		if (!I2Cbus_WriteReg16(this->i2c, this->addr, off, this->pagebuf, n, this->speed)) return False;

		// wait for write (time-out 20ms)
		u32 start = Time();
		while (True)
		{
			if (I2Cbus_Check(this->i2c, this->addr, this->speed)) break;
			if ((u32)(Time() - start) > 20000) return False;
		} 

		// shift address
		num -= n;
		off += n;
	}
	return True;
}

// format new EEPROM config storage (destroys EEPROM content; returns False on error)
// - The entire EEPROM is first initialized to the values 0xff.
// - The beginning of the EEPROM contains the 3-character magic string "BPi" (= Bare Pi).
// - At offset 3 is character indicating size in bits ('7'=128B,...'9'=512B,'A'=1KB,...'G'=64KB).
// - Each configuration entry begins with a 16-bit ID, a 16-bit count of data bytes, and the data itself.
// - The list of configuration items ends with the 16-bit value 0xFFFF.
// - ID values 0x0001 - 0x00FF are reserved for BarePi library.
Bool cEEPROM::Format()
{
	// clear all EEPROM to values 0xff
	if (!this->Clear(0, this->size, 0xff)) return False;

	// prepare header - magic
	u8 buf[4];
	buf[0] = 'B';		// 'BPi' magic
	buf[1] = 'P';
	buf[2] = 'i';

	// EEPROM size
	int n = this->sizebits;
	buf[3] = (n <= 9) ? ('0'+n) : ('A'+n-10); // EEPROM size

	// write header
	return this->Write(0, buf, 4);
}

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
Bool cEEPROM::LoadCfg(int id, int len, void* buf)
{
	u16 h[2];

	// while offset is valid
	int off = 4; // skip magic
	while (off + 4 + len <= this->size)
	{
		// read entry ID and size
		if (!this->Read(off, h, 4)) return False;

		// check if this is the required entry
		if ((h[0] == (u16)id) && (h[1] == (u16)len))
		{
			// read data of the entry
			return this->Read(off+4, buf, len);
		}

		// check end of entries
		if (h[0] == (u16)0xffff) break;

		// shift offset
		off += 4 + h[1];
	}

	// entry not found
	return False;
}

// Save configuration entry to EEPROM (returns False on error)
//  id ... ID of the entry
//	- value 0xFFFF is reserved, marks end of list (EEPROM is initialized to values 0xff)
//	- values 0x0001 - 0x00FF are reserved for BarePi library
//	- values 0x0100 - 0xFFFE are free to use
//  len ... length of the data of the entry, without entries 'id' and 'len'
//  buf ... pointer to data of the entry, without entries 'id' and 'len' (buffer size = 'len')
Bool cEEPROM::SaveCfg(int id, int len, void* buf)
{
	u16 h[2];

	// while offset is valid
	int off = 4; // skip magic
	while (off + 4 + len <= this->size)
	{
		// read entry ID and size
		if (!this->Read(off, h, 4)) return False;

		// check if this is the required entry
		if ((h[0] == (u16)id) && (h[1] == (u16)len))
		{
			// write data of the entry
			return this->Write(off+4, buf, len);
		}

		// check end of entries, or invalid entry size (damaged EEPROM) - write new entry
		if ((h[0] == (u16)0xffff) || (off + 4 + h[1] > this->size))
		{
			// write header
			h[0] = (u16)id;
			h[1] = (u16)len;
			if (!this->Write(off, h, 4)) return False;

			// write data of the entry
			if (!this->Write(off+4, buf, len)) return False;

			// write end mark
			if (off + 4 + len + 2 <= this->size)
			{
				h[0] = 0xffff;
				return this->Write(off+4+len, h, 2);
			}
			return True;
		}

		// shift offset
		off += 4 + h[1];
	}

	// cannot write entry
	return False;
}

#endif // USE_EEPROM
