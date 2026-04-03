
// ****************************************************************************
//
//                                 ARM Timer
//
// ****************************************************************************
// ARM Timer: One decremented 32-bit counter with pre-scaler, pre-divider and interval value, interrupted on 0.
//   - can be used as precise interrupt timer
// Free Running Counter: One incremented 32-bit counter with pre-scaler, no pre-divider, no interval value, no interrupt.
//   - can be used as ns timer

// Note - ARM Timer timing is derived from the VideoCore system clock, which may change during operation.
// To ensure a stable frequency, use parameters "force_turbo=1" and "core_freq=..." in config.txt.

#ifndef _SDK_ARMTIMER_H
#define _SDK_ARMTIMER_H

// ARM timer pre-scaler
#define ARMTIMER_PRESC_1	0	// f/1
#define ARMTIMER_PRESC_16	1	// f/16
#define ARMTIMER_PRESC_256	2	// f/256

// ARM timer
typedef struct
{
	io32	LOAD;		// 0x00: Load register
				//	This value is loaded into the timer value register
				//	after the load register has been written or if the
				//	timer-value register has counted down to 0.

	io32	VALUE;		// 0x04: (read-only) Counter current value - counting down, reloaded from LOAD on 0

	io32	CTRL;		// 0x08: Time control register
				//  bit 1: select 0 = 16-bit counter, 1 = 32 bit counter
				//  bit 2..3: pre-scaler 0=f/1, 1=f/16, 2=f/256, 3=not used (=f/1)
				//  bit 5: 0 = interrupt disabled, 1 = interrupt enabled (default enabled)
				//  bit 7: 0 = timer disabled, 1 = timer enabled
				//  bit 8: 0 = keeps running in debug halt, 1 = halt in debug halt
				//  bit 9: 0 = free running counter disabled, 1 = free running counter enabled
				//  bit 16..23: (8 bits) free running counter pre-scaler, freq = sysclk / (prescale+1)
				//		Reset value is 0x3E, freq = sysclk / 63
				//		System clock is SYSFREQ.

	io32	IRQCLR;		// 0x0C: (write only) IRQ clear
				//  Writing "1" to this register clears interrupt pending bit.
				//  Reading from this register returns 0x544D5241 = "ARMT"

	io32	RAWIRQ;		// 0x10: (read only) Raw IRQ
				//  bit 0: 1=interrupt pending bis is set (counter counted down to zero)
				//	To generate interrupt, interrupt enable bit must be set.

	io32	MSKIRQ;		// 0x14: (read only) Masked IRQ
				//  bit 0: 1=interrupt line is asserted (interrupt pending
				//	  and interrupt enable bits are set)

	io32	RELOAD;		// 0x18: Timer reload value
				//	This register is a copy of the timer LOAD register.
				//	The difference is that a write to this register does not
				//	trigger an immediate reload of the timer value register.
				//	This register value is only accessed if the value register
				//	has finished counting down to zero.

	io32	PREDIV;		// 0x1C: Timer pre-divider
				//  bit 0..9: 10-bit pre-divider value
				//	timer_clock = apb_clock / (pre_divider + 1)
				//	Default value after reset = 0x7D (= divide by 126).

	io32	FREECNT;	// 0x20: (read only) 32_bit free running counter, counting up.

} ARMTimer_t;
STATIC_ASSERT(sizeof(ARMTimer_t) == 0x24, "Incorrect ARMTimer_t!");

// ARM timer interface
#define ARMTimer	((ARMTimer_t*)ARM_ARMTIMER_BASE)

// start ARM timer
//   presc = pre-scaler ARM_PRESC_* (0=f/1, 1=f/16, 2=f/256)
//   prediv = pre-divider 0..1023 (division = prediv + 1)
//   load = timer interval, reload value
//   en = True to enable interrupt
// Timer tick frequency = CoreClock / (presc * (prediv + 1)).
// Timer IRQ interval = CoreClock / (presc * (prediv + 1) * (load + 1))
// Counter starts counting from load value.
// To use interrupt, set handler with IntHandler(IRQ_ARMTIMER, handler)
void ARMTimer_Start(int presc, int prediv, u32 load, Bool en);

// Stop ARM timer (and disable interrupt)
void ARMTimer_Stop();

// check ARM Timer IRQ pending bit
INLINE Bool ARMTimer_Pending() { return (ARMTimer->RAWIRQ & B0) != 0; }

// clear ARM Timer IRQ pending bit
INLINE void ARMTimer_Clr() { ARMTimer->IRQCLR = 0; }

// update ARM timer interval (reload counter immediate to load value)
INLINE void ARMTimer_Load(u32 load) { ARMTimer->LOAD = load; }

// update ARM timer interval (does not reload counter immediate)
INLINE void ARMTimer_Reload(u32 reload) { ARMTimer->RELOAD = reload; }

// get current value of ARM timer counter (get remaining time)
INLINE u32 ARMTimer_Val() { return ARMTimer->VALUE; }

// Start free running counter ... initialized in SysInit() to presc = 0
//   presc = pre-scaler 0..255
// Timer tick frequency = CoreClock / (presc + 1)
void FreeTimer_Start(u8 presc);

// Stop free running counter ... called from SysTerm()
INLINE void FreeTimer_Stop() { ARMTimer->CTRL &= ~B9; }

// get free running counter
INLINE u32 FreeTimer() { return ARMTimer->FREECNT; }

// wait system clocks using free running counter (max. 4 seconds)
void FreeTimer_WaitClk(u32 clk);

// wait nanoseconds using free running counter (max. 4 seconds)
void FreeTimer_WaitNs(u32 ns);

// wait microseconds using free running counter (max. 4 seconds)
void FreeTimer_WaitUs(u32 us);

// wait miliseconds using free running counter (max. 4 seconds)
void FreeTimer_WaitMs(u32 ms);

#endif // _SDK_ARMTIMER_H
