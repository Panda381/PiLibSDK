
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                                 PNG file                                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#ifndef _PNG_H
#define _PNG_H

#include "d3color.h"
#include "d3timport.h"

///////////////////////////////////////////////////////////////////////////////
// load PNG file (returns TRUE if OK)

BOOL PNGLoad(FILE* file, BYTE** dst, int* w, int* h, int* wb, TEXIFMT* fmt,
										D3COLRGBA** pal, D3COLRGBA** trans);

#endif // _PNG_H
