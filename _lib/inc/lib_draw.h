
// ****************************************************************************
//
//                             Drawing library
//
// ****************************************************************************

#if USE_DRAW		// 1=use Drawing library, 0=not used (lib_draw.*)

#ifndef _LIB_DRAW_H
#define _LIB_DRAW_H

// 32-bit colors, format RGBA (components are 0..255, alpha 0=transparent, 255=opaque)
#define COLOR(r,g,b,a)	((u32)( (r) | ((g)<<8) | ((b)<<16) | ((a)<<24) ))
// - base colors
#define COL_BLACK	COLOR(  0,  0,  0,255)
#define COL_BLUE	COLOR(  0,  0,255,255)
#define COL_GREEN	COLOR(  0,255,  0,255)
#define COL_CYAN	COLOR(  0,255,255,255)
#define COL_RED		COLOR(255,  0,  0,255)
#define COL_MAGENTA	COLOR(255,  0,255,255)
#define COL_YELLOW	COLOR(255,255,  0,255)
#define COL_WHITE	COLOR(255,255,255,255)
#define COL_GRAY	COLOR(128,128,128,255)
// - dark colors
#define COL_DKBLUE	COLOR(  0,  0,127,255)
#define COL_DKGREEN	COLOR(  0,127,  0,255)
#define COL_DKCYAN	COLOR(  0,127,127,255)
#define COL_DKRED	COLOR(127,  0,  0,255)
#define COL_DKMAGENTA	COLOR(127,  0,127,255)
#define COL_DKYELLOW	COLOR(127,127,  0,255)
#define COL_DKWHITE	COLOR(127,127,127,255)
#define COL_DKGRAY	COLOR( 64, 64, 64,255)
// - light colors
#define COL_LTBLUE	COLOR(127,127,255,255)
#define COL_LTGREEN	COLOR(127,255,127,255)
#define COL_LTCYAN	COLOR(127,255,255,255)
#define COL_LTRED	COLOR(255,127,127,255)
#define COL_LTMAGENTA	COLOR(255,127,255,255)
#define COL_LTYELLOW	COLOR(255,255,127,255)
#define COL_LTGRAY	COLOR(192,192,192,255)
// - special colors
#define COL_AZURE	COLOR(  0,127,255,255)
#define COL_ORANGE	COLOR(255,127,  0,255)
#define COL_BROWN	COLOR(192, 96,  0,255)

// Random opaque color
#define COL_RAND	(RandU32()|0xff000000)

// Random transparent color
#define COL_RANDTRANS	(RandU32())

// draw round (filled circle), ring or filled ellipse quarters
#define	DRAW_ROUND_NOTOP	B0			// hide top part of the round
#define DRAW_ROUND_NOBOTTOM	B1			// hide bottom part of the round
#define DRAW_ROUND_NOLEFT	B2			// hide left part of the round
#define DRAW_ROUND_NORIGHT	B3			// hide right part of the round

#define DRAW_ROUND_ALL		0			// draw whole round
#define DRAW_ROUND_TOP		DRAW_ROUND_NOBOTTOM	// draw top half-round
#define DRAW_ROUND_BOTTOM	DRAW_ROUND_NOTOP	// draw bottom half-round
#define DRAW_ROUND_LEFT		DRAW_ROUND_NORIGHT	// draw left half-round
#define DRAW_ROUND_RIGHT	DRAW_ROUND_NOLEFT	// draw right half-round

// draw circle arcs
#define DRAW_CIRCLE_ARC0	B0			// draw arc 0..45 deg
#define DRAW_CIRCLE_ARC1	B1			// draw arc 45..90 deg
#define DRAW_CIRCLE_ARC2	B2			// draw arc 90..135 deg
#define DRAW_CIRCLE_ARC3	B3			// draw arc 135..180 deg
#define DRAW_CIRCLE_ARC4	B4			// draw arc 180..225 deg
#define DRAW_CIRCLE_ARC5	B5			// draw arc 225..270 deg
#define DRAW_CIRCLE_ARC6	B6			// draw arc 270..315 deg
#define DRAW_CIRCLE_ARC7	B7			// draw arc 315..360 deg

