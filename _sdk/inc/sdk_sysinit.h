
// ****************************************************************************
//
//                          System initialize
//
// ****************************************************************************

#ifndef _SDK_SYSINIT_H
#define _SDK_SYSINIT_H

// SoC chip type
#define SOC_BCM2835	0	// Broadcom BCM2835
#define SOC_BCM2836	1	// Broadcom BCM2836
#define SOC_BCM2837	2	// Broadcom BCM2837/BCM2710A1
#define SOC_BCM2711	3	// Broadcom BCM2711
#define SOC_BCM2712	4	// Broadcom BCM2712
#define SOC_UNKNOWN	5	// unknown type

#define SOC_NUM		6	// number of SoC chip types

// ARM CPU type
#define ARM_CPU_ARM1176JZFS	0	// Broadcom BCM2835: ARM1176JZF-S
#define ARM_CPU_CORTEXA7	1	// Broadcom BCM2836: Cortex-A7
#define ARM_CPU_CORTEXA53	2	// Broadcom BCM2837/BCM2710A1: Cortex-A53
#define ARM_CPU_CORTEXA72	3	// Broadcom BCM2711: Cortex-A72
#define ARM_CPU_CORTEXA76	4	// Broadcom BCM2712: Cortex-A76
#define ARM_CPU_UNKNOWN		5	// unknown ARM CPU type

#define ARM_CPU_NUM		6	// number of ARM CPU types

// SoC Manufacturer
#define SOC_MANU_SONYUK		0	// Sony UK
#define SOC_MANU_EGOMAN		1	// Egoman
#define SOC_MANU_EMBEST		2	// Embest
#define SOC_MANU_SONYJAP	3	// Sony Japan
#define SOC_MANU_EMBEST2	4	// Embest
#define SOC_MANU_STADIUM	5	// Stadium
#define SOC_MANU_QISDA		6	// Qisda

#define SOC_MANU_UNKOWN		7	// unknown manufacturer

#define SOC_MANU_NUM		8	// number of SoC manufacturers

// system info
extern u32 FirmwareVersion;	// firmware version (= build time in Unix format)
extern u32 BoardModel;		// board model
extern u32 BoardRevision;	// board revision
extern u64 BoardSerial;		// board serial
extern const char* BoardModelName; // board model name (empty = unknown model)
extern int BoardModelRev;	// board model revision (0, 1 or 2)
extern u32 BoardMemSize;	// memory size in MB
extern int BoardSoc;		// board SoC chip type SOC_*
extern const char* BoardSocName; // board SoC chip type name
extern int BoardSocManu;	// board SoC chip manufacturer SOC_MANU_*
extern const char* BoardSocManuName; // board SoC chip manufacturer name
extern int ArmCpuType;		// ARM CPU type
extern const char* ArmCpuTypeName; // ARM CPU type name
extern const char* ArmArchTypeName; // ARM CPU architecture name
extern const char* GpuName;	// GPU name
extern const char* GpuMaxResName; // GPU max. resolution
#define MACADDR_LEN	6	// length of MAC address in bytes
extern u8 MACAddress[MACADDR_LEN]; // MAC address
extern u32 CoreVoltage;		// core voltage in [uV]
extern u32 SDRAMCVoltage;	// SDRAM C voltage in [uV] (SDRAM Controller)
extern u32 SDRAMPVoltage;	// SDRAM P voltage in [uV] (SDRAM Physical)
extern u32 SDRAMIVoltage;	// SDRAM I voltage in [uV] (SDRAM Interface)

extern u32 ARMMemAddr;		// ARM memory address
extern u32 ARMMemSize;		// ARM memory size
extern u32 VCMemAddr;		// VideoCore memory address
extern u32 VCMemSize;		// VideoCore memory size
extern u32 ARMClock;		// ARM clock in [Hz]
extern u32 CoreClock;		// VideoCore clock in [Hz]
extern u32 EMMCClock;		// EMMC clock
extern u32 UARTClock;		// UART clock
extern u32 V3DClock;		// V3D clock
extern u32 H264Clock;		// H264 clock
extern u32 ISPClock;		// ISP clock
extern u32 SDRAMClock;		// SDRAM clock

extern u32 SysTickNs;		// number of VideCore clock ticks per 1 ns << 32 (= (CoreClock << 32)/1000000000)
extern u32 SysTickUs;		// number of VideCore clock ticks per 1 us
extern u32 SysTickMs;		// number of VideCore clock ticks per 1 ms
extern u32 NsSysTick;		// number of ns per 1 VideoCore clock tick << 24 (= (1000000000 << 24)/CoreClock)
extern u32 UsSysTick;		// number of us per 1 VideoCore clock tick << 32 (= (1000000 << 32)/CoreClock)
extern u32 MsSysTick;		// number of ms per 1 VideoCore clock tick << 32 (= (1000 << 32)/CoreClock)

// board model name list - old style
#define BOARDMODELOLD_NUM 23
extern const char* BoardModelOldName[BOARDMODELOLD_NUM];

// board memory size in MB - old style
extern const u32 BoardModelOldMem[BOARDMODELOLD_NUM];

// board SoC manufacturer - old style
extern const int BoardSocManuOld[BOARDMODELOLD_NUM];

// board model name list - new style
#define BOARDMODELNEW_NUM 29
extern const char* BoardModelNewName[BOARDMODELNEW_NUM];

// SoC chip type name
extern const char* BoardSocNameList[SOC_NUM];

// CPU name
extern const char* ArmCpuNameList[SOC_NUM];

// CPU architecture name
extern const char* ArmArchNameList[SOC_NUM];

// SoC Manufacturer name
extern const char* BoardSocManuNameList[SOC_MANU_NUM];

#if AARCH == 32

// Initialize VFP coprocessor
extern "C" void VfpInit(void);

// Terminate VFP coprocessor
extern "C" void VfpTerm(void);

#endif // AARCH == 32

// Get current core temperature in thousands of °C (0 = on errror)
u32 GetCoreTemp(void);

// System initialize (called from startup.S and startup64.S)
extern "C" void SysInit(void);

#if (CORES > 1) && USE_MULTICORE	// 1=use multicore (for applications), 0=do not use other cores (for loader)
#if RASPPI > 1
// Core system initialize
extern "C" void SysInitSecondary(void);
#endif // RASPPI > 1
#endif // (CORES > 1) && USE_MULTICORE

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

#endif // _SDK_SYSINIT_H
