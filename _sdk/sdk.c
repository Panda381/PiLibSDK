
#include "../includes.h"	// includes

#include "src/sdk_armlocal.c"		// ARM Local
#include "src/sdk_armtimer.c"		// ARM timer
#include "src/sdk_aux.c"		// Auxiliaries - UART1, SPI1, SPI2
#include "src/sdk_bsc.c"		// BSC Broadcom Serial Controller Slave (I2C slave, SPI slave)
#include "src/sdk_cpu.c"		// CPU control
#include "src/sdk_dma.c"		// DMA controller
#include "src/sdk_emmc.c"		// EMMC - External Mass Media Controller (SD card)
#include "src/sdk_framebuf.c"		// Frame Buffer
#include "src/sdk_gpio.c"		// GPIO - General Purpose I/O
#include "src/sdk_gpioclock.c"		// GPIO Clock Manager
#include "src/sdk_i2c.c"		// I2C0, I2C1 master (BSC - Broadcom Serial Controller Master)
#include "src/sdk_interr.c"		// Interrupt controller
#include "src/sdk_mailbox.c"		// Mailbox
#include "src/sdk_mailprop.c"		// Mailbox properties
#include "src/sdk_mmu.c"		// MMU Memory Management Unit
#include "src/sdk_multicore.c"		// Multicore
#include "src/sdk_pwm.c"		// PWM - Pulse Width Modulator
#include "src/sdk_pwr.c"		// Power Manager
#include "src/sdk_rng.c"		// Hardware Random Number Generator
#include "src/sdk_sdhost.c"		// SD Host driver
#include "src/sdk_spi.c"		// SPI0
#include "src/sdk_spinlock.c"		// SpinLock
#include "src/sdk_sysinit.c"		// System initialize
#include "src/sdk_systimer.c"		// System timer
#include "src/sdk_uart.c"		// UART0