#define DRAW_CIRCLE_ALL		0xff			// draw whole circle
#define DRAW_CIRCLE_TOP		(DRAW_CIRCLE_ARC0 | DRAW_CIRCLE_ARC1 | DRAW_CIRCLE_ARC2 | DRAW_CIRCLE_ARC3) // draw top half-circle
#define DRAW_CIRCLE_BOTTOM	(DRAW_CIRCLE_ARC4 | DRAW_CIRCLE_ARC5 | DRAW_CIRCLE_ARC6 | DRAW_CIRCLE_ARC7) // draw bottom half-circle
#define DRAW_CIRCLE_LEFT	(DRAW_CIRCLE_ARC2 | DRAW_CIRCLE_ARC3 | DRAW_CIRCLE_ARC4 | DRAW_CIRCLE_ARC5) // draw left half-circle
#define DRAW_CIRCLE_RIGHT	(DRAW_CIRCLE_ARC6 | DRAW_CIRCLE_ARC7 | DRAW_CIRCLE_ARC0 | DRAW_CIRCLE_ARC1) // draw right half-circle

// draw ellipse arcs
#define DRAW_ELLIPSE_ARC0	B0		// draw arc 0..90 deg
#define DRAW_ELLIPSE_ARC1	B1		// draw arc 90..180 deg
#define DRAW_ELLIPSE_ARC2	B2		// draw arc 180..270 deg
#define DRAW_ELLIPSE_ARC3	B3		// draw arc 270..360 deg

#define DRAW_ELLIPSE_ALL	0x0f		// draw whole ellipse
#define DRAW_ELLIPSE_TOP	(DRAW_ELLIPSE_ARC0 | DRAW_ELLIPSE_ARC1)	// draw top half-ellipse
#define DRAW_ELLIPSE_BOTTOM	(DRAW_ELLIPSE_ARC2 | DRAW_ELLIPSE_ARC3)	// draw bottom half-ellipse
#define DRAW_ELLIPSE_LEFT	(DRAW_ELLIPSE_ARC1 | DRAW_ELLIPSE_ARC2)	// draw left half-ellipse
#define DRAW_ELLIPSE_RIGHT	(DRAW_ELLIPSE_ARC0 | DRAW_ELLIPSE_ARC3)	// draw right half-ellipse

#define DRAW_ELLIPSE_MAXD	430		// max. diameter (X or Y) of the ellipse

// DrawImgMat mode
#define DRAW_IMG_WRAP		0		// wrap image
#define DRAW_IMG_NOBORDER	1		// no border (transparent border)
#define DRAW_IMG_CLAMP		2		// clamp image (use last pixel as border)
#define DRAW_IMG_COLOR		3		// color border
#define DRAW_IMG_TRANSP		4		// transparent image with key color
#define DRAW_IMG_PERSP		5		// perspective floor

// Proportional font definitions
#define FONTMINLR	-7	// minimal left/right spacing
#define FONTMAXLR	8	// maximal left/right spacing
#define FONTOFFLR	7	// offset of left/right spacing
#define FONTSHIFTR	4	// shifts of right spacing

// font character definition
typedef struct {
	u16	x;		// 0x00: X coordinate of character in bitmap
	u8	w;		// 0x02: width of character (0=invalid character)
	u8	spacing;	// 0x03: spacing: bit 0..3: left+7, bit 4..7: right+7
} sFontChar;
STATIC_ASSERT(sizeof(sFontChar) == 0x04, "Incorrect sFontChar!");

