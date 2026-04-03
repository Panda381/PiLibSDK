
// ****************************************************************************
//
//                       PWM - Pulse Width Modulator
//
// ****************************************************************************

#include "../../includes.h"	// includes

// start PWM output, 2 channels
//  freq ... Required frequency in [Hz] - GPIOCLOCK_SRC_PLLD
//		clock source with integer division will be used
//  period ... PWM period in number of clocks
//  msmode ... use simple M/S mode (or use spread out N/M mode otherwise - recommended for audio)
//  invert ... invert output
void PWM_Start(u32 freq, int period, Bool msmode, Bool invert)
{
	// stop PWM output
	PWM_Stop();

	// start GPIO clock source
	GpioClockStartHz(GPIOCLOCK_PWM, GPIOCLOCK_SRC_PLLD, freq, GPIOCLOCK_MASH_INT);

	// setup period
	PWM_t* pwm = PWM;
	pwm->RNG1 = period;
	pwm->RNG2 = period;

	// setup control word
	u32 ctl = B0 | B8;	// enable channel 1 and channel 2
	if (msmode) ctl |= B7 | B15; // use M/S simple transmission mode
	if (invert) ctl |= B4 | B12; // invert output
	pwm->CTL = ctl;
}

// stop PWM output
void PWM_Stop(void)
{
	// stop PWM clock
	GpioClockStop(GPIOCLOCK_PWM);

	// short delay to stabilize clock manager
	WaitUs(50);

	// stop PWM
	PWM_t* pwm = PWM;
	pwm->CTL = 0;	// disable PWM

	// short delay to stabilize PWM
	WaitUs(100);
}
