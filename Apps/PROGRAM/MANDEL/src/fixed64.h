
// ****************************************************************************
//
//                       Fixed point number, 64-bit segments
//
// ****************************************************************************

#ifndef _FIXED64_H
#define _FIXED64_H

#define FIX_BITS	64	// fixed bites
#define FIX_BASE	u64	// type of fixed base segment
#define FIX_SEGNUM	12	// number of 64-bit segments in 'fix' number
#define FIX_INTBITS	12	// number of bits of integer part of the 'fix' number (max. 63, minimum 2*integer bits = 8)
#define FIX_DIGNUM	((FIX_SEGNUM*FIX_BITS-FIX_INTBITS)/3+1+5) // size of number buffer (12 segments -> 258 digits, in real 230 digits)
#define SIZEN_MAX	(FIX_SEGNUM*FIX_BITS-FIX_INTBITS-1-7)	// max. SizeN (= 748, Zoom max. 10^225)

// temporary buffer for multiply
extern u64 FixTempBuf[2*FIX_SEGNUM];

// multiply two u64 numbers, with 128-bit result
#if AARCH==32
void Mul64(u64 a, u64 b, u64* low, u64* high);
#else
INLINE void Mul64(u64 a, u64 b, u64* low, u64* high)
{
	__uint128_t res = (__uint128_t)a * b;
	*low  = (u64)res;
	*high = (u64)(res >> 64);
}
#endif

// square of u64 number, with 128-bit result
#if AARCH==32
void Sqr64(u64 a, u64* low, u64* high);
#else
INLINE void Sqr64(u64 a, u64* low, u64* high)
{
	__uint128_t res = (__uint128_t)a * a;
	*low  = (u64)res;
	*high = (u64)(res >> 64);
}
#endif

// divide u64 by 10
INLINE u64 Div64By10(u64 word, u64* rem)
{
	u64 lo, hi;
	Mul64(word, 0xCCCCCCCCCCCCCCCDULL, &lo, &hi);
	u64 q = hi >> 3;
	*rem = word - q*10;
	return q;
}

// calculate number of u64 segments needed for 'fix' numbers, from the current view size
// (Size = 1/Zoom; at Size=1 the base full view is shown; image is 640 x 480 pixels)
//int SizeToFixSeg(double size);

// calculate number of u64 segments needed for 'fix' numbers, from the current view size,
// given as SizeN where Size = 1 >> SizeN (SizeN = negative binary logarithm of Size)
// - integer-only alternative to SizeToSeg()
//int SizeNToFixSeg(int sizen);

// calculate number of decimal digits needed to display a number with 'seg' segments via fix::ToText()
int FixSegToDig(int seg);

// calculate minimum number of u64 segments needed to represent a number with 'dig' decimal digits
int DigToFixSeg(int dig);

// fixed-point number
class fix
{
public:
	Bool	m_neg;			// flag - number is negative (zero cannot be negative)
	Bool	m_zero;			// flag - number is zero (zero number is always not-negative)
	u64	m_data[FIX_SEGNUM];	// data of the mantissa (stored from the most significant bits)

// internal functions (could be private)

	// internal function - check if mantissa is zero
	Bool MantIsZero() const;

	// internal function - check if mantissa is negative
	INLINE Bool MantIsNeg() const { return (s64)this->m_data[0] < 0; }

	// internal function - negate mantissa (only mantissa, not flag)
	void MantNeg();

	// internal function - raw add two mantissas to this mantissa, without sign corrections
	void MantAdd(const fix* a, const fix* b);

	// internal function - raw subtract two mantissas to this mantissa, without sign corrections (returns True if result is zero)
	Bool MantSub(const fix* a, const fix* b);

	// internal function - raw multiply two mantissas, result (unsigned magnitude) into this mantissa
	void MantMul(const fix* a, const fix* b);

	// internal function - raw square mantissa of mantissa, result (unsigned magnitude) into this mantissa
	void MantSqr(const fix* a);

	// internal function - shift down raw double-wide product (2*FIX_SEGNUM words, little-endian array)
	// by the fractional-point offset and store the result as this mantissa
	void MantMulFinish(const u64* prod);

// public functions

	// check if number is negative
	INLINE Bool IsNeg() const { return this->m_neg; }

	// check if number is zero
	INLINE Bool IsZero() const { return this->m_zero; }

	// negate number (only flag, not mantissa)
	INLINE void Neg() { if (!this->m_zero) this->m_neg = !this->m_neg; }

	// set number to zero
	void SetZero();

	// set number to one
	void SetOne();

	// copy another number to this number
	void Copy(const fix* num);

	// compare numbers (returns: -1 if this < num, 0 if this == num, +1 if this > num)
	int Comp(const fix* num) const;

	// add two numbers (source numbers and destination number can be the same)
	void Add(const fix* a, const fix* b);

	// subtract two numbers (source numbers and destination number can be the same)
	void Sub(const fix* a, const fix* b);

	// multiply two numbers (source numbers and destination number can be the same)
	void Mul(const fix* a, const fix* b);

	// square a number (source number and destination number can be the same)
	void Sqr(const fix* a);

	// convert double number to fix number (low conversion accuracy)
	void FromDbl(double num);

	// convert to double number
	double ToDouble() const;

	// multiply by 2 (source number and destination number can be the same)
	void Mul2(const fix* a);

	// divide by 2 (source number and destination number can be the same)
	void Div2(const fix* a);

	// multiply number by a signed 32-bit integer constant
	void MulInt(s32 num);

	// divide number by a signed 32-bit integer constant (division by zero is not handled)
	void DivInt(s32 num);

	// convert number to text with a decimal point, into buffer of given length (including terminating zero)
	//  buf ... output bufer
	//  buflen ... length of the buffer, including terminating zero
	// Returns text length (without terminating 0).
	int ToText(char* buf, int buflen) const;

	// convert number from ASCIIZ text (optional sign, optional decimal point) - no overflow checking
	// Returns length of parsed text.
	int FromText(const char* buf);
};

#endif // _FIXED64_H
