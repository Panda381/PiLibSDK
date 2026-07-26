
##############################################################################
#                      Architecture configuration
##############################################################################

# Path to root directory from the project directory (without trailing '/' delimiter)
PI_ROOT_PATH=../../..

# Makefile includes
include ${PI_ROOT_PATH}/Makefile1.mk

##############################################################################
#                             Input files
##############################################################################

# ASM source files
ASRC +=

# C source files
CSRC +=

# C++ source files
SRC += src/pilibsdk.cpp
SRC += src/main.cpp
SRC += src/CLASS_TMISSILE.cpp
SRC += src/ELECTROLIB.cpp
SRC += src/PIC_TMISSILE.cpp
SRC += src/Tiny-Missile.cpp

##############################################################################
#                         Library configuration
##############################################################################
# For available options see config_def.mk in root directory.

# 1=use BIT integers, 0=not used (_lib/bigint/*)
USE_BIGINT=0

# 1=use Calendar 32-bit (year range 1970..2099), 0=not used (lib_calendar.*)
USE_CALENDAR=1

# 1=use Calendar 64-bit (year range -29227..+29227), 0=not used (lib_calendar64.*)
USE_CALENDAR64=0

# 1=use CRC Cyclic Redundancy Check, 0=not used (lib_crc.*)
USE_CRC=1

# 1=use JPG support, 0=not used (_lib/pic/lib_jpg*)
USE_JPG=0

# 1=use 2D transformation matrix (lib_mat2d.*)
USE_MAT2D=0

# 1=use MP3 decoder, 0=not used (mp3/lib_mp3.*)
USE_MP3=0

# 1=use PNG support, 0=not used (_lib/pic/lib_png*)
USE_PNG=0

# 1=use REAL numbers, 0=not used (_lib/real/*)
USE_REAL=0

# 1=use text "string", 0=not used (lib_text*)
USE_TEXT=1

##############################################################################
#                         Drivers configuration
##############################################################################
# Select driver modules (folder _drv)
# If you add a new flag, add the update flag to _drv\_makefile.inc.

# 1=use BarePi CalcKey, 0=not used (drv_calckey.*)
USE_CALCKEY=0

# 1=use EEPROM driver, 0=not used (drv_eeprom.*)
USE_EEPROM=1

# 1=use I2C bus driver, 0=not used (drv_i2cbus.*)
USE_I2CBUS=1

# 1=use BarePi KeyPad, 0=not used (drv_keypad.*)
USE_KEYPAD=1

# 1=use LCD SPI display driver, 0=not used (drv_lcd.*)
USE_LCD=1

# 1=use LCD TEXT I2C display driver, 0=not used (drv_lcdtxt.*)
USE_LCDTXT=0

# 1=use BarePi LED12 display, 0=not used (drv_led12.*)
USE_LED12=0

# 1=use BarePi MiniKey, 0=not used (drv_minikey.*)
USE_MINIKEY=0

# 1=use BarePi 12-channel PORT module, 0=not used (drv_port12.*)
USE_PORT12=0

# 1=use PWM sound output, 0=not used (drv_pwmsnd.*)
USE_PWMSND=1

# 1=use RTC driver, 0=not used (drv_rtc.*)
USE_RTC=1

# 1=use SSD1306 display driver, 0=not used (drv_ssd1306.*)
USE_SSD1306=0

##############################################################################
#                         Font configuration
##############################################################################
# For available options see config_def.mk in root directory.

# 1=use support of proportional fonts, 0=not used
USE_FONTPROP=0

##############################################################################
#                         SDK configuration
##############################################################################
# For available options see config_def.mk in root directory.

##############################################################################
#                  Include build Makefile 2nd stage - Build
##############################################################################

# Makefile includes
include ${PI_ROOT_PATH}/Makefile2.mk
