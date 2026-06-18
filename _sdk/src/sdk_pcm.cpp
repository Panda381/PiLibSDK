
// ****************************************************************************
//
//                             PCM I2S audio
//
// ****************************************************************************

#include "../../includes.h"	// includes

// start PCM clock in [Hz] (using 19.2 MHz base clock)
void PCM_ClockStart(u32 freq)
{
	GpioClockStartHz(GPIOCLOCK_PCM, GPIOCLOCK_SRC_OSC, freq, GPIOCLOCK_MASH_INT);
}

// stop PCM clock
void PCM_ClockStop()
{
	GpioClockStop(GPIOCLOCK_PCM);
}
