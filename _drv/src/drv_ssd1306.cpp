
// ****************************************************************************
//
//       Display driver with chip SSD1305-SSD1309 and I2C interface
//
// ****************************************************************************

#include "../../includes.h"	// includes

#if USE_SSD1306		// 1=use SSD1306 display driver, 0=not used (drv_ssd1306.*)

// ----------------------------------------------------------------------------
//                           Display base service
// ----------------------------------------------------------------------------

// initialize (returns False on error)
//  i2c ... I2C bus index (0 or 1)
//  addr ... I2C address (default SSD1306_ADDR=0x3C)
//  dispw ... display physical width in pixels (max. 128, or set 132 for SH1106 controller)
//  disph ... display physical height in pixels (max. 64)
//  rot ... image rotation SSD1306_ROT_*
//  speed ... transfer speed in Hz (usually 100000 to 400000, default I2C_DEF_SPEED=100000)
// On start of power - wait for at least 5ms to stabilize power supply of OLED.
// After Init() delay 2ms to guarantee initialization.
Bool cSSD1306::Init(int i2c, int addr, int dispw, int disph, int rot /* = SSD1306_ROT_0 */, int speed /* = I2C_DEF_SPEED */)
{
	// SH1106 controller
	this->sh1106 = False;
	if (dispw == 132)
	{
		this->sh1106 = True;
		dispw = 128;
	}

	// setup data
	this->i2c = i2c;	// I2C device index (0 or 1)
	this->addr = addr;	// I2C address (default 0x3C)
	this->speed = speed;	// I2C transfer speed
	this->dispw = dispw;	// display physical width in pixels
	this->disph = disph;	// display physical height in pixels
	this->rot = rot;	// image rotation SSD1306_ROT_*
	this->pagenum = (disph+7)/8; // number of pages (max. 8)

	// prepare frame buffer dimension
	int w = dispw;
	int h = disph;
	if ((rot == SSD1306_ROT_90) || (rot == SSD1306_ROT_270))
	{
		w = disph;
		h = dispw;
	}
	int wb = (w + 7)/8;	// width in bytes
	this->w = w;		// frame buffer width
	this->h = h;		// frame buffer height
	this->wb = wb;		// frame buffer width in bytes
	this->font = FontBold8x8; // draw font (cells 8 pixels, 128 characters)
	this->fontw = 8;	// font width (5 to 8 pixels)
	this->fonth = 8;	// font height

	// clear screen with black color
	this->DrawClear();

	// write init data
	u8* d = this->pagebuf;
	*d++ = 0x00;			// control byte to indicate commands
	*d++ = 0xAE;			// display off
	*d++ = 0xD5; *d++ = 0x80;	// set clock divide ratio
	*d++ = 0xA8; *d++ = disph-1;	// set number of multiplexing lines
	*d++ = 0xD3; *d++ = 0x00;	// set display offset to 0
	*d++ = 0x40;			// set starting line to 0
	*d++ = 0x8D; *d++ = 0x14;	// activate internal charge pump
	*d++ = 0xA1;			// invert segment mapping (orientation)
	*d++ = 0xC8;			// reverse the scanning direction of COM lines
	*d++ = 0xDA; *d++ = (disph <= 32) ? 0x02 : 0x12; // configure COM pins
	*d++ = 0x81; *d++ = 16;		// set screen contrast
	*d++ = 0xD9; *d++ = 0xF1;	// define the preload period
	*d++ = 0xDB; *d++ = 0x40;	// set VCOMH voltage
	*d++ = 0x20; *d++ = 0x00;	// set the memory mode to horizontal
	*d++ = 0xAF;			// display on
	return I2Cbus_Write(i2c, addr, this->pagebuf, d - this->pagebuf, NULL, 0, this->speed);
}

// set font
//  font ... draw font (cells 8 pixels, 128 characters)
//  fontw ... font width (5 to 8 pixels)
//  fonth ... font height
void cSSD1306::SetFont(const u8* font, int fontw, int fonth)
{
	this->font = font; 	// draw font (cells 8 pixels, 128 characters)
	this->fontw = fontw;	// font width (5 to 8 pixels)
	this->fonth = fonth;	// font height
}

// select SSD1306 page 0..7 (returns False on error)
Bool cSSD1306::SelectPage(int page)
{
	u8* d = this->pagebuf;
	*d++ = 0;			// control byte for command
	*d++ = 0xb0 | (page & 7);	// select page
	int c = (128 - this->dispw)/2;	// start column
	if (this->sh1106) c = 2;	// SH1106 column
	*d++ = c & 0x0f;		// set low column (or 2 with SH1106)
	*d = 0x10 | (c >> 4);		// set high column
	return I2Cbus_Write(this->i2c, this->addr, this->pagebuf, 4, NULL, 0, this->speed);
}

