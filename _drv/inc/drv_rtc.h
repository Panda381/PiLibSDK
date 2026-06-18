
// ****************************************************************************
//
//                           RTC Real Time Clock DS3231
//
// ****************************************************************************
// Only time registers of the DS3231 are supported - limitation of cheap clones.

#if USE_RTC		// 1=use RTC driver, 0=not used (drv_rtc.*)

#ifndef _DRV_RTC_H
#define _DRV_RTC_H

#include "../../_lib/inc/lib_calendar.h"

#define RTC_ADDR	0x68	// I2C address of the DS3231 chip

#define RTC_REG_NUM	19	// number of RTC registers
#define RTC_TIMEREG_NUM	7	// number of RTC time registers

// RTC registers
typedef struct {
// 0x00: (7) Time registers
	u8	sec;		// 0x00: seconds 00-59
				//	bit 0-3: 1-seconds (0-9)
				//	bit 4-6: 10-seconds (0-5)
				//	bit 7: always 0
	u8	min;		// 0x01: minutes 00-59
				//	bit 0-3: 1-minutes (0-9)
				//	bit 4-6: 10-minutes (0-5)
				//	bit 7: always 0
	u8	hour;		// 0x02: hours 01-12 + AM/PM, or 00-23
				//	bit 0-3: 1-hour (0-9)
				//	24h mode:
				//		bit 4-5: 10-hour (0-2)
				//	12h mode:
				//		bit 4: 10-hour (0-1)
				//		bit 5: 0=AM, 1=PM
				//	bit 6: mode 0=24h, 1=12h
				//	bit 7: always 0
	u8	dayofweek;	// 0x03: day of week 1-7
				//	bit 0-2: day of week (1-7)
				//	bit 3-7: always 0
	u8	day;		// 0x04: day in month 01-31
				//	bit 0-3: 1-day in month (0-9)
				//	bit 4-5: 10-day in month (0-3)
				//	bit 6-7: always 0
	u8	month;		// 0x05: month 01-12 and century 0-1
				//	bit 0-3: 1-month (0-9)
				//	bit 4: 10-month (0-1)
				//	bit 5-6: always 0
				//	bit 7: century
	u8	year;		// 0x06: year 00-99
				//	bit 0-3: 1-year (0-9)
				//	bit 4-7: 10-year (0-9)

// 0x07: (4) Alarm 1 (not supported in some cheap Chinese clones)
	u8	a1_sec;		// 0x07: alarm 1 seconds 00-59
				//	bit 0-3: 1-seconds (0-9)
				//	bit 4-6: 10-seconds (0-5)
				//	bit 7: A1M1 bit
	u8	a1_min;		// 0x08: alarm 1 minutes 00-59
				//	bit 0-3: 1-minutes (0-9)
				//	bit 4-6: 10-minutes (0-5)
				//	bit 7: A1M2 bit
	u8	a1_hour;	// 0x09: alarm 1 hours 01-12 + AM/PM, or 00-23
				//	bit 0-3: 1-hour (0-9)
				//	24h mode:
				//		bit 4-5: 10-hour (0-2)
				//	12h mode:
				//		bit 4: 10-hour (0-1)
				//		bit 5: 0=AM, 1=PM
				//	bit 6: mode 0=24h, 1=12h
				//	bit 7: A1M3 bit
	u8	a1_day;		// 0x0A: alarm 1 day of week 1-7 of day in month 1-31
				//	bit 0-3: day of week (1-7) or 1-day in month (0-9)
				//	bit 4-5: 10-day in month (0-3)
				//	bit 6: mode 0=day in month, 1=day of week
				//	bit 7: A1M4 bit
// 0x0B: (3) Alarm 2 (not supported in some cheap Chinese clones)
	u8	a2_min;		// 0x0B: alarm 2 minutes 00-59
				//	bit 0-3: 1-minutes (0-9)
				//	bit 4-6: 10-minutes (0-5)
				//	bit 7: A2M2 bit
	u8	a2_hour;	// 0x0C: alarm 2 hours 01-12 + AM/PM, or 00-23
				//	bit 0-3: 1-hour (0-9)
				//	24h mode:
				//		bit 4-5: 10-hour (0-2)
				//	12h mode:
				//		bit 4: 10-hour (0-1)
				//		bit 5: 0=AM, 1=PM
				//	bit 6: mode 0=24h, 1=12h
				//	bit 7: A2M3 bit
	u8	a2_day;		// 0x0D: alarm 2 day of week 1-7 of day in month 1-31
				//	bit 0-3: day of week (1-7) or 1-day in month (0-9)
				//	bit 4-5: 10-day in month (0-3)
				//	bit 6: mode 0=day in month, 1=day of week
				//	bit 7: A2M4 bit
// 0x0E: (3) Control and status (not supported in some cheap Chinese clones)
	u8	control;	// 0x0E: control register
				//	bit 0: A1IE 1=alarm 1 interrupt enable
				//	bit 1: A2IE 1=alarm 2 interrupt enable
				//	bit 2: INTCN interrupt control 0=square-wave output, 1=alarm interrupt output
				//	bit 3: RS1 rate select of square-wave output
				//	bit 4: RS2 rate select of square-wave output
				//	bit 5: CONV setting this bit to 1 forces reading temperature
				//	bit 6: BBSQW 1=enable square-wave output also on battery power
				//	bit 7: /EOSC 1=oscillator is stopped, 0=running
	u8	status;		// 0x0F: status register
				//	bit 0: A1F 1=alarm 1 matches
				//	bit 1: A2F 1=alarm 2 matches
				//	bit 2: BSY 1=device is busy (reading temperature)
				//	bit 3: EN32KHZ 1=enable 32kHz output
				//	bit 4-6: always 0
				//	bit 7: OSF 1=oscillator is stopped or was stopped, need time check (write 0 to clear)
	u8	aging;		// 0x10: aging offset
				//	bit 0-6: aging data
				//	bit 7: sign
// 0x11: (2) Temperature, updated ever 64 seconds (not supported in some cheap Chinese clones)
	u8	temp_msb;	// 0x11: temperature MSB
				//	bit 0-6: temp MSB data
				//	bit 7: sign
	u8	temp_lsb;	// 0x12: temperature LSB
				//	bit 0-5: always 0
				//	bit 6-7: temp LSB data
} RTCREG_t;
STATIC_ASSERT(sizeof(RTCREG_t) == 0x13, "Incorrect RTCREG_t!");

