
// ****************************************************************************
//
//                       GPIO - General Purpose I/O
//
// ****************************************************************************

#include "../../includes.h"	// includes

// set GPIO function GPIO_FUNC_*
//  - This function is not thread-safe, but it is IRQ/FIQ interrupt save
void GPIO_Func(int pin, int func)
{
	// split pin to register and bit index
	u8 reg = pin/10;
	u8 bit = (pin - reg*10)*3;

	// IRQ and FIQ lock
	IRQFIQ_LOCK;

	// load register
	volatile u32* r = &GPIO->GPFSEL[reg];
	u32 k = *r;

	// update function
	k &= ~(GPIO_FUNC_MASK << bit);
	k |= func << bit;

	// set register
	*r = k;

	// IRQ and FIQ unlock
	IRQFIQ_UNLOCK;
}

// set GPIO pull-up GPIO_PULL_*
//  - This function is not thread-safe
void GPIO_Pull(int pin, int pull)
{
#if RASPPI <= 3
	
	// set required pull mode
	GPIO->GPPUD = pull;

	// wait 150 gpio cycles - it is about 0.7us at system clock 200 or 250 MHz
	FreeTimer_WaitClk(150);

	// prepare pointer to clock register
	io32* g;
	u32 m;
	if (pin < 32)
	{
		g = &GPIO->GPPUDCLK0;
		m = 1 << pin;
	}
	else
	{
		g = &GPIO->GPPUDCLK1;
		m = 1 << (pin-32);
	}

	// turn on required pin clock
	*g = m;

	// wait 150 gpio cycles - it is about 0.7us at system clock 200 or 250 MHz
	FreeTimer_WaitClk(150);

	// clear pull mode
	GPIO->GPPUD = GPIO_PULL_OFF;

	// turn off pin clock
	*g = 0;

#else // RASPPI <= 3

	// prepare register index
	int i = pin >> 4;	// register index 0..3
	io32* g = &GPIO->GPPUPPDN[i];

	// prepare mask
	pin &= 0x0f;		// pin number 0..15
	pin <<= 1;		// *2 = 2 bits per pin
	u32 m = ~(3 << pin);	// prepare clear mask

	// setup pull
	*g = (*g & m) | (pull << pin);

#endif // RASPPI <= 3
}

// set pin detect mode (mode = combination of GPIO_EVENT_* flags)
//  - This function is not thread-safe
void GPIO_Detect(int pin, int mode)
{
	u32 mask, nmask;

	if (pin < 32)
	{
		// prepare bit mask
		mask = BIT(pin);
		nmask = ~mask;

		// rising edge
		if ((mode & GPIO_EVENT_RISE) != 0)
			GPIO->GPREN0 |= mask;
		else
			GPIO->GPREN0 &= nmask;

		// falling edge
		if ((mode & GPIO_EVENT_FALL) != 0)
			GPIO->GPFEN0 |= mask;
		else
			GPIO->GPFEN0 &= nmask;

		// high level
		if ((mode & GPIO_EVENT_HIGH) != 0)
			GPIO->GPHEN0 |= mask;
		else
			GPIO->GPHEN0 &= nmask;

		// low level
		if ((mode & GPIO_EVENT_LOW) != 0)
			GPIO->GPLEN0 |= mask;
		else
			GPIO->GPLEN0 &= nmask;

		// asynchronous rising edge
		if ((mode & GPIO_EVENT_ARISE) != 0)
			GPIO->GPAREN0 |= mask;
		else
			GPIO->GPAREN0 &= nmask;

		// asynchronous falling edge
		if ((mode & GPIO_EVENT_AFALL) != 0)
			GPIO->GPAFEN0 |= mask;
		else
			GPIO->GPAFEN0 &= nmask;
	}
	else
	{
		pin -= 32;

		// prepare bit mask
		mask = BIT(pin);
		nmask = ~mask;

		// rising edge
		if ((mode & GPIO_EVENT_RISE) != 0)
			GPIO->GPREN1 |= mask;
		else
			GPIO->GPREN1 &= nmask;

		// falling edge
		if ((mode & GPIO_EVENT_FALL) != 0)
			GPIO->GPFEN1 |= mask;
		else
			GPIO->GPFEN1 &= nmask;

		// high level
		if ((mode & GPIO_EVENT_HIGH) != 0)
			GPIO->GPHEN1 |= mask;
		else
			GPIO->GPHEN1 &= nmask;

		// low level
		if ((mode & GPIO_EVENT_LOW) != 0)
			GPIO->GPLEN1 |= mask;
		else
			GPIO->GPLEN1 &= nmask;

		// asynchronous rising edge
		if ((mode & GPIO_EVENT_ARISE) != 0)
			GPIO->GPAREN1 |= mask;
		else
			GPIO->GPAREN1 &= nmask;

		// asynchronous falling edge
		if ((mode & GPIO_EVENT_AFALL) != 0)
			GPIO->GPAFEN1 |= mask;
		else
			GPIO->GPAFEN1 &= nmask;
	}
}
