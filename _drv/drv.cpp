
#include "../includes.h"	// includes

#if USE_CALCKEY		// 1=use BarePi CalcKey, 0=not used (drv_calckey.*)
#include "src/drv_calckey.cpp"	// BarePi keyboard CalcKey driver
#endif

#if USE_EEPROM		// 1=use EEPROM driver, 0=not used (drv_eeprom.*)
#include "src/drv_eeprom.cpp"	// EEPROM memory I2C
#endif

#if USE_I2CBUS		// 1=use I2C bus driver, 0=not used (drv_i2cbus.*)
#include "src/drv_i2cbus.cpp"	// I2C bus driver
#endif

#if USE_KEYPAD && !USE_MINIKEY	// 1=use BarePi KeyPad, 0=not used (drv_keypad.*)
#include "src/drv_keypad.cpp"	// BarePi keyboard KeyPad driver
#endif

#if USE_LCD		// 1=use LCD SPI display driver, 0=not used (drv_lcd.*)
#include "src/drv_lcd.cpp"	// LCD SPI display driver
#endif

#if USE_LCDTXT		// 1=use LCD TEXT I2C display driver, 0=not used (drv_lcdtxt.*)
#include "src/drv_lcdtxt.cpp"	// LCD TEXT I2C display driver
#endif

#if USE_LED12		// 1=use BarePi LED12 display, 0=not used (drv_led12.*)
#include "src/drv_led12.cpp"	// LED12 display driver
#endif

#if USE_MINIKEY		// 1=use BarePi MiniKey, 0=not used (drv_minikey.*)
#include "src/drv_minikey.cpp"	// BarePi keyboard MiniKey driver
#endif

#if USE_PORT12		// 1=use BarePi 12-channel PORT module, 0=not used (drv_port12.*)
#include "src/drv_port12.cpp"	// BarePi 12-channel PORT module driver
#endif

#if USE_PWMSND		// 1=use PWM sound output, 0=not used (drv_pwmsnd.*)
#include "src/drv_pwmsnd.cpp"	// PWM sound output
#endif

#if USE_RTC		// 1=use RTC driver, 0=not used (drv_rtc.*)
#include "src/drv_rtc.cpp"	// RTC real time clock
#endif

#if USE_SSD1306		// 1=use SSD1306 display driver, 0=not used (drv_ssd1306.*)
#include "src/drv_ssd1306.cpp"	// SSD1306 display driver I2C
#endif
