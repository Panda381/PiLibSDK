
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                                 BMP file                                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#ifndef _BMP_H
#define _BMP_H

#include "d3color.h"
#include "d3timport.h"

///////////////////////////////////////////////////////////////////////////////
// load BMP file (returns TRUE if OK)

BOOL BMPLoad(FILE* file, BYTE** dst, int* w, int* h, int* wb, TEXIFMT* fmt,
												D3COLRGBA** pal, BOOL* vflip);

#endif // _BMP_H
