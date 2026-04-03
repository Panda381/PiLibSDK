
// ****************************************************************************
//
//                              PNG images
//
// ****************************************************************************

#if USE_PNG		// 1=use PNG support, 0=not used (_lib/pic/lib_png.*)

#ifndef _LIB_PNG_H
#define _LIB_PNG_H

// decode PNG image from stream to image in CF_A8B8G8R8, CF_B8G8R8 or CF_A8 format
//  Returns sPic* image (allocated with MemAlloc()) or NULL on error.
sPic* PNGLoadStream(sStream* str);

// decode JPEG image from file to image in CF_A8B8G8R8, CF_B8G8R8 or CF_A8 format
//  Returns sPic* image (allocated with MemAlloc()) or NULL on error.
sPic* PNGLoadFile(sFile* file);

// decode JPEG image from buffer to image in CF_A8B8G8R8, CF_B8G8R8 or CF_A8 format
//  Returns sPic* image (allocated with MemAlloc()) or NULL on error.
sPic* PNGLoadBuf(const void* buf, int size);
#define PNGLOAD(buf) PNGLoadBuf((buf), sizeof(buf))

#endif // _LIB_PNG_H

#endif // USE_PNG
