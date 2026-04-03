
// ****************************************************************************
//
//                               Formatted print
//
// ****************************************************************************
/*
  format:
%[flags][width][.precision][length]specifier


specifier
---------
% ... print character '%'
b ... print unsigned integer in BIN format (1011010111)
B ... print floating point as capacity in bytes, with unit prefix (3.45KB; alternative '#' with space 3.45 KB)
c ... print single-byte character
d or i ... print signed integer in DEC format (1234567)
E or e ... print floating point with exponent 'E' or 'e' ('exponent mode', -1.2345e+123)
		Trailing zeros are not reduced, or are reduced with the '#' flag.
F or f ... print floating point with fixed point ('fixed mode', -123.456)
		Trailing zeros are not reduced, or are reduced with the '#' flag.
G or g ... print floating point with automatic format selection (general mode)
		Trailing zeros are reduced, or are not reduced with the '#' flag.
m ... print memory block pointed by argument, as HEX 8-bit bytes with commas,
	precision .n is total number of bytes or .* from argument,
	width is number of bytes per row or * from argument (default 16), '#' flag is print as decimals
M ... print memory block pointed by argument, as HEX 16-bit words with commas,
	precision .n is total number of words or .* from argument,
	width is number of words per row or * from argument (default 8), '#' flag is print as decimals
o ... print unsigned integer in OCT format
P or p ... print pointer (upper or lower case, with prefix 0x). (0x0200457A)
s ... print ASCIIZ text (length can be limited with '.precision')
t ... print floating point in technical notation, with exponent 'e' in multiples of 3 (alternative '#' reduce zeroes)
T ... print floating point in technical notation, with prefix u, m, k, M, G,... (alternative '#' with space)
       If number is out of range 10^-30..10^+32, it changes to 't' format with exponent 'e'.
u ... print unsigned integer in DEC format
X or x ... print unsigned integer in HEX format, upper or lower case


flags
-----
' ... grouping separator, integer part of the number is grouped by 3 (or 4) digits, with a separator character '
	'Mm' - print in compressed form XX-XX-XX-...
(space) ... number will be preceded by a space instead of a '+' (ignored when '+' flag is specified)
# ... alternative form: 'Gg' without end-zero reduction, 'EeFft' with end-zero reduction, 'Mm' decimal
	'Pp' without prefix 0x, Xxbo with printing prefix 0x 0b 0, 'BT' with space
- ... argument will be left-aligned in the 'width' field (otherwise the default is right-aligned)
+ ... the '+' sign is printed on a positive argument (otherwise only the '-' sign is displayed)
0 ... number is padded with zeros instead of spaces in the 'width' field on the left


width
-----
width ... number is minimum width of the field to display the argument (padded with spaces)
* ... the width is not specified in the format string, but is read from the argument.
	The negative value of the width is interpreted as the character '-' (i.e. a left-aligned argument).

precision
---------
.precision ... number after the point determines the precision: for 'bdiouXx' (integer) determines
      the minimum number of displayed digits (padded with leading zeros; default 1).
      For 'EeFf' specifies the number of decimal places (default 6). For 'BGgTt', it is the maximum number of
      printable significant digits. For 's' (text) it is the maximum number of characters displayed.
.* ... by specifying * instead of precision, the precision is read from the argument (absolute value is used).


In the case when '.*' is used, the precision is specified by an additional argument
of type 'int', which appears before the argument to be converted, but after
the argument supplying minimum field width if one is supplied with '*'.


length modifier
---------------
ll ... argument is 'long long int' (u64)

reserved letters from other printf libraries (not implemented in this library):
----------------
hh ... argument is char
h ... argument is short
l ... argument is long-int
L ... argument is long double
z ... argument is size_t
j ... argument is intmax_t
I ... argument is ptrdiff_t
q ... argument is s64,u64
a, A ... double in hexadecimal notation
n ... print nothing, get length

free letters to use: k, K, r, R, v, V, w, W, y, Y

Special values of decimal number:
 infinity ... 'inf' or '-inf' ... 'INF'
 NaN ... 'nan' or '-nan' ... 'NAN'


va_list ... variadic functions (variable number of arguments)

*/

#if USE_PRINT		// 1=use Formatted print, 0=not used (lib_print.*)

#ifndef _LIB_PRINT_H
#define _LIB_PRINT_H

/*
// check special float numbers
INLINE Bool CheckInfF(float num)  { return (*(u32*)&num>>22) == (0x7f800000>>22); } // +1#INF
INLINE Bool CheckSInfF(float num) { return (*(u32*)&num>>22) == (0xff800000>>22); } // -1#INF
INLINE Bool CheckNanF(float num)  { return (*(u32*)&num>>22) == (0x7fc00000>>22); } // +1#NAN
INLINE Bool CheckSNanF(float num) { return (*(u32*)&num>>22) == (0xffc00000>>22); } // -1#NAN

// check special double numbers
INLINE Bool CheckInfD(double num)  { return (((u32*)&num)[1]>>19) == (0x7ff00000>>19); } // +1#INF
INLINE Bool CheckSInfD(double num) { return (((u32*)&num)[1]>>19) == (0xfff00000>>19); } // -1#INF
INLINE Bool CheckNanD(double num)  { return (((u32*)&num)[1]>>19) == (0x7ff80000>>19); } // +1#NAN
INLINE Bool CheckSNanD(double num) { return (((u32*)&num)[1]>>19) == (0xfff80000>>19); } // -1#NAN
*/

// bit mask of used stdio streams
extern u32 StdioMask;

// stream for memory printing
extern sStream MemStream;

#if USE_FAT	// use FAT file system (lib_fat.c, lib_fat.h)
// open file to file printing
extern sFile* FileToPrint;
#endif