// font
// - Only CF_A8 format of the font image is supported in this library
typedef struct {
	u8		first;		// 0x00: first character
	u8		last;		// 0x01: last character
	u8		space;		// 0x02: width of space character
	u8		base;		// 0x03: base line with cursor
	u16		pic;		// 0x04: offset of picture with font
	u16		res;		// 0x06: ... reserved (align)
	sFontChar	fontchar[1];	// 0x08: font character table ... (last-first+1) characters
					// 0x0C: ... picture PIC follows, in CF_A8/CF_A4/CF_A2/CF_A1 format
} sFont;
STATIC_ASSERT(sizeof(sFont) == 0x0C, "Incorrect sFont!");

// palette type
enum PALTYPE {
	PAL_RGB = 0,		// 3 color components: B, G, R
	PAL_RGBT = 1,		// 3 color components: B, G, R, with transparent color
};

/*
typedef struct {
	u16	size;		// total size of this palette structure
	u8	type;		// palette type PALTYPE
	u8	trans;		// transparent color
	RGBENTRY rgb[0];	// color entries
} sPalette;
*/

// picture color format
enum COLFMT {
	// without alpha
	CF_B8G8R8,		// (3B) B8G8R8, 24 bits RGB
//	CF_R8G8B8,		// (3B) R8G8B8, 24 bits RGB
//	CF_R5G6B5,		// (2B) R5G6B5, 16 bits RGB
//	CF_R3G3B2,		// (1B) R3G3B2, 8 bits RGB

	// with alpha
	CF_A8B8G8R8,		// (4B) A8B8G8R8, 24 bits BGR with 8 bits alpha
//	CF_X8B8G8R8,		// (4B) X8B8G8R8, 24 bits BGR with 8 bits not used (=255)
//	CF_A8R8G8B8,		// (4B) A8R8G8B8, 24 bits RGB with 8 bits alpha
//	CF_A1R5G5B5,		// (2B) A1R5G5B5, 15 bits RGB with 1 bit alpha
//	CF_A2R5G5B4,		// (2B) A2R5G5B4, 14 bits RGB with 2 bits alpha
//	CF_A4R4G4B4,		// (2B) A4R4G4B4, 12 bits RGB with 4 bits alpha
	CF_A8,			// (1B) A8, 8 bits alpha
//	CF_A4,			// (1B) A4, 4 bits alpha
//	CF_A3,			// (1B) A3, 2+3+3 bits alpha
//	CF_A2,			// (1B) A2, 2 bits alpha
//	CF_A1,			// (1B) A1, 1 bit alpha

	// compression
//	CF_DXT1C,		// (1/2B) DXT1 compression, 8 byte texel 4x4 without alpha
//	CF_DXT1C2,		// (1/2B) DXT1 compression, 8 byte texel 4x4 without alpha, double sized CF_DXT1C
//	CF_DXT1A,		// (1/2B) DXT1 compression, 8 byte texel 4x4 with 1-bit alpha
//	CF_DXT3,		// (1B) DXT3 compression, 16 byte texel 4x4 with 4-bit alpha
//	CF_DXT5,		// (1B) DXT5 compression, 16 byte texel 4x4 with interpolated alpha

	// palettes
//	CF_PAL,			// (1B) 8 bits color with internal palettes (in front of data)
//	CF_PALUNI,		// (1B) 8 bits color with uniform palettes (6*6*6)
//	CF_PALUNIT,		// (1B) 8 bits color with uniform palettes (6*6*6) with transparency
//	CF_PALUNI2,		// (1B) 8 bits color with uniform palettes 2 (6*7*6)
//	CF_PALUNI2T,		// (1B) 8 bits color with uniform palettes 2 (6*7*6) with transparency
//	CF_PALGEM,		// (1B) 8 bits color with Gemtree palettes
//	CF_PALGEMT,		// (1B) 8 bits color with Gemtree palettes with transparency
//	CF_PAL332,		// (1B) 8 bits color with uniform palettes CF_R3G3B2 (8*8*4)
//	CF_PALCOMP,		// (1B) 8 bits color with Compass palettes (2*2*2+Y)
//	CF_PAL1,		// (1B) 8 bits color with palette set 1
//	CF_PAL2,		// (1B) 8 bits color with palette set 2
//	CF_PAL3,		// (1B) 8 bits color with palette set 3
//	CF_PAL4,		// (1B) 8 bits color with palette set 4
//	CF_PAL5,		// (1B) 8 bits color with palette set 5
};

