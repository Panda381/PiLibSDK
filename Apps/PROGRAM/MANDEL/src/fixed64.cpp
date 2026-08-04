
// ****************************************************************************
//
//                       Fixed point number, 64-bit segments
//
// ****************************************************************************

#include "../include.h"

// temporary buffer for multiply
u64 FixTempBuf[2*FIX_SEGNUM];

// multiply two u64 numbers, with 128-bit result
#if AARCH==32
void Mul64(u64 a, u64 b, u64* low, u64* high)
{
	//       aHaL
	//     x bHbL
	// ----------
	//       aLbL ...   k0
	//     aLbH   ...  k1
	//     aHbL   ...  k2
	//   aHbH     ... k3

	// prepare elements
	u32 aL = (u32)a;
	u32 aH = (u32)(a >> 32);
	u32 bL = (u32)b;
	u32 bH = (u32)(b >> 32);

	// multiply elements (max. FFFFFFFF * FFFFFFFF = FFFFFFFE00000001)
	u64 k0 = (u64)aL * bL;
	u64 k1 = (u64)aL * bH;
	u64 k2 = (u64)aH * bL;
	u64 k3 = (u64)aH * bH;

	// sum komponents
	k1 += (k0 >> 32);	// max. FFFFFFFE00000001 + FFFFFFFE = FFFFFFFEFFFFFFFF, no carry yet
	k1 += k2;		// max. FFFFFFFEFFFFFFFF + FFFFFFFE00000001 = (1)FFFFFFFD00000000, it can carry
	if (k1 < k2) k3 += (u64)1 << 32; // add carry, FFFFFFFE00000001 + 100000000 = FFFFFFFF00000001, no carry

	// get result, max. FFFFFFFFFFFFFFFF * FFFFFFFFFFFFFFFF = FFFFFFFFFFFFFFFE:0000000000000001
	*low = (k1 << 32) + (u32)k0; // result low, FFFFFFFF00000000 + FFFFFFFF = FFFFFFFFFFFFFFFF, no carry
	*high = k3 + (k1 >> 32); // result high, FFFFFFFF00000001 + FFFFFFFD = FFFFFFFFFFFFFFFE, no carry 
}
#endif

// square of u64 number, with 128-bit result
#if AARCH==32
void Sqr64(u64 a, u64* low, u64* high)
{
	//       aHaL
	//     x aHaL
	// ----------
	//       aLaL ...   k0
	//  2x aLaH   ...  ka
	//   aHaH     ... k3

	// prepare elements
	u32 aL = (u32)a;
	u32 aH = (u32)(a >> 32);

	// multiply elements (max. FFFFFFFF * FFFFFFFF = FFFFFFFE00000001)
	u64 k0 = (u64)aL * aL;
	u64 ka = (u64)aL * aH;
	u64 k3 = (u64)aH * aH;

	// sum komponents
	u64 k1 = ka;
	k1 += (k0 >> 32);	// max. FFFFFFFE00000001 + FFFFFFFE = FFFFFFFEFFFFFFFF, no carry yet
	k1 += ka;		// max. FFFFFFFEFFFFFFFF + FFFFFFFE00000001 = (1)FFFFFFFD00000000, it can carry
	if (k1 < ka) k3 += (u64)1 << 32; // add carry, FFFFFFFE00000001 + 100000000 = FFFFFFFF00000001, no carry

	// get result, max. FFFFFFFFFFFFFFFF * FFFFFFFFFFFFFFFF = FFFFFFFFFFFFFFFE:0000000000000001
	*low = (k1 << 32) + (u32)k0; // result low, FFFFFFFF00000000 + FFFFFFFF = FFFFFFFFFFFFFFFF, no carry
	*high = k3 + (k1 >> 32); // result high, FFFFFFFF00000001 + FFFFFFFD = FFFFFFFFFFFFFFFE, no carry 
}
#endif