// RTC clock
class cRTC
{
public:
	int 		i2c;		// I2C bus index (0 or 1)
	int		addr;		// I2C address (default 0x3C)
	int		speed;		// transfer speed in Hz (usually 100000 to 400000)
	RTCREG_t	reg;		// copy of RTC registers
	Bool		ok;		// True = RTC chip is detected OK

	// unpack entry from BCD format to binary format
	u8 UnpackBCD(u8 n);

	// pack entry from binary format to BCD format
	u8 PackBCD(u8 n);

	// initialize (returns False on eror)
	//  i2c ... I2C bus index (0 or 1)
	//  addr ... I2C address (default SSD1306_ADDR=0x3C)
	//  speed ... transfer speed in Hz (usually 100000 to 400000, default I2C_DEF_SPEED=100000)
	Bool Init(int i2c = 0, int addr = RTC_ADDR, int speed = I2C_DEF_SPEED);

	// read RTC registers (returns False on error)
	Bool ReadReg(int inx, u8* reg, int num = 1);

	// write RTC registers (returns False on error)
	Bool WriteReg(int inx, const u8* reg, int num = 1);

	// read RTC clock from the DS3231 chip (returns False on error)
	//  Use DateTimePack() function to pack 'dt' to Unix time.
	Bool Read(sDateTime* dt);

	// write RTC clock to the DS3231 chip (returns False on error)
	//  Use DateTimeUnpack() function to unpack 'dt' from Unix time.
	Bool Write(const sDateTime* dt);
};

// RTC clock
extern cRTC RTC;

#endif // _DRV_RTC_H

#endif // USE_RTC
