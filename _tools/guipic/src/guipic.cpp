
#include <stdio.h>
#include <string.h>
#include <malloc.h>						// memory allocation
#include <memory.h>						// memory
#include "bmp.h"
#include "tga.h"
#include "jpg.h"
#include "png.h"
#include "dds.h"
#include "d3timport.h"

#define PALNOTRANS 0x100

// input file
BYTE* buf = NULL;
int w, h, wb, mips;
TEXIFMT fmt;
BOOL vflip = FALSE;
D3COLRGBA* pal = NULL;
D3COLRGBA* trans = NULL;
int transinx = PALNOTRANS;
FILE* f = NULL;
int forminx;

const char* formname[] = {
	"CF_R8G8B8",
	"CF_R5G6B5",
	"CF_R3G3B2",
	"CF_A8B8G8R8",
	"CF_A8R8G8B8",
	"CF_A1R5G5B5",
	"CF_A2R5G5B4",
	"CF_A4R4G4B4",
	"CF_A8",
	"CF_A4",
	"CF_A3",
	"CF_A2",
	"CF_A1",
	"CF_DXT1C",
	"CF_DXT1C2",
	"CF_DXT1A",
	"CF_DXT3",
	"CF_DXT5",
	"CF_PAL",
	"CF_PALUNI",
	"CF_PALUNIT",
	"CF_PALUNI2",
	"CF_PALUNI2T",
	"CF_PALGEM",
	"CF_PALGEMT",
	"CF_PAL332",
	"CF_PALCOMP",
	"CF_PAL1",
	"CF_PAL2",
	"CF_PAL3",
	"CF_PAL4",
	"CF_PAL5",

	"GENPAL",
	"UNI",
	"UNIT",
	"UNI2",
	"UNI2T",
	"332",
	"COMP",
};

// picture color format
enum COLFMT {
	// without alpha
	CF_R8G8B8,			// (3B) R8G8B8, 24 bits RGB
	CF_R5G6B5,			// (2B) R5G6B5, 16 bits RGB
	CF_R3G3B2,			// (1B) R3G3B2, 8 bits RGB

	// with alpha
	CF_A8B8G8R8,		// (4B) A8B8G8R8, 24 bits BGR with 8 bits alpha
	CF_A8R8G8B8,		// (4B) A8R8G8B8, 24 bits RGB with 8 bits alpha
	CF_A1R5G5B5,		// (2B) A1R5G5B5, 15 bits RGB with 1 bit alpha
	CF_A2R5G5B4,		// (2B) A2R5G5B4, 14 bits RGB with 2 bits alpha
	CF_A4R4G4B4,		// (2B) A4R4G4B4, 12 bits RGB with 4 bits alpha
	CF_A8,				// (1B) A8, 8 bits alpha
	CF_A4,				// (1B) A4, 4 bits alpha
	CF_A3,				// (1B) A3, 2+3+3 bits alpha
	CF_A2,				// (1B) A2, 2 bits alpha
	CF_A1,				// (1B) A1, 1 bit alpha

	// compression
	CF_DXT1C,			// (1/2B) DXT1 compression, 8 byte texel 4x4 without alpha
	CF_DXT1C2,			// (1/2B) DXT1 compression, 8 byte texel 4x4 without alpha, double sized
	CF_DXT1A,			// (1/2B) DXT1 compression, 8 byte texel 4x4 with 1-bit alpha
	CF_DXT3,			// (1B) DXT3 compression, 16 byte texel 4x4 with 4-bit alpha
	CF_DXT5,			// (1B) DXT5 compression, 16 byte texel 4x4 with interpolated alpha

	// palettes
	CF_PAL,				// (1B) 8 bits color with internal palettes (in front of data)
	CF_PALUNI,			// (1B) 8 bits color with uniform palettes (6*6*6)
	CF_PALUNIT,			// (1B) 8 bits color with uniform palettes (6*6*6) with transparency
	CF_PALUNI2,			// (1B) 8 bits color with uniform palettes 2 (6*7*6)
	CF_PALUNI2T,		// (1B) 8 bits color with uniform palettes 2 (6*7*6) with transparency
	CF_PALGEM,			// (1B) 8 bits color with Gemtree palettes
	CF_PALGEMT,			// (1B) 8 bits color with Gemtree palettes with transparency
	CF_PAL332,			// (1B) 8 bits color with uniform palettes CF_R3G3B2 (8*8*4)
	CF_PALCOMP,			// (1B) 8 bits color with Compass palettes R2G2B2W1 (2*2*2+1)
	CF_PAL1,			// (1B) 8 bits color with palette set 1
	CF_PAL2,			// (1B) 8 bits color with palette set 2
	CF_PAL3,			// (1B) 8 bits color with palette set 3
	CF_PAL4,			// (1B) 8 bits color with palette set 4
	CF_PAL5,			// (1B) 8 bits color with palette set 5

	// commands
	CF_GENPAL,			// generate palettes
	CF_UNI,				// write uniform palettes
	CF_UNIT,			// write uniform palettes with transparency
	CF_UNI2,			// write uniform palettes 2
	CF_UNI2T,			// write uniform palettes 2 with transparency
	CF_332,				// write uniform palettes G3R3B2
	CF_COMP,			// write uniform palettes COMPASS

	CMDNUM
};

int pos = 0;

void writeB(BYTE b)
{
	fprintf(f, "0x%02x,", b);
	pos++;
	if (pos == 16)
	{
		fprintf(f, "\n\t");
		pos = 0;
	}
}

void writeW(WORD w)
{
	writeB((BYTE)w);
	writeB((BYTE)(w >> 8));
}

