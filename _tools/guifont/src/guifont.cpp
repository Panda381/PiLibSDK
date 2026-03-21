// import font from INI file

#include <stdio.h>
#include <malloc.h>						// memory allocation
#include <memory.h>						// memory
#include <windows.h>
#include "bmp.h"
#include "d3timport.h"

#define MAXBMPNUM	20		// maximum number of font BMP files
#define MAXCHARS	1024	// maximum number of font characters
#define MAXFONTHEIGHT 50	// maximum font height (and aprox. width)
#define MAXBMPWIDTH	TEXMAXWH // maximum width of BMP file
#define FONTMINLR	-7		// minimal left/right spacing
#define FONTMAXLR	8		// maximal left/right spacing
#define FONTOFFLR	7		// offset of left/right spacing
#define FONTSHIFTR	4		// shifts of right spacing

// input file
BYTE* buf = NULL;
TEXIFMT fmt;
BOOL vflip = FALSE;
D3COLRGBA* pal = NULL;
FILE* f = NULL;
int forminx;

const char* formname[] = {
	"CF_A8",
	"CF_A4",
	"CF_A3",
	"CF_A2",
	"CF_A1",
};

// picture color format
enum COLFMT {
	CF_A8,		// (1B) A8, 8 bits alpha
	CF_A4,		// (1B) A4, 4 bits alpha
	CF_A3,		// (1B) A3, 2+3+3 bits alpha
	CF_A2,		// (1B) A2, 2 bits alpha
	CF_A1,		// (1B) A1, 1 bit alpha

	CMDNUM
};

// font
int FontHeight;		// font height
int FontSpacing;	// width of space character

// one font character
typedef struct FONTCHAR_ {
	int	code;		// UNICODE code
	int	left;		// left space
	int w;			// width
	int	right;		// right space
	int x;			// X coordinate
} FONTCHAR;

// one font file
typedef struct FONTFILE_ {
	int		w;		// total width of bitmap
	int		wb;		// bytes per scaline
	BYTE*	data;	// buffer with picture
	int		fontnum; // number of characters
	FONTCHAR fontchar[MAXCHARS]; // fonts	
} FONTFILE;

// Translation table from Windows-1250 (Central Europe) to Unicode
#define CHINV -1
short CP1250ToUniTab[256] = {
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
	0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
	0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,
	0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
	0x20ac,  CHINV, 0x201a,  CHINV, 0x201e, 0x2026, 0x2020, 0x2021,
	 CHINV, 0x2030, 0x0160, 0x2039, 0x015a, 0x0164, 0x017d, 0x0179,
	 CHINV, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014,
	 CHINV, 0x2122, 0x0161, 0x203a, 0x015b, 0x0165, 0x017e, 0x017a,
	0x00a0, 0x02c7, 0x02d8, 0x0141, 0x00a4, 0x0104, 0x00a6, 0x00a7,
	0x00a8, 0x00a9, 0x015e, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x017b,
	0x00b0, 0x00b1, 0x02db, 0x0142, 0x00b4, 0x00b5, 0x00b6, 0x00b7,
	0x00b8, 0x0105, 0x015f, 0x00bb, 0x013d, 0x02dd, 0x013e, 0x017c,
	0x0154, 0x00c1, 0x00c2, 0x0102, 0x00c4, 0x0139, 0x0106, 0x00c7,
	0x010c, 0x00c9, 0x0118, 0x00cb, 0x011a, 0x00cd, 0x00ce, 0x010e,
	0x0110, 0x0143, 0x0147, 0x00d3, 0x00d4, 0x0150, 0x00d6, 0x00d7,
	0x0158, 0x016e, 0x00da, 0x0170, 0x00dc, 0x00dd, 0x0162, 0x00df,
	0x0155, 0x00e1, 0x00e2, 0x0103, 0x00e4, 0x013a, 0x0107, 0x00e7,
	0x010d, 0x00e9, 0x0119, 0x00eb, 0x011b, 0x00ed, 0x00ee, 0x010f,
	0x0111, 0x0144, 0x0148, 0x00f3, 0x00f4, 0x0151, 0x00f6, 0x00f7,
	0x0159, 0x016f, 0x00fa, 0x0171, 0x00fc, 0x00fd, 0x0163, 0x02d9,
};

FONTFILE Fonts[MAXBMPNUM];
int FontsN = 0;

char** ArgV;

// get INI string
#define MAXKEY	1000	// max. length of INI key
BOOL ReadIniText(LPCSTR group, LPCSTR key, LPCSTR def, char* buf)
{
	int n = ::GetPrivateProfileString(group, key, def, buf, MAXKEY, ArgV[1]);
	buf[n] = 0;
	return (n > 0);
}

