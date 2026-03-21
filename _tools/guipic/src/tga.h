
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                                 TGA file                                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#ifndef _TGA_H
#define _TGA_H

#include "d3color.h"
#include "d3timport.h"

///////////////////////////////////////////////////////////////////////////////
// load TGA file (returns TRUE if OK)
// size = max. file size if file is with RLE compression, ignored otherwise

BOOL TGALoad(FILE* file, BYTE** dst, int* w, int* h, int* wb, TEXIFMT* fmt,
									D3COLRGBA** pal, BOOL* vflip, int size);

#endif // _TGA_H
