
// ****************************************************************************
//
//               Random Generator (Linear Congruential Generator LCG)
//
// ****************************************************************************

#if USE_RAND		// 1=use Random number generator, 0=not used (lib_rand.*)

#ifndef _LIB_RAND_H
#define _LIB_RAND_H

// Seed of random number generator (for all CPU cores)
extern u64 RandSeed[CORES];

// randomize random number generator - should be used before initialize timers (called from SysInit)
// - takes 64 ms (due 1st using of RNG_Get() after RNG_Init())
void RandInit(void);

// get seed of random number generator (for current CPU core)
u64 RandGet();

// set seed of random number generator (for current CPU core)
void RandSet(u64 seed);

// randomize random number generator of current CPU core with hardware random number generator
// - It can take up to 32 us to generate
// - This function is not interrupt-safe.
void Randomize(void);

// shift random number generator and return 32-bit random number (for current CPU core)
u32 RandShift();

// generate random Bool number
Bool RandBool();

// generate 8-bit unsigned integer random number
u8 RandU8();

// generate 16-bit unsigned integer random number
u16 RandU16();
int rand();

#ifndef RAND_MAX
#define RAND_MAX 0x7fffffff
#endif

// generate 32-bit unsigned integer random number
INLINE u32 RandU32() { return RandShift(); }

// generate 64-bit unsigned integer random number
u64 RandU64();

// generate 8-bit signed integer random number
INLINE s8 RandS8() { return (s8)RandU8(); }

// generate 16-bit signed integer random number
INLINE s16 RandS16() { return (s16)RandU16(); }

// generate 32-bit signed integer random number
INLINE s32 RandS32() { return (s32)RandShift(); }

// generate 64-bit signed integer random number
INLINE s64 RandS64() { return (s64)RandU64(); }

// generate float random number in range 0 (including) to 1 (excluding)
// (maximum number is 0.99999994, minimal step 0.00000006)
float RandFloat();

// generate double random number in range 0 (including) to 1 (excluding)
// (maximum number is 0.999999999999996, step 0.0000000000000035, 14 valid digits)
double RandDouble();

// generate 8-bit unsigned integer random number in range 0 to MAX (including)
u8 RandU8Max(u8 max);

// generate 16-bit unsigned integer random number in range 0 to MAX (including)
u16 RandU16Max(u16 max);

// generate 32-bit unsigned integer random number in range 0 to MAX (including)
u32 RandU32Max(u32 max);

// generate 64-bit unsigned integer random number in range 0 to MAX (including)
u64 RandU64Max(u64 max);

// generate 8-bit signed integer random number in range 0 to MAX (including, 'max' can be negative)
s8 RandS8Max(s8 max);

// generate 16-bit signed integer random number in range 0 to MAX (including, 'max' can be negative)
s16 RandS16Max(s16 max);

// generate 32-bit signed integer random number in range 0 to MAX (including, 'max' can be negative)
s32 RandS32Max(s32 max);

// generate 64-bit signed integer random number in range 0 to MAX (including, 'max' can be negative)
s64 RandS64Max(s64 max);

// generate float random number in range 0 (including) to MAX (excluding)
float RandFloatMax(float max);

// generate double random number in range 0 (including) to MAX (excluding)
double RandDoubleMax(double max);

// generate 8-bit unsigned integer random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval, including MIN and MAX.
u8 RandU8MinMax(u8 min, u8 max);

// generate 16-bit unsigned integer random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval, including MIN and MAX.
u16 RandU16MinMax(u16 min, u16 max);

// generate 32-bit unsigned integer random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval, including MIN and MAX.
u32 RandU32MinMax(u32 min, u32 max);

// generate 64-bit unsigned integer random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval, including MIN and MAX.
u64 RandU64MinMax(u64 min, u64 max);

// generate 8-bit signed integer random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval, including MIN and MAX.
s8 RandS8MinMax(s8 min, s8 max);

// generate 16-bit signed integer random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval, including MIN and MAX.
s16 RandS16MinMax(s16 min, s16 max);

// generate 32-bit signed integer random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval, including MIN and MAX.
s32 RandS32MinMax(s32 min, s32 max);

// generate 64-bit signed integer random number in range MIN to MAX (including)
// If MIN > MAX, then number is generated out of interval, including MIN and MAX.
s64 RandS64MinMax(s64 min, s64 max);

// generate float random number in range MIN (including) to MAX (excluding)
//  The order of MIN and MAX values does not matter.
float RandFloatMinMax(float min, float max);

// generate double random number in range MIN (including) to MAX (excluding)
//  The order of MIN and MAX values does not matter.
double RandDoubleMinMax(double min, double max);

// generate Gaussian float random number (mean = center, default use 0, sigma = width, default use 1)
// - Using Marsaglia polar method (optimized variant of the Box–Muller transformation)
float RandFloatGauss(float mean, float sigma);

// generate Gaussian double random number (mean = center, default use 0, sigma = width, default use 1)
// - Using Marsaglia polar method (optimized variant of the Box–Muller transformation)
double RandDoubleGauss(double mean, double sigma);

// generate random test integer number (with random number of bits)
u32 RandTestU32(u32 max);

#endif // _LIB_RAND_H

#endif // USE_RAND
