
// ****************************************************************************
//                                 
//                              Common definitions
//
// ****************************************************************************

#pragma warning(disable : 4996) // function unsafe
//#pragma warning(disable : 4101) // unreferenced

/*
#ifndef REAL_EDITBUF_MAX
#define REAL_EDITBUF_MAX 3000 //(MAX_DIG+30) // size of edit buffer (without terminating 0)
#endif
extern char EditBuf[REAL_EDITBUF_MAX+1]; // edit buffer
*/

// system
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <math.h>

#include "global.h"
#include "config.h"

// angle unit
#define REAL_UNIT_DEG	0	// degrees
#define REAL_UNIT_RAD	1	// radians
#define REAL_UNIT_GRAD	2	// grads

// radix numeric base
#define REAL_BASE_DEC	0	// decimal
#define REAL_BASE_BIN	1	// binary
#define REAL_BASE_OCT	2	// octal
#define REAL_BASE_HEX	3	// hexadecimal

//extern unsigned char RealBase;		// current numeric radix base REAL_BASE_*

// exponent mode
#define REAL_EXP_AUTO	0	// auto mode
#define REAL_EXP_FIX	1	// fixed mode (no exponent on small numbers)
#define REAL_EXP_EE	2	// exponent mode
#define REAL_EXP_ENG	3	// engineering technical mode (exponent multiply of 3)

// rounding
#define REAL_FIX_OFF	-1	// fix decimals are off

// default floating point type
#if USE_DOUBLE		// use double support
typedef double real;	// floating number, preffered double
#else
typedef float real;	// floating number, preffered double
#endif

extern u8 RealUnit;	// current angle unit UNIT_* (used by functions ToRad() and FromRad())
extern u8 RealBase;	// current numeric radix base REAL_BASE_*
extern u8 RealExpMode;	// current exponent mode REAL_EXP_*
extern int RealFix;	// current fixed decimal places (0..digits, or REAL_FIX_OFF=off)

// configuration of ToText function
extern char RealCharPlus;	// character of positive number ('+', ' ' or 0=do not use)
extern char RealCharExp;	// character of exponent ('e', 'E' or 0=do not use)
extern char RealExpPlus;	// character of positive exponent ('+', ' ' or 0=do not use)
extern char RealCharDec;	// character used as decimal separator (',', '.' or B7=add flag to previous digit)
extern Bool RealRightAlign;	// right align text in buffer
extern int RealMaxDig;		// max. number of digits of mantissa (0 = default, negative = cut digits from end)

// number editor (and ToText result)
extern int ExpLen;		// number of exponent digits returned by ToText function
extern int EditLen;		// length of mantissa
extern Bool PointOn;	// decimal point was entered

#ifndef REAL_EDITBUF_MAX
#define REAL_EDITBUF_MAX 3000 //(MAX_DIG+30) // size of edit buffer (without terminating 0)
						// - should be big enough to decode BIN format, use MAX_DIG*4
#endif
extern char EditBuf[REAL_EDITBUF_MAX+1]; // edit buffer

#include "main.h"
#include "rand.h"

// generators
#include "genconst.h"	// generate constants

#include "../../bigint/bigint.h"