// display update after drawing (returns False on error)
Bool cSSD1306::Update()
{
	int x, y, m, b, wb, n;
	const u8 *s;
	u8 *d;
	wb = this->wb;

	switch (this->rot)
	{
	case SSD1306_ROT_270:
		for (y = 0; y < this->pagenum; y++)
		{
			// select page
			if (!this->SelectPage(y)) return False;

			// source address of this page
			s = &this->framebuf[y + (this->h-1)*wb];

			// prepare destination
			d = this->pagebuf;
			*d++ = 0x40;		// control byte to start transfer data

			// loop horizontal bytes
			for (x = 0; x < this->dispw; x++)
			{
				b = *s;
				*d++ = (u8)Reverse8(b);
				s -= wb;
			}

			// send this page
			if (!I2Cbus_Write(this->i2c, this->addr, this->pagebuf, d - this->pagebuf, NULL, 0, this->speed)) return False;
		}
		break;

	case SSD1306_ROT_180:
		for (y = 0; y < this->pagenum; y++)
		{
			// select page
			if (!this->SelectPage(y)) return False;

			// source address of this page
			s = &this->framebuf[(this->pagenum-1-y)*8*wb + wb];

			// prepare destination
			d = this->pagebuf;
			*d++ = 0x40;		// control byte to start transfer data

			// loop horizontal bytes
			n = this->dispw;
			for (x = 0; x < (this->dispw+7)/8; x++)
			{
				// loop mask of the bits
				s--;
				for (m = 1; (m != 0x100) && (n > 0); m <<= 1)
				{
					b = 0;
					if ((s[7*wb] & m) != 0) b |= B0;
					if ((s[6*wb] & m) != 0) b |= B1;
					if ((s[5*wb] & m) != 0) b |= B2;
					if ((s[4*wb] & m) != 0) b |= B3;
					if ((s[3*wb] & m) != 0) b |= B4;
					if ((s[2*wb] & m) != 0) b |= B5;
					if ((s[1*wb] & m) != 0) b |= B6;
					if ((s[0*wb] & m) != 0) b |= B7;
					*d++ = (u8)b;
					n--;
				}
			}

			// send this page
			if (!I2Cbus_Write(this->i2c, this->addr, this->pagebuf, d - this->pagebuf, NULL, 0, this->speed)) return False;
		}
		break;

	case SSD1306_ROT_90:
		for (y = 0; y < this->pagenum; y++)
		{
			// select page
			if (!this->SelectPage(y)) return False;

			// source address of this page
			s = &this->framebuf[this->pagenum-1-y];

			// prepare destination
			d = this->pagebuf;
			*d++ = 0x40;		// control byte to start transfer data

			// loop horizontal bytes
			for (x = 0; x < this->dispw; x++)
			{
				b = *s;
				//*d++ = (u8)Reverse8(b);
				*d++ = (u8)b;
				s += wb;
			}

			// send this page
			if (!I2Cbus_Write(this->i2c, this->addr, this->pagebuf, d - this->pagebuf, NULL, 0, this->speed)) return False;
		}
		break;

	// case SSD1306_ROT_0:
	default:
		for (y = 0; y < this->pagenum; y++)
		{
			// select page
			if (!this->SelectPage(y)) return False;

			// source address of this page
			s = &this->framebuf[y*8*wb];

			// prepare destination
			d = this->pagebuf;
			*d++ = 0x40;		// control byte to start transfer data

			// loop horizontal bytes
			n = this->dispw;
			for (x = 0; x < (this->dispw+7)/8; x++)
			{
				// loop mask of the bits
				for (m = 0x80; (m != 0) && (n > 0); m >>= 1)
				{
					b = 0;
					if ((s[0*wb] & m) != 0) b |= B0;
					if ((s[1*wb] & m) != 0) b |= B1;
					if ((s[2*wb] & m) != 0) b |= B2;
					if ((s[3*wb] & m) != 0) b |= B3;
					if ((s[4*wb] & m) != 0) b |= B4;
					if ((s[5*wb] & m) != 0) b |= B5;
					if ((s[6*wb] & m) != 0) b |= B6;
					if ((s[7*wb] & m) != 0) b |= B7;
					*d++ = (u8)b;
					n--;
				}
				s++;
			}

			// send this page
			if (!I2Cbus_Write(this->i2c, this->addr, this->pagebuf, d - this->pagebuf, NULL, 0, this->speed)) return False;
		}
		break;
	}

	return True;
}

// clear screen with black color
void cSSD1306::DrawClear()
{
	this->printinv = False;	// print invert
	memset(this->framebuf, 0, this->wb * this->h);
}

// get pixel (returns color cSSD1306::Col*)
int cSSD1306::GetPoint(int x, int y)
{
	if (((u32)x >= (u32)this->w) || ((u32)y >= (u32)this->h)) return 0;
	const u8* s = &this->framebuf[(x>>3) + y*this->wb];
	x = 7 - (x & 7);
	return (*s >> x) & 1;
}

// copy frame buffer to the main screen
void cSSD1306::CopyScreen(int x, int y)
{
	int xs, ys, w, h;
	w = this->w;
	h = this->h;
	for (ys = 0; ys < h; ys++)
	{
		for (xs = 0; xs < w; xs++)
		{
			::DrawPoint(x+xs, y+ys, (this->GetPoint(xs, ys) == 0) ? COL_BLACK : COL_WHITE);
		}
	}
}

// ----------------------------------------------------------------------------
//                               Draw point
// ----------------------------------------------------------------------------

// draw pixel fast - no check coordinates (col = color cSSD1306::Col*)
void cSSD1306::DrawPointFast(int x, int y, int col)
{
	u8* d = &this->framebuf[(x>>3) + y*this->wb];
	x = 7 - (x & 7);
	x = 1<<x;
	if (col == 0)
		*d &= ~x;
	else
		*d |= x;
}

// clear pixel fast - no check coordinates
void cSSD1306::DrawPointClrFast(int x, int y)
{
	u8* d = &this->framebuf[(x>>3) + y*this->wb];
	x = 7 - (x & 7);
	*d &= ~(1<<x);
}

// set pixel fast - no check coordinates
void cSSD1306::DrawPointSetFast(int x, int y)
{
	u8* d = &this->framebuf[(x>>3) + y*this->wb];
	x = 7 - (x & 7);
	*d |= 1<<x;
}

// invert pixel fast - no check coordinates
void cSSD1306::DrawPointInvFast(int x, int y)
{
	u8* d = &this->framebuf[(x>>3) + y*this->wb];
	x = 7 - (x & 7);
	*d ^= 1<<x;
}

