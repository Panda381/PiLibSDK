
// ****************************************************************************
//
//                          System initialize
//
// ****************************************************************************
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
//		bit 12..15: SoC (0 -> BCM2835, 1 -> BCM2836, 2 -> BCM2837/BCM2710A1, 3 -> BCM2711, 4 -> BCM2712)
//		bit 16..19: Manufacturer (0 -> Sony UK, 1 -> Egoman, 2 -> Embest, 3 -> Sony Japan, 4 -> Embest, 5 -> Stadium)
//		bit 20..22: RAM size (0 -> 256 MB, 1 -> 512 MB, 2 -> 1 GB, 3 -> 2 GB, 4 -> 4 GB, 5 -> 8 GB)
//		bit 23: 1=new style of the revision
//		bit 25: Warranty 1=Bit violated (overvolting)
//	old style revision: https://gist.github.com/apetrone/5850426
//		2 = Pi 1 B rev. 1.0 256 MB (Egoman)
//		3 = Pi 1 B rev. 1.0 + Fuses mod and D14 removed, 256 MB (Egoman)
//		4 = Pi 1 B rev. 2.0 256 MB (Sony UK)
//		5 = Pi 1 B rev. 2.0 256 MB (Qisda)
//		6 = Pi 1 B rev. 2.0 256 MB (Egoman)
//		7 = Pi 1 A rev. 2.0 256 MB (Egoman)
//		8 = Pi 1 A rev. 2.0 256 MB (Sony UK)
//		9 = Pi 1 A rev. 2.0 256 MB (Qisda)
//		0x0D = Pi 1 B rev. 2.0 512 MB (Egoman)
//		0x0E = Pi 1 B rev. 2.0 512 MB (Sony UK)
//		0x0F = Pi 1 B rev. 2.0 512 MB (Egoman)
//		0x10 = Pi 1 B+ rev. 1.0 512 MB (Sony UK)
//		0x11 = Pi CM 1 rev. 1.0 512 MB (Sony UK)
//		0x12 = Pi 1 A+ rev. 1.0 256 MB (Sony UK)
//		0x13 = Pi 1 B+ rev. 1.0 512 MB (Embest)
//		0x14 = Pi CM 1 rev. 1.0 512 MB (Embest)
//		0x15 = Pi 1 A+ rev. 1.0 256 MB/512 MB (Embest)

#include "../../includes.h"	// includes

// system info
u32 FirmwareVersion = 0;	// firmware version (= build time in Unix format)
u32 BoardModel = 0;		// board model
u32 BoardRevision = 0;		// board revision
u64 BoardSerial = 0;		// board serial
const char* BoardModelName = ""; // board model name (empty = unknown model)
int BoardModelRev = 0;		// board model revision (0, 1 or 2)
u32 BoardMemSize = 256;		// memory size in MB
int BoardSoc = SOC_UNKNOWN;	// board SoC chip type SOC_*
const char* BoardSocName = "";	// board SoC chip type name
int BoardSocManu = SOC_MANU_UNKOWN; // board SoC chip manufacturer SOC_MANU_*
const char* BoardSocManuName = ""; // board SoC chip manufacturer name
int ArmCpuType = ARM_CPU_UNKNOWN; // ARM CPU type
const char* ArmCpuTypeName = ""; // ARM CPU type name
const char* ArmArchTypeName = ""; // ARM CPU architecture name
const char* GpuName = "";	// GPU name
const char* GpuMaxResName = "";	// GPU max. resolution
u8 MACAddress[MACADDR_LEN] = { 0, 0, 0, 0, 0, 0 }; // MAC address
u32 CoreVoltage = 0;		// core voltage in [uV]
u32 SDRAMCVoltage = 0;		// SDRAM C voltage in [uV] (SDRAM Controller)
u32 SDRAMPVoltage = 0;		// SDRAM P voltage in [uV] (SDRAM Physical)
u32 SDRAMIVoltage = 0;		// SDRAM I voltage in [uV] (SDRAM Interface)

