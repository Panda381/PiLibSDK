
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                            DDS files (DXT textures)                       //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

// Lossy S3 Texture Compression (S3TC):
// ------------------------------------
// Images are divided into 4x4 pixel blocks, "texels" (64 bits each texel)
//		2 color values (2x16=32 bits) - range of pixel colors within the block
//		each pixel is 2 bites (16x2=32 bits) = index into color range
//
// (Warning, color values c0 and c1 are interpreted on some cards /ATI/
//	in the same case as for DXT1 on all other formats too.)
//
// DXT1 - 1 bit alpha (2-bit color index = 3 color pixels, 1 transparent pixel)
//				- (32) two R5G6B5 color values c0 and c1
//						if (c0 > c1):
//							c2 = 2/3*c0 + 1/3*c1
//							c3 = 1/3*c0 + 2/3*c1
//						if (c0 <= c1):
//							c2 = 1/2*c0 + 1/2*c1
//							c3 = transparent black
//				- (32) 4x4 2-bit lookup table (0 to 3 = c0 to c3)
//
// DXT2 - 4 bit alpha (in separate 64b block), color premultiplied by alpha
// DXT3 - 4 bit alpha (in separate 64b block), color data not premultiplied
//				- (64) 4x4 4-bit alpha (0 to 15 = 0 to 255)
//				- (32) two R5G6B5 color values c0 and c1
//							c2 = 2/3*c0 + 1/3*c1
//							c3 = 1/3*c0 + 2/3*c1
//				- (32) 4x4 2-bit lookup table (0 to 3 = c0 to c3)
//
// DXT4 - interpolated alpha, color data premultiplied by alpha
// DXT5 - interpolated alpha, color data not premultiplied
//				- (16) two A8 alpha values a0 and a1
//						if (a0 > a1):
//							a2 = 6/7*a0 + 1/7*a1
//							a3 = 5/7*a0 + 2/7*a1
//							a4 = 4/7*a0 + 3/7*a1
//							a5 = 3/7*a0 + 4/7*a1
//							a6 = 2/7*a0 + 5/7*a1
//							a7 = 1/7*a0 + 6/7*a1
//						if (a0 <= a1):
//							a2 = 4/5*a0 + 1/5*a1
//							a3 = 3/5*a0 + 2/5*a1
//							a4 = 2/5*a0 + 3/5*a1
//							a5 = 1/5*a0 + 4/5*a1
//							a6 = 0
//							a7 = 255
//				- (48) 4x4 3-bit lookup table (0 to 7 = a0 to a7)
//				- (32) two R5G6B5 color values c0 and c1
//							c2 = 2/3*c0 + 1/3*c1
//							c3 = 1/3*c0 + 2/3*c1
//				- (32) 4x4 2-bit lookup table (0 to 3 = c0 to c3)

// Compression:
//	http://www.sjbrown.co.uk/2006/01/19/dxt-compression-techniques/

// GPU decompresssion:
//   http://www.ludicon.com/castano/blog/2009/03/gpu-dxt-decompression/

// http://www.rigsofrods.com/wiki/index.php?title=Making_DDS_textures

// DDS: http://msdn.microsoft.com/en-us/library/bb943982(v=vs.85).aspx

#include <stdio.h>						// I/O functions
#include <malloc.h>						// memory allocation
#include <memory.h>						// memory
#include "dds.h"

///////////////////////////////////////////////////////////////////////////////
// DDS file header (128 bytes)

