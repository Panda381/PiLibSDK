
// ****************************************************************************
//
//                             Frame Buffer
//
// ****************************************************************************

#include "../../includes.h"	// includes

#if WIDTH	// draw width (0=do not initialize default videomode)
#if SCREENSCALE // 0 = no back buffer, using back buffer with screen scale 1, 2 or 4
// back buffer
u32 BackBuf[WIDTH*HEIGHT];
#endif
#endif

// current frame buffer
sFrameBuffer FrameBuffer;

// initialize frame buffer (returns False on error)
Bool FrameBufferInit(int width, int height)
{
	u32 *prop;

	// create frame buffer
	MailPropInit();						// initialize property buffer
	MailPropAdd(TAG_SET_PHYSICAL_SIZE, width, height);	// set physical width/height of buffer
	MailPropAdd(TAG_SET_VIRTUAL_SIZE, width, height);	// set virtual width/height of buffer
	MailPropAdd(TAG_SET_DEPTH, 32);				// set depth (bits per pixel)
	MailPropAdd(TAG_SET_PIXEL_ORDER, 1);			// set pixel order RGB (to be compatile with OpenGL)
	MailPropAdd(TAG_SET_VIRTUAL_OFFSET, 0, 0);		// set virtual offset
	MailPropAdd(TAG_ALLOCATE_BUFFER, 4);			// allocate frame buffer
	MailPropProcess();

	// get buffer address and size
	prop = MailPropGet(TAG_ALLOCATE_BUFFER);
	sFrameBuffer* f = &FrameBuffer;
	if (prop == NULL) return False;
	f->screenbuf = (u32*)(uintptr_t)(prop[1] & ~0xC0000000); // buffer address
	if (f->screenbuf == NULL) return False;

	// get properties of frame buffer
	MailPropInit();						// initialize property buffer
	MailPropAdd(TAG_GET_PHYSICAL_SIZE);			// get physical width/height of buffer
	MailPropAdd(TAG_GET_PITCH);				// get pitch
	MailPropProcess();

	// frame size
	prop = MailPropGet(TAG_GET_PHYSICAL_SIZE);
	f->screenwidth = prop[1]; // screen width
	f->screenheight = prop[2]; // screen height
	if ((f->screenwidth != width) || (f->screenheight != height)) return False;

	// pitch
	prop = MailPropGet(TAG_GET_PITCH);
	f->screenpitch = width*4;	// pitch in bytes
	f->screenpitchpix = width;	// pitch in pixels
	if ((prop[1] != width) && (prop[1] != width*4));

	// select default font, disable text inversion
	f->font = FONT;
	f->fontw = FONTW;
	f->fonth = FONTH;
#if USE_FONTPROP		// 1=use support of proportional fonts, 0=not used
	f->fontprop = FONTPROP;
#endif

	// printing
	f->lastupdate = Time();	// time in [us] of last display update DispUpdate()
	f->inv = False;		// flag - invert text with system fixed font (XORed with bit 7 of the characters)
	f->printsize = 0;	// text size 0=normal, 1=double-height, 2=double-width, 3=double-size
	f->printcolfg = COL_GRAY;	// print color - foreground
	f->printcolbg = COL_BLACK;	// print color - background

	// select to draw directly to screen buffer
	SelectScreenBuf();
	return True;
}

// select to draw directly to screen buffer
void SelectScreenBuf(void)
{
	sFrameBuffer* f = &FrameBuffer;
	// draw clipping
	f->minx = 0;
	f->miny = 0;
	f->maxx = f->drawwidth = f->screenwidth;
	f->maxy = f->drawheight = f->screenheight;
	f->drawpitch = f->screenpitch;
	f->drawpitchpix = f->screenpitchpix;
	f->drawbuf = f->screenbuf;	
	f->drawscale = 1;
	DrawSelFont(f->font, f->fontw, f->fonth); // select current system fixed font and update printing
}

