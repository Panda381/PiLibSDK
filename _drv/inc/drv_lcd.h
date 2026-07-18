
// ****************************************************************************
//
//                LCD display driver with SPI1 interface
//
// ****************************************************************************
// Currently supported chips:
//	ST7789, TFT-LCD resolution up to 240x320
//	   Usual configurations:
//		240x320: LCD_MODEL_ST7789, 240, 320, 0, 0
//		...240x240: LCD_MODEL_ST7789, 240, 240, 0, 0 (not tested)
//		...240x135: LCD_MODEL_ST7789, 135, 240, 53, 40 (not tested)
//	ST7735, ST7735S, resolution up to 132x162
//	   Usual configurations:
//		80x160: LCD_MODEL_ST7735, 80, 160, 26, 1
//		...128x160: LCD_MODEL_ST7735, 128, 160, 2, 1 (not tested)

// Other possible chips (not tested):
//	ILI9341 320x240
//	GC9A01 240x240
//	ST7796, ST7796S 320x280
//	ILI9163 128x128
//	SSD1351, SSD1331 128x128 or 96x64
// https://github.com/adafruit/Adafruit-ST7735-Library

#if USE_LCD		// 1=use LCD SPI display driver, 0=not used (drv_lcd.*)

#ifndef _DRV_LCD_H
#define _DRV_LCD_H

// LCD default transfer speed in Hz
#ifndef LCD_DEF_SPEED
#define LCD_DEF_SPEED	30000000
#endif

/*
	Pull	ALT0		ALT1		ALT2		ALT3		ALT4		ALT5		usual function
GPIO16	Low	-		SD8		-		CTS0		SPI1_CE2_N	CTS1		SPI1 CE2
GPIO17	Low	-		SD9		-		RTS0		SPI1_CE1_N	RTS1		SPI1 CE1
GPIO18	Low	PCM_CLK		SD10		-		BSCSL_SDA/MOSI	SPI1_CE0_N	PWM0		SPI1 CE0, PWM0 audio L
GPIO19	Low	PCM_FS		SD11		-		BSCSL_SCL/SCLK	SPI1_MISO	PWM1		SPI1 MISO, PWM1 audio R
GPIO20	Low	PCM_DIN		SD12		-		BSCSL/MISO	SPI1_MOSI	GPCLK0		SPI1 MOSI, clock0
GPIO21	Low	PCM_DOUT	SD13		-		BSCSL/CE_N	SPI1_SCLK	GPCLK1		SPI1 SCLK, clock1
*/

// GPIOs
#ifndef LCD_DC_GPIO
#define LCD_DC_GPIO	16	// GPIO with LCD_DC signal (ALT4:SPI1_CE2_N)
#endif

#ifndef LCD_RES_GPIO
#define LCD_RES_GPIO	17	// GPIO with LCD_RES signal (ALT4:SPI1_CE1_N), or set -1 if RES signal is not used
#endif

#ifndef LCD_CS_GPIO
#define LCD_CS_GPIO	18	// GPIO with LCD_CS signal (ALT0:PCM_CLK, ALT4:SPI1_CE0_N)
#endif

#define LCD_BL_GPIO	19	// GPIO with LCD_BL signal (ALT0:PCM_FS, ALT4:SPI1_MISO), or set -1 if BL signal is not used
// The display backlight will be adjusted only if GPIO 19 (the PCM_FS signal) is used. Otherwise, the backlight will remain at maximum brightness.

#define LCD_MOSI_GPIO	20	// GPIO with LCD_MOSI signal (ALT0:PCM_DIN, ALT4:SPI1_MOSI) ... GPIO cannot be changed
#define LCD_SCK_GPIO	21	// GPIO with LCD_SCK signal (ALT0:PCM_DOUT, ALT4:SPI_SCLK) ... GPIO cannot be changed

#define LCD_BL_MIN	0	// minimal backlight level
#define LCD_BL_MAX	10	// maximal backlight level
#define LCD_BL_DEF	7	// default backlight level

// display model
enum {
	LCD_MODEL_NONE = 0,	// no display model
	LCD_MODEL_ST7789,	// ST7789
	LCD_MODEL_ST7735,	// ST7735 or ST7735S
};

// display rotation
enum {
	LCD_ROT_0 = 0,		// display native resolution
	LCD_ROT_90,
	LCD_ROT_180,
	LCD_ROT_270,
};

// LCD display device
class cLCD
{
public:

