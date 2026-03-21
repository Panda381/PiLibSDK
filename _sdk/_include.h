
#include "../global.h"

#ifndef _SDK_INCLUDE_H
#define _SDK_INCLUDE_H

#include "inc/sdk_addressmap.h"		// address map

#include "inc/sdk_armlocal.h"		// ARM Local
#include "inc/sdk_armtimer.h"		// ARM timer
#include "inc/sdk_aux.h"		// Auxiliaries - UART1, SPI1, SPI2
#include "inc/sdk_bsc.h"		// BSC Broadcom Serial Controller Slave (I2C slave, SPI slave)
#include "inc/sdk_cpu.h"		// CPU control
#include "inc/sdk_dma.h"		// DMA controller
#include "inc/sdk_emmc.h"		// EMMC - External Mass Media Controller (SD card)
#include "inc/sdk_framebuf.h"		// Frame Buffer
#include "inc/sdk_gpio.h"		// GPIO - General Purpose I/O
#include "inc/sdk_gpioclock.h"		// GPIO Clock Manager
#include "inc/sdk_i2c.h"		// I2C0, I2C1 master (BSC - Broadcom Serial Controller Master)
#include "inc/sdk_interr.h"		// Interrupt controller
#include "inc/sdk_mailprop.h"		// Mailbox properties
#include "inc/sdk_mailbox.h"		// Mailbox
#include "inc/sdk_mmu.h"		// MMU Memory Management Unit
#include "inc/sdk_multicore.h"		// Multicore
#include "inc/sdk_pwm.h"		// PWM - Pulse Width Modulator
#include "inc/sdk_pwr.h"		// Power Manager
#include "inc/sdk_rng.h"		// Hardware Random Number Generator
#include "inc/sdk_sdhost.h"		// SD Host driver
#include "inc/sdk_spi.h"		// SPI0
#include "inc/sdk_spinlock.h"		// SpinLock
#include "inc/sdk_sysinit.h"		// System initialize
#include "inc/sdk_systimer.h"		// System timer
#include "inc/sdk_uart.h"		// UART0

#endif // _SDK_INCLUDE_H