u32 ARMMemAddr = 0x00000000;	// ARM memory address
u32 ARMMemSize = 0x1C000000;	// ARM memory size
u32 VCMemAddr = 0x1C000000;	// VideoCore memory address
u32 VCMemSize = 0x04000000;	// VideoCore memory size
u32 ARMClock = 1000000000;	// ARM clock in [Hz]
u32 CoreClock = 250000000;	// VideoCore clock in [Hz]
u32 EMMCClock = 1000000;	// EMMC clock
u32 UARTClock = 1000000;	// UART clock
u32 V3DClock = 1000000;		// V3D clock
u32 H264Clock = 1000000;	// H264 clock
u32 ISPClock = 1000000;		// ISP clock
u32 SDRAMClock = 1000000;	// SDRAM clock

u32 SysTickNs = 1073741824;	// number of VideCore clock ticks per 1 ns << 32 (= (CoreClock << 32)/1000000000)
u32 SysTickUs = 250;		// number of VideCore clock ticks per 1 us
u32 SysTickMs = 250000;		// number of VideCore clock ticks per 1 ms
u32 NsSysTick = 67108864;	// number of ns per 1 VideoCore clock tick << 24 (= (1000000000 << 24)/CoreClock)
u32 UsSysTick = 17179869;	// number of us per 1 VideoCore clock tick << 32 (= (1000000 << 32)/CoreClock)
u32 MsSysTick = 17180;		// number of ms per 1 VideoCore clock tick << 32 (= (1000 << 32)/CoreClock)

// board model name - old style
#define BOARDMODELOLD_NUM 23
const char* BoardModelOldName[BOARDMODELOLD_NUM] = {
	"",			// 0
	"",			// 1
	"Pi 1B",		// 2
	"Pi 1B",		// 3
	"Pi 1B",		// 4
	"Pi 1B",		// 5
	"Pi 1B",		// 6
	"Pi 1A",		// 7
	"Pi 1A",		// 8
	"Pi 1A",		// 9
	"",			// 10
	"",			// 11
	"",			// 12
	"Pi 1B",		// 13
	"Pi 1B",		// 14
	"Pi 1B",		// 15
	"Pi 1B+",		// 16
	"Pi CM 1",		// 17
	"Pi 1A+",		// 18
	"Pi 1B+",		// 19
	"Pi CM 1",		// 20
	"Pi 1A+",		// 21
	"",			// 22
};

// board memory size in MB - old style
const u32 BoardModelOldMem[BOARDMODELOLD_NUM] = {
	0,			// 0
	0,			// 1
	256,			// 2
	256,			// 3
	256,			// 4
	256,			// 5
	256,			// 6
	256,			// 7
	256,			// 8
	256,			// 9
	0,			// 10
	0,			// 11
	0,			// 12
	512,			// 13
	512,			// 14
	512,			// 15
	512,			// 16
	512,			// 17
	256,			// 18
	512,			// 19
	512,			// 20
	256,			// 21
	0,			// 22
};

// board SoC manufacturer - old style
const int BoardSocManuOld[BOARDMODELOLD_NUM] = {
	SOC_MANU_UNKOWN,		// 0
	SOC_MANU_UNKOWN,		// 1
	SOC_MANU_EGOMAN,		// 2
	SOC_MANU_EGOMAN,		// 3
	SOC_MANU_SONYUK,		// 4
	SOC_MANU_QISDA,			// 5
	SOC_MANU_EGOMAN,		// 6
	SOC_MANU_EGOMAN,		// 7
	SOC_MANU_SONYUK,		// 8
	SOC_MANU_QISDA,			// 9
	SOC_MANU_UNKOWN,		// 10
	SOC_MANU_UNKOWN,		// 11
	SOC_MANU_UNKOWN,		// 12
	SOC_MANU_EGOMAN,		// 13
	SOC_MANU_SONYUK,		// 14
	SOC_MANU_EGOMAN,		// 15
	SOC_MANU_SONYUK,		// 16
	SOC_MANU_SONYUK,		// 17
	SOC_MANU_EMBEST,		// 18
	SOC_MANU_EMBEST,		// 19
	SOC_MANU_EMBEST,		// 20
	SOC_MANU_EMBEST,		// 21
	SOC_MANU_UNKOWN,		// 22
};

