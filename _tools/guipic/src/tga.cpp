
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                                 TGA file                                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
// (c) Miroslav Nemecek

#include <stdio.h>						// I/O functions
#include <malloc.h>						// memory allocation
#include <memory.h>						// memory
#include "tga.h"
#include "d3timport.h"

// TGA header format (18 bytes, followed by identification string)
#pragma pack(push,1)
typedef struct {
	BYTE	IDLength;			// 0: length of identification string
	BYTE	ColorMapType;		// 1: color map type (1=color map, 0=none)
	BYTE	DataTypeCode;		// 2: data type code
								//		0=no image
								//		1=uncompressed, color-mapped
								//		2=uncompressed, RGB
								//		3=uncompressed, B&W
								//		9=RLE color-mapped
								//		10=RLE RGB
								//		11=RLE B&W
	WORD	ColourMapOrigin;	// 3: index of first color map entry
	WORD	ColourMapLength;	// 5: count of color map entries
	BYTE	ColourMapDepth;		// 7: number of bits in each color map entry
								//	  (16:A1R5G5B5, 24:R8G8B8, 32:A8R8G8B8)
	WORD	X_Origin;			// 8: X coordinate of lower left corner
	WORD	Y_Origin;			// 10: Y coordinate of lower left corner
	WORD	Width;				// 12: width of image
	WORD	Height;				// 14: height of image
	BYTE	BitsPerPixel;		// 16: bits per each color value
								//	(16:A1R5G5B5, 24:R8G8B8, 32:A8R8G8B8)
	BYTE	ImageDescriptor;	// 17: image descriptor
								//		bit 0..3: number of alpha channel bits
								//				associated with each pixel
								//				0 or 1 for TGA16, 0 for TGA24,
								//				8 for TGA32
								//		- invalid from some apps,incl.Photoshop
								//		bit 4:	0=origin in left, 1=right
								//		bit 5:	0=origin in bottom, 1=top
								//		bit 6,7: interleaving
								//				0=non interleaved
								//				1=even/odd interleaving
								//				2=four way interleaving
	// - identification string follows (0 to 255 characters)
	// - color map data follows (16, 24 or 32 bits per color entry)
	// - image data follows
	//		RLE:	bit 7:	1=repeat following pixel 1 to 128
	//						0=copy following pixels 1 to 128
} TGAHEADER;
#pragma pack(pop)

///////////////////////////////////////////////////////////////////////////////
// TGA RLE data decompression (returns size of unused input data)
//	dst = destination buffer
//	dstN = size of destination buffer
//	src = source data buffer
//	srcN = size of source data
//	b = number of bytes per pixel 1, 2, 3 or 4

int DekompTGA(BYTE* dst, int dstN, BYTE* src, int srcN, int b)
{
	int num;				// repeat counter

	// while there is some data
	while ((dstN > 0) && (srcN > b))
	{
		// load head of data packet
		num = *src++;
		srcN--;

		// repeat pixel
		if ((num & 0x80) != 0)
		{
			num = (num & 0x7f) + 1;
			if (num*b > dstN) num = dstN/b;
			dstN -= num*b;
			srcN -= b;
			switch (b)
			{
			case 1:
				memset(dst, *src++, num);
				dst += num;
				break;

			case 2:
				{
					WORD d = *(WORD*)src;
					src += 2;
					for (; num > 0; num--)
					{
						*(WORD*)dst = d;
						dst += 2;
					}
				}
				break;

			case 3:
				{
					WORD d = *(WORD*)src;
					BYTE d2 = src[2];
					src += 3;
					for (; num > 0; num--)
					{
						*(WORD*)dst = d;
						dst[2] = d2;
						dst += 3;
					}
				}
				break;

			case 4:
				{
					DWORD d = *(DWORD*)src;
					src += 4;
					for (; num > 0; num--)
					{
						*(DWORD*)dst = d;
						dst += 4;
					}
				}
				break;
			}
		}
		else
		{
			// copy bytes without a change
			num++;
			if (num*b > dstN) num = dstN/b;
			if (num*b > srcN) num = srcN/b;
			num *= b;
			dstN -= num;
			srcN -= num;
			memcpy(dst, src, num);
			dst += num;
			src += num;
		}
	}
	return srcN;
}

