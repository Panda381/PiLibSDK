
ifeq ($(strip $(AARCH)),32)
ASRC += ${PI_SDK_DIR}/startup32.S
else
ASRC += ${PI_SDK_DIR}/startup64.S
endif

ifeq (${OS},Windows_NT)
CSRC += ${PI_SDK_DIR}/sdk.c
else
CSRC += ${PI_SDK_DIR}/src/sdk_armlocal.c
CSRC += ${PI_SDK_DIR}/src/sdk_armtimer.c
CSRC += ${PI_SDK_DIR}/src/sdk_aux.c
CSRC += ${PI_SDK_DIR}/src/sdk_bsc.c
CSRC += ${PI_SDK_DIR}/src/sdk_cpu.c
CSRC += ${PI_SDK_DIR}/src/sdk_dma.c
CSRC += ${PI_SDK_DIR}/src/sdk_emmc.c
CSRC += ${PI_SDK_DIR}/src/sdk_framebuf.c
CSRC += ${PI_SDK_DIR}/src/sdk_gpio.c
CSRC += ${PI_SDK_DIR}/src/sdk_gpioclock.c
CSRC += ${PI_SDK_DIR}/src/sdk_i2c.c
CSRC += ${PI_SDK_DIR}/src/sdk_interr.c
CSRC += ${PI_SDK_DIR}/src/sdk_mailbox.c
CSRC += ${PI_SDK_DIR}/src/sdk_mailprop.c
CSRC += ${PI_SDK_DIR}/src/sdk_mmu.c
CSRC += ${PI_SDK_DIR}/src/sdk_multicore.c
CSRC += ${PI_SDK_DIR}/src/sdk_pwm.c
CSRC += ${PI_SDK_DIR}/src/sdk_pwr.c
CSRC += ${PI_SDK_DIR}/src/sdk_rng.c
CSRC += ${PI_SDK_DIR}/src/sdk_sdhost.c
CSRC += ${PI_SDK_DIR}/src/sdk_spi.c
CSRC += ${PI_SDK_DIR}/src/sdk_spinlock.c
CSRC += ${PI_SDK_DIR}/src/sdk_sysinit.c
CSRC += ${PI_SDK_DIR}/src/sdk_systimer.c
CSRC += ${PI_SDK_DIR}/src/sdk_uart.c
endif