/*
// calculate number of u64 segments needed for 'fix' numbers, from the current view size
// (Size = 1/Zoom; at Size=1 the base full view is shown; image is 640 x 480 pixels)
int SizeToFixSeg(double size)
{
	// worst-case step between two neighboring pixels, in the complex plane
	int biggerdim = (WIDTH > HEIGHT) ? WIDTH : HEIGHT;
	double pixelsize = size / biggerdim;

	// number of fractional bits needed to distinguish one pixel step, plus safety margin
	//  10 ... extra safety bits reserved for accumulated rounding errors during iteration
	int fracbits = (int)ceil(-log2(pixelsize)) + 10;
	if (fracbits < 1) fracbits = 1;

	// total mantissa bits = fractional bits + integer bits
	int bits = fracbits + FIX_INTBITS;

	// round up to whole u64 segments
	int seg = (bits + 63) / 64;
	if (seg < 1) seg = 1;
	if (seg > FIX_SEGNUM) seg = FIX_SEGNUM;

	return seg;
}
*/

/*
// calculate number of u64 segments needed for 'fix' numbers, from the current view size,
// given as SizeN where Size = 1 >> SizeN (SizeN = negative binary logarithm of Size)
// - integer-only alternative to SizeToSeg()
int SizeNToFixSeg(int sizen)
{
	if (sizen < 0) sizen = 0;

	// number of fractional bits needed to distinguish one pixel step, plus safety margin
	//  first 10 ... log2(1024) as an approximate representation of the 640-pixel image size in bits
	//  second 10 ... extra safety bits reserved for accumulated rounding errors during iteration
	int fracbits = sizen + 10 + 10;

	// total mantissa bits = fractional bits + integer bits
	int bits = fracbits + FIX_INTBITS;

	// round up to whole u64 segments
	int seg = (bits + 63) / 64;
	if (seg < 1) seg = 1;
	if (seg > FIX_SEGNUM) seg = FIX_SEGNUM;

	return seg;
}
*/

// calculate number of decimal digits needed to display a number with 'seg' segments via fix::ToText()
int FixSegToDig(int seg)
{
	if (seg < 1) seg = 1;
	int fracbits = 64*seg - FIX_INTBITS;
	if (fracbits < 0) fracbits = 0;

	int dig = (int)(fracbits*0.30103) + 1;	// log10(2) = 0.30103
	return dig;
}

// calculate minimum number of u64 segments needed to represent a number with 'dig' decimal digits
int DigToFixSeg(int dig)
{
	if (dig < 1) dig = 1;

	// smallest fracbits such that floor(fracbits*log10(2))+1 >= dig
	int fracbits = (int)ceil((dig-1)/0.30103);
	if (fracbits < 0) fracbits = 0;

	int bits = fracbits + FIX_INTBITS;
	int seg = (bits + 63) / 64;
	if (seg < 1) seg = 1;
	if (seg > FIX_SEGNUM) seg = FIX_SEGNUM;

	return seg;
}

// internal function - check if mantissa is zero
Bool fix::MantIsZero() const
{
	int i, n;
	n = FIX_SEGNUM;
	for (i = 0; i < n; i++) if (this->m_data[i] != 0) return False;
	return True;
}

// internal function - negate mantissa (only mantissa, not flag)
void fix::MantNeg()
{
	int i;
	int carry = 1;
	for (i = FIX_SEGNUM-1; i >= 0; i--)
	{
		u64 k = ~this->m_data[i];
		u64 sum = k + carry;
		carry = (sum < k) ? 1 : 0;
		this->m_data[i] = sum;
	}
}

// internal function - raw add two mantissas to this mantissa, without sign corrections
void fix::MantAdd(const fix* a, const fix* b)
{
	int i = FIX_SEGNUM-1;
	u64* d = &this->m_data[i];
	const u64* s1 = &a->m_data[i];
	const u64* s2 = &b->m_data[i];
	int carry = 0;

	for (; i >= 0; i--)
	{
		u64 k1 = *s1 + carry;
		carry = (k1 < carry) ? 1 : 0;
		u64 k2 = *s2;
		k1 += k2;
		carry += (k1 < k2) ? 1 : 0;
		*d = k1;
		d--;
		s1--;
		s2--;
	}
}

