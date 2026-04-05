
// ****************************************************************************
//
//                             Drawing library
//
// ****************************************************************************

#include "../../includes.h"	// includes

#if USE_DRAW		// 1=use Drawing library, 0=not used (lib_draw.*)

// macro - prepare components for blending
#define DRAW_BLEND_PREP()				\
	u32 inv = 255 - a;				\
	u32 srb = (col & 0x00ff00ff)*a + 0x00ff00ff;	\
	u32 sg = (col & 0x0000ff00)*a + 0x0000ff00

// macro - blend one pixel
#define DRAW_BLEND_PIXEL()	{			\
	u32 dst = *d;					\
	u32 rb = (dst & 0x00ff00ff);			\
	u32 g = (dst & 0x0000ff00);			\
	rb = (srb + rb*inv) & 0xff00ff00;		\
	g = (sg + g*inv) & 0x00ff0000;			\
	*d = ((rb | g) >> 8) | 0xff000000; }

// invert value
#define DRAW_INV_VAL	0x00ffffff

// divide by 255
#define DIV255(nn) (((nn)*32897) >> 23)

// Blend two colors
//  alpha ... 255=full col1, 0=full col2 
u32 BlendCol(u32 col1, u32 col2, int alpha)
{
	if (alpha <= 0) return col2;
	if (alpha >= 255) return col1;
	u32 m1 = 0x00ff00ff;
	u32 rb1 = (col1 & m1)*alpha + m1;
	u32 m2 = 0x0000ff00;
	u32 g1 = (col1 & m2)*alpha + m2;
	u32 inv = 255 - alpha;
	u32 rb2 = (col2 & m1)*inv;
	u32 g2 = (col2 & m2)*inv;
	rb2 = ((rb1 + rb2) >> 8) & m1;
	g2 = ((g1 + g2) >> 8) & m2;
	return rb2 | g2 | (m2 << 16);
}

// ----------------------------------------------------------------------------
//                            Clear screen
// ----------------------------------------------------------------------------

// Draw clear screen with given foreground and background color, set the colors as new printing colors
void DrawClearCol(u32 colfg, u32 colbg)
{
	sFrameBuffer* f = &FrameBuffer;
	int minx = f->minx;
	int maxx = f->maxx;
	int miny = f->miny;
	int maxy = f->maxy;
	f->inv = False;
	f->printsize = 0;
	f->printpos = 0;
	f->printrow = 0;
	f->printcolfg = colfg;
	f->printcolbg = colbg;
	DrawRect(minx, miny, maxx-minx, maxy-miny, colbg);
}

// Draw clear screen with current printing background color (default black)
void DrawClear()
{
	DrawClearCol(FrameBuffer.printcolfg, FrameBuffer.printcolbg);
}

// ----------------------------------------------------------------------------
//                            Draw point
// ----------------------------------------------------------------------------

// draw point (alpha 0=transparent, 255=opaque)
void DrawPoint(int x, int y, u32 col)
{
	sFrameBuffer* f = &FrameBuffer;
	int minx = f->minx;
	int maxx = f->maxx;
	int miny = f->miny;
	int maxy = f->maxy;
	if ((x >= minx) && (x < maxx) && (y >= miny) && (y < maxy))
	{
		// pointer to buffer
		u32* d = &f->drawbuf[x + y*f->drawpitchpix];

		// get alpha
		u32 a = col >> 24;
		if (a == 0) return; // full transparent
		if (a == 255) // full opaque
		{
			*d = col;
			return;
		}

		// prepare components for blending
		DRAW_BLEND_PREP();

		// blend one pixel
		DRAW_BLEND_PIXEL();
	}
}

// invert point
void DrawPointInv(int x, int y)
{
	sFrameBuffer* f = &FrameBuffer;
	int minx = f->minx;
	int maxx = f->maxx;
	int miny = f->miny;
	int maxy = f->maxy;
	if ((x >= minx) && (x < maxx) && (y >= miny) && (y < maxy)) f->drawbuf[x + y*f->drawpitchpix] ^= DRAW_INV_VAL;
}

// get point (returns COL_BLACK if out of clipping range)
u32 DrawGetPoint(int x, int y)
{
	sFrameBuffer* f = &FrameBuffer;
	int minx = f->minx;
	int maxx = f->maxx;
	int miny = f->miny;
	int maxy = f->maxy;
	if ((x >= minx) && (x < maxx) && (y >= miny) && (y < maxy)) return f->drawbuf[x + y*f->drawpitchpix];
	return COL_BLACK;
}

// ----------------------------------------------------------------------------
//                            Draw rectangle
// ----------------------------------------------------------------------------

// draw rectangle (w and h can be negative = flip rectangle; alpha 0=transparent, 255=opaque)
void DrawRect(int x, int y, int w, int h, u32 col)
{
	// flip rectangle
	if (w < 0)
	{
		x += w;
		w = -w;
	}

	if (h < 0)
	{
		y += h;
		h = -h;
	}

	// limit x
	sFrameBuffer* f = &FrameBuffer;
	int k = f->minx - x;
	if (k > 0)
	{
		w -= k;
		x += k;
	}

	// limit w
	k = f->maxx;
	if (x + w > k) w = k - x;
	if (w <= 0) return;

	// limit y
	k = f->miny - y;
	if (k > 0)
	{
		h -= k;
		y += k;
	}

	// limit h
	k = f->maxy;
	if (y + h > k) h = k - y;
	if (h <= 0) return;

	// prepare destination
	int wb = f->drawpitchpix;
	u32* d = &f->drawbuf[x + y*wb];
	wb -= w;
	int i;

	// get alpha
	u32 a = col >> 24;
	if (a == 0) return; // full transparent
	if (a == 255)
	{
		// full opaque
		for (; h > 0; h--)
		{
			for (i = w; i > 0; i--) *d++ = col;
			d += wb;
		}
		return;
	}

	// prepare components for blending
	DRAW_BLEND_PREP();

	// draw with blending
	for (; h > 0; h--)
	{
		for (i = w; i > 0; i--)
		{
			// blend one pixel
			DRAW_BLEND_PIXEL();
			d++;
		}
		d += wb;
	}
}

// invert rectangle (w and h can be negative = flip rectangle)
void DrawRectInv(int x, int y, int w, int h)
{
	// flip rectangle
	if (w < 0)
	{
		x += w;
		w = -w;
	}

	if (h < 0)
	{
		y += h;
		h = -h;
	}

	// limit x
	sFrameBuffer* f = &FrameBuffer;
	int k = f->minx - x;
	if (k > 0)
	{
		w -= k;
		x += k;
	}

	// limit w
	k = f->maxx;
	if (x + w > k) w = k - x;
	if (w <= 0) return;

	// limit y
	k = f->miny - y;
	if (k > 0)
	{
		h -= k;
		y += k;
	}

	// limit h
	k = f->maxy;
	if (y + h > k) h = k - y;
	if (h <= 0) return;

	// draw
	int wb = f->drawpitchpix;
	u32* d = &f->drawbuf[x + y*wb];
	wb -= w;
	int i;
	for (; h > 0; h--)
	{
		for (i = w; i > 0; i--)
		{
			*d ^= DRAW_INV_VAL;
			d++;
		}
		d += wb;
	}
}

// ----------------------------------------------------------------------------
//                          Draw horizontal line
// ----------------------------------------------------------------------------

// draw horizontal line (w can be negative = flip line; alpha 0=transparent, 255=opaque)
void DrawHLine(int x, int y, int w, u32 col)
{
	DrawRect(x, y, w, 1, col);
}

// invert horizontal line (w can be negative = flip line)
void DrawHLineInv(int x, int y, int w)
{
	DrawRectInv(x, y, w, 1);
}

// ----------------------------------------------------------------------------
//                          Draw vertical line
// ----------------------------------------------------------------------------

// draw vertical line (h can be negative = flip line; alpha 0=transparent, 255=opaque)
void DrawVLine(int x, int y, int h, u32 col)
{
	DrawRect(x, y, 1, h, col);
}

// invert vertical line (h can be negative = flip line)
void DrawVLineInv(int x, int y, int h)
{
	DrawRectInv(x, y, 1, h);
}

// ----------------------------------------------------------------------------
//                           Draw frame
// ----------------------------------------------------------------------------

// draw frame (dimensions must be > 0; alpha 0=transparent, 255=opaque)
void DrawFrame(int x, int y, int w, int h, u32 col)
{
	// check size
	if ((w <= 0) || (h <= 0)) return;

	// bottom line
	DrawHLine(x, y+h-1, w, col);
	if (h > 1)
	{
		// right line
		DrawVLine(x+w-1, y, h-1, col);

		// top line
		DrawHLine(x, y, w-1, col);

		// left line (light)
		if (h > 2) DrawVLine(x, y+1, h-2, col);
	}
}

// draw frame 3D (dimensions must be > 0; alpha 0=transparent, 255=opaque)
void DrawFrame2(int x, int y, int w, int h, u32 col_light, u32 col_dark)
{
	// check size
	if ((w <= 0) || (h <= 0)) return;

	// bottom line (dark)
	DrawHLine(x, y+h-1, w, col_dark);
	if (h > 1)
	{
		// right line (dark)
		DrawVLine(x+w-1, y, h-1, col_dark);

		// top line (light)
		DrawHLine(x, y, w-1, col_light);

		// left line (light)
		if (h > 2) DrawVLine(x, y+1, h-2, col_light);
	}
}

// invert frame (dimensions must be > 0)
void DrawFrameInv(int x, int y, int w, int h)
{
	// check size
	if ((w <= 0) || (h <= 0)) return;

	// bottom line (dark)
	DrawHLineInv(x, y+h-1, w);
	if (h > 1)
	{
		// right line (dark)
		DrawVLineInv(x+w-1, y, h-1);

		// top line (light)
		DrawHLineInv(x, y, w-1);

		// left line (light)
		if (h > 2) DrawVLineInv(x, y+1, h-2);
	}
}

// draw thick frame (dimensions must be > 0; alpha 0=transparent, 255=opaque)
void DrawFrameW(int x, int y, int w, int h, int thick, u32 col)
{
	// check size
	if ((w <= 0) || (h <= 0)) return;

	// full frame
	if ((w <= 2*thick) || (h <= 2*thick))
	{
		DrawRect(x, y, w, h, col);
	}

	// thick frame
	else
	{
		// top line
		DrawRect(x, y, w, thick, col);

		// bottom line
		DrawRect(x, y+h-thick, w, thick, col);

		// left line
		h -= 2*thick;
		y += thick;
		DrawRect(x, y, thick, h, col);

		// right line
		DrawRect(x+w-thick, y, thick, h, col);
	}
}

// invert thick frame (dimensions must be > 0)
void DrawFrameWInv(int x, int y, int w, int h, int thick)
{
	// check size
	if ((w <= 0) || (h <= 0)) return;

	// full frame
	if ((w <= 2*thick) || (h <= 2*thick))
	{
		DrawRectInv(x, y, w, h);
	}

	// thick frame
	else
	{
		// top line
		DrawRectInv(x, y, w, thick);

		// bottom line
		DrawRectInv(x, y+h-thick, w, thick);

		// left line
		h -= 2*thick;
		y += thick;
		DrawRectInv(x, y, thick, h);

		// right line
		DrawRectInv(x+w-thick, y, thick, h);
	}
}

// ----------------------------------------------------------------------------
//                           Draw line
// ----------------------------------------------------------------------------

// Internal function - draw line with overlapped pixels (alpha 0=transparent, 255=opaque)
void DrawLineOver(int x1, int y1, int x2, int y2, Bool over, u32 col)
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
	sFrameBuffer* f = &FrameBuffer;
	int sy = 1;
	int wb = f->drawpitchpix;
	int ddy = wb;
	if (dy < 0)
	{
		sy = -1;
		ddy = -ddy;
		dy = -dy;
	}

	// destination address (pointer can be out of clipping range, but it is OK for now)
	u32* d = &f->drawbuf[x1 + y1*wb];

	// get clipping
	int minx = f->minx;
	int maxx = f->maxx;
	int miny = f->miny;
	int maxy = f->maxy;

	// get alpha
	u32 a = col >> 24;
	if (a == 0) return; // full transparent
	if (a == 255) // full opaque
	{
		// draw first pixel
		if ((x1 >= minx) && (x1 < maxx) && (y1 >= miny) && (y1 < maxy)) *d = col;

		// steeply in X direction, X is prefered as base
		if (dx > dy)
		{
			int m = 2*dy;
			int p = m - dx;
			dx = 2*dx;
			while (x1 != x2)
			{
				x1 += sx;
				d += sx;
				if (p > 0)
				{
					// draw overlapped pixel
					if (over && (x1 >= minx) && (x1 < maxx) && (y1 >= miny) && (y1 < maxy)) *d = col;
					y1 += sy;
					d += ddy;
					p -= dx;
				}
				p += m;
				if ((x1 >= minx) && (x1 < maxx) && (y1 >= miny) && (y1 < maxy)) *d = col;
			}
		}

		// steeply in Y direction, Y is prefered as base
		else
		{
			int m = 2*dx;
			int p = m - dy;
			dy = 2*dy;
			while (y1 != y2)
			{
				y1 += sy;
				d += ddy;
				if (p > 0)
				{
					// draw overlapped pixel
					if (over && (x1 >= minx) && (x1 < maxx) && (y1 >= miny) && (y1 < maxy)) *d = col;
					x1 += sx;
					d += sx;
					p -= dy;
				}
				p += m;
				if ((x1 >= minx) && (x1 < maxx) && (y1 >= miny) && (y1 < maxy)) *d = col;
			}
		}
	}

	// transparency
	else
	{
		// prepare components for blending
		DRAW_BLEND_PREP();

		// draw first pixel
		if ((x1 >= minx) && (x1 < maxx) && (y1 >= miny) && (y1 < maxy))
		{
			// blend one pixel
			DRAW_BLEND_PIXEL();
		}

		// steeply in X direction, X is prefered as base
		if (dx > dy)
		{
			int m = 2*dy;
			int p = m - dx;
			dx = 2*dx;
			while (x1 != x2)
			{
				x1 += sx;
				d += sx;
				if (p > 0)
				{
					// draw overlapped pixel
					if (over && (x1 >= minx) && (x1 < maxx) && (y1 >= miny) && (y1 < maxy))
					{
						// blend one pixel
						DRAW_BLEND_PIXEL();
					}
					y1 += sy;
					d += ddy;
					p -= dx;
				}
				p += m;
				if ((x1 >= minx) && (x1 < maxx) && (y1 >= miny) && (y1 < maxy))
				{
					// blend one pixel
					DRAW_BLEND_PIXEL();
				}
			}
		}

		// steeply in Y direction, Y is prefered as base
		else
		{
			int m = 2*dx;
			int p = m - dy;
			dy = 2*dy;
			while (y1 != y2)
			{
				y1 += sy;
				d += ddy;
				if (p > 0)
				{
					// draw overlapped pixel
					if (over && (x1 >= minx) && (x1 < maxx) && (y1 >= miny) && (y1 < maxy))
					{
						// blend one pixel
						DRAW_BLEND_PIXEL();
					}
					x1 += sx;
					d += sx;
					p -= dy;
				}
				p += m;
				if ((x1 >= minx) && (x1 < maxx) && (y1 >= miny) && (y1 < maxy))
				{
					// blend one pixel
					DRAW_BLEND_PIXEL();
				}
			}
		}
	}
}

