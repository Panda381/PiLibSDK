
// ****************************************************************************
//
//       Display driver with chip SSD1305-SSD1309 and I2C interface
//
// ****************************************************************************

#if USE_SSD1306		// 1=use SSD1306 display driver, 0=not used (drv_ssd1306.*)

#ifndef _DRV_SSD1306_H
#define _DRV_SSD1306_H

#ifndef SSD1306_ADDR
#define SSD1306_ADDR	0x3c		// default I2C address
#endif

#define SSD1306_WMAX		128	// display max. width
#define SSD1306_HMAX		64	// display max. height
#define SSD1306_FRAMEMAXSIZE	(SSD1306_WMAX*SSD1306_HMAX/8) // frame nax. size
#define SSD1306_PAGESIZE	SSD1306_WMAX // page size in bytes
#define SSD1306_PAGENUM		8	// number of pages

// display rotation
enum {
	SSD1306_ROT_0 = 0,
	SSD1306_ROT_90,
	SSD1306_ROT_180,
	SSD1306_ROT_270,
};

// SSD1306 device
class cSSD1306
{
public:
	int 	i2c;		// I2C bus index (0 or 1)
	int	addr;		// I2C address (default 0x3C)
	int	speed;		// transfer speed in Hz (usually 100000 to 400000)
	int	dispw;		// display physical width in pixels (max. 128)
	int	disph;		// display physical height in pixels (max. 64)
	int	pagenum;	// number of pages (max. 8)
	int	rot;		// image rotation SSD1306_ROT_*
	int	w;		// frame buffer width
	int	h;		// frame buffer height
	int	wb;		// frame buffer width in bytes
	const u8* font;		// draw font (cells 8 pixels, 128 characters)
	int	fontw;		// font width (5 to 8 pixels)
	int	fonth;		// font height
	Bool	printinv;	// print invert
	Bool	sh1106;		// SH1106 controller, instead of SSD1306

	// colors
	static const int ColBlack = 0;	// black color
	static const int ColWhite = 1;	// white color

	// frame buffer
	u8	framebuf[SSD1306_FRAMEMAXSIZE];

	// page buffer (+ control byte)
	u8	pagebuf[SSD1306_PAGESIZE + 1];

	// initialize (returns False on error)
	//  i2c ... I2C bus index (0 or 1)
	//  addr ... I2C address (default SSD1306_ADDR=0x3C)
	//  dispw ... display physical width in pixels (max. 128, or set 132 for SH1106 controller)
	//  disph ... display physical height in pixels (max. 64)
	//  rot ... image rotation SSD1306_ROT_*
	//  speed ... transfer speed in Hz (usually 100000 to 400000, default I2C_DEF_SPEED=100000)
	// On start of power - wait for at least 5ms to stabilize power supply of OLED.
	// After Init() delay 2ms to guarantee initialization.
	Bool Init(int i2c, int addr, int dispw, int disph, int rot = SSD1306_ROT_0, int speed = I2C_DEF_SPEED);

	// set font
	//  font ... draw font (cells 8 pixels, 128 characters)
	//  fontw ... font width (5 to 8 pixels)
	//  fonth ... font height
	void SetFont(const u8* font, int fontw, int fonth);

	// select SSD1306 page 0..7 (returns False on error)
	Bool SelectPage(int page);

	// display update after drawing (returns False on error)
	Bool Update();

	// clear screen with black color
	void DrawClear();

	// copy frame buffer to the main screen
	void CopyScreen(int x, int y);

	// get pixel (returns color SSD1306::Col*)
	int GetPoint(int x, int y);

	// draw/clear/set/invert pixel fast - no check coordinates (col = color cSSD1306::Col*)
	void DrawPointFast(int x, int y, int col);
	void DrawPointClrFast(int x, int y);
	void DrawPointSetFast(int x, int y);
	void DrawPointInvFast(int x, int y);

	// draw/clear/set/invert pixel (col = color cSSD1306::Col*)
	void DrawPoint(int x, int y, int col);
	void DrawPointClr(int x, int y);
	void DrawPointSet(int x, int y);
	void DrawPointInv(int x, int y);

	// draw/clear/set/invert rectangle (col = color cSSD1306::Col*)
	void DrawRect(int x, int y, int w, int h, int col);
	void DrawRectClr(int x, int y, int w, int h);
	void DrawRectSet(int x, int y, int w, int h);
	void DrawRectInv(int x, int y, int w, int h);

	// draw/clear/set/invert horizontal line (col = color cSSD1306::Col*)
	void DrawHLine(int x, int y, int w, int col);
	void DrawHLineClr(int x, int y, int w);
	void DrawHLineSet(int x, int y, int w);
	void DrawHLineInv(int x, int y, int w);

	// draw/clear/set/invert vertical line (col = color cSSD1306::Col*)
	void DrawVLine(int x, int y, int h, int col);
	void DrawVLineClr(int x, int y, int h);
	void DrawVLineSet(int x, int y, int h);
	void DrawVLineInv(int x, int y, int h);

	// draw/clear/set/invert frame (col = color cSSD1306::Col*)
	void DrawFrame(int x, int y, int w, int h, int col);
	void DrawFrameClr(int x, int y, int w, int h);
	void DrawFrameSet(int x, int y, int w, int h);
	void DrawFrameInv(int x, int y, int w, int h);