// stream printing (returns number of characters ... text is not terminated with NUL)
//  wstr ... write stream (destination buffer)
//  rstr ... read stream (formatting string)
//  args ... arguments
u32 StreamPrintArg(sStream* wstr, sStream* rstr, va_list args);

#define SPRINTF_MAX	40	// default max. size of buffer in sprintf functions

// formatted print string into memory buffer, with argument list (returns number of characters, without terminating 0)
//  max ... size of destination buffer, including terminating 0
u32 MemPrintArg(char* buf, u32 max, const char* fmt, va_list args);
int vsprintf(char* buf, const char* fmt, va_list args);
int vsnprintf(char* buf, size_t max, const char* fmt, va_list args);

// formatted print string into memory buffer, with variadic arguments (returns number of characters, without terminating 0)
//  max ... size of destination buffer, including terminating 0
NOINLINE u32 MemPrint(char* buf, u32 max, const char* fmt, ...);
int sprintf(char* buf, const char* fmt, ...);
int snprintf(char* buf, size_t max, const char* fmt, ...);

// formatted print string into NUL to get result length, with argument list (returns number of characters)
u32 NulPrintArg(const char* fmt, va_list args);

// formatted print string into NUL to get result length, with variadic arguments (returns number of characters)
NOINLINE u32 NulPrint(const char* fmt, ...);

#if USE_USB_STDIO	// use USB stdio (UsbPrint function)
// formatted print string to USB, with argument list (returns number of characters)
u32 UsbPrintArg(const char* fmt, va_list args);

// formatted print string to USB, with variadic arguments (returns number of characters)
NOINLINE u32 UsbPrint(const char* fmt, ...);

// print character to USB
INLINE void UsbPrintChar(char ch) { UsbDevCdcWriteChar(ch); }

// print unformatted text to USB (returns number of characters)
u32 UsbPrintText(const char* txt);
#endif // USE_USB_STDIO

// stdio interface
#define STDIO_MASK_DRAW		B0	// display (use USE_DRAW_STDIO to enable)
#define STDIO_MASK_USB		B1	// USB port (use USE_USB_STDIO to enable)
#define STDIO_MASK_MEM		B2	// memory print
#define STDIO_MASK_UART1	B3	// UART1 port
#define STDIO_MASK_FILE		B4	// file stdio

// start display printing (using printf function; must be enabled with USE_DRAW_STDIO)
INLINE void DrawPrintStart() { StdioMask |= STDIO_MASK_DRAW; }

// stop display printing
INLINE void DrawPrintStop() { StdioMask &= ~STDIO_MASK_DRAW; }

// start USB printing (using printf function; must be enabled with USE_USB_STDIO)
INLINE void UsbPrintStart() { StdioMask |= STDIO_MASK_USB; }

// stop USB printing
INLINE void UsbPrintStop() { StdioMask &= ~STDIO_MASK_USB; }

// start memory printing (using printf function)
//  max ... size of destination buffer, including terminating 0
void MemPrintStart(char* buf, u32 max);

// get current length of text in memory printing buffer
INLINE u32 MemPrintLen() { return MemStream.pos; }

// stop memory printing
INLINE void MemPrintStop() { StdioMask &= ~STDIO_MASK_MEM; }

// start UART1 printing (using printf function; must be enabled with USE_UART1_STDIO)
INLINE void UART1_PrintStart() { StdioMask |= STDIO_MASK_UART1; }

// stop UART1 printing
INLINE void UART1_PrintStop() { StdioMask &= ~STDIO_MASK_UART1; }

#if USE_FAT	// use FAT file system (lib_fat.c, lib_fat.h)
// start file printing (using printf function)
//  file ... open file
void FilePrintStart(sFile* file);

// stop file printing
INLINE void FilePrintStop() { StdioMask &= ~STDIO_MASK_FILE; }
#endif

// print character to print channels
void PrintChar(char ch);
INLINE void printchar(char ch) { PrintChar(ch); }
extern "C" int putchar(int ch);
INLINE int putchar_raw(char ch) { return putchar(ch); }

// print unformatted text to print channels
int PrintText(const char* txt);
INLINE int printtext(const char* txt) { return PrintText(txt); }

// print unformatted text with added new-line
extern "C" int puts(const char* txt);
INLINE int puts_raw(const char* txt) { return puts(txt); }

// formatted print to print channels
// - Do not print simultaneously from the program and from the interrupt handler!
//   If you really need it, use SpinLock lock to access it.
int PrintArg(const char* fmt, va_list args);
extern "C" int vprintf(const char* fmt, va_list args);
int Print(const char* fmt, ...);
extern "C" int printf(const char* fmt, ...);
int print(const char* fmt, ...);

#define NOCHAR 0

// get character from stdio (returns NOCHAR=0 if no character)
char GetChar();

// get character from stdio, with wait
char GetCharWait();
extern "C" char getchar();

// get character from stdio with time-out in [us] (returns NOCHAR=0 if no character)
char GetCharUs(u32 us);
INLINE char GetCharMs(u32 ms) { return GetCharUs(ms*1000); }

/*
// get character from stdio with time-out in [us] (returns -1 if no character)
INLINE int getchar_timeout_us(u32 us)
{
	char ch = GetCharUs(us);
	if (ch == NOCHAR) return -1;
	return ch;
}

INLINE int getchar_timeout_ms(u32 ms) { return getchar_timeout_us(ms*1000); }
*/

// flush input characters
void FlushChar(void);
//INLINE void stdio_flush(void) { FlushChar(); }

#endif // _LIB_PRINT_H

#endif // USE_PRINT
