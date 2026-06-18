
// ****************************************************************************
//
//                           RTC Real Time Clock DS3231
//
// ****************************************************************************

#include "../../includes.h"	// includes

#if USE_RTC		// 1=use RTC driver, 0=not used (drv_rtc.*)

// RTC clock
cRTC RTC;

// unpack entry from BCD format to binary format
u8 cRTC::UnpackBCD(u8 n)
{
	return (u8)((n & 0x0f) + (n >> 4)*10);
}

// pack entry from binary format to BCD format
u8 cRTC::PackBCD(u8 n)
{
	int i = n/10;
	int j = n - i*10;
	return (u8)((i << 4) | j);
}

// initialize (returns False on eror)
//  i2c ... I2C bus index (0 or 1)
//  addr ... I2C address (default SSD1306_ADDR=0x3C)
//  speed ... transfer speed in Hz (usually 100000 to 400000, default I2C_DEF_SPEED=100000)
Bool cRTC::Init(int i2c /* = 0 */, int addr /* = RTC_ADDR */, int speed /* = I2C_DEF_SPEED */)
{
	this->i2c = i2c;	// I2C bus index (0 or 1)
	this->addr = addr;	// I2C address (default 0x3C)
	this->speed = speed;	// transfer speed in Hz (usually 100000 to 400000)
	this->ok = False;	// not supported

	// read all RTC registers
	if (!this->ReadReg(0, (u8*)&this->reg, RTC_REG_NUM)) return False;

	// oscillator is stopped, run it
	if ((this->reg.control & B7) != 0)
	{
		this->reg.control &= ~B7;
		this->WriteReg(0x0E, &this->reg.control, 1);
	}

	// set 24-hour mode
	if ((this->reg.hour & B6) != 0)
	{
		this->reg.hour &= ~B6;
		this->WriteReg(0x02, &this->reg.hour, 1);
	}

	// supported
	this->ok = True;
	return True;
}

// read RTC registers (returns False on error)
Bool cRTC::ReadReg(int inx, u8* reg, int num /* = 1 */)
{
	return I2Cbus_ReadReg8(this->i2c, this->addr, inx, reg, num, this->speed);
}

// write RTC registers (returns False on error)
Bool cRTC::WriteReg(int inx, const u8* reg, int num /* = 1 */)
{
	return I2Cbus_WriteReg8(this->i2c, this->addr, inx, reg, num, this->speed);
}

// read RTC clock from the DS3231 chip (returns False on error)
//  Use DateTimePack() function to pack 'dt' to Unix time.
Bool cRTC::Read(sDateTime* dt)
{
	u8 r[RTC_TIMEREG_NUM];
	int i;

	// read time registers, 1st attempt
	if (!this->ReadReg(0, (u8*)&this->reg, RTC_TIMEREG_NUM)) return False;

	// attempts (some cheap Chinese clones do not include shadow registers for atomic time reading)
	for (i = 10; i > 0; i--)
	{
		// read time registers, 2nd attempt
		if (!this->ReadReg(0, r, RTC_TIMEREG_NUM)) return False;

		// compare registers
		if (memcmp(&this->reg, r, RTC_TIMEREG_NUM) == 0) break;

		// copy
		memcpy(&this->reg, r, RTC_TIMEREG_NUM);
	}

	// get date and time
	dt->us = 0;
	dt->ms = 0;

	i = (this->reg.dayofweek & 0x07);
	if (i < 1) i = 1;
	if (i > 7) i = 7;
	dt->dayofweek = (s8)(i-1);

	i = this->UnpackBCD(this->reg.sec & 0x7f);
	if (i > 59) i = 59;
	dt->sec = (s8)i;

	i = this->UnpackBCD(this->reg.min & 0x7f);
	if (i > 59) i = 59;
	dt->min = (s8)i;

	i = this->UnpackBCD(this->reg.hour & 0x3f);
	if (i > 23) i = 23;
	dt->hour = (s8)i;

	i = this->UnpackBCD(this->reg.day & 0x3f);
	if (i < 1) i = 1;
	if (i > 31) i = 31;
	dt->day = (s8)i;

	i = this->UnpackBCD(this->reg.month & 0x1f);
	if (i < 1) i = 1;
	if (i > 12) i = 12;
	dt->mon = (s8)i;

	i = this->UnpackBCD(this->reg.year);
	i += 2000; // years 2000..2099
	if ((this->reg.month & B7) != 0) // century flag ... split to years 1970-1999, 2000-2099, 2100-2169
	{
		if (i >= 2070)
			i -= 100; // 1970..1999
		else
			i += 100; // 2100..2169
	}
	dt->year = (s16)i;

	return True;
}

// write RTC clock to the DS3231 chip (returns False on error)
//  Use DateTimeUnpack() function to unpack 'dt' from Unix time.
Bool cRTC::Write(const sDateTime* dt)
{
	u8 r[RTC_TIMEREG_NUM];
	int i;

	// prepare time registers
	this->reg.sec = this->PackBCD(dt->sec);
	this->reg.min = this->PackBCD(dt->min);
	this->reg.hour = this->PackBCD(dt->hour);
	this->reg.dayofweek = dt->dayofweek + 1;
	this->reg.day = this->PackBCD(dt->day);
	int m = this->PackBCD(dt->mon);
	int y = dt->year;
	if (y < 2000)
	{
		m |= B7;	
		y += 100;
	}
	else if (y >= 2100)
	{
		m |= B7;
		y -= 100;
	}
	y -= 2000;
	this->reg.month = (u8)m;
	this->reg.year = this->PackBCD((u8)y);

	// attempts (some cheap Chinese clones do not include shadow registers for atomic time write)
	for (i = 10; i > 0; i--)
	{
		// write time registers
		if (!this->WriteReg(0, (u8*)&this->reg, RTC_TIMEREG_NUM)) return False;

		// read time registers
		if (!this->ReadReg(0, r, RTC_TIMEREG_NUM)) return False;

		// copy century flag (Some clones may not contain century bit)
		r[5] = (r[5] & 0x7f) | (this->reg.month & B7);

		// compare registers
		if (memcmp(&this->reg, r, RTC_TIMEREG_NUM) == 0) break;
	}

	return True;
}

#endif // USE_RTC
