
// ****************************************************************************
//                                 
//                        Project library configuration
//
// ****************************************************************************

#ifndef _CONFIG_H
#define _CONFIG_H

// Font
#define FONT		FontBold8x16	// default fixed font
#define FONTW		8		// width of fixed font
#define FONTH		16		// height of fixed font
//#define FONTPROP	FontARB10	// default proportional font

// Screen
#define WIDTH		640		// draw width (0=do not initialize default videomode)
#define HEIGHT		480		// draw height
#define SCREENSCALE	1		// 0 = no back buffer, using back buffer with screen scale 1, 2 or 4

// Print
//#define USE_DRAW_STDIO	1		// use DRAW stdio (printf() or DrawPrint() function)
//#define USE_UART1_STDIO	0		// 1=use UART1 stdio (printf() or UART1_Print() function; 8 bits, 1 stop bit, no parity)
//#define USE_UART1_BAUD	115200		// UART1 stdio baudrate (recommended 115200)
//#define USE_UART1_GPIO	14		// UART1 stdio TX GPIO output port number (use 14, 32 or 40)

// System
// If you use uncached video memory (MMU_CACHE_MODE = 1), it will not be necessary
// to invalidate the cache memory after writing. However, it is important to
// remember that uncached memory does not allow unaligned access. This mainly
// applies to the memcpy() function, which internally uses unaligned access -
// using it on unaligned memory can therefore cause the system to crash.
#define MMU_CACHE_MODE	0		// MMU cache mode: 0=all cached (default), 1=video not cached, 2=no cache

// increment of system time in [ms] on SysTick interrupt (0 = do not use SysTick interrupt)
//#define SYSTICK_MS	5

// 1=use multicore (for applications), 0=do not use other cores (for loader)
// Loader must be run without multicore, because cannot park other cores to their original address
#define USE_MULTICORE	0

// Include default configuration
// - For available options see config_def.h in root directory.
#include CONFIG_DEF_H				// default configuration

#endif // _CONFIG_H