// draw pixel (col = color cSSD1306::Col*)
void cSSD1306::DrawPoint(int x, int y, int col)
{
	if (((u32)x < (u32)this->w) && ((u32)y < (u32)this->h))
	{
		u8* d = &this->framebuf[(x>>3) + y*this->wb];
		x = 7 - (x & 7);
		x = 1<<x;
		if (col == 0)
			*d &= ~x;
		else
			*d |= x;
	}
}

// clear pixel
void cSSD1306::DrawPointClr(int x, int y)
{
	if (((u32)x < (u32)this->w) && ((u32)y < (u32)this->h))
	{
		u8* d = &this->framebuf[(x>>3) + y*this->wb];
		x = 7 - (x & 7);
		*d &= ~(1<<x);
	}
}

// set pixel
void cSSD1306::DrawPointSet(int x, int y)
{
	if (((u32)x < (u32)this->w) && ((u32)y < (u32)this->h))
	{
		u8* d = &this->framebuf[(x>>3) + y*this->wb];
		x = 7 - (x & 7);
		*d |= 1<<x;
	}
}

// invert pixel
void cSSD1306::DrawPointInv(int x, int y)
{
	if (((u32)x < (u32)this->w) && ((u32)y < (u32)this->h))
	{
		u8* d = &this->framebuf[(x>>3) + y*this->wb];
		x = 7 - (x & 7);
		*d ^= 1<<x;
	}
}

// ----------------------------------------------------------------------------
//                            Draw rectangle
// ----------------------------------------------------------------------------

// draw rectangle (col = color cSSD1306::Col*)
void cSSD1306::DrawRect(int x, int y, int w, int h, int col)
{
	// limit x
	if (x < 0) { w += x; x = 0; }

	// limit w
	if (x + w > this->w) w = this->w - x;
	if (w <= 0) return;

	// limit y
	if (y < 0) { h += y; y = 0; }

	// limit h
	if (y + h > this->h) h = this->h - y;
	if (h <= 0) return;

	// draw rectangle
	int x0 = x;
	int w2;
	if (col == 0)
	{
		for (; h > 0; h--)
		{
			x = x0;
			for (w2 = w; w2 > 0; w2--)
			{
				this->DrawPointClrFast(x, y);
				x++;
			}
			y++;
		}
	}
	else
	{
		for (; h > 0; h--)
		{
			x = x0;
			for (w2 = w; w2 > 0; w2--)
			{
				this->DrawPointSetFast(x, y);
				x++;
			}
			y++;
		}
	}
}

// clear rectangle
void cSSD1306::DrawRectClr(int x, int y, int w, int h)
{
	// limit x
	if (x < 0) { w += x; x = 0; }

	// limit w
	if (x + w > this->w) w = this->w - x;
	if (w <= 0) return;

	// limit y
	if (y < 0) { h += y; y = 0; }

	// limit h
	if (y + h > this->h) h = this->h - y;
	if (h <= 0) return;

	// draw rectangle
	int x0 = x;
	int w2;
	for (; h > 0; h--)
	{
		x = x0;
		for (w2 = w; w2 > 0; w2--)
		{
			this->DrawPointClrFast(x, y);
			x++;
		}
		y++;
	}
}

// set rectangle
void cSSD1306::DrawRectSet(int x, int y, int w, int h)
{
	// limit x
	if (x < 0) { w += x; x = 0; }

	// limit w
	if (x + w > this->w) w = this->w - x;
	if (w <= 0) return;

	// limit y
	if (y < 0) { h += y; y = 0; }

	// limit h
	if (y + h > this->h) h = this->h - y;
	if (h <= 0) return;

	// draw rectangle
	int x0 = x;
	int w2;
	for (; h > 0; h--)
	{
		x = x0;
		for (w2 = w; w2 > 0; w2--)
		{
			this->DrawPointSetFast(x, y);
			x++;
		}
		y++;
	}
}

// invert rectangle
void cSSD1306::DrawRectInv(int x, int y, int w, int h)
{
	// limit x
	if (x < 0) { w += x; x = 0; }

	// limit w
	if (x + w > this->w) w = this->w - x;
	if (w <= 0) return;

	// limit y
	if (y < 0) { h += y; y = 0; }

	// limit h
	if (y + h > this->h) h = this->h - y;
	if (h <= 0) return;

	// draw rectangle
	int x0 = x;
	int w2;
	for (; h > 0; h--)
	{
		x = x0;
		for (w2 = w; w2 > 0; w2--)
		{
			this->DrawPointInvFast(x, y);
			x++;
		}
		y++;
	}
}

// ----------------------------------------------------------------------------
//                 Draw horizontal line, vertical line, frame
// ----------------------------------------------------------------------------

// draw horizontal line (col = color cSSD1306::Col*)
void cSSD1306::DrawHLine(int x, int y, int w, int col) { this->DrawRect(x, y, w, 1, col); }

// clear horizontal line
void cSSD1306::DrawHLineClr(int x, int y, int w) { this->DrawRectClr(x, y, w, 1); }

// set horizontal line
void cSSD1306::DrawHLineSet(int x, int y, int w) { this->DrawRectSet(x, y, w, 1); }

// invert horizontal line
void cSSD1306::DrawHLineInv(int x, int y, int w) { this->DrawRectInv(x, y, w, 1); }

// draw vertical line (col = color cSSD1306::Col*)
void cSSD1306::DrawVLine(int x, int y, int h, int col) { this->DrawRect(x, y, 1, h, col); }

// clear vertical line
void cSSD1306::DrawVLineClr(int x, int y, int h) { this->DrawRectClr(x, y, 1, h); }

