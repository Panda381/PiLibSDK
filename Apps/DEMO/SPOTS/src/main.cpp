
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

int main()
{
	u32* buf = FrameBuffer.drawbuf;

	// main loop
	while (True)
	{
		// random radius
		int r = RandU8MinMax(10,50);
		int r2 = r*r;

		// random center coordinate
		int xc = RandU16MinMax(r,WIDTH-r);
		int yc = RandU16MinMax(r,HEIGHT-r);

		// random color
		u32 col = RandU32();

		// loop Y
		int x, y, dist;
		u32* d;
		for (y = -r; y < r; y++)
		{
			// pointer into draw box
			d = &buf[WIDTH*(yc+y) + xc-r];

			// loop X
			for (x = -r; x < r; x++)
			{
				// distance from center
				dist = x*x + y*y;

				// draw pixel, near center there is more probability
				if (dist < RandU16Max(r2)) *d = col;

				// increase destination pointer
				d++;
			}
		}

		// auto update display
		DispUpdateAuto(); // auto update display

		// keyboard
		int key = KeyGet();
		if (key == KEY_ESC) Reboot();	// Program exit
		if (key == KEY_SCREENSHOT) ScreenShot(); //  Screenshot - This may take a few seconds to write.
	}

	return 0;
}
