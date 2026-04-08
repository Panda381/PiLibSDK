
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
#define WIDTH		320		// draw width (0=do not initialize default videomode)
#define HEIGHT		240		// draw height
#define SCREENSCALE	2		// 0 = no back buffer, or use back buffer with screen scale 1, 2 or 4

// Real numbers (also set USE_REAL=1 and USE_BIGINT=1 in Makefile to enable it)
//#define USE_REAL16		1		// 1 = use real16 numbers (3 digits, exp +-4)
//#define USE_REAL32		1		// 1 = use real32 numbers (float, 7 digits, exp +-38)
//#define USE_REAL48		1		// 1 = use real48 numbers (11 digits, exp +-153)
//#define USE_REAL64		1		// 1 = use real64 numbers (double, 16 digits, exp +-308)
//#define USE_REAL80		1		// 1 = use real80 numbers (19 digits, exp +-4932)
//#define USE_REAL96		1		// 1 = use real96 numbers (25 digits, exp +-1233)
//#define USE_REAL128		1		// 1 = use real128 numbers (34 digits, exp +-4932)
//#define USE_REAL160		1		// 1 = use real160 numbers (43 digits, exp +-9864)
//#define USE_REAL192		1		// 1 = use real192 numbers (52 digits, exp +-19728)
//#define USE_REAL256		1		// 1 = use real256 numbers (71 digits, exp +-78913)
//#define USE_REAL384		1		// 1 = use real384 numbers (109 digits, exp +-315652)
//#define USE_REAL512		1		// 1 = use real512 numbers (147 digits, exp +-1262611)
//#define USE_REAL768		1		// 1 = use real768 numbers (224 digits, exp +-5050445)
//#define USE_REAL1024		1		// 1 = use real1024 numbers (300 digits, exp +-20201781)
//#define USE_REAL1536		1		// 1 = use real1536 numbers (453 digits, exp +-161614248)
//#define USE_REAL2048		1		// 1 = use real2048 numbers (607 digits, exp +-161614248)
//#define USE_REAL3072		1		// 1 = use real3072 numbers (915 digits, exp +-161614248)
//#define USE_REAL4096		1		// 1 = use real4096 numbers (1224 digits, exp +-161614248)
//#define USE_REAL6144		1		// 1 = use real6144 numbers (1841 digits, exp +-161614248)
//#define USE_REAL8192		1		// 1 = use real8192 numbers (2457 digits, exp +-161614248))
//#define USE_REAL12288		1		// 1 = use real12288 numbers (3690 digits, exp +-161614248)

// Include default configuration
// - For device predefined options see _devices/{device}/_config.h.
// - For all available options see config_def.h in root directory.
#include CONFIG_DEF_H				// default configuration

#endif // _CONFIG_H
