
// ****************************************************************************
//
//                         Hardware Random Number Generator
//
// ****************************************************************************

#include "../../includes.h"	// includes

// initialize hardware random number generator (first RNG_Get() takes 64 ms)
void RNG_Init(void)
{
	// disable RNG
	RNG->CTRL = 0;

	// the initial numbers generated are "less random" so will be discarded
	//  0x10000 ... 1st RNG_Get() takes 960 ms
	//  0x20000 ... 1st RNG_Get() takes 900 ms
	//  0x40000 ... 1st RNG_Get() takes 750 ms ... Linux default
	//  0x80000 ... 1st RNG_Get() takes 500 ms
	//  0xC0000 ... 1st RNG_Get() takes 256 ms
	//  0xF0000 ... 1st RNG_Get() takes 64 ms
	RNG->STATUS = 0xF0000;

	// enable RNG
	RNG->CTRL = 1;
}

// terminate hardware random number generator
void RNG_Term(void)
{
	// disable RNG
	RNG->CTRL = 0;
}

// get random number (it can take up to 32 us to generate)
// - This function is not interrupt and core safe.
u32 RNG_Get(void)
{
	// wait for data to be ready
	while ((RNG->STATUS >> 24) == 0) {}

	// get data
	return RNG->DATA;
}