// board model name - new style
#define BOARDMODELNEW_NUM 29
const char* BoardModelNewName[BOARDMODELNEW_NUM] = {
	"Pi 1A",		// 0
	"Pi 1B",		// 1
	"Pi 1A+",		// 2
	"Pi 1B+",		// 3
	"Pi 2B",		// 4
	"Alpha",		// 5
	"Pi CM 1",		// 6
	"",			// 7
	"Pi 3B",		// 8
	"Pi Zero",		// 9
	"Pi CM 3",		// 10
	"",			// 11
	"Pi Zero W",		// 12
	"Pi 3B+",		// 13
	"Pi 3A+",		// 14
	"",			// 15
	"Pi CM 3+",		// 16
	"Pi 4B",		// 17
	"Pi Zero 2 W",		// 18
	"Pi 400",		// 19
	"Pi CM 4",		// 20
	"Pi CM 4S",		// 21
	"",			// 22
	"Pi 5",			// 23
	"Pi CM 5",		// 24
	"Pi 500",		// 25
	"Pi CM 5 Lite",		// 26
	"Pi CM 0",		// 27
	"",			// 28
};

// SoC chip type name
const char* BoardSocNameList[SOC_NUM] = {
	"BCM2835",		// SOC_BCM2835	0	// Broadcom BCM2835
	"BCM2836",		// SOC_BCM2836	1	// Broadcom BCM2836
	"BCM2837/BCM2710A1",	// SOC_BCM2837	2	// Broadcom BCM2837/BCM2710A1
	"BCM2711",		// SOC_BCM2711	3	// Broadcom BCM2711
	"BCM2712",		// SOC_BCM2712	4	// Broadcom BCM2712
	"",			// SOC_UNKNOWN	5	// unknown type
};

// CPU name
const char* ArmCpuNameList[SOC_NUM] = {
	"ARM1176JZF-S",		// Broadcom BCM2835: ARM1176JZF-S
	"Cortex-A7",		// Broadcom BCM2836: Cortex-A7
	"Cortex-A53",		// Broadcom BCM2837/BCM2710A1: Cortex-A53
	"Cortex-A72",		// Broadcom BCM2711: Cortex-A72
	"Cortex-A76",		// Broadcom BCM2712: Cortex-A76
	"",			// unknown ARM CPU type
};	

// CPU architecture name
const char* ArmArchNameList[SOC_NUM] = {
	"Armv6Z",		// Broadcom BCM2835: ARM1176JZF-S
	"Armv7",		// Broadcom BCM2836: Cortex-A7
	"Armv8",		// Broadcom BCM2837/BCM2710A1: Cortex-A53
	"Armv8-A",		// Broadcom BCM2711: Cortex-A72
	"Armv8-A",		// Broadcom BCM2712: Cortex-A76
	"",			// unknown ARM CPU type
};	

// VideoCore name
const char* GpuNameList[SOC_NUM] = {
	"VideoCore IV",		// Broadcom BCM2835
	"VideoCore IV",		// Broadcom BCM2836
	"VideoCore IV",		// Broadcom BCM2837/BCM2710A1
	"VideoCore VI",		// Broadcom BCM2711
	"VideoCore VII",	// Broadcom BCM2712
	"",			// unknown GPU type
};	

// VideoCore max. resolution
const char* GpuMaxResNameList[SOC_NUM] = {
	"1920 x 1080",		// Broadcom BCM2835
	"1920 x 1080",		// Broadcom BCM2836
	"1920 x 1080",		// Broadcom BCM2837/BCM2710A1
	"3840 x 2160",		// Broadcom BCM2711
	"3840 x 2160",		// Broadcom BCM2712
	"",			// unknown GPU type
};	

// SoC Manufacturer name
const char* BoardSocManuNameList[SOC_MANU_NUM] = {
	"Sony UK",		// SOC_MANU_SONYUK	0	// Sony UK
	"Egoman",		// SOC_MANU_EGOMAN	1	// Egoman
	"Embest",		// SOC_MANU_EMBEST	2	// Embest
	"Sony Japan",		// SOC_MANU_SONYJAP	3	// Sony Japan
	"Embest",		// SOC_MANU_EMBEST	4	// Embest
	"Stadium",		// SOC_MANU_STADIUM	5	// Stadium
	"Qisda",		// SOC_MANU_QISDA	6	// Qisda
	"",			// SOC_MANU_UNKOWN	7	// unknown manufacturer
};

