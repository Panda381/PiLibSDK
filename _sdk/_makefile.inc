
ifeq ($(strip $(AARCH)),32)
ASRC += ${PI_SDK_DIR}/startup32.S
else
ASRC += ${PI_SDK_DIR}/startup64.S
endif

ifeq (${OS},Windows_NT)
SRC += ${PI_SDK_DIR}/sdk.cpp
else
SRC += ${PI_SDK_DIR}/src/sdk_armlocal.cpp
SRC += ${PI_SDK_DIR}/src/sdk_armtimer.cpp
SRC += ${PI_SDK_DIR}/src/sdk_aux.cpp
SRC += ${PI_SDK_DIR}/src/sdk_bsc.cpp
SRC += ${PI_SDK_DIR}/src/sdk_cpu.cpp
SRC += ${PI_SDK_DIR}/src/sdk_dma.cpp
SRC += ${PI_SDK_DIR}/src/sdk_emmc.cpp
SRC += ${PI_SDK_DIR}/src/sdk_framebuf.cpp
SRC += ${PI_SDK_DIR}/src/sdk_gpio.cpp
SRC += ${PI_SDK_DIR}/src/sdk_gpioclock.cpp
SRC += ${PI_SDK_DIR}/src/sdk_i2c.cpp
SRC += ${PI_SDK_DIR}/src/sdk_interr.cpp
SRC += ${PI_SDK_DIR}/src/sdk_mailbox.cpp
SRC += ${PI_SDK_DIR}/src/sdk_mailprop.cpp
SRC += ${PI_SDK_DIR}/src/sdk_mmu.cpp
SRC += ${PI_SDK_DIR}/src/sdk_multicore.cpp
SRC += ${PI_SDK_DIR}/src/sdk_pwm.cpp
SRC += ${PI_SDK_DIR}/src/sdk_pwr.cpp
SRC += ${PI_SDK_DIR}/src/sdk_rng.cpp
SRC += ${PI_SDK_DIR}/src/sdk_sdhost.cpp
SRC += ${PI_SDK_DIR}/src/sdk_spi.cpp
SRC += ${PI_SDK_DIR}/src/sdk_spinlock.cpp
SRC += ${PI_SDK_DIR}/src/sdk_sysinit.cpp
SRC += ${PI_SDK_DIR}/src/sdk_systimer.cpp
SRC += ${PI_SDK_DIR}/src/sdk_uart.cpp
endif
