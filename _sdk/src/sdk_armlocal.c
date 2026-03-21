
// ****************************************************************************
//
//                               ARM Local
//
// ****************************************************************************

#include "../../includes.h"	// includes

#if RASPPI > 1

// Initialize Generic Timer - set clock to 1 MHz and 1 us tick (this function is called when the system starts up)
void GenericTimerInit(void)
{
	ArmLocal_t* a = ArmLocal;

	// use crystal clock
	// 0x00: control register of 64-bit core timer "Generic Timer" (selects timer source for all 4 cores)
	//	bit 8:	0 = 64-bit core timer runs from the crystal clock (19.2 or 54 MHz)
	//		1 = 64-bit core timer runs from the APB clock
	//			APB clock is running at half speed of ARM clock
	//	bit 9:	0 = 64-bit core timer increments by 2
	//		1 = 64-bit core timer increments by 1
	a->TIMERCTRL = B9;

	// setup prescaler to get 1 MHz
#if RASPPI >= 4
	// To get divider ratio of 54 use: 2^31/54 = 39768216 = 0x025ED098
	a->TIMERPRESC = 39768216;	// crystal 54 MHz
#else
	// To get divider ratio of 19.2 use: 2^31/19.2 = 111848107 = 0x06AAAAAB (with error 8.9e-9)
	a->TIMERPRESC = 111848107;	// crystal 19.2 MHz
#endif

	// synchronize timers "System Timer" and "Generic Timer"
	IRQ_LOCK;	// lock IRQ
	u32 high;
	u32 high2 = SysTimer->CHI;  // get raw time HIGH
	do {
		// accept new time HIGH
		high = high2;

		// get raw time LOW
		cb();
		a->TIMERLS = SysTimer->CLO;

		// get raw time HIGH again
		cb();
		high2 = SysTimer->CHI;
		a->TIMERMS = high2;

	// check that HIGH has not changed, otherwise a re-read will be necessary
	} while (high != high2);
	IRQ_UNLOCK;	// unlock IRQ
}

#endif // RASPPI > 1