	// display
	int	model;		// display model LCD_MODEL_* (LCD_MODEL_NONE = no valid display)
	int	dispw;		// display physical width in pixels (columns) - viewport width
	int	disph;		// display physical height in pixels (rows) - viewport height
	int	offx;		// display physical start offset X (columns) - viewport X
	int	offy;		// display physical start offset Y (rows) - viewport Y
	int	maxw;		// display physical max. width in pixels (columns)
	int	maxh;		// display physical max. height in pixels (rows)
	int	speed;		// transfer speed in Hz
	Bool	on;		// True = display is ON enabled
	int	bl;		// backlight level in range 0..10
	int	zoom;		// display zoom of main frame buffer (0..4, 0=full screen)

	// rotation
	int	rot;		// image rotation LCD_ROT_*
	int	xstart;		// start X of drawing window
	int	ystart;		// start Y of drawinf window

	// frame buffer
	u32*	framebuf;	// pointer to frame buffer with format 32-bits per pixel, size dispw*disph*4 bytes (can be shared with main display frame buffer)
	int	w;		// frame buffer width
	int	h;		// frame buffer height
	int	wb;		// frame buffer width in bytes

	// font
	const u8* font;		// draw font (cells 8 pixels, 128 characters)
	int	fontw;		// font width (5 to 8 pixels)
	int	fonth;		// font height
	Bool	printinv;	// print invert

	// check if LCD display is valid (model is not LCD_MODEL_NONE)
	INLINE Bool IsValid() { return this->model != LCD_MODEL_NONE; }

	// write byte
	//  After sending a byte, it does not wait for the transmission
	//  to complete - call WaitBusy() before ending the transaction.
	void Write8(u8 data);

	// wait for the transmission to complet
	void WaitBusy();

	// display connect (activate chip selection)
	void Connect();

	// display disconnect (deactivate chip selection)
	void Disconnect();

	// write command byte
	void WriteCmd(u8 cmd);

	// write data byte
	void WriteData(u8 data);

	// set display rotation LCD_ROT_*
	void SetRot(int rot);

	// setup drawing window (and connect display; image data should follow)
	void SetWindow(int x, int y, int w, int h);

	// set backlight control (range 0..10)
	void Backlight(int bl);

	// Detect display module (returns LCD_MODEL_*, or LCD_MODEL_NONE on error)
	// Supported module and their detection:
	// - Display module ST7789 320x240, rotation LCD_ROT_270: resistor 10K between signals DC and CS
	// - Display module ST7735 160x80, rotation LCD_ROT_270: resistor 10K between signals RES and CS
	// After detection, the display must be fully initialized, because modifying the pin states may have caused the display to reset.
	static int Detect();

	// Auto-initialize display module (returns False on error)
	// Supported module and their detection:
	// - Display module ST7789 320x240, rotation LCD_ROT_270: resistor 10K between signals DC and CS
	// - Display module ST7735 160x80, rotation LCD_ROT_270: resistor 10K between signals RES and CS
	// Initialization is performed with framebuf=NULL. The image can be output from the
	// main frame buffer using the UpdateMain() function. To use a custom frame buffer,
	// set its address in the variable this->framebuf.
	// To check later whether the initialization was successful, check whether
	// the "model" variable contains the value "LCD_MODEL_NONE".
	Bool AutoInit();

	// initialize
	//  model ... display model LCD_MODEL_*
	//  dispw ... display physical width in pixels (columns) - viewport width
	//  disph ... display physical height in pixels (rows) - viewport height
	//  offx ... display physical start offset X (columns) - viewport X
	//  offy ... display physical start offset Y (rows) - viewport Y
	//  framebuf ... pointer to frame buffer with format 32-bits per pixel, size dispw*disph*4 bytes (can be shared with main display frame buffer)
	//  rot ... image rotation LCD_ROT_*
	//  speed ... transfer speed in Hz (30517..125000000 Hz)
	// Frame buffer can be shared with the main screen (uses the same graphics format).
	// The frame buffer is not accessed during initialization, it can be set up later.
	// The display will remain OFF until the first Update().
	// The physical dimensions and offset apply to the default rotation of 0.
	// The framebuf can be NULL if you are updating using the UpdateMain() function from the main frame buffer.
	void Init(int model, int dispw, int disph, int offx, int offy, u32* framebuf, int rot = LCD_ROT_0, int speed = LCD_DEF_SPEED);

	// re-initialize to another viewport
	//  dispw ... display physical width in pixels (columns) - viewport width
	//  disph ... display physical height in pixels (rows) - viewport height
	//  offx ... display physical start offset X (columns) - viewport X
	//  offy ... display physical start offset Y (rows) - viewport Y
	// If you need to change the frame buffer address, set the framebuf variable.
	// The physical dimensions and offset apply to the default rotation of 0.
	void ReInit(int dispw, int disph, int offx, int offy);

	// terminate
	void Term();

	// clear display without using frame buffer and without enable display
	void UpdateClearCol(u32 col);

	// display update
	void Update();

	// display update from main frame buffer - only if driver is valid (does not use its own frame buffer)
	void UpdateMain();

