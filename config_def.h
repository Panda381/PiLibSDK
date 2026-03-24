
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
#define USE_MULTICORE		1
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

#endif // _CONFIG_DEF_H