// Internal function - invert line with overlapped pixels
void DrawLineOverInv(int x1, int y1, int x2, int y2, Bool over)
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
	sFrameBuffer* f = &FrameBuffer;
	int sy = 1;
	int wb = f->drawpitchpix;
	int ddy = wb;
	if (dy < 0)
	{
		sy = -1;
		ddy = -ddy;
		dy = -dy;
	}

	// destination address (pointer can be out of clipping range, but it is OK for now)
	u32* d = &f->drawbuf[x1 + y1*wb];

	// get clipping
	int minx = f->minx;
	int maxx = f->maxx;
	int miny = f->miny;
	int maxy = f->maxy;

	// draw first pixel
	if ((x1 >= minx) && (x1 < maxx) && (y1 >= miny) && (y1 < maxy)) *d ^= DRAW_INV_VAL;

	// steeply in X direction, X is prefered as base
	if (dx > dy)
	{
		int m = 2*dy;
		int p = m - dx;
		dx = 2*dx;
		while (x1 != x2)
		{
			x1 += sx;
			d += sx;
			if (p > 0)
			{
				// draw overlapped pixel
				if (over && (x1 >= minx) && (x1 < maxx) && (y1 >= miny) && (y1 < maxy)) *d ^= DRAW_INV_VAL;
				y1 += sy;
				d += ddy;
				p -= dx;
			}
			p += m;
			if ((x1 >= minx) && (x1 < maxx) && (y1 >= miny) && (y1 < maxy)) *d ^= DRAW_INV_VAL;
		}
	}

	// steeply in Y direction, Y is prefered as base
	else
	{
		int m = 2*dx;
		int p = m - dy;
		dy = 2*dy;
		while (y1 != y2)
		{
			y1 += sy;
			d += ddy;
			if (p > 0)
			{
				// draw overlapped pixel
				if (over && (x1 >= minx) && (x1 < maxx) && (y1 >= miny) && (y1 < maxy)) *d ^= DRAW_INV_VAL;
				x1 += sx;
				d += sx;
				p -= dy;
			}
			p += m;
			if ((x1 >= minx) && (x1 < maxx) && (y1 >= miny) && (y1 < maxy)) *d ^= DRAW_INV_VAL;
		}
	}
}

// draw line (alpha 0=transparent, 255=opaque)
void DrawLine(int x1, int y1, int x2, int y2, u32 col)
{
	DrawLineOver(x1, y1, x2, y2, False, col);
}

// invert line
void DrawLineInv(int x1, int y1, int x2, int y2)
{
	DrawLineOverInv(x1, y1, x2, y2, False);
}

// Draw thick line (alpha 0=transparent, 255=opaque)
//  thick ... thick of line in pixels
//  round ... draw round ends
// Do not use "round ends" and "transparent" at the same time - round ends incorrectly redraw transparency.
void DrawLineW(int x1, int y1, int x2, int y2, int thick, Bool round, u32 col)
{
	int i, k, dx, dy, dxabs, dyabs;
	Bool over;

	// thick is 1
	if (thick <= 1)
	{
		if (thick == 1) DrawLineOver(x1, y1, x2, y2, False, col);
		return;
	}

	// difference of coordinates
	dxabs = x2 - x1;
	dyabs = y2 - y1;
	if (dxabs < 0) dxabs = -dxabs;
	if (dyabs < 0) dyabs = -dyabs;

	// draw round ends
	if (round && (thick > 2))
	{
		int thick2 = (thick-1) | 1; // a circle of odd size is needed to be placed exactly on the coordinate
		DrawRound(x1, y1, thick2, DRAW_ROUND_ALL, col);
		DrawRound(x2, y2, thick2, DRAW_ROUND_ALL, col);
	}

	// difference of coordinates
	//   X and Y coordinates are "swapped" because they express
	//   a direction orthogonal to the direction of the line
	dy = x2 - x1;
	dx = y2 - y1;
	Bool swap = True;

	// increment X
	int sx = 1;
	if (dx < 0)
	{
		sx = -1;
		dx = -dx;
		swap = !swap;
	}

	// increment Y
	int sy = 1;
	if (dy < 0)
	{
		sy = -1;
		dy = -dy;
		swap = !swap;
	}

	// thick correction (otherwise the diagonal lines would be thicker than the perpendicular lines)
	if (thick >= 4)
	{
		if ((dxabs >= dyabs*3/4) && (dxabs*3/4 <= dyabs))
		{
			if (thick >= 17) thick--;
			if (thick >= 14) thick--;
			if (thick >= 11) thick--;
			if (((thick & 1) == 0) || (thick >= 7)) thick--;
		}
	}

	// prepare adjust to shift to the middle of the line
	int dx2 = dx*2;
	int dy2 = dy*2;
	int adj = thick/2;

	// steeply in X direction, X is prefered as base
	if (dx >= dy)
	{
		// swapped direction
		if (swap)
		{
			adj = thick - 1 - adj;
			sy = -sy;
		}
		else
			sx = -sx;

		// shift to the middle of the line
		int err = dy2 - dx;
		for (i = adj; i > 0; i--)
		{
			x1 -= sx;
			x2 -= sx;
			if (err >= 0)
			{
				y1 -= sy;
				y2 -= sy;
				err -= dx2;
			}
			err += dy2;
		}

		// draw first line
		DrawLineOver(x1, y1, x2, y2, False, col);

		// draw other lines
		err = dy2 - dx;
		for (i = thick; i > 1; i--)
		{
			x1 += sx;
			x2 += sx;
			over = False;
			if (err >= 0)
			{
				y1 += sy;
				y2 += sy;
				err -= dx2;
				over = True;
			}

			// draw line
			DrawLineOver(x1, y1, x2, y2, over, col);
			err += dy2;
		}
	}

	// steeply in Y direction, Y is prefered as base
	else
	{
		// swapped direction
		if (swap)
			sx = -sx;
		else
		{
			adj = thick - 1 - adj;
			sy = -sy;
		}

		// shift to the middle of the line
		int err = dx2 - dy;
		for (i = adj; i > 0; i--)
		{
			y1 -= sy;
			y2 -= sy;
			if (err >= 0)
			{
				x1 -= sx;
				x2 -= sx;
				err -= dy2;
			}
			err += dx2;
		}

		// draw first line
		DrawLineOver(x1, y1, x2, y2, False, col);

		// draw other lines
		err = dx2 - dy;
		for (i = thick; i > 1; i--)
		{
			y1 += sy;
			y2 += sy;
			over = False;
			if (err >= 0)
			{
				x1 += sx;
				x2 += sx;
				err -= dy2;
				over = True;
			}

			// draw line
			DrawLineOver(x1, y1, x2, y2, over, col);
			err += dx2;
		}
	}
}

// Draw thick line inverted
//  thick ... thick of line in pixels
void DrawLineWInv(int x1, int y1, int x2, int y2, int thick)
{
	int i, k, dx, dy, dxabs, dyabs;
	Bool over;

	// thick is 1
	if (thick <= 1)
	{
		if (thick == 1) DrawLineOverInv(x1, y1, x2, y2, False);
		return;
	}

	// difference of coordinates
	dxabs = x2 - x1;
	dyabs = y2 - y1;
	if (dxabs < 0) dxabs = -dxabs;
	if (dyabs < 0) dyabs = -dyabs;

	// difference of coordinates
	//   X and Y coordinates are "swapped" because they express
	//   a direction orthogonal to the direction of the line
	dy = x2 - x1;
	dx = y2 - y1;
	Bool swap = True;

	// increment X
	int sx = 1;
	if (dx < 0)
	{
		sx = -1;
		dx = -dx;
		swap = !swap;
	}

	// increment Y
	int sy = 1;
	if (dy < 0)
	{
		sy = -1;
		dy = -dy;
		swap = !swap;
	}

	// thick correction (otherwise the diagonal lines would be thicker than the perpendicular lines)
	if (thick >= 4)
	{
		if ((dxabs >= dyabs*3/4) && (dxabs*3/4 <= dyabs))
		{
			if (thick >= 17) thick--;
			if (thick >= 14) thick--;
			if (thick >= 11) thick--;
			if (((thick & 1) == 0) || (thick >= 7)) thick--;
		}
	}

	// prepare adjust to shift to the middle of the line
	int dx2 = dx*2;
	int dy2 = dy*2;
	int adj = thick/2;

	// steeply in X direction, X is prefered as base
	if (dx >= dy)
	{
		// swapped direction
		if (swap)
		{
			adj = thick - 1 - adj;
			sy = -sy;
		}
		else
			sx = -sx;

		// shift to the middle of the line
		int err = dy2 - dx;
		for (i = adj; i > 0; i--)
		{
			x1 -= sx;
			x2 -= sx;
			if (err >= 0)
			{
				y1 -= sy;
				y2 -= sy;
				err -= dx2;
			}
			err += dy2;
		}

		// draw first line
		DrawLineOverInv(x1, y1, x2, y2, False);

		// draw other lines
		err = dy2 - dx;
		for (i = thick; i > 1; i--)
		{
			x1 += sx;
			x2 += sx;
			over = False;
			if (err >= 0)
			{
				y1 += sy;
				y2 += sy;
				err -= dx2;
				over = True;
			}

			// draw line
			DrawLineOverInv(x1, y1, x2, y2, over);
			err += dy2;
		}
	}

	// steeply in Y direction, Y is prefered as base
	else
	{
		// swapped direction
		if (swap)
			sx = -sx;
		else
		{
			adj = thick - 1 - adj;
			sy = -sy;
		}

		// shift to the middle of the line
		int err = dx2 - dy;
		for (i = adj; i > 0; i--)
		{
			y1 -= sy;
			y2 -= sy;
			if (err >= 0)
			{
				x1 -= sx;
				x2 -= sx;
				err -= dy2;
			}
			err += dx2;
		}

		// draw first line
		DrawLineOverInv(x1, y1, x2, y2, False);

		// draw other lines
		err = dx2 - dy;
		for (i = thick; i > 1; i--)
		{
			y1 += sy;
			y2 += sy;
			over = False;
			if (err >= 0)
			{
				x1 += sx;
				x2 += sx;
				err -= dy2;
				over = True;
			}

			// draw line
			DrawLineOverInv(x1, y1, x2, y2, over);
			err += dx2;
		}
	}
}

// ----------------------------------------------------------------------------
//                          Draw round (Filled circle)
// ----------------------------------------------------------------------------

#define DRAW_ROUND_HIDE()	{				\
	if ((mask & DRAW_ROUND_NOTOP) != 0) y1 = 0;		\
	if ((mask & DRAW_ROUND_NOBOTTOM) != 0) y2 = 0;		\
	if ((mask & DRAW_ROUND_NOLEFT) != 0) x1 = 0;		\
	if ((mask & DRAW_ROUND_NORIGHT) != 0) x2 = 0; }

#define DRAW_ROUND_CLIP()	{			\
		int k = f->minx;			\
		if (x+x1 < k) x1 = k - x;		\
		k = f->maxx;				\
		if (x+x2 >= k) x2 = k - 1 - x;		\
		k = f->miny;				\
		if (y+y1 < k) y1 = k - y;		\
		k = f->maxy;				\
		if (y+y2 >= k) y2 = k - 1 - y;		\
		if ((x2 < x1) || (y2 < y1)) return; }

//  diam ... diameter of the round (radius = d/2)
//  mask ... hide parts of the round with DRAW_ROUND_* (or their combination); use DRAW_ROUND_ALL or 0 to draw whole round
//		DRAW_ROUND_NOTOP	= hide top part of the round
//		DRAW_ROUND_NOBOTTOM	= hide bottom part of the round
//		DRAW_ROUND_NOLEFT	= hide left part of the round
//		DRAW_ROUND_NORIGHT	= hide right part of the round
//		DRAW_ROUND_ALL		= draw whole round

// Draw round (alpha 0=transparent, 255=opaque)
void DrawRound(int x, int y, int diam, int mask, u32 col)
{
	if (diam < 1) diam = 1;
	int r = (diam+1)/2;
	int x1 = -r;
	int x2 = +r;
	int y1 = -r;
	int y2 = +r;
	sFrameBuffer* f = &FrameBuffer;
	int wb = f->drawpitchpix;
	u32 *d0, *d;
	int r2;

	// hide parts of the round
	DRAW_ROUND_HIDE();

	// get alpha
	u32 a = col >> 24;
	if (a == 0) return; // full transparent

	// odd diameter (1, 3, 5, ...)
	if ((diam & 1) != 0)
	{
		// clipping
		DRAW_ROUND_CLIP();

		// prepare buffer address
		d0 = &f->drawbuf[(x+x1) + (y+y1)*wb];
		r2 = r*(r-1);
		if (diam == 3) r2--;

		// full opaque
		if (a == 255)
		{
			// draw round
			for (y = y1; y <= y2; y++)
			{
				d = d0;
				for (x = x1; x <= x2; x++)
				{
					if ((x*x + y*y) <= r2) *d = col;
					d++;
				}
				d0 += wb;
			}
		}

		// transparent
		else
		{
			// prepare components for blending
			DRAW_BLEND_PREP();

			// draw round
			for (y = y1; y <= y2; y++)
			{
				d = d0;
				for (x = x1; x <= x2; x++)
				{
					if ((x*x + y*y) <= r2)
					{
						// blend one pixel
						DRAW_BLEND_PIXEL();
					}
					d++;
				}
				d0 += wb;
			}
		}
	}

	// even diameter (2, 4, 6, ...)
	else
	{
		// to correspond to the center of DrawCircle [x0,y0] = [x+d/2,y+d/2]
		x--;
		y--;
		x1++;
		y1++;

		// clipping
		DRAW_ROUND_CLIP();

		// prepare buffer address
		d0 = &f->drawbuf[(x+x1) + (y+y1)*wb];
		r2 = r*r;

		// full opaque
		if (a == 255)
		{
			// draw round
			for (y = y1; y <= y2; y++)
			{
				d = d0;
				for (x = x1; x <= x2; x++)
				{
					if ((x*(x-1) + y*(y-1)) < r2) *d = col;
					d++;
				}
				d0 += wb;
			}
		}

		// transparent
		else
		{
			// prepare components for blending
			DRAW_BLEND_PREP();

			// draw round
			for (y = y1; y <= y2; y++)
			{
				d = d0;
				for (x = x1; x <= x2; x++)
				{
					if ((x*(x-1) + y*(y-1)) < r2)
					{
						// blend one pixel
						DRAW_BLEND_PIXEL();
					}
					d++;
				}
				d0 += wb;
			}
		}
	}
}

// Invert round
void DrawRoundInv(int x, int y, int diam, int mask)
{
	if (diam < 1) diam = 1;
	int r = (diam+1)/2;
	int x1 = -r;
	int x2 = +r;
	int y1 = -r;
	int y2 = +r;
	sFrameBuffer* f = &FrameBuffer;
	int wb = f->drawpitchpix;
	u32 *d0, *d;
	int r2;

	// hide parts of the round
	DRAW_ROUND_HIDE();

	// odd diameter (1, 3, 5, ...)
	if ((diam & 1) != 0)
	{
		// clipping
		DRAW_ROUND_CLIP();

		// prepare buffer address
		d0 = &f->drawbuf[(x+x1) + (y+y1)*wb];
		r2 = r*(r-1);
		if (diam == 3) r2--;

		// draw round
		for (y = y1; y <= y2; y++)
		{
			d = d0;
			for (x = x1; x <= x2; x++)
			{
				if ((x*x + y*y) <= r2) *d ^= DRAW_INV_VAL;
				d++;
			}
			d0 += wb;
		}
	}

	// even diameter (2, 4, 6, ...)
	else
	{
		// to correspond to the center of DrawCircle [x0,y0] = [x+d/2,y+d/2]
		x--;
		y--;
		x1++;
		y1++;

		// clipping
		DRAW_ROUND_CLIP();

		// prepare buffer address
		d0 = &f->drawbuf[(x+x1) + (y+y1)*wb];
		r2 = r*r;

		// draw round
		for (y = y1; y <= y2; y++)
		{
			d = d0;
			for (x = x1; x <= x2; x++)
			{
				if ((x*(x-1) + y*(y-1)) < r2) *d ^= DRAW_INV_VAL;
				d++;
			}
			d0 += wb;
		}
	}
}

// ----------------------------------------------------------------------------
//                               Draw circle
// ----------------------------------------------------------------------------
// Using Mid-Point Circle Drawing Algorithm