// set vertical line
void cSSD1306::DrawVLineSet(int x, int y, int h) { this->DrawRectSet(x, y, 1, h); }

// invert vertical line
void cSSD1306::DrawVLineInv(int x, int y, int h) { this->DrawRectInv(x, y, 1, h); }

// draw frame (col = color cSSD1306::Col*)
void cSSD1306::DrawFrame(int x, int y, int w, int h, int col)
{
	if (w <= 1)
	{
		this->DrawVLine(x, y, h, col);
	}
	else if (h <= 1)
	{
		this->DrawHLine(x, y, w, col);
	}
	else
	{
		this->DrawHLine(x, y, w, col);
		this->DrawHLine(x, y+h-1, w, col);
		this->DrawVLine(x, y+1, h-2, col);
		this->DrawVLine(x+w-1, y+1, h-2, col);
	}
}

// clear frame
void cSSD1306::DrawFrameClr(int x, int y, int w, int h)
{
	if (w <= 1)
	{
		this->DrawVLineClr(x, y, h);
	}
	else if (h <= 1)
	{
		this->DrawHLineClr(x, y, w);
	}
	else
	{
		this->DrawHLineClr(x, y, w);
		this->DrawHLineClr(x, y+h-1, w);
		this->DrawVLineClr(x, y+1, h-2);
		this->DrawVLineClr(x+w-1, y+1, h-2);
	}
}

// set frame
void cSSD1306::DrawFrameSet(int x, int y, int w, int h)
{
	if (w <= 1)
	{
		this->DrawVLineSet(x, y, h);
	}
	else if (h <= 1)
	{
		this->DrawHLineSet(x, y, w);
	}
	else
	{
		this->DrawHLineSet(x, y, w);
		this->DrawHLineSet(x, y+h-1, w);
		this->DrawVLineSet(x, y+1, h-2);
		this->DrawVLineSet(x+w-1, y+1, h-2);
	}
}

// invert frame
void cSSD1306::DrawFrameInv(int x, int y, int w, int h)
{
	if (w <= 1)
	{
		this->DrawVLineInv(x, y, h);
	}
	else if (h <= 1)
	{
		this->DrawHLineInv(x, y, w);
	}
	else
	{
		this->DrawHLineInv(x, y, w);
		this->DrawHLineInv(x, y+h-1, w);
		this->DrawVLineInv(x, y+1, h-2);
		this->DrawVLineInv(x+w-1, y+1, h-2);
	}
}

// ----------------------------------------------------------------------------
//                                  Draw line
// ----------------------------------------------------------------------------

// draw line (col = color cSSD1306::Col*)
void cSSD1306::DrawLine(int x1, int y1, int x2, int y2, int col)
{
	// difference of coordinates
	int dx = x2 - x1;
	int dy = y2 - y1;

	// increment X
	int sx = 1;
	if (dx < 0)
	{
		sx = -1;
		dx = -dx;
	}

	// increment Y
	int sy = 1;
	if (dy < 0)
	{
		sy = -1;
		dy = -dy;
	}

	// steeply in X direction, X is prefered as base
	if (dx > dy)
	{
		int m = 2*dy;
		int p = m - dx;
		dx = 2*dx;
		x2 += sx;
		for (; x1 != x2; x1 += sx)
		{
			this->DrawPoint(x1, y1, col);

			if (p > 0)
			{
				y1 += sy;
				p -= dx;
			}
			p += m;
		}
	}

	// steeply in Y direction, Y is prefered as base
	else
	{
		int m = 2*dx;
		int p = m - dy;
		dy = 2*dy;
		y2 += sy;
		for (; y1 != y2; y1 += sy)
		{
			this->DrawPoint(x1, y1, col);

			if (p > 0)
			{
				x1 += sx;
				p -= dy;
			}
			p += m;
		}
	}
}

// clear line
void cSSD1306::DrawLineClr(int x1, int y1, int x2, int y2)
{
	// difference of coordinates
	int dx = x2 - x1;
	int dy = y2 - y1;

	// increment X
	int sx = 1;
	if (dx < 0)
	{
		sx = -1;
		dx = -dx;
	}

	// increment Y
	int sy = 1;
	if (dy < 0)
	{
		sy = -1;
		dy = -dy;
	}

	// steeply in X direction, X is prefered as base
	if (dx > dy)
	{
		int m = 2*dy;
		int p = m - dx;
		dx = 2*dx;
		x2 += sx;
		for (; x1 != x2; x1 += sx)
		{
			this->DrawPointClr(x1, y1);

			if (p > 0)
			{
				y1 += sy;
				p -= dx;
			}
			p += m;
		}
	}

	// steeply in Y direction, Y is prefered as base
	else
	{
		int m = 2*dx;
		int p = m - dy;
		dy = 2*dy;
		y2 += sy;
		for (; y1 != y2; y1 += sy)
		{
			this->DrawPointClr(x1, y1);

			if (p > 0)
			{
				x1 += sx;
				p -= dy;
			}
			p += m;
		}
	}
}

// set line
void cSSD1306::DrawLineSet(int x1, int y1, int x2, int y2)
{
	// difference of coordinates
	int dx = x2 - x1;
	int dy = y2 - y1;

	// increment X
	int sx = 1;
	if (dx < 0)
	{
		sx = -1;
		dx = -dx;
	}

	// increment Y
	int sy = 1;
	if (dy < 0)
	{
		sy = -1;
		dy = -dy;
	}

	// steeply in X direction, X is prefered as base
	if (dx > dy)
	{
		int m = 2*dy;
		int p = m - dx;
		dx = 2*dx;
		x2 += sx;
		for (; x1 != x2; x1 += sx)
		{
			this->DrawPointSet(x1, y1);

			if (p > 0)
			{
				y1 += sy;
				p -= dx;
			}
			p += m;
		}
	}

	// steeply in Y direction, Y is prefered as base
	else
	{
		int m = 2*dx;
		int p = m - dy;
		dy = 2*dy;
		y2 += sy;
		for (; y1 != y2; y1 += sy)
		{
			this->DrawPointSet(x1, y1);

			if (p > 0)
			{
				x1 += sx;
				p -= dy;
			}
			p += m;
		}
	}
}

