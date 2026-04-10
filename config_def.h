
// ****************************************************************************
//                                 
//                         Library Default Configuration
//
// ****************************************************************************

#ifndef _CONFIG_DEF_H
#define _CONFIG_DEF_H

// base device
#if USE_PIBASE
#include "_devices/pibase/_config.h"
#endif

// ZeroPC
#if USE_ZEROPC
#include "_devices/zeropc/_config.h"
#endif

// ZeroTiny
#if USE_ZEROTINY
#include "_devices/zerotiny/_config.h"
#endif

// keyboard
#ifndef SYSTICK_KEYSCAN
#define SYSTICK_KEYSCAN		0		// call KeyScan() function from SysTick system timer
#endif

#ifndef SYSTICK_MS
#define SYSTICK_MS		5		// increment of system time in [ms] on SysTick interrupt (0 = do not use SysTick interrupt)
#endif

// Screen
#ifndef WIDTH
#define WIDTH			640		// draw width (0=do not initialize default videomode)
#endif

#ifndef HEIGHT
#define HEIGHT			480		// draw height
#endif

#ifndef SCREENSCALE
#define SCREENSCALE		1		// 0 = no back buffer, using back buffer with screen scale 1, 2 or 4
#endif

// Print
#ifndef USE_DRAW_STDIO
#define USE_DRAW_STDIO		1		// use DRAW stdio (printf() or DrawPrint() function)
#endif

#ifndef USE_UART1_STDIO
#define USE_UART1_STDIO		0		// 1=use UART1 stdio (printf() or UART1_Print() function; 8 bits, 1 stop bit, no parity)
#endif

#ifndef USE_UART1_BAUD
#define USE_UART1_BAUD		115200		// UART1 stdio baudrate (recommended 115200)
#endif

#ifndef USE_UART1_GPIO
#define USE_UART1_GPIO		14		// UART1 stdio TX GPIO output port number (use 14, 32 or 40)
#endif

// System
// If you use uncached video memory (MMU_CACHE_MODE = 1), it will not be necessary
// to invalidate the cache memory after writing. However, it is important to
// remember that uncached memory does not allow unaligned access. This mainly
// applies to the memcpy() function, which internally uses unaligned access -
// using it on unaligned memory can therefore cause the system to crash.
// If you use uncached memory, you must adhere to strict aligned memory access.
#ifndef MMU_CACHE_MODE
#define MMU_CACHE_MODE		0		// MMU cache mode: 0=all cached (default), 1=video not cached, 2=no cache
#endif

// 1=use multicore (for applications), 0=do not use other cores (for loader)
#ifndef USE_MULTICORE
#if CORES > 1
#define USE_MULTICORE		1
#else
#define USE_MULTICORE		0
#endif
#endif

// system initialize date and time
#ifndef SYSTIME_INIT_YEAR
#define SYSTIME_INIT_YEAR	2026
#define SYSTIME_INIT_MONTH	1
#define SYSTIME_INIT_DAY	1
#define SYSTIME_INIT_HOUR	12
#endif

// PWM sound output
#ifndef PWMSND_CHANNUM
#define PWMSND_CHANNUM	4	// number of sound channels of sound mixer
#endif

#ifndef PWMSND_GPIO0
#define PWMSND_GPIO0	12	// GPIO pin for PWM0 (12 alt0, 18 alt5, 40 alt0)
#endif

#ifndef PWMSND_GPIO1
#define PWMSND_GPIO1	13	// GPIO pin for PWM1 (13 alt0, 19 alt5, 41 alt0, 45 alt0)
#endif

#ifndef PWMSND_SWAP
#define PWMSND_SWAP	0	// 1=swap channels (PWM1 left, PWM0 right), 0=no swap (PWM0 left, PWM1 right)
#endif

// Big integer numbers
#if USE_BIGINT	// 1=use BIT integers, 0=not used (_lib/bigint/*)
#ifndef BIGINT_BERN_NUM
#define BIGINT_BERN_NUM	1024	// number of table Bernoulli numbers - use number 0, 256, 512, 768 or 1024
#endif
#endif // USE_BIGINT

// REAL number
#if USE_REAL	// 1=use REAL numbers, 0=not used (_lib/real/*)

#ifndef USE_REAL16
#define USE_REAL16	0		// 1 = use real16 numbers (3 digits, exp +-4)
#endif

#ifndef USE_REAL32
#define USE_REAL32	0		// 1 = use real32 numbers (float, 7 digits, exp +-38)
#endif

#ifndef USE_REAL48
#define USE_REAL48	0		// 1 = use real48 numbers (11 digits, exp +-153)
#endif

#ifndef USE_REAL64
#define USE_REAL64	0		// 1 = use real64 numbers (double, 16 digits, exp +-308)
#endif

#ifndef USE_REAL80
#define USE_REAL80	0		// 1 = use real80 numbers (19 digits, exp +-4932)
#endif

