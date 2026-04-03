
// ****************************************************************************
//
//               Random Generator (Linear Congruential Generator LCG)
//
// ****************************************************************************

#include "../../includes.h"	// includes

#if USE_RAND		// 1=use Random number generator, 0=not used (lib_rand.*)

// Seed of random number generator (for all CPU cores)
u64 RandSeed[CORES];

// randomize random number generator (called from SysInit)
// - takes 64 ms (due 1st using of RNG_Get() after RNG_Init())
void RandInit(void)
{
	// randomize random number generator of current CPU core with hardware random number generator
	// - It can take up to 32 us to generate
	// - This function is not interrupt and core safe.
	Randomize();

	// setup other 3 cores
#if CORES > 1
	int i;
	for (i = 1; i < CORES; i++) RandSeed[i] = RandU64() + RandU64();
#endif
}

// get seed of random number generator (for current CPU core)
u64 RandGet()
{
	return RandSeed[CpuID()];
}

// set seed of random number generator (for current CPU core)
void RandSet(u64 seed)
{
	RandSeed[CpuID()] = seed;
}

// randomize random number generator of current CPU core with hardware random number generator
// - It can take up to 32 us to generate
// - This function is not interrupt and core safe.
void Randomize(void)
{
	RandSeed[CpuID()] += SysTime32() + RNG_Get();
	RandShift();
	RandShift();
	RandShift();
}

// shift random number generator and return 32-bit random number (for current CPU core)
NOINLINE u32 RandShift()
{
	// disable interrupts
	IRQ_LOCK;

	// get pointer to the seed
	u64* s = &RandSeed[CpuID()];

	// shift seed
	u64 k = *s;
	k = k*214013 + 2531011;
	*s = k;

	// enable interrupts
	IRQ_UNLOCK;

	return (u32)(k >> 32);
}

// generate random Bool number
Bool RandBool()
{
	return (Bool)(RandShift() >> 31);
}

// generate 8-bit unsigned integer random number
u8 RandU8()
{
	return (u8)(RandShift() >> 24);
}

// generate 16-bit unsigned integer random number
u16 RandU16()
{
	return (u16)(RandShift() >> 16);
}

int rand()
{
	return (int)(RandShift() >> 1);
}

// generate 64-bit unsigned integer random number
u64 RandU64()
{
	u32 n = RandShift();
	return RandShift() | ((u64)n << 32);
}

// generate float random number in range 0 (including) to 1 (excluding)
// (maximum number is 0.99999994, minimal step 0.00000006)
float RandFloat()
{
	return (float)(RandShift() >> (32-24))*(1.0f/0x1000000);
}

// generate double random number in range 0 (including) to 1 (excluding)
// (maximum number is 0.999999999999996, step 0.0000000000000035, 14 valid digits)
double RandDouble()
{
	RandShift();
	return (double)(RandGet() >> (64-48))*(1.0/0x1000000000000ULL);
}

// generate 8-bit unsigned integer random number in range 0 to MAX (including)
u8 RandU8Max(u8 max)
{
	// zero maximal value
	if (max == 0) return 0;

	// Multiplicative scaling
	return (u8)(((u64)RandU32() * (u32)((u32)max+1)) >> 32);
}

// generate 16-bit unsigned integer random number in range 0 to MAX (including)
u16 RandU16Max(u16 max)
{
	// zero maximal value
	if (max == 0) return 0;

	// Multiplicative scaling
	return (u16)(((u64)RandU32() * (u32)((u32)max+1)) >> 32);
}

// generate 32-bit unsigned integer random number in range 0 to MAX (including)
u32 RandU32Max(u32 max)
{
	// zero maximal value
	if (max == 0) return 0;

	// Multiplicative scaling
	u32 res = RandU32();
	if (max != 0xffffffff)
	{
		max++;
		res = (u32)(((u64)res * max) >> 32);
	}
	return res;
}

// generate 64-bit unsigned integer random number in range 0 to MAX (including)
u64 RandU64Max(u64 max)
{
	u64 res;
	u64 msk;

	// DWORD value
	if (max < 0x100000000ULL) return RandU32Max((u32)max);

	// zero maximal value
	if (max == 0) return 0;

	// prepare mask
	u8 pos = Order((u32)(max >> 32));
	msk = (u64)-1 >> (32 - pos);

	// generate random number ... rejection sampling
	do {
		res = RandU64() & msk;
	} while (res > max);

	return res;
}