	// draw/clear/set/invert line (col = color cSSD1306::Col*)
	void DrawLine(int x1, int y1, int x2, int y2, int col);
	void DrawLineClr(int x1, int y1, int x2, int y2);
	void DrawLineSet(int x1, int y1, int x2, int y2);
	void DrawLineInv(int x1, int y1, int x2, int y2);

	// draw/clear/set/invert round (filled circle; col = color cSSD1306::Col*)
	void DrawRound(int x0, int y0, int r, int col);
	void DrawRoundClr(int x0, int y0, int r);
	void DrawRoundSet(int x0, int y0, int r);
	void DrawRoundInv(int x0, int y0, int r);

	// draw/clear/set/invert circle (col = color cSSD1306::Col*)
	void DrawCircle(int x0, int y0, int r, int col);
	void DrawCircleClr(int x0, int y0, int r);
	void DrawCircleSet(int x0, int y0, int r);
	void DrawCircleInv(int x0, int y0, int r);

	// draw/clear/set/invert character sized (no background; col = color cSSD1306::Col*)
	// Character can be inverted with bit 7 or with this->printinv flag.
	void DrawCharScale(char ch, int x, int y, int scalex, int scaley, int col);
	void DrawCharClrScale(char ch, int x, int y, int scalex, int scaley);
	void DrawCharSetScale(char ch, int x, int y, int scalex, int scaley);
	void DrawCharInvScale(char ch, int x, int y, int scalex, int scaley);
	void DrawChar(char ch, int x, int y, int col);
	void DrawCharW(char ch, int x, int y, int col);
	void DrawCharH(char ch, int x, int y, int col);
	void DrawChar2(char ch, int x, int y, int col);
	void DrawChar3(char ch, int x, int y, int col);
	void DrawChar4(char ch, int x, int y, int col);

	// draw character sized with background (white text, black background)
	// Character can be inverted with bit 7 or with this->printinv flag.
	void DrawCharBgScale(char ch, int x, int y, int scalex, int scaley);
	void DrawCharBg(char ch, int x, int y);
	void DrawCharBgW(char ch, int x, int y);
	void DrawCharBgH(char ch, int x, int y);
	void DrawCharBg2(char ch, int x, int y);
	void DrawCharBg3(char ch, int x, int y);
	void DrawCharBg4(char ch, int x, int y);

	// draw sized text with length (no background; returns shift of X in pixels)
	// Characters can be inverted with bit 7 or with this->printinv flag.
	int DrawTextLenScale(const char* text, int len, int x, int y, int scalex, int scaley, int col);
	int DrawTextLen(const char* text, int len, int x, int y, int col);
	int DrawTextLenW(const char* text, int len, int x, int y, int col);
	int DrawTextLenH(const char* text, int len, int x, int y, int col);
	int DrawTextLen2(const char* text, int len, int x, int y, int col);
	int DrawTextLen3(const char* text, int len, int x, int y, int col);
	int DrawTextLen4(const char* text, int len, int x, int y, int col);

	// draw ASCIIZ text (no background; returns shift of X in pixels)
	// Characters can be inverted with bit 7 or with this->printinv flag.
	int DrawTextScale(const char* text, int x, int y, int scalex, int scaley, int col);
	int DrawText(const char* text, int x, int y, int col);
	int DrawTextW(const char* text, int x, int y, int col);
	int DrawTextW2(const char* text, int x, int y, int col);
	int DrawTextH(const char* text, int x, int y, int col);
	int DrawText2(const char* text, int x, int y, int col);
	int DrawText3(const char* text, int x, int y, int col);
	int DrawText4(const char* text, int x, int y, int col);

	// draw sized text with length with background (white text, black background; returns shift of X in pixels)
	// Characters can be inverted with bit 7 or with this->printinv flag.
	int DrawTextBgLenScale(const char* text, int len, int x, int y, int scalex, int scaley);
	int DrawTextBgLen(const char* text, int len, int x, int y);
	int DrawTextBgLenW(const char* text, int len, int x, int y);
	int DrawTextBgLenH(const char* text, int len, int x, int y);
	int DrawTextBgLen2(const char* text, int len, int x, int y);
	int DrawTextBgLen3(const char* text, int len, int x, int y);
	int DrawTextBgLen4(const char* text, int len, int x, int y);

	// draw ASCIIZ text with background (white text, black background; returns shift of X in pixels)
	// Characters can be inverted with bit 7 or with this->printinv flag.
	int DrawTextBgScale(const char* text, int x, int y, int scalex, int scaley);
	int DrawTextBg(const char* text, int x, int y);
	int DrawTextBgW(const char* text, int x, int y);
	int DrawTextBgW2(const char* text, int x, int y);
	int DrawTextBgH(const char* text, int x, int y);
	int DrawTextBg2(const char* text, int x, int y);
	int DrawTextBg3(const char* text, int x, int y);
	int DrawTextBg4(const char* text, int x, int y);

	// draw mono image, transparent background
	//  img ... image in format sPic, must be in CF_A1 format
	//  x ... destination X coordiate
	//  y ... destination Y coordiate
	//  col ... color cSSD1306::Col*
	void DrawImg(const u8* img, int x, int y, int col);

	// draw mono image with background
	//  img ... image in format sPic, must be in CF_A1 format
	//  x ... destination X coordiate
	//  y ... destination Y coordiate
	void DrawImgBg(const u8* img, int x, int y);
};

#endif // _DRV_SSD1306_H

#endif // USE_SSD1306
