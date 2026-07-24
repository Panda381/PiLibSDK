
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
volatile int LCDDispSend;		// time to send LCD frame in [us]
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

#if USE_LCD320x240==2		// 1=enable output to LCD SPI display ST7789 320x240, 2=use core3
// drawing from code 3
void CoreFncLCD(int core, void* arg)
{
	while (!CoreStopReq(core))
	{
		u32 t = Time();
		LCDDispTime = t - LCDDispLast;
		LCDDispLast = t;
		LCDDispSend = LCDDisp.UpdateCore(core);
	}
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
	LCDDisp.model = LCD_MODEL_NONE;
	if (LCDDisp.Detect() == LCD_MODEL_ST7789)
	{
		LCDDisp.Init(LCD_MODEL_ST7789, 240, 320, 0, 0, NULL, LCD_ROT_270, LCD_DEF_SPEED);
#if USE_LCD320x240==2		// 1=enable output to LCD SPI display ST7789 320x240, 2=use core3
		// drawing from code 3
		RunCore(CORES-1, CoreFncLCD, NULL);
#endif
	}
#endif

#if USE_KEYPAD	// 1=use BarePi KeyPad, 0=not used (drv_keypad.*)
	// keyboard initialize
	// - Must be initialized after initialization of the I2C bus driver.
	KeyInit();
#endif

#if USE_CALCKEY		// 1=use BarePi CalcKey, 0=not used (drv_calckey.*)
	// CalcKey initialize
	CalcKeyInit();
#endif
}

// terminate device
void DevTerm()
{
#if USE_CALCKEY		// 1=use BarePi CalcKey, 0=not used (drv_calckey.*)
	// CalcKey terminate
	CalcKeyTerm();
#endif

#if USE_KEYPAD	// 1=use BarePi KeyPad, 0=not used (drv_keypad.*)
	// keyboard terminate
	KeyTerm();
#endif

	// LCD SPI display
#if USE_LCD && USE_LCD320x240		// 1=enable output to LCD SPI display ST7789 320x240 (BarePi module LCD320x240), 2=use core3
	if (LCDDisp.IsValid())
	{
#if USE_LCD320x240==2		// 1=enable output to LCD SPI display ST7789 320x240, 2=use core3
		StopCore(CORES-1);	// stop core 3
		WaitCoreIdle(CORES-1);	// wait core 3
#endif
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