#ifndef USE_REAL96
#define USE_REAL96	0		// 1 = use real96 numbers (25 digits, exp +-1233)
#endif

#ifndef USE_REAL128
#define USE_REAL128	0		// 1 = use real128 numbers (34 digits, exp +-4932)
#endif

#ifndef USE_REAL160
#define USE_REAL160	0		// 1 = use real160 numbers (43 digits, exp +-9864)
#endif

#ifndef USE_REAL192
#define USE_REAL192	0		// 1 = use real192 numbers (52 digits, exp +-19728)
#endif

#ifndef USE_REAL256
#define USE_REAL256	0		// 1 = use real256 numbers (71 digits, exp +-78913)
#endif

#ifndef USE_REAL384
#define USE_REAL384	0		// 1 = use real384 numbers (109 digits, exp +-315652)
#endif

#ifndef USE_REAL512
#define USE_REAL512	0		// 1 = use real512 numbers (147 digits, exp +-1262611)
#endif

#ifndef USE_REAL768
#define USE_REAL768	0		// 1 = use real768 numbers (224 digits, exp +-5050445)
#endif

#ifndef USE_REAL1024
#define USE_REAL1024	0		// 1 = use real1024 numbers (300 digits, exp +-20201781)
#endif

#ifndef USE_REAL1536
#define USE_REAL1536	0		// 1 = use real1536 numbers (453 digits, exp +-161614248)
#endif

#ifndef USE_REAL2048
#define USE_REAL2048	0		// 1 = use real2048 numbers (607 digits, exp +-161614248)
#endif

#ifndef USE_REAL3072
#define USE_REAL3072	0		// 1 = use real3072 numbers (915 digits, exp +-161614248)
#endif

#ifndef USE_REAL4096
#define USE_REAL4096	0		// 1 = use real4096 numbers (1224 digits, exp +-161614248)
#endif

#ifndef USE_REAL6144
#define USE_REAL6144	0		// 1 = use real6144 numbers (1841 digits, exp +-161614248)
#endif

#ifndef USE_REAL8192
#define USE_REAL8192	0		// 1 = use real8192 numbers (2457 digits, exp +-161614248)
#endif

#ifndef USE_REAL12288
#define USE_REAL12288	0		// 1 = use real12288 numbers (3690 digits, exp +-161614248)
#endif

// Limiting the usage of large Chebyshev tables
//  Chebyshev approximations can speed up calculations of mathematical functions.
//  However, the use of Chebyshev approximations for long numbers takes a lot of
//  memory and can be slow due to slow Flash memory access. If needed, set
//  a limit here on the bit length of the numbers for which Chebyshev approximations
//  will be used.
#ifndef MAXCHEB_LN
#define MAXCHEB_LN	0 //1536		// max. REAL number supporting Chebyshev approximations of Ln()
#endif

#ifndef MAXCHEB_EXP
#define MAXCHEB_EXP	0 //1536		// max. REAL number supporting Chebyshev approximations of Exp()
#endif

#ifndef MAXCHEB_SIN
#define MAXCHEB_SIN	0 //1536		// max. REAL number supporting Chebyshev approximations of Sin()
#endif

#ifndef MAXCHEB_ASIN
#define MAXCHEB_ASIN	0 //1536		// max. REAL number supporting Chebyshev approximations of ASin()
#endif

#ifndef MAXCHEB_ATAN
#define MAXCHEB_ATAN	0 //1536		// max. REAL number supporting Chebyshev approximations of ATan()
#endif

#ifndef MAXCHEB_SQRT
#define MAXCHEB_SQRT	0 //1536		// max. REAL number supporting Chebyshev approximations of Sqrt()
#endif

// Limiting the usage of large Cordic tables (see note of Chebyshev tables)
//   Note: Calculations using Cordic are not complete in this library because
//   they have lower precision and they are not recommended for use in this library.
#ifndef MAXCORD_ATAN
#define MAXCORD_ATAN	0 //8192	// max. REAL number supporting Cordic atan table (to calculate sin, cos, tan, asin, acos, atan)
#endif

// Limiting the usage of linear factorials
//  Calculating the linear factorial requires tables that take up much Flash memory.
//  If you need to limit the size of the tables, set a limit here of the bit length of
//  the numbers that will support the linear factorial. The integer factorial
//  can still be used, but it is slow for large numbers.
#ifndef MAXFACT_COEFF
#define MAXFACT_COEFF	8192	// max. REAL number supporting linear factorial
#endif

#ifndef REAL_EDITBUF_MAX
#define REAL_EDITBUF_MAX 1000 //3000	// size of edit buffer (without terminating 0)
				// - should be big enough to decode BIN format, use MAX_DIG*4
#endif

#endif // USE_REAL


#endif // _CONFIG_DEF_H