// internal function - raw subtract two mantissas to this mantissa, without sign corrections (returns True if result is zero)
Bool fix::MantSub(const fix* a, const fix* b)
{
	int i = FIX_SEGNUM-1;
	u64* d = &this->m_data[i];
	const u64* s1 = &a->m_data[i];
	const u64* s2 = &b->m_data[i];
	int carry = 0;
	u64 sum = 0;

	for (; i >= 0; i--)
	{
		u64 k2 = *s2 + carry;
		carry = (k2 < carry) ? 1 : 0;
		u64 k1 = *s1;
		carry += (k1 < k2) ? 1 : 0;
		k1 -= k2;
		sum |= k1;
		*d = k1;
		d--;
		s1--;
		s2--;
	}
	return sum == 0;
}

// set number to zero
void fix::SetZero()
{
	this->m_neg = False;
	this->m_zero = True;
	memset(this->m_data, 0, FIX_SEGNUM*sizeof(FIX_BASE));
}

// set number to one
void fix::SetOne()
{
	this->SetZero();
	this->m_zero = False;
	this->m_data[0] = 1ull << (FIX_BITS - FIX_INTBITS);
}

// copy another number to this number
void fix::Copy(const fix* num)
{
	if (this == num) return;
	this->m_neg = num->m_neg;
	this->m_zero = num->m_zero;
	memcpy(this->m_data, num->m_data, FIX_SEGNUM*sizeof(FIX_BASE));
}

// compare numbers (returns: -1 if this < num, 0 if this == num, +1 if this > num)
int fix::Comp(const fix* num) const
{
	int i, n;

	// this number is zero
	if (this->m_zero)
	{
		if (num->m_zero) return 0;	// both numbers are zero
		if (num->m_neg) return 1;	// 0 > negative
		return -1;			// 0 < positive
	}

	// this number is negative
	if (this->m_neg)
	{
		if (!num->m_neg)  return -1;	// negative < positive or zero
	}

	// this number is positive (not zero)
	else
	{
		if (num->m_neg || num->m_zero) return 1; // positive > negative or zero
	}

	// both numbers are positive
	const u64* d1 = this->m_data;
	const u64* d2 = num->m_data;
	i = FIX_SEGNUM;
	if (!num->m_neg)
	{
		for (; i > 0; i--)
		{
			u64 a = *d1++;
			u64 b = *d2++;
			if (a != b)
			{
				if (a < b) return -1;
				return 1;
			}
		}
	}

	// both numbers are negative
	else
	{
		for (; i > 0; i--)
		{
			u64 a = *d1++;
			u64 b = *d2++;
			if (a != b)
			{
				if (a < b) return 1;
				return -1;
			}
		}
	}

	// numbers are equal
	return 0;
}

// add two numbers (source numbers and destination number can be the same)
void fix::Add(const fix* a, const fix* b)
{
	// 1st number is zero
	if (a->m_zero)
	{
		this->Copy(b);
		return;
	}

	// 2nd number is zero
	if (b->m_zero)
	{
		this->Copy(a);
		return;
	}

	// signs are equal (and not zero)
	if (a->m_neg == b->m_neg)
	{
		// raw add two mantissas to this mantissa, without sign corrections
		this->MantAdd(a, b);
		this->m_zero = False;
		this->m_neg = a->m_neg;
	}

	// signs are different
	else
	{
		// raw subtract two mantissas to this mantissa, without sign corrections (returns True if result is zero)
		this->m_zero = this->MantSub(a, b);
		this->m_neg = (this->m_zero) ? False : a->m_neg;
		if (this->MantIsNeg())
		{
			this->MantNeg();
			this->m_neg = !this->m_neg;
		}
	}
}