// invert line
void cSSD1306::DrawLineInv(int x1, int y1, int x2, int y2)
{
	// difference of coordinates
	int dx = x2 - x1;
	int dy = y2 - y1;

	// increment X
	int sx = 1;
	if (dx < 0)
	{
		sx = -1;
		dx = -dx;
	}

	// increment Y
	int sy = 1;
	if (dy < 0)
	{
		sy = -1;
		dy = -dy;
	}

	// steeply in X direction, X is prefered as base
	if (dx > dy)
	{
		int m = 2*dy;
		int p = m - dx;
		dx = 2*dx;
		x2 += sx;
		for (; x1 != x2; x1 += sx)
		{
			this->DrawPointInv(x1, y1);

			if (p > 0)
			{
				y1 += sy;
				p -= dx;
			}
			p += m;
		}
	}

	// steeply in Y direction, Y is prefered as base
	else
	{
		int m = 2*dx;
		int p = m - dy;
		dy = 2*dy;
		y2 += sy;
		for (; y1 != y2; y1 += sy)
		{
			this->DrawPointInv(x1, y1);

			if (p > 0)
			{
				x1 += sx;
				p -= dy;
			}
			p += m;
		}
	}
}

// ----------------------------------------------------------------------------
//                          Draw round (Filled circle)
// ----------------------------------------------------------------------------

// draw round (filled circle; col = color cSSD1306::Col*)
void cSSD1306::DrawRound(int x0, int y0, int r, int col)
{
	int x, y;
	if (r <= 0) return;
	int r2 = r*(r-1);
	r--;

	// full circle
	for (y = -r; y <= r; y++)
	{
		for (x = -r; x <= r; x++)
		{
			if ((x*x + y*y) <= r2) this->DrawPoint(x+x0, y+y0, col);
		}
	}
}

// clear round (filled circle)
void cSSD1306::DrawRoundClr(int x0, int y0, int r)
{
	int x, y;
	if (r <= 0) return;
	int r2 = r*(r-1);
	r--;

	// full circle
	for (y = -r; y <= r; y++)
	{
		for (x = -r; x <= r; x++)
		{
			if ((x*x + y*y) <= r2) this->DrawPointClr(x+x0, y+y0);
		}
	}
}

// set round (filled circle)
void cSSD1306::DrawRoundSet(int x0, int y0, int r)
{
	int x, y;
	if (r <= 0) return;
	int r2 = r*(r-1);
	r--;

	// full circle
	for (y = -r; y <= r; y++)
	{
		for (x = -r; x <= r; x++)
		{
			if ((x*x + y*y) <= r2) this->DrawPointSet(x+x0, y+y0);
		}
	}
}

// invert round (filled circle)
void cSSD1306::DrawRoundInv(int x0, int y0, int r)
{
	int x, y;
	if (r <= 0) return;
	int r2 = r*(r-1);
	r--;

	// full circle
	for (y = -r; y <= r; y++)
	{
		for (x = -r; x <= r; x++)
		{
			if ((x*x + y*y) <= r2) this->DrawPointInv(x+x0, y+y0);
		}
	}
}

// ----------------------------------------------------------------------------
//                               Draw circle
// ----------------------------------------------------------------------------

// draw circle (col = color cSSD1306::Col*)
void cSSD1306::DrawCircle(int x0, int y0, int r, int col)
{
	int x, y;
	if (r <= 0) return;
	r--;

	x = 0;
	y = r;
	int p = 1 - r;

	while (x <= y)
	{
		this->DrawPoint(x0+y, y0-x, col);
		this->DrawPoint(x0+x, y0-y, col);
		this->DrawPoint(x0-x, y0-y, col);
		this->DrawPoint(x0-y, y0-x, col);
		this->DrawPoint(x0-y, y0+x, col);
		this->DrawPoint(x0-x, y0+y, col);
		this->DrawPoint(x0+x, y0+y, col);
		this->DrawPoint(x0+y, y0+x, col);

		x++;
		if (p > 0)
		{
			y--;
			p -= 2*y;
		}
		p += 2*x + 1;
	}
}

// clear circle
void cSSD1306::DrawCircleClr(int x0, int y0, int r)
{
	int x, y;
	if (r <= 0) return;
	r--;

	x = 0;
	y = r;
	int p = 1 - r;

	while (x <= y)
	{
		this->DrawPointClr(x0+y, y0-x);
		this->DrawPointClr(x0+x, y0-y);
		this->DrawPointClr(x0-x, y0-y);
		this->DrawPointClr(x0-y, y0-x);
		this->DrawPointClr(x0-y, y0+x);
		this->DrawPointClr(x0-x, y0+y);
		this->DrawPointClr(x0+x, y0+y);
		this->DrawPointClr(x0+y, y0+x);

		x++;
		if (p > 0)
		{
			y--;
			p -= 2*y;
		}
		p += 2*x + 1;
	}
}

