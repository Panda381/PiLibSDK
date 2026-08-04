
// ****************************************************************************
//
//                       Fixed point number, 32-bit segments
//
// ****************************************************************************

#include "../include.h"

// scale constant of 1st segment m_data[0] (= ldexp(1.0, -(FIX_BITS - FIX_INTBITS)))
const double FixScaleConst0 = 1.0 / (1 << (FIX_BITS - FIX_INTBITS));

// scale constant of 2nd segment m_data[0] (= ldexp(FixScaleConst0, -FIX_BITS))
const double FixScaleConst1 = 1.0 / (1 << (FIX_BITS - FIX_INTBITS)) / (1ull << 32);

// scale constant of 3rd segment m_data[0] (= ldexp(FixScaleConst1, -FIX_BITS))
const double FixScaleConst2 = 1.0 / (1 << (FIX_BITS - FIX_INTBITS)) / (1ull << 32) / (1ull << 32);

// temporary buffer for multiply
u32 FixTempBuf[2*FIX_SEGNUM];

/*
// calculate number of u32 segments needed for 'fix' numbers, from the current view size
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

	// round up to whole u32 segments
	int seg = (bits + FIX_BITS - 1) / FIX_BITS;
	if (seg < 1) seg = 1;
	if (seg > FIX_SEGNUM) seg = FIX_SEGNUM;

	return seg;
}
*/

/*
// calculate number of u32 segments needed for 'fix' numbers, from the current view size,
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

	// round up to whole u32 segments
	int seg = (bits + FIX_BITS - 1) / FIX_BITS;
	if (seg < 1) seg = 1;
	if (seg > FIX_SEGNUM) seg = FIX_SEGNUM;

	return seg;
}
*/

// calculate number of decimal digits needed to display a number with 'seg' segments via fix::ToText()
int FixSegToDig(int seg)
{
	if (seg < 1) seg = 1;
	int fracbits = FIX_BITS*seg - FIX_INTBITS;
	if (fracbits < 0) fracbits = 0;

	int dig = (int)(fracbits*0.30103) + 1;	// log10(2) = 0.30103
	return dig;
}

// calculate minimum number of u32 segments needed to represent a number with 'dig' decimal digits
int DigToFixSeg(int dig)
{
	if (dig < 1) dig = 1;

	// smallest fracbits such that floor(fracbits*log10(2))+1 >= dig
	int fracbits = (int)ceil((dig-1)/0.30103);
	if (fracbits < 0) fracbits = 0;

	int bits = fracbits + FIX_INTBITS;
	int seg = (bits + FIX_BITS - 1) / FIX_BITS;
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
	u32 carry = 1;
	for (i = FIX_SEGNUM-1; i >= 0; i--)
	{
		u32 k = ~this->m_data[i];
		u64 sum = (u64)k + carry;
		carry = (u32)(sum >> 32);
		this->m_data[i] = sum;
	}
}

// internal function - raw add two mantissas to this mantissa, without sign corrections
void fix::MantAdd(const fix* a, const fix* b)
{
	int i = FIX_SEGNUM-1;
	u32* d = this->m_data;
	const u32* s1 = a->m_data;
	const u32* s2 = b->m_data;
	u64 carry = 0;

	for (; i >= 0; i--)
	{
		u64 k = carry + (u64)s1[i] + (u64)s2[i];
		carry = k >> 32;
		d[i] = (u32)k;
	}
}