// select to draw to back buffer (select scale = 1, 2 or 4)
void SelectDrawBuf(u32* buf, int scale)
{
	int shift = 0;
	if (scale == 2) shift = 1;
	if (scale == 4) shift = 2;
	sFrameBuffer* f = &FrameBuffer;
	f->minx = 0;
	f->miny = 0;
	f->maxx = f->drawwidth = f->screenwidth >> shift;
	f->maxy = f->drawheight = f->screenheight >> shift;
	f->drawpitch = f->screenpitch >> shift;
	f->drawpitchpix = f->screenpitchpix >> shift;
	f->drawbuf = buf;	
	f->drawscale = scale;
	DrawSelFont(f->font, f->fontw, f->fonth); // select current system fixed font and update printing
}

// select current system fixed font (1-bit image with width 128 characters, total width 1024 pixels, 1 character width max. 8 pixels)
void DrawSelFont(const u8* font, int w, int h)
{
	sFrameBuffer* f = &FrameBuffer;
	f->font = font;
	f->fontw = w;
	f->fonth = h;
	f->printposnum = f->drawwidth/w;	// number of text positions per row (= drawwidth / fontw)
	f->printrownum = f->drawheight/h;	// number of text rows (= drawheight / fonth)
	f->printpos = 0;			// print position
	f->printrow = 0;			// print row
}

// update draw buffer (send to screen buffer)
void DispUpdate()
{
	int y, w, h, i, wbs, wbd;
	u32 *s, *d, pix;

	// auto-update
	sFrameBuffer* f = &FrameBuffer;
	f->lastupdate = Time();	// time in [us] of last display update DispUpdate()

	// drawing directly to screen buffer
	if (f->drawbuf == f->screenbuf)
	{
		// update data cache
		CleanDataCache();
		return;
	}

	// scale 1
	s = f->drawbuf;
	d = f->screenbuf;
	h = f->drawheight;
	int scale = f->drawscale;
	if (scale <= 1)
		memcpy(d, s, h*f->drawpitch);

	// scale 2
	else if (scale == 2)
	{
		w = f->drawwidth;
		wbs = f->drawpitchpix;
		wbd = f->screenpitchpix;
		for (y = 0; y < h; y++)
		{
			s = f->drawbuf + y*wbs;
			d = f->screenbuf + y*2*wbd;

			for (i = w; i > 0; i--)
			{
				pix = *s;
				d[0] = pix;
				d[1] = pix;
				d[wbd] = pix;
				d[wbd+1] = pix;
				s++;
				d += 2;
			}
		}
	}

	// scale 4
	else
	{
		w = f->drawwidth;
		wbs = f->drawpitchpix;
		wbd = f->screenpitchpix;
		for (y = 0; y < h; y++)
		{
			s = f->drawbuf + y*wbs;
			d = f->screenbuf + y*4*wbd;

			for (i = w; i > 0; i--)
			{
				pix = *s;
				d[0] = pix;
				d[1] = pix;
				d[2] = pix;
				d[3] = pix;

				d[wbd] = pix;
				d[wbd+1] = pix;
				d[wbd+2] = pix;
				d[wbd+3] = pix;

				d[2*wbd] = pix;
				d[2*wbd+1] = pix;
				d[2*wbd+2] = pix;
				d[2*wbd+3] = pix;

				d[3*wbd] = pix;
				d[3*wbd+1] = pix;
				d[3*wbd+2] = pix;
				d[3*wbd+3] = pix;

				s++;
				d += 4;
			}
		}
	}

	// update data cache
	CleanDataCache();
}

// update draw buffer with auto interval
void DispUpdateAuto()
{
	if ((u32)(Time() - FrameBuffer.lastupdate) >= (u32)(AUTOUPDATE_MS*1000)) DispUpdate();
}

// initialize default videomode (called from SysInit)
void DispInit(void)
{
#if WIDTH	// draw width (0=do not initialize default videomode)
#if SCREENSCALE
	if (!FrameBufferInit(WIDTH*SCREENSCALE, HEIGHT*SCREENSCALE)) while (True) {}
	SelectDrawBuf(BackBuf, SCREENSCALE);
#else
	if (!FrameBufferInit(WIDTH, HEIGHT)) while (True) {}
#endif
#endif
}