// draw circle arcs
//#define DRAW_CIRCLE_ARC0	B0		// draw arc 0..45 deg
//#define DRAW_CIRCLE_ARC1	B1		// draw arc 45..90 deg
//#define DRAW_CIRCLE_ARC2	B2		// draw arc 90..135 deg
//#define DRAW_CIRCLE_ARC3	B3		// draw arc 135..180 deg
//#define DRAW_CIRCLE_ARC4	B4		// draw arc 180..225 deg
//#define DRAW_CIRCLE_ARC5	B5		// draw arc 225..270 deg
//#define DRAW_CIRCLE_ARC6	B6		// draw arc 270..315 deg
//#define DRAW_CIRCLE_ARC7	B7		// draw arc 315..360 deg
//#define DRAW_CIRCLE_ALL	0xff		// draw whole circle

#define DRAW_CIRCLE_PIX(xxx,yyy,mmm)		\
	if ((mask & mmm) != 0)			\
	{	int x2 = x+(xxx);		\
		int y2 = y+(yyy);		\
		if ((x2 >= minx) &&		\
			(x2 < maxx) &&		\
			(y2 >= miny) &&		\
			(y2 < maxy)) {		\
		d = &d0[x2 + y2*wb];		\
		*d = col; } }

#define DRAW_CIRCLE_PIX_BLEND(xxx,yyy,mmm)	\
	if ((mask & mmm) != 0)			\
	{	int x2 = x+(xxx);		\
		int y2 = y+(yyy);		\
		if ((x2 >= minx) &&		\
			(x2 < maxx) &&		\
			(y2 >= miny) &&		\
			(y2 < maxy)) {		\
		d = &d0[x2 + y2*wb];		\
		DRAW_BLEND_PIXEL(); } }

#define DRAW_CIRCLE_PIXINV(xxx,yyy,mmm)		\
	if ((mask & mmm) != 0)			\
	{	int x2 = x+(xxx);		\
		int y2 = y+(yyy);		\
		if ((x2 >= minx) &&		\
			(x2 < maxx) &&		\
			(y2 >= miny) &&		\
			(y2 < maxy)) {		\
		d = &d0[x2 + y2*wb];		\
		*d = ~*d; } }

//  diam ... diameter of the circle (radius = d/2)
//  mask ... draw circle arcs, use combination of DRAW_CIRCLE_*; use DRAW_CIRCLE_ALL or 0xff to draw whole circle

// Draw circle or arc (alpha 0=transparent, 255=opaque)
void DrawCircle(int x, int y, int diam, int mask, u32 col)
{
	// check diameter
	if (diam <= 1)
	{
		if (diam == 1) DrawPoint(x, y, col);
		return;
	}

	// prepare
	int r = diam/2;
	int xx = 0;
	int yy = r;
	int p = 1 - r;
	sFrameBuffer* f = &FrameBuffer;
	int minx = f->minx;
	int maxx = f->maxx;
	int miny = f->miny;
	int maxy = f->maxy;

	// pointer to middle of the circle
	int wb = f->drawpitchpix;
	u32* d0 = f->drawbuf;
	u32* d;

	// get alpha
	u32 a = col >> 24;
	if (a == 0) return; // full transparent
	int c = -1; // even correction - on even diameter do not draw middle point
	if (a == 255) // full opaque
	{
		// odd diameter
		if ((diam & 1) != 0) // on odd diameter - draw middle point
		{
			// draw middle point (xx = 0)
			DRAW_CIRCLE_PIX(+yy,  0,DRAW_CIRCLE_ARC0);	// 0..45 deg, 315..360 deg
			DRAW_CIRCLE_PIX(  0,-yy,DRAW_CIRCLE_ARC1);	// 45..90 deg, 90..135 deg
			DRAW_CIRCLE_PIX(-yy,  0,DRAW_CIRCLE_ARC3);	// 135..180 deg, 180..225 deg
			DRAW_CIRCLE_PIX(  0,+yy,DRAW_CIRCLE_ARC5);	// 225..270 deg, 270..315 deg
			c = 0;
		}

		while (True)
		{
			// shift to next point
			xx++;
			if (p > 0)
			{
				yy--;
				p -= 2*yy;
			}
			p += 2*xx + 1;

			// stop drawing
			if (xx >= yy)
			{
				if (xx == yy)
				{
					// draw last point (xx == yy)
					DRAW_CIRCLE_PIX(+xx+c,-xx  ,DRAW_CIRCLE_ARC0);	// 0..45 deg, 45..90 deg
					DRAW_CIRCLE_PIX(-xx  ,-xx  ,DRAW_CIRCLE_ARC2);	// 90..135 deg, 135..180 deg
					DRAW_CIRCLE_PIX(-xx  ,+xx+c,DRAW_CIRCLE_ARC4);	// 180..225 deg, 225..270 deg
					DRAW_CIRCLE_PIX(+xx+c,+xx+c,DRAW_CIRCLE_ARC6);	// 270..315 deg, 315..360 deg
				}
				break;
			}

			// draw points
			DRAW_CIRCLE_PIX(+yy+c,-xx  ,DRAW_CIRCLE_ARC0);		// 0..45 deg
			DRAW_CIRCLE_PIX(+xx+c,-yy  ,DRAW_CIRCLE_ARC1);		// 45..90 deg
			DRAW_CIRCLE_PIX(-xx  ,-yy  ,DRAW_CIRCLE_ARC2);		// 90..135 deg
			DRAW_CIRCLE_PIX(-yy  ,-xx  ,DRAW_CIRCLE_ARC3);		// 135..180 deg
			DRAW_CIRCLE_PIX(-yy  ,+xx+c,DRAW_CIRCLE_ARC4);		// 180..225 deg
			DRAW_CIRCLE_PIX(-xx  ,+yy+c,DRAW_CIRCLE_ARC5);		// 225..270 deg
			DRAW_CIRCLE_PIX(+xx+c,+yy+c,DRAW_CIRCLE_ARC6);		// 270..315 deg
			DRAW_CIRCLE_PIX(+yy+c,+xx+c,DRAW_CIRCLE_ARC7);		// 315..360 deg
		}
	}

	// transparency
	else
	{
		// prepare components for blending
		DRAW_BLEND_PREP();

		// odd diameter
		if ((diam & 1) != 0) // on odd diameter - draw middle point
		{
			// draw middle point (xx = 0)
			DRAW_CIRCLE_PIX_BLEND(+yy,  0,DRAW_CIRCLE_ARC0);	// 0..45 deg, 315..360 deg
			DRAW_CIRCLE_PIX_BLEND(  0,-yy,DRAW_CIRCLE_ARC1);	// 45..90 deg, 90..135 deg
			DRAW_CIRCLE_PIX_BLEND(-yy,  0,DRAW_CIRCLE_ARC3);	// 135..180 deg, 180..225 deg
			DRAW_CIRCLE_PIX_BLEND(  0,+yy,DRAW_CIRCLE_ARC5);	// 225..270 deg, 270..315 deg
			c = 0;
		}

		while (True)
		{
			// shift to next point
			xx++;
			if (p > 0)
			{
				yy--;
				p -= 2*yy;
			}
			p += 2*xx + 1;

			// stop drawing
			if (xx >= yy)
			{
				if (xx == yy)
				{
					// draw last point (xx == yy)
					DRAW_CIRCLE_PIX_BLEND(+xx+c,-xx  ,DRAW_CIRCLE_ARC0);	// 0..45 deg, 45..90 deg
					DRAW_CIRCLE_PIX_BLEND(-xx  ,-xx  ,DRAW_CIRCLE_ARC2);	// 90..135 deg, 135..180 deg
					DRAW_CIRCLE_PIX_BLEND(-xx  ,+xx+c,DRAW_CIRCLE_ARC4);	// 180..225 deg, 225..270 deg
					DRAW_CIRCLE_PIX_BLEND(+xx+c,+xx+c,DRAW_CIRCLE_ARC6);	// 270..315 deg, 315..360 deg
				}
				break;
			}

			// draw points
			DRAW_CIRCLE_PIX_BLEND(+yy+c,-xx  ,DRAW_CIRCLE_ARC0);		// 0..45 deg
			DRAW_CIRCLE_PIX_BLEND(+xx+c,-yy  ,DRAW_CIRCLE_ARC1);		// 45..90 deg
			DRAW_CIRCLE_PIX_BLEND(-xx  ,-yy  ,DRAW_CIRCLE_ARC2);		// 90..135 deg
			DRAW_CIRCLE_PIX_BLEND(-yy  ,-xx  ,DRAW_CIRCLE_ARC3);		// 135..180 deg
			DRAW_CIRCLE_PIX_BLEND(-yy  ,+xx+c,DRAW_CIRCLE_ARC4);		// 180..225 deg
			DRAW_CIRCLE_PIX_BLEND(-xx  ,+yy+c,DRAW_CIRCLE_ARC5);		// 225..270 deg
			DRAW_CIRCLE_PIX_BLEND(+xx+c,+yy+c,DRAW_CIRCLE_ARC6);		// 270..315 deg
			DRAW_CIRCLE_PIX_BLEND(+yy+c,+xx+c,DRAW_CIRCLE_ARC7);		// 315..360 deg
		}
	}
}

// Invert circle or arc
void DrawCircleInv(int x, int y, int diam, int mask)
{
	// check diameter
	if (diam <= 1)
	{
		if (diam == 1) DrawPointInv(x, y);
		return;
	}

	// prepare
	int r = diam/2;
	int xx = 0;
	int yy = r;
	int p = 1 - r;
	sFrameBuffer* f = &FrameBuffer;
	int minx = f->minx;
	int maxx = f->maxx;
	int miny = f->miny;
	int maxy = f->maxy;

	// pointer to middle of the circle
	int wb = f->drawpitchpix;
	u32* d0 = f->drawbuf;
	u32* d;

	int c = -1; // even correction - on even diameter do not draw middle point

	// odd diameter
	if ((diam & 1) != 0) // on odd diameter - draw middle point
	{
		// draw middle point (xx = 0)
		DRAW_CIRCLE_PIXINV(+yy,  0,DRAW_CIRCLE_ARC0);	// 0..45 deg, 315..360 deg
		DRAW_CIRCLE_PIXINV(  0,-yy,DRAW_CIRCLE_ARC1);	// 45..90 deg, 90..135 deg
		DRAW_CIRCLE_PIXINV(-yy,  0,DRAW_CIRCLE_ARC3);	// 135..180 deg, 180..225 deg
		DRAW_CIRCLE_PIXINV(  0,+yy,DRAW_CIRCLE_ARC5);	// 225..270 deg, 270..315 deg
		c = 0;
	}

	while (True)
	{
		// shift to next point
		xx++;
		if (p > 0)
		{
			yy--;
			p -= 2*yy;
		}
		p += 2*xx + 1;

		// stop drawing
		if (xx >= yy)
		{
			if (xx == yy)
			{
				// draw last point (xx == yy)
				DRAW_CIRCLE_PIXINV(+xx+c,-xx  ,DRAW_CIRCLE_ARC0);	// 0..45 deg, 45..90 deg
				DRAW_CIRCLE_PIXINV(-xx  ,-xx  ,DRAW_CIRCLE_ARC2);	// 90..135 deg, 135..180 deg
				DRAW_CIRCLE_PIXINV(-xx  ,+xx+c,DRAW_CIRCLE_ARC4);	// 180..225 deg, 225..270 deg
				DRAW_CIRCLE_PIXINV(+xx+c,+xx+c,DRAW_CIRCLE_ARC6);	// 270..315 deg, 315..360 deg
			}
			break;
		}

		// draw points
		DRAW_CIRCLE_PIXINV(+yy+c,-xx  ,DRAW_CIRCLE_ARC0);		// 0..45 deg
		DRAW_CIRCLE_PIXINV(+xx+c,-yy  ,DRAW_CIRCLE_ARC1);		// 45..90 deg
		DRAW_CIRCLE_PIXINV(-xx  ,-yy  ,DRAW_CIRCLE_ARC2);		// 90..135 deg
		DRAW_CIRCLE_PIXINV(-yy  ,-xx  ,DRAW_CIRCLE_ARC3);		// 135..180 deg
		DRAW_CIRCLE_PIXINV(-yy  ,+xx+c,DRAW_CIRCLE_ARC4);		// 180..225 deg
		DRAW_CIRCLE_PIXINV(-xx  ,+yy+c,DRAW_CIRCLE_ARC5);		// 225..270 deg
		DRAW_CIRCLE_PIXINV(+xx+c,+yy+c,DRAW_CIRCLE_ARC6);		// 270..315 deg
		DRAW_CIRCLE_PIXINV(+yy+c,+xx+c,DRAW_CIRCLE_ARC7);		// 315..360 deg
	}
}

// ----------------------------------------------------------------------------
//                               Draw ellipse
// ----------------------------------------------------------------------------
// Using mid-point ellipse drawing algorithm

#define DRAW_ELLIPSE_PIX(xxx,yyy,mmm)		\
	if ((mask & mmm) != 0)			\
	{	int x2 = x+(xxx);		\
		int y2 = y+(yyy);		\
		if ((x2 >= minx) &&		\
			(x2 < maxx) &&		\
			(y2 >= miny) &&		\
			(y2 < maxy)) {		\
		d = &d0[x2 + y2*wb];		\
		*d = col; } }

#define DRAW_ELLIPSE_PIXBLEND(xxx,yyy,mmm)	\
	if ((mask & mmm) != 0)			\
	{	int x2 = x+(xxx);		\
		int y2 = y+(yyy);		\
		if ((x2 >= minx) &&		\
			(x2 < maxx) &&		\
			(y2 >= miny) &&		\
			(y2 < maxy)) {		\
		d = &d0[x2 + y2*wb];		\
		DRAW_BLEND_PIXEL(); } }

#define DRAW_ELLIPSE_PIXINV(xxx,yyy,mmm)	\
	if ((mask & mmm) != 0)			\
	{	int x2 = x+(xxx);		\
		int y2 = y+(yyy);		\
		if ((x2 >= minx) &&		\
			(x2 < maxx) &&		\
			(y2 >= miny) &&		\
			(y2 < maxy)) {		\
		d = &d0[x2 + y2*wb];		\
		*d ^= 0x00ffffff; } }

//  dx ... diameter of ellipse in X direction, range 2..430 (X radius = dx/2)
//  dy ... diameter of ellipse in Y direction, range 2..430 (Y radius = dy/2)
//  mask ... draw ellipse arcs, use combination of DRAW_ELLIPSE_*; use DRAW_ELLIPSE_ALL or 0x0f to draw whole ellipse
//		DRAW_ELLIPSE_ARC0	= draw arc 0..90 deg
//		DRAW_ELLIPSE_ARC1	= draw arc 90..180 deg
//		DRAW_ELLIPSE_ARC2	= draw arc 180..270 deg
//		DRAW_ELLIPSE_ARC3	= draw arc 270..360 deg
//		DRAW_ELLIPSE_ALL	= draw whole ellipse

