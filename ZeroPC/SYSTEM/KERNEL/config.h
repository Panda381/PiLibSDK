
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
// - For device predefined options see _devices/{device}/_config.h.
// - For all available options see config_def.h in root directory.
#include CONFIG_DEF_H				// default configuration

#endif // _CONFIG_H