// set circle
void cSSD1306::DrawCircleSet(int x0, int y0, int r)
{
	int x, y;
	if (r <= 0) return;
	r--;

	x = 0;
	y = r;
	int p = 1 - r;

	while (x <= y)
	{
		this->DrawPointSet(x0+y, y0-x);
		this->DrawPointSet(x0+x, y0-y);
		this->DrawPointSet(x0-x, y0-y);
		this->DrawPointSet(x0-y, y0-x);
		this->DrawPointSet(x0-y, y0+x);
		this->DrawPointSet(x0-x, y0+y);
		this->DrawPointSet(x0+x, y0+y);
		this->DrawPointSet(x0+y, y0+x);

		x++;
		if (p > 0)
		{
			y--;
			p -= 2*y;
		}
		p += 2*x + 1;
	}
}

// invert circle
void cSSD1306::DrawCircleInv(int x0, int y0, int r)
{
	int x, y;
	if (r <= 0) return;
	r--;

	x = 0;
	y = r;
	int p = 1 - r;

	while (x <= y)
	{
		this->DrawPointInv(x0+y, y0-x);
		this->DrawPointInv(x0+x, y0-y);
		this->DrawPointInv(x0-x, y0-y);
		this->DrawPointInv(x0-y, y0-x);
		this->DrawPointInv(x0-y, y0+x);
		this->DrawPointInv(x0-x, y0+y);
		this->DrawPointInv(x0+x, y0+y);
		this->DrawPointInv(x0+y, y0+x);

		x++;
		if (p > 0)
		{
			y--;
			p -= 2*y;
		}
		p += 2*x + 1;
	}
}

// ----------------------------------------------------------------------------
//                               Text
// ----------------------------------------------------------------------------
// Character can be inverted with bit 7 or with this->printinv flag.

// Clear character sized (no background)
void cSSD1306::DrawCharClrScale(char ch, int x, int y, int scalex, int scaley)
{
	int sx, sy, b, inv, x2, i, w, h, ww, hh;

	// prepare inversion mask
	inv = this->printinv ? (~0) : 0;
	if ((u8)ch >= 0x80)
	{
		ch &= 0x7f;
		inv = ~inv;
	}

	// pointer to the font
	const u8* s = &this->font[(u8)ch];

	// font size
	w = this->fontw;
	h = this->fonth;

	// screen size
	hh = this->h;
	ww = this->w;

	// loop through font height
	for (; h > 0; h--)
	{
		// loop through scale Y
		for (sy = scaley; sy > 0; sy--)
		{
			// check if Y is valid
			if ((u32)y < (u32)hh)
			{
				// load font sample
				b = *s ^ inv;

				// loop through font width
				x2 = x;
				for (i = w; i > 0; i--)
				{
					// check if draw this pixel
					if ((b & B7) == 0)
					{
						// loop through scale X
						for (sx = scalex; sx > 0; sx--)
						{
							// check if X is valid
							if ((u32)x2 < (u32)ww)
							{
								this->DrawPointClrFast(x2, y);
							}

							// shift X
							x2++;
						}
					}
					else
					{
						// skip this pixel
						x2 += scalex;
					}

					// shift font sample
					b <<= 1;
				}
			}

			// shift Y
			y++;
		}

		// shift source to next line
		s += 128;
	}
}

// Set character sized (no background)
void cSSD1306::DrawCharSetScale(char ch, int x, int y, int scalex, int scaley)
{
	int sx, sy, b, inv, x2, i, w, h, ww, hh;

	// prepare inversion mask
	inv = this->printinv ? (~0) : 0;
	if ((u8)ch >= 0x80)
	{
		ch &= 0x7f;
		inv = ~inv;
	}

	// pointer to the font
	const u8* s = &this->font[(u8)ch];

	// font size
	w = this->fontw;
	h = this->fonth;

	// screen size
	hh = this->h;
	ww = this->w;

	// loop through font height
	for (; h > 0; h--)
	{
		// loop through scale Y
		for (sy = scaley; sy > 0; sy--)
		{
			// check if Y is valid
			if ((u32)y < (u32)hh)
			{
				// load font sample
				b = *s ^ inv;

				// loop through font width
				x2 = x;
				for (i = w; i > 0; i--)
				{
					// check if draw this pixel
					if ((b & B7) == 0)
					{
						// loop through scale X
						for (sx = scalex; sx > 0; sx--)
						{
							// check if X is valid
							if ((u32)x2 < (u32)ww)
							{
								this->DrawPointSetFast(x2, y);
							}

							// shift X
							x2++;
						}
					}
					else
					{
						// skip this pixel
						x2 += scalex;
					}

					// shift font sample
					b <<= 1;
				}
			}

			// shift Y
			y++;
		}

		// shift source to next line
		s += 128;
	}
}

// draw character sized (no background; col = color cSSD1306::Col*)
void cSSD1306::DrawCharScale(char ch, int x, int y, int scalex, int scaley, int col)
{
	if (col == 0)
		this->DrawCharClrScale(ch, x, y, scalex, scaley);
	else
		this->DrawCharSetScale(ch, x, y, scalex, scaley);
}

// invert character sized (no background)
void cSSD1306::DrawCharInvScale(char ch, int x, int y, int scalex, int scaley)
{
	int sx, sy, b, inv, x2, i, w, h, ww, hh;

	// prepare inversion mask
	inv = this->printinv ? (~0) : 0;
	if ((u8)ch >= 0x80)
	{
		ch &= 0x7f;
		inv = ~inv;
	}

	// pointer to the font
	const u8* s = &this->font[(u8)ch];

	// font size
	w = this->fontw;
	h = this->fonth;

	// screen size
	hh = this->h;
	ww = this->w;

	// loop through font height
	for (; h > 0; h--)
	{
		// loop through scale Y
		for (sy = scaley; sy > 0; sy--)
		{
			// check if Y is valid
			if ((u32)y < (u32)hh)
			{
				// load font sample
				b = *s ^ inv;

				// loop through font width
				x2 = x;
				for (i = w; i > 0; i--)
				{
					// check if draw this pixel
					if ((b & B7) == 0)
					{
						// loop through scale X
						for (sx = scalex; sx > 0; sx--)
						{
							// check if X is valid
							if ((u32)x2 < (u32)ww)
							{
								this->DrawPointInvFast(x2, y);
							}

							// shift X
							x2++;
						}
					}
					else
					{
						// skip this pixel
						x2 += scalex;
					}

					// shift font sample
					b <<= 1;
				}
			}

			// shift Y
			y++;
		}

		// shift source to next line
		s += 128;
	}
}