// Draw ellipse (alpha 0=transparent, 255=opaque)
void DrawEllipse(int x, int y, int dx, int dy, int mask, u32 col)
{
	// get alpha
	u32 a = col >> 24;
	if (a == 0) return; // full transparent

	// limit range
	if (dx < 2) dx = 2;
	if (dx > DRAW_ELLIPSE_MAXD) dx = DRAW_ELLIPSE_MAXD;
	if (dy < 2) dy = 2;
	if (dy > DRAW_ELLIPSE_MAXD) dy = DRAW_ELLIPSE_MAXD;

	// prepare
	int rx = dx/2;
	int ry = dy/2;
	int xx = 0;
	int yy = ry;
	sFrameBuffer* f = &FrameBuffer;
	int minx = f->minx;
	int maxx = f->maxx;
	int miny = f->miny;
	int maxy = f->maxy;

	// pointer to middle of the ellipse
	int wb = f->drawpitchpix;
	u32* d0 = f->drawbuf;
	u32* d;

	// odd diameters X and Y
	int cx = -1; // even correction X (on even diameter X do not draw middle point X)
	int cy = -1; // even correction Y (on even diameter Y do not draw middle point Y)
	if ((dy & 1) != 0) cy = 0;

	// full opaque
	if (a == 255)
	{
		if ((dx & 1) != 0)
		{
			DRAW_ELLIPSE_PIX(0, -ry   , DRAW_ELLIPSE_ARC0);
			DRAW_ELLIPSE_PIX(0, +ry+cy, DRAW_ELLIPSE_ARC2);
			cx = 0;
		}

		// odd diameter Y
		if ((dy & 1) != 0)
		{
			DRAW_ELLIPSE_PIX(-rx   , 0, DRAW_ELLIPSE_ARC1);
			DRAW_ELLIPSE_PIX(+rx+cx, 0, DRAW_ELLIPSE_ARC3);
		}

		// draw top and bottom arcs
		int p = ry*ry - rx*rx*ry + rx*rx/4;
		int ddx = 0;
		int ddy = 2*rx*rx*ry;
		while (True)
		{
			xx++;
			ddx += 2*ry*ry;
			if (p < 0)
				p += ddx + ry*ry;
			else
			{
				yy--;
				ddy -= 2*rx*rx;
				p += ddx - ddy + ry*ry;
			}

			if (ddx > ddy) break;

			DRAW_ELLIPSE_PIX(+xx+cx, -yy   , DRAW_ELLIPSE_ARC0);
			DRAW_ELLIPSE_PIX(-xx   , -yy   , DRAW_ELLIPSE_ARC1);
			DRAW_ELLIPSE_PIX(-xx   , +yy+cy, DRAW_ELLIPSE_ARC2);
			DRAW_ELLIPSE_PIX(+xx+cx, +yy+cy, DRAW_ELLIPSE_ARC3);
		}

		// draw left and right arcs
		xx = rx;
		yy = 0;
		p = rx*rx - ry*ry*rx + ry*ry/4;
		ddy = 0;
		ddx = 2*ry*ry*rx;
		while (True)
		{
			yy++;
			ddy += 2*rx*rx;
			if (p < 0)
				p += ddy + rx*rx;
			else
			{
				xx--;
				ddx -= 2*ry*ry;
				p += ddy - ddx + rx*rx;
			}

			if (ddy > ddx) break;

			DRAW_ELLIPSE_PIX(+xx+cx, -yy   , DRAW_ELLIPSE_ARC0);
			DRAW_ELLIPSE_PIX(-xx   , -yy   , DRAW_ELLIPSE_ARC1);
			DRAW_ELLIPSE_PIX(-xx   , +yy+cy, DRAW_ELLIPSE_ARC2);
			DRAW_ELLIPSE_PIX(+xx+cx, +yy+cy, DRAW_ELLIPSE_ARC3);
		}
	}

	// transparent
	else
	{
		// prepare components for blending
		DRAW_BLEND_PREP();

		if ((dx & 1) != 0)
		{
			DRAW_ELLIPSE_PIXBLEND(0, -ry   , DRAW_ELLIPSE_ARC0);
			DRAW_ELLIPSE_PIXBLEND(0, +ry+cy, DRAW_ELLIPSE_ARC2);
			cx = 0;
		}

		// odd diameter Y
		if ((dy & 1) != 0)
		{
			DRAW_ELLIPSE_PIXBLEND(-rx   , 0, DRAW_ELLIPSE_ARC1);
			DRAW_ELLIPSE_PIXBLEND(+rx+cx, 0, DRAW_ELLIPSE_ARC3);
		}

		// draw top and bottom arcs
		int p = ry*ry - rx*rx*ry + rx*rx/4;
		int ddx = 0;
		int ddy = 2*rx*rx*ry;
		while (True)
		{
			xx++;
			ddx += 2*ry*ry;
			if (p < 0)
				p += ddx + ry*ry;
			else
			{
				yy--;
				ddy -= 2*rx*rx;
				p += ddx - ddy + ry*ry;
			}

			if (ddx > ddy) break;

			DRAW_ELLIPSE_PIXBLEND(+xx+cx, -yy   , DRAW_ELLIPSE_ARC0);
			DRAW_ELLIPSE_PIXBLEND(-xx   , -yy   , DRAW_ELLIPSE_ARC1);
			DRAW_ELLIPSE_PIXBLEND(-xx   , +yy+cy, DRAW_ELLIPSE_ARC2);
			DRAW_ELLIPSE_PIXBLEND(+xx+cx, +yy+cy, DRAW_ELLIPSE_ARC3);
		}

		// draw left and right arcs
		xx = rx;
		yy = 0;
		p = rx*rx - ry*ry*rx + ry*ry/4;
		ddy = 0;
		ddx = 2*ry*ry*rx;
		while (True)
		{
			yy++;
			ddy += 2*rx*rx;
			if (p < 0)
				p += ddy + rx*rx;
			else
			{
				xx--;
				ddx -= 2*ry*ry;
				p += ddy - ddx + rx*rx;
			}

			if (ddy > ddx) break;

			DRAW_ELLIPSE_PIXBLEND(+xx+cx, -yy   , DRAW_ELLIPSE_ARC0);
			DRAW_ELLIPSE_PIXBLEND(-xx   , -yy   , DRAW_ELLIPSE_ARC1);
			DRAW_ELLIPSE_PIXBLEND(-xx   , +yy+cy, DRAW_ELLIPSE_ARC2);
			DRAW_ELLIPSE_PIXBLEND(+xx+cx, +yy+cy, DRAW_ELLIPSE_ARC3);
		}
	}
}

// Invert ellipse
void DrawEllipseInv(int x, int y, int dx, int dy, int mask)
{
	// limit range
	if (dx < 2) dx = 2;
	if (dx > DRAW_ELLIPSE_MAXD) dx = DRAW_ELLIPSE_MAXD;
	if (dy < 2) dy = 2;
	if (dy > DRAW_ELLIPSE_MAXD) dy = DRAW_ELLIPSE_MAXD;

	// prepare
	int rx = dx/2;
	int ry = dy/2;
	int xx = 0;
	int yy = ry;
	sFrameBuffer* f = &FrameBuffer;
	int minx = f->minx;
	int maxx = f->maxx;
	int miny = f->miny;
	int maxy = f->maxy;

	// pointer to middle of the ellipse
	int wb = f->drawpitchpix;
	u32* d0 = f->drawbuf;
	u32* d;

	// odd diameters X and Y
	int cx = -1; // even correction X (on even diameter X do not draw middle point X)
	int cy = -1; // even correction Y (on even diameter Y do not draw middle point Y)
	if ((dy & 1) != 0) cy = 0;
	if ((dx & 1) != 0)
	{
		DRAW_ELLIPSE_PIXINV(0, -ry   , DRAW_ELLIPSE_ARC0);
		DRAW_ELLIPSE_PIXINV(0, +ry+cy, DRAW_ELLIPSE_ARC2);
		cx = 0;
	}

	// odd diameter Y
	if ((dy & 1) != 0)
	{
		DRAW_ELLIPSE_PIXINV(-rx   , 0, DRAW_ELLIPSE_ARC1);
		DRAW_ELLIPSE_PIXINV(+rx+cx, 0, DRAW_ELLIPSE_ARC3);
	}

	// draw top and bottom arcs
	int p = ry*ry - rx*rx*ry + rx*rx/4;
	int ddx = 0;
	int ddy = 2*rx*rx*ry;
	while (True)
	{
		xx++;
		ddx += 2*ry*ry;
		if (p < 0)
			p += ddx + ry*ry;
		else
		{
			yy--;
			ddy -= 2*rx*rx;
			p += ddx - ddy + ry*ry;
		}

		if (ddx > ddy) break;

		DRAW_ELLIPSE_PIXINV(+xx+cx, -yy   , DRAW_ELLIPSE_ARC0);
		DRAW_ELLIPSE_PIXINV(-xx   , -yy   , DRAW_ELLIPSE_ARC1);
		DRAW_ELLIPSE_PIXINV(-xx   , +yy+cy, DRAW_ELLIPSE_ARC2);
		DRAW_ELLIPSE_PIXINV(+xx+cx, +yy+cy, DRAW_ELLIPSE_ARC3);
	}

	// draw left and right arcs
	xx = rx;
	yy = 0;
	p = rx*rx - ry*ry*rx + ry*ry/4;
	ddy = 0;
	ddx = 2*ry*ry*rx;
	while (True)
	{
		yy++;
		ddy += 2*rx*rx;
		if (p < 0)
			p += ddy + rx*rx;
		else
		{
			xx--;
			ddx -= 2*ry*ry;
			p += ddy - ddx + rx*rx;
		}

		if (ddy > ddx) break;

		DRAW_ELLIPSE_PIXINV(+xx+cx, -yy   , DRAW_ELLIPSE_ARC0);
		DRAW_ELLIPSE_PIXINV(-xx   , -yy   , DRAW_ELLIPSE_ARC1);
		DRAW_ELLIPSE_PIXINV(-xx   , +yy+cy, DRAW_ELLIPSE_ARC2);
		DRAW_ELLIPSE_PIXINV(+xx+cx, +yy+cy, DRAW_ELLIPSE_ARC3);
	}
}

// ----------------------------------------------------------------------------
//                          Draw filled ellipse
// ----------------------------------------------------------------------------

//  dx ... diameter of ellipse in X direction, range 2..430 (X radius = dx/2)
//  dy ... diameter of ellipse in Y direction, range 2..430 (Y radius = dy/2)
//  mask ... hide parts of the ellipse with DRAW_ROUND_* (or their combination); use DRAW_ROUND_ALL or 0 to draw whole ellipse
//		DRAW_ROUND_NOTOP	= hide top part of the round
//		DRAW_ROUND_NOBOTTOM	= hide bottom part of the round
//		DRAW_ROUND_NOLEFT	= hide left part of the round
//		DRAW_ROUND_NORIGHT	= hide right part of the round
//		DRAW_ROUND_ALL	= draw whole round

// Draw filled ellipse (alpha 0=transparent, 255=opaque)
void DrawFillEllipse(int x, int y, int dx, int dy, int mask, u32 col)
{
	// limit range
	if (dx < 2) dx = 2;
	if (dx > DRAW_ELLIPSE_MAXD) dx = DRAW_ELLIPSE_MAXD;
	if (dy < 2) dy = 2;
	if (dy > DRAW_ELLIPSE_MAXD) dy = DRAW_ELLIPSE_MAXD;

	// prepare
	int rx = (dx+1)/2;
	int ry = (dy+1)/2;
	int x1 = -rx;
	int x2 = +rx;
	int y1 = -ry;
	int y2 = +ry;
	sFrameBuffer* f = &FrameBuffer;
	int wb = f->drawpitchpix;
	u32 *d0, *d;
	int r2, xx, yy;
	int rx2 = rx*rx;
	int ry2 = ry*ry;

	// hide parts of the ellipse
	DRAW_ROUND_HIDE();

	// get alpha
	u32 a = col >> 24;
	if (a == 0) return; // full transparent

	// even diameter correction
	int cx = 1 - (dx & 1);
	int cy = 1 - (dy & 1);
	x -= cx;
	y -= cy;
	x1 += cx;
	y1 += cy;	

	// clipping
	DRAW_ROUND_CLIP();

	// prepare buffer address
	d0 = &f->drawbuf[(x+x1) + (y+y1)*wb];
	r2 = (rx-1+cx)*(ry-1+cy);

	// draw ellipse
	r2 = rx2*ry2;

	// full opaque
	if (a == 255)
	{
		for (yy = y1; yy <= y2; yy++)
		{
			d = d0;
			for (xx = x1; xx <= x2; xx++)
			{
				if (xx*(xx-cx)*ry2 + yy*(yy-cy)*rx2 < r2) *d = col;
				d++;
			}
			d0 += wb;
		}
	}

	// transparent
	else
	{
		// prepare components for blending
		DRAW_BLEND_PREP();

		for (yy = y1; yy <= y2; yy++)
		{
			d = d0;
			for (xx = x1; xx <= x2; xx++)
			{
				if (xx*(xx-cx)*ry2 + yy*(yy-cy)*rx2 < r2)
				{
					// blend one pixel
					DRAW_BLEND_PIXEL();
				}
				d++;
			}
			d0 += wb;
		}
	}
}

// Invert filled ellipse
void DrawFillEllipseInv(int x, int y, int dx, int dy, int mask)
{
	// limit range
	if (dx < 2) dx = 2;
	if (dx > DRAW_ELLIPSE_MAXD) dx = DRAW_ELLIPSE_MAXD;
	if (dy < 2) dy = 2;
	if (dy > DRAW_ELLIPSE_MAXD) dy = DRAW_ELLIPSE_MAXD;

	// prepare
	int rx = (dx+1)/2;
	int ry = (dy+1)/2;
	int x1 = -rx;
	int x2 = +rx;
	int y1 = -ry;
	int y2 = +ry;
	sFrameBuffer* f = &FrameBuffer;
	int wb = f->drawpitchpix;
	u32 *d0, *d;
	int r2, xx, yy;
	int rx2 = rx*rx;
	int ry2 = ry*ry;

	// hide parts of the ellipse
	DRAW_ROUND_HIDE();

	// even diameter correction
	int cx = 1 - (dx & 1);
	int cy = 1 - (dy & 1);
	x -= cx;
	y -= cy;
	x1 += cx;
	y1 += cy;	

	// clipping
	DRAW_ROUND_CLIP();

	// prepare buffer address
	d0 = &f->drawbuf[(x+x1) + (y+y1)*wb];
	r2 = (rx-1+cx)*(ry-1+cy);

	// draw ellipse
	r2 = rx2*ry2;
	for (yy = y1; yy <= y2; yy++)
	{
		d = d0;
		for (xx = x1; xx <= x2; xx++)
		{
			if (xx*(xx-cx)*ry2 + yy*(yy-cy)*rx2 < r2) *d ^= DRAW_INV_VAL;
			d++;
		}
		d0 += wb;
	}
}

// ----------------------------------------------------------------------------
//                              Draw ring
// ----------------------------------------------------------------------------

//  dout ... outer diameter of the ring (outer radius = dout/2)
//  din ... inner diameter of the ring (inner radius = din/2)
//  mask ... hide parts of the ring with DRAW_ROUND_* (or their combination); use DRAW_ROUND_ALL or 0 to draw whole ring
//		DRAW_ROUND_NOTOP	= hide top part of the ring
//		DRAW_ROUND_NOBOTTOM	= hide bottom part of the ring
//		DRAW_ROUND_NOLEFT	= hide left part of the ring
//		DRAW_ROUND_NORIGHT	= hide right part of the ring
//		DRAW_ROUND_ALL		= draw whole ring

