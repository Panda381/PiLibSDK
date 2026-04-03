
// ****************************************************************************
//                                 
//                     Real numbers - codes of all formats
//
// ****************************************************************************

#ifdef REAL_GENERATOR
#include "genconst/include.h"
#else
#include "../../includes.h"	// globals
#endif

#if USE_REAL	// use real numbers

//#ifndef REAL_GENERATOR
//#include "../../_sdk/inc/sdk_divider.h"
//#include "../../_sdk/inc/sdk_bootrom.h"
//#include "../inc/lib_rand.h"
//#include "../inc/lib_malloc.h"
//#endif

//#include "../bigint/bigint.h"

// === Real numbers control

u8 RealUnit = REAL_UNIT_DEG;	// current angle unit UNIT_* (used by functions ToRad() and FromRad())
u8 RealBase = REAL_BASE_DEC;	// current numeric radix base REAL_BASE_*
u8 RealExpMode = REAL_EXP_AUTO;	// current exponent mode REAL_EXP_*
int RealFix = REAL_FIX_OFF;	// current fixed decimal places (0..digits, or REAL_FIX_OFF=off)

// configuration of ToText function
char RealCharPlus = 0;			// character of positive number ('+', ' ' or 0=do not use)
char RealCharExp = 'e';			// character of exponent ('e', 'E' or 0=do not use)
char RealExpPlus = '+';			// character of positive exponent ('+', ' ' or 0=do not use)
char RealCharDec = '.';			// character used as decimal separator (',', '.' or B7=add flag to previous digit)
Bool RealRightAlign = False;	// right align text in buffer
int RealMaxDig = -1;			// max. number of digits of mantissa (0 = default, negative = cut digits from end)

// number editor (and ToText result)
int ExpLen = 0;			// number of exponent digits returned by ToText function
int EditLen = 0;		// length of mantissa
Bool PointOn = False;	// decimal point was entered

char EditBuf[REAL_EDITBUF_MAX+1]; // edit buffer
char EditBuf2[REAL_EDITBUF_MAX+1]; // edit buffer
char EditBuf3[REAL_EDITBUF_MAX+1]; // edit buffer
char EditBuf4[REAL_EDITBUF_MAX+1]; // edit buffer

int Deb_LoopNum;		// number of iteration loops

// === REAL numbers definition

#ifdef REAL_GENERATOR
#define REAL_PATH	"real_h.h"
#include "real.h"			// all real numbers
#endif

#define REAL_PATH	"real_c.h"
#include "real.h"		// all real numbers

#endif // USE_REAL	// use real numbers