	// set font
	//  font ... draw font (cells 8 pixels, 128 characters)
	//  fontw ... font width (5 to 8 pixels)
	//  fonth ... font height
	void SetFont(const u8* font, int fontw, int fonth);

	// clear screen with given color
	void DrawClearCol(u32 col);

	// clear screen with black color
	void DrawClear();

	// copy frame buffer to drawing buffer of the main screen (It doesn't check
	//   the validity of the coordinates; it just quickly copies the memory)
	void CopyToScreen(int x, int y);

	// copy frame buffer from drawing buffer of the main screen (It doesn't check
	//   the validity of the coordinates; it just quickly copies the memory)
	void CopyFromScreen(int x, int y);

	// draw point (alpha 0=transparent, 255=opaque)
	void DrawPoint(int x, int y, u32 col);

	// get pixel (returns color COL_*)
	u32 GetPoint(int x, int y);

	// draw rectangle (w and h can be negative = flip rectangle; alpha 0=transparent, 255=opaque)
	void DrawRect(int x, int y, int w, int h, u32 col);

	// draw horizontal line (w can be negative = flip line; alpha 0=transparent, 255=opaque)
	void DrawHLine(int x, int y, int w, u32 col);

	// draw vertical line (h can be negative = flip line; alpha 0=transparent, 255=opaque)
	void DrawVLine(int x, int y, int h, u32 col);

	// draw frame (dimensions must be > 0; alpha 0=transparent, 255=opaque)
	void DrawFrame(int x, int y, int w, int h, u32 col);

	// draw frame 3D (dimensions must be > 0; alpha 0=transparent, 255=opaque)
	void DrawFrame2(int x, int y, int w, int h, u32 col_light, u32 col_dark);

	// draw thick frame (dimensions must be > 0; alpha 0=transparent, 255=opaque)
	void DrawFrameW(int x, int y, int w, int h, int thick, u32 col);

	// Internal function - draw line with overlapped pixels (alpha 0=transparent, 255=opaque)
	void DrawLineOver(int x1, int y1, int x2, int y2, Bool over, u32 col);

	// draw line (alpha 0=transparent, 255=opaque)
	void DrawLine(int x1, int y1, int x2, int y2, u32 col);

	// Draw thick line (alpha 0=transparent, 255=opaque)
	//  thick ... thick of line in pixels
	//  round ... draw round ends
	// Do not use "round ends" and "transparent" at the same time - round ends incorrectly redraw transparency.
	void DrawLineW(int x1, int y1, int x2, int y2, int thick, Bool round, u32 col);

	// Draw round (alpha 0=transparent, 255=opaque)
	//  diam ... diameter of the round (radius = d/2)
	//  mask ... hide parts of the round with DRAW_ROUND_* (or their combination); use DRAW_ROUND_ALL or 0 to draw whole round
	//		DRAW_ROUND_NOTOP	= hide top part of the round
	//		DRAW_ROUND_NOBOTTOM	= hide bottom part of the round
	//		DRAW_ROUND_NOLEFT	= hide left part of the round
	//		DRAW_ROUND_NORIGHT	= hide right part of the round
	//		DRAW_ROUND_ALL		= draw whole round
	void DrawRound(int x, int y, int diam, int mask, u32 col);

	// Draw circle or arc (alpha 0=transparent, 255=opaque)
	//  diam ... diameter of the circle (radius = d/2)
	//  mask ... draw circle arcs, use combination of DRAW_CIRCLE_*; use DRAW_CIRCLE_ALL or 0xff to draw whole circle
	void DrawCircle(int x, int y, int diam, int mask, u32 col);

	// Draw image
	//  img ... image in format sPic, must be in aligned CF_A8B8G8R8 or CF_B8G8R8 format
	//  x ... destination X coordiate
	//  y ... destination Y coordiate
	//  xs ... source X coordinate
	//  ys ... source Y coordinate
	//  w ... width
	//  h ... height
	//  alpha ... transparency 0..255: 0=transparent, 255=opaque
	void DrawImg(const u8* img, int x=0, int y=0, int xs=0, int ys=0, int w=0x20000, int h=0x20000, int alpha=255);

	// draw character using system fixed font (alpha 0=transparent, 255=opaque; use DrawSelFont() to select font)
	// - To invert text, use bit 7 of the characters, or FrameBuffer.inv flag (flags are XORed).
	void DrawCharScale(u8 ch, int x, int y, int scalex, int scaley, u32 col);
	void DrawChar(u8 ch, int x, int y, u32 col);
	void DrawCharW(u8 ch, int x, int y, u32 col);
	void DrawCharH(u8 ch, int x, int y, u32 col);
	void DrawChar2(u8 ch, int x, int y, u32 col);
	void DrawChar3(u8 ch, int x, int y, u32 col);
	void DrawChar4(u8 ch, int x, int y, u32 col);