// get INI number
int ReadIniInt(LPCSTR group, LPCTSTR key, int def)
{
	return ::GetPrivateProfileInt(group, key, def, ArgV[1]);
}

// split text to tokens (list ends with 0)
int Tokenize(char* buf, char delim)
{
	int n = 0;
	while (*buf != 0)
	{
		n++;
		while ((*buf != 0) && (*buf != delim)) { buf++; }
		if (*buf == delim)
		{
			*buf = 0;
			buf++;
		}
	}
	buf[1] = 0;
	return n;
}

// read 4-dig HEX number (return -1 on error)
int Read4Hex(char* text)
{
	char ch;
	int i;
	int n = 0;
	for (i = 4; i > 0; i--)
	{
		ch = *text++;
		if ((ch >= 'a') && (ch <= 'z')) ch -= 32;
		if ((ch >= 'A') && (ch <= 'F'))
			ch -= 7;
		else
			if ((ch < '0') || (ch > '9')) return -1;
		ch -= '0';
		n = n*16 + ch;
	}
	return n;
}

// read INT number (returns INTERR on error)
#define INTERR -10000
int ReadInt(char* text)
{
	char ch;
	int n = 0;
	BOOL sig = FALSE;
	if (*text == '-')
	{
		text++;
		sig = TRUE;
	}

	while (*text != 0)
	{
		ch = *text++;
		if ((ch < '0') || (ch > '9')) return INTERR;
		n = n*10 + (ch - '0');
	}
	if (sig) n = -n;
	return n;
}

// parse one charset
char CharSets[MAXKEY+1];
char CharKey[5];
char CharSize[MAXKEY+1];
int XSet;
BOOL GetCharSet(char* range, FONTFILE* font)
{
	int i;
	char* range0 = range;

	// get start of interval
	int first = Read4Hex(range);
	if (first < 0)
	{
		printf("Invalid charset %s!\n", range0);
		return FALSE;
	}
	range += 4;

	// get stop of interval
	int last = first;
	if (*range == '-')
	{
		range++;
		last = Read4Hex(range);
		if (last < 0)
		{
			printf("Invalid charset %s!\n", range0);
			return FALSE;
		}
		range += 4;
	}

	if (*range != 0)
	{
		printf("Invalid charset %s!\n", range0);
		return FALSE;
	}

	// loop through characters
	for (i = first; i <= last; i++)
	{
		if (font->fontnum >= MAXCHARS) break;

		// prepare key
		sprintf(CharKey, "%04x", i);
		if (!ReadIniText("CharSize", CharKey, "", CharSize))
		{
			sprintf(CharKey, "%04X", i);
			if (!ReadIniText("CharSize", CharKey, "", CharSize))
			{
				printf("Cannot find CharSize=%s!", CharKey);
				return FALSE;
			}
		}
		if (Tokenize(CharSize, ',') != 3)
		{
			printf("Invalid CharSize %s!", CharKey);
			return FALSE;
		}

		// load font size
		char* ch = CharSize;
		FONTCHAR* fontchar = &font->fontchar[font->fontnum];
		fontchar->left = ReadInt(ch);
		ch += strlen(ch) + 1;
		fontchar->w = ReadInt(ch);
		ch += strlen(ch) + 1;
		fontchar->right = ReadInt(ch);
		fontchar->x = XSet;
		XSet += fontchar->w;

		if ((fontchar->left == INTERR) ||
			(fontchar->w == INTERR) ||
			(fontchar->w < 0) ||
			(fontchar->w > FontHeight*2) ||
			(fontchar->right == INTERR))
		{
			printf("Invalid CharSize %s!", CharKey);
			return FALSE;
		}

		if (fontchar->left < FONTMINLR) fontchar->left = FONTMINLR;
		if (fontchar->left > FONTMAXLR) fontchar->left = FONTMAXLR;
		if (fontchar->right < FONTMINLR) fontchar->right = FONTMINLR;
		if (fontchar->right > FONTMAXLR) fontchar->right = FONTMAXLR;

		fontchar->code = i;
		
		font->fontnum++;
	}

	return TRUE;
}