// generate 8-bit signed integer random number in range 0 to MAX (including, 'max' can be negative)
s8 RandS8Max(s8 max)
{
	if (max >= 0)
		return (s8)RandU8Max((u8)max);
	else
		return -(s8)RandU8Max((u8)-max);
}

// generate 16-bit signed integer random number in range 0 to MAX (including, 'max' can be negative)
s16 RandS16Max(s16 max)
{
	if (max >= 0)
		return (s16)RandU16Max((u16)max);
	else
		return -(s16)RandU16Max((u16)-max);
}

// generate 32-bit signed integer random number in range 0 to MAX (including, 'max' can be negative)
s32 RandS32Max(s32 max)
{
	if (max >= 0)
		return (s32)RandU32Max((u32)max);
	else
		return -(s32)RandU32Max((u32)-max);
}

// generate 64-bit signed integer random number in range 0 to MAX (including, 'max' can be negative)
s64 RandS64Max(s64 max)
{
	if (max >= 0)
		return (s64)RandU64Max((u64)max);
	else
		return -(s64)RandU64Max((u64)-max);
}

// generate float random number in range 0 (including) to MAX (excluding)
float RandFloatMax(float max)
{
	return RandFloat() * max;
}

// generate double random number in range 0 (including) to MAX (excluding)
double RandDoubleMax(double max)
{
	return RandDouble() * max;
}

// generate 8-bit unsigned integer random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval, including MIN and MAX.
u8 RandU8MinMax(u8 min, u8 max)
{
	return RandU8Max(max - min) + min;
}

// generate 16-bit unsigned integer random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval, including MIN and MAX.
u16 RandU16MinMax(u16 min, u16 max)
{
	return RandU16Max(max - min) + min;
}

// generate 32-bit unsigned integer random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval, including MIN and MAX.
u32 RandU32MinMax(u32 min, u32 max)
{
	return RandU32Max(max - min) + min;
}

// generate 64-bit unsigned integer random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval, including MIN and MAX.
u64 RandU64MinMax(u64 min, u64 max)
{
	return RandU64Max(max - min) + min;
}

// generate 8-bit signed integer random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval, including MIN and MAX.
s8 RandS8MinMax(s8 min, s8 max)
{
	return (s8)RandU8Max(max - min) + min;
}

// generate 16-bit signed integer random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval, including MIN and MAX.
s16 RandS16MinMax(s16 min, s16 max)
{
	return (s16)RandU16Max(max - min) + min;
}

// generate 32-bit signed integer random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval, including MIN and MAX.
s32 RandS32MinMax(s32 min, s32 max)
{
	return (s32)RandU32Max(max - min) + min;
}

// generate 64-bit signed integer random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval, including MIN and MAX.
s64 RandS64MinMax(s64 min, s64 max)
{
	return (s64)RandU64Max(max - min) + min;
}

// generate float random number in range MIN (including) to MAX (excluding)
//  The order of MIN and MAX values does not matter.
float RandFloatMinMax(float min, float max)
{
	return RandFloatMax(max - min) + min;
}

// generate double random number in range MIN (including) to MAX (excluding)
//  The order of MIN and MAX values does not matter.
double RandDoubleMinMax(double min, double max)
{
	return RandDoubleMax(max - min) + min;
}

// generate Gaussian float random number (mean = center, default use 0, sigma = width, default use 1)
// - Using Marsaglia polar method (optimized variant of the Box–Muller transformation)
float RandFloatGauss(float mean, float sigma)
{
	float x, y, r;

	// Generate value inside the unit circle
	do {
		x = RandFloat()*2 - 1;
		y = RandFloat()*2 - 1;
		r = x*x + y*y;
	} while ((r == 0) || (r > 1));

	// Transformation to normal distribution
	r = sqrtf(-2*logf(r)/r);
	return x*r*sigma + mean;
}

// generate Gaussian double random number (mean = center, default use 0, sigma = width, default use 1)
// - Using Marsaglia polar method (optimized variant of the Box–Muller transformation)
double RandDoubleGauss(double mean, double sigma)
{
	double x, y, r;

	// Generate value inside the unit circle
	do {
		x = RandDouble()*2 - 1;
		y = RandDouble()*2 - 1;
		r = x*x + y*y;
	} while ((r == 0) || (r > 1));

	// Transformation to normal distribution
	r = sqrt(-2*log(r)/r);
	return x*r*sigma + mean;
}

// generate random test integer number (with random number of bits)
u32 RandTestU32(u32 max)
{
	u32 num = RandU32Max(max);
	u8 shift = RandU8Max(Order(num));
	return num >> shift;
}

#endif // USE_RAND