// subtract two numbers (source numbers and destination number can be the same)
void fix::Sub(const fix* a, const fix* b)
{
	// 1st number is zero
	if (a->m_zero)
	{
		this->Copy(b);
		this->Neg();
		return;
	}

	// 2nd number is zero
	if (b->m_zero)
	{
		this->Copy(a);
		return;
	}

	// signs are equal (and not zero)
	if (a->m_neg == b->m_neg)
	{
		// raw subtract two mantissas to this mantissa, without sign corrections (returns True if result is zero)
		this->m_zero = this->MantSub(a, b);
		this->m_neg = (this->m_zero) ? False : a->m_neg;
		if (this->MantIsNeg())
		{
			this->MantNeg();
			this->m_neg = !this->m_neg;
		}
	}

	// signs are different
	else
	{
		// raw add two mantissas to this mantissa, without sign corrections
		this->MantAdd(a, b);
		this->m_zero = False;
		this->m_neg = a->m_neg;
	}
}

// internal helper - add one u64 word into a little-endian array at position 'pos',
// propagating carry into higher words (array has 'size' words)
static void MantMulAddCarry(u64* arr, int pos, int size, u64 val)
{
	u64 sum = arr[pos] + val;
	u64 carry = (sum < val) ? 1 : 0;
	arr[pos] = sum;
	pos++;
	while ((carry != 0) && (pos < size))
	{
		sum = arr[pos] + carry;
		carry = (sum == 0) ? 1 : 0;
		arr[pos] = sum;
		pos++;
	}
}

// internal function - shift down raw double-wide product (2*FIX_SEGNUM words, little-endian array)
// by the fractional-point offset and store the result as this mantissa
void fix::MantMulFinish(const u64* prod)
{
	int n = FIX_SEGNUM;
	int wordshift = n - 1;
	int bitshift = FIX_BITS - FIX_INTBITS;

	int i;
	for (i = 0; i < n; i++)
	{
		u64 lo = prod[wordshift+i] >> bitshift;
		u64 hi = prod[wordshift+i+1] << FIX_INTBITS;
		this->m_data[n-1-i] = lo | hi;
	}
}

// internal function - raw multiply two mantissas, result (unsigned magnitude) into this mantissa
void fix::MantMul(const fix* a, const fix* b)
{
	int n = FIX_SEGNUM;
	u64* prod = FixTempBuf;
	memset(prod, 0, 2*n*sizeof(FIX_BASE));

	int i, j;
	for (i = 0; i < n; i++)
	{
		u64 ai = a->m_data[n-1-i];		// little-endian word 'i' of 'a'
		if (ai != 0)
		{
			for (j = 0; j < n; j++)
			{
				u64 bj = b->m_data[n-1-j];	// little-endian word 'j' of 'b'
				if (bj != 0)
				{
					u64 lo, hi;
					Mul64(ai, bj, &lo, &hi);
					MantMulAddCarry(prod, i+j,   2*n, lo);
					MantMulAddCarry(prod, i+j+1, 2*n, hi);
				}
			}
		}
	}

	// this->m_data is only written here, after all reads of a/b are done,
	// so it is safe even if 'this' aliases 'a' or 'b'
	this->MantMulFinish(prod);
}

