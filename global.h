
// ****************************************************************************
//                                 
//                         Global common C definitions
//
// ****************************************************************************

#ifndef _GLOBAL_H
#define _GLOBAL_H

// ----------------------------------------------------------------------------
//                               Attributes
// ----------------------------------------------------------------------------

// count of array entries
#define count_of(a) (sizeof(a)/sizeof((a)[0]))

// I/O port prefix
//#define __IO	volatile

// request to use inline
#define INLINE __attribute__((always_inline)) inline

// avoid to use inline
#define NOINLINE __attribute__((noinline))

// coherent region (mailbox, DMA)
extern unsigned char __coherent_start;
extern unsigned char __coherent_end;
#define COHERENT __attribute__((section(".coherent")))

// weak function
#define WEAK __attribute__((weak))

// align array to 4-bytes
#define ALIGNED __attribute__((aligned(4)))

// packed structure, to allow access to unaligned entries
#define PACKED __attribute__ ((packed))

// fast function optimization
#define FASTCODE __attribute__ ((optimize("-Ofast")))

// small function optimization
#define SMALLCODE __attribute__ ((optimize("-Os")))

// no optimization (may be necessary for some problematic functions)
#define NOOPTIM __attribute__ ((optimize("-O0")))

// compile-time check
#ifdef __cplusplus
#define STATIC_ASSERT(c, msg) static_assert((c), msg)
#else
#define STATIC_ASSERT(c, msg) _Static_assert((c), msg)
#endif

// wrapper function (to replace standard library function)
#define WRAPPER_FUNC(x) __wrap_ ## x

// ----------------------------------------------------------------------------
//                              Base data types
// ----------------------------------------------------------------------------

/* Basic data types and their sizes (in bytes) depending on the system:

		8-bit	16-bit	32-bit	64-bit
char		1	1	1	1
short		1	1	2	2
int		2	2	4	4
long		4	4	4	4,8
long long	8	8	8	8
*/

// Note: 'char' can be signed or unsigned

// base types
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef signed long long int s64;
typedef unsigned long long int u64;

typedef unsigned int uint;

#ifdef __aarch64__
typedef unsigned long size_t;
typedef unsigned long uintptr_t;
typedef signed long intptr_t;
#else
typedef unsigned int size_t;
typedef unsigned int uintptr_t;
typedef signed int intptr_t;
#endif

// hw registers
typedef volatile u32 io32;
typedef volatile u16 io16;
typedef volatile u8 io8;

// optimised Bool (1-byte size, value 0 or not-0)
typedef unsigned char Bool;
#define True 1
#define False 0

// NULL
#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void*)0)
#endif
#endif

// compile-time check 
STATIC_ASSERT(sizeof(s8) == 1, "Incorrect typedef s8!");
STATIC_ASSERT(sizeof(u8) == 1, "Incorrect typedef u8!");
STATIC_ASSERT(sizeof(s16) == 2, "Incorrect typedef s16!");
STATIC_ASSERT(sizeof(u16) == 2, "Incorrect typedef u16!");
STATIC_ASSERT(sizeof(s32) == 4, "Incorrect typedef s32!");
STATIC_ASSERT(sizeof(u32) == 4, "Incorrect typedef u32!");
STATIC_ASSERT(sizeof(s64) == 8, "Incorrect typedef s64!");
STATIC_ASSERT(sizeof(u64) == 8, "Incorrect typedef u64!");

// BMP file header (size 54 bytes)
#pragma pack(push,1)
typedef struct {
	// BMP file header (size 14 bytes)
	u16	bfType;			// 0x00: magic, 'B' 'M' = 0x4D42
	u32	bfSize;			// 0x02: file size, aligned to DWORD = 54 + (((640*3+3)&~3)*480) + 2 = 921656 = 0xE1038
	u16	bfReserved1;		// 0x06: = 0
	u16	bfReserved2;		// 0x08: = 0
	u32	bfOffBits;		// 0x0A: offset of data bits after file header = 54 (0x36)
	// BMP info header (size 40 bytes)
	u32	biSize;			// 0x0E: size of this info header = 40 (0x28)
	s32	biWidth;		// 0x12: width = 640 (0x280)
	s32	biHeight;		// 0x16: height, negate if flip row order = -480 (0xFFFFFE20)
	u16	biPlanes;		// 0x1A: planes = 1
	u16	biBitCount;		// 0x1C: number of bits per pixel = 24 (0x18)
	u32	biCompression;		// 0x1E: compression = 0 (BI_NONE)
	u32	biSizeImage;		// 0x22: size of data of image + aligned file = (((640*3+3)&~3)*480) + 2 = 921602 (0xE1002)
	s32	biXPelsPerMeter;	// 0x26: X pels per meter = 2834 (= 0xB12)
	s32	biYPelsPerMeter;	// 0x2A: Y pels per meter = 2834 (= 0xB12)
	u32	biClrUsed;		// 0x2E: number of user colors (0 = all)
	u32	biClrImportant;		// 0x32: number of important colors (0 = all)
					// 0x36
} sBmp;
#pragma pack(pop)

// ----------------------------------------------------------------------------
//                               Constants
// ----------------------------------------------------------------------------

