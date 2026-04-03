
// ****************************************************************************
//
//                         Hardware Random Number Generator
//
// ****************************************************************************

#ifndef _SDK_RNG_H
#define _SDK_RNG_H

// Hardware Random Number Generator interface
typedef struct {

	io32	CTRL;		// 0x00: control register
				//	bit 0: 1=enable RNG

	io32	STATUS;		// 0x04: status, number of available bytes

	io32	DATA;		// 0x08: random 32-bit data

	io32	res;		// 0x0C: ... reserved

	io32	INTMASK;	// 0x10: interrupt mask

} RNG_t;
STATIC_ASSERT(sizeof(RNG_t) == 0x14, "Incorrect RNG_t!");

// RNG interface
#define RNG ((RNG_t*)ARM_HW_RNG_BASE)

// initialize hardware random number generator (first RNG_Get() takes 64 ms)
void RNG_Init(void);

// terminate hardware random number generator
void RNG_Term(void);

// get random number (it can take up to 32 us to generate)
// - This function is not interrupt and core safe.
u32 RNG_Get(void);

#endif // _SDK_RNG_H
