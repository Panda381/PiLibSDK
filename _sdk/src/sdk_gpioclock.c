
// ****************************************************************************
//
//                            GPIO Clock Manager
//
// ****************************************************************************

#include "../../includes.h"	// includes

// minimal divisor
const u32 GpioClockMinDiv[4] = { 0x1000, 0x2000, 0x3000, 0x5000 };

// get frequency in [Hz] of clock source (returns 0 if source is not valid)
//  src ... GPIO clock source GPIOCLOCK_SRC_*
u32 GpioClockSrcFreq(int src)
{
#if RARSPPI <= 3
	if (src == GPIOCLOCK_SRC_OSC)  return  19200000;
	if (src == GPIOCLOCK_SRC_PLLD) return 500000000;
#else
	if (src == GPIOCLOCK_SRC_OSC)  return 54000000;
	if (src == GPIOCLOCK_SRC_PLLD) return 750000000;
#endif
	return 0;
}

// Start GPIO clock
//  inx ... GPIO clock index GPIOCLOCK_*
//  src ... GPIO clock source GPIOCLOCK_SRC_*
//  div ... divisor * 4096
//  mash ... noise-shaping divider GPIOCLOCK_MASH_*
// Minimal divisor mash 0: 0x1000, mash 1: 0x2000, mash 2: 0x3000, mash 3: 0x5000
// When using the GPIOCLOCK output on the GPIO pin, it is
// necessary to activate the corresponding alternative function.
void GpioClockStart(int inx, int src, u32 div, int mash)
{
	// minimal and maximal divisor
	u32 mindiv = GpioClockMinDiv[mash];
	if (div < mindiv ) div = mindiv;
	if (div > 0xFFFFFFul) div = 0xFFFFFFul;

	// get GPIO clock manager
	GpioClock_t* g = GpioClock(inx);

	// Stop GPIO clock
	GpioClockStop(inx);

	// set divider
	g->DIV = div | GPIOCLOCK_PASSWD;

	// short delay
	WaitUs(10);

	// set control register
	u32 ctl = src | (mash << 9) | GPIOCLOCK_PASSWD;
	g->CTL = ctl;

	// short delay
	WaitUs(10);

	// enable clock
	g->CTL = ctl | B4;

	// short delay to start-up
	WaitUs(20);
}

// Start GPIO clock with frequency in [Hz]
//  inx ... GPIO clock index GPIOCLOCK_*
//  src ... GPIO clock source GPIOCLOCK_SRC_*
//  freq ... required frequency in [Hz]
//  mash ... noise-shaping divider GPIOCLOCK_MASH_*
// When using the GPIOCLOCK output on the GPIO pin, it is
// necessary to activate the corresponding alternative function.
void GpioClockStartHz(int inx, int src, u32 freq, int mash)
{
	// Stop GPIO clock
	GpioClockStop(inx);

	// get frequency of the source
	u32 srcfreq = GpioClockSrcFreq(src);
	if (srcfreq == 0) return;

	// get divider
	u32 div = (u32)(((u64)srcfreq*4096 + freq/2) / freq);

	// start clock
	GpioClockStart(inx, src, div, mash);
}

// Stop GPIO clock
//  inx ... GPIO clock index GPIOCLOCK_*
void GpioClockStop(int inx)
{
	// get GPIO clock manager
	GpioClock_t* g = GpioClock(inx);

	// stop clock (B5 = KILL flag)
	g->CTL = B5 | GPIOCLOCK_PASSWD;

	// wait for ready
	while ((g->CTL & B7) != 0) {}

	// short delay to stabilize clock manager
	WaitUs(50);
}

// Get frequency in [Hz] of GPIO clock (returns 0 if clock is not running or clock source is unknown)
//  inx ... GPIO clock index GPIOCLOCK_*
u32 GpioClockGetHz(int inx)
{
	// get GPIO clock manager
	GpioClock_t* g = GpioClock(inx);

	// check if clock is running
	u32 ctl = g->CTL;
	if ((ctl & B7) == 0) return 0;

	// get frequency of the source
	int src = ctl & 0x0f;
	u32 srcfreq = GpioClockSrcFreq(src);
	if (srcfreq == 0) return 0;

	// get MASH
	int mash = (ctl >> 9) & 3;

	// get divider
	u32 div = g->DIV & 0xFFFFFF;
	if (mash == 0) div &= 0xFFF000;
	u32 mindiv = GpioClockMinDiv[mash];
	if (div < mindiv ) div = mindiv;

	// get frequency
	u32 freq = (u32)(((u64)srcfreq*4096 + div/2) / div);
	return freq;
}