// default EGA palettes
BYTE DefEGAPalRGB[3*16] = {
	0,0,0,			// 0: black
	0,0,170,		// 1: blue
	0,170,0,		// 2: green
	0,170,170,		// 3: cyan
	170,0,0,		// 4: red
	170,0,170,		// 5: magenta
	170,170,0,		// 6: brown
	170,170,170,	// 7: white
	85,85,85,		// 8: dark gray
	85,85,255,		// 9: light blue
	85,255,85,		// 10: light green
	85,255,255,		// 11: light cyan
	255,85,85,		// 12: light red
	255,85,255,		// 13: light magenta
	255,255,85,		// 14: yellow
	255,255,255,	// 15: light white
};

int main(int argc, char* argv[])
{
	int i, bs, b, wb2, x, y;
	BYTE* s;

	// parse output format
	forminx = -1;
	if (argc == 5)
	{
		for (i = 0; i < CMDNUM; i++)
		{
			if (strcmp(argv[4], formname[i]) == 0)
			{
				forminx = i;
				break;
			}
		}
	}

	// check arguments
	if ((argc != 5) || (forminx < 0))
	{
		printf( "Syntax: guipic input_file output_file name format\n"
				"     input_file = BMP, PNG, JPG, DDS or TGA file\n"
				"     output_file = C source file\n"
				"     name = name of picture structure\n"
				"     format (or command)\n"
				"        without alpha: CF_R8G8B8, CF_R5G6B5, CF_R3G3B2\n"
				"        with alpha: CF_A8B8G8R8, CF_A8R8G8B8, CF_A1R5G5B5, CF_A2R5G5B4\n"
				"                    CF_A4R4G4B4, CF_A8, CF_A4, CF_A3, CF_A2, CF_A1\n"
				"        compression: CF_DXT1C, CF_DXT1C2, CF_DXT1A, CF_DXT3, CF_DXT5\n"
				"                     (CF_DXT1C2 is double sized CF_DXT1C)\n"
				"        palettes: CF_PAL, CF_PALUNI, CF_PALUNIT, CF_PALUNI2, CF_PALUNI2T,\n"
				"                  CF_PALGEM, CF_PALGEMT, CF_PAL332, CF_PALCOMP,\n"
				"                  CF_PAL1, CF_PAL2, CF_PAL3, CF_PAL4, CF_PAL5\n"
				"        commands: GENPAL=export only palettes\n"
				"                  UNI, UNIT, UNI2, UNI2T, 332, COMP=generate uniform palette (ACT file format)\n"
				);
		return 1;	
	}

	// open input file
	f = fopen(argv[1], "rb");
	if (f == NULL)
	{
		printf("Cannot open input file!\n");
		return 2;
	}

	// get file size
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, 0, SEEK_SET);

	// try to load file as BMP
	if (!BMPLoad(f, &buf, &w, &h, &wb, &fmt, &pal, &vflip))
	{
		fseek(f, 0, SEEK_SET);

		// try to load file as TGA
		if (!TGALoad(f, &buf, &w, &h, &wb, &fmt, &pal, &vflip, size))
		{
			fseek(f, 0, SEEK_SET);

			// try to load file as PNG
			vflip = FALSE;
			if (!PNGLoad(f, &buf, &w, &h, &wb, &fmt, &pal, &trans))
			{
				fseek(f, 0, SEEK_SET);

				// try to load file as DDS
				if (!DDSLoad(f, &buf, &w, &h, &wb, &fmt, &pal, &mips))
				{
					fseek(f, 0, SEEK_SET);

					// try to load file as JPG
					if (!JPEGLoad(f, &buf, &w, &h, &wb, &fmt))
					{
						printf("Invalid picture format!\n");
						fclose(f);
						return 3;
					}
				}
			}
		}
	}

	// close input file
	fclose(f);

	// generate uniform palette
	if ((forminx == CF_UNI) || (forminx == CF_UNIT))
	{
		// create output file
		f = fopen(argv[2], "wb");
		if (f == NULL)
		{
			printf("Cannot create output file!\n");
			return 4;
		}

		// generate palette
		// 0..15 = default EGA palettes
		// 16..31 = gray scale
		// 32..39 = bright colors
		// 40..255 = uniform colors: 6 levels blue * 6 levels green * 6 levels red
		BYTE buf[3*256+4];

		// default EGA palettes
		memcpy(buf, DefEGAPalRGB, 3*16);

		// gray scale
		for (i = 16; i <= 31; i++)
		{
			buf[3*i] = (i-16)*17;
			buf[3*i+1] = (i-16)*17;
			buf[3*i+2] = (i-16)*17;
		}

		// bright colors
		for (i = 32; i <= 39; i++)
		{
			buf[3*i+2] = (((i-32) & 1) != 0) ? 255 : 0;
			buf[3*i+1] = (((i-32) & 2) != 0) ? 255 : 0;
			buf[3*i] = (((i-32) & 4) != 0) ? 255 : 0;
		}

		// uniform colors
		int r = 0;
		int g = 0;
		int b = 0;
		for (i = 40; i <= 255; i++)
		{
			buf[3*i] = (BYTE)r;
			buf[3*i+1] = (BYTE)g;
			buf[3*i+2] = (BYTE)b;
			b += 51;
			if (b > 255)
			{
				b = 0;
				g += 51;
				if (g > 255)
				{
					g = 0;
					r += 51;
				}
			}
		}

		// transparent color
		buf[3*256] = 1;		// number of colors HIGH
		buf[3*256+1] = 0;	// number of colors LOW
		buf[3*256+2] = 0;	// transparent color HIGH
		buf[3*256+3] = 255; // transparent color LOW

		// write data
		fwrite(buf, 1, (forminx == CF_UNIT) ? 3*256+4 : 3*256, f);

		// close output file
		fclose(f);
		return 0;
	}

	// generate uniform palette 2
	if ((forminx == CF_UNI2) || (forminx == CF_UNI2T))
	{
		// create output file
		f = fopen(argv[2], "wb");
		if (f == NULL)
		{
			printf("Cannot create output file!\n");
			return 4;
		}

		// generate palette
		// 0..251 = uniform colors: 6 levels blue * 7 levels green * 6 levels red
		// 252 = transparent color
		BYTE buf[3*256+4];
		memset(buf, 0, sizeof(buf));

		// uniform colors
		int r = 0;
		int g = 0;
		int b = 0;
		for (i = 0; i <= 251; i++)
		{
			buf[3*i] = (BYTE)r;
			buf[3*i+1] = (BYTE)(g>>1);
			buf[3*i+2] = (BYTE)b;
			b += 51;
			if (b > 255)
			{
				b = 0;
				g += 85;
				if (g > 511)
				{
					g = 0;
					r += 51;
				}
			}
		}

		buf[3*252] = 255;
		buf[3*252+1] = 255;
		buf[3*252+2] = 255;

		// transparent color
		buf[3*256] = 0;		// number of colors HIGH
		buf[3*256+1] = 253;	// number of colors LOW
		buf[3*256+2] = 0;	// transparent color HIGH
		buf[3*256+3] = 252; // transparent color LOW

		// write data
		fwrite(buf, 1, (forminx == CF_UNI2T) ? 3*256+4 : 3*256, f);

		// close output file
		fclose(f);
		return 0;
	}

	// generate uniform palette 332
	if (forminx == CF_332)
	{
		// create output file
		f = fopen(argv[2], "wb");
		if (f == NULL)
		{
			printf("Cannot create output file!\n");
			return 4;
		}

		// generate palette
		BYTE buf[3*256];
		int r = 0;
		int g = 0;
		int b = 0;
		for (i = 0; i <= 255; i++)
		{
			buf[3*i] = Col3To8(r);
			buf[3*i+1] = Col3To8(g);
			buf[3*i+2] = Col2To8(b);
			b++;
			if (b > 3)
			{
				b = 0;
				g++;
				if (g > 7)
				{
					g = 0;
					r++;
				}
			}
		}

		// write data
		fwrite(buf, 1, 3*256, f);

		// close output file
		fclose(f);
		return 0;
	}

	// generate uniform palette COMPASS
	if (forminx == CF_COMP)
	{
		// create output file
		f = fopen(argv[2], "wb");
		if (f == NULL)
		{
			printf("Cannot create output file!\n");
			return 4;
		}

		// generate palette
		BYTE buf[3*256 + 4];
		BYTE r = 0;
		BYTE g = 0;
		BYTE b = 0;
		BYTE y = 0;
		for (i = 0; i <= 255; i++)
		{
			if (i >= 128)
			{
				buf[3*i] = 0;
				buf[3*i+1] = 0;
				buf[3*i+2] = 0;
			}
			else
			{
				buf[3*i] = (BYTE)(r + y);
				buf[3*i+1] = (BYTE)(g + y);
				buf[3*i+2] = (BYTE)(b + y);
				y ^= 0x20;
				if (y == 0)
				{
					b += 0x40;
					if (b == 0)
					{
						g += 0x40;
						if (g == 0)
						{
							r += 0x40;
						}
					}
				}
			}
		}

		// transparent color
		buf[3*256] = 0;		// number of colors HIGH
		buf[3*256+1] = 129;	// number of colors LOW
		buf[3*256+2] = 0;	// transparent color HIGH
		buf[3*256+3] = 128; // transparent color LOW

		// write data
		fwrite(buf, 1, 3*256+4, f);

		// close output file
		fclose(f);
		return 0;
	}

	// create output file
	f = fopen(argv[2], "w");
	if (f == NULL)
	{
		printf("Cannot create output file!\n");
		return 4;
	}

	// prepare palette index
	if ((pal != NULL) && (fmt == TEXIFMT_PAL8A))
	{
		for (i = 0; i < 256; i++)
		{
			if (pal[i].a < 0x80)
			{
				transinx = i;
				break;
			}
		}
	}

	if ((pal != NULL) && (trans != NULL))
	{
		for (i = 0; i < 256; i++)
		{
			if ((pal[i].r == trans->r) &&
				(pal[i].g == trans->g) &&
				(pal[i].b == trans->b))
			{
				transinx = i;
				break;
			}
		}
	}

	// export palettes
	int palsize = 2 + 1 + 1 + 256*3;

	if (forminx == CF_GENPAL)
	{
		if (pal == NULL)
		{
			printf("Picture has no palettes!\n");
			return 5;
		}
/*
typedef struct RGBENTRY_ {
	u8	b;		// BLUE component
	u8	g;		// GREEN component
	u8	r;		// RED component
} RGBENTRY;

// palette type
enum PALTYPE {
	PAL_RGB = 0,		// 3 color components: B, G, R
	PAL_RGBT = 1,		// 3 color components: B, G, R, with transparent color
};

typedef struct PALETTE_ {
	u16	size;		// total size of this palette structure
	u8	type;		// palette type PALTYPE
	u8	trans;		// transparent color
	RGBENTRY rgb[1];	// color entries
} PALETTE;
*/
		// head
		fprintf(f, "/* Do not modify, auto generated with guipic.exe! */\n"
				//"\n#include \"../global.h\"\n"
				"\n"
				"const u8 %s[%u] = {\n"
				"\t%u,%u,\n"
				"\t%s,\n"
				"\t%u,\n",
				argv[3], palsize,
				palsize & 0xff, palsize >> 8,
				(transinx == PALNOTRANS) ? "PAL_RGB" : "PAL_RGBT",
				transinx & 0xff);

		// data
		for (i = 0; i <= 255; i++)
		{
			fprintf(f, "\t%u,%u,%u,\n", pal[i].b, pal[i].g, pal[i].r, i);
		}

		fprintf(f, "};\n");

		// close output file
		fclose(f);

		// delete buffers
		free(buf);
		free(pal);
		return 0;
	}

	// prepare output number of bits per pixel
	switch (forminx)
	{
	case CF_R8G8B8:
		b = 24;
		break;

	case CF_A8R8G8B8:
	case CF_A8B8G8R8:
		b = 32;
		break;

	case CF_R5G6B5:
	case CF_A1R5G5B5:
	case CF_A2R5G5B4:
	case CF_A4R4G4B4:
		b = 16;
		break;

	case CF_A4:
	case CF_DXT1C:
	case CF_DXT1C2:
	case CF_DXT1A:
		b = 4;
		break;

	case CF_A3:
		b = 3;
		break;

	case CF_A2:
		b = 2;
		break;

	case CF_A1:
		b = 1;
		break;

	default:
		b = 8;
	}
	wb2 = (w*b+7)/8;
	if (forminx == CF_A3) wb2 = (w+2)/3;