void cSSD1306::DrawChar(char ch, int x, int y, int col) { this->DrawCharScale(ch, x, y, 1, 1, col); }
void cSSD1306::DrawCharW(char ch, int x, int y, int col) { this->DrawCharScale(ch, x, y, 2, 1, col); }
void cSSD1306::DrawCharH(char ch, int x, int y, int col) { this->DrawCharScale(ch, x, y, 1, 2, col); }
void cSSD1306::DrawChar2(char ch, int x, int y, int col) { this->DrawCharScale(ch, x, y, 2, 2, col); }
void cSSD1306::DrawChar3(char ch, int x, int y, int col) { this->DrawCharScale(ch, x, y, 3, 3, col); }
void cSSD1306::DrawChar4(char ch, int x, int y, int col) { this->DrawCharScale(ch, x, y, 4, 4, col); }

// draw character sized with background (white text, black background)
void cSSD1306::DrawCharBgScale(char ch, int x, int y, int scalex, int scaley)
{
	int sx, sy, b, inv, x2, i, w, h, ww, hh;

	// prepare inversion mask
	inv = this->printinv ? (~0) : 0;
	if ((u8)ch >= 0x80)
	{
		ch &= 0x7f;
		inv = ~inv;
	}

	// pointer to the font
	const u8* s = &this->font[(u8)ch];

	// font size
	w = this->fontw;
	h = this->fonth;

	// screen size
	hh = this->h;
	ww = this->w;

	// loop through font height
	for (; h > 0; h--)
	{
		// loop through scale Y
		for (sy = scaley; sy > 0; sy--)
		{
			// check if Y is valid
			if ((u32)y < (u32)hh)
			{
				// load font sample
				b = *s ^ inv;

				// loop through font width
				x2 = x;
				for (i = w; i > 0; i--)
				{
					// check if draw this pixel
					if ((b & B7) == 0)
					{
						// loop through scale X
						for (sx = scalex; sx > 0; sx--)
						{
							// check if X is valid
							if ((u32)x2 < (u32)ww)
							{
								this->DrawPointSetFast(x2, y);
							}

							// shift X
							x2++;
						}
					}
					else
					{
						// loop through scale X
						for (sx = scalex; sx > 0; sx--)
						{
							// check if X is valid
							if ((u32)x2 < (u32)ww)
							{
								this->DrawPointClrFast(x2, y);
							}

							// shift X
							x2++;
						}
					}

					// shift font sample
					b <<= 1;
				}
			}

			// shift Y
			y++;
		}

		// shift source to next line
		s += 128;
	}
}

void cSSD1306::DrawCharBg(char ch, int x, int y) { this->DrawCharBgScale(ch, x, y, 1, 1); }
void cSSD1306::DrawCharBgW(char ch, int x, int y) { this->DrawCharBgScale(ch, x, y, 2, 1); }
void cSSD1306::DrawCharBgH(char ch, int x, int y) { this->DrawCharBgScale(ch, x, y, 1, 2); }
void cSSD1306::DrawCharBg2(char ch, int x, int y) { this->DrawCharBgScale(ch, x, y, 2, 2); }
void cSSD1306::DrawCharBg3(char ch, int x, int y) { this->DrawCharBgScale(ch, x, y, 3, 3); }
void cSSD1306::DrawCharBg4(char ch, int x, int y) { this->DrawCharBgScale(ch, x, y, 4, 4); }

// draw sized text with length (no background; returns shift of X in pixels)
int cSSD1306::DrawTextLenScale(const char* text, int len, int x, int y, int scalex, int scaley, int col)
{
	int dx = this->fontw*scalex; // increment X
	int res = 0;
	for (; len > 0; len--)
	{
		this->DrawCharScale(*text++, x, y, scalex, scaley, col);
		x += dx;
		res += dx;
	}
	return res;
}

int cSSD1306::DrawTextLen(const char* text, int len, int x, int y, int col) { return this->DrawTextLenScale(text, len, x, y, 1, 1, col); }
int cSSD1306::DrawTextLenW(const char* text, int len, int x, int y, int col) { return this->DrawTextLenScale(text, len, x, y, 2, 1, col); }
int cSSD1306::DrawTextLenH(const char* text, int len, int x, int y, int col) { return this->DrawTextLenScale(text, len, x, y, 1, 2, col); }
int cSSD1306::DrawTextLen2(const char* text, int len, int x, int y, int col) { return this->DrawTextLenScale(text, len, x, y, 2, 2, col); }
int cSSD1306::DrawTextLen3(const char* text, int len, int x, int y, int col) { return this->DrawTextLenScale(text, len, x, y, 3, 3, col); }
int cSSD1306::DrawTextLen4(const char* text, int len, int x, int y, int col) { return this->DrawTextLenScale(text, len, x, y, 4, 4, col); }

// draw ASCIIZ text (no background; returns shift of X in pixels)
int cSSD1306::DrawTextScale(const char* text, int x, int y, int scalex, int scaley, int col)
{
	return this->DrawTextLenScale(text, StrLen(text), x, y, scalex, scaley, col);
}

