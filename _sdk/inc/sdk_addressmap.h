
// ****************************************************************************
//
//                               Address map
//
// ****************************************************************************
// Included from both C and assembly.

#ifndef _SDK_ADDRESSMAP_H
#define _SDK_ADDRESSMAP_H

// ARM peripherals base address
#if RASPPI == 1
#define ARM_IO_BASE		0x20000000
#elif RASPPI <= 3
#define ARM_IO_BASE		0x3F000000
#elif RASPPI == 4
#define ARM_IO_BASE		0xFE000000
#else
#define ARM_IO_BASE		0x107C000000UL
#endif

// ARM peripherals end address
#if RASPPI <= 4
#define ARM_IO_END		(ARM_IO_BASE + 0x00FFFFFF)
#else
#define ARM_IO_END		(ARM_IO_BASE + 0x03FFFFFF)
#endif

#if RASPPI <= 3
#define ARM_LOCAL_BASE		0x40000000		// ARM Local
#else
#define ARM_LOCAL_BASE		0xFF800000		// ARM Local
#endif

#define ARM_LOCAL_END		(ARM_LOCAL_BASE + 0x0003FFFF)

// GPI peripherals base address
#define GPU_IO_BASE		0x7E000000
#define GPU_CACHED_BASE		0x40000000
#define GPU_UNCACHED_BASE	0xC0000000

#if RASPPI == 1
#define GPU_MEM_BASE	GPU_CACHED_BASE
#else
#define GPU_MEM_BASE	GPU_UNCACHED_BASE
#endif

// Convert ARM address to GPU bus address (does also work for aliases, but not to peripherals)
#define BUS_ADDRESS(addr)	(((uintptr_t)(addr) & ~0xC0000000) | GPU_MEM_BASE)

// Convert GPU bus address to ARM address
#define ARM_ADDRESS(addr)	((void*)((uintptr_t)(addr) & ~0xC0000000))

// ARM peripherals
#define ARM_SYSTIMER_BASE	(ARM_IO_BASE+0x00003000)	// System timer
#define ARM_INTC_BASE		(ARM_IO_BASE+0x0000B200)	// Interrupt controller
#define ARM_ARMTIMER_BASE	(ARM_IO_BASE+0x0000B400)	// ARM timer
#define ARM_GPIO_BASE		(ARM_IO_BASE+0x00200000)	// GPIO base

#if RASPPI <= 4
#define	ARM_DMA_BASE		(ARM_IO_BASE+0x00007000)	// DMA controller
#define ARM_DMA15_BASE		(ARM_IO_BASE+0x00E05000)	// DMA15 controller
#else
#define ARM_DMA_BASE		0x1000010000UL			// DMA controller
#endif

#if RASPPI <= 4
#define MAILBOX_BASE		(ARM_IO_BASE+0x0000B880)	// Mailbox base
#else
#define MAILBOX_BASE		(ARM_IO_BASE+0x00013880)	// Mailbox base
#endif

#if RASPPI <= 4
#define ARM_PM_BASE		(ARM_IO_BASE+0x00100000)	// Power Manager
#else
#define ARM_PM_BASE		(ARM_IO_BASE+0x01200000)	// Power Manager
#endif

#define ARM_CM_BASE		(ARM_IO_BASE+0x00101000)	// Clock Manager

#define ARM_HW_RNG_BASE		(ARM_IO_BASE+0x00104000)	// Hardware Random Number Generator

#define ARM_PWM_BASE		(ARM_IO_BASE+0x0020C000)	// Pulse Width Modulator 0
#if RASPPI == 4
#define ARM_PWM1_BASE		(ARM_IO_BASE+0x0020C800)	// Pulse Width Modulator 1
#endif

#define ARM_AUX_BASE		(ARM_IO_BASE+0x00215000)	// AUX base address

// External Mass Media Controller (SD Card)
// ... EMMC is used to access WLAN on Pi 4
#if RASPPI <= 4
#define ARM_EMMC_BASE		(ARM_IO_BASE+0x00300000)	// EMMC External Mass Media Controller (SD Card and Wifi)
#else
#define ARM_EMMC_BASE		0x1000FFF000UL
#endif

#if RASPPI >= 4
#define ARM_EMMC2_BASE		(ARM_IO_BASE+0x00340000)	// EMMC External Mass Media Controller 2 (SD Card)
#endif

#define ARM_UART0_BASE		(ARM_IO_BASE+0x00201000)	// UART0

#if RASPPI < 4
#define ARM_SDHOST_BASE		(ARM_IO_BASE+0x00202000)	// SDHost controller (SD card)
#endif

#define ARM_PCM_BASE		(ARM_IO_BASE+0x00203000)	// PCM / I2S Audio Module

#define ARM_SPI0_BASE		(ARM_IO_BASE+0x00204000)	// SPI0 Master

// I2C Master (BSC Broadcom Serial Controller Master)
#define ARM_I2C0_BASE		(ARM_IO_BASE+0x00205000)	// I2C0 master
#define ARM_I2C1_BASE		(ARM_IO_BASE+0x00804000)	// I2C1 master
#define ARM_I2C2_BASE		(ARM_IO_BASE+0x00808000)	// I2C2 master ... used by HDMI interface, should not be accessed by user programs
#if RASPPI >= 4
#define ARM_I2C3_BASE		(ARM_IO_BASE+0x00205600)	// I2C3 master
#define ARM_I2C4_BASE		(ARM_IO_BASE+0x00205800)	// I2C4 master
#define ARM_I2C5_BASE		(ARM_IO_BASE+0x00205A80)	// I2C5 master
#define ARM_I2C6_BASE		(ARM_IO_BASE+0x00205C00)	// I2C6 master
#endif

// BSC Broadcom Serial Controller Slave (I2C / SPI Slave)
#define ARM_BSC_SLAVE_BASE	(ARM_IO_BASE+0x00214000)	// BSC Slave (I2C / SPI Slave)

// Generic Interrupt Controller (GIC-400)
#if RASPPI == 4
#define ARM_GICD_BASE		0xFF841000
#define ARM_GICC_BASE		0xFF842000
#define ARM_GIC_END		0xFF847FFF
#elif RASPPI > 4
#define ARM_GICD_BASE		(ARM_IO_BASE+0x3FF9000UL)
#define ARM_GICC_BASE		(ARM_IO_BASE+0x3FFA000UL)
#define ARM_GIC_END		(ARM_IO_BASE+0x3FFFFFFUL)
#endif

#endif // _SDK_ADDRESSMAP_H
