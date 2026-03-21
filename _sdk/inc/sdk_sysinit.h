
// ****************************************************************************
//
//                          System initialize
//
// ****************************************************************************

#ifndef _SDK_SYSINIT_H
#define _SDK_SYSINIT_H

#ifdef __cplusplus
extern "C" {
#endif

// system info
extern u32 FirmwareVersion;	// firmware version (= build time in Unix format)
extern u32 BoardModel;		// board model
extern u32 BoardRevision;	// board revision
extern u64 BoardSerial;		// board serial
extern u32 ARMMemAddr;		// ARM memory address
extern u32 ARMMemSize;		// ARM memory size
extern u32 VCMemAddr;		// VideoCore memory address
extern u32 VCMemSize;		// VideoCore memory size
extern u32 ARMClock;		// ARM clock in [Hz]
extern u32 CoreClock;		// VideoCore clock in [Hz]
extern u32 EMMCClock;		// EMMC clock
extern u32 UARTClock;		// UART clock

extern u32 SysTickNs;		// number of VideCore clock ticks per 1 ns << 32 (= (CoreClock << 32)/1000000000)
extern u32 SysTickUs;		// number of VideCore clock ticks per 1 us
extern u32 SysTickMs;		// number of VideCore clock ticks per 1 ms
extern u32 NsSysTick;		// number of ns per 1 VideoCore clock tick << 24 (= (1000000000 << 24)/CoreClock)
extern u32 UsSysTick;		// number of us per 1 VideoCore clock tick << 32 (= (1000000 << 32)/CoreClock)
extern u32 MsSysTick;		// number of ms per 1 VideoCore clock tick << 32 (= (1000 << 32)/CoreClock)

// Board revision code:
//      https://github.com/AndrewFromMelbourne/raspberry_pi_revision
//		bit 0..3: revision 1.x (0 -> 1.0, 1 -> 1.1, 2 -> 2.2)
//		bit 4..11: board model
//			0 = Pi 1 A
//			1 = Pi 1 B
//			2 = Pi 1 A+
//			3 = Pi 1 B+
//			4 = Pi 2 B
//			5 = Alpha
//			6 = Pi CM 1
//			8 = Pi 3 B
//			9 = Pi Zero
//			0x0A: Pi CM 3
//			0x0C = Pi Zero W
//			0x0D = Pi 3 B+
//			0x0E = Pi 3 A+
//			0x10 = Pi CM 3+
//			0x11 = Pi 4 B
//			0x12 = Pi Zero 2 W
//			0x13 = Pi 400
//			0x14 = Pi CM 4
//			0x15 = Pi CM 4S
//			0x17 = Pi 5
//			0x18 = Pi CM 5
//			0x19 = Pi 500
//			0x1A = Pi CM 5 Lite
//			0x1B = Pi CM 0
//		bit 12..15: SoC (0 -> BCM2835, 1 -> BCM2836, 2 -> BCM2837/BCM2710A1, 3 -> BCM2711)
//		bit 16..19: Manufacturer (0 -> Sony UK, 1 -> Egoman, 2 -> Embest, 4 -> Sony Japan)
//		bit 20..22: RAM size (0 -> 256 MB, 1 -> 512 MB, 2 -> 1 GB, 3 -> 2 GB, 4 -> 4 GB, 5 -> 8 GB)
//		bit 23: 1=new style of the revision
//		bit 25: Warranty 1=Bit violated (overvolting)
//	old style revision: https://gist.github.com/apetrone/5850426
//		2 = Pi 1 B rev. 1.0
//		3 = Pi 1 B rev. 1.0 + Fuses mod and D14 removed
//		4 = Pi 1 B rev. 2.0 256 MB (Sony)
//		5 = Pi 1 B rev. 2.0 256 MB (Qisda)
//		6 = Pi 1 B rev. 2.0 256 MB (Egoman)
//		7 = Pi 1 A rev. 2.0 256 MB (Egoman)
//		8 = Pi 1 A rev. 2.0 256 MB (Sony)
//		9 = Pi 1 A rev. 2.0 256 MB (Qisda)
//		0x0D = Pi 1 B rev. 2.0 512 MB (Egoman)
//		0x0E = Pi 1 B rev. 2.0 512 MB (Sony)
//		0x0F = Pi 1 B rev. 2.0 512 MB (Qisda)

#if AARCH == 32

// Initialize VFP coprocessor
void VfpInit(void);

// Terminate VFP coprocessor
void VfpTerm(void);

#endif // AARCH == 32

// System initialize (called from startup.S and startup64.S)
void SysInit(void);

#if USE_MULTICORE		// 1=use multicore (for applications), 0=do not use other cores (for loader)
#if RASPPI > 1
// Core system initialize
void SysInitSecondary(void);
#endif // RASPPI > 1
#endif // USE_MULTICORE

#if !USE_MULTICORE		// 1=use multicore (for applications), 0=do not use other cores (for loader)
// system terminate (cannot be run with multicore, used only by boot loader)
void SysTerm(void);
#endif // !USE_MULTICORE

// convert nanoseconds to system core ticks
INLINE u32 NsToSysTicks(u32 ns) { return (u32)(((u64)ns*SysTickNs) >> 32); }

// convert microseconds to system core ticks
INLINE u32 UsToSysTicks(u32 us) { return us*SysTickUs; }

// convert milliseconds to system core ticks
INLINE u32 MsToSysTicks(u32 ms) { return ms*SysTickMs; }

// convert system core ticks to nanoseconds
INLINE u32 SysTicksToNs(u32 clk) { return (u32)(((u64)clk*NsSysTick) >> 24); }

// convert system core ticks to microseconds
INLINE u32 SysTicksToUs(u32 clk) { return (u32)(((u64)clk*UsSysTick) >> 32); }

// convert system core ticks to milliseconds
INLINE u32 SysTicksToMs(u32 clk) { return (u32)(((u64)clk*MsSysTick) >> 32); }

#ifdef __cplusplus
}
#endif

#endif // _SDK_SYSINIT_H
