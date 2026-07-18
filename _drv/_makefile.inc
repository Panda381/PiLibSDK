
DEFINE += -D USE_CALCKEY=$(USE_CALCKEY)
DEFINE += -D USE_EEPROM=$(USE_EEPROM)
DEFINE += -D USE_I2CBUS=$(USE_I2CBUS)
DEFINE += -D USE_KEYPAD=$(USE_KEYPAD)
DEFINE += -D USE_LCD=$(USE_LCD)
DEFINE += -D USE_LCDTXT=$(USE_LCDTXT)
DEFINE += -D USE_LED12=$(USE_LED12)
DEFINE += -D USE_MINIKEY=$(USE_MINIKEY)
DEFINE += -D USE_PORT12=$(USE_PORT12)
DEFINE += -D USE_PWMSND=$(USE_PWMSND)
DEFINE += -D USE_RTC=$(USE_RTC)
DEFINE += -D USE_SSD1306=$(USE_SSD1306)

ifeq (${OS},Windows_NT)
SRC += ${PI_DRV_DIR}/drv.cpp
else

ifeq ($(strip $(USE_CALCKEY)),1)
SRC += ${PI_DRV_DIR}/src/drv_calckey.cpp
endif
ifeq ($(strip $(USE_EEPROM)),1)
SRC += ${PI_DRV_DIR}/src/drv_eeprom.cpp
endif
ifeq ($(strip $(USE_I2CBUS)),1)
SRC += ${PI_DRV_DIR}/src/drv_i2cbus.cpp
endif
ifeq ($(strip $(USE_KEYPAD)),1)
SRC += ${PI_DRV_DIR}/src/drv_keypad.cpp
endif
ifeq ($(strip $(USE_LCD)),1)
SRC += ${PI_DRV_DIR}/src/drv_lcd.cpp
endif
ifeq ($(strip $(USE_LCDTXT)),1)
SRC += ${PI_DRV_DIR}/src/drv_lcdtxt.cpp
endif
ifeq ($(strip $(USE_LED12)),1)
SRC += ${PI_DRV_DIR}/src/drv_led12.cpp
endif
ifeq ($(strip $(USE_MINIKEY)),1)
SRC += ${PI_DRV_DIR}/src/drv_minikey.cpp
endif
ifeq ($(strip $(USE_PORT12)),1)
SRC += ${PI_DRV_DIR}/src/drv_port12.cpp
endif
ifeq ($(strip $(USE_PWMSND)),1)
SRC += ${PI_DRV_DIR}/src/drv_pwmsnd.cpp
endif
ifeq ($(strip $(USE_RTC)),1)
SRC += ${PI_DRV_DIR}/src/drv_rtc.cpp
endif
ifeq ($(strip $(USE_SSD1306)),1)
SRC += ${PI_DRV_DIR}/src/drv_ssd1306.cpp
endif

endif