// get system info
void SysInfo(void)
{
	int i;

	// get firmware version
	MailPropInit();
	MailPropAdd(TAG_GET_FIRMWARE_VERSION);
	MailPropProcess();
	u32* p = MailPropGet(TAG_GET_FIRMWARE_VERSION);
	if (p != NULL) FirmwareVersion = p[1];

	// get board model
	MailPropInit();
	MailPropAdd(TAG_GET_BOARD_MODEL);
	MailPropProcess();
	p = MailPropGet(TAG_GET_BOARD_MODEL);
	if (p != NULL) BoardModel = p[1];

	// get board revision
	MailPropInit();
	MailPropAdd(TAG_GET_BOARD_REVISION);
	MailPropProcess();
	p = MailPropGet(TAG_GET_BOARD_REVISION);
	if (p != NULL) BoardRevision = p[1];

	// prepare board model name
	if (BoardRevision != 0)
	{
		if ((BoardRevision & B23) == 0) // old style of the revision?
		{
			// model name
			i = BoardRevision & 0xff;
			if (i >= BOARDMODELOLD_NUM) i = BOARDMODELOLD_NUM-1;
			const char* t = BoardModelOldName[i];
			BoardModelName = t;

			// is board model valid?
			if ((t != NULL) && (*t != 0))
			{
				// model revision
				BoardModelRev = ((i >= 4) && (i < 16)) ? 2 : 1;

				// memory size
				BoardMemSize = BoardModelOldMem[i];

				// SoC
				BoardSoc = SOC_BCM2835;
				BoardSocName = BoardSocNameList[SOC_BCM2835];

				// board SoC manufacturer
				BoardSocManu = BoardSocManuOld[i];
				BoardSocManuName = BoardSocManuNameList[BoardSocManu];
			}
		}
		else
		{
			i = (BoardRevision >> 4) & 0xff;
			if (i >= BOARDMODELNEW_NUM) i = BOARDMODELNEW_NUM-1;
			const char* t = BoardModelNewName[i];
			BoardModelName = t;

			// model revision
			BoardModelRev = BoardRevision & 0x0f;

			// memory size
			i = (BoardRevision >> 20) & 7;
			BoardMemSize = 1 << (i + 8);

			// SoC
			i = (BoardRevision >> 12) & 0x0f;
			if (i >= SOC_NUM) i = SOC_UNKNOWN;
			BoardSoc = i;
			BoardSocName = BoardSocNameList[i];

			// SoC manufacturer
			i = (BoardRevision >> 16) & 0x0f;
			if (i >= SOC_MANU_NUM) i = SOC_MANU_UNKOWN;
			BoardSocManu = i;
			BoardSocManuName = BoardSocManuNameList[i];
		}
	}

	// CPU name
	ArmCpuType = BoardSoc;
	ArmCpuTypeName = ArmCpuNameList[BoardSoc];
	ArmArchTypeName = ArmArchNameList[BoardSoc];
	GpuName = GpuNameList[BoardSoc];
	GpuMaxResName = GpuMaxResNameList[BoardSoc];

	// get board serial
	MailPropInit();
	MailPropAdd(TAG_GET_BOARD_SERIAL);
	MailPropProcess();
	p = MailPropGet(TAG_GET_BOARD_SERIAL);
	if (p != NULL) BoardSerial = (u64)p[1] | ((u64)p[2] << 32);

	// get ARM memory
	MailPropInit();
	MailPropAdd(TAG_GET_ARM_MEMORY);
	MailPropProcess();
	p = MailPropGet(TAG_GET_ARM_MEMORY);
	if (p != NULL)
	{
		ARMMemAddr = p[1];
		ARMMemSize = p[2];
	}

	// get VideoCore memory
	MailPropInit();
	MailPropAdd(TAG_GET_VC_MEMORY);
	MailPropProcess();
	p = MailPropGet(TAG_GET_VC_MEMORY);
	if (p != NULL)
	{
		VCMemAddr = p[1];
		VCMemSize = p[2];
	}

	// get ARM clock
	MailPropInit();
	MailPropAdd(TAG_GET_CLOCK_RATE, MP_CLK_ARM);
	MailPropProcess();
	p = MailPropGet(TAG_GET_CLOCK_RATE);
	if (p != NULL) ARMClock = p[2];

	// get VideoCore clock
	MailPropInit();
	MailPropAdd(TAG_GET_CLOCK_RATE, MP_CLK_CORE);
	MailPropProcess();
	p = MailPropGet(TAG_GET_CLOCK_RATE);
	if (p != NULL) CoreClock = p[2];

	// get EMMC clock
	MailPropInit();
	MailPropAdd(TAG_GET_CLOCK_RATE, MP_CLK_EMMC);
	MailPropProcess();
	p = MailPropGet(TAG_GET_CLOCK_RATE);
	if (p != NULL) EMMCClock = p[2];

	// get UART clock
	MailPropInit();
	MailPropAdd(TAG_GET_CLOCK_RATE, MP_CLK_UART);
	MailPropProcess();
	p = MailPropGet(TAG_GET_CLOCK_RATE);
	if (p != NULL) UARTClock = p[2];

	// get V3D clock
	MailPropInit();
	MailPropAdd(TAG_GET_CLOCK_RATE, MP_CLK_V3D);
	MailPropProcess();
	p = MailPropGet(TAG_GET_CLOCK_RATE);
	if (p != NULL) V3DClock = p[2];

	// get H264 clock
	MailPropInit();
	MailPropAdd(TAG_GET_CLOCK_RATE, MP_CLK_H264);
	MailPropProcess();
	p = MailPropGet(TAG_GET_CLOCK_RATE);
	if (p != NULL) H264Clock = p[2];

	// get ISP clock
	MailPropInit();
	MailPropAdd(TAG_GET_CLOCK_RATE, MP_CLK_ISP);
	MailPropProcess();
	p = MailPropGet(TAG_GET_CLOCK_RATE);
	if (p != NULL) ISPClock = p[2];

	// get SDRAM clock
	MailPropInit();
	MailPropAdd(TAG_GET_CLOCK_RATE, MP_CLK_SDRAM);
	MailPropProcess();
	p = MailPropGet(TAG_GET_CLOCK_RATE);
	if (p != NULL) SDRAMClock = p[2];

	// get MAC address
	MailPropInit();
	MailPropAdd(TAG_GET_BOARD_MAC_ADDRESS);
	MailPropProcess();
	p = MailPropGet(TAG_GET_BOARD_MAC_ADDRESS);
	if (p != NULL)
	{
		const u8* s = (const u8*)&p[1];
		for (i = 0; i < MACADDR_LEN; i++) MACAddress[i] = s[i];
	}

	// get Core voltage
	MailPropInit();
	MailPropAdd(TAG_GET_VOLTAGE, MP_VOLT_CORE);
	MailPropProcess();
	p = MailPropGet(TAG_GET_VOLTAGE);
	if (p != NULL) CoreVoltage = p[2];

	// get SDRAM C voltage
	MailPropInit();
	MailPropAdd(TAG_GET_VOLTAGE, MP_VOLT_SDRAM_C);
	MailPropProcess();
	p = MailPropGet(TAG_GET_VOLTAGE);
	if (p != NULL) SDRAMCVoltage = p[2];

	// get SDRAM P voltage
	MailPropInit();
	MailPropAdd(TAG_GET_VOLTAGE, MP_VOLT_SDRAM_P);
	MailPropProcess();
	p = MailPropGet(TAG_GET_VOLTAGE);
	if (p != NULL) SDRAMPVoltage = p[2];

	// get SDRAM I voltage
	MailPropInit();
	MailPropAdd(TAG_GET_VOLTAGE, MP_VOLT_SDRAM_I);
	MailPropProcess();
	p = MailPropGet(TAG_GET_VOLTAGE);
	if (p != NULL) SDRAMIVoltage = p[2];

	// prepare timings
	SysTickNs = (u32)(((u64)CoreClock << 32)/1000000000);	// number of VideCore clock ticks per 1 ns << 32 (= (CoreClock << 32)/1000000000)
	SysTickUs = (CoreClock+500000)/1000000;	// number of VideCore clock ticks per 1 us
	SysTickMs = (CoreClock+500)/1000; // number of VideCore clock ticks per 1 ms
	NsSysTick = (u32)(((u64)1000000000 << 24)/CoreClock); // number of ns per 1 VideoCore clock tick << 24 (= (1000000000 << 24)/CoreClock)
	UsSysTick = (u32)(((u64)1000000 << 32)/CoreClock); // number of us per 1 VideoCore clock tick << 32 (= (1000000 << 32)/CoreClock)
	MsSysTick = (u32)((((u64)1000 << 32)+CoreClock/2)/CoreClock); // number of ms per 1 VideoCore clock tick << 32 (= (1000 << 32)/CoreClock)
}

