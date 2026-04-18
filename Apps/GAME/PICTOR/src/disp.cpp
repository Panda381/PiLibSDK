
// ****************************************************************************
//
//                             Display Commons
//
// ****************************************************************************

#include "../include.h"

// shadow screen
void DispShadow()
{
	// shadow image
	u32* d = FrameBuffer.drawbuf;
	int i = FrameBuffer.drawpitchpix * FrameBuffer.drawheight;
	u32 k, r, g, b, a;
	for (; i > 0; i--)
	{
		k = *d;
		r = k & 0xff;
		g = (k >> 8) & 0xff;
		b = (k >> 16) & 0xff;
		a = (r + g + b) >> 3;
		*d = a | (a << 8) | (a << 16) | 0xff000000;
		d++;
	}
}