#pragma pack(push,1)
typedef struct {
	DWORD			Magic;				// 0: magic 0x20534444 (= 'DDS ')
	DWORD			Size;				// 4: size without magic (=124)
	DWORD			Flags;				// 8: flags indicating valid members
	DWORD			Height;				// 12: texture height
	DWORD			Width;				// 16: texture width
	DWORD			Pitch;				// 20: bytes per scan line
	DWORD			Depth;				// 24: depth of volume texture
	DWORD			Mipmaps;			// 28: number of mipmap levels
	DWORD			Res1[11];			// 32:  ...reserved
	DWORD			Size2;				// 76: size 2 (must be 32)
	DWORD			Flags2;				// 80: pixel format flags
	DWORD			FourCC;				// 84: four-character code specifying
										//  compression ('DXT1', 'DXT2',
										//	'DXT3', 'DXT4', 'DXT5')
	DWORD			Bits;				// 88: number of bits per pixel
	DWORD			RMask;				// 92: red (or Y or L) color mask
	DWORD			GMask;				// 96: green (or U) color mask
	DWORD			BMask;				// 100: blue (or V) color mask
	DWORD			AMask;				// 104: alpha mask
	DWORD			Caps1;				// 108: complexity of the surface
	DWORD			Caps2;				// 112: additional details
	DWORD			Res2[3];			// 116:  ...unused
} DDSHEADER;
#pragma pack(pop)

// Pitch should be calculated:
//	(compressed) pitch = max(1, ((width+3)/4)) * block-size
//			block-size is 8 bytes for DXT1/BC1/BC4 or 16 bytes for other
//	(UYVY-packed) pitch = ((width+1) >> 1)*4
//	(uncompressed) pitch = (width*bits-per-pixel+7)/8

// Flags indicating valid members
#define DDS_CAPS			0x1				// Caps1 is valid (required)
#define DDS_HEIGHT			0x2				// Height is valid (required)
#define DDS_WIDTH			0x4				// Width is valid (required)
#define DDS_PITCH			0x8				// Pitch is valid, uncompressed
#define DDS_PIXELFORMAT		0x1000			// pixel format is valid (required)
#define DDS_MIPMAPS			0x20000			// Mipmaps is valid
#define DDS_LINEARSIZE		0x80000			// Pitch is valid, compressed
#define DDS_DEPTH			0x800000		// Depth is valid

// Caps1, complexity of the surface
#define DDSC1_COMPLEX		0x8				// more textures (cube, mipmaps)
#define DDSC1_TEXTURE		0x1000			// must be set
#define DDSC1_MIPMAP		0x400000		// set for mipmaps

// Caps2, additional details
#define DDSC2_CUBEMAP		0x200			// required for cube map
#define DDSC2_CUBEMAP_POSITIVEX	0x400		// required for cube map
#define DDSC2_CUBEMAP_NEGATIVEX	0x800		// required for cube map
#define DDSC2_CUBEMAP_POSITIVEY	0x1000		// required for cube map
#define DDSC2_CUBEMAP_NEGATIVEY	0x2000		// required for cube map
#define DDSC2_CUBEMAP_POSITIVEZ	0x4000		// required for cube map
#define DDSC2_CUBEMAP_NEGATIVEZ	0x8000		// required for cube map
#define DDSC2_VOLUME		0x200000		// required for volume texture

// Pixel format flags
#define	DPF_ALPHAPIXELS		0x1		// texture contains alpha data;
									//  AMask contains valid data
#define DPF_ALPHA			0x2		// alpha channel only uncompressed data
									//  (Bits contains alpha channel bitcount;
									//  AMask contains valid data)
#define DPF_FOURCC			0x4		// texture contains compressed RGB data;
									//  FourCC contains valid data
#define DPF_PALETTE			0x20	// palette indexed
#define DPF_RGB				0x40	// texture contains uncompressed RGB data;
									//  Bits, RMask, GMask, BMask
#define DPF_YUV				0x200	// YUV uncompressed data (Bits
									//	contains the YUV bit count;
									//	RMask contains the Y mask,
									//	GMask contains the U mask,
									//	BMask contains the V mask)
#define DPF_LUMINANCE		0x20000	// single channel color uncompressed data
									//	(Bits contains the luminance
									//	channel bit count; RMask contains
									//	the channel mask). Can be combined with
									//	DPF_ALPHAPIXELS for a two channel
									//	DDS file