// Draw ring (alpha 0=transparent, 255=opaque)
void DrawRing(int x, int y, int dout, int din, int mask, u32 col)
{
	if (dout < 1) dout = 1;

	// nothing to draw
	if (din >= dout) return;

	// get alpha
	u32 a = col >> 24;
	if (a == 0) return; // full transparent

	// draw round
	if (din <= 0)
	{
		DrawRound(x, y, dout, mask, col);
		return;
	}

	// draw circle
	if (din == dout-1)
	{
		u8 mask2 = DRAW_CIRCLE_ALL;
		if ((mask & DRAW_ROUND_NOTOP) != 0) mask2 &= DRAW_CIRCLE_BOTTOM;
		if ((mask & DRAW_ROUND_NOBOTTOM) != 0) mask2 &= DRAW_CIRCLE_TOP;
		if ((mask & DRAW_ROUND_NOLEFT) != 0) mask2 &= DRAW_CIRCLE_RIGHT;
		if ((mask & DRAW_ROUND_NORIGHT) != 0) mask2 &= DRAW_CIRCLE_LEFT;
		DrawCircle(x, y, dout, mask2, col);
		return;
	}

	// prepare
	int r = (dout+1)/2;
	int rin = (din+1)/2;
	int x1 = -r;
	int x2 = +r;
	int y1 = -r;
	int y2 = +r;
	sFrameBuffer* f = &FrameBuffer;
	int wb = f->drawpitchpix;
	u32 *d0, *d;
	int r2, rin2;

	// hide parts of the ring
	DRAW_ROUND_HIDE();

	// full opaque
	if (a == 255)
	{
		// odd diameter (1, 3, 5, ...)
		if ((dout & 1) != 0)
		{
			// clipping
			DRAW_ROUND_CLIP();

			// prepare buffer address
			d0 = &f->drawbuf[(x+x1) + (y+y1)*wb];
			r2 = r*(r-1);
			rin2 = rin*(rin-1);
			if (dout == 3) r2--;
			if (din == 3) rin2--;

			// draw ring
			for (y = y1; y <= y2; y++)
			{
				d = d0;
				for (x = x1; x <= x2; x++)
				{
					r = x*x + y*y;
					if ((r <= r2) && (r > rin2)) *d = col;
					d++;
				}
				d0 += wb;
			}
		}

		// even diameter (2, 4, 6, ...)
		else
		{
			// to correspond to the center of DrawCircle [x0,y0] = [x+d/2,y+d/2]
			x--;
			y--;
			x1++;
			y1++;

			// clipping
			DRAW_ROUND_CLIP();

			// prepare buffer address
			d0 = &f->drawbuf[(x+x1) + (y+y1)*wb];
			r2 = r*r;
			rin2 = rin*rin;

			// draw round
			for (y = y1; y <= y2; y++)
			{
				d = d0;
				for (x = x1; x <= x2; x++)
				{
					r = x*(x-1) + y*(y-1);
					if ((r < r2) && (r >= rin2)) *d = col;
					d++;
				}
				d0 += wb;
			}
		}
	}

	// transparency
	else
	{
		// prepare components for blending
		DRAW_BLEND_PREP();

		// odd diameter (1, 3, 5, ...)
		if ((dout & 1) != 0)
		{
			// clipping
			DRAW_ROUND_CLIP();

			// prepare buffer address
			d0 = &f->drawbuf[(x+x1) + (y+y1)*wb];
			r2 = r*(r-1);
			rin2 = rin*(rin-1);
			if (dout == 3) r2--;
			if (din == 3) rin2--;

			// draw ring
			for (y = y1; y <= y2; y++)
			{
				d = d0;
				for (x = x1; x <= x2; x++)
				{
					r = x*x + y*y;
					if ((r <= r2) && (r > rin2))
					{
						// blending pixel
						DRAW_BLEND_PIXEL();
					}
					d++;
				}
				d0 += wb;
			}
		}

		// even diameter (2, 4, 6, ...)
		else
		{
			// to correspond to the center of DrawCircle [x0,y0] = [x+d/2,y+d/2]
			x--;
			y--;
			x1++;
			y1++;

			// clipping
			DRAW_ROUND_CLIP();

			// prepare buffer address
			d0 = &f->drawbuf[(x+x1) + (y+y1)*wb];
			r2 = r*r;
			rin2 = rin*rin;

			// draw round
			for (y = y1; y <= y2; y++)
			{
				d = d0;
				for (x = x1; x <= x2; x++)
				{
					r = x*(x-1) + y*(y-1);
					if ((r < r2) && (r >= rin2))
					{
						// blending pixel
						DRAW_BLEND_PIXEL();
					}
					d++;
				}
				d0 += wb;
			}
		}
	}
}

// Invert ring
void DrawRingInv(int x, int y, int dout, int din, int mask)
{
	if (dout < 1) dout = 1;

	// nothing to draw
	if (din >= dout) return;

	// draw round
	if (din <= 0)
	{
		DrawRoundInv(x, y, dout, mask);
		return;
	}

	// draw circle
	if (din == dout-1)
	{
		u8 mask2 = DRAW_CIRCLE_ALL;
		if ((mask & DRAW_ROUND_NOTOP) != 0) mask2 &= DRAW_CIRCLE_BOTTOM;
		if ((mask & DRAW_ROUND_NOBOTTOM) != 0) mask2 &= DRAW_CIRCLE_TOP;
		if ((mask & DRAW_ROUND_NOLEFT) != 0) mask2 &= DRAW_CIRCLE_RIGHT;
		if ((mask & DRAW_ROUND_NORIGHT) != 0) mask2 &= DRAW_CIRCLE_LEFT;
		DrawCircleInv(x, y, dout, mask2);
		return;
	}

	// prepare
	int r = (dout+1)/2;
	int rin = (din+1)/2;
	int x1 = -r;
	int x2 = +r;
	int y1 = -r;
	int y2 = +r;
	sFrameBuffer* f = &FrameBuffer;
	int wb = f->drawpitchpix;
	u32 *d0, *d;
	int r2, rin2;

	// hide parts of the ring
	DRAW_ROUND_HIDE();

	// odd diameter (1, 3, 5, ...)
	if ((dout & 1) != 0)
	{
		// clipping
		DRAW_ROUND_CLIP();

		// prepare buffer address
		d0 = &f->drawbuf[(x+x1) + (y+y1)*wb];
		r2 = r*(r-1);
		rin2 = rin*(rin-1);
		if (dout == 3) r2--;
		if (din == 3) rin2--;

		// draw ring
		for (y = y1; y <= y2; y++)
		{
			d = d0;
			for (x = x1; x <= x2; x++)
			{
				r = x*x + y*y;
				if ((r <= r2) && (r > rin2)) *d ^= DRAW_INV_VAL;
				d++;
			}
			d0 += wb;
		}
	}

	// even diameter (2, 4, 6, ...)
	else
	{
		// to correspond to the center of DrawCircle [x0,y0] = [x+d/2,y+d/2]
		x--;
		y--;
		x1++;
		y1++;

		// clipping
		DRAW_ROUND_CLIP();

		// prepare buffer address
		d0 = &f->drawbuf[(x+x1) + (y+y1)*wb];
		r2 = r*r;
		rin2 = rin*rin;

		// draw round
		for (y = y1; y <= y2; y++)
		{
			d = d0;
			for (x = x1; x <= x2; x++)
			{
				r = x*(x-1) + y*(y-1);
				if ((r < r2) && (r >= rin2)) *d ^= DRAW_INV_VAL;
				d++;
			}
			d0 += wb;
		}
	}
}

// ----------------------------------------------------------------------------
//                              Draw triangle
// ----------------------------------------------------------------------------

// draw horizontal line
#define DRAW_TRIANGLE_HLINE(xx)		{	\
	d = &f->drawbuf[(xx) + y*f->drawpitchpix];	\
	for (; k > 0; k--) *d++ = col; }

// draw horizontal line transparent
#define DRAW_TRIANGLE_HLINE_BLEND(xx)	{	\
	d = &f->drawbuf[(xx) + y*f->drawpitchpix];	\
	for (; k > 0; k--)			\
		{ DRAW_BLEND_PIXEL(); d++; } }

// invert horizontal line
#define DRAW_TRIANGLE_HLINE_INV(xx)	{	\
	d = &f->drawbuf[(xx) + y*f->drawpitchpix];	\
	for (; k > 0; k--)			\
		{ *d ^= DRAW_INV_VAL; d++; } }

// Draw triangle (alpha 0=transparent, 255=opaque)
void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, u32 col)
{
	int x, y, k, xmin, xmax;

	// sort coordinates by Y (order from top to bottom: 1 / 2 / 3)
	if (y2 < y1)
	{
		k = x2; x2 = x1; x1 = k;
		k = y2; y2 = y1; y1 = k;
	}

	if (y3 < y1)
	{
		k = x3; x3 = x1; x1 = k;
		k = y3; y3 = y1; y1 = k;
	}
			
	if (y3 < y2)
	{
		k = x3; x3 = x2; x2 = k;
		k = y3; y3 = y2; y2 = k;
	}

	// get alpha
	u32 a = col >> 24;
	if (a == 0) return; // full transparent

	// prepare clipping
	sFrameBuffer* f = &FrameBuffer;
	int minx = f->minx;
	int maxx = f->maxx;
	int miny = f->miny;
	int maxy = f->maxy;

	// top sub-triangle y1 <= y < y2 (without bottom y2)
	y = y1;
	if (y < miny) y = miny;
	int ymax = y2;
	if (ymax > maxy) ymax = maxy;

	// full transparency
	u32* d;
	if (a == 255)
	{
		for (; y < ymax; y++)
		{
			xmin = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
			xmax = x1 + (x3 - x1) * (y - y1) / (y3 - y1);

			if (xmax < xmin)
			{
				if (xmax < minx) xmax = minx;
				if (xmin >= maxx) xmin = maxx-1;
				k = xmin - xmax + 1;
				DRAW_TRIANGLE_HLINE(xmax);
			}
			else
			{
				if (xmin < minx) xmin = minx;
				if (xmax >= maxx) xmax = maxx-1;
				k = xmax - xmin + 1;
				DRAW_TRIANGLE_HLINE(xmin);
			}
		}

		// bottom y2 == y3
		if (y2 == y3)
		{
			if ((y2 >= miny) && (y2 < maxy))
			{
				xmin = x2;
				xmax = x3;

				if (xmax < xmin)
				{
					if (xmax < minx) xmax = minx;
					if (xmin >= maxx) xmin = maxx-1;
					k = xmin - xmax + 1;
					DRAW_TRIANGLE_HLINE(xmax);
				}
				else
				{
					if (xmin < minx) xmin = minx;
					if (xmax >= maxx) xmax = maxx-1;
					k = xmax - xmin + 1;
					DRAW_TRIANGLE_HLINE(xmin);
				}
			}
		}

		// bottom sub-triangle y2 <= y <= y3
		else
		{
			y = y2;
			if (y < miny) y = miny;
			ymax = y3;
			if (ymax >= maxy) ymax = maxy-1;
			for (; y <= ymax; y++)
			{
				xmin = x2 + (x3 - x2) * (y - y2) / (y3 - y2);
				xmax = x1 + (x3 - x1) * (y - y1) / (y3 - y1);

				if (xmax < xmin)
				{
					if (xmax < minx) xmax = minx;
					if (xmin >= maxx) xmin = maxx-1;
					k = xmin - xmax + 1;
					DRAW_TRIANGLE_HLINE(xmax);
				}
				else
				{
					if (xmin < minx) xmin = minx;
					if (xmax >= maxx) xmax = maxx-1;
					k = xmax - xmin + 1;
					DRAW_TRIANGLE_HLINE(xmin);
				}
			}
		}
	}

	// transparency
	else
	{
		// prepare components for blending
		DRAW_BLEND_PREP();

		for (; y < ymax; y++)
		{
			xmin = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
			xmax = x1 + (x3 - x1) * (y - y1) / (y3 - y1);

			if (xmax < xmin)
			{
				if (xmax < minx) xmax = minx;
				if (xmin >= maxx) xmin = maxx-1;
				k = xmin - xmax + 1;
				DRAW_TRIANGLE_HLINE_BLEND(xmax);
			}
			else
			{
				if (xmin < minx) xmin = minx;
				if (xmax >= maxx) xmax = maxx-1;
				k = xmax - xmin + 1;
				DRAW_TRIANGLE_HLINE_BLEND(xmin);
			}
		}

		// bottom y2 == y3
		if (y2 == y3)
		{
			if ((y2 >= miny) && (y2 < maxy))
			{
				xmin = x2;
				xmax = x3;

				if (xmax < xmin)
				{
					if (xmax < minx) xmax = minx;
					if (xmin >= maxx) xmin = maxx-1;
					k = xmin - xmax + 1;
					DRAW_TRIANGLE_HLINE_BLEND(xmax);
				}
				else
				{
					if (xmin < minx) xmin = minx;
					if (xmax >= maxx) xmax = maxx-1;
					k = xmax - xmin + 1;
					DRAW_TRIANGLE_HLINE_BLEND(xmin);
				}
			}
		}

		// bottom sub-triangle y2 <= y <= y3
		else
		{
			y = y2;
			if (y < miny) y = miny;
			ymax = y3;
			if (ymax >= maxy) ymax = maxy-1;
			for (; y <= ymax; y++)
			{
				xmin = x2 + (x3 - x2) * (y - y2) / (y3 - y2);
				xmax = x1 + (x3 - x1) * (y - y1) / (y3 - y1);

				if (xmax < xmin)
				{
					if (xmax < minx) xmax = minx;
					if (xmin >= maxx) xmin = maxx-1;
					k = xmin - xmax + 1;
					DRAW_TRIANGLE_HLINE_BLEND(xmax);
				}
				else
				{
					if (xmin < minx) xmin = minx;
					if (xmax >= maxx) xmax = maxx-1;
					k = xmax - xmin + 1;
					DRAW_TRIANGLE_HLINE_BLEND(xmin);
				}
			}
		}
	}
}

// Invert triangle
void DrawTriangleInv(int x1, int y1, int x2, int y2, int x3, int y3)
{
	int x, y, k, xmin, xmax;

	// sort coordinates by Y (order from top to bottom: 1 / 2 / 3)
	if (y2 < y1)
	{
		k = x2; x2 = x1; x1 = k;
		k = y2; y2 = y1; y1 = k;
	}

	if (y3 < y1)
	{
		k = x3; x3 = x1; x1 = k;
		k = y3; y3 = y1; y1 = k;
	}
			
	if (y3 < y2)
	{
		k = x3; x3 = x2; x2 = k;
		k = y3; y3 = y2; y2 = k;
	}

	// prepare clipping
	sFrameBuffer* f = &FrameBuffer;
	int minx = f->minx;
	int maxx = f->maxx;
	int miny = f->miny;
	int maxy = f->maxy;

	// top sub-triangle y1 <= y < y2 (without bottom y2)
	y = y1;
	if (y < miny) y = miny;
	int ymax = y2;
	if (ymax > maxy) ymax = maxy;

	u32* d;
	for (; y < ymax; y++)
	{
		xmin = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
		xmax = x1 + (x3 - x1) * (y - y1) / (y3 - y1);

		if (xmax < xmin)
		{
			if (xmax < minx) xmax = minx;
			if (xmin >= maxx) xmin = maxx-1;
			k = xmin - xmax + 1;
			DRAW_TRIANGLE_HLINE_INV(xmax);
		}
		else
		{
			if (xmin < minx) xmin = minx;
			if (xmax >= maxx) xmax = maxx-1;
			k = xmax - xmin + 1;
			DRAW_TRIANGLE_HLINE_INV(xmin);
		}
	}

	// bottom y2 == y3
	if (y2 == y3)
	{
		if ((y2 >= miny) && (y2 < maxy))
		{
			xmin = x2;
			xmax = x3;

			if (xmax < xmin)
			{
				if (xmax < minx) xmax = minx;
				if (xmin >= maxx) xmin = maxx-1;
				k = xmin - xmax + 1;
				DRAW_TRIANGLE_HLINE_INV(xmax);
			}
			else
			{
				if (xmin < minx) xmin = minx;
				if (xmax >= maxx) xmax = maxx-1;
				k = xmax - xmin + 1;
				DRAW_TRIANGLE_HLINE_INV(xmin);
			}
		}
	}

	// bottom sub-triangle y2 <= y <= y3
	else
	{
		y = y2;
		if (y < miny) y = miny;
		ymax = y3;
		if (ymax >= maxy) ymax = maxy-1;
		for (; y <= ymax; y++)
		{
			xmin = x2 + (x3 - x2) * (y - y2) / (y3 - y2);
			xmax = x1 + (x3 - x1) * (y - y1) / (y3 - y1);

			if (xmax < xmin)
			{
				if (xmax < minx) xmax = minx;
				if (xmin >= maxx) xmin = maxx-1;
				k = xmin - xmax + 1;
				DRAW_TRIANGLE_HLINE_INV(xmax);
			}
			else
			{
				if (xmin < minx) xmin = minx;
				if (xmax >= maxx) xmax = maxx-1;
				k = xmax - xmin + 1;
				DRAW_TRIANGLE_HLINE_INV(xmin);
			}
		}
	}
}