// picture (size 8 bytes + picture data)
//  Fonts: Only CF_A8 format is supported in this library for the fonts.
//  Images: Only CF_A8B8G8R8, CF_R8G8B8 or CF_A8 format is supported in the library for the images.
typedef struct {
	u16	w;		// 0x00: width
	u16	h;		// 0x02: height
	u16	wb;		// 0x04: bytes per scanline
	u8	colfmt;		// 0x06: color format COLFMT
	u8	bits;		// 0x07: number of bits per pixel
	u8	data[4];	// 0x08: start of data (aligned - can be accessed as u16 or u32)
} sPic;
STATIC_ASSERT(sizeof(sPic) == 0x0C, "Incorrect sPic!");
#define SPIC_HEADER_SIZE  8	// sPic header size

// Blend two colors
//  alpha ... 255=full col1, 0=full col2 
u32 BlendCol(u32 col1, u32 col2, int alpha);

// Draw clear screen with given foreground and background color, set the colors as new printing colors
void DrawClearCol(u32 colfg, u32 colbg);

// Draw clear screen with current printing background color (default black)
void DrawClear();

// draw point (alpha 0=transparent, 255=opaque)
void DrawPoint(int x, int y, u32 col);

// invert point
void DrawPointInv(int x, int y);

// get point (returns COL_BLACK if out of clipping range)
u32 DrawGetPoint(int x, int y);

// draw rectangle (w and h can be negative = flip rectangle; alpha 0=transparent, 255=opaque)
void DrawRect(int x, int y, int w, int h, u32 col);

// invert rectangle (w and h can be negative = flip rectangle)
void DrawRectInv(int x, int y, int w, int h);

// draw horizontal line (w can be negative = flip line; alpha 0=transparent, 255=opaque)
void DrawHLine(int x, int y, int w, u32 col);

// invert horizontal line (w can be negative = flip line)
void DrawHLineInv(int x, int y, int w);

// draw vertical line (h can be negative = flip line; alpha 0=transparent, 255=opaque)
void DrawVLine(int x, int y, int h, u32 col);

// invert vertical line (h can be negative = flip line)
void DrawVLineInv(int x, int y, int h);

// draw frame (dimensions must be > 0; alpha 0=transparent, 255=opaque)
void DrawFrame(int x, int y, int w, int h, u32 col);
void DrawFrame2(int x, int y, int w, int h, u32 col_light, u32 col_dark);

// invert frame (dimensions must be > 0)
void DrawFrameInv(int x, int y, int w, int h);

// draw thick frame (dimensions must be > 0; alpha 0=transparent, 255=opaque)
void DrawFrameW(int x, int y, int w, int h, int thick, u32 col);

// invert thick frame (dimensions must be > 0)
void DrawFrameWInv(int x, int y, int w, int h, int thick);

// draw line (alpha 0=transparent, 255=opaque)
void DrawLine(int x1, int y1, int x2, int y2, u32 col);

// invert line
void DrawLineInv(int x1, int y1, int x2, int y2);

// Draw thick line (alpha 0=transparent, 255=opaque)
//  thick ... thick of line in pixels
//  round ... draw round ends
// Do not use "round ends" and "transparent" at the same time - round ends incorrectly redraw transparency.
void DrawLineW(int x1, int y1, int x2, int y2, int thick, Bool round, u32 col);

// Draw thick line inverted
//  thick ... thick of line in pixels
void DrawLineWInv(int x1, int y1, int x2, int y2, int thick);