// load one BMP file
char PicFile[MAXKEY+1];
BOOL LoadFont(char* name)
{
	int i, j;
	BYTE bb;

	// get picture name
	if (!ReadIniText(name, "Source", "", PicFile))
	{
		printf("Invalid INI parameter [%s]Source!", name);
		return FALSE;
	}

	// open picture file
	f = fopen(PicFile, "rb");
	if (f == NULL)
	{
		printf("Cannot fint picture file %s!", PicFile);
		return FALSE;
	}

	int hh;

	// load picture file
	FONTFILE* font = &Fonts[FontsN];
	if (!BMPLoad(f, &font->data, &font->w, &hh, &font->wb,
		&fmt, &pal, &vflip) ||
		(fmt != TEXIFMT_PAL8) ||
		(pal == NULL) ||
		(hh != FontHeight))
	{
		printf("Invalid picture file %s!", PicFile);
		return FALSE;
	}
	FontsN++;

	// close picture file
	fclose(f);

	// vertical flip picture
	if (vflip)
	{
		for (i = 0; i < hh/2; i++)
		{
			for (j = 0; j < font->w; j++)
			{
				bb = font->data[i*font->wb + j];
				font->data[i*font->wb + j] = font->data[font->wb*(hh-1-i) + j];
				font->data[font->wb*(hh-1-i) + j] = bb;
			}
		}
	}

	// get charsets
	if (!ReadIniText(name, "CharSet", "", CharSets))
	{
		printf("Invalid font definition [%s]CharSet!\n", name);
		return FALSE;
	}
	Tokenize(CharSets, ',');

	// loop through charsets
	font->fontnum = 0;
	XSet = 0;
	char* chs = CharSets;
	while ((*chs != 0) && (font->fontnum < MAXCHARS))
	{
		if (!GetCharSet(chs, font)) return FALSE;
		chs += strlen(chs) + 1;
	}

	if (XSet > font->w)
	{
		printf("Invalid width of picture %s!\n", PicFile);
		return FALSE;
	}

	return TRUE;
}

// find Unicode character
BOOL FindChar(int code, FONTFILE** font, FONTCHAR** fontchar)
{
	int i, j;
	FONTFILE* font2;
	FONTCHAR* fontchar2;
	for (i = 0; i < FontsN; i++)
	{
		font2 = &Fonts[i];
		for (j = 0; j < font2->fontnum; j++)
		{
			fontchar2 = &font2->fontchar[j];
			if (fontchar2->code == code)
			{
				*font = font2;
				*fontchar = fontchar2;
				return TRUE;
			}
		}
	}
	return FALSE;
}

char CodePages[MAXKEY+1];

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