// internal function - raw square mantissa of 'a', result (unsigned magnitude) into this mantissa
void fix::MantSqr(const fix* a)
{
	int n = FIX_SEGNUM;
	u64* prod = FixTempBuf;
	memset(prod, 0, 2*n*sizeof(FIX_BASE));

	u64 lo, hi;
	int i, j;

	// diagonal terms: ai*ai
	for (i = 0; i < n; i++)
	{
		u64 ai = a->m_data[n-1-i];
		if (ai != 0)
		{
			Sqr64(ai, &lo, &hi);
			MantMulAddCarry(prod, 2*i,   2*n, lo);
			MantMulAddCarry(prod, 2*i+1, 2*n, hi);
		}
	}

	// off-diagonal terms: ai*aj (i != j) - each pair appears twice, add it in twice instead
	for (i = 0; i < n; i++)
	{
		u64 ai = a->m_data[n-1-i];
		if (ai != 0)
		{
			for (j = i+1; j < n; j++)
			{
				u64 aj = a->m_data[n-1-j];
				if (aj != 0)
				{
					Mul64(ai, aj, &lo, &hi);
					MantMulAddCarry(prod, i+j,   2*n, lo);
					MantMulAddCarry(prod, i+j,   2*n, lo);	// added twice (symmetric term)
					MantMulAddCarry(prod, i+j+1, 2*n, hi);
					MantMulAddCarry(prod, i+j+1, 2*n, hi);
				}
			}
		}
	}

	this->MantMulFinish(prod);
}

// multiply two numbers (source numbers and destination number can be the same)
void fix::Mul(const fix* a, const fix* b)
{
	// if either operand is zero, result is zero
	if (a->m_zero || b->m_zero)
	{
		this->SetZero();
		return;
	}

	// multiply
	this->MantMul(a, b);

	// result can be zero only due to rounding down a very small fractional product
	this->m_zero = this->MantIsZero();
	this->m_neg = (this->m_zero) ? False : (a->m_neg != b->m_neg);
}

// square a number (source number and destination number can be the same)
void fix::Sqr(const fix* a)
{
	// result is zero
	if (a->m_zero)
	{
		this->SetZero();
		return;
	}

	// square
	this->MantSqr(a);

	// result can be zero only due to rounding down a very small fractional product
	this->m_zero = this->MantIsZero();
	this->m_neg = False;	// square of a non-zero number is always positive
}

// convert double number to fix number (low conversion accuracy)
void fix::FromDbl(double num)
{
	this->SetZero();
	this->m_data[0] = (u64)(s64)(num * (1ull << (FIX_BITS - FIX_INTBITS)));
	this->m_zero = this->MantIsZero();
	this->m_neg = this->MantIsNeg();
	if (this->m_neg) this->MantNeg();
}

// convert to double number
double fix::ToDouble() const
{
	// zero
	if (this->m_zero) return 0.0;

	// get segments
	double c = 1.0 / (1ull << (FIX_BITS - FIX_INTBITS)); // multiplier of 1st segment = ldexp(1.0, -(FIX_BITS - FIX_INTBITS))
	double res = (double)this->m_data[0] * c;

	int i;
	for (i = 1; i < FIX_SEGNUM; i++)
	{
		c *= 1.0 / (1ull << 32) / (1ull << 32);
		res += (double)this->m_data[i] * c;
	}

	// return with sign
	return (this->m_neg) ? -res : res;
}

// multiply by 2 (source number and destination number can be the same)
void fix::Mul2(const fix* a)
{
	// zero number
	if (a->m_zero)
	{
		this->SetZero();
		return;
	}

	// get sign
	this->m_neg = a->m_neg;
	this->m_zero = False;

	int i = FIX_SEGNUM-1;
	u64* d = &this->m_data[i];
	const u64* s = &a->m_data[i];
	int carry = 0;
	int carry2;

	for (; i >= 0; i--)
	{
		u64 k = *s;
		carry2 = (s64)k < 0;
		*d = (k << 1) | carry;
		carry = carry2;
		s--;
		d--;
	}
}

// divide by 2 (source number and destination number can be the same)
void fix::Div2(const fix* a)
{
	// zero number
	if (a->m_zero)
	{
		this->SetZero();
		return;
	}

	int i = FIX_SEGNUM-1;
	u64* d = &this->m_data[0];
	const u64* s = &a->m_data[0];
	u64 carry = 0;
	u64 carry2;
	u64 sum = 0;

	for (; i >= 0; i--)
	{
		u64 k = *s;
		carry2 = k << 63;
		k = (k >> 1) | carry;
		*d = k;
		sum |= k;
		carry = carry2;
		s++;
		d++;
	}
	this->m_zero = (sum == 0);
	this->m_neg = (this->m_zero) ? False : a->m_neg;
}

