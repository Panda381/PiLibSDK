
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                            DDS files (DXT textures)                       //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#ifndef _DDS_H
#define _DDS_H

#include "d3color.h"
#include "d3timport.h"

///////////////////////////////////////////////////////////////////////////////
// load DDS file (returns TRUE if OK)

BOOL DDSLoad(FILE* file, BYTE** dst, int* w, int* h, int* wb, TEXIFMT* fmt,
												D3COLRGBA** pal, int* mips);

#endif // _DDS_H