// picture (size 8 bytes + picture data)
/*typedef struct PIC_ {
	s16	w;		// width
	s16	h;		// height
	s16	wb;		// bytes per scanline
	u8	colfmt;		// color format COLFMT
	u8	bits;		// number of bits per pixel
	u8	data[1];	// start of data
} PIC;*/

	int picsize = 2 + 2 + 2 + 1 + 1 + h*wb2;
	if (forminx == CF_PAL) picsize += palsize;

	// write head
	int w2 = (forminx == CF_DXT1C2) ? (w*2) : w;
	int h2 = (forminx == CF_DXT1C2) ? (h*2) : h;
	fprintf(f,  "/* Do not modify, auto generated with guipic.exe! */\n"
				"\n#include \"../include.h\"\n"
				"\n"
				"const u8 %s[%u] __attribute__ ((aligned(4))) = {\n"
				"\t%u,%u,\n"		// width
				"\t%u,%u,\n"		// height
				"\t%u,%u,\n"		// bytes per scanline
				"\t%s,\n"			// format name
				"\t%u,\n"			// bits per pixel
				"\t",
				argv[3], picsize,
				w2 & 0xff, w2 >> 8,
				h2 & 0xff, h2 >> 8,
				wb2 & 0xff, wb2 >> 8,
				formname[forminx],
				b);

	// export private palettes
	if (forminx == CF_PAL)
	{
		if (pal == NULL)
		{
			printf("Picture has no palettes!\n");
			return 5;
		}

		// head
		fprintf(f, "%u,%u,\n"
				"\t%s,\n"
				"\t%u,\n\t",
				palsize & 0xff, palsize >> 8,
				(transinx == PALNOTRANS) ? "PAL_RGB" : "PAL_RGBT",
				transinx & 0xff);

		// data
		for (i = 0; i <= 255; i++)
		{
			fprintf(f, "%u,%u,%u,\n\t", pal[i].b, pal[i].g, pal[i].r);
		}
	}
	pos = 0;

	// export DXT format
	if ((forminx >= CF_DXT1C) && (forminx <= CF_DXT5))
	{
		if ((fmt == TEXIFMT_DXT1C) && (forminx == CF_DXT1A)) fmt = TEXIFMT_DXT1A;
		if ((fmt == TEXIFMT_DXT1A) && ((forminx == CF_DXT1C) || (forminx == CF_DXT1C2))) fmt = TEXIFMT_DXT1C;

		if ((fmt != TEXIFMT_DXT1C) || (forminx != CF_DXT1C2))
		{
			if ((forminx - CF_DXT1C) != (fmt - TEXIFMT_DXT1C))
			{
				printf("Unsupported conversion!\n");
				return 7;
			}
		}

		// check picture dimension
		if (((w & 3) != 0) || (h & 3) != 0)
		{
			printf("DXT format must be multiply of 4 sized!\n");
			return 8;
		}

		s = buf;
		for (i = h*wb2; i > 0; i--) writeB(*s++);

		goto CLOSE;
	}

	// source bytes per pixel
	switch(fmt)
	{
	case TEXIFMT_X8R8G8B8:
	case TEXIFMT_A8R8G8B8:
		bs = 4; break;

	case TEXIFMT_R8G8B8:
	case TEXIFMT_B8G8R8:
		bs = 3; break;

	case TEXIFMT_X1R5G5B5:
	case TEXIFMT_A1R5G5B5:
	case TEXIFMT_A4R4G4B4:
	case TEXIFMT_A8L8:
		bs = 2; break;

	default:
		bs = 1;
	}

	// vertical flip
	int wb3 = wb - w*bs;
	s = buf;
	if (vflip)
	{
		s = buf + (h-1)*wb;
		wb3 = -wb - w*bs;
	}

	u8 accu;
	int accuN;

	// write picture data
	for (y = 0; y < h; y++)
	{
		accuN = 0;
		accu = 0;

		for (x = 0; x < w; x++)
		{
			switch (forminx)
			{
			case CF_R8G8B8:
				switch(fmt)
				{
				case TEXIFMT_X8R8G8B8:
				case TEXIFMT_A8R8G8B8:
				case TEXIFMT_R8G8B8:
					writeB(s[0]);
					writeB(s[1]);
					writeB(s[2]);
					break;

				case TEXIFMT_B8G8R8:
					writeB(s[2]);
					writeB(s[1]);
					writeB(s[0]);
					break;

				case TEXIFMT_X1R5G5B5:
				case TEXIFMT_A1R5G5B5:
					writeB(Col5To8(s[0] & 0x1f));
					writeB(Col5To8((*(WORD*)s >> 5) & 0x1f));
					writeB(Col5To8((s[1] >> 2) & 0x1f));
					break;

				case TEXIFMT_A4R4G4B4:
					writeB(Col4To8(s[0] & 0xf));
					writeB(Col4To8(s[0] >> 4));
					writeB(Col4To8(s[1] & 0xf));
					break;

				case TEXIFMT_PAL8:
				case TEXIFMT_PAL8A:
					writeB(pal[*s].b);
					writeB(pal[*s].g);
					writeB(pal[*s].r);
					break;

				case TEXIFMT_A8L8:
				case TEXIFMT_L8:
					writeB(*s);
					writeB(*s);
					writeB(*s);
					break;
				}
				break;

			case CF_R5G6B5:
				switch(fmt)
				{
				case TEXIFMT_X8R8G8B8:
				case TEXIFMT_A8R8G8B8:
				case TEXIFMT_R8G8B8:
					writeW((s[0] >> 3) | ((s[1] & 0xfc) << 3) | ((s[2] & 0xf8) << 8));
					break;

				case TEXIFMT_B8G8R8:
					writeW((s[2] >> 3) | ((s[1] & 0xfc) << 3) | ((s[0] & 0xf8) << 8));
					break;

				case TEXIFMT_X1R5G5B5:
				case TEXIFMT_A1R5G5B5:
					writeW((s[0] & 0x1f) | ((*(WORD*)s << 1) & 0xffc0));
					break;

				case TEXIFMT_A4R4G4B4:
					writeW(Col4To5(s[0] & 0xf) | (Col4To6(s[0] >> 4) << 5) |
								(Col4To5(s[1] & 0xf) << 10));
					break;

				case TEXIFMT_PAL8:
				case TEXIFMT_PAL8A:
					writeW((pal[*s].b >> 3) | ((pal[*s].g & 0xfc) << 3) |
						((pal[*s].r & 0xf8) << 8));
					break;

				case TEXIFMT_A8L8:
				case TEXIFMT_L8:
					writeW((*s >> 3) | ((*s & 0xf8) << 3) | ((*s & 0xf8) << 8));
					break;
				}
				break;

			case CF_R3G3B2:
				switch(fmt)
				{
				case TEXIFMT_X8R8G8B8:
				case TEXIFMT_A8R8G8B8:
				case TEXIFMT_R8G8B8:
					writeB((s[0] >> 6) | ((s[1] & 0xe0) >> 3) | (s[2] & 0xe0));
					break;

				case TEXIFMT_B8G8R8:
					writeB((s[2] >> 6) | ((s[1] & 0xe0) >> 3) | (s[0] & 0xe0));
					break;

				case TEXIFMT_X1R5G5B5:
				case TEXIFMT_A1R5G5B5:
					writeB(((s[0] & 0x18) >> 3) | ((*(WORD*)s >> 5) & 0x1c) |
						((s[1] << 1) & 0xe0));
					break;

				case TEXIFMT_A4R4G4B4:
					writeB(((s[0] & 0xc) >> 2) | ((s[0] >> 3) & 0x1c) |
						((s[1] << 4) & 0xe0));
					break;

				case TEXIFMT_PAL8:
				case TEXIFMT_PAL8A:
					writeB((pal[*s].b >> 6) | ((pal[*s].g & 0xe0) >> 3) |
						(pal[*s].r & 0xe0));
					break;

				case TEXIFMT_A8L8:
				case TEXIFMT_L8:
					writeB((*s >> 6) | ((*s & 0xe0) >> 3) | (*s & 0xe0));
					break;
				}
				break;

			case CF_A8B8G8R8:
				switch(fmt)
				{
				case TEXIFMT_X8R8G8B8:
				case TEXIFMT_R8G8B8:
					writeB(s[2]);
					writeB(s[1]);
					writeB(s[0]);
					writeB(255);
					break;

				case TEXIFMT_B8G8R8:
					writeB(s[0]);
					writeB(s[1]);
					writeB(s[2]);
					writeB(255);
					break;

				case TEXIFMT_A8R8G8B8:
					writeB(s[2]);
					writeB(s[1]);
					writeB(s[0]);
					writeB(s[3]);
					break;

				case TEXIFMT_X1R5G5B5:
					writeB(Col5To8((s[1] >> 2) & 0x1f));
					writeB(Col5To8((*(WORD*)s >> 5) & 0x1f));
					writeB(Col5To8(s[0] & 0x1f));
					writeB(255);
					break;

				case TEXIFMT_A1R5G5B5:
					writeB(Col5To8((s[1] >> 2) & 0x1f));
					writeB(Col5To8((*(WORD*)s >> 5) & 0x1f));
					writeB(Col5To8(s[0] & 0x1f));
					writeB(((s[1] & 0x80) != 0) ? 0xff : 0);
					break;

				case TEXIFMT_A4R4G4B4:
					writeB(Col4To8(s[1] & 0xf));
					writeB(Col4To8(s[0] >> 4));
					writeB(Col4To8(s[0] & 0xf));
					writeB(Col4To8(s[1] >> 4));
					break;

				case TEXIFMT_PAL8:
					writeB(pal[*s].b);
					writeB(pal[*s].g);
					writeB(pal[*s].r);
					writeB(255);
					break;

				case TEXIFMT_PAL8A:
					writeB(pal[*s].b);
					writeB(pal[*s].g);
					writeB(pal[*s].r);
					writeB(pal[*s].a);
					break;

				case TEXIFMT_L8:
					writeB(*s);
					writeB(*s);
					writeB(*s);
					writeB(255);
					break;

				case TEXIFMT_A8L8:
					writeB(*s);
					writeB(*s);
					writeB(*s);
					writeB(s[1]);
					break;
				}
				break;

			case CF_A8R8G8B8:
				switch(fmt)
				{
				case TEXIFMT_X8R8G8B8:
				case TEXIFMT_R8G8B8:
					writeB(s[0]);
					writeB(s[1]);
					writeB(s[2]);
					writeB(255);
					break;

				case TEXIFMT_B8G8R8:
					writeB(s[2]);
					writeB(s[1]);
					writeB(s[0]);
					writeB(255);
					break;

				case TEXIFMT_A8R8G8B8:
					writeB(s[0]);
					writeB(s[1]);
					writeB(s[2]);
					writeB(s[3]);
					break;

				case TEXIFMT_X1R5G5B5:
					writeB(Col5To8(s[0] & 0x1f));
					writeB(Col5To8((*(WORD*)s >> 5) & 0x1f));
					writeB(Col5To8((s[1] >> 2) & 0x1f));
					writeB(255);
					break;

				case TEXIFMT_A1R5G5B5:
					writeB(Col5To8(s[0] & 0x1f));
					writeB(Col5To8((*(WORD*)s >> 5) & 0x1f));
					writeB(Col5To8((s[1] >> 2) & 0x1f));
					writeB(((s[1] & 0x80) != 0) ? 0xff : 0);
					break;

				case TEXIFMT_A4R4G4B4:
					writeB(Col4To8(s[0] & 0xf));
					writeB(Col4To8(s[0] >> 4));
					writeB(Col4To8(s[1] & 0xf));
					writeB(Col4To8(s[1] >> 4));
					break;

				case TEXIFMT_PAL8:
					writeB(pal[*s].b);
					writeB(pal[*s].g);
					writeB(pal[*s].r);
					writeB(255);
					break;

				case TEXIFMT_PAL8A:
					writeB(pal[*s].b);
					writeB(pal[*s].g);
					writeB(pal[*s].r);
					writeB(pal[*s].a);
					break;

				case TEXIFMT_L8:
					writeB(*s);
					writeB(*s);
					writeB(*s);
					writeB(255);
					break;

				case TEXIFMT_A8L8:
					writeB(*s);
					writeB(*s);
					writeB(*s);
					writeB(s[1]);
					break;
				}
				break;

			case CF_A1R5G5B5:
				switch(fmt)
				{
				case TEXIFMT_X8R8G8B8:
				case TEXIFMT_R8G8B8:
					writeW((s[0] >> 3) | ((s[1] & 0xf8) << 2) |
						((s[2] & 0xf8) << 7) | 0x8000);
					break;

				case TEXIFMT_B8G8R8:
					writeW((s[2] >> 3) | ((s[1] & 0xf8) << 2) |
						((s[0] & 0xf8) << 7) | 0x8000);
					break;

				case TEXIFMT_A8R8G8B8:
					writeW((s[0] >> 3) | ((s[1] & 0xf8) << 2) |
						((s[2] & 0xf8) << 7) | ((s[3] & 0x80) << 8));
					break;

				case TEXIFMT_X1R5G5B5:
					writeW(*(WORD*)s | 0x8000);
					break;

				case TEXIFMT_A1R5G5B5:
					writeW(*(WORD*)s);
					break;

				case TEXIFMT_A4R4G4B4:
					writeW(Col4To5(s[0] & 0xf) | (Col4To5(s[0] >> 4) << 5) |
						(Col4To5(s[1] & 0xf) << 10) | ((s[1] >= 0x80) ? 0x8000 : 0));
					break;

				case TEXIFMT_PAL8:
					writeW((pal[*s].b >> 3) | ((pal[*s].g & 0xf8) << 2) |
						((pal[*s].r & 0xf8) << 7) | 0x8000);
					break;

				case TEXIFMT_PAL8A:
					writeW((pal[*s].b >> 3) | ((pal[*s].g & 0xf8) << 2) |
						((pal[*s].r & 0xf8) << 7) | ((pal[*s].a & 0x80) << 8));
					break;

				case TEXIFMT_L8:
					writeW((*s >> 3) | ((*s & 0xf8) << 2) |
						((*s & 0xf8) << 7) | 0x8000);
					break;

				case TEXIFMT_A8L8:
					writeW((*s >> 3) | ((*s & 0xf8) << 2) |
						((*s & 0xf8) << 7) | ((s[1] & 0x80) << 8));
					break;
				}
				break;

			case CF_A2R5G5B4:
				switch(fmt)
				{
				case TEXIFMT_X8R8G8B8:
				case TEXIFMT_R8G8B8:
					writeW((s[0] >> 4) | ((s[1] & 0xf8) << 1) |
						((s[2] & 0xf8) << 6) | 0xc000);
					break;

				case TEXIFMT_B8G8R8:
					writeW((s[2] >> 4) | ((s[1] & 0xf8) << 1) |
						((s[0] & 0xf8) << 6) | 0xc000);
					break;

				case TEXIFMT_A8R8G8B8:
					writeW((s[0] >> 4) | ((s[1] & 0xf8) << 1) |
						((s[2] & 0xf8) << 6) | ((s[3] & 0xc0) << 8));
					break;

				case TEXIFMT_X1R5G5B5:
					writeW(((s[0] >> 1) & 0xf) | ((*(WORD*)s >> 1)& 0x1f0) |
						((*(WORD*)s >> 1) & 0x3e00) | 0xc000);
					break;

				case TEXIFMT_A1R5G5B5:
					writeW(((s[0] >> 1) & 0xf) | ((*(WORD*)s >> 1)& 0x1f0) |
						((*(WORD*)s >> 1) & 0x3e00) | (((s[1] & 0x80) != 0) ? 0xc000 : 0));
					break;

				case TEXIFMT_A4R4G4B4:
					writeW((s[0] & 0xf) | (Col4To5(s[0] >> 4) << 4) |
						(Col4To5(s[1] & 0xf) << 9) | (s[1] & 0xc000));
					break;

				case TEXIFMT_PAL8:
					writeW((pal[*s].b >> 4) | ((pal[*s].g & 0xf8) << 1) |
						((pal[*s].r & 0xf8) << 6) | 0xc000);
					break;

				case TEXIFMT_PAL8A:
					writeW((pal[*s].b >> 4) | ((pal[*s].g & 0xf8) << 1) |
						((pal[*s].r & 0xf8) << 6) | ((pal[*s].a & 0xc0) << 8));
					break;

				case TEXIFMT_L8:
					writeW((*s >> 4) | ((*s & 0xf0) << 1) |
						((*s & 0xf0) << 6) | 0xc000);
					break;

				case TEXIFMT_A8L8:
					writeW((*s >> 4) | ((*s & 0xf0) << 1) |
						((*s & 0xf0) << 6) | ((s[1] & 0xc0) << 8));
					break;
				}
				break;

			case CF_A4R4G4B4:
				switch(fmt)
				{
				case TEXIFMT_X8R8G8B8:
				case TEXIFMT_R8G8B8:
					writeW((s[0] >> 4) | (s[1] & 0xf0) |
						((s[2] & 0xf0) << 4) | 0xf000);
					break;

				case TEXIFMT_B8G8R8:
					writeW((s[2] >> 4) | (s[1] & 0xf0) |
						((s[0] & 0xf0) << 4) | 0xf000);
					break;

				case TEXIFMT_A8R8G8B8:
					writeW((s[0] >> 4) | (s[1] & 0xf0) |
						((s[2] & 0xf0) << 4) | ((s[3] & 0xf0) << 8));
					break;

				case TEXIFMT_X1R5G5B5:
					writeW(((s[0] >> 1) & 0xf) | ((*(WORD*)s >> 2)& 0xf0) |
						((*(WORD*)s >> 3) & 0xf00) | 0xf000);
					break;

				case TEXIFMT_A1R5G5B5:
					writeW(((s[0] >> 1) & 0xf) | ((*(WORD*)s >> 2)& 0xf0) |
						((*(WORD*)s >> 3) & 0xf00) | (((s[1] & 0x80) != 0) ? 0xf000 : 0));
					break;

				case TEXIFMT_A4R4G4B4:
					writeW(*(WORD*)s);
					break;

				case TEXIFMT_PAL8:
					writeW((pal[*s].b >> 4) | (pal[*s].g & 0xf0) |
						((pal[*s].r & 0xf0) << 4) | 0xf000);
					break;

				case TEXIFMT_PAL8A:
					writeW((pal[*s].b >> 4) | (pal[*s].g & 0xf0) |
						((pal[*s].r & 0xf0) << 4) | ((pal[*s].a & 0xf0) << 8));
					break;

				case TEXIFMT_L8:
					writeW((*s >> 4) | (*s & 0xf0) |
						((*s & 0xf0) << 4) | 0xf000);
					break;

				case TEXIFMT_A8L8:
					writeW((*s >> 4) | (*s & 0xf0) |
						((*s & 0xf0) << 4) | ((s[1] & 0xf0) << 8));
					break;
				}
				break;

			case CF_A8:
				switch(fmt)
				{
				case TEXIFMT_A8R8G8B8:
					writeB(s[3]);
					break;

				case TEXIFMT_X8R8G8B8:
				case TEXIFMT_R8G8B8:
				case TEXIFMT_B8G8R8:
					writeB((s[0] + s[1] + s[2])/3);
					break;

				case TEXIFMT_X1R5G5B5:
					writeB((((s[0] << 3) & 0xf8) + ((*(WORD*)s >> 2) & 0xf8) +
						((s[1] << 1) & 0xf8))/3);
					break;

				case TEXIFMT_A1R5G5B5:
					writeB((s[1] & 0x80) != 0 ? 0xff : 0);
					break;

				case TEXIFMT_PAL8:
					writeB((pal[*s].b + pal[*s].g + pal[*s].r)/3);
					break;

				case TEXIFMT_PAL8A:
					writeB(pal[*s].a);
					break;

				case TEXIFMT_A8L8:
					writeB(s[1]);
					break;

				case TEXIFMT_L8:
					writeB(*s);
					break;
				}
				break;

			case CF_A4:
				switch(fmt)
				{
				case TEXIFMT_A8R8G8B8:
					i = s[3];
					goto WRITEA4;

				case TEXIFMT_X8R8G8B8:
				case TEXIFMT_R8G8B8:
				case TEXIFMT_B8G8R8:
					i = (s[0] + s[1] + s[2])/3;
					goto WRITEA4;

				case TEXIFMT_X1R5G5B5:
					i = (((s[0] << 3) & 0xf8) + ((*(WORD*)s >> 2) & 0xf8) +
						((s[1] << 1) & 0xf8))/3;
					goto WRITEA4;

				case TEXIFMT_A1R5G5B5:
					i = ((s[1] & 0x80) != 0 ? 0xff : 0);
					goto WRITEA4;

				case TEXIFMT_PAL8:
					i = (pal[*s].b + pal[*s].g + pal[*s].r)/3;
					goto WRITEA4;

				case TEXIFMT_PAL8A:
					i = pal[*s].a;
					goto WRITEA4;

				case TEXIFMT_A8L8:
					i = s[1];
					goto WRITEA4;

				case TEXIFMT_L8:
					i = *s;
WRITEA4:
					if (accuN == 0)
					{
						accu = i & 0xf0;
						accuN = 1;
					}
					else
					{
						accu |= i >> 4;
						writeB(accu);
						accuN = 0;
					}
					break;
				}
				break;

			case CF_A3:
				switch(fmt)
				{
				case TEXIFMT_A8R8G8B8:
					i = s[3];
					goto WRITEA3;

				case TEXIFMT_X8R8G8B8:
				case TEXIFMT_R8G8B8:
				case TEXIFMT_B8G8R8:
					i = (s[0] + s[1] + s[2])/3;
					goto WRITEA3;

				case TEXIFMT_X1R5G5B5:
					i = (((s[0] << 3) & 0xf8) + ((*(WORD*)s >> 2) & 0xf8) +
						((s[1] << 1) & 0xf8))/3;
					goto WRITEA3;

				case TEXIFMT_A1R5G5B5:
					i = ((s[1] & 0x80) != 0 ? 0xff : 0);
					goto WRITEA3;

				case TEXIFMT_PAL8:
					i = (pal[*s].b + pal[*s].g + pal[*s].r)/3;
					goto WRITEA3;

				case TEXIFMT_PAL8A:
					i = pal[*s].a;
					goto WRITEA3;

				case TEXIFMT_A8L8:
					i = s[1];
					goto WRITEA3;

				case TEXIFMT_L8:
					i = *s;
WRITEA3:
					switch (accuN)
					{
					case 0:
						accu = i & 0xc0;
						accuN = 1;
						break;

					case 1:
						accu |= (i & 0xe0) >> 2;
						accuN = 2;
						break;

					default:
						accu |= i >> 5;
						writeB(accu);
						accuN = 0;
						break;
					}
					break;
				}
				break;

			case CF_A2:
				switch(fmt)
				{
				case TEXIFMT_A8R8G8B8:
					i = s[3];
					goto WRITEA2;

				case TEXIFMT_X8R8G8B8:
				case TEXIFMT_R8G8B8:
				case TEXIFMT_B8G8R8:
					i = (s[0] + s[1] + s[2])/3;
					goto WRITEA2;

				case TEXIFMT_X1R5G5B5:
					i = (((s[0] << 3) & 0xf8) + ((*(WORD*)s >> 2) & 0xf8) +
						((s[1] << 1) & 0xf8))/3;
					goto WRITEA2;

				case TEXIFMT_A1R5G5B5:
					i = ((s[1] & 0x80) != 0 ? 0xff : 0);
					goto WRITEA2;

				case TEXIFMT_PAL8:
					i = (pal[*s].b + pal[*s].g + pal[*s].r)/3;
					goto WRITEA2;

				case TEXIFMT_PAL8A:
					i = pal[*s].a;
					goto WRITEA2;

				case TEXIFMT_A8L8:
					i = s[1];
					goto WRITEA2;

				case TEXIFMT_L8:
					i = *s;
WRITEA2:
					switch (accuN)
					{
					case 0:
						accu = i & 0xc0;
						accuN = 1;
						break;

					case 1:
						accu |= (i & 0xc0) >> 2;
						accuN = 2;
						break;

					case 2:
						accu |= (i & 0xc0) >> 4;
						accuN = 3;
						break;

					default:
						accu |= i >> 6;
						writeB(accu);
						accuN = 0;
						break;
					}
					break;
				}
				break;

			case CF_A1:
				switch(fmt)
				{
				case TEXIFMT_A8R8G8B8:
					i = s[3];
					goto WRITEA1;

				case TEXIFMT_X8R8G8B8:
				case TEXIFMT_R8G8B8:
				case TEXIFMT_B8G8R8:
					i = (s[0] + s[1] + s[2])/3;
					goto WRITEA1;

				case TEXIFMT_X1R5G5B5:
					i = (((s[0] << 3) & 0xf8) + ((*(WORD*)s >> 2) & 0xf8) +
						((s[1] << 1) & 0xf8))/3;
					goto WRITEA1;

				case TEXIFMT_A1R5G5B5:
					i = ((s[1] & 0x80) != 0 ? 0xff : 0);
					goto WRITEA1;

				case TEXIFMT_PAL8:
					i = (pal[*s].b + pal[*s].g + pal[*s].r)/3;
					goto WRITEA1;

				case TEXIFMT_PAL8A:
					i = pal[*s].a;
					goto WRITEA1;

				case TEXIFMT_A8L8:
					i = s[1];
					goto WRITEA1;

				case TEXIFMT_L8:
					i = *s;
WRITEA1:
					accu |= (i & 0x80) >> accuN;
					accuN++;
					if (accuN == 8)
					{
						writeB(accu);
						accuN = 0;
						accu = 0;
					}
					break;
				}
				break;

			default:	// CF_PALx
				switch(fmt)
				{
				case TEXIFMT_PAL8:
				case TEXIFMT_PAL8A:
					writeB(*s);
					break;

				default:
					printf("Unsupported conversion!\n");
					return 9;
				}
				break;
			}
			s += bs;
		}

		if (accuN > 0) writeB(accu);

		s += wb3;
	}

CLOSE:

	if (pos != 0) fprintf(f, "\n");
	fprintf(f, "};\n");

	// close output file
	fclose(f);

	// delete buffers
	free(buf);
	free(pal);
	return 0;
}
