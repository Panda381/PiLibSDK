
// ****************************************************************************
//
//                             Frame Buffer
//
// ****************************************************************************
// If you use uncached video memory (MMU_CACHE_MODE = 1), it will not be necessary
// to invalidate the cache memory after writing. However, it is important to
// remember that uncached memory does not allow unaligned access. This mainly
// applies to the memcpy() function, which internally uses unaligned access -
// using it on unaligned memory can therefore cause the system to crash.

#ifndef _SDK_FRAMEBUF_H
#define _SDK_FRAMEBUF_H

// Frame Buffer (bit depth is always 32 bits per pixel, format RGBA, memory byte order R-G-B-A)
// - Do not set clipping range out of valid drawing buffer!
typedef struct {
	// screen frame buffer
	u32		screenwidth;	// screen width
	u32		screenheight;	// screen height
	u32		screenpitch;	// screen pitch (byte offset of the row)
	u32		screenpitchpix;	// screen pitch in pixels u32
	u32*		screenbuf;	// pointer to screen buffer
	// drawing frame buffer
	u32		drawwidth;	// drawing width
	u32		drawheight;	// drawing height
	u32		drawpitch;	// drawing pitch (byte offset of the row)
	u32		drawpitchpix;	// drawing pitch in pixels u32
	u32*		drawbuf;	// pointer to drawing buffer
	int		drawscale;	// scale of drawing buffer (= 1, 2 or 4; scrensize = drawsize*drawscale)
	// draw clipping
	int		minx;		// clipping - minimal X (default = 0)
	int		maxx;		// clipping - maximal X + 1 (default = drawwidth)
	int		miny;		// clipping - minimal Y (default = 0);
	int		maxy;		// clipping - maximal Y + 1 (default = drawheight)
	// draw font
	const u8*	font;		// pointer to current selected system fixed font (data of bitmap image)
	int		fontw;		// width of character of system fixed font (5 to 8 pixels)
	int		fonth;		// height of system fixed font
	const u8*	fontprop;	// pointer to current selected proportional font (format "sFont")
	// printing (functions DrawPrintChar(), DrawPrintText(), DrawPrint())
	u32		lastupdate;	// time in [us] of last display update DispUpdate()
	Bool		inv;		// flag - invert text with system fixed font (XORed with bit 7 of the characters)
	int		printsize;	// text size 0=normal, 1=double-height, 2=double-width, 3=double-size
	int		printposnum;	// number of text positions per row (= drawwidth / fontw)
	int		printrownum;	// number of text rows (= drawheight / fonth)
	int		printpos;	// print position
	int		printrow;	// print row
	u32		printcolfg;	// print color - foreground (COL_GRAY default)
	u32		printcolbg;	// print color - background (COL_BLACK default)
} sFrameBuffer;

// current frame buffer
extern sFrameBuffer FrameBuffer;

// default font and screen
#ifndef FONT
#define FONT		FontBold8x16	// default fixed font
#endif

#ifndef FONTW
#define FONTW		8		// width of fixed font
#endif

#ifndef FONTH
#define FONTH		16		// height of fixed font
#endif

#ifndef FONTPROP
#define FONTPROP	FontAR14	// default proportional font
#endif

// Screen config
#ifndef WIDTH
#define WIDTH		640		// draw width (0=do not initialize default videomode)
#endif

#ifndef HEIGHT
#define HEIGHT		480		// draw height
#endif

#ifndef SCREENSCALE
#define SCREENSCALE	1		// 0 = no back buffer, using back buffer with screen scale 1, 2 or 4
#endif

#ifndef AUTOUPDATE_MS
#define AUTOUPDATE_MS	100		// interval of display auto-update in [ms]
#endif

#if WIDTH	// draw width (0=do not initialize default videomode)
#if SCREENSCALE // 0 = no back buffer, using back buffer with screen scale 1, 2 or 4
// back buffer
extern u32 BackBuf[WIDTH*HEIGHT];
#endif
#endif

// initialize frame buffer (returns False on error)
// The image is adjusted to the target monitor size using bilinear interpolation,
// which causes blurring of low-resolution images. If you are using a low-resolution
// video mode, use larger dimensions for the frame buffer and transfer the image from
// the draw buffer to the frame buffer with enlargement without interpolation (set drawscale to 2 or 4).
Bool FrameBufferInit(int width, int height);

// select to draw directly to screen buffer
void SelectScreenBuf(void);

// select to draw to back buffer (select scale = 1, 2 or 4)
void SelectDrawBuf(u32* buf, int scale);

// select current system fixed font (1-bit image with width 128 characters, total width 1024 pixels, 1 character width max. 8 pixels)
void DrawSelFont(const u8* font, int w, int h);

INLINE void SelFont8x8() { DrawSelFont(FontBold8x8, 8, 8); }
INLINE void SelFont8x12() { DrawSelFont(FontBold8x12, 8, 12); }
INLINE void SelFont8x14() { DrawSelFont(FontBold8x14, 8, 14); }
INLINE void SelFont8x14B() { DrawSelFont(FontBold8x14B, 8, 14); }
INLINE void SelFont8x16() { DrawSelFont(FontBold8x16, 8, 16); }
INLINE void SelFont8x16B() { DrawSelFont(FontBold8x16B, 8, 16); }
INLINE void SelFont6x6() { DrawSelFont(FontCond6x6, 6, 6); }
INLINE void SelFont6x6B() { DrawSelFont(FontCond6x6B, 6, 6); }
INLINE void SelFont6x8() { DrawSelFont(FontCond6x8, 6, 8); }
INLINE void SelFont5x8() { DrawSelFont(FontTiny5x8, 5, 8); }
INLINE void SelFont8x8Game() { DrawSelFont(FontGame8x8, 8, 8); }
INLINE void SelFont8x8Italic() { DrawSelFont(FontItalic8x8, 8, 8); }
INLINE void SelFont8x8Thin() { DrawSelFont(FontThin8x8, 8, 8); }

// select current proportional font (with format "sFont")
INLINE void DrawSelFontProp(const u8* font) { FrameBuffer.fontprop = font; }

// enable or disable text inversion (only system fixed font)
INLINE void DrawInvOn(void) { FrameBuffer.inv = True; }
INLINE void DrawInvOff(void) { FrameBuffer.inv = False; }

// update draw buffer (send to screen buffer)
void DispUpdate();

// update draw buffer with auto interval
void DispUpdateAuto();

// initialize default videomode (called from SysInit)
void DispInit(void);

#endif // _SDK_FRAMEBUF_H