int cSSD1306::DrawText(const char* text, int x, int y, int col) { return this->DrawTextScale(text, x, y, 1, 1, col); }
int cSSD1306::DrawTextW(const char* text, int x, int y, int col) { return this->DrawTextScale(text, x, y, 2, 1, col); }
int cSSD1306::DrawTextW2(const char* text, int x, int y, int col) { return this->DrawTextScale(text, x, y, 4, 2, col); }
int cSSD1306::DrawTextH(const char* text, int x, int y, int col) { return this->DrawTextScale(text, x, y, 1, 2, col); }
int cSSD1306::DrawText2(const char* text, int x, int y, int col) { return this->DrawTextScale(text, x, y, 2, 2, col); }
int cSSD1306::DrawText3(const char* text, int x, int y, int col) { return this->DrawTextScale(text, x, y, 3, 3, col); }
int cSSD1306::DrawText4(const char* text, int x, int y, int col) { return this->DrawTextScale(text, x, y, 4, 4, col); }

// draw sized text with length with background (white text, black background; returns shift of X in pixels)
int cSSD1306::DrawTextBgLenScale(const char* text, int len, int x, int y, int scalex, int scaley)
{
	int dx = this->fontw*scalex; // increment X
	int res = 0;
	for (; len > 0; len--)
	{
		this->DrawCharBgScale(*text++, x, y, scalex, scaley);
		x += dx;
		res += dx;
	}
	return res;
}

int cSSD1306::DrawTextBgLen(const char* text, int len, int x, int y) { return this->DrawTextBgLenScale(text, len, x, y, 1, 1); }
int cSSD1306::DrawTextBgLenW(const char* text, int len, int x, int y) { return this->DrawTextBgLenScale(text, len, x, y, 2, 1); }
int cSSD1306::DrawTextBgLenH(const char* text, int len, int x, int y) { return this->DrawTextBgLenScale(text, len, x, y, 1, 2); }
int cSSD1306::DrawTextBgLen2(const char* text, int len, int x, int y) { return this->DrawTextBgLenScale(text, len, x, y, 2, 2); }
int cSSD1306::DrawTextBgLen3(const char* text, int len, int x, int y) { return this->DrawTextBgLenScale(text, len, x, y, 3, 3); }
int cSSD1306::DrawTextBgLen4(const char* text, int len, int x, int y) { return this->DrawTextBgLenScale(text, len, x, y, 4, 4); }

// draw ASCIIZ text with background (white text, black background; returns shift of X in pixels)
int cSSD1306::DrawTextBgScale(const char* text, int x, int y, int scalex, int scaley)
{
	return this->DrawTextBgLenScale(text, StrLen(text), x, y, scalex, scaley);
}

int cSSD1306::DrawTextBg(const char* text, int x, int y) { return this->DrawTextBgScale(text, x, y, 1, 1); }
int cSSD1306::DrawTextBgW(const char* text, int x, int y) { return this->DrawTextBgScale(text, x, y, 2, 1); }
int cSSD1306::DrawTextBgW2(const char* text, int x, int y) { return this->DrawTextBgScale(text, x, y, 4, 2); }
int cSSD1306::DrawTextBgH(const char* text, int x, int y) { return this->DrawTextBgScale(text, x, y, 1, 2); }
int cSSD1306::DrawTextBg2(const char* text, int x, int y) { return this->DrawTextBgScale(text, x, y, 2, 2); }
int cSSD1306::DrawTextBg3(const char* text, int x, int y) { return this->DrawTextBgScale(text, x, y, 3, 3); }
int cSSD1306::DrawTextBg4(const char* text, int x, int y) { return this->DrawTextBgScale(text, x, y, 4, 4); }

// ----------------------------------------------------------------------------
//                               Draw image
// ----------------------------------------------------------------------------

// draw mono image, transparent background
//  img ... image in format sPic, must be in CF_A1 format
//  x ... destination X coordiate
//  y ... destination Y coordiate
//  col ... color cSSD1306::Col*
void cSSD1306::DrawImg(const u8* img, int x, int y, int col)
{
	int xd, ys, xs, w, h, wsb, m, b;
	const u8* s;

	// source image
	const sPic* pic = (const sPic*)img;
	if (pic->colfmt != CF_A1) return;

	// get size of source image
	w = pic->w;
	h = pic->h;
	wsb = pic->wb;

	// lines
	for (ys = 0; ys < h; ys++)
	{
		s = &pic->data[ys*wsb];
		xd = x;
		m = B7;
		b = *s++;
		for (xs = 0; xs < w; xs++)
		{
			if ((b & m) != 0) this->DrawPoint(xd, y, col);
			m >>= 1;
			if (m == 0)
			{
				m = B7;
				b = *s++;
			}
			xd++;
		}
		y++;
	}
}

// draw mono image with background
//  img ... image in format sPic, must be in CF_A1 format
//  x ... destination X coordiate
//  y ... destination Y coordiate
void cSSD1306::DrawImgBg(const u8* img, int x, int y)
{
	int xd, ys, xs, w, h, wsb, m, b;
	const u8* s;

	// source image
	const sPic* pic = (const sPic*)img;
	if (pic->colfmt != CF_A1) return;

	// get size of source image
	w = pic->w;
	h = pic->h;
	wsb = pic->wb;

	// lines
	for (ys = 0; ys < h; ys++)
	{
		s = &pic->data[ys*wsb];
		xd = x;
		m = B7;
		b = *s++;
		for (xs = 0; xs < w; xs++)
		{
			if ((b & m) != 0)
				this->DrawPointSet(xd, y);
			else
				this->DrawPointClr(xd, y);
			m >>= 1;
			if (m == 0)
			{
				m = B7;
				b = *s++;
			}
			xd++;
		}
		y++;
	}
}

#endif // USE_SSD1306
