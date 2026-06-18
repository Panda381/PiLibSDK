
#ifndef _DRV_INCLUDE_H
#define _DRV_INCLUDE_H

#if USE_EEPROM		// 1=use EEPROM driver, 0=not used (drv_eeprom.*)
#include "inc/drv_eeprom.h"	// EEPROM memory I2C
#endif

#if USE_I2CBUS		// 1=use I2C bus driver, 0=not used (drv_i2cbus.*)
#include "inc/drv_i2cbus.h"	// I2C bus driver
#endif

#if USE_LCD		// 1=use LCD SPI display driver, 0=not used (drv_lcd.*)
#include "inc/drv_lcd.h"	// LCD SPI display driver
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