// Draw round (alpha 0=transparent, 255=opaque)
//  diam ... diameter of the round (radius = d/2)
//  mask ... hide parts of the round with DRAW_ROUND_* (or their combination); use DRAW_ROUND_ALL or 0 to draw whole round
void DrawRound(int x, int y, int diam, int mask, u32 col);

// Invert round
void DrawRoundInv(int x, int y, int diam, int mask);

// Draw circle or arc (alpha 0=transparent, 255=opaque)
//  diam ... diameter of the circle (radius = d/2)
//  mask ... draw circle arcs, use combination of DRAW_CIRCLE_*; use DRAW_CIRCLE_ALL or 0xff to draw whole circle
void DrawCircle(int x, int y, int diam, int mask, u32 col);

// Invert circle or arc
void DrawCircleInv(int x, int y, int diam, int mask);

// Draw ellipse (alpha 0=transparent, 255=opaque)
//  dx ... diameter of ellipse in X direction, range 2..430 (X radius = dx/2)
//  dy ... diameter of ellipse in Y direction, range 2..430 (Y radius = dy/2)
//  mask ... draw ellipse arcs, use combination of DRAW_ELLIPSE_*; use DRAW_ELLIPSE_ALL or 0x0f to draw whole ellipse
void DrawEllipse(int x, int y, int dx, int dy, int mask, u32 col);

// Invert ellipse
void DrawEllipseInv(int x, int y, int dx, int dy, int mask);

// Draw filled ellipse (alpha 0=transparent, 255=opaque)
//  dx ... diameter of ellipse in X direction, range 2..430 (X radius = dx/2)
//  dy ... diameter of ellipse in Y direction, range 2..430 (Y radius = dy/2)
//  mask ... hide parts of the ellipse with DRAW_ROUND_* (or their combination); use DRAW_ROUND_ALL or 0 to draw whole ellipse
void DrawFillEllipse(int x, int y, int dx, int dy, int mask, u32 col);

// Invert filled ellipse
void DrawFillEllipseInv(int x, int y, int dx, int dy, int mask);

// Draw ring (alpha 0=transparent, 255=opaque)
//  dout ... outer diameter of the ring (outer radius = dout/2)
//  din ... inner diameter of the ring (inner radius = din/2)
//  mask ... hide parts of the ring with DRAW_ROUND_* (or their combination); use DRAW_ROUND_ALL or 0 to draw whole ring
void DrawRing(int x, int y, int dout, int din, int mask, u32 col);

// Invert ring
void DrawRingInv(int x, int y, int dout, int din, int mask);

// Draw triangle (alpha 0=transparent, 255=opaque)
void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, u32 col);

// Invert triangle
void DrawTriangleInv(int x1, int y1, int x2, int y2, int x3, int y3);

// Draw fill area (no blending)
void DrawFill(int x, int y, u32 col);

// Draw image (image must be in aligned CF_A8B8G8R8 or CF_B8G8R8 format; alpha 0=transparent, 255=opaque)
#define IMG_MAXWH	0x20000		// can be used as 'w' and 'h' to display the entire image
void DrawImg(const u8* img, int x, int y, int xs=0, int ys=0, int w=IMG_MAXWH, int h=IMG_MAXWH, int alpha=255);
//#define DRAWIMG(img, x, y) DrawImg(img, x, y, 0, 0, IMG_MAXWH, IMG_MAXWH, 255)

// Invert image (image must be in aligned CF_A8B8G8R8 or CF_B8G8R8 format)
void DrawImgInv(const u8* img, int x, int y, int xs, int ys, int w, int h);

// Draw image mask (image must be in CF_A8 format; alpha 0=transparent, 255=opaque)
void DrawImgMask(const u8* img, int x, int y, int xs, int ys, int w, int h, u32 col);

// Invert image mask (image must be in CF_A8 format)
void DrawImgMaskInv(const u8* img, int x, int y, int xs, int ys, int w, int h);

