
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                                3D color                                   //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#ifndef _D3COLOR_H
#define _D3COLOR_H

#include "def.h"				// base definitions

///////////////////////////////////////////////////////////////////////////////
// 3D color as DWORD (a=255 if opaque, 0 if transparency,
//		entries are in range 0..255) (compatible with D3DCOLOR_RGBA)

typedef DWORD D3COLOR;

#define D3RGBA(r,g,b,a) ((DWORD)((((a) & 0xff) << 24) | \
				(((r) & 0xff) << 16) | (((g) & 0xff) << 8) | ((b) & 0xff)))
#define D3RGB(r,g,b) D3RGBA(r,g,b,255)

#define D3RGB_R(rgb) (((rgb) >> 16) & 0xff)
#define D3RGB_G(rgb) (((rgb) >> 8) & 0xff)
#define D3RGB_B(rgb) ((rgb) & 0xff)
#define D3RGB_A(rgb) ((rgb) >> 24)

// 3D color as QUADRUPLE (palette entry)
typedef struct _D3COLRGBA {
		BYTE	b;
		BYTE	g;
		BYTE	r;
		BYTE	a;
} D3COLRGBA;

#endif // _D3COLOR_H