#define	B0 (1<<0)
#define	B1 (1<<1)
#define	B2 (1<<2)
#define	B3 (1<<3)
#define	B4 (1<<4)
#define	B5 (1<<5)
#define	B6 (1<<6)
#define	B7 (1<<7)
#define	B8 (1U<<8)
#define	B9 (1U<<9)
#define	B10 (1U<<10)
#define	B11 (1U<<11)
#define	B12 (1U<<12)
#define	B13 (1U<<13)
#define	B14 (1U<<14)
#define	B15 (1U<<15)
#define B16 (1UL<<16)
#define B17 (1UL<<17)
#define B18 (1UL<<18)
#define	B19 (1UL<<19)
#define B20 (1UL<<20)
#define B21 (1UL<<21)
#define B22 (1UL<<22)
#define B23 (1UL<<23)
#define B24 (1UL<<24)
#define B25 (1UL<<25)
#define B26 (1UL<<26)
#define B27 (1UL<<27)
#define B28 (1UL<<28)
#define B29 (1UL<<29)
#define B30 (1UL<<30)
#define B31 (1UL<<31)

#define BIT(pos) (1UL<<(pos))
#define BIT64(pos) (1ULL<<(pos))

#define PI 3.14159265358979324
#define PI2 (3.14159265358979324*2)

#define KILOBYTE	0x00000400
#define MEGABYTE 	0x00100000
#define GIGABYTE	0x40000000

// Control characters (ASCII characters)
#define CH_NUL		0x00	// '\0'  NUL null ^@			... no character, end of text
#define CH_ALL		0x01	// '\1'  SOH start of heading ^A	... select [A]ll
#define CH_BLOCK	0x02	// '\2'  STX start of text ^B		... mark [B]lock
#define CH_STX		CH_BLOCK
#define CH_COPY		0x03	// '\3'  ETX end of text ^C		... [C]opy block, copy file
#define CH_ETX		CH_COPY
#define CH_END		0x04	// '\4'  EOT end of transmission ^D	... en[D] of row, end of files
#define CH_MOVE		0x05	// '\5'  ENQ enquiry ^E			... rename files, mov[E] block
#define CH_FIND		0x06	// '\6'  ACK acknowledge ^F		... [F]ind
#define CH_NEXT		0x07	// '\a'  BEL bell (alert) ^G		... [G]o next, repeat find
#define CH_BS		0x08	// '\b'  BS backspace ^H		... backspace
#define CH_TAB		0x09	// '\t'  HT horizontal tabulator ^I	... tabulator
#define CH_LF		0x0A	// '\n'  LF line feed ^J		... line feed
#define CH_PGUP		0x0B	// '\v'  VT vertical tabulator ^K	... page up
#define CH_PGDN		0x0C	// '\f'  FF form feed ^L		... page down
#define CH_FF		CH_PGDN
#define CH_CR		0x0D	// '\r'  CR carriage return ^M		... enter, next row, run file
#define CH_NEW		0x0E	// '\16' SO shift-out ^N		... [N]ew file
#define CH_OPEN		0x0F	// '\17' SI shift-in ^O			... [O]pen file, edit file
#define CH_PRINT	0x10	// '\20' DLE data link escape ^P	... [P]rint file, send file
#define CH_QUERY	0x11	// '\21' DC1 device control 1 ^Q	... [Q]uery, display help
#define CH_REPLACE	0x12	// '\22' DC2 device control 2 ^R	... find and [R]eplace
#define CH_SAVE		0x13	// '\23' DC3 device control 3 ^S	... [S]ave file
#define CH_INS		0x14	// '\24' DC4 device control 4 ^T	... [T]oggle Insert switch, mark file
#define CH_HOME		0x15	// '\25' NAK negative acknowledge ^U	... Home, begin of row, begin of files
#define CH_PASTE	0x16	// '\26' SYN synchronous idle ^V	... paste from clipboard
#define CH_SYN		CH_PASTE
#define CH_CLOSE	0x17	// '\27' ETB end of transmission block ^W ... close file
#define CH_CUT		0x18	// '\30' CAN cancel ^X			... cut selected text
#define CH_REDO		0x19	// '\31' EM end of medium ^Y		... redo previously undo action
#define CH_UNDO		0x1A	// '\32' SUB subtitle character ^Z	... undo action
#define CH_ESC		0x1B	// '\e' or '\33' ESC escape ^[		... Esc, break, menu
#define CH_RIGHT	0x1C	// '\34' FS file separator "^\"		... Right, Shift: End, Ctrl: Word right
#define CH_UP		0x1D	// '\35' GS group separator ^]		... Up, Shift: PageUp, Ctrl: Text start
#define CH_LEFT		0x1E	// '\36' RS record separator ^^		... Left, Shift: Home, Ctrl: Word left
#define CH_DOWN		0x1F	// '\37' US unit separator ^_		... Down, Shift: PageDn, Ctrl: Text end
#define CH_SPC		0x20	// SPC space

#define CH_DEL		0x7F	// '\x7F' delete			... delete character on cursor, Ctrl: delete block, delete file

#define NOCHAR		0	// no character
#define NOKEY		-1	// no key from keyboard

// ----------------------------------------------------------------------------
//                           Base Includes
// ----------------------------------------------------------------------------

#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)

// project configuration
#include "config.h"		// project configuration

#include <string.h>		// memcpy
#include <stdarg.h>		// va_list
#include <math.h>		// HUGE_VAL

#endif // _GLOBAL_H
