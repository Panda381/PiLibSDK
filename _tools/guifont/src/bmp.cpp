
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                                 BMP file                                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
// (c) Miroslav Nemecek

#include <stdio.h>						// I/O functions
#include <malloc.h>						// memory allocation
#include <memory.h>						// memory
#include "bmp.h"

#pragma pack(push,1)
typedef struct _bmpBITMAPFILEHEADER {
		WORD	bfType;
		DWORD	bfSize;
		WORD	bfReserved1;
		WORD	bfReserved2;
		DWORD	bfOffBits;
} bmpBITMAPFILEHEADER;

typedef struct _bmpBITMAPINFOHEADER{
		DWORD	biSize;
		long	biWidth;
		long	biHeight;
		WORD	biPlanes;
		WORD	biBitCount;
		DWORD	biCompression;
		DWORD	biSizeImage;
		long	biXPelsPerMeter;
		long	biYPelsPerMeter;
		DWORD	biClrUsed;
		DWORD	biClrImportant;
} bmpBITMAPINFOHEADER;
#pragma pack(pop)

#define bmpBI_RGB	0
#define bmpBI_RLE8	1
#define bmpBI_RLE4	2

////////////////////////////////////////////////////////////////////
// BI_RLE8 data decompression

void DekompRLE8(BYTE* dst, int dstN, BYTE* src, int srcN, int wb)
{
	BYTE data;				// one data byte
	BYTE num;				// repeat counter
	int lineoff = 0;		// pixel offset on a line

	// while there is some data (it is only a rough control)
	while ((dstN > 0) && (srcN > 0))
	{
		// load first and second byte
		num = *src;
		src++;
		srcN--;

		data = *src;
		src++;
		srcN--;

		// repeat byte
		if (num != 0)
		{
			if ((int)num > dstN) num = (BYTE)dstN;
			lineoff += num;
			dstN -= num;

			memset(dst, data, num);
			dst += num;
		}
		else
		{
			// copy bytes without a change
			if (data > 2)
			{
				if ((int)data > srcN) data = (BYTE)srcN;
				if ((int)data > dstN) data = (BYTE)dstN;
				lineoff += data;
				dstN -= data;
				srcN -= data;

				memcpy(dst, src, data);
				dst += data;
				src += (data + 1) & ~1;
			}
			else
			{
				// escape code - end of line
				if (data == 0)
				{
					int i = wb - lineoff;
					if (i > dstN) i = dstN;
					if (i > 0)
					{
						dstN -= i;
						memset(dst, 0, i);
						dst += i;
					}
					lineoff = 0;
				}
				else
				{
					// escape code - end of bitmap
					if (data == 1) break;
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// load BMP file (returns TRUE if OK)

BOOL BMPLoad(FILE* file, BYTE** dst, int* w, int* h, int* wb, TEXIFMT* fmt,
												D3COLRGBA** pal, BOOL* vflip)
{
	// load BMP file header
	bmpBITMAPFILEHEADER bmh;
	size_t n = fread(&bmh, 1, sizeof(bmpBITMAPFILEHEADER), file);
	if (n != sizeof(bmpBITMAPFILEHEADER)) return FALSE;

	// data offset and data size
	int off = bmh.bfOffBits - sizeof(bmpBITMAPFILEHEADER)
							- sizeof(bmpBITMAPINFOHEADER);
	int size = bmh.bfSize - bmh.bfOffBits;

	// check BMP file header
	if ((bmh.bfType != 0x4d42) || (off < 0) || (off > 256*4 + 1000))
		return FALSE;

	// load BMP info header
	bmpBITMAPINFOHEADER bmi;
	n = fread(&bmi, 1, sizeof(bmpBITMAPINFOHEADER), file);
	if (n != sizeof(bmpBITMAPINFOHEADER)) return FALSE;

	// skip rest of info header
	int s = bmi.biSize - sizeof(bmpBITMAPINFOHEADER);
	if (s > 0)
	{
		fseek(file, s, SEEK_CUR);
		off -= s;
	}

	// get picture size
	int ww = bmi.biWidth;
	int hh = bmi.biHeight;
	int b = bmi.biBitCount;
	int c = bmi.biCompression;
	int wb2 = ((ww*b+7)/8 + 3) & ~3;
	BOOL vf = TRUE;
	if (hh < 0)
	{
		vf = FALSE;
		hh = -hh;
	}

	// check BMP info header
	if ((bmi.biSize < sizeof(bmpBITMAPINFOHEADER)) ||
		(ww < 1) || (ww > TEXMAXWH) ||
		(hh < 1) || (hh > TEXMAXWH) ||
		(bmi.biPlanes != 1) ||
		((b != 8) && (b != 16) && (b != 24) && (b != 32)) ||
		((c != bmpBI_RGB) &&
		((c != bmpBI_RLE8) || (b != 8))) ||
		(size < ((b > 8) ? (wb2*hh) : 1)) || (size > (wb2+wb2/2)*hh+100))
		return FALSE;

	// load palettes
	D3COLRGBA* p = NULL;
	if (b <= 8)
	{
		s = 256*sizeof(D3COLRGBA);
		p = (D3COLRGBA*)malloc(s);
		if (p == NULL) return FALSE;
		if (s > off) s = off;
		n = fread(p, 1, s, file);
		if ((int)n != s)
		{
			free(p);
			return FALSE;
		}
		off -= s;
	}

	// skip rest of head
	if (off > 0) fseek(file, off, SEEK_CUR);

	// prepare buffer
	BYTE* buf = (BYTE*)malloc(size);
	if (buf == NULL)
	{
		free(p);
		return FALSE;
	}

	// load BMP data
	n = fread(buf, 1, size, file);
	if ((int)n != size)
	{
		free(p);
		free(buf);
		return FALSE;
	}

	// RLE decompression
	if ((b == 8) && (c == bmpBI_RLE8))
	{
		int size2 = wb2*hh;
		BYTE* buf2 = (BYTE*)malloc(size2);
		if (buf2 == NULL)
		{
			free(p);
			free(buf);
			return FALSE;
		}
		DekompRLE8(buf2, size2, buf, size, wb2);
		size = size2;
		free(buf);
		buf = buf2;
	}

	// prepare output format
	switch (b)
	{
	case 8: *fmt = TEXIFMT_PAL8; break;
	case 16: *fmt = TEXIFMT_X1R5G5B5; break;
	case 24: *fmt = TEXIFMT_R8G8B8; break;
	case 32: *fmt = TEXIFMT_X8R8G8B8; break;
	}

	// set output parameters
	*dst = buf;
	*w = ww;
	*h = hh;
	*wb = wb2;
	*pal = p;
	*vflip = vf;
	return TRUE;
}
