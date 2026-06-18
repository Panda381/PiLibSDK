
// ****************************************************************************
//
//                           BarePi - Initialize
//
// ****************************************************************************

#include "../../includes.h"

#if USE_LCD && USE_LCD320x240		// 1=enable output to LCD SPI display ST7789 320x240 (BarePi module LCD320x240), 2=use core3
cLCD LCDDisp;				// LCD display driver
u32 LCDDispLast;			// last time of start of LCD frame
volatile int LCDDispTime;		// time delta of LCD frame in [us], to get FPS
volatile int LCDDispZoom = 0;		// LCD display zoom (0..4, 0=full screen)

// shift LCD display zoom (on key Insert)
void LCDRezoom()
{
	int z = LCDDisp.zoom+1;
	if (z > 4) z = 0;
	LCDDisp.zoom = z;
	LCDDisp.UpdateMain();	// display update
} 
#endif

// Initialize device
void DevInit()
{
	// initialize I2C bus driver
#if USE_I2CBUS		// 1=use I2C bus driver, 0=not used (drv_i2cbus.*)
#if USE_I2C0BUS
	I2Cbus_Init(0);
#endif
#if USE_I2C1BUS
	I2Cbus_Init(1);
#endif
#endif

	// initialize and read RTC clock
#if USE_RTC		// 1=use RTC driver, 0=not used (drv_rtc.*)
	RTC.Init();
	if (RTC.ok)
	{
		sDateTime dt;
		if (RTC.Read(&dt)) SetUnixTime(DateTimePack(&dt, NULL, NULL), 0);
	}
#endif

	// output to LCD SPI display
#if USE_LCD && USE_LCD320x240		// 1=enable output to LCD SPI display ST7789 320x240 (BarePi module LCD320x240), 2=use core3
	if (LCDDisp.Detect() == LCD_MODEL_ST7789)
	{
		LCDDisp.Init(LCD_MODEL_ST7789, 240, 320, 0, 0, NULL, LCD_ROT_270, LCD_DEF_SPEED);
	}
#endif

	// keyboard initialize
	KeyInit();
}

// terminate device
void DevTerm()
{
	// keyboard terminate
	KeyTerm();

	// LCD SPI display
#if USE_LCD && USE_LCD320x240		// 1=enable output to LCD SPI display ST7789 320x240 (BarePi module LCD320x240), 2=use core3
	if (LCDDisp.IsValid())
	{
		LCDDisp.Term();		// terminate LCD
	}
#endif

#if USE_I2CBUS		// 1=use I2C bus driver, 0=not used (drv_i2cbus.*)
	// terminate I2C bus driver
	I2Cbus_Term(0);
	I2Cbus_Term(1);
#endif // USE_I2CBUS

	// terminate I2C, if it was initialized
	I2C_Term(0);
	I2C_Term(1);
}
