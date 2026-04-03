
// ****************************************************************************
//
//                                 ARM Timer
//
// ****************************************************************************

#include "../../includes.h"	// includes

// start ARM timer
//   presc = pre-scaler ARM_PRESC_* (0=f/1, 1=f/16, 2=f/256)
//   prediv = pre-divider 0..1023 (division = prediv + 1)
//   load = timer interval, reload value
//   en = True to enable interrupt
// Timer tick frequency = CoreClock / (presc * (prediv + 1)).
// Timer IRQ interval = CoreClock / (presc * (prediv + 1) * (load + 1))
// Counter starts counting from load value.
// To use interrupt, set handler with IntHandler(IRQ_ARMTIMER, handler)
void ARMTimer_Start(int presc, int prediv, u32 load, Bool en)
{
	// load ARM timer control register
	u32 ctrl = ARMTimer->CTRL;

	// clear all bits, except free running counter
	ctrl &= 0x00FF0000 | B9;

	// set 32-bit counter mode
	ctrl |= B1;

	// set pre-scaler
	ctrl |= presc << 2;

	// stop counter
	ARMTimer->CTRL = ctrl;

	// set pre-divider
	ARMTimer->PREDIV = prediv;

	// set timer interval, reload max. value into counter
	ARMTimer->LOAD = load;

	// clear pending flag
	ARMTimer->IRQCLR = 0;

	// timer enable
	ctrl |= B7;

	// enable interrupt
	if (en) ctrl |= B5;

	// save control register and start counter
	ARMTimer->CTRL = ctrl;

	// enable interrupt
	if (en) IntEnable(IRQ_ARMTIMER);
}

// Stop ARM timer (and disable interrupt)
void ARMTimer_Stop()
{
	// disable interrupt
	IntDisable(IRQ_ARMTIMER);

	// load ARM timer control register
	u32 ctrl = ARMTimer->CTRL;

	// interrupt disable, timer disable
	ctrl &= ~(B5 + B7);

	// save control register
	ARMTimer->CTRL = ctrl;
	
	// clear pending flag
	ARMTimer->IRQCLR = 0;
}

// Start free running counter ... initialized in SysInit() to presc = 0
//   presc = pre-scaler 0..255
// Timer tick frequency = CoreClock / (presc + 1)
void FreeTimer_Start(u8 presc)
{
	// load ARM timer control register
	u32 ctrl = ARMTimer->CTRL;

	// set new value of prescaler
	ctrl &= ~(0xff << 16);
	ctrl |= (u32)presc << 16;

	// enable free running counter
	ctrl |= B9;

	// save control register and start free running counter
	ARMTimer->CTRL = ctrl;
}

// wait system clocks using free running counter (max. 4 seconds)
void FreeTimer_WaitClk(u32 clk)
{
	u32 old = FreeTimer();
	while ((u32)(FreeTimer() - old) < clk) {}
}

// wait nanoseconds using free running counter (max. 4 seconds)
void FreeTimer_WaitNs(u32 ns)
{
	FreeTimer_WaitClk((u32)(((u64)ns*SysTickNs) >> 32));
}

// wait microseconds using free running counter (max. 4 seconds)
void FreeTimer_WaitUs(u32 us)
{
	FreeTimer_WaitClk(us*SysTickUs);
}

// wait miliseconds using free running counter (max. 4 seconds)
void FreeTimer_WaitMs(u32 ms)
{
	FreeTimer_WaitClk(ms*SysTickMs);
}
