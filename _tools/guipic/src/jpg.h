
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                                JPEG file                                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#ifndef _JPG_H
#define _JPG_H

#include "d3timport.h"

///////////////////////////////////////////////////////////////////////////////
// load JPEG file (returns TRUE if OK)

BOOL JPEGLoad(FILE* file, BYTE** dst, int* w, int* h, int* wb, TEXIFMT* fmt);

#endif // _JPG_H
