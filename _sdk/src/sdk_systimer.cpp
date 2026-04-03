
// ****************************************************************************
//
//                                 System Timer
//
// ****************************************************************************

#include "../../includes.h"	// includes

// system time counter in [ms], counts time from system start - incremented every SYSTICK_MS ms
// with overflow after 49 days (use difference, not absolute value!)
volatile u32 SysTime = 0;

// current date and time (incremented every SYSTICK_MS ms from CPU core 0)
volatile u32 UnixTime = 0;		// current date and time in Unix format
					// - number of seconds since 1/1/1970 (thursday) up to 12/31/2099
volatile int CurTimeMs = 0;		// current time in [ms] 0..999
volatile u32 NextTimeUs = 0;		// time LOW of next tick
volatile u32 SysTimeLock = 0;		// odd number - lock to access to time variables

// get 64-bit absolute system time in [us]
u64 SysTime64(void)
{
	u32 low, high;
	u32 high2 = SysTimer->CHI;  // get raw time HIGH

	do {
		// accept new time HIGH
		high = high2;

		// get raw time LOW
		cb();
		low = SysTimer->CLO;

		// get raw time HIGH again
		cb();
		high2 = SysTimer->CHI;

	// check that HIGH has not changed, otherwise a re-read will be necessary
	} while (high != high2);

	// return result value
	return ((u64)high << 32) | low;
}

// wait for delay in [us] (max. 71 minutes)
void WaitUs(u32 us)
{
	u32 start = Time();
	while ((u32)(Time() - start) < us) {}
}

// wait for delay in [ms] (max. 71 minutes)
void WaitMs(int ms)
{
	WaitUs((u32)ms*1000);
}

// SysTick handler (must be called only from core 0)
void SysTick_Handler()
{
	// get current time LOW in [us]
	u32 now = SysTime32();

	// get time of last time pulse
	u32 last = SysTimer->C3;

	// get number of ticks to next pulse, with 5us reserve
	u32 ticks = (u32)(now - last + SYSTICK_US + 5)/SYSTICK_US;

	// set time of next interrupt
	u32 next = last + ticks*SYSTICK_US;
	SysTimer->C3 = next;

	// clear status flag
	SysTimer->CS = B3;

	// get time increment in [ms]
	u32 ms = ticks*SYSTICK_MS;

	// shift system time
	u32 systime = SysTime + ms;

	// shift Unix time
	ms += CurTimeMs;
	u32 sec = ms/1000;
	ms -= sec*1000;

	// save new state
	SysTimeLock++;		// odd number - lock values
	dmb();

	SysTime = systime;
	UnixTime += sec;
	CurTimeMs = ms;
	NextTimeUs = next;

	dmb();
	SysTimeLock++;		// even number - unlock values

#if SYSTICK_KEYSCAN	// call KeyScan() function from SysTick system timer
	KeyScan();
#endif
}

// initialize System timer to interrupt every SYSTICK_MS ms ... this is called from SysInit()
void SysTimerInit()
{
	// disable interrupts
	IRQFIQ_LOCK;
	SysTimeLock = 1;		// odd number - lock values
	dmb();

	// set compare register
	u32 next = SysTime32() + SYSTICK_US;
	SysTimer->C3 = next;
	NextTimeUs = next;

	// clear status flag
	SysTimer->CS = B3;

	// enable interrupts
	dmb();
	SysTimeLock = 2;
	IRQFIQ_UNLOCK;

#if SYSTICK_MS
	// register handler
	IntHandler(IRQ_TIMER3, SysTick_Handler);

	// enable interrupt
	IntEnable(IRQ_TIMER3);
#endif // SYSTICK_MS
}

// terminate System timer
void SysTimerTerm()
{
	// disable interrup
	IntDisable(IRQ_TIMER3);

	// clear status flag
	SysTimer->CS = B3;
}

// get current date and time in Unix format with [ms] and [us] (NULL = entries not required)
u32 GetUnixTime(s16* ms, s16* us)
{
	u32 sec, next, seq1, seq2;
	int msec, usec;

	// inter-core safe reading
	for (;;)
	{
		// check lock
		seq1 = SysTimeLock;
		if ((seq1 & 1) != 0) continue; // values locked

		// get current state
		dmb();
		sec = UnixTime;		// Unix time in [sec]
		msec = CurTimeMs;	// time in [ms]
		next = NextTimeUs;	// time of next tick in [us]
		dmb();

		// check lock
		seq2 = SysTimeLock;
		if ((seq1 == seq2) && ((seq2 & 1) == 0)) break;
	}

	// get time
	usec = (int)(SysTime32() - next);

	// normalize microseconds
	while (usec < 0) { usec += 1000; msec--; }
	while (usec >= 1000) { usec -= 1000; msec++; }

	// normalize milliseconds
	while (msec < 0) { msec += 1000; sec--; }
	while (msec >= 1000) { msec -= 1000; sec++; }

	// save current [us]
	if (us != NULL) *us = usec;
	
	// save current [ms]
	if (ms != NULL) *ms = msec;

	return sec;
}

// set current date and time in Unix format with [ms] (0..999)
// - Should be called from CPU core 0.
void SetUnixTime(u32 time, s16 ms)
{
	// shift to time of next interrupt
	ms += SYSTICK_MS;
	while (ms >= 1000)
	{
		ms -= 1000;
		time++;
	}

	// save new state
	IRQ_LOCK;
	SysTimeLock++;		// odd number - lock values
	dmb();

	UnixTime = time;
	CurTimeMs = ms;
	u32 next = SysTime32() + SYSTICK_US;
	SysTimer->C3 = next;
	NextTimeUs = next;

	dmb();
	SysTimeLock++;		// even number - unlock values
	IRQ_UNLOCK;
}

// start fast alarm (time = time interval in us, max. 71 minutes)
//  To enable interrupt, use IntEn(IRQ_SYSTIMER1)
void FastAlarmStart(u32 us)
{
	// set compare register
	SysTimer->C1 = SysTime32() + us;

	// clear status flag
	SysTimer->CS = B1;
}

// shift fast alarm (time = delta time shift in us, max. 71 minutes)
void FastAlarmShift(u32 us)
{
	// shift compare register
	SysTimer->C1 += us;

	// clear status flag
	SysTimer->CS = B1;
}

// get fast alarm remaining time in us
//  Note: Returns interval -30..+30 minutes.
s32 FastAlarmRemain(void)
{
	return (s32)(SysTimer->C1 - SysTime32());
}

// check if fast alarm time has been elapsed (returns True if elapsed)
Bool FastAlarmCheck()
{
	return (SysTimer->CS & B1) != 0;
}
