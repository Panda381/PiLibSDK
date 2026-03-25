
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

// earth image
#define EARTHW	1024	// image width (must be power of 2)
#define EARTHH	512	// image height

// globe
#define GLOBER	(HEIGHT/2 - 1) // globe radius
#define GLOBEX	(WIDTH/2) // globe X center
#define GLOBEY	(HEIGHT/2) // globe Y center

#define USERANDSHADOW	2	// 2=use blend shadow, 1=use random shadow, 0=use rastered shadow

// precalculated U coordinate
#define UNUM 1600
#define UNUM2 (UNUM/2)
int UTab[UNUM];

int main()
{
	int i, x, y, xr, u, v, off, key;
	u32* d;
	const u32 *s1, *s2;
	u32 c1, c2;
	u32 t, t2;
	u8 night;

	// clear draw box
	DrawClear();

	// prepare U table
	for (i = 0; i < UNUM; i++)
	{
		UTab[i] = (int)((asinf((float)(i-UNUM2)/UNUM2)/PI2+0.25f)*EARTHW);
	}

	// image data
	const u32* img1 = (const u32*)(Earth1Img+8);
	const u32* img2 = (const u32*)(Earth2Img+8);

	// main loop
	off = 0;
	t = Time();
	u32* buf = FrameBuffer.drawbuf;
	while (True)
	{
		// keyboard
		key = KeyGet();
		if (key == KEY_ESC) Reboot();	// Program exit
		if (key == KEY_SCREENSHOT) ScreenShot(); //  Screenshot - This may take a few seconds to write.

		// Y loop
		for (y = -GLOBER; y < +GLOBER; y++)
		{
			// X offset
			xr = (int)(sqrtf((float)GLOBER*GLOBER - y*y) + 0.5f);
			if (xr == 0) xr = 1; // to avoid divide by zero

			// destination address
			d = &buf[GLOBEX-xr + (y+GLOBEY)*WIDTH];

			// texture V coordinate
			v = (int)((asinf((float)y/GLOBER)/PI+0.5f)*EARTHH);

			// source address
			s1 = &img1[v*EARTHW];
			s2 = &img2[v*EARTHW];

			// X loop
			for (x = -xr; x < xr; x++)
			{
				// texture U coordinate
				u = UTab[x*UNUM2/xr+UNUM2];

#define SHADOWBEG	(xr*6/16) // shadow beging
#define SHADOW1		(xr*7/16) // shadow boundary 1
#define SHADOW2		(xr*8/16) // shadow boundary 2
#define SHADOWEND	(xr*9/16) // shadow end

#if USERANDSHADOW == 2 // use blend shadow

				// select bitmap
				if (x < SHADOWBEG) // always day
					night = False;	// day
				else if (x >= SHADOWEND) // always night
					night = True;	// night
				else
					night = 2 + (x - SHADOWBEG)*256/(SHADOWEND-SHADOWBEG);

#elif USERANDSHADOW == 1 // use random shadow

				// select bitmap
				if (x < SHADOWBEG) // always day
					night = False;	// day
				else if (x >= SHADOWEND) // always night
					night = True;	// night
				else
				{
					if (x <= (int)RandU16MinMax(SHADOWBEG, SHADOWEND))
						night = False;	// day
					else
						night = True;	// night
				}

#else // use random shadow

				if (x < SHADOWBEG) // always day
					night = False;	// day
				else if (x >= SHADOWEND) // always night
					night = True;	// night
				else if (x < SHADOW1) // light shadow
				{
					if (((x & 1) + ((y & 1)<<1)) == 0)
						night = True;	// night
					else
						night = False;	// day
				}
				else if (x < SHADOW2) // shadow
				{
					if (((x + y) & 1) == 0)
						night = True;	// night
					else
						night = False;	// day
				}
				else // dark shadow
				{
					if (((x & 1) + ((y & 1)<<1)) != 3)
						night = True;	// night
					else
						night = False;	// day
				}

#endif // use random shadow

				// draw pixel
				c1 = s1[(u + off + EARTHW/2) & (EARTHW-1)]; // day
				c2 = s2[(u + off + EARTHW/2) & (EARTHW-1)]; // night

				if (night > 1)
					*d++ = BlendCol(c1, c2, 258 - night);
				else 
				if (night)
					*d++ = c2;
				else
					*d++ = c1;
			}
		}

		// shift angle
		off -= 1;
		while (off < EARTHW) off += EARTHW;

		// display update
		DispUpdate();
	}

	return 0;
}
