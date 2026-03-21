
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                             3D Texture Import                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#ifndef _D3TIMPORT_H
#define _D3TIMPORT_H

#include "def.h"				// base definitions

#define TEXMAXWH	4096	// texture maximal width and height

///////////////////////////////////////////////////////////////////////////////
// imported texture data formats

typedef enum TEXIFMT {
	TEXIFMT_NONE,			// unknown format
	TEXIFMT_X8R8G8B8,		// (BMP) X8R8G8B8, 32 bits (8 bits unused)
	TEXIFMT_R8G8B8,			// (BMP, TGA, PNG) R8G8B8, 24 bits
	TEXIFMT_B8G8R8,			// (JPG) B8G8R8, 24 bits
	TEXIFMT_X1R5G5B5,		// (BMP) RGB 15 bits, R5+G5+B5
	TEXIFMT_PAL8,			// (BMP, TGA, PNG) 8 bits with palettes D3COLRGBA
	TEXIFMT_L8,				// (TGA, PNG, JPG) L8 luminance, 8 bits
	TEXIFMT_A8R8G8B8,		// (TGA, PNG) A8R8G8B8, 32 bits with alpha
	TEXIFMT_PAL8A,			// (PNG) 8 bits with palettes with 8-bit alpha
	TEXIFMT_A1R5G5B5,		// (TGA) A1R5G5B5, 16 bits
	TEXIFMT_A8L8,			// (PNG) A8L8 luminance with alpha, 16 bits
};

#endif // _D3TIMPORT_H
