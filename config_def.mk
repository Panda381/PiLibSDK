
##############################################################################
#
#                     Makefile default configuration
#
##############################################################################

##############################################################################
#                         Library configuration
##############################################################################
# Select library modules (folder _lib)
# If you add a new flag, add the update flag to _lib\_makefile.inc.

# 1=use BIT integers, 0=not used (_lib/bigint/*)
USE_BIGINT=0

# 1=use Calendar 32-bit (year range 1970..2099), 0=not used (lib_calendar.*)
USE_CALENDAR=1

# 1=use Calendar 64-bit (year range -29227..+29227), 0=not used (lib_calendar64.*)
USE_CALENDAR64=0

# 1=use CRC Cyclic Redundancy Check, 0=not used (lib_crc.*)
USE_CRC=1

# 1=use Decode integer number, 0=not used (lib_decnum.*)
USE_DECNUM=1

# 1=use Drawing library, 0=not used (lib_draw.*)
USE_DRAW=1

# 1=use FAT file system, 0=not used (lib_fat.*)
USE_FAT=1

# 1=use JPG support, 0=not used (_lib/pic/lib_jpg*)
USE_JPG=0

# 1=use Doubly Linked List, 0=not used (lib_list.*)
USE_LIST=1

# 1=use memory allocator, 0=not used (lib_malloc.*)
USE_MALLOC=1

# 1=use 2D transformation matrix (lib_mat2d.*)
USE_MAT2D=0

# 1=use MP3 decoder, 0=not used (mp3/lib_mp3.*)
USE_MP3=0

# 1=use PNG support, 0=not used (_lib/pic/lib_png*)
USE_PNG=0

# 1=use Formatted print, 0=not used (lib_print.*)
USE_PRINT=1

# 1=use Random number generator, 0=not used (lib_rand.*)
USE_RAND=1

# 1=use REAL numbers, 0=not used (_lib/real/*)
USE_REAL=0

# 1=use Data stream, 0=not used (lib_stream.*)
USE_STREAM=1

# 1=use text "string", 0=not used (lib_text*)
USE_TEXT=1

# 1=use Tree list, 0=not used (lib_tree.*)
USE_TREE=1

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
USE_MINIKEY=1

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
# Select fonts (folder _font)
# If you add a new flag, add the update flag to _font\_makefile.inc.

# 1=include all fixed fonts, 0=include only default font FontBold8x16
USE_FONTFIX=1

# 1=use support of proportional fonts, 0=not used
USE_FONTPROP=0

# 1=include Arial fonts, 0=include only default font FontARB10
USE_FONTPROP_ARIAL=1

# 1=include Garamond fonts, 0=not included
USE_FONTPROP_GARAMOND=1

# 1=include Lucida Grande fonts, 0=not included
USE_FONTPROP_LUCIDA=1

# 1=include Sabon fonts, 0=not included
USE_FONTPROP_SABON=1

# 1=include Tahoma fonts, 0=not included
USE_FONTPROP_TAHOMA=1

# 1=include mixed fonts, 0=not included
USE_FONTPROP_MIXED=1

##############################################################################
#                         SDK configuration
##############################################################################
# Select SDK modules (foldes _sdk)
# If you add a new flag, add the update flag to _sdk\_makefile.inc.
