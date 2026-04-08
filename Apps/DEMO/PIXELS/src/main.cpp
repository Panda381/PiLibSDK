
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

int main()
{
	int x, y;
	u32* buf = FrameBuffer.drawbuf;

	// main loop
	while (True)
	{
		x = RandU16Max(WIDTH-1); // random X coordinate
		y = RandU16Max(HEIGHT-1); // random Y coordinate
		buf[x + y*WIDTH] = RandU32(); // write pixel
		DispUpdateAuto(); // auto update display

		// keyboard
		int key = KeyGet();
		if (key == KEY_ESC) Reboot();	// Program exit
		if (key == KEY_SCREENSHOT) ScreenShot(); //  Screenshot - This may take a few seconds to write.
	}

	return 0;
}