// draw character using system fixed font (alpha 0=transparent, 255=opaque; use DrawSelFont() to select font)
// - To invert text, use bit 7 of the characters, or FrameBuffer.inv flag (flags are XORed).
void DrawCharScale(u8 ch, int x, int y, int scalex, int scaley, u32 col);
void DrawChar(u8 ch, int x, int y, u32 col);
void DrawCharW(u8 ch, int x, int y, u32 col);
void DrawCharH(u8 ch, int x, int y, u32 col);
void DrawChar2(u8 ch, int x, int y, u32 col);
void DrawChar3(u8 ch, int x, int y, u32 col);
void DrawChar4(u8 ch, int x, int y, u32 col);

// draw character with background, using system fixed font (no alpha transparency; use DrawSelFont() to select font)
// - To invert text, use bit 7 of the characters, or FrameBuffer.inv flag (flags are XORed).
void DrawCharBgScale(u8 ch, int x, int y, int scalex, int scaley, u32 colfg, u32 colbg);
void DrawCharBg(u8 ch, int x, int y, u32 colfg, u32 colbg);
void DrawCharBgW(u8 ch, int x, int y, u32 colfg, u32 colbg);
void DrawCharBgH(u8 ch, int x, int y, u32 colfg, u32 colbg);
void DrawCharBg2(u8 ch, int x, int y, u32 colfg, u32 colbg);
void DrawCharBg3(u8 ch, int x, int y, u32 colfg, u32 colbg);
void DrawCharBg4(u8 ch, int x, int y, u32 colfg, u32 colbg);

// invert character using system fixed font (use DrawSelFont() to select font)
// - To invert text, use bit 7 of the characters, or FrameBuffer.inv flag (flags are XORed).
void DrawCharScaleInv(u8 ch, int x, int y, int scalex, int scaley);
void DrawCharInv(u8 ch, int x, int y);
void DrawCharWInv(u8 ch, int x, int y);
void DrawCharHInv(u8 ch, int x, int y);
void DrawChar2Inv(u8 ch, int x, int y);
void DrawChar3Inv(u8 ch, int x, int y);
void DrawChar4Inv(u8 ch, int x, int y);

// draw text with length, using system fixed font (alpha 0=transparent, 255=opaque; use DrawSelFont() to select font)
// - To invert text, use bit 7 of the characters, or FrameBuffer.inv flag (flags are XORed).
// - Returns shift of X in pixels.
int DrawTextLenScale(const char* text, int len, int x, int y, int scalex, int scaley, u32 col);
int DrawTextLen(const char* text, int len, int x, int y, u32 col);
int DrawTextLenW(const char* text, int len, int x, int y, u32 col);
int DrawTextLenH(const char* text, int len, int x, int y, u32 col);
int DrawTextLen2(const char* text, int len, int x, int y, u32 col);
int DrawTextLen3(const char* text, int len, int x, int y, u32 col);
int DrawTextLen4(const char* text, int len, int x, int y, u32 col);

// invert text with length, using system fixed font (use DrawSelFont() to select font)
// - To invert text, use bit 7 of the characters, or FrameBuffer.inv flag (flags are XORed).
// - Returns shift of X in pixels.
int DrawTextLenScaleInv(const char* text, int len, int x, int y, int scalex, int scaley);
int DrawTextLenInv(const char* text, int len, int x, int y);
int DrawTextLenWInv(const char* text, int len, int x, int y);
int DrawTextLenHInv(const char* text, int len, int x, int y);
int DrawTextLen2Inv(const char* text, int len, int x, int y);
int DrawTextLen3Inv(const char* text, int len, int x, int y);
int DrawTextLen4Inv(const char* text, int len, int x, int y);