// Get current core temperature in thousands of °C (0 = on errror)
u32 GetCoreTemp(void)
{
	MailPropInit();
	MailPropAdd(TAG_GET_TEMPERATURE, 0);
	MailPropProcess();
	u32* p = MailPropGet(TAG_GET_TEMPERATURE);
	u32 temp = 0;
	if (p != NULL) temp = p[2];
	return temp;
}

// System initialize (called from startup.S and startup64.S)
// - starts at time 1.5 seconds after reset, takes 100 ms
void SysInit(void)
{

#if AARCH == 32
#if RASPPI > 1
	// L1 data cache may contain random entries after reset, delete them
	// - takes 80 us
	InvalidateDataCacheL1Only();
#endif // RASPPI > 1

	// Initialize VFP coprocessor
	VfpInit();

#endif // AARCH == 32

	// get system info
	// - takes 300us
	SysInfo();

	// clean caches
	// - takes up to 2.5 ms
	SyncDataAndInstructionCache();

#if MMU_CACHE_MODE != 2  // MMU cache mode: 0=all cached (default), 1=video not cached, 2=no cache
	// Initialize page table
	// - takes up to 3 ms
	PageTableInit();

	// enable MMU
	EnableMMU();
#endif // MMU_CACHE_MODE

	// initialize interrupt system
	IntInit();

#if RASPPI > 1
	// route IRQ anf FIQ to core 0
	//	bit 0,1: GPU IRQ routing to core 0..3
	//	bit 2,3: GPU FIQ routing to core 0..3
	ArmLocal->GPUROUT = 0;

	// temporary disable mailbox interrupts (they will be enabled later, after multicore init)
	ArmLocal->MAILBOXINT[0] = 0;
	ArmLocal->MAILBOXINT[1] = 0;
	ArmLocal->MAILBOXINT[2] = 0;
	ArmLocal->MAILBOXINT[3] = 0;
#endif // RASPPI > 1

	// enable global interrupts IRQ and fast interrupts FIQ
	eif();

#if USE_MALLOC		// 1=use memory allocator, 0=not used (lib_malloc.*)
	// initialize memory allocator
	// - takes 2 ms
	MemInit();
#endif

#if RASPPI > 1
	// Initialize Generic Timer - set clock to 1 MHz and 1 us tick
	GenericTimerInit();
#endif

	// Start free running counter to max. frequency
	FreeTimer_Start(0);

	// initialize System timer to interrupt every SYSTICK_MS ms
	SysTimerInit();

	// initialize hardware random number generator (first RNG_Get() takes 64 ms)
	RNG_Init();

#if USE_RAND		// 1=use Random number generator, 0=not used (lib_rand.*)
	// randomize random number generator
	// - takes 64 ms (due RNG_Get())
	RandInit();
#endif

#if USE_CALENDAR	// 1=use Calendar 32-bit (year range 1970..2099), 0=not used (lib_calendar.*)
	// setup initialize date and time
	sDateTime dt;
	dt.year = SYSTIME_INIT_YEAR;
	dt.mon = SYSTIME_INIT_MONTH;
	dt.day = SYSTIME_INIT_DAY;
	dt.hour = SYSTIME_INIT_HOUR;
	dt.min = 0;
	dt.sec = 0;
	SetUnixTime(DateTimePack(&dt, NULL, NULL), 0);
#endif

#if USE_UART1_STDIO	// 1=use UART1 stdio (printf() or UART1_Print() function; 8 bits, 1 stop bit, no parity)
	GPIO_Func(USE_UART1_GPIO, GPIO_FUNC_AF5);
	UART1_Init(USE_UART1_BAUD);
#endif

#if USE_MULTICORE		// 1=use multicore (for applications), 0=do not use other cores (for loader)
#if RASPPI > 1
	// Multicore initialize - start cores 1..3
	// - takes up to 3 ms
	MultiInit();
#endif
#endif

	// Call constructors of static objects (.init_array)
	extern void (*__init_start) (void);
	extern void (*__init_end) (void);
	for (void (**pFunc) (void) = &__init_start; pFunc < &__init_end; pFunc++)
	{
		(**pFunc) ();
	}

#if USE_PWMSND		// 1=use PWM sound output, 0=not used (lib_pwmsnd.*)
	// Initialite PWM sound output
	// - takes 100 us
	PWMSndInit();
#endif

	// SD Host (SD Card) initialize
	// - takes 20 ms
	SD_Init();

#if WIDTH	// draw width (0=do not initialize default videomode)
	// initialize default videomode
	// - takes up to 3.5 ms
	DispInit();
#endif

	// initialize device
	DevInit();

	// start main code
extern int main(void);
	main();

	// reboot system
	Reboot();

/*
	// stop system
	while (True)
	{
#if RASPPI > 1
		// data synchronization barrier
		dsb();

		// wait for interrupt
		wfi();
#endif
	}
*/
}