// ----------------------------------------------------------------------------
//                              Draw fill area
// ----------------------------------------------------------------------------

// Draw fill sub-area (internal function)
void _DrawSubFill(int x, int y, u32 fnd, u32 col)
{
	sFrameBuffer* f = &FrameBuffer;
	int wb = f->drawpitchpix;

	// prepare buffer address
	u32* d = &f->drawbuf[x + y*wb];

	// save start position -> x2, s2
	int x2 = x;
	u32* d2 = d;

	// fill start point
	*d = col;

	// search start of segment in LEFT-X direction -> x1, s1
	int minx = f->minx;
	while ((x > minx) && ((d[-1] | 0xff000000) == fnd))
	{
		x--;
		d--;
		*d = col;
	}
	int x1 = x;
	u32* d1 = d;

	// search end of segment in RIGHT-X direction -> x2, s2
	int maxx = f->maxx-1;
	while ((x2 < maxx) && ((d2[1] | 0xff000000) == fnd))
	{
		x2++;
		d2++;
		*d2 = col;
	}

	// search segments in UP-Y direction
	if (y > f->miny)
	{
		x = x1;
		d = d1 - wb;
		y--;
		while (x <= x2)
		{
			if ((*d | 0xff000000) == fnd) _DrawSubFill(x, y, fnd, col);
			x++;
			d++;
		}
		y++;
	}

	// search segments in DOWN-Y direction
	if (y < f->maxy-1)
	{
		x = x1;
		d = d1 + wb;
		y++;
		while (x <= x2)
		{
			if ((*d | 0xff000000) == fnd) _DrawSubFill(x, y, fnd, col);
			x++;
			d++;
		}
	}
}

// Draw fill area (no blending)
void DrawFill(int x, int y, u32 col)
{
	// check clipping
	sFrameBuffer* f = &FrameBuffer;
	if ((x < f->minx) || (x >= f->maxx) || (y < f->miny) || (y >= f->maxy)) return;

	// get color to search
	u32 fnd = DrawGetPoint(x, y) | 0xff000000;

	// fill if color is different
	col |= 0xff000000;
	if (fnd != col) _DrawSubFill(x, y, fnd, col);
}

// ----------------------------------------------------------------------------
//                              Draw image
// ----------------------------------------------------------------------------

// limit image coordinates
#define DRAW_IMG_LIMIT() {				\
	/* limit X */					\
	if (xs < 0) { w += xs; x -= xs; xs = 0; }	\
	int k = f->minx - x;				\
	if (k > 0) { w -= k; xs += k; x += k; }		\
	/* limit w */					\
	k = f->maxx - x; if (w > k) w = k;		\
	ws = pic->w;					\
	k = ws - xs; if (w > k) w = k;			\
	if (w <= 0) return;				\
	/* limit y */					\
	if (ys < 0) { h += ys; y -= ys; ys = 0; }	\
	k = f->miny - y;				\
	if (k > 0) { h -= k; ys += k; y += k; }		\
	/* limit h */					\
	k = f->maxy - y; if (h > k) h = k;		\
	hs = pic->h;					\
	k = hs - ys; if (h > k) h = k;			\
	if (h <= 0) return; }


// Draw image (image must be in aligned CF_A8B8G8R8 or CF_B8G8R8 format; alpha 0=transparent, 255=opaque)
void DrawImg(const u8* img, int x, int y, int xs, int ys, int w, int h, int alpha)
{
	int ws, hs, i;

	// check source alpha
	if (alpha <= 0) return;
	if (alpha > 255) alpha = 255;

	// source image
	const sPic* pic = (const sPic*)img;
	if ((pic->colfmt != CF_A8B8G8R8) && (pic->colfmt != CF_B8G8R8)) return;

	// frame buffer
	sFrameBuffer* f = &FrameBuffer;

	// limit coordinates
	DRAW_IMG_LIMIT();

	// destination address
	int wb = f->drawpitchpix;
	u32* d = &f->drawbuf[x + y*wb];
	wb -= w;

	// 24-bit format without alpha
	if (pic->colfmt == CF_B8G8R8)
	{
		// source address
		int wbs = pic->wb;
		const u8* s = pic->data + xs*3 + ys*wbs;
		wbs -= w*3;

		// full opaque
		if (alpha == 255)
		{
			for (; h > 0; h--)
			{
				for (i = w; i > 0; i--)
				{
					*d = s[0] | ((u32)s[1] << 8) | ((u32)s[2] << 16) | 0xff000000;
					d++;
					s += 3;
				}
				d += wb;
				s += wbs;
			}
		}

		// draw with blending
		else
		{
			u32 inv = 255 - alpha;

			for (; h > 0; h--)
			{
				for (i = w; i > 0; i--)
				{
					// get source pixel
					u8 red = s[0];
					u8 green = s[1];
					u8 blue = s[2];
					s += 3;
			
					u32 srb = (red | ((u32)blue << 16))*alpha + 0x00ff00ff;
					u32 sg = ((u32)green << 8)*alpha + 0x0000ff00;
					u32 dst = *d;
					u32 rb = (dst & 0x00ff00ff);
					u32 g = (dst & 0x0000ff00);
					rb = (srb + rb*inv) & 0xff00ff00;
					g = (sg + g*inv) & 0x00ff0000;
					*d = ((rb | g) >> 8) | 0xff000000;
					d++;
				}
				d += wb;
				s += wbs;
			}
		}
	}

	// 32-bit format CF_A8B8G8R8 with Alpha
	else
	{
		// source address
		int wbs = pic->wb/4;
		const u32* s = ((const u32*)pic->data) + xs + ys*wbs;
		wbs -= w;

		// draw with blending
		for (; h > 0; h--)
		{
			for (i = w; i > 0; i--)
			{
				// get source pixel
				u32 col = *s++;
			
				// get source alpha
				u32 a = col >> 24;
				a *= alpha;
				a = DIV255(a);

				// full opaque
				if (a == 255)
				{
					*d = col;
				}
				else if (a != 0)
				{
					u32 inv = 255 - a;
					u32 srb = (col & 0x00ff00ff)*a + 0x00ff00ff;
					u32 sg = (col & 0x0000ff00)*a + 0x0000ff00;
					u32 dst = *d;
					u32 rb = (dst & 0x00ff00ff);
					u32 g = (dst & 0x0000ff00);
					rb = (srb + rb*inv) & 0xff00ff00;
					g = (sg + g*inv) & 0x00ff0000;
					*d = ((rb | g) >> 8) | 0xff000000;
				}
				d++;
			}
			d += wb;
			s += wbs;
		}
	}
}

// Invert image (image must be in aligned CF_A8B8G8R8 or CF_B8G8R8 format)
void DrawImgInv(const u8* img, int x, int y, int xs, int ys, int w, int h)
{
	int ws, hs, i;

	// source image
	const sPic* pic = (const sPic*)img;
	if ((pic->colfmt != CF_A8B8G8R8) && (pic->colfmt != CF_B8G8R8)) return;

	// frame buffer
	sFrameBuffer* f = &FrameBuffer;

	// limit coordinates
	DRAW_IMG_LIMIT();

	// destination address
	int wb = f->drawpitchpix;
	u32* d = &f->drawbuf[x + y*wb];
	wb -= w;

	// 24-bit format 
	if (pic->colfmt == CF_B8G8R8)
	{
		// source address
		int wbs = pic->wb;
		const u8* s = pic->data + xs*3 + ys*wbs;
		wbs -= w*3;

		// draw
		for (; h > 0; h--)
		{
			for (i = w; i > 0; i--)
			{
				// get source pixel
				u32 col = s[0] | ((u32)s[1] << 8) | ((u32)s[2] << 16);
				s += 3;

				// flip colors
				*d ^= col;
				d++;
			}
			d += wb;
			s += wbs;
		}
	}

	// 32-bit format CF_A8B8G8R8 with Alpha
	else
	{
		// source address
		int wbs = pic->wb/4;
		const u32* s = ((const u32*)pic->data) + xs + ys*wbs;
		wbs -= w;

		// draw image
		for (; h > 0; h--)
		{
			for (i = w; i > 0; i--)
			{
				*d ^= *s & 0x00ffffff;
				d++;
				s++;
			}
			d += wb;
			s += wbs;
		}
	}
}

// Draw image mask (image must be in CF_A8 format; alpha 0=transparent, 255=opaque)
void DrawImgMask(const u8* img, int x, int y, int xs, int ys, int w, int h, u32 col)
{
	int ws, hs, i;

	// get alpha
	u32 alpha = col >> 24;
	if (alpha == 0) return; // full transparent

	// source image
	const sPic* pic = (const sPic*)img;
	if (pic->colfmt != CF_A8) return;

	// frame buffer
	sFrameBuffer* f = &FrameBuffer;

	// limit coordinates
	DRAW_IMG_LIMIT();

	// destination address
	int wb = f->drawpitchpix;
	u32* d = &f->drawbuf[x + y*wb];
	wb -= w;

	// source address
	int wbs = pic->wb;
	const u8* s = pic->data + xs + ys*wbs;
	wbs -= w;

	// prepare source color
	u32 srb = col & 0x00ff00ff;
	u32 sg = col & 0x0000ff00;
	col |= 0xff000000;

	// draw with blending
	for (; h > 0; h--)
	{
		for (i = w; i > 0; i--)
		{
			// get source alpha
			u32 a = *s++;
			a *= alpha;
			a = DIV255(a);

			// full opaque
			if (a == 255)
			{
				*d = col;
			}
			else if (a != 0)
			{
				u32 inv = 255 - a;
				u32 srb2 = srb*a + 0x00ff00ff;
				u32 sg2 = sg*a + 0x0000ff00;
				u32 dst = *d;
				u32 rb = (dst & 0x00ff00ff);
				u32 g = (dst & 0x0000ff00);
				rb = (srb2 + rb*inv) & 0xff00ff00;
				g = (sg2 + g*inv) & 0x00ff0000;
				*d = ((rb | g) >> 8) | 0xff000000;
			}
			d++;
		}
		d += wb;
		s += wbs;
	}
}

// Invert image mask (image must be in CF_A8 format)
void DrawImgMaskInv(const u8* img, int x, int y, int xs, int ys, int w, int h)
{
	int ws, hs, i;

	// source image
	const sPic* pic = (const sPic*)img;
	if (pic->colfmt != CF_A8) return;

	// frame buffer
	sFrameBuffer* f = &FrameBuffer;

	// limit coordinates
	DRAW_IMG_LIMIT();

	// destination address
	int wb = f->drawpitchpix;
	u32* d = &f->drawbuf[x + y*wb];
	wb -= w;

	// source address
	int wbs = pic->wb;
	const u8* s = pic->data + xs + ys*wbs;
	wbs -= w;

	// invert
	for (; h > 0; h--)
	{
		for (i = w; i > 0; i--)
		{
			// get source alpha
			u8 a = *s++;
			if (a >= 128) *d ^= 0x00ffffff;
			d++;
		}
		d += wb;
		s += wbs;
	}
}

// ----------------------------------------------------------------------------
//                              Draw character
// ----------------------------------------------------------------------------