int main(int argc, char* argv[])
{
	int w, wb2, i, j, row, min, max;
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
		printf( "Syntax: guifont input_file output_file name form\n"
				"     input_file = INI file, entered with path\n"
				"     output_file = C source file\n"
				"     name = name of font structure\n"
				"     form = data format CF_A8, CF_A4, CF_A3, CF_A2, CF_A1\n"
				);
		return 1;	
	}

	ArgV = argv;
	FontHeight = ReadIniInt("Font", "Height", 0);
	FontSpacing = ReadIniInt("Font", "Spacing", -1);

	// check input file
	f = fopen(argv[1], "rb");
	if ((f == NULL) || ((FontHeight == 0) && (FontSpacing == -1)))
	{
		printf("Cannot open input file (enter filename with path)!\n");
		return 2;
	}
	fclose(f);

	// load font parameters
	if ((FontHeight > MAXFONTHEIGHT) ||
		(FontHeight < 4) ||
		((DWORD)FontSpacing > MAXFONTHEIGHT))
	{
		printf("Invalid font definition (Height, Spacing)!\n");
		return 2;
	}

	// load main group
	if (!LoadFont("Font")) return 3;

	// get code pages
	ReadIniText("Font", "CodePage", "", CodePages);
	Tokenize(CodePages, ',');

	// loop through codepages
	char* cp = CodePages;
	while ((*cp != 0) && (FontsN < MAXBMPNUM))
	{
		if (!LoadFont(cp)) return 5;
		cp += strlen(cp) + 1;
	}

	// calculate picture width, first and last character
	w = 0;
	FONTFILE* font;
	FONTCHAR* fontchar;
	min = 256;
	max = 0;
	for (i = 0; i < 256; i++)
	{
		if (FindChar(CP1250ToUniTab[i], &font, &fontchar))
		{
			if (i > max) max = i;
			if (i < min) min = i;
			w += fontchar->w;
		}
	}

	// no character
	if (max < min)
	{
		printf("Invalid font file!\n");
		return 6;
	}

	// prepare output number of bits per pixel
	int b;
	switch (forminx)
	{
	case CF_A4:
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

	// create output file
	f = fopen(argv[2], "w");
	if (f == NULL)
	{
		printf("Cannot create output file!\n");
		return 7;
	}

/*
// picture (size 8 bytes + picture data)
typedef struct PIC_ {
	s16	w;		// width
	s16	h;		// height
	s16	wb;		// bytes per scanline
	u8	colfmt;		// color format COLFMT
	u8	bits;		// number of bits per pixel
	u8	data[1];	// start of data
				//   CF_PAL format: PALETTE structure precedes data
} PIC;
*/
	int picsize = 2 + 2 + 2 + 1 + 1 + FontHeight*wb2;

/*
// font character definition (size 4 bytes)
typedef struct FONTCHAR_ {
	u16	x;		// X coordinate of character in bitmap
	u8	w;		// width of character (0=invalid character)
	u8	spacing;	// spacing: b0..b3: left+7, b4..b7: right+7
} FONTCHAR;

// font (size 8 bytes + character table + font picture)
typedef struct FONT_ {
	u8		first;	// first character
	u8		last;	// last character
	u8		space;	// width of space character
	u8		base;	// base line with cursor
	u16		pic;	// offset of picture with font, in CF_A8/CF_A4/CF_A2/CF_A1 format
	u16		res;	// ... reserved (align)
	FONTCHAR fontchar[1];	// font character table
				// ... picture PIC follows
} FONT;
*/

	i = 4*1 + 2*2 + (max - min + 1)*4;

	int fontsize = i + picsize;

	// write head of font
	fprintf(f,  "/* Do not modify, auto generated with guifont.exe! */\n"
				"\n#include \"include.h\"\n"
				"\n"
				"const u8 %s[%u] = {\n"
				"\t%u,\t// First character\n"				// first character
				"\t%u,\t// Last character\n"				// last character
				"\t%u,\t// Width of space\n"				// width of space character
				"\t%u,\t// Base line with cursor\n"			// base line with cursor
				"\t%u,%u,\t// Offset of picture with font\n" // offset of picture with font
				"\t0,0,\t// ... reserved (align)\n"			// ... reserved (align)
				"\t",
				argv[3], fontsize,
				min,
				max,
				FontSpacing,
				FontHeight*3/4,
				i & 0xff, i >> 8
				);

	// write font character table
	j = 0;
	for (i = min; i <= max; i++)
	{
		writeW(j);
		if (FindChar(CP1250ToUniTab[i], &font, &fontchar))
		{
			writeB(fontchar->w);
			writeB((fontchar->left + FONTOFFLR) +
				((fontchar->right + FONTOFFLR) << FONTSHIFTR));
			j += fontchar->w;
		}
		else
		{
			writeB(0);
			writeB(0);
		}
	}

	// write head of picture
	fprintf(f,  "\n"
				"\t%u,%u,\t// Width\n"					// width
				"\t%u,%u,\t// Height\n"					// height
				"\t%u,%u,\t// Bytes per scanline\n"		// bytes per scanline
				"\t%s,\t// Format\n"					// format name
				"\t%u,\t// Bits per pixel\n"			// bits per pixel
				"\t",
				w & 0xff, w >> 8,
				FontHeight & 0xff, FontHeight >> 8,
				wb2 & 0xff, wb2 >> 8,
				formname[forminx],
				b);

	// write picture data
	pos = 0;
	u8 accu;
	int accuN;
	u8 bb;

	for (row = 0; row < FontHeight; row++)
	{
		accuN = 0;
		accu = 0;

		for (i = min; i <= max; i++)
		{
			if (FindChar(CP1250ToUniTab[i], &font, &fontchar))
			{
				s = &font->data[row*font->wb + fontchar->x];
				for (w = fontchar->w; w > 0; w--)
				{
					bb = *s++;

					switch (forminx)
					{
					case CF_A8:
						writeB(bb);
						break;

					case CF_A4:
						if (accuN == 0)
						{
							accu = bb & 0xf0;
							accuN = 1;
						}
						else
						{
							accu |= bb >> 4;
							writeB(accu);
							accuN = 0;
						}
						break;

					case CF_A3:
						switch (accuN)
						{
						case 0:
							accu = bb & 0xc0;
							accuN = 1;
							break;

						case 1:
							accu |= (bb & 0xe0) >> 2;
							accuN = 2;
							break;

						default:
							accu |= bb >> 5;
							writeB(accu);
							accuN = 0;
							break;
						}
						break;

					case CF_A2:
						switch (accuN)
						{
						case 0:
							accu = bb & 0xc0;
							accuN = 1;
							break;

						case 1:
							accu |= (bb & 0xc0) >> 2;
							accuN = 2;
							break;

						case 2:
							accu |= (bb & 0xc0) >> 4;
							accuN = 3;
							break;

						default:
							accu |= bb >> 6;
							writeB(accu);
							accuN = 0;
							break;
						}
						break;

					case CF_A1:
						accu |= (bb & 0x80) >> accuN;
						accuN++;
						if (accuN == 8)
						{
							writeB(accu);
							accuN = 0;
							accu = 0;
						}
						break;
					}
				}
			}
		}

		if (accuN > 0) writeB(accu);
	}

	if (pos != 0) fprintf(f, "\n");
	fprintf(f, "};\n");

	// close output file
	fclose(f);

	return 0;
}