#define DPF_ALPHA2 (DPF_ALPHA|DPF_ALPHAPIXELS)

///////////////////////////////////////////////////////////////////////////////
// load DDS file (returns TRUE if OK)

BOOL DDSLoad(FILE* file, BYTE** dst, int* w, int* h, int* wb, TEXIFMT* fmt,
													D3COLRGBA** pal, int* mips)
{
	// load file header
	DDSHEADER dds;
	size_t n = fread(&dds, 1, sizeof(DDSHEADER), file);
	if (n != sizeof(DDSHEADER)) return FALSE;

	// get texture size
	int ww = dds.Width;
	int hh = dds.Height;

	// check header
	if ((dds.Magic != 0x20534444) ||
		(dds.Size < 124) ||
		(dds.Size > 512-4) ||
		(dds.Size2 != 32) ||
		(ww < 1) || (ww > TEXMAXWH) ||
		(hh < 1) || (hh > TEXMAXWH))
		return FALSE;

	// skip rest of head
	if (dds.Size > 124)
		fseek(file, dds.Size - 124, SEEK_CUR);

	// prepare to find data format
	TEXIFMT fmt2 = TEXIFMT_NONE;
	int wb2 = 0;
	*mips = -1;
	int mipN = 1;
	D3COLRGBA* p = NULL;
	BOOL comp = FALSE;

	// RGB is valid
	if ((dds.Flags2 & (DPF_RGB | DPF_LUMINANCE)) != 0)
	{
		dds.Bits = (dds.Bits + 7) & ~7;

		// alpha is valid
		if ((dds.Flags2 & DPF_ALPHA2) != 0)
		{
			// 32 bits
			if ((dds.Bits == 32) &&
				(dds.GMask == 0xff00) &&
				(dds.AMask == 0xff000000))
			{
				// A8R8G8B8, 32 bits with alpha
				if ((dds.RMask == 0xff0000) &&
					(dds.BMask == 0xff))
				{
					fmt2 = TEXIFMT_A8R8G8B8;
				}
			}

			// 16 bits
			if (dds.Bits == 16)
			{
				// A1R5G5B5, 16 bits
				if ((dds.RMask == 0x7c00) &&
					(dds.GMask == 0x3e0) &&
					(dds.BMask == 0x1f) &&
					(dds.AMask == 0x8000))
				{
					fmt2 = TEXIFMT_A1R5G5B5;
				}

				// A4R4G4B4, 16 bits
				if ((dds.RMask == 0xf00) &&
					(dds.GMask == 0xf0) &&
					(dds.BMask == 0xf) &&
					(dds.AMask == 0xf000))
				{
					fmt2 = TEXIFMT_A4R4G4B4;
				}

				// A8L8 luminance with alpha, 16 bits
				if ((dds.RMask == 0xff) &&
					(dds.AMask == 0xff00))
				{
					fmt2 = TEXIFMT_A8L8;
				}
			}
		}

		// alpha is not valid
		else
		{
			// 32 bits
			if ((dds.Bits == 32) &&
				(dds.GMask == 0xff00))
			{
				// X8R8G8B8, 32 bits
				if ((dds.RMask == 0xff0000) &&
					(dds.BMask == 0xff))
				{
					fmt2 = TEXIFMT_X8R8G8B8;
				}
			}

			// 24 bits
			if ((dds.Bits == 24) &&
				(dds.GMask == 0xff00))
			{
				// R8G8B8, 24 bits
				if ((dds.RMask == 0xff0000) &&
					(dds.BMask == 0xff))
				{
					fmt2 = TEXIFMT_R8G8B8;
				}

				// B8G8R8, 24 bits
				if ((dds.RMask == 0xff) &&
					(dds.BMask == 0xff0000))
				{
					fmt2 = TEXIFMT_B8G8R8;
				}
			}

			// 16 bits
			if (dds.Bits == 16)
			{
				// RGB 15 bits, X1R5G5B5
				if ((dds.RMask == 0x7c00) &&
					(dds.GMask == 0x3e0) &&
					(dds.BMask == 0x1f))
				{
					fmt2 = TEXIFMT_X1R5G5B5;
				}
			}

			// 8 bits
			if (dds.Bits == 8)
			{
				// L8 luminance, 8 bits
				if (dds.RMask == 0xff)
				{
					fmt2 = TEXIFMT_L8;
				}
			}
		}
	}
	// RGB is not valid
	else
	{
		// compressed data
		if ((dds.Flags2 & DPF_FOURCC) != 0)
		{
			// DXT1C
			if (dds.FourCC == 0x30545844)
			{
				fmt2 = TEXIFMT_DXT1C;
				comp = TRUE;
				dds.Bits = 4;
			}

			// DXT1A
			if (dds.FourCC == 0x31545844)
			{
				fmt2 = TEXIFMT_DXT1A;
				comp = TRUE;
				dds.Bits = 4;
			}

			// DXT3
			if (dds.FourCC == 0x33545844)
			{
				fmt2 = TEXIFMT_DXT3;
				comp = TRUE;
				dds.Bits = 8;
			}

			// DXT5
			if (dds.FourCC == 0x35545844)
			{
				fmt2 = TEXIFMT_DXT5;
				comp = TRUE;
				dds.Bits = 8;
			}

			// A8L8 (= 'AG8 ')
			if (dds.FourCC == 0x20384741)
			{
				fmt2 = TEXIFMT_A8L8;
				dds.Bits = 16;
			}
		}
		else
		{
			// palettes
			if (((dds.Flags2 & DPF_PALETTE) != 0) &&
				(dds.Bits > 0) && (dds.Bits <= 8))
			{
				// prepare buffer
				p = (D3COLRGBA*)malloc(256*sizeof(D3COLRGBA));
				if (p == NULL) return FALSE;
				int s = (1 << dds.Bits) * sizeof(D3COLRGBA);
				n = fread(p, 1, s, file);
				if ((int)n != s)
				{
					free(p);
					return FALSE;
				}
				for (int i = 255; i >= 0; i--)
				{
					BYTE c = p[i].b;
					p[i].b = p[i].r;
					p[i].r = c;
				}
				fmt2 = TEXIFMT_PAL8A;
				dds.Bits = 8;
			}
		}
	}

	// check format
	if (fmt2 == TEXIFMT_NONE)
	{
		free(p);
		return FALSE;
	}

	// data size
	wb2 = (ww*dds.Bits+7)/8;
	int ww2 = ww;
	int hh2 = hh;
	int size = 0;

	// mipmaps
	mipN = ((dds.Flags & DDS_MIPMAPS) != 0) ? dds.Mipmaps : 1;
	if (mipN == 0) mipN = 1;
	for (int m = mipN; m > 0; m--)
	{
		if (comp)
			size += ((ww2+3)/4)*((hh2+3)/4)*dds.Bits*2;
		else
			size += (ww2*dds.Bits+7)/8*hh2;
		ww2 /= 2;
		if (ww2 == 0) ww2 = 1;
		hh2 /= 2;
		if (hh2 == 0) hh2 = 1;
	}

	// prepare buffer
	BYTE* buf = (BYTE*)malloc(size);
	if (buf == NULL)
	{
		free(p);
		return FALSE;
	}

	// load image data
	n = fread(buf, 1, size, file);
	if ((int)n != size)
	{
		free(p);
		free(buf);
		return FALSE;
	}

	// get output parameters
	*dst = buf;
	*w = ww;
	*h = hh;
	*wb = wb2;
	*fmt = fmt2;
	*mips = mipN;
	*pal = p;
	return TRUE;
}
