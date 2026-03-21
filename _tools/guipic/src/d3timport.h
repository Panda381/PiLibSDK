
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
// BMP format: TEXIFMT_X8R8G8B8, TEXIFMT_R8G8B8, TEXIFMT_X1R5G5B5, TEXIFMT_PAL8
// TGA format: TEXIFMT_R8G8B8, TEXIFMT_PAL8, TEXIFMT_L8, TEXIFMT_A8R8G8B8, TEXIFMT_A1R5G5B5
// JPG format: TEXIFMT_B8G8R8, TEXIFMT_L8
// PNG format: TEXIFMT_R8G8B8, TEXIFMT_PAL8, TEXIFMT_L8, TEXIFMT_A8R8G8B8, TEXIFMT_PAL8A, TEXIFMT_A8L8
// DDS format: TEXIFMT_A8R8G8B8, TEXIFMT_A1R5G5B5, TEXIFMT_A4R4G4B4, TEXIFMT_A8L8,
//				TEXIFMT_X8R8G8B8, TEXIFMT_R8G8B8, TEXIFMT_B8G8R8, TEXIFMT_X1R5G5B5,
//				TEXIFMT_L8, TEXIFMT_DXT1C, TEXIFMT_DXT1A, TEXIFMT_DXT3, TEXIFMT_DXT5

typedef enum TEXIFMT {
	TEXIFMT_NONE,			// unknown format
	TEXIFMT_X8R8G8B8,		// (BMP, DDS) X8R8G8B8, 32 bits (8 bits unused)
	TEXIFMT_R8G8B8,			// (BMP, TGA, PNG, DDS) R8G8B8, 24 bits
	TEXIFMT_B8G8R8,			// (JPG, DDS) B8G8R8, 24 bits

	TEXIFMT_X1R5G5B5,		// (BMP, DDS) RGB 15 bits, R5+G5+B5
	TEXIFMT_PAL8,			// (BMP, TGA, PNG) 8 bits with palettes D3COLRGBA
	TEXIFMT_L8,				// (TGA, PNG, JPG, DDS) L8 luminance, 8 bits
	TEXIFMT_A8R8G8B8,		// (TGA, PNG, DDS) A8R8G8B8, 32 bits with alpha
	TEXIFMT_PAL8A,			// (PNG) 8 bits with palettes with 8-bit alpha
	TEXIFMT_A1R5G5B5,		// (TGA, DDS) A1R5G5B5, 16 bits
	TEXIFMT_A4R4G4B4,		// (DDS) A4R4G4B4, 16 bits with alpha
	TEXIFMT_A8L8,			// (PNG, DDS) A8L8 luminance with alpha, 16 bits

	TEXIFMT_DXT1C,			// DXT1 without alpha, 4 bits/pixel
	TEXIFMT_DXT1C2,			// DXT1 without alpha, 4 bits/pixel, double sized (not used, only to validate index)
	TEXIFMT_DXT1A,			// DXT1, 1 bit alpha, 4 bits/pixel
	TEXIFMT_DXT3,			// DXT3, 4 bits alpha, not premultiplied, 8b/pixel
	TEXIFMT_DXT5,			// DXT5, interpolated alpha, not premultiplied,8b/p
};

#endif // _D3TIMPORT_H
