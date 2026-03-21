
// ****************************************************************************
//
//                          System initialize
//
// ****************************************************************************

#include "../../includes.h"	// includes

// system info
u32 FirmwareVersion = 0;	// firmware version (= build time in Unix format)
u32 BoardModel = 0;		// board model
u32 BoardRevision = 0;		// board revision
u64 BoardSerial = 0;		// board serial
u32 ARMMemAddr = 0x00000000;	// ARM memory address
u32 ARMMemSize = 0x1C000000;	// ARM memory size
u32 VCMemAddr = 0x1C000000;	// VideoCore memory address
u32 VCMemSize = 0x04000000;	// VideoCore memory size
u32 ARMClock = 1000000000;	// ARM clock in [Hz]
u32 CoreClock = 250000000;	// VideoCore clock in [Hz]
u32 EMMCClock = 250000000;	// EMMC clock
u32 UARTClock = 250000000;	// UART clock

u32 SysTickNs = 1073741824;	// number of VideCore clock ticks per 1 ns << 32 (= (CoreClock << 32)/1000000000)
u32 SysTickUs = 250;		// number of VideCore clock ticks per 1 us
u32 SysTickMs = 250000;		// number of VideCore clock ticks per 1 ms
u32 NsSysTick = 67108864;	// number of ns per 1 VideoCore clock tick << 24 (= (1000000000 << 24)/CoreClock)
u32 UsSysTick = 17179869;	// number of us per 1 VideoCore clock tick << 32 (= (1000000 << 32)/CoreClock)
u32 MsSysTick = 17180;		// number of ms per 1 VideoCore clock tick << 32 (= (1000 << 32)/CoreClock)

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

// get system info
void SysInfo(void)
{
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

	// prepare timings
	SysTickNs = (u32)(((u64)CoreClock << 32)/1000000000);	// number of VideCore clock ticks per 1 ns << 32 (= (CoreClock << 32)/1000000000)
	SysTickUs = (CoreClock+500000)/1000000;	// number of VideCore clock ticks per 1 us
	SysTickMs = (CoreClock+500)/1000; // number of VideCore clock ticks per 1 ms
	NsSysTick = (u32)(((u64)1000000000 << 24)/CoreClock); // number of ns per 1 VideoCore clock tick << 24 (= (1000000000 << 24)/CoreClock)
	UsSysTick = (u32)(((u64)1000000 << 32)/CoreClock); // number of us per 1 VideoCore clock tick << 32 (= (1000000 << 32)/CoreClock)
	MsSysTick = (u32)((((u64)1000 << 32)+CoreClock/2)/CoreClock); // number of ms per 1 VideoCore clock tick << 32 (= (1000 << 32)/CoreClock)
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
	// - takes 200us
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