///////////////////////////////////////////////////////////////////////////////
// load TGA file (returns TRUE if OK)
// size = max. file size if file is with RLE compression, ignored otherwise

BOOL TGALoad(FILE* file, BYTE** dst, int* w, int* h, int* wb, TEXIFMT* fmt,
								D3COLRGBA** pal, BOOL* vflip, int size)
{
	// load TGA header
	TGAHEADER tga;
	size_t n = fread(&tga, 1, sizeof(TGAHEADER), file);
	if (n != sizeof(TGAHEADER)) return FALSE;

	// get picture parameters
	int ww = tga.Width;				// width
	int hh = tga.Height;			// height
	int b = tga.BitsPerPixel;		// pixel bits
	int c = tga.DataTypeCode;		// compression
	BOOL vf = (tga.ImageDescriptor & (1<<5)) == 0; // vertical flip
	size -= sizeof(TGAHEADER) + tga.IDLength;	// RLE data size
	if (size < 0) size = 0;
	if (b == 15) b = 16;
	int ps = tga.ColourMapOrigin;	// first palette entry
	int pn = tga.ColourMapLength;	// number of palette entries
	int pb = tga.ColourMapDepth;	// bits per palette entry

	// prepare data format
	*fmt = TEXIFMT_NONE;
	BOOL rle = FALSE;

	// PAL 8 bit
	if ((tga.ColorMapType == 1) && (ps < 256) && (pn <= 256) &&
		(ps + pn <= 256) && (pb == 24) && (b == 8))
	{
		if (c == 1) *fmt = TEXIFMT_PAL8;
		if (c == 9) { *fmt = TEXIFMT_PAL8; rle  = TRUE; }
	}

	// RGB
	if ((tga.ColorMapType == 0) && (pn == 0))
	{
		if ((c == 2) || (c == 10))
		{
			if (c == 10) rle = TRUE;
			if (b == 16) *fmt = TEXIFMT_A1R5G5B5;
			if (b == 24) *fmt = TEXIFMT_R8G8B8;
			if (b == 32) *fmt = TEXIFMT_A8R8G8B8;
		}

		if (((c == 3) || (c == 11)) && (b == 8))
		{
			if (c == 11) rle = TRUE;
			*fmt = TEXIFMT_L8;
		}
	}

	// check TGA header
	if ((*fmt == TEXIFMT_NONE) ||
		(ww < 1) || (ww > TEXMAXWH) ||
		(hh < 1) || (hh > TEXMAXWH))
		return FALSE;

	// skip identification string
	if (tga.IDLength > 0) fseek(file, tga.IDLength, SEEK_CUR);

	// load color map
	D3COLRGBA* p = NULL;
	if (pn > 0)
	{
		BYTE p2[3*256];
		n = fread(p2, 1, pn*3, file);
		if ((int)n != pn*3) return FALSE;

		size -= pn*3;
		if (size < 0) size = 0;

		p = (D3COLRGBA*)malloc(256*sizeof(D3COLRGBA));
		if (p == NULL) return FALSE;
		memset(p, 0, 256*sizeof(D3COLRGBA));

		for (int i = 0; i < pn; i++)
		{
			p[i+ps].b	= p2[i*3+0];
			p[i+ps].g	= p2[i*3+1];
			p[i+ps].r	= p2[i*3+2];
			p[i+ps].a = 255;
		}
	}

	// data size
	b /= 8;
	int wb2 = ww*b;
	if (!rle)
		size = wb2*hh;
	else
		if (size > wb2*hh*3/2) size = wb2*hh*3/2;

	// prepare buffer
	BYTE* buf = (BYTE*)malloc(size);
	if (buf == NULL)
	{
		free(p);
		return FALSE;
	}

	// load data
	n = fread(buf, 1, size, file);
	if ((int)n != size)
	{
		free(p);
		free(buf);
		return FALSE;
	}

	// decompression
	if (rle)
	{
		int size2 = wb2*hh;
		BYTE* buf2 = (BYTE*)malloc(size2);
		if (buf2 == NULL)
		{
			free(p);
			free(buf);
			return FALSE;
		}
		size2 = DekompTGA(buf2, size2, buf, size, b);
		free(buf);
		buf = buf2;
		if (size2 > 0) fseek(file, -size2, SEEK_CUR);
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