// multiply number by a signed 32-bit integer constant
void fix::MulInt(s32 num)
{
	// result is zero
	if (this->m_zero) return;
	if (num == 0)
	{
		this->SetZero();
		return;
	}

	// prepare sign
	Bool negnum = num < 0;
	u64 mult = (u64)(negnum ? (-(s64)num) : (s64)num);

	// multiply
	int n = FIX_SEGNUM;
	u64 carry = 0;
	int i;
	u64 sum = 0;
	for (i = n-1; i >= 0; i--)
	{
		u64 lo, hi;
		Mul64(this->m_data[i], mult, &lo, &hi);

		u64 k = lo + carry;
		u64 carryout = (k < lo) ? 1 : 0;

		sum |= k; // to check zero result
		this->m_data[i] = k;
		carry = hi + carryout;	// overflow beyond the top segment is silently discarded
	}

	// result flags
	this->m_zero = (sum == 0);
	this->m_neg = (this->m_zero) ? False : (this->m_neg != negnum);
}

// divide number by a signed 32-bit integer constant (division by zero is not handled)
void fix::DivInt(s32 num)
{
	// check zero result
	if (this->m_zero) return;

	// prepare sign
	Bool negnum = num < 0;
	u64 div = (u64)(negnum ? (-(s64)num) : (s64)num);

	// divide
	int n = FIX_SEGNUM;
	u64 rem = 0;
	int i;
	u64 sum = 0;
	for (i = 0; i < n; i++)
	{
		// split 64-bit to 32-bit parts
		u64 word = this->m_data[i];
		u32 hiword = (u32)(word >> 32);
		u32 loword = (u32)word;

		// divide upper half, combined with remainder from the previous (more significant) segment
		u64 cur = (rem << 32) | hiword;
		u64 qhi = cur / div;
		rem = cur - qhi*div;

		// divide lower half, combined with remainder from the upper half
		cur = (rem << 32) | loword;
		u64 qlo = cur / div;
		rem = cur - qlo*div;

		u64 k = (qhi << 32) | qlo;
		sum |= k; // to check zero result
		this->m_data[i] = k;
	}
	// final remainder (rem) represents a fraction below the available precision - discarded

	// result flags
	this->m_zero = (sum == 0);
	this->m_neg = (this->m_zero) ? False : (this->m_neg != negnum);
}

// convert number to text with a decimal point, into buffer of given length (including terminating zero)
//  buf ... output bufer
//  buflen ... length of the buffer, including terminating zero
// Returns text length (without terminating 0).
int fix::ToText(char* buf, int buflen) const
{
	int i, k;
	char* d = buf;
	char* dmax = buf + buflen - 1;	// reserve place for terminating zero

	// sign
	if (this->m_neg)
	{
		if (d < dmax) *d++ = '-';
	}

	// integer part fits into a plain u64 (top FIX_INTBITS bits of m_data[0])
	u64 intpart = this->m_data[0] >> (FIX_BITS - FIX_INTBITS);

	// convert integer part by repeated division by 10, into a small temporary buffer
	char tmp[24];
	int tn = 0;
	do {
		u64 intpart2 = intpart/10;
		tmp[tn++] = (char)('0' + (int)(intpart - intpart2*10));
		intpart = intpart2;
	} while (intpart != 0);

	// reverse integer part
	while ((tn > 0) && (d < dmax))
	{
		tn--;
		*d++ = tmp[tn];
	}

	// decimal point
	if (d < dmax) *d++ = '.';

	// copy number into temporary buffer
	int n = FIX_SEGNUM; // length of the number
	u64* frac = FixTempBuf; // temporary buffer
	memcpy(frac, this->m_data, n*sizeof(FIX_BASE));

	// number of decimal digits needed to cover the fraction precision, derived from FIX_SEGNUM
	int fracdigits = FixSegToDig(n);

	// generate fraction digits by repeated multiplication of the whole array by 10;
	// the overflow to integer part is the next decimal digit
	for (k = 0; k < fracdigits; k++)
	{
		// output buffer is full
		if (d >= dmax) break;

		// clear integer part
		frac[0] &= (u64)~0 >> FIX_INTBITS;

		// multiply number * 10
		u64 carry = 0;
		for (i = n-1; i >= 0; i--)
		{
			u64 lo, hi;
			Mul64(frac[i], 10, &lo, &hi);
			u64 sum = lo + carry;
			u64 carryout = (sum < lo) ? 1 : 0;
			frac[i] = sum;
			carry = hi + carryout;
		}

		// get result digit
		*d++ = (char)('0' + (int)(frac[0] >> (FIX_BITS - FIX_INTBITS)));
	}

	// terminating 0
	*d = 0;

	// text length
	return d - buf;
}