// internal function - raw subtract two mantissas to this mantissa, without sign corrections (returns True if result is zero)
Bool fix::MantSub(const fix* a, const fix* b)
{
	int i = FIX_SEGNUM-1;
	u32* d = this->m_data;
	const u32* s1 = a->m_data;
	const u32* s2 = b->m_data;
	u64 carry = 0;
	u32 sum = 0;

	for (; i >= 0; i--)
	{
		u64 k = carry + (u64)s1[i] - (u64)s2[i];
		carry = k >> 32;
		d[i] = (u32)k;
		sum += (u32)k;
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
	this->m_data[0] = 1 << (FIX_BITS - FIX_INTBITS);
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
	const u32* d1 = this->m_data;
	const u32* d2 = num->m_data;
	n = FIX_SEGNUM;
	i = 0;
	if (!num->m_neg)
	{
		for (; i < n; i++)
		{
			u32 a = d1[i];
			u32 b = d2[i];
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
		for (; i < n; i++)
		{
			u32 a = d1[i];
			u32 b = d2[i];
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
// propagating carry into higher words (array has 2*FIX_SEGNUM words)
static void MantMulAddCarry(int pos, u32 val)
{
	u32* arr = FixTempBuf;
	int size = 2*FIX_SEGNUM;
	u64 sum = (u64)arr[pos] + val;
	u64 carry = sum >> 32;
	arr[pos] = (u32)sum;
	pos++;
	while ((carry != 0) && (pos < size))
	{
		sum = (u64)arr[pos] + carry;
		arr[pos] = (u32)sum;
		carry = sum >> 32;
		pos++;
	}
}

// internal function - shift down raw double-wide product (2*FIX_SEGNUM words, little-endian array)
// by the fractional-point offset and store the result as this mantissa
void fix::MantMulFinish()
{
	u32* prod = FixTempBuf;
	int n = FIX_SEGNUM;
	int wordshift = n - 1;
	int bitshift = FIX_BITS - FIX_INTBITS;

	int i;
	for (i = 0; i < n; i++)
	{
		u32 lo = prod[wordshift+i] >> bitshift;
		u32 hi = prod[wordshift+i+1] << FIX_INTBITS;
		this->m_data[n-1-i] = lo | hi;
	}
}

// internal function - raw multiply two mantissas, result (unsigned magnitude) into this mantissa
void fix::MantMul(const fix* a, const fix* b)
{
	int n = FIX_SEGNUM;
	u32* prod = FixTempBuf;
	memset(prod, 0, 2*n*sizeof(FIX_BASE));

	int i, j;
	for (i = 0; i < n; i++)
	{
		u32 ai = a->m_data[n-1-i];		// little-endian word 'i' of 'a'
		if (ai != 0)
		{
			for (j = 0; j < n; j++)
			{
				u32 bj = b->m_data[n-1-j];	// little-endian word 'j' of 'b'
				if (bj != 0)
				{
					u64 res = Mul32(ai, bj);
					MantMulAddCarry(i+j, (u32)res);
					MantMulAddCarry(i+j+1, (u32)(res >> 32));
				}
			}
		}
	}

	// this->m_data is only written here, after all reads of a/b are done,
	// so it is safe even if 'this' aliases 'a' or 'b'
	this->MantMulFinish();
}

// internal function - raw square mantissa of 'a', result (unsigned magnitude) into this mantissa
void fix::MantSqr(const fix* a)
{
	int n = FIX_SEGNUM;
	u32* prod = FixTempBuf;
	memset(prod, 0, 2*n*sizeof(FIX_BASE));

	int i, j;

	// diagonal terms: ai*ai
	for (i = 0; i < n; i++)
	{
		u32 ai = a->m_data[n-1-i];
		if (ai != 0)
		{
			u64 res = Sqr32(ai);
			MantMulAddCarry(2*i, (u32)res);
			MantMulAddCarry(2*i+1, (u32)(res >> 32));
		}
	}

	// off-diagonal terms: ai*aj (i != j) - each pair appears twice, add it in twice instead
	for (i = 0; i < n; i++)
	{
		u32 ai = a->m_data[n-1-i];
		if (ai != 0)
		{
			for (j = i+1; j < n; j++)
			{
				u32 aj = a->m_data[n-1-j];
				if (aj != 0)
				{
					u64 res = Mul32(ai, aj);
					MantMulAddCarry(i+j, (u32)res);
					MantMulAddCarry(i+j, (u32)res);	// added twice (symmetric term)
					res >>= 32;
					MantMulAddCarry(i+j+1, (u32)res);
					MantMulAddCarry(i+j+1, (u32)res);
				}
			}
		}
	}

	this->MantMulFinish();
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
	this->m_data[0] = (u32)(s32)(num * (1 << (FIX_BITS - FIX_INTBITS)));
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
		c *= 1.0 / (1ull << 32);
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
	u32* d = this->m_data;
	const u32* s = a->m_data;
	int carry = 0;
	int carry2;

	for (; i >= 0; i--)
	{
		u32 k = s[i];
		carry2 = (s32)k < 0;
		d[i] = (k << 1) | carry;
		carry = carry2;
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

	int n = FIX_SEGNUM;
	u32* d = this->m_data;
	const u32* s = a->m_data;
	u32 carry = 0;
	u32 carry2;
	u32 sum = 0;

	int i = 0;
	for (; i < n; i++)
	{
		u32 k = s[i];
		carry2 = k << 31;
		k = (k >> 1) | carry;
		d[i] = k;
		sum |= k;
		carry = carry2;
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
	u32 mult = (u32)(negnum ? (-(s32)num) : (s32)num);

	// multiply
	u64 carry = 0;
	u32 sum = 0;
	int i = FIX_SEGNUM - 1;
	for (; i >= 0; i--)
	{
		u64 res = Mul32(this->m_data[i], mult);
		res += carry;
		carry = res >> 32;
		sum |= (u32)res; // to check zero result
		this->m_data[i] = (u32)res;
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
	u32 div = (u32)(negnum ? (-(s32)num) : (s32)num);

	// divide
	u64 rem = 0;
	u32 sum = 0;
	int n = FIX_SEGNUM;
	int i;
	for (i = 0; i < n; i++)
	{
		u64 cur = (rem << 32) | this->m_data[i];
		u32 q = (u32)(cur / div);
		rem = cur - (u64)q*div;
		sum |= q; // to check zero result
		this->m_data[i] = q;
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
	char* d = buf;
	char* dmax = buf + buflen - 1;	// reserve place for terminating zero

	// sign
	if (this->m_neg)
	{
		if (d < dmax) *d++ = '-';
	}

	// integer part fits into a plain u32 (top FIX_INTBITS bits of m_data[0])
	u32 intpart = this->m_data[0] >> (FIX_BITS - FIX_INTBITS);

	// convert integer part by repeated division by 10, into a small temporary buffer
	char tmp[16];
	int tn = 0;
	do {
		u32 intpart2 = intpart/10;
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
	u32* frac = FixTempBuf; // temporary buffer
	memcpy(frac, this->m_data, n*sizeof(FIX_BASE));

	// number of decimal digits needed to cover the fraction precision, derived from FIX_SEGNUM
	int fracdigits = FixSegToDig(n);

	// generate fraction digits by repeated multiplication of the whole array by 10;
	// the overflow to integer part is the next decimal digit
	int i, k;
	for (k = 0; k < fracdigits; k++)
	{
		// output buffer is full
		if (d >= dmax) break;

		// clear integer part
		frac[0] &= (u32)~0 >> FIX_INTBITS;

		// multiply number * 10
		u64 carry = 0;
		for (i = n-1; i >= 0; i--)
		{
			u64 res = Mul32(frac[i], 10);
			res += carry;
			carry = res >> 32;
			frac[i] = (u32)res;
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
void int::FromText(const char* buf)
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

	// integer part - simple accumulation into a plain u32
	u32 intpart = 0;
	while ((*buf >= '0') && (*buf <= '9'))
	{
		intpart = intpart*10 + (u32)(*buf - '0');
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

		const u32 Q0 = 429496729;		// = 2^32 / 10 ... 429 496 729,6 = 1999 9999
		const u32 R0 = 6;			// = 2^32 % 10

		// process fraction digits from the last one to the first one
		const char* p = fend;
		while (p > fstart)
		{
			p--;
			u32 digit = (u32)(*p - '0');	// get digit

			// insert the digit directly at its final position - the lowest bit of the
			// integer field, right above the fraction's top bit. This position is
			// guaranteed to be 0 here, because the value built so far is always < 1.
			this->m_data[0] |= digit << (FIX_BITS - FIX_INTBITS);

			// divide the whole array (with 'carry' added above it) by 10, word by word,
			// most significant word first
			u32 carry = 0;
			int i;
			for (i = 0; i < n; i++)
			{
				u32 rem1;
				u32 q1 = Div32By10(this->m_data[i], &rem1);

				// combine with carry-in digit (0..9) from the more significant word
				u32 t = carry*R0 + rem1;
				u32 extra = t / 10;		// tiny range (0..6), cheap even without HW divider
				digit = t - extra*10;

				this->m_data[i] = carry*Q0 + q1 + extra; // proven to never overflow u32
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