	// draw character with background, using system fixed font (no alpha transparency; use SetFont() to select font)
	// - To invert text, use bit 7 of the characters, or this->printinv flag (flags are XORed).
	void DrawCharBgScale(u8 ch, int x, int y, int scalex, int scaley, u32 colfg, u32 colbg = 0);
	void DrawCharBg(u8 ch, int x, int y, u32 colfg, u32 colbg = 0);
	void DrawCharBgW(u8 ch, int x, int y, u32 colfg, u32 colbg = 0);
	void DrawCharBgH(u8 ch, int x, int y, u32 colfg, u32 colbg = 0);
	void DrawCharBg2(u8 ch, int x, int y, u32 colfg, u32 colbg = 0);
	void DrawCharBg3(u8 ch, int x, int y, u32 colfg, u32 colbg = 0);
	void DrawCharBg4(u8 ch, int x, int y, u32 colfg, u32 colbg = 0);

	// draw text with length, using system fixed font (alpha 0=transparent, 255=opaque; use DrawSelFont() to select font)
	// - To invert text, use bit 7 of the characters, or this->printinv flag (flags are XORed).
	// - Returns shift of X in pixels.
	int DrawTextLenScale(const char* text, int len, int x, int y, int scalex, int scaley, u32 col);
	int DrawTextLen(const char* text, int len, int x, int y, u32 col);
	int DrawTextLenW(const char* text, int len, int x, int y, u32 col);
	int DrawTextLenH(const char* text, int len, int x, int y, u32 col);
	int DrawTextLen2(const char* text, int len, int x, int y, u32 col);
	int DrawTextLen3(const char* text, int len, int x, int y, u32 col);
	int DrawTextLen4(const char* text, int len, int x, int y, u32 col);

	// draw ASCIIZ text (terminated with zero), using system fixed font (alpha 0=transparent, 255=opaque; use DrawSelFont() to select font)
	// - To invert text, use bit 7 of the characters, or this->printinv flag (flags are XORed).
	// - Returns shift of X in pixels.
	int DrawTextScale(const char* text, int x, int y, int scalex, int scaley, u32 col);
	int DrawText(const char* text, int x, int y, u32 col);
	int DrawTextW(const char* text, int x, int y, u32 col);
	int DrawTextW2(const char* text, int x, int y, u32 col);
	int DrawTextH(const char* text, int x, int y, u32 col);
	int DrawText2(const char* text, int x, int y, u32 col);
	int DrawText3(const char* text, int x, int y, u32 col);
	int DrawText4(const char* text, int x, int y, u32 col);

	// draw text with length and background, using system fixed font (no alpha transparency; use DrawSelFont() to select font)
	// - To invert text, use bit 7 of the characters, or FrameBuffer.inv flag (flags are XORed).
	// - Returns shift of X in pixels.
	int DrawTextBgLenScale(const char* text, int len, int x, int y, int scalex, int scaley, u32 colfg, u32 colbg = 0);
	int DrawTextBgLen(const char* text, int len, int x, int y, u32 colfg, u32 colbg = 0);
	int DrawTextBgLenW(const char* text, int len, int x, int y, u32 colfg, u32 colbg = 0);
	int DrawTextBgLenH(const char* text, int len, int x, int y, u32 colfg, u32 colbg = 0);
	int DrawTextBgLen2(const char* text, int len, int x, int y, u32 colfg, u32 colbg = 0);
	int DrawTextBgLen3(const char* text, int len, int x, int y, u32 colfg, u32 colbg = 0);
	int DrawTextBgLen4(const char* text, int len, int x, int y, u32 colfg, u32 colbg = 0);

	// draw ASCIIZ text with background (terminated with zero), using system fixed font (no alpha transparency; use DrawSelFont() to select font)
	// - To invert text, use bit 7 of the characters, or FrameBuffer.inv flag (flags are XORed).
	// - Returns shift of X in pixels.
	int DrawTextBgScale(const char* text, int x, int y, int scalex, int scaley, u32 colfg, u32 colbg = 0);
	int DrawTextBg(const char* text, int x, int y, u32 colfg, u32 colbg = 0);
	int DrawTextBgW(const char* text, int x, int y, u32 colfg, u32 colbg = 0);
	int DrawTextBgH(const char* text, int x, int y, u32 colfg, u32 colbg = 0);
	int DrawTextBg2(const char* text, int x, int y, u32 colfg, u32 colbg = 0);
	int DrawTextBg3(const char* text, int x, int y, u32 colfg, u32 colbg = 0);
	int DrawTextBg4(const char* text, int x, int y, u32 colfg, u32 colbg = 0);
};

#endif // _DRV_LCD_H

#endif // USE_LCD
