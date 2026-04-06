
// ****************************************************************************
//
//                              JPG images
//
// ****************************************************************************

#if USE_JPG		// 1=use JPG support, 0=not used (_lib/pic/lib_jpg*)

#ifndef _LIB_JPG_H
#define _LIB_JPG_H

// decode JPG image from stream to image in CF_B8G8R8 or CF_A8 format
//  Returns sPic* image (allocated with MemAlloc()) or NULL on error.
sPic* JPGLoadStream(sStream* str);

#if USE_FAT	// 1=use FAT file system, 0=not used (lib_fat.*)
// decode JPG image from file to image in CF_B8G8R8 or CF_A8 format
//  Returns sPic* image (allocated with MemAlloc()) or NULL on error.
sPic* JPGLoadFile(sFile* file);
#endif // USE_FAT

// decode JPG image from buffer to image in CF_B8G8R8 or CF_A8 format
//  Returns sPic* image (allocated with MemAlloc()) or NULL on error.
sPic* JPGLoadBuf(const void* buf, int size);
#define JPGLOAD(buf) JPGLoadBuf((buf), sizeof(buf))

#endif // _LIB_JPG_H

#endif // USE_JPG
