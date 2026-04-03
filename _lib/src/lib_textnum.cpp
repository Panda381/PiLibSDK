
// ****************************************************************************
//
//                        Text strings - numeric conversions
//
// ****************************************************************************

#include "../../includes.h"	// includes

#if USE_TEXT		// 1=use text strings, 0=not used (lib_text*)

// day of week name, 2 characters
const char* DayOfWeekName2[7] = { "Su", "Mo", "Tu", "We", "Th", "Fr", "Sa" };

// day of week name, 3 characters
const char* DayOfWeekName3[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

// day of week name, long
const char* DayOfWeekNameLong[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

// month name, 3 characters
const char* MonthName3[12] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

// month name, long
const char* MonthNameLong[12] = { "January", "February", "March", "April", "May", "June",
	"July", "August", "September", "October", "November", "December" };

// pre-set date formats
//    d ... day 1 or 2 digits
//    D ... day 2 digits
//    m ... month 1 or 2 digits
//    M ... month 2 digits
//    n ... month name 3 letters
//    N ... month name full
//    y ... year 2 digits
//    Y ... year 4 digits
const char* DateForm[13] = {
	// USA format
	"M/D/y",	// 0 (01/25/22)
	"m/d/Y",	// 1 (1/25/2022)
	"M/D/Y",	// 2 (01/25/2022)
	"n d,Y",	// 3 (Jan 25,2022)
	"N d,Y",	// 4 (January 25,2022)
	// European format
	"D.M.y",	// 5 (25.01.22)
	"d.m.Y",	// 6 (25.1.2022)
	"D.M.Y",	// 7 (25.01.2022)
	"d. n Y",	// 8 (25. Jan 2022)
	"d. N Y",	// 9 (25. January 2022)
	// ISO format
	"y-M-D",	// 10 (22-01-25)
	"Y-M-D",	// 11 (2022-01-25)
	// Technical format
	"yMD",		// 12 (220125)
};

// round coefficients, used in SetAddFloat function
const float SetFloatCoeff[11] = { 5e0f, 5e-1f, 5e-2f, 5e-3f, 5e-4f, 5e-5f, 5e-6f,
	5e-7f, 5e-8f, 5e-9f, 5e-10f };

// round coefficients, used in SetAddDouble function
const double SetDoubleCoeff[19] = { 5e0, 5e-1, 5e-2, 5e-3, 5e-4, 5e-5, 5e-6, 5e-7,
	5e-8, 5e-9, 5e-10, 5e-11, 5e-12, 5e-13, 5e-14, 5e-15, 5e-16 };

// convert float to number
typedef union { float f; u32 n; } float_u32;
INLINE u32 FloatToU32(float f) { float_u32 tmp; tmp.f = f; return tmp.n; }

typedef union { double f; u64 n; } double_u64;
INLINE u64 DoubleToU64(double f) { double_u64 tmp; tmp.f = f; return tmp.n; }

// check if float number is invalid number (#INF or #NaN)
Bool FloatIsOver(float num)
{
	return ((FloatToU32(num) >> 23) & 0xff) == 0xff;
}

// check if double number is invalid number (#INF or #NaN)
Bool DoubleIsOver(double num)
{
	return ((DoubleToU64(num) >> 52) & 0x7ff) == 0x7ff;
}

// convert ASCIIZ text to unsigned INT number
//  - can be in HEX format 0xN or $N
//  - can be in BIN format 0bN or %N
//  - limited to min. and max. value
//  - optionally can return new text pointer (if "newtext" is not NULL)
u32 StrToUInt(const char* text, const char** newtext /*= NULL*/)
{
	u32 res = 0;
	char ch;
	Bool hex, bin;
	Bool sign = False;

	// NULL pointer
	if (text == NULL) goto TOUINT_RES; // no text

	// get sign and skip spaces, load first valid character
	for (;;)
	{
		// load next character
		ch = *text++;
		if (ch == 0) goto TOUINT_RES; // end of text

		// negative sign
		if (ch == '-')
			sign = !sign;
		else
		{
			// skip positive sign or whitespace
			if ((ch != '+') && ((uint)ch > (uint)' ')) break;
		}
	}

	// check HEX format
	hex = False;
	if (ch == '$')
	{
		ch = *text++;
		hex = True;
	}
	else
	{
		if ((ch == '0') && ((*text == 'x') || (*text == 'X')))
		{
			text++;
			ch = *text++;
			hex = True;
		}
	}

	// check BIN format
	bin = False;
	if (ch == '%')
	{
		ch = *text++;
		bin = True;
	}
	else
	{
		if ((ch == '0') && ((*text == 'b') || (*text == 'B')))
		{
			text++;
			ch = *text++;
			bin = True;
		}
	}

	// load HEX digits
	if (hex)
	{
		for (;;)
		{
			// convert to upper case letter
			if ((ch >= 'a') && (ch <= 'f')) ch -= 'a' - 'A';

			// invalid character
			if (((ch < '0') || (ch > '9')) &&
				((ch < 'A') || (ch > 'F'))) break;

			// check overflow
			if (res > 0x0fffffff)
			{
				res = 0xffffffff;
			}
			else
			{
				// add digit
				ch = ch - '0';
				if (ch >= 10) ch -= 7;
				res <<= 4;
				res |= ch;
			}

			// load next character
			ch = *text++;
			if (ch == 0) break;
		}
	}

	// load BIN digits
	else if (bin)
	{
		for (;;)
		{
			// invalid character
			if ((ch < '0') || (ch > '1')) break;

			// check overflow
			if (res > 0x7fffffff)
			{
				res = 0xffffffff;
			}
			else
			{
				// add digit
				ch = ch - '0';
				res <<= 1;
				res |= ch;
			}

			// load next character
			ch = *text++;
			if (ch == 0) break;
		}
	}

	// load DEC digits
	else
	{	
		// load decimal digits
		while ((ch >= '0') && (ch <= '9')) // digit?
		{
			ch -= '0';

			// check overflow
			if (res > 0x19999999) // 429496729 = 0xffffffff/10
			{
				res = 0xffffffff;
			}
			else
			{
				// add digit
				res *= 10; // multiply accumulator
				res += ch; // add digit

				// overflow
				if ((u32)res < (u32)ch) res = 0xffffffff;
			}

			// load next character
			ch = *text++;
			if (ch == 0) break;
		}
	}

TOUINT_RES:

	// limit negative number
	if (sign) res = 0;

	// return result
	if (newtext != NULL) *newtext = text;
	return res;
}

// convert ASCIIZ text to signed INT number
//  - can be in HEX format 0xN or $N
//  - can be in BIN format 0bN or %N
//  - limited to min. and max. value
//  - optionally can return new text pointer (if "newtext" is not NULL)
s32 StrToInt(const char* text, const char** newtext /*= NULL*/)
{
	u32 res;
	char ch;
	Bool sign = False;

	// NULL pointer
	if (text == NULL)
	{
		if (newtext != NULL) *newtext = text;
		return 0;
	}

	// get sign and skip spaces, load first valid character
	for (;;)
	{
		// load next character
		ch = *text;
		if (ch == 0) // end of text
		{
			if (newtext != NULL) *newtext = text;
			return 0;
		}

		// negative sign
		if (ch == '-')
		{
			sign = !sign;
		}
		else
		{
			// skip positive sign or whitespace
			if ((ch != '+') && ((uint)ch > (uint)' ')) break;
		}
		text++;
	}

	// load unsigned number
	res = StrToUInt(text, newtext);

	// limit result
	if (sign)
	{
		// negative number
		if (res > 0x80000000) res = 0x80000000;
		return -(s32)res;
	}
	else
	{
		// positive number
		if (res > 0x7fffffff) res = 0x7fffffff;
		return res;
	}
}

// convert ASCIIZ text to 64-bit unsigned INT number
//  - can be in HEX format 0xN or $N
//  - can be in BIN format 0bN or %N
//  - limited to min. and max. value
//  - optionally can return new text pointer (if "newtext" is not NULL)
u64 StrToUInt64(const char* text, const char** newtext /*= NULL*/)
{
	u64 res = 0;
	char ch;
	Bool hex, bin;
	Bool sign = False;

	// NULL pointer
	if (text == NULL) goto TOUINT64_RES; // no text

	// get sign and skip spaces, load first valid character
	for (;;)
	{
		// load next character
		ch = *text++;
		if (ch == 0) goto TOUINT64_RES; // end of text

		// negative sign
		if (ch == '-')
			sign = !sign;
		else
		{
			// skip positive sign or whitespace
			if ((ch != '+') && ((uint)ch > (uint)' ')) break;
		}
	}

	// check HEX format
	hex = False;
	if (ch == '$')
	{
		ch = *text++;
		hex = True;
	}
	else
	{
		if ((ch == '0') && ((*text == 'x') || (*text == 'X')))
		{
			text++;
			ch = *text++;
			hex = True;
		}
	}

	// check BIN format
	bin = False;
	if (ch == '%')
	{
		ch = *text++;
		bin = True;
	}
	else
	{
		if ((ch == '0') && ((*text == 'b') || (*text == 'B')))
		{
			text++;
			ch = *text++;
			bin = True;
		}
	}

	// load HEX digits
	if (hex)
	{
		for (;;)
		{
			// convert to upper case letter
			if ((ch >= 'a') && (ch <= 'f')) ch -= 'a' - 'A';

			// invalid character
			if (((ch < '0') || (ch > '9')) &&
				((ch < 'A') || (ch > 'F'))) break;

			// check overflow
			if (res > 0x0fffffffffffffffull)
			{
				res = 0xffffffffffffffffull;
			}
			else
			{
				// add digit
				ch = ch - '0';
				if (ch >= 10) ch -= 7;
				res <<= 4;
				res |= ch;
			}

			// load next character
			ch = *text++;
			if (ch == 0) break;
		}
	}

	// load BIN digits
	else if (bin)
	{
		for (;;)
		{
			// invalid character
			if ((ch < '0') || (ch > '1')) break;

			// check overflow
			if (res > 0x7fffffffffffffffull)
			{
				res = 0xffffffffffffffffull;
			}
			else
			{
				// add digit
				ch = ch - '0';
				res <<= 1;
				res |= ch;
			}

			// load next character
			ch = *text++;
			if (ch == 0) break;
		}
	}

	// load DEC digits
	else
	{	
		// load decimal digits
		while ((ch >= '0') && (ch <= '9')) // digit?
		{
			ch -= '0';

			// check overflow
			if (res > 0x1999999999999999ull) // 1844674407370955161 = 0xffffffffffffffff/10
			{
				res = 0xffffffffffffffffull;
			}
			else
			{
				// add digit
				res *= 10; // multiply accumulator
				res += ch; // add digit

				// overflow
				if ((u64)res < (u64)ch) res = 0xffffffffffffffffull;
			}

			// load next character
			ch = *text++;
			if (ch == 0) break;
		}
	}

TOUINT64_RES:

	// limit negative number
	if (sign) res = 0;

	// return result
	if (newtext != NULL) *newtext = text;
	return res;
}

// convert ASCIIZ text to 64-bit signed INT number
//  - can be in HEX format 0xN or $N
//  - can be in BIN format 0bN or %N
//  - limited to min. and max. value
//  - optionally can return new text pointer (if "newtext" is not NULL)
s64 StrToInt64(const char* text, const char** newtext /*= NULL*/)
{
	u64 res;
	char ch;
	Bool sign = False;

	// NULL pointer
	if (text == NULL)
	{
		if (newtext != NULL) *newtext = text;
		return 0;
	}

	// get sign and skip spaces, load first valid character
	for (;;)
	{
		// load next character
		ch = *text;
		if (ch == 0) // end of text
		{
			if (newtext != NULL) *newtext = text;
			return 0;
		}

		// negative sign
		if (ch == '-')
		{
			sign = !sign;
		}
		else
		{
			// skip positive sign or whitespace
			if ((ch != '+') && ((uint)ch > (uint)' ')) break;
		}
		text++;
	}

	// load unsigned number
	res = StrToUInt64(text, newtext);

	// limit result
	if (sign)
	{
		// negative number
		if (res > 0x8000000000000000ull) res = 0x8000000000000000ull;
		return -(s64)res;
	}
	else
	{
		// positive number
		if (res > 0x7fffffffffffffffull) res = 0x7fffffffffffffffull;
		return res;
	}
}

// convert ASCIIZ text to float number
//  - does not check overflows
//  - optionally can return new text pointer (if "newtext" is not NULL)
float StrToFloat(const char* text, const char** newtext /*= NULL*/)
{
	// get sign and skip spaces
	char ch;
	Bool sign = False;
	for (;;)
	{
		// load next character
		ch = *text++;
		if (ch == 0)
		{
			if (newtext != NULL) *newtext = text;
			return 0;
		}

		// negative sign
		if (ch == '-')
			sign = !sign;
		else
			// skip positive sign or whitespace
			if ((ch != '+') && ((uint)ch > (uint)' ')) break;
	}

	// load digits of integer part
	float res = 0;
	while ((ch >= '0') && (ch <= '9'))
	{
		res *= 10;
		res += ch - '0';

		// load next character
		ch = *text++;
		if (ch == 0)
		{
			text--;
			break;
		}
	}

	// load digits of fractional part
	float frac = 1;
	if ((ch == '.') || (ch == ','))
	{
		// load next character
		ch = *text;
		if (ch != 0)
		{
			text++;

			while ((ch >= '0') && (ch <= '9'))
			{
				frac *= 10;
				res += (ch - '0')/frac;

				// load next character
				ch = *text++;
				if (ch == 0)
				{
					text--;
					break;
				}
			}
		}
	}

	// check exponent
	int expN = 0;
	Bool signE = False;
	if ((ch == 'E') || (ch == 'e'))
	{
		// get sign and skip spaces
		for (;;)
		{
			// load next character
			ch = *text++;
			if (ch == 0)
			{
				text--;
				break;
			}

			if (ch == '-')
				signE = !signE;
			else
				if (ch != '+') break;
		}

		// load exponent digits
		while ((ch >= '0') && (ch <= '9'))
		{
			expN *= 10;
			expN += ch - '0';

			// load next character
			ch = *text++;
			if (ch == 0)
			{
				text--;
				break;
			}
		}

		// multiply number by exponent
		if (expN != 0)
		{
			if (signE)
			{
				if (expN >=  32) { res *=  1e-32f; expN -=  32; };
				if (expN >=  16) { res *=  1e-16f; expN -=  16; };
				if (expN >=   8) { res *=   1e-8f; expN -=   8; };
				if (expN >=   4) { res *=   1e-4f; expN -=   4; };
				if (expN >=   2) { res *=   1e-2f; expN -=   2; };
				if (expN >=   1) { res *=   1e-1f;              };
			}
			else
			{
				if (expN >=  32) { res *=  1e32f; expN -=  32; };
				if (expN >=  16) { res *=  1e16f; expN -=  16; };
				if (expN >=   8) { res *=   1e8f; expN -=   8; };
				if (expN >=   4) { res *=   1e4f; expN -=   4; };
				if (expN >=   2) { res *=   1e2f; expN -=   2; };
				if (expN >=   1) { res *=   1e1f;              };
			}
		}
	}

	// result pointer
	if (newtext != NULL) *newtext = text;

	// result with sign
	if (sign) return -res;
	return res;
}

// convert ASCIIZ text to double number
//  - does not check overflows
//  - optionally can return new text pointer (if "newtext" is not NULL)
double StrToDouble(const char* text, const char** newtext /*= NULL*/)
{
	// get sign and skip spaces
	char ch;
	Bool sign = False;
	for (;;)
	{
		// load next character
		ch = *text++;
		if (ch == 0)
		{
			if (newtext != NULL) *newtext = text;
			return 0;
		}

		// negative sign
		if (ch == '-')
			sign = !sign;
		else
			// skip positive sign or whitespace
			if ((ch != '+') && ((uint)ch > (uint)' ')) break;
	}

	// load digits of integer part
	double res = 0;
	while ((ch >= '0') && (ch <= '9'))
	{
		res *= 10;
		res += ch - '0';

		// load next character
		ch = *text++;
		if (ch == 0)
		{
			text--;
			break;
		}
	}

	// load digits of fractional part
	double frac = 1;
	if ((ch == '.') || (ch == ','))
	{
		// load next character
		ch = *text;
		if (ch != 0)
		{
			text++;

			while ((ch >= '0') && (ch <= '9'))
			{
				frac *= 10;
				res += (ch - '0')/frac;

				// load next character
				ch = *text++;
				if (ch == 0)
				{
					text--;
					break;
				}
			}
		}
	}

	// check exponent
	int expN = 0;
	Bool signE = False;
	if ((ch == 'E') || (ch == 'e'))
	{
		// get sign and skip spaces
		for (;;)
		{
			// load next character
			ch = *text++;
			if (ch == 0)
			{
				text--;
				break;
			}

			if (ch == '-')
				signE = !signE;
			else
				if (ch != '+') break;
		}

		// load exponent digits
		while ((ch >= '0') && (ch <= '9'))
		{
			expN *= 10;
			expN += ch - '0';

			// load next character
			ch = *text++;
			if (ch == 0)
			{
				text--;
				break;
			}
		}

		// multiply number by exponent
		if (expN != 0)
		{
			if (signE)
			{
				if (expN >= 256) { res *= 1e-256; expN -= 256; };
				if (expN >= 128) { res *= 1e-128; expN -= 128; };
				if (expN >=  64) { res *=  1e-64; expN -=  64; };
				if (expN >=  32) { res *=  1e-32; expN -=  32; };
				if (expN >=  16) { res *=  1e-16; expN -=  16; };
				if (expN >=   8) { res *=   1e-8; expN -=   8; };
				if (expN >=   4) { res *=   1e-4; expN -=   4; };
				if (expN >=   2) { res *=   1e-2; expN -=   2; };
				if (expN >=   1) { res *=   1e-1;              };
			}
			else
			{
				if (expN >= 256) { res *= 1e256; expN -= 256; };
				if (expN >= 128) { res *= 1e128; expN -= 128; };
				if (expN >=  64) { res *=  1e64; expN -=  64; };
				if (expN >=  32) { res *=  1e32; expN -=  32; };
				if (expN >=  16) { res *=  1e16; expN -=  16; };
				if (expN >=   8) { res *=   1e8; expN -=   8; };
				if (expN >=   4) { res *=   1e4; expN -=   4; };
				if (expN >=   2) { res *=   1e2; expN -=   2; };
				if (expN >=   1) { res *=   1e1;              };
			}
		}
	}

	// result pointer
	if (newtext != NULL) *newtext = text;

	// result with sign
	if (sign) return -res;
	return res;
}

// set/add signed/unsigned 32-bit integer number to text (returns False on memory error)
//  num ... number (signed or unsigned)
//  add ... True=add number, False=set number
//  unsign ... True=unsigned, False=signed
//  sep ... thousand separator or 0=none
Bool string::SetAddInt(s32 num, Bool add, Bool unsign, char sep /*= 0*/)
{
	// prepare temporary buffer
	char buf[14]; // temporary buffer: 10 digits, 3 separators, 1 NUL
	char* dst = buf+14; // pointer to end of temporary buffer
	int len = 0; // length of the text
	u32 n = num; // number as unsigned
	u32 k;

	// prepare sign
	Bool sign = False;
	if (!unsign && (num < 0))
	{
		sign = True;
		n = -num;
	}

	// convert digits to the buffer
	int sepn = 0;
	do {
		// thousand separator
		if ((sepn == 3) && (sep != 0))
		{
			sepn = 0;
			*--dst = sep;
			len++;
		}
		sepn++;

		// decode digit
		k = n / 10;
		*--dst = (char)((n - k*10) + '0');
		n = k;
		len++;
	} while (n != 0);

	// store sign
	if (sign)
	{
		*--dst = '-';
		len++;
	}

	// copy number into buffer
	int len0; // destination offset
	if (add)
	{
		// add number to destination text
		len0 = this->m_Data->len; // current length of destination text
		if (!this->SetLength(len0 + len)) return False; // set new length of the text
	}
	else
	{
		// set number to destinaton text
		len0 = 0; // destination offset = 0
		this->Detach(); // detach old text
		if (!this->NewBuffer(len)) return False; // create new data
	}

	// copy number to destination buffer
	memcpy(this->m_Data->str + len0, dst, len);
	return True;
}

// set/add signed/unsigned 64-bit integer number to text (returns False on memory error)
//  num ... number (signed or unsigned)
//  add ... True=add number, False=set number
//  unsign ... True=unsigned, False=signed
//  sep ... thousand separator or 0=none
Bool string::SetAddInt64(s64 num, Bool add, Bool unsign, char sep /*= 0*/)
{
	// prepare temporary buffer
	char buf[28]; // temporary buffer: 20 digits, 6 separators, 1 NUL
	char* dst = buf+28; // pointer to end of temporary buffer
	int len = 0; // length of the text
	u64 n = num; // number as unsigned
	u64 k;

	// prepare sign
	Bool sign = False;
	if (!unsign && (num < 0))
	{
		sign = True;
		n = -num;
	}

	// convert digits to the buffer
	int sepn = 0;
	do {
		// thousand separator
		if ((sepn == 3) && (sep != 0))
		{
			sepn = 0;
			*--dst = sep;
			len++;
		}
		sepn++;

		// decode digit
		k = n / 10;
		*--dst = (char)((n - k*10) + '0');
		n = k;
		len++;
	} while (n != 0);

	// store sign
	if (sign)
	{
		*--dst = '-';
		len++;
	}

	// copy number into buffer
	int len0; // destination offset
	if (add)
	{
		// add number to destination text
		len0 = this->m_Data->len; // current length of destination text
		if (!this->SetLength(len0 + len)) return False; // set new length of the text
	}
	else
	{
		// set number to destinaton text
		len0 = 0; // destination offset = 0
		this->Detach(); // detach old text
		if (!this->NewBuffer(len)) return False; // create new data
	}

	// copy number to destination buffer
	memcpy(this->m_Data->str + len0, dst, len);
	return True;
}

// set/add 2 digits to text (returns False on memory error)
//  num ... number 0..99
//  add ... True=add number, False=set number
Bool string::SetAdd2Dig(int num, Bool add)
{
	s32 len;

	// limit range of the number
	if (num < 0) num = 0;
	if (num > 99) num = 99;

	if (add)
	{
		// add number
		len = this->m_Data->len; // destination offset
		if (!this->SetLength(len + 2)) return False; // set new length of the text
	}
	else
	{
		// set number
		len = 0; // destination offset = 0
		this->Detach(); // detach old text
		if (!this->NewBuffer(2)) return False;
	}

	// decode number to the buffer
	int n = num/10;
	this->m_Data->str[len] = (char)(n + '0');
	this->m_Data->str[len+1] = (char)((num - n*10) + '0');
	return True;
}

// set/add 2 digits with space to text (returns False on memory error)
//  num ... number 0..99
//  add ... True=add number, False=set number
Bool string::SetAdd2DigSpc(int num, Bool add)
{
	s32 len;

	// limit range of the number
	if (num < 0) num = 0;
	if (num > 99) num = 99;

	if (add)
	{
		// add number
		len = this->m_Data->len; // destination offset
		if (!this->SetLength(len + 2)) return False; // set new length of the text
	}
	else
	{
		// set number
		len = 0; // destination offset = 0
		this->Detach(); // detach old text
		if (!this->NewBuffer(2)) return False;
	}

	// decode number to the buffer
	int n = num/10;
	this->m_Data->str[len] = (num < 10) ? ' ' : (char)(n + '0');
	this->m_Data->str[len+1] = (char)((num - n*10) + '0');
	return True;
}

// set/add 4 digits to text (returns False on memory error)
//  num ... number 0..9999
//  add ... True=add number, False=set number
Bool string::SetAdd4Dig(int num, Bool add)
{
	s32 len;

	// limit range of the number
	if (num < 0) num = 0;
	if (num > 9999) num = 9999;

	if (add)
	{
		// add number
		len = this->m_Data->len; // destination offset
		if (!this->SetLength(len + 4)) return False; // set new length of the text
	}
	else
	{
		// set number
		len = 0; // destination offset = 0
		this->Detach(); // detach old text
		if (!this->NewBuffer(4)) return False;
	}

	// decode number to the buffer
	int n = num/100;
	int k = n/10;
	this->m_Data->str[len] = (char)(k + '0');
	this->m_Data->str[len+1] = (char)((n - k*10) + '0');
	n = num - n*100;
	k = n/10;
	this->m_Data->str[len+2] = (char)(k + '0');
	this->m_Data->str[len+3] = (char)((n - k*10) + '0');
	return True;
}

// set/add 32-bit integer to text as HEX (returns False on memory error)
//  num ... unsigned number
//  digits ... number of digits, or <= 0 to auto-digits
//  add ... True=add number, False=set number
//  sep ... 4-digit separator or 0=none
Bool string::SetAddHex(u32 num, int digits, Bool add, char sep /*= 0*/)
{
	s32 len;

	// prepare default number of digits
	// .... Order() = get bit order of value, = "32 - clz(val)" (logarithm, returns position of highest bit + 1: 1..32, 0=no bit)
	if (digits <= 0) digits = (Order(num)+3)/4;
	if (digits == 0) digits = 1;

	// prepare number of separators
	int sepn = 0;
	if (sep != 0) sepn = (digits - 1)/4;

	if (add)
	{
		// add number
		len = this->m_Data->len; // destination offset
		if (!this->SetLength(len + digits + sepn)) return False; // set new length of the text
	}
	else
	{
		// set number
		len = 0; // destination offset = 0
		this->Detach(); // detach old text
		if (!this->NewBuffer(digits + sepn)) return False;
	}

	// destination pointer to end of number
	char* d = &this->m_Data->str[len + digits + sepn];

	// decode digits
	char ch;
	sepn = 0;
	for (; digits > 0; digits--)
	{
		// thousand separator
		if ((sepn == 4) && (sep != 0))
		{
			sepn = 0;
			*--d = sep;
		}
		sepn++;

		// decode digit
		ch = (char)(num & 0xf);
		*--d = (char)((ch <= 9) ? (ch + '0') : (ch - 10 + 'A'));
		num >>= 4;
	}
	return True;
}

// set/add 64-bit integer to text as HEX (returns False on memory error)
//  num ... unsigned number
//  digits ... number of digits, or <= 0 to auto-digits
//  add ... True=add number, False=set number
//  sep ... 4-digit separator or 0=none
Bool string::SetAddHex64(u64 num, int digits, Bool add, char sep /*= 0*/)
{
	s32 len;

	// prepare default number of digits
	if (digits <= 0) digits = (Order64(num)+3)/4;
	if (digits == 0) digits = 1;

	// prepare number of separators
	int sepn = 0;
	if (sep != 0) sepn = (digits - 1)/4;

	if (add)
	{
		// add number
		len = this->m_Data->len; // destination offset
		if (!this->SetLength(len + digits + sepn)) return False; // set new length of the text
	}
	else
	{
		// set number
		len = 0; // destination offset = 0
		this->Detach(); // detach old text
		if (!this->NewBuffer(digits + sepn)) return False;
	}

	// destination pointer to end of number
	char* d = &this->m_Data->str[len + digits + sepn];

	// decode digits
	char ch;
	sepn = 0;
	for (; digits > 0; digits--)
	{
		// thousand separator
		if ((sepn == 4) && (sep != 0))
		{
			sepn = 0;
			*--d = sep;
		}
		sepn++;

		// decode digit
		ch = (char)(num & 0xf);
		*--d = (char)((ch <= 9) ? (ch + '0') : (ch - 10 + 'A'));
		num >>= 4;
	}
	return True;
}

// set/add 32-bit integer to text as BIN (returns False on memory error)
//  num ... unsigned number
//  digits ... number of digits, or <= 0 to auto-digits
//  add ... True=add number, False=set number
//  sep ... 4-digit separator or 0=none
Bool string::SetAddBin(u32 num, int digits, Bool add, char sep /*= 0*/)
{
	s32 len;

	// prepare default number of digits
	// .... Order() = get bit order of value, = "32 - clz(val)" (logarithm, returns position of highest bit + 1: 1..32, 0=no bit)
	if (digits <= 0) digits = Order(num);
	if (digits == 0) digits = 1;

	// prepare number of separators
	int sepn = 0;
	if (sep != 0) sepn = (digits - 1)/4;

	if (add)
	{
		// add number
		len = this->m_Data->len; // destination offset
		if (!this->SetLength(len + digits + sepn)) return False; // set new length of the text
	}
	else
	{
		// set number
		len = 0; // destination offset = 0
		this->Detach(); // detach old text
		if (!this->NewBuffer(digits + sepn)) return False;
	}

	// destination pointer to end of number
	char* d = &this->m_Data->str[len + digits + sepn];

	// decode digits
	sepn = 0;
	for (; digits > 0; digits--)
	{
		// thousand separator
		if ((sepn == 4) && (sep != 0))
		{
			sepn = 0;
			*--d = sep;
		}
		sepn++;

		// decode digit
		*--d = (char)((num & 1) + '0');
		num >>= 1;
	}
	return True;
}

// set/add 64-bit integer to text as BIN (returns False on memory error)
//  num ... unsigned number
//  digits ... number of digits, or <= 0 to auto-digits
//  add ... True=add number, False=set number
//  sep ... 4-digit separator or 0=none
Bool string::SetAddBin64(u64 num, int digits, Bool add, char sep /*= 0*/)
{
	s32 len;

	// prepare default number of digits
	if (digits <= 0) digits = Order64(num);
	if (digits == 0) digits = 1;

	// prepare number of separators
	int sepn = 0;
	if (sep != 0) sepn = (digits - 1)/4;

	if (add)
	{
		// add number
		len = this->m_Data->len; // destination offset
		if (!this->SetLength(len + digits + sepn)) return False; // set new length of the text
	}
	else
	{
		// set number
		len = 0; // destination offset = 0
		this->Detach(); // detach old text
		if (!this->NewBuffer(digits + sepn)) return False;
	}

	// destination pointer to end of number
	char* d = &this->m_Data->str[len + digits + sepn];

	// decode digits
	sepn = 0;
	for (; digits > 0; digits--)
	{
		// thousand separator
		if ((sepn == 4) && (sep != 0))
		{
			sepn = 0;
			*--d = sep;
		}
		sepn++;

		// decode digit
		*--d = (char)((num & 1) + '0');
		num >>= 1;
	}
	return True;
}

// set/add FLOAT number to text (returns False on error)
//  num ... number
//  digits ... number of digits 1..8 (recommended 6)
//  add ... True=add number, False=set number
Bool string::SetAddFloat(float num, Bool add, int digits /*= 6*/)
{
	// limit number of digits
	if (digits < 1) digits = 1;
	if (digits > 8) digits = 8;

	// overflow
	if (FloatIsOver(num))
	{
		if (!add) this->Empty();
		return this->Add((num < 0) ? "-INF" : "INF");
	}

	// zero
	if (num == 0)
	{
		if (!add) this->Empty();
		return this->Add((char)'0');
	}

	// prepare sign
	Bool sign = False;
	if (num < 0)
	{
		sign = True;
		num = -num;
	}

	// split number to exponent and mantissa
	int expI = 0;
	float mantD = num;
	if (mantD >= 1)
	{
		if (mantD >= 1e32f)  { mantD *= 1e-32f;  expI += 32;  };
		if (mantD >= 1e16f)  { mantD *= 1e-16f;  expI += 16;  };
		if (mantD >= 1e8f)   { mantD *= 1e-8f;   expI += 8;   };
		if (mantD >= 1e4f)   { mantD *= 1e-4f;   expI += 4;   };
		if (mantD >= 1e2f)   { mantD *= 1e-2f;   expI += 2;   };
		if (mantD >= 1e1f)   { mantD *= 1e-1f;   expI += 1;   };
	}
	else
	{
		if (mantD < 1e-32f)  { mantD *= 1e33f;   expI -= 33;  };
		if (mantD < 1e-16f)  { mantD *= 1e17f;   expI -= 17;  };
		if (mantD < 1e-8f)   { mantD *= 1e9f;    expI -= 9;   };
		if (mantD < 1e-4f)   { mantD *= 1e5f;    expI -= 5;   };
		if (mantD < 1e-2f)   { mantD *= 1e3f;    expI -= 3;   };
		if (mantD < 1e-1f)   { mantD *= 1e2f;    expI -= 2;   };
		if (mantD < 1e0f)    { mantD *= 1e1f;    expI -= 1;   };
	}

	// round last digit
	mantD += SetFloatCoeff[digits];

	// mantissa correction - prepare to export mantissa to 8 digits
	mantD *= 1e7f;
	if (mantD >= 1e8f)
	{
		mantD /= 10;
		expI++;
	}

	if (mantD < 1e7)
	{
		mantD *= 10;
		expI--;
	}

	// export mantissa
	u32 mant = (u32)mantD; // 8 digits of the mantissa

	// decode mantissa
	int i;
	char mantT[8]; // mantissa buffer (8 digits)
	char* mantP = mantT + 8; // pointer into buffer

	u32 k;
	for (i = 8; i > 0; i--)
	{
		mantP--;
		k = mant/10;
		*mantP = (char)((mant - k*10) + '0');
		mant = k;
	}

	// get number of significant digits (skip terminating '0' digits)
	char* dst = mantT + digits; // pointer to end of mantissa
	int dig = digits;
	for (; dig > 1; dig--)
	{
		dst--;
		if (*dst != (char)'0') break;
	}

	// prepare sign
	char buf[20]; // destination buffer (sign + 0000 + 8 digits + decimal + e + sign + 2 exp + NUL = 19)
	dst = buf;
	if (sign)
	{
		// store sign
		*dst = '-';
		dst++;
	}

	// decode number without exponent, number >= 1
	if ((expI < digits) && (expI >= 0))
	{
		for (;;)
		{
			*dst++ = *mantP++;
			dig--;
			if ((expI <= 0) && (dig <= 0)) break;
			if (expI == 0) *dst++ = '.';
			expI--;
		}
	}
	else
	{
		// decode number without exponent, number < 1
		if ((expI < 0) && (expI >= -3))
		{
			*dst++ = '0';
			*dst++ = '.';
			expI++;
			for (; expI < 0; expI++) *dst++ = '0';
			for (; dig > 0; dig--) *dst++ = *mantP++;
		}

		// decode number with exponent
		else
		{
			*dst++ = *mantP++;
			dig--;
			if (dig > 0)
			{
				*dst++ = '.';
				for (; dig > 0; dig--) *dst++ = *mantP++;
			}

			*dst++ = 'e';
			if (expI < 0)
			{
				*dst++ = '-';
				expI = -expI;
			}

			if (expI > 9)
			{
				k = expI/10;
				*dst++ = (char)(k + '0');
				expI = expI - k*10;
			}
			
			*dst++ = (char)(expI + '0');
		}
	}

	// save number into buffer
	int len = (int)(dst - buf);
	int len0;
	if (add)
	{
		// add number
		len0 = this->m_Data->len; // destination offset
		if (!this->SetLength(len0 + len)) return False; // set new length of the text
	}
	else
	{
		// set number
		len0 = 0; // destination offset = 0
		this->Detach(); // detach old text
		if (!this->NewBuffer(len)) return False;
	}

	// copy number into buffer
	memcpy(&this->m_Data->str[len0], buf, len);
	return True;
}

// set/add DOUBLE number to text (returns False on error)
//  num ... number
//  digits ... number of digits 1..17 (recommended 14)
//  add ... True=add number, False=set number
Bool string::SetAddDouble(double num, Bool add, int digits /*= 14*/)
{
	// limit number of digits
	if (digits < 1) digits = 1;
	if (digits > 17) digits = 17;

	// overflow
	if (DoubleIsOver(num))
	{
		if (!add) this->Empty();
		return this->Add((num < 0) ? "-INF" : "INF");
	}

	// zero
	if (num == 0)
	{
		if (!add) this->Empty();
		return this->Add((char)'0');
	}

	// prepare sign
	Bool sign = False;
	if (num < 0)
	{
		sign = True;
		num = -num;
	}

	// split number to exponent and mantissa
	int expI = 0;
	double mantD = num;
	if (mantD >= 1)
	{
		if (mantD >= 1e256) { mantD *= 1e-256; expI += 256; };
		if (mantD >= 1e128) { mantD *= 1e-128; expI += 128; };
		if (mantD >= 1e64)  { mantD *= 1e-64;  expI += 64;  };
		if (mantD >= 1e32)  { mantD *= 1e-32;  expI += 32;  };
		if (mantD >= 1e16)  { mantD *= 1e-16;  expI += 16;  };
		if (mantD >= 1e8)   { mantD *= 1e-8;   expI += 8;   };
		if (mantD >= 1e4)   { mantD *= 1e-4;   expI += 4;   };
		if (mantD >= 1e2)   { mantD *= 1e-2;   expI += 2;   };
		if (mantD >= 1e1)   { mantD *= 1e-1;   expI += 1;   };
	}
	else
	{
		if (mantD < 1e-256) { mantD *= 1e257;  expI -= 257; };
		if (mantD < 1e-128) { mantD *= 1e129;  expI -= 129; };
		if (mantD < 1e-64)  { mantD *= 1e65;   expI -= 65;  };
		if (mantD < 1e-32)  { mantD *= 1e33;   expI -= 33;  };
		if (mantD < 1e-16)  { mantD *= 1e17;   expI -= 17;  };
		if (mantD < 1e-8)   { mantD *= 1e9;    expI -= 9;   };
		if (mantD < 1e-4)   { mantD *= 1e5;    expI -= 5;   };
		if (mantD < 1e-2)   { mantD *= 1e3;    expI -= 3;   };
		if (mantD < 1e-1)   { mantD *= 1e2;    expI -= 2;   };
		if (mantD < 1e0)    { mantD *= 1e1;    expI -= 1;   };
	}

	// round last digit
	mantD += SetDoubleCoeff[digits];

	// mantissa correction - prepare to export mantissa to 17 digits
	mantD *= 1e16;
	if (mantD >= 1e17)
	{
		mantD /= 10;
		expI++;
	}

	if (mantD < 1e16)
	{
		mantD *= 10;
		expI--;
	}

	// export mantissa
	u64 mant = (u64)mantD; // 17 digits of the mantissa

	// decode mantissa
	int i;
	char mantT[17]; // mantissa buffer (17 digits)
	char* mantP = mantT + 17; // pointer into buffer

	u64 k;
	for (i = 17; i > 0; i--)
	{
		mantP--;
		k = mant/10;
		*mantP = (char)((mant - k*10) + '0');
		mant = k;
	}

	// get number of significant digits (skip terminating '0' digits)
	char* dst = mantT + digits; // pointer to end of mantissa
	int dig = digits;
	for (; dig > 1; dig--)
	{
		dst--;
		if (*dst != (char)'0') break;
	}

	// prepare sign
	char buf[31]; // destination buffer (sign + 0000 + 17 digits + decimal + e + sign + 3 exp + NUL = 29)
	dst = buf;
	if (sign)
	{
		// store sign
		*dst = '-';
		dst++;
	}

	// decode number without exponent, number >= 1
	if ((expI < digits) && (expI >= 0))
	{
		for (;;)
		{
			*dst++ = *mantP++;
			dig--;
			if ((expI <= 0) && (dig <= 0)) break;
			if (expI == 0) *dst++ = '.';
			expI--;
		}
	}

	else
	{
		// decode number without exponent, number < 1
		if ((expI < 0) && (expI >= -3))
		{
			*dst++ = '0';
			*dst++ = '.';
			expI++;
			for (; expI < 0; expI++) *dst++ = '0';
			for (; dig > 0; dig--) *dst++ = *mantP++;
		}

		// decode number with exponent
		else
		{
			*dst++ = *mantP++;
			dig--;
			if (dig > 0)
			{
				*dst++ = '.';
				for (; dig > 0; dig--) *dst++ = *mantP++;
			}

			*dst++ = 'e';
			if (expI < 0)
			{
				*dst++ = '-';
				expI = -expI;
			}

			if (expI > 99)
			{
				k = expI/100;
				*dst++ = (char)(k + '0');
				expI = expI - k*100;

				k = expI/10;
				*dst++ = (char)(k + '0');
				expI = expI - k*10;
			}
			else
			{
				if (expI > 9)
				{
					k = expI/10;
					*dst++ = (char)(k + '0');
					expI = expI - k*10;
				}
			}
			
			*dst++ = (char)(expI + '0');
		}
	}

	// save number into buffer
	int len = (int)(dst - buf);
	int len0;
	if (add)
	{
		// add number
		len0 = this->m_Data->len; // destination offset
		if (!this->SetLength(len0 + len)) return False; // set new length of the text
	}
	else
	{
		// set number
		len0 = 0; // destination offset = 0
		this->Detach(); // detach old text
		if (!this->NewBuffer(len)) return False;
	}

	// copy number into buffer
	memcpy(&this->m_Data->str[len0], buf, len);
	return True;
}

#define SET2DIG(n) { int k1=(n); int k2=k1/10; k1-=k2*10; *d++=(char)(k2+'0'); *d++=(char)(k1+'0'); }

// set/add time in Unix format (returns False on memory error)
//  ut ... Unix time
//  hourform ... hour format HOURFORM_*
//  secform ... second format SECFORM_*
//  timesep ... time separator TIMESEP_*
//  add ... True=add time, False=set time
//  ms ... number of milliseconds 0..999
Bool string::SetAddTime(u32 ut, int hourform, int secform, int timesep, Bool add, int ms /*= 0*/)
{
	// split Unix format to date and time
	int day = ut / (24*60*60);	// day
	int t = ut - (day * (24*60*60)); // time

	// unpack time h:m:s
	int h = t / (60*60);	// hour
	t -= h * (60*60);
	int m = t / 60;		// minute
	int s = t - m*60;	// second

	// prepare length of result
	int len0 = 4; // 0123 ... minimal time
	if ((hourform == HOURFORM_INT) && (h < 10)) len0--; // no leading space (integer format)
	char ampm = 0; // no am/pm
	if (hourform == HOURFORM_PM) // am/pm English format
	{
		ampm = 'a'; // am
		len0++; // add "a" or "b"
		if (h >= 12) // afternoon?
		{
			h -= 12; // 12-hour format
			ampm = 'p'; // pm
		}
		if (h == 0) h = 12; // change 0:23 -> 12:23
		if (h < 10) len0--; // is hour 1 digit?
	}
	if (secform != SECFORM_NO) len0 += 2; // add seconds
	if (secform == SECFORM_MS) len0 += 3; // add milliseconds
	if (timesep != TIMESEP_NO) len0 += secform + 1; // add separators

	// prepare destination buffer
	int len;
	if (add)
	{
		// add number
		len = this->m_Data->len; // destination offset
		if (!this->SetLength(len + len0)) return False; // set new length of the text
	}
	else
	{
		// set number
		len = 0; // destination offset = 0
		this->Detach(); // detach old text
		if (!this->NewBuffer(len0)) return False;
	}
	char* d = &this->m_Data->str[len];

	// prepare separators
	char sep1;
	char sep2;
	switch (timesep)
	{
	case TIMESEP_DOT: sep1 = ':'; sep2 = '.'; break;
	case TIMESEP_COMMA: sep1 = ':'; sep2 = ','; break;
	case TIMESEP_DOTCOM: sep1 = '.'; sep2 = ','; break;
	//case TIMESEP_NO:
	default: sep1 = 0; sep2 = 0; break;
	}
	
	// decode hour: 0=1:23, 1= 1:23, 2=01:23, 3=1.23p (case "3" should use separator '.')
	int k = h/10;
	h -= k*10;
	switch (hourform)
	{
	case HOURFORM_SPC: // _1:23
		*d++ = (k == 0) ? ' ' : (char)(k + '0');
		break;

	case HOURFORM_ZERO: // 01:23
		*d++ = (char)(k + '0');
		break;

	// case HOURFORM_INT:
	// case HOURFORM_PM:
	default: // 1:23, 1:23p
		if (k > 0) *d++ = (char)(k + '0');
	}
	*d++ = (char)(h + '0');

	// decode minute
	if (sep1 != 0) *d++ = sep1;
	SET2DIG(m);

	// decode second: 0=1:23, 1=1:23:45, 2=1:23:45.678
	if (secform != SECFORM_NO)
	{
		if (sep1 != 0) *d++ = sep1;
		SET2DIG(s);

		// decode milliseconds
		if (secform == SECFORM_MS)
		{
			if (sep2 != 0) *d++ = sep2;
			SET2DIG(ms/10);
			*d++ = (char)((ms%10) + '0');
		}
	}

	// am/pm
	if (ampm != 0) *d++ = ampm;
	return True;
}

// set/add day of week 2-character text from Unix time
//  ut ... Unix time
//  add ... True=add time, False=set time
//  dowform ... day of week format DOWFORM_*
Bool string::SetAddDow(u32 ut, Bool add, int dowform /*= DOWFORM_2*/)
{
	// get day of week from Unix time (0 = Sunday)
	int dow = ((ut / (24*60*60)) + 4) % 7;

	// prepare text
	char buf[2];
	const char* t;
	switch (dowform)
	{
	case DOWFORM_NUM0: // number 0..6
		buf[0] = dow + '0';
		buf[1] = 0;
		t = buf;
		break;

	case DOWFORM_NUM1: // number 1..7
		buf[0] = dow + '1';
		buf[1] = 0;
		t = buf;
		break;

	case DOWFORM_3:
		t = DayOfWeekName3[dow];
		break;

	case DOWFORM_LONG:
		t = DayOfWeekNameLong[dow];
		break;

	case DOWFORM_2: // 2 characters
	default:
		t = DayOfWeekName2[dow];
		break;
	}

	// prepare length of the text
	int len = StrLen(t);

	// increase buffer
	int len0;
	if (add)
	{
		// add text
		len0 = this->m_Data->len; // destination offset
		if (!this->SetLength(len0 + len)) return False; // set new length of the text
	}
	else
	{
		// set text
		len0 = 0; // destination offset = 0
		this->Detach(); // detach old text
		if (!this->NewBuffer(len)) return False;
	}

	// copy text
	memcpy(&this->m_Data->str[len0], t, len);
	return True;
}

// set/add date in custom format (returns False on memory error)
//   form: formatting string
//    d ... day 1 or 2 digits
//    D ... day 2 digits
//    m ... month 1 or 2 digits
//    M ... month 2 digits
//    n ... month name 3 letters
//    N ... month name full
//    y ... year 2 digits
//    Y ... year 4 digits
Bool string::SetAddDateCustom(int year, int mon, int day, const char* form, Bool add)
{
	// prepare length
	int len0 = 0;
	const char* f = form;
	char ch;
	while ((ch = *f++) != 0)
	{
		switch (ch)
		{
		case 'd': len0 += (day < 10) ? 1 : 2; break; // day 1 or 2 digits
		case 'm': len0 += (mon < 10) ? 1 : 2; break; // month 1 or 2 digits
		case 'N': len0 += StrLen(MonthNameLong[mon-1]); break; // month name long
		case 'y': // year 2 digits
		case 'M': // month 2 digits
		case 'D': len0 += 2; break; // day 2 digits
		case 'n': len0 += 3; break; // month name 3 characters
		case 'Y': len0 += 4; break; // year 4 digits
		default: len0++; // other characters
		}
	}

	// increase buffer
	int len;
	if (add)
	{
		// add text
		len = this->m_Data->len; // destination offset
		if (!this->SetLength(len + len0)) return False; // set new length of the text
	}
	else
	{
		// set text
		len = 0; // destination offset = 0
		this->Detach(); // detach old text
		if (!this->NewBuffer(len0)) return False;
	}

	// pointer into buffer
	char* d = &this->m_Data->str[len];

	// decode date
	int k, m;
	f = form;
	const char* t;
	while ((ch = *f++) != 0)
	{
		switch (ch)
		{
		case 'd': // day 1 or 2 digits
			k = day / 10;
			m = day - k*10;
			if (k > 0) *d++ = (char)(k + '0');
			*d++ = (char)(m + '0');
			break;

		case 'm': // month 1 or 2 digits
			k = mon / 10;
			m = mon - k*10;
			if (k > 0) *d++ = (char)(k + '0');
			*d++ = (char)(m + '0');
			break;

		case 'Y': // year 4 digits
			SET2DIG(year/100);

	// --- continue low 2 digits

		case 'y': // year 2 digits
			SET2DIG(year%100);
			break;

		case 'D': // day 2 digits
			SET2DIG(day);
			break;

		case 'M': // month 2 digits
			SET2DIG(mon);
			break;

		case 'N': // month name long
			t = MonthNameLong[mon - 1];
			k = StrLen(t);
			memcpy(d, t, k);
			d += k;
			break;

		case 'n': // month name 3 characters
			t = MonthName3[mon - 1];
			*d++ = *t++;
			*d++ = *t++;
			*d++ = *t++;
			break;

		default: // other characters
			*d++ = ch;
		}
	}

	return True;
}

// set date in format DATEFORM_* (returns False on memory error)
Bool string::SetDate(int year, int mon, int day, int form)
{
	return this->SetAddDateCustom(year, mon, day, DateForm[form], False);
}

// add date in format DATEFORM_* (returns False on memory error)
Bool string::AddDate(int year, int mon, int day, int form)
{
	return this->SetAddDateCustom(year, mon, day, DateForm[form], True);
}

#if USE_CALENDAR	// use 32-bit calendar (lib_calendar.c, lib_calendar.h)

// set Unix date in format DATEFORM_* (returns False on memory error)
Bool string::SetDateUnix(u32 ut, int form)
{
	sDateTime dt;
	DateTimeUnpack(&dt, ut, 0, 0);
	return this->SetDate(dt.year, dt.mon, dt.day, form);
}

// add Unix date in format DATEFORM_* (returns False on memory error)
Bool string::AddDateUnix(u32 ut, int form)
{
	sDateTime dt;
	DateTimeUnpack(&dt, ut, 0, 0);
	return this->AddDate(dt.year, dt.mon, dt.day, form);
}

// set/add Unix date and time in technical format yyyymmddhhmmssmmm (17 digits, returns False on memory error)
Bool string::SetAddDateTech(u32 ut, int ms, Bool add)
{
	// unpack date and time
	sDateTime dt;
	DateTimeUnpack(&dt, ut, ms, 0);

	// increase buffer
	int len;
	if (add)
	{
		// add text
		len = this->m_Data->len; // destination offset
		if (!this->SetLength(len + 17)) return False; // set new length of the text
	}
	else
	{
		// set text
		len = 0; // destination offset = 0
		this->Detach(); // detach old text
		if (!this->NewBuffer(17)) return False;
	}

	// destination pointer
	char* d = &this->m_Data->str[len];

	// decode
	SET2DIG(dt.year/100); // year high 2 digits
	SET2DIG(dt.year%100); // year low 2 digits
	SET2DIG(dt.mon); // month
	SET2DIG(dt.day); // day
	SET2DIG(dt.hour); // hour
	SET2DIG(dt.min); // minute
	SET2DIG(dt.sec); // second
	int m = dt.ms; // milliseconds
	SET2DIG(m/10); // millisecond high
	*d++=(char)((m%10)+'0'); // millisecond low

	return True;
}

#endif // USE_CALENDAR

#endif // USE_TEXT
