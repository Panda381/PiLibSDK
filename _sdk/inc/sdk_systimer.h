
// ****************************************************************************
//
//                                 System Timer
//
// ****************************************************************************
// Four 32-bit timer channels on 64-bit free running counter with 1 us ticks.
// Only channels 1 and 3 are available for CPU. Channels 0 and 2 are reserved for GPU.

// Note - The values of the "System Timer" and "Generic Timer" counters are
// synchronized at system startup so that their time value is equivalent.
// However, values from timers should not be confused with each other.
// After a long period of time, the time value may drift and differ.

#ifndef _SDK_SYSTIMER_H
#define _SDK_SYSTIMER_H

// increment of system time in [ms] on SysTick interrupt (0 = do not use SysTick interrupt)
#ifndef SYSTICK_MS
#define SYSTICK_MS	5
#endif

// SysTick period in [us] (number of 1 us ticks to interrupt every SYSTICK_MS ms)
#define SYSTICK_US	(SYSTICK_MS*1000)

// SysTick channel
#define SYSTICK_CHAN	3

// system time counter in [ms], counts time from system start - incremented every SYSTICK_MS ms
// with overflow after 49 days (use difference, not absolute value!)
extern volatile u32 SysTime;

// current date and time (incremented every SYSTICK_MS ms from CPU core 0)
extern volatile u32 UnixTime;		// current date and time in Unix format
					// - number of seconds since 1/1/1970 (thursday) up to 12/31/2099
extern volatile int CurTimeMs;		// current time in [ms] 0..999
extern volatile u32 NextTimeUs;		// time LOW of next tick
extern volatile u32 SysTimeLock;	// odd number - lock to access to time variables

// System timer
typedef struct
{
	io32	CS;		// 0x00: Control/status
				//  bit 0: 1=Timer 0 match detected (write '1' to clear) ... this channel is reserved for GPU
				//  bit 1: 1=Timer 1 match detected (write '1' to clear) ... used as fast alarm
				//  bit 2: 1=Timer 2 match detected (write '1' to clear) ... this channel is reserved for GPU
				//  bit 3: 1=Timer 3 match detected (write '1' to clear) ... used as SysTick
				//  bit 4..31: reserved (write as '0')

	io32	CLO;		// 0x04: (read only) counter low
	io32	CHI;		// 0x08: (read only) counter high

	union {
		io32	C[4]; // 0x0C: Compare registers of channels 0..3
		struct {
			io32	C0;	// 0x0C: Compare register of channel 0 ... this channel is reserved for GPU
			io32	C1;	// 0x10: Compare register of channel 1 ... used as fast alarm
			io32	C2;	// 0x14: Compare register of channel 2 ... this channel is reserved for GPU
			io32	C3;	// 0x18: Compare register of channel 3 ... used as SysTick
		};
	};

} SysTimer_t;
STATIC_ASSERT(sizeof(SysTimer_t) == 0x1C, "Incorrect SysTimer_t!");

// System timer interface
#define SysTimer	((SysTimer_t*)ARM_SYSTIMER_BASE)

// get 32-bit absolute system time LOW in [us]
//  Note - The values of the "System Timer" and "Generic Timer" counters are
//  synchronized at system startup so that their time value is equivalent.
//  However, values from timers should not be confused with each other.
//  After a long period of time, the time value may drift and differ.
INLINE u32 SysTime32(void) { return SysTimer->CLO; }
#if RASPPI == 1 // on Pi 2 and later use Time() from Generic Timer
INLINE u32 Time(void) { return SysTimer->CLO; }
#endif

// get 16-bit absolute system time in [ms] - not accurate, time in [us] is divided by 1024
INLINE u16 TimeMs(void) { return (u16)(Time()>>10); }

// get 64-bit absolute system time in [us]
//  Note - The values of the "System Timer" and "Generic Timer" counters are
//  synchronized at system startup so that their time value is equivalent.
//  However, values from timers should not be confused with each other.
//  After a long period of time, the time value may drift and differ.
u64 SysTime64(void);
#if RASPPI == 1 // on Pi 2 and later use Time64() from Generic Timer
INLINE u64 Time64(void) { return SysTime64(); }
#endif

// wait for delay in [us] (max. 71 minutes)
void WaitUs(u32 us);

// wait for delay in [ms] (max. 71 minutes)
void WaitMs(int ms);

// initialize System timer to interrupt every SYSTICK_MS ms ... this is called from SysInit()
void SysTimerInit();

// terminate System timer
void SysTimerTerm();

// get current date and time in Unix format with [ms] and [us] (NULL = entries not required)
u32 GetUnixTime(s16* ms, s16* us);

// set current date and time in Unix format with [ms] (0..999)
// - Should be called from CPU core 0.
void SetUnixTime(u32 time, s16 ms);

// Using alarm:
//	(- Set interrupt handler IntHandler(IRQ_TIMER1, handler))
//	- Start alarm interval with FastAlarmStart()
//	- Check alarm event with AlarmCheck() (or remaining time AlarmRemain())
//		(or enable interrupt IntEnable(IRQ_TIMER1))
//	- After alarm, reinit new alarm event with FastAlarmShift()
// Alarm interrupt cen be raised with IntHandler(IRQ_TIMER1, handler) and IntEnable(IRQ_TIMER1).

// start fast alarm (time = time interval in us, max. 71 minutes)
//  To enable interrupt, use IntHandler(IRQ_TIMER1, handler) and IntEnable(IRQ_TIMER1)
void FastAlarmStart(u32 us);

// shift fast alarm (time = delta time shift in us, max. 71 minutes)
void FastAlarmShift(u32 us);

// get fast alarm remaining time in us
//  Note: Returns interval -30..+30 minutes.
s32 FastAlarmRemain(void);

// check if fast alarm time has been elapsed (returns True if elapsed)
Bool FastAlarmCheck();

#endif // _SDK_SYSTIMER_H