#if USE_MULTICORE		// 1=use multicore (for applications), 0=do not use other cores (for loader)
#if RASPPI > 1

// Core system initialize
void SysInitSecondary(void)
{
#if AARCH == 32
	// L1 data cache may contain random entries after reset, delete them
	InvalidateDataCacheL1Only();

	// Initialize VFP coprocessor
	VfpInit();
#endif // AARCH == 32

	// clean caches
	SyncDataAndInstructionCache();

#if MMU_CACHE_MODE != 2  // MMU cache mode: 0=all cached (default), 1=video not cached, 2=no cache
	// enable MMU
	EnableMMU();
#endif // MMU_CACHE_MODE

	// confirmation of kernel startup
	int core = CpuID();		// get current core ID
#if AARCH==32
	LocalMailboxClr(core, 3, ~0);	// clear bits in messagebox 3 of this core
#else // AARCH==32
	*SPINCORE_PTR(core) = 0;
	dsb();
#endif

#if USE_RAND		// 1=use Random number generator, 0=not used (lib_rand.*)
	// randomise
	RandSeed[core] += core*12345;
#endif

	// start main code - entry secondary core
	MainSecondary();

	// stop core
	while (True)
	{
		// data synchronization barrier
		dsb();

		// wait for interrupt
		wfi();
	}
}
#endif // RASPPI > 1
#endif // USE_MULTICORE

