
#ifndef _DRV_INCLUDE_H
#define _DRV_INCLUDE_H

#if USE_CALCKEY		// 1=use BarePi CalcKey, 0=not used (drv_calckey.*)
#include "inc/drv_calckey.h"	// BarePi keyboard CalcKey driver
#endif

#if USE_EEPROM		// 1=use EEPROM driver, 0=not used (drv_eeprom.*)
#include "inc/drv_eeprom.h"	// EEPROM memory I2C
#endif

#if USE_I2CBUS		// 1=use I2C bus driver, 0=not used (drv_i2cbus.*)
#include "inc/drv_i2cbus.h"	// I2C bus driver
#endif

#if USE_KEYPAD && !USE_MINIKEY	// 1=use BarePi KeyPad, 0=not used (drv_keypad.*)
#include "inc/drv_keypad.h"	// BarePi keyboard KeyPad driver
#endif

#if USE_LCD		// 1=use LCD SPI display driver, 0=not used (drv_lcd.*)
#include "inc/drv_lcd.h"	// LCD SPI display driver
#endif

#if USE_LCDTXT		// 1=use LCD TEXT I2C display driver, 0=not used (drv_lcdtxt.*)
#include "inc/drv_lcdtxt.h"	// LCD TEXT I2C display driver
#endif

#if USE_LED12		// 1=use BarePi LED12 display, 0=not used (drv_led12.*)
#include "inc/drv_led12.h"	// LED12 display driver
#endif

#if USE_MINIKEY		// 1=use BarePi MiniKey, 0=not used (drv_minikey.*)
#include "inc/drv_minikey.h"	// BarePi keyboard MiniKey driver
#endif

#if USE_PORT12		// 1=use BarePi 12-channel PORT module, 0=not used (drv_port12.*)
#include "inc/drv_port12.h"	// BarePi 12-channel PORT module driver
#endif

#if USE_PWMSND		// 1=use PWM sound output, 0=not used (drv_pwmsnd.*)
#include "inc/drv_pwmsnd.h"	// PWM sound output
#endif

#if USE_RTC		// 1=use RTC driver, 0=not used (drv_rtc.*)
#include "inc/drv_rtc.h"	// RTC real time clock
#endif

#if USE_SSD1306		// 1=use SSD1306 display driver, 0=not used (drv_ssd1306.*)
#include "inc/drv_ssd1306.h"	// SSD1306 display driver I2C
#endif

#endif // _DRV_INCLUDE_H