// draw ASCIIZ text (terminated with zero), using system fixed font (alpha 0=transparent, 255=opaque; use DrawSelFont() to select font)
// - To invert text, use bit 7 of the characters, or FrameBuffer.inv flag (flags are XORed).
// - Returns shift of X in pixels.
int DrawTextScale(const char* text, int x, int y, int scalex, int scaley, u32 col);
int DrawText(const char* text, int x, int y, u32 col);
int DrawTextW(const char* text, int x, int y, u32 col);
int DrawTextH(const char* text, int x, int y, u32 col);
int DrawText2(const char* text, int x, int y, u32 col);
int DrawText3(const char* text, int x, int y, u32 col);
int DrawText4(const char* text, int x, int y, u32 col);

// invert ASCIIZ text (terminated with zero), using system fixed font (use DrawSelFont() to select font)
// - To invert text, use bit 7 of the characters, or FrameBuffer.inv flag (flags are XORed).
// - Returns shift of X in pixels.
int DrawTextScaleInv(const char* text, int x, int y, int scalex, int scaley);
int DrawTextInv(const char* text, int x, int y);
int DrawTextWInv(const char* text, int x, int y);
int DrawTextHInv(const char* text, int x, int y);
int DrawText2Inv(const char* text, int x, int y);
int DrawText3Inv(const char* text, int x, int y);
int DrawText4Inv(const char* text, int x, int y);

// draw text with length and background, using system fixed font (no alpha transparency; use DrawSelFont() to select font)
// - To invert text, use bit 7 of the characters, or FrameBuffer.inv flag (flags are XORed).
// - Returns shift of X in pixels.
int DrawTextBgLenScale(const char* text, int len, int x, int y, int scalex, int scaley, u32 colfg, u32 colbg);
int DrawTextBgLen(const char* text, int len, int x, int y, u32 colfg, u32 colbg);
int DrawTextBgLenW(const char* text, int len, int x, int y, u32 colfg, u32 colbg);
int DrawTextBgLenH(const char* text, int len, int x, int y, u32 colfg, u32 colbg);
int DrawTextBgLen2(const char* text, int len, int x, int y, u32 colfg, u32 colbg);
int DrawTextBgLen3(const char* text, int len, int x, int y, u32 colfg, u32 colbg);
int DrawTextBgLen4(const char* text, int len, int x, int y, u32 colfg, u32 colbg);

// draw ASCIIZ text with background (terminated with zero), using system fixed font (no alpha transparency; use DrawSelFont() to select font)
// - To invert text, use bit 7 of the characters, or FrameBuffer.inv flag (flags are XORed).
// - Returns shift of X in pixels.
int DrawTextBgScale(const char* text, int x, int y, int scalex, int scaley, u32 colfg, u32 colbg);
int DrawTextBg(const char* text, int x, int y, u32 colfg, u32 colbg);
int DrawTextBgW(const char* text, int x, int y, u32 colfg, u32 colbg);
int DrawTextBgH(const char* text, int x, int y, u32 colfg, u32 colbg);
int DrawTextBg2(const char* text, int x, int y, u32 colfg, u32 colbg);
int DrawTextBg3(const char* text, int x, int y, u32 colfg, u32 colbg);
int DrawTextBg4(const char* text, int x, int y, u32 colfg, u32 colbg);

// draw character using proportional font, returns shift of X coordinate (alpha 0=transparent, 255=opaque; to select font, use DrawSelFontProp())
//  first ... flag of first character (= do not apply left spacing)
int DrawCharProp(u8 ch, int x, int y, Bool first, u32 col);

// invert character using proportional font, returns shift of X coordinate (to select font, use DrawSelFontProp())
//  first ... flag of first character (= do not apply left spacing)
int DrawCharPropInv(u8 ch, int x, int y, Bool first);

// draw text with length using proportional font (alpha 0=transparent, 255=opaque; to select font, use DrawSelFontProp())
// - Returns shift of X in pixels.
int DrawTextPropLen(const char* text, int len, int x, int y, u32 col);