// convert number from ASCIIZ text (optional sign, optional decimal point) - no overflow checking
// Returns length of parsed text.
int fix::FromText(const char* buf)
{
	const char* buf0 = buf;
	Bool neg = False;
	char ch;
	for (;;)
	{
		ch = *buf;
		if (ch == '-')
			neg = !neg;
		else if ((ch != '+') && (ch != ' ') && (ch != 9))
			break;
		buf++;
	}

	// integer part - simple accumulation into a plain u64
	u64 intpart = 0;
	while ((*buf >= '0') && (*buf <= '9'))
	{
		intpart = intpart*10 + (u64)(*buf - '0');
		buf++;
	}

	// clear temporary buffer
	int n = FIX_SEGNUM;
	memset(this->m_data, 0, n*sizeof(FIX_BASE));

	// skip decimal point and load fractional part
	if (*buf == '.')
	{
		buf++;

		// find the range of the fraction digits
		const char* fstart = buf;
		while ((*buf >= '0') && (*buf <= '9')) buf++;
		const char* fend = buf;

		const u64 Q0 = 1844674407370955161ULL;	// = 2^64 / 10 ... 1 844 674 407 370 955 161,6 = 1999 9999 9999 9999
		const u64 R0 = 6;			// = 2^64 % 10

		// process fraction digits from the last one to the first one
		const char* p = fend;
		while (p > fstart)
		{
			p--;
			u64 digit = (u64)(*p - '0');	// get digit

			// insert the digit directly at its final position - the lowest bit of the
			// integer field, right above the fraction's top bit. This position is
			// guaranteed to be 0 here, because the value built so far is always < 1.
			this->m_data[0] |= digit << (FIX_BITS - FIX_INTBITS);

			// divide the whole array (with 'carry' added above it) by 10, word by word,
			// most significant word first
			u64 carry = 0;
			int i;
			for (i = 0; i < n; i++)
			{
				u64 rem1;
				u64 q1 = Div64By10(this->m_data[i], &rem1);

				// combine with carry-in digit (0..9) from the more significant word
				u64 t = carry*R0 + rem1;	// t is at most 9*6+9 = 63
				u64 extra = t / 10;		// tiny range (0..6), cheap even without HW divider
				digit = t - extra*10;

				this->m_data[i] = carry*Q0 + q1 + extra; // proven to never overflow u64
				carry = digit;				// remainder (0..9) passed to next word
			}
			// remainder left after the last word is beyond available precision - discarded
		}
	}

	// integer part goes into the same top field, now guaranteed free (fraction built above is < 1)
	this->m_data[0] |= intpart << (FIX_BITS - FIX_INTBITS);

	this->m_zero = this->MantIsZero();
	this->m_neg = (this->m_zero) ? False : neg;

	// length of parsed text
	return buf - buf0;
}