// draw character using system fixed font (alpha 0=transparent, 255=opaque; use DrawSelFont() to select font)
// - To invert text, use bit 7 of the characters, or FrameBuffer.inv flag (flags are XORed).
void DrawCharScale(u8 ch, int x, int y, int scalex, int scaley, u32 col)
{
	int sx, sy;
	int b, m, i, x2, dwb;
	u32* d;

	// get alpha
	u32 a = col >> 24;
	if (a == 0) return;	// full transparent

	// get clipping
	sFrameBuffer* f = &FrameBuffer;
	int minx = f->minx;
	int maxx = f->maxx;
	int miny = f->miny;
	int maxy = f->maxy;

	// prepare inversion mask
	u32 binv = f->inv ? (~0) : 0;
	if (ch >= 0x80)
	{
		ch &= 0x7f;
		binv = ~binv;
	}

	// pointer to the font
	const u8* s = &f->font[ch];

	// prepare destination (at this place, it can be out of screen)
	int wb = f->drawpitchpix;

	// font size
	int w = f->fontw;
	int h = f->fonth;

	// full opaque
	if (a == 255)
	{
		// loop through font height
		for (; h > 0; h--)
		{
			// loop through scale Y
			for (sy = scaley; sy > 0; sy--)
			{
				// check if Y is valid
				if ((y >= miny) && (y < maxy))
				{
					// destination
					d = &f->drawbuf[x + y*wb];

					// load font sample
					b = *s ^ binv;

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
								if ((x2 >= minx) && (x2 < maxx))
								{
									// draw pixel
									*d = col;
								}

								// shift X
								x2++;
								d++;
							}
						}
						else
						{
							x2 += scalex;
							d += scalex;
						}

						// shift 
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

	// transparent
	else
	{
		// prepare components for blending
		DRAW_BLEND_PREP();

		// loop through font height
		for (; h > 0; h--)
		{
			// loop through scale Y
			for (sy = scaley; sy > 0; sy--)
			{
				// check if Y is valid
				if ((y >= miny) && (y < maxy))
				{
					// destination
					d = &f->drawbuf[x + y*wb];

					// load font sample
					b = *s ^ binv;

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
								if ((x2 >= minx) && (x2 < maxx))
								{
									// blend pixel
									DRAW_BLEND_PIXEL();
								}

								// shift X
								x2++;
								d++;
							}
						}
						else
						{
							x2 += scalex;
							d += scalex;
						}

						// shift 
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
}

void DrawChar(u8 ch, int x, int y, u32 col) { DrawCharScale(ch, x, y, 1, 1, col); }
void DrawCharW(u8 ch, int x, int y, u32 col) { DrawCharScale(ch, x, y, 2, 1, col); }
void DrawCharH(u8 ch, int x, int y, u32 col) { DrawCharScale(ch, x, y, 1, 2, col); }
void DrawChar2(u8 ch, int x, int y, u32 col) { DrawCharScale(ch, x, y, 2, 2, col); }
void DrawChar3(u8 ch, int x, int y, u32 col) { DrawCharScale(ch, x, y, 3, 3, col); }
void DrawChar4(u8 ch, int x, int y, u32 col) { DrawCharScale(ch, x, y, 4, 4, col); }

// draw character with background, using system fixed font (no alpha transparency; use DrawSelFont() to select font)
// - To invert text, use bit 7 of the characters, or FrameBuffer.inv flag (flags are XORed).
void DrawCharBgScale(u8 ch, int x, int y, int scalex, int scaley, u32 colfg, u32 colbg)
{
	int sx, sy;
	int b, m, i, x2, dwb;
	u32* d;

	// get clipping
	sFrameBuffer* f = &FrameBuffer;
	int minx = f->minx;
	int maxx = f->maxx;
	int miny = f->miny;
	int maxy = f->maxy;

	// prepare inversion mask
	u32 binv = f->inv ? (~0) : 0;
	if (ch >= 0x80)
	{
		ch &= 0x7f;
		binv = ~binv;
	}

	// pointer to the font
	const u8* s = &f->font[ch];

	// prepare destination (at this place, it can be out of screen)
	int wb = f->drawpitchpix;

	// font size
	int w = f->fontw;
	int h = f->fonth;

	// loop through font height
	for (; h > 0; h--)
	{
		// loop through scale Y
		for (sy = scaley; sy > 0; sy--)
		{
			// check if Y is valid
			if ((y >= miny) && (y < maxy))
			{
				// destination
				d = &f->drawbuf[x + y*wb];

				// load font sample
				b = *s ^ binv;

				// loop through font width
				x2 = x;
				for (i = w; i > 0; i--)
				{
					// loop through scale X
					for (sx = scalex; sx > 0; sx--)
					{
						// check if X is valid
						if ((x2 >= minx) && (x2 < maxx))
						{
							// draw pixel
							*d = ((b & B7) == 0) ? colfg : colbg;
						}

						// shift X
						x2++;
						d++;
					}

					// shift 
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

void DrawCharBg(u8 ch, int x, int y, u32 colfg, u32 colbg) { DrawCharBgScale(ch, x, y, 1, 1, colfg, colbg); }
void DrawCharBgW(u8 ch, int x, int y, u32 colfg, u32 colbg) { DrawCharBgScale(ch, x, y, 2, 1, colfg, colbg); }
void DrawCharBgH(u8 ch, int x, int y, u32 colfg, u32 colbg) { DrawCharBgScale(ch, x, y, 1, 2, colfg, colbg); }
void DrawCharBg2(u8 ch, int x, int y, u32 colfg, u32 colbg) { DrawCharBgScale(ch, x, y, 2, 2, colfg, colbg); }
void DrawCharBg3(u8 ch, int x, int y, u32 colfg, u32 colbg) { DrawCharBgScale(ch, x, y, 3, 3, colfg, colbg); }
void DrawCharBg4(u8 ch, int x, int y, u32 colfg, u32 colbg) { DrawCharBgScale(ch, x, y, 4, 4, colfg, colbg); }

// invert character using system fixed font (use DrawSelFont() to select font)
// - To invert text, use bit 7 of the characters, or FrameBuffer.inv flag (flags are XORed).
void DrawCharScaleInv(u8 ch, int x, int y, int scalex, int scaley)
{
	int sx, sy;
	int b, m, i, x2, dwb;
	u32* d;

	// get clipping
	sFrameBuffer* f = &FrameBuffer;
	int minx = f->minx;
	int maxx = f->maxx;
	int miny = f->miny;
	int maxy = f->maxy;

	// prepare inversion mask
	u32 binv = f->inv ? (~0) : 0;
	if (ch >= 0x80)
	{
		ch &= 0x7f;
		binv = ~binv;
	}

	// pointer to the font
	const u8* s = &f->font[ch];

	// prepare destination (at this place, it can be out of screen)
	int wb = f->drawpitchpix;

	// font size
	int w = f->fontw;
	int h = f->fonth;

	// loop through font height
	for (; h > 0; h--)
	{
		// loop through scale Y
		for (sy = scaley; sy > 0; sy--)
		{
			// check if Y is valid
			if ((y >= miny) && (y < maxy))
			{
				// destination
				d = &f->drawbuf[x + y*wb];

				// load font sample
				b = *s ^ binv;

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
							if ((x2 >= minx) && (x2 < maxx))
							{
								// draw pixel
								*d ^= 0x00ffffff;
							}

							// shift X
							x2++;
							d++;
						}
					}
					else
					{
						x2 += scalex;
						d += scalex;
					}

					// shift 
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

void DrawCharInv(u8 ch, int x, int y) { DrawCharScaleInv(ch, x, y, 1, 1); }
void DrawCharWInv(u8 ch, int x, int y) { DrawCharScaleInv(ch, x, y, 2, 1); }
void DrawCharHInv(u8 ch, int x, int y) { DrawCharScaleInv(ch, x, y, 1, 2); }
void DrawChar2Inv(u8 ch, int x, int y) { DrawCharScaleInv(ch, x, y, 2, 2); }
void DrawChar3Inv(u8 ch, int x, int y) { DrawCharScaleInv(ch, x, y, 3, 3); }
void DrawChar4Inv(u8 ch, int x, int y) { DrawCharScaleInv(ch, x, y, 4, 4); }

// draw text with length, using system fixed font (alpha 0=transparent, 255=opaque; use DrawSelFont() to select font)
// - To invert text, use bit 7 of the characters, or FrameBuffer.inv flag (flags are XORed).
// - Returns shift of X in pixels.
int DrawTextLenScale(const char* text, int len, int x, int y, int scalex, int scaley, u32 col)
{
	int dx = FrameBuffer.fontw*scalex;
	int res = 0;
	for (; len > 0; len--)
	{
		DrawCharScale(*text++, x, y, scalex, scaley, col);
		x += dx;
		res += dx;
	}
	return res;
}

int DrawTextLen(const char* text, int len, int x, int y, u32 col) { return DrawTextLenScale(text, len, x, y, 1, 1, col); }
int DrawTextLenW(const char* text, int len, int x, int y, u32 col) { return DrawTextLenScale(text, len, x, y, 2, 1, col); }
int DrawTextLenH(const char* text, int len, int x, int y, u32 col) { return DrawTextLenScale(text, len, x, y, 1, 2, col); }
int DrawTextLen2(const char* text, int len, int x, int y, u32 col) { return DrawTextLenScale(text, len, x, y, 2, 2, col); }
int DrawTextLen3(const char* text, int len, int x, int y, u32 col) { return DrawTextLenScale(text, len, x, y, 3, 3, col); }
int DrawTextLen4(const char* text, int len, int x, int y, u32 col) { return DrawTextLenScale(text, len, x, y, 4, 4, col); }

// invert text with length, using system fixed font (use DrawSelFont() to select font)
// - To invert text, use bit 7 of the characters, or FrameBuffer.inv flag (flags are XORed).
// - Returns shift of X in pixels.
int DrawTextLenScaleInv(const char* text, int len, int x, int y, int scalex, int scaley)
{
	int dx = FrameBuffer.fontw*scalex;
	int res = 0;
	for (; len > 0; len--)
	{
		DrawCharScaleInv(*text++, x, y, scalex, scaley);
		x += dx;
		res += dx;
	}
	return res;
}

int DrawTextLenInv(const char* text, int len, int x, int y) { return DrawTextLenScaleInv(text, len, x, y, 1, 1); }
int DrawTextLenWInv(const char* text, int len, int x, int y) { return DrawTextLenScaleInv(text, len, x, y, 2, 1); }
int DrawTextLenHInv(const char* text, int len, int x, int y) { return DrawTextLenScaleInv(text, len, x, y, 1, 2); }
int DrawTextLen2Inv(const char* text, int len, int x, int y) { return DrawTextLenScaleInv(text, len, x, y, 2, 2); }
int DrawTextLen3Inv(const char* text, int len, int x, int y) { return DrawTextLenScaleInv(text, len, x, y, 3, 3); }
int DrawTextLen4Inv(const char* text, int len, int x, int y) { return DrawTextLenScaleInv(text, len, x, y, 4, 4); }

// draw ASCIIZ text (terminated with zero), using system fixed font (alpha 0=transparent, 255=opaque; use DrawSelFont() to select font)
// - To invert text, use bit 7 of the characters, or FrameBuffer.inv flag (flags are XORed).
// - Returns shift of X in pixels.
int DrawTextScale(const char* text, int x, int y, int scalex, int scaley, u32 col)
{
	return DrawTextLenScale(text, StrLen(text), x, y, scalex, scaley, col);
}

int DrawText(const char* text, int x, int y, u32 col) { return DrawTextScale(text, x, y, 1, 1, col); }
int DrawTextW(const char* text, int x, int y, u32 col) { return DrawTextScale(text, x, y, 2, 1, col); }
int DrawTextH(const char* text, int x, int y, u32 col) { return DrawTextScale(text, x, y, 1, 2, col); }
int DrawText2(const char* text, int x, int y, u32 col) { return DrawTextScale(text, x, y, 2, 2, col); }
int DrawText3(const char* text, int x, int y, u32 col) { return DrawTextScale(text, x, y, 3, 3, col); }
int DrawText4(const char* text, int x, int y, u32 col) { return DrawTextScale(text, x, y, 4, 4, col); }

// invert ASCIIZ text (terminated with zero), using system fixed font (use DrawSelFont() to select font)
// - To invert text, use bit 7 of the characters, or FrameBuffer.inv flag (flags are XORed).
// - Returns shift of X in pixels.
int DrawTextScaleInv(const char* text, int x, int y, int scalex, int scaley)
{
	return DrawTextLenScaleInv(text, StrLen(text), x, y, scalex, scaley);
}

int DrawTextInv(const char* text, int x, int y) { return DrawTextScaleInv(text, x, y, 1, 1); }
int DrawTextWInv(const char* text, int x, int y) { return DrawTextScaleInv(text, x, y, 2, 1); }
int DrawTextHInv(const char* text, int x, int y) { return DrawTextScaleInv(text, x, y, 1, 2); }
int DrawText2Inv(const char* text, int x, int y) { return DrawTextScaleInv(text, x, y, 2, 2); }
int DrawText3Inv(const char* text, int x, int y) { return DrawTextScaleInv(text, x, y, 3, 3); }
int DrawText4Inv(const char* text, int x, int y) { return DrawTextScaleInv(text, x, y, 4, 4); }

// draw text with length and background, using system fixed font (no alpha transparency; use DrawSelFont() to select font)
// - To invert text, use bit 7 of the characters, or FrameBuffer.inv flag (flags are XORed).
// - Returns shift of X in pixels.
int DrawTextBgLenScale(const char* text, int len, int x, int y, int scalex, int scaley, u32 colfg, u32 colbg)
{
	int dx = FrameBuffer.fontw*scalex;
	int res = 0;
	for (; len > 0; len--)
	{
		DrawCharBgScale(*text++, x, y, scalex, scaley, colfg, colbg);
		x += dx;
		res += dx;
	}
	return res;
}

int DrawTextBgLen(const char* text, int len, int x, int y, u32 colfg, u32 colbg) { return DrawTextBgLenScale(text, len, x, y, 1, 1, colfg, colbg); }
int DrawTextBgLenW(const char* text, int len, int x, int y, u32 colfg, u32 colbg) { return DrawTextBgLenScale(text, len, x, y, 2, 1, colfg, colbg); }
int DrawTextBgLenH(const char* text, int len, int x, int y, u32 colfg, u32 colbg) { return DrawTextBgLenScale(text, len, x, y, 1, 2, colfg, colbg); }
int DrawTextBgLen2(const char* text, int len, int x, int y, u32 colfg, u32 colbg) { return DrawTextBgLenScale(text, len, x, y, 2, 2, colfg, colbg); }
int DrawTextBgLen3(const char* text, int len, int x, int y, u32 colfg, u32 colbg) { return DrawTextBgLenScale(text, len, x, y, 3, 3, colfg, colbg); }
int DrawTextBgLen4(const char* text, int len, int x, int y, u32 colfg, u32 colbg) { return DrawTextBgLenScale(text, len, x, y, 4, 4, colfg, colbg); }

// draw ASCIIZ text with background (terminated with zero), using system fixed font (no alpha transparency; use DrawSelFont() to select font)
// - To invert text, use bit 7 of the characters, or FrameBuffer.inv flag (flags are XORed).
// - Returns shift of X in pixels.
int DrawTextBgScale(const char* text, int x, int y, int scalex, int scaley, u32 colfg, u32 colbg)
{
	return DrawTextBgLenScale(text, StrLen(text), x, y, scalex, scaley, colfg, colbg);
}

int DrawTextBg(const char* text, int x, int y, u32 colfg, u32 colbg) { return DrawTextBgScale(text, x, y, 1, 1, colfg, colbg); }
int DrawTextBgW(const char* text, int x, int y, u32 colfg, u32 colbg) { return DrawTextBgScale(text, x, y, 2, 1, colfg, colbg); }
int DrawTextBgH(const char* text, int x, int y, u32 colfg, u32 colbg) { return DrawTextBgScale(text, x, y, 1, 2, colfg, colbg); }
int DrawTextBg2(const char* text, int x, int y, u32 colfg, u32 colbg) { return DrawTextBgScale(text, x, y, 2, 2, colfg, colbg); }
int DrawTextBg3(const char* text, int x, int y, u32 colfg, u32 colbg) { return DrawTextBgScale(text, x, y, 3, 3, colfg, colbg); }
int DrawTextBg4(const char* text, int x, int y, u32 colfg, u32 colbg) { return DrawTextBgScale(text, x, y, 4, 4, colfg, colbg); }

// draw character using proportional font, returns shift of X coordinate (alpha 0=transparent, 255=opaque; to select font, use DrawSelFontProp())
//  first ... flag of first character (= do not apply left spacing)
int DrawCharProp(u8 ch, int x, int y, Bool first, u32 col)
{
	// prepare font
	sFrameBuffer* f = &FrameBuffer;
	const u8* fnt = f->fontprop;
	const sFont* font = (const sFont*)fnt;

	// space character
	if (ch == ' ') return font->space;

	// check if character is valid
	if ((ch < font->first) || (ch > font->last)) return 0;

	// get pointer to character definition
	const sFontChar* fch = &font->fontchar[ch - font->first];

	// left spacing
	int dx = 0;
	if (!first)
	{
		dx = (fch->spacing & 0xf) - FONTOFFLR;
		x += dx;
	}

	// right spacing
	dx += (fch->spacing >> FONTSHIFTR) - FONTOFFLR;

	// prepare character width
	int w = fch->w;
	dx += w;
	int xs = 0;
	if (x < f->minx)
	{
		xs = f->minx - x;
		x += xs;
		w -= xs;
	}
	if (x + w > f->maxx) w = f->maxx - x;
	if (w <= 0) return dx;

	// get pointer to picture
	const sPic* pic = (const sPic*)(fnt + font->pic);

	// prepare character height
	int h = pic->h;	// height of the font 

	// prepare Y coordinate
	int ys = 0;
	if (y < f->miny)
	{
		ys = f->miny - y;
		y += ys;
		h -= ys;
	}
	if (y + h > f->maxy) h = f->maxy - y;
	if (h <= 0) return dx;

	// prepare color components
	u32 srb = col & 0x00ff00ff;
	u32 sg = col & 0x0000ff00;
	u32 col_a = col >> 24;

	// prepare destination buffer
	int wb = f->drawpitchpix;
	u32* d = &f->drawbuf[x + y*wb];
	wb -= w;

	// prepare source address
	int wbs = pic->wb - w;
	const u8* s = &pic->data[xs + fch->x + ys*wbs];

	// decode character
	int w2;
	for (; h > 0; h--)
	{
		for (w2 = w; w2 > 0; w2--)
		{
			u32 a1 = *s++;
			a1 *= col_a;
			a1 = DIV255(a1);

			if (a1 == 255)
			{
				*d = col;
			}
			else if (a1 != 0)
			{
				u32 inv = 255 - a1;
				u32 dst = *d;
				u32 rb = (dst & 0x00ff00ff);
				u32 g = (dst & 0x0000ff00);
				rb = (srb*a1 + rb*inv + 0x00ff00ff) & 0xff00ff00;
				g = (sg*a1 + g*inv + 0x0000ff00) & 0x00ff0000;
				*d = ((rb | g) >> 8) | 0xff000000;
			}
			d++;
		}
		s += wbs;
		d += wb;
	}
	return dx;
}

// invert character using proportional font, returns shift of X coordinate (to select font, use DrawSelFontProp())
//  first ... flag of first character (= do not apply left spacing)
int DrawCharPropInv(u8 ch, int x, int y, Bool first)
{
	// prepare font
	sFrameBuffer* f = &FrameBuffer;
	const u8* fnt = f->fontprop;
	const sFont* font = (const sFont*)fnt;

	// space character
	if (ch == ' ') return font->space;

	// check if character is valid
	if ((ch < font->first) || (ch > font->last)) return 0;

	// get pointer to character definition
	const sFontChar* fch = &font->fontchar[ch - font->first];

	// left spacing
	int dx = 0;
	if (!first)
	{
		dx = (fch->spacing & 0xf) - FONTOFFLR;
		x += dx;
	}

	// right spacing
	dx += (fch->spacing >> FONTSHIFTR) - FONTOFFLR;

	// prepare character width
	int w = fch->w;
	dx += w;
	int xs = 0;
	if (x < f->minx)
	{
		xs = f->minx - x;
		x += xs;
		w -= xs;
	}
	if (x + w > f->maxx) w = f->maxx - x;
	if (w <= 0) return dx;

	// get pointer to picture
	const sPic* pic = (const sPic*)(fnt + font->pic);

	// prepare character height
	int h = pic->h;	// height of the font 

	// prepare Y coordinate
	int ys = 0;
	if (y < f->miny)
	{
		ys = f->miny - y;
		y += ys;
		h -= ys;
	}
	if (y + h > f->maxy) h = f->maxy - y;
	if (h <= 0) return dx;

	// prepare destination buffer
	int wb = f->drawpitchpix;
	u32* d = &f->drawbuf[x + y*wb];
	wb -= w;

	// prepare source address
	int wbs = pic->wb - w;
	const u8* s = &pic->data[xs + fch->x + ys*wbs];

	// decode character
	int w2;
	for (; h > 0; h--)
	{
		for (w2 = w; w2 > 0; w2--)
		{
			if (*s++ >= 0x80) *d ^= 0x00ffffff;
			d++;
		}
		s += wbs;
		d += wb;
	}
	return dx;
}

// draw text with length using proportional font (alpha 0=transparent, 255=opaque; to select font, use DrawSelFontProp())
// - Returns shift of X in pixels.
int DrawTextPropLen(const char* text, int len, int x, int y, u32 col)
{
	int dx;
	int res = 0;
	if (len <= 0) return res;
	u8 ch = *text++;
	dx = DrawCharProp(ch, x, y, True, col);
	x += dx;
	res += dx;
	len--;

	for (; len > 0; len--)
	{
		ch = *text++;
		dx = DrawCharProp(ch, x, y, False, col);
		x += dx;
		res += dx;
	}
	return res;
}

// invert text with length using proportional font (to select font, use DrawSelFontProp())
// - Returns shift of X in pixels.
int DrawTextPropLenInv(const char* text, int len, int x, int y)
{
	int dx;
	int res = 0;
	if (len <= 0) return res;
	u8 ch = *text++;
	dx = DrawCharPropInv(ch, x, y, True);
	x += dx;
	res += dx;
	len--;

	for (; len > 0; len--)
	{
		ch = *text++;
		dx = DrawCharPropInv(ch, x, y, False);
		x += dx;
		res += dx;
	}
	return res;
}

// draw ASCIIZ text using proportional font (alpha 0=transparent, 255=opaque; to select font, use DrawSelFontProp())
// - Returns shift of X in pixels.
int DrawTextProp(const char* text, int x, int y, u32 col)
{
	int len = StrLen(text);
	return DrawTextPropLen(text, len, x, y, col);
}

// invert ASCIIZ text using proportional font (to select font, use DrawSelFontProp())
// - Returns shift of X in pixels.
int DrawTextPropInv(const char* text, int x, int y)
{
	int len = StrLen(text);
	return DrawTextPropLenInv(text, len, x, y);
}

// get length in pixels of character using proportional font (to select font, use DrawSelFontProp())
//  first ... flag of first character (= do not apply left spacing)
int DrawCharPropDX(u8 ch, Bool first)
{
	// prepare font
	sFrameBuffer* f = &FrameBuffer;
	const u8* fnt = f->fontprop;
	const sFont* font = (const sFont*)fnt;

	// space character
	if (ch == ' ') return font->space;

	// check if character is valid
	if ((ch < font->first) || (ch > font->last)) return 0;

	// get pointer to character definition
	const sFontChar* fch = &font->fontchar[ch - font->first];

	// left spacing
	int dx = 0;
	if (!first)
	{
		dx = (fch->spacing & 0xf) - FONTOFFLR;
	}

	// right spacing
	dx += (fch->spacing >> FONTSHIFTR) - FONTOFFLR;

	// character width
	int w = fch->w;
	dx += w;

	return dx;
}

// get length in pixels of text with length using proportional font (to select font, use DrawSelFontProp())
int DrawTextPropLenDX(const char* text, int len)
{
	int res = 0;
	if (len <= 0) return res;
	u8 ch = *text++;
	res += DrawCharPropDX(ch, True);
	len--;

	for (; len > 0; len--)
	{
		ch = *text++;
		res += DrawCharPropDX(ch, False);
	}
	return res;
}

// get length in pilxes of ASCIIZ text using proportional font (to select font, use DrawSelFontProp())
int DrawTextPropDX(const char* text)
{
	int len = StrLen(text);
	return DrawTextPropLenDX(text, len);
}

// ----------------------------------------------------------------------------
//                                 Print
// ----------------------------------------------------------------------------

// scroll screen (in clipping area) up one row (does not update current cursor row)
void DrawScroll()
{
	int y, y2;

	// get frame buffer
	sFrameBuffer* f = &FrameBuffer;

	// get clipping area
	int minx = f->minx;
	int maxx = f->maxx;
	int miny = f->miny;
	int maxy = f->maxy;
	int fonth = f->fonth;
	u32 wb = f->drawpitchpix;
	int rowsize = fonth*wb;
	int w = maxx - minx;
	int h = maxy - miny;
	u32* buf = f->drawbuf + minx + miny*wb;

	// row correction
	f->printrow--;

	// scroll screen
	y2 = h - fonth;
	for (y = miny; y < y2; y++)
	{
		memcpy(buf, buf + rowsize, w*4);
		buf += wb;
	}

	// clear last row
	DrawRect(minx, maxy - fonth, w, fonth, f->printcolbg);
}

// scroll screen in given area up one row (does not update current cursor row)
void DrawScrollRect(int x, int y, int w, int h)
{
	// get frame buffer
	sFrameBuffer* f = &FrameBuffer;

	// get old clipping area
	int oldminx = f->minx;
	int oldmaxx = f->maxx;
	int oldminy = f->miny;
	int oldmaxy = f->maxy;

	// limit new clipping area
	int minx = x;
	if (minx < oldminx) minx = oldminx;
	if (minx > oldmaxx) minx = oldmaxx;
	if (w < 0) w = 0;
	int maxx = minx + w;
	if (maxx > oldmaxx) maxx = oldmaxx;
	int miny = y;
	if (miny < oldminy) miny = oldminy;
	if (miny > oldmaxy) miny = oldmaxy;
	if (h < 0) h = 0;
	int maxy = miny + h;
	if (maxy > oldmaxy) maxy - oldmaxy;
	if ((minx >= maxx) || (miny >= maxy)) return;

	// set new clipping area
	f->minx = minx;
	f->maxx = maxx;
	f->miny = miny;
	f->maxy = maxy;

	// scroll
	DrawScroll();

	// restore clipping area
	f->minx = oldminx;
	f->maxx = oldmaxx;
	f->miny = oldminy;
	f->maxy = oldmaxy;
}

/*
DrawPrint test sample:

	SelFont8x16(); // select font 8x16
	DrawPrint(
		"\f"					// clear screen (resolution: 40 characters x 15 rows)

		"- invisible row scrolls up -\n"	// 0: this row will be scrolled-up

		"Hello world?\b!\n"			// 1: first visible print, overprint "?" -> "!"

		"First! row\r" "Second\n"		// 2: overprint "First! row" -> "Second row"

		"\3normal \4height \5width \6sized\3\n"	// 3: double-height row with font of all 4 sizes
		"\3font   \a\a\a\a\a\a\a\5font\3\n"	// 4: second half of previous double-height row

		"non-invert \2invert\1 non-invert\n"	// 5: inverted text

		"0123456701234567012345670123456701234567" // 6: print ruler 40 characters and move to next row (row is 40 columns width)
		"\t123\t1234567\t12345678\tnext row\n"	// 7, 8: move to tab positions and move to next row after all

		"\20 normal \21 blue \22 green \23 cyan \n" // 9: print colors
		"\24 red \25 magenta \26 yellow \27 white \n" // 10:

		"\2"
		"\20 normal \21 blue \22 green \23 cyan \n" // 11: print colors inverted
		"\24 red \25 magenta \26 yellow \27 white \n" // 12:

		"\1\20"					// back to default state
		"%d 0x%08x ALL OK\n"			// 13: print numbers 12345678, 0x12345678

		"\nscrolling up",			// 14: this command scrolls screen up

		12345678, 0x12345678
	);
*/

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
void DrawPrintChar(char ch)
{
	// get frame buffer
	sFrameBuffer* f = &FrameBuffer;

	switch ((u8)ch)
	{
	// printable characters
	default:
		if ((u8)ch < 32) break; // not printable character

		// new line
		if (f->printpos >= f->printposnum) DrawPrintChar(0x0A);

		// double-width or double-sized font
		if (f->printsize >= 2)
		{
			if (f->printsize == 3)
			{
				if (f->printrow >= f->printrownum-1)
				{
					DrawScroll(); // scroll screen one row up
					f->printrow = f->printrownum-2;
				}

				// double-sized font
				DrawCharBg2(ch, (int)f->printpos*f->fontw, (int)f->printrow*f->fonth, f->printcolfg, f->printcolbg);
			}
			else
				// double-width font
				DrawCharBgW(ch, (int)f->printpos*f->fontw, (int)f->printrow*f->fonth, f->printcolfg, f->printcolbg);

			// move cursor 1 position right
			f->printpos++;
		}
		else
		{
			if (f->printsize == 1)
			{
				if (f->printrow >= f->printrownum-1)
				{
					DrawScroll(); // scroll screen one row up
					f->printrow = f->printrownum-2;
				}

				// double-height font
				DrawCharBgH(ch, (int)f->printpos*f->fontw, (int)f->printrow*f->fonth, f->printcolfg, f->printcolbg);
			}
			else
				// normal font size
				DrawCharBg(ch, (int)f->printpos*f->fontw, (int)f->printrow*f->fonth, f->printcolfg, f->printcolbg);
		}

  // ! continue case 0x07 (move right)

	// '\a' ... (bell) move cursor 1 position right (no print)
	case 0x07:
		f->printpos++;
		if (f->printpos < f->printposnum) break;

  // ! continue case 0x0A (new line)

	// '\n' ... (new line) move cursor to start of next row
	case 0x0A:
		f->printpos = 0;
		f->printrow++; // increase row
		if (f->printrow >= f->printrownum)
		{
			DrawScroll(); // scroll screen one row up
			f->printrow = f->printrownum-1;
		}

		// double height
		if ((f->printsize & 1) != 0)
		{
			f->printrow++; // increase row
			if (f->printrow >= f->printrownum)
			{
				DrawScroll(); // scroll screen one row up
				f->printrow = f->printrownum-1;
			}
		}
		break;

	// 'x01' ... set not-inverted text
	case 0x01:
		f->inv = False;
		break;

	// 'x02' ... set inverted text
	case 0x02:
		f->inv = True;
		break;

	// 'x03' ... use normal-sized font (default)
	case 0x03:
		f->printsize = 0; // fonf size: 0=normal, 1=double-height, 2=double-width, 3=double-size
		break;

	// 'x04' ... use double-height font
	case 0x04:
		f->printsize = 1; // fonf size: 0=normal, 1=double-height, 2=double-width, 3=double-size
		break;

	// 'x05' ... use double-width font
	case 0x05:
		f->printsize = 2; // fonf size: 0=normal, 1=double-height, 2=double-width, 3=double-size
		break;

	// 'x06' ... use double-sized font
	case 0x06:
		f->printsize = 3; // fonf size: 0=normal, 1=double-height, 2=double-width, 3=double-size
		break;

	// '\b' ... (back space) move cursor 1 position left (no print)
	case 0x08:
		if (f->printpos > 0) f->printpos--;
		break;

	// '\t' ... (tabulator) move cursor to next 8-character position, print spaces
	case 0x09:
		{
			u8 oldsize = f->printsize; // save current font size
			f->printsize = oldsize & 1; // change double-width or double-size to normal or double-height
			do DrawPrintChar(' '); while ((f->printpos & 7) != 0); // print spaces
			f->printsize = oldsize; // restore font size
		}
		break;		

	// '\v' ... (vertical tabulator) move cursor to start of previous row
	case 0x0B:
		if (f->printrow > 0) f->printrow--; // decrease row
		f->printpos = 0;
		break;

	// '\f' ... (form feed) clear screen, reset cursor position, select default colors
	case 0x0C:
		DrawClearCol(COL_GRAY, COL_BLACK);
		break;

	// '\r' ... (carriage return) move cursor to start of current row
	case 0x0D:
		f->printpos = 0;
		break;

	// 'x10' ... set gray text color (COL_GRAY, normal color)
	case 0x10:
		f->printcolfg = COL_GRAY;
		break;

	// 'x11' ... set blue text color (COL_AZURE)
	case 0x11:
		f->printcolfg = COL_AZURE;
		break;

	// 'x12' ... set green text color (COL_GREEN)
	case 0x12:
		f->printcolfg = COL_GREEN;
		break;

	// 'x13' ... set cyan text color (COL_CYAN)
	case 0x13:
		f->printcolfg = COL_CYAN;
		break;

	// 'x14' ... set red text color (COL_RED)
	case 0x14:
		f->printcolfg = COL_RED;
		break;

	// 'x15' ... set magenta text color (COL_MAGENTA)
	case 0x15:
		f->printcolfg = COL_MAGENTA;
		break;

	// 'x16' ... set yellow text color (COL_YELLOW)
	case 0x16:
		f->printcolfg = COL_YELLOW;
		break;

	// 'x17' ... set white text color (COL_WHITE)
	case 0x17:
		f->printcolfg = COL_WHITE;
		break;
	}
}

// print text to console with length (Control characters - see DrawPrintCharRaw)
void DrawPrintTextLen(const char* text, int len)
{
	for (; len > 0; len--) DrawPrintChar(*text++);
}

// print ASCIIZ text to console (Control characters - see DrawPrintCharRaw)
//  If text contains digit after hex numeric code of control character,
//  split text to more parts: use "\4" "1" instead of "\41".
void DrawPrintText(const char* text)
{
	DrawPrintTextLen(text, StrLen(text));
}

#if USE_STREAM	// use Data stream (lib_stream.c, lib_stream.h)

// callback - write data to drawing console
u32 StreamWriteDrawPrint(sStream* str, const void* buf, u32 num)
{
	u32 n = num;
	const char* txt = (const char*)buf;
	for (; n > 0; n--) DrawPrintChar(*txt++);
	return num;
}

// formatted print string to drawing console, with argument list, without DispUpdate() (returns number of characters, without terminating 0)
u32 DrawPrintArg(const char* fmt, va_list args)
{
	// write and read stream
	sStream wstr, rstr;

	// initialize stream to read from
	StreamReadBufInit(&rstr, fmt, StrLen(fmt));

	// initialize stream to write to
	Stream0Init(&wstr); // initialize nul stream
	wstr.write = StreamWriteDrawPrint; // write callback
	
	// print string
	u32 num = StreamPrintArg(&wstr, &rstr, args);

	// display update
	//if (num > 0) DispUpdate();
	return num;
}

// formatted print string to drawing console, with variadic arguments, without DispUpdate() (returns number of characters, without terminating 0)
NOINLINE u32 DrawPrint(const char* fmt, ...)
{
	u32 n;
	va_list args;
	va_start(args, fmt);
	n = DrawPrintArg(fmt, args);
	va_end(args);
	return n;
}

#endif // USE_STREAM

#undef DRAW_BLEND_PREP
#undef DRAW_BLEND_PIXEL
#undef DRAW_INV_VAL
#undef DIV255
#undef DRAW_ROUND_HIDE
#undef DRAW_ROUND_CLIP
#undef DRAW_CIRCLE_PIX
#undef DRAW_CIRCLE_PIX_BLEND
#undef DRAW_CIRCLE_PIXINV
#undef DRAW_ELLIPSE_PIX
#undef DRAW_ELLIPSE_PIXBLEND
#undef DRAW_ELLIPSE_PIXINV
#undef DRAW_TRIANGLE_HLINE
#undef DRAW_TRIANGLE_HLINE_BLEND
#undef DRAW_TRIANGLE_HLINE_INV
#undef DRAW_IMG_LIMIT

#endif // USE_DRAW