// invert text with length using proportional font (to select font, use DrawSelFontProp())
// - Returns shift of X in pixels.
int DrawTextPropLenInv(const char* text, int len, int x, int y);

// draw ASCIIZ text using proportional font (alpha 0=transparent, 255=opaque; to select font, use DrawSelFontProp())
// - Returns shift of X in pixels.
int DrawTextProp(const char* text, int x, int y, u32 col);

// invert ASCIIZ text using proportional font (to select font, use DrawSelFontProp())
// - Returns shift of X in pixels.
int DrawTextPropInv(const char* text, int x, int y);

// get length in pixels of character using proportional font (to select font, use DrawSelFontProp())
//  first ... flag of first character (= do not apply left spacing)
int DrawCharPropDX(u8 ch, Bool first);

// get length in pixels of text with length using proportional font (to select font, use DrawSelFontProp())
int DrawTextPropLenDX(const char* text, int len);

// get length in pilxes of ASCIIZ text using proportional font (to select font, use DrawSelFontProp())
int DrawTextPropDX(const char* text);

// scroll screen (in clipping area) up one row (does not update current cursor row)
void DrawScroll();

// scroll screen in given area up one row (does not update current cursor row)
void DrawScrollRect(int x, int y, int w, int h);

// console print character (without display update - use DispUdpate() or DispUpdateAuto())
//   Control characters:
//     0x01 '\1' ^A ... set not-inverted text
//     0x02 '\2' ^B ... set inverted text (XORer with bit 7 of the character)
//     0x03 '\3' ^C ... use normal-sized font (default)
//     0x04 '\4' ^D ... use double-height font
//     0x05 '\5' ^E ... use double-width font
//     0x06 '\6' ^F ... use double-sized font
//     0x07 '\a' ^G ... (bell) move cursor 1 position right (no print; uses width of normal-sized font)
//     0x08 '\b' ^H ... (back space) move cursor 1 position left (no print; uses width of normal-sized font)
//     0x09 '\t' ^I ... (tabulator) move cursor to next 8-character position, print spaces (uses width of normal-sized font)
//     0x0A '\n' ^J ... (new line) move cursor to start of next row
//     0x0B '\v' ^K ... (vertical tabulator) move cursor to start of previous row
//     0x0C '\f' ^L ... (form feed) clear screen, reset cursor position and set default color
//     0x0D '\r' ^M ... (carriage return) move cursor to start of current row
//     0x10 '\20' ^P ... set gray text color (COL_GRAY, default)
//     0x11 '\21' ^Q ... set blue text color (COL_AZURE)
//     0x12 '\22' ^R ... set green text color (COL_GREEN)
//     0x13 '\23' ^S ... set cyan text color (COL_CYAN)
//     0x14 '\24' ^T ... set red text color (COL_RED)
//     0x15 '\25' ^U ... set magenta text color (COL_MAGENTA)
//     0x16 '\26' ^V ... set yellow text color (COL_YELLOW)
//     0x17 '\27' ^W ... set white text color (COL_WHITE)
void DrawPrintChar(char ch);

// print text to console with length (Control characters - see DrawPrintCharRaw)
void DrawPrintTextLen(const char* text, int len);

// print ASCIIZ text to console (Control characters - see DrawPrintCharRaw)
//  If text contains digit after hex numeric code of control character,
//  split text to more parts: use "\4" "1" instead of "\41".
void DrawPrintText(const char* text);

#if USE_STREAM	// use Data stream (lib_stream.c, lib_stream.h)

// formatted print string to drawing console, with argument list, without DispUpdate() (returns number of characters, without terminating 0)
u32 DrawPrintArg(const char* fmt, va_list args);

// formatted print string to drawing console, with variadic arguments, without DispUpdate() (returns number of characters, without terminating 0)
NOINLINE u32 DrawPrint(const char* fmt, ...);

#endif // USE_STREAM

#endif // _LIB_DRAW_H

#endif // USE_DRAW