#if !USE_MULTICORE		// 1=use multicore (for applications), 0=do not use other cores (for loader)
// system terminate (cannot be run with multicore, used only by boot loader)
void SysTerm(void)
{
	// terminate device
	DevTerm();

#if USE_PWMSND		// 1=use PWM sound output, 0=not used (lib_pwmsnd.*)
	// Terminate PWM sound output
	PWMSndTerm();
#endif

#if USE_UART1_STDIO	// 1=use UART1 stdio (printf() or UART1_Print() function; 8 bits, 1 stop bit, no parity)
	UART1_Term();
	GPIO_Func(USE_UART1_GPIO, GPIO_FUNC_IN);
#endif

	// terminate hardware random number generator
	RNG_Term();

	// terminate System timer
	SysTimerTerm();

	// Stop free running counter
	FreeTimer_Stop();

	// disable global interrupts IRQ and fast interrupts FIQ
	dif();

#if RASPPI > 1
	// route IRQ anf FIQ to core 0
	//	bit 0,1: GPU IRQ routing to core 0..3
	//	bit 2,3: GPU FIQ routing to core 0..3
	ArmLocal->GPUROUT = 0;

	// disable mailbox interrupts
	ArmLocal->MAILBOXINT[0] = 0;
	ArmLocal->MAILBOXINT[1] = 0;
	ArmLocal->MAILBOXINT[2] = 0;
	ArmLocal->MAILBOXINT[3] = 0;
#endif // RASPPI > 1

	// terminate interrupt system
	IntTerm();

	// clean data cache
	CleanDataCache();

	// invalidate data cache
	InvalidateDataCache();

	// disable MMU
	DisableMMU();

#if AARCH == 32

// @TODO: This crashes, but it is not needed to use !!!!!!!!!

	// terminate VFP coprocessor
//	VfpTerm();
#endif // AARCH == 32
}
#endif // !USE_MULTICORE
