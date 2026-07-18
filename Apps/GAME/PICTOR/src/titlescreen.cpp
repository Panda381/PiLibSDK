
// ****************************************************************************
//
//                               Title screen
//
// ****************************************************************************

#include "../include.h"

// title screen (returns False to exit)
Bool TitleScreen()
{
	int strip;
#define TOPBUFN 30
	char buf[TOPBUFN];

	// flush keys
	KeyFlush();

	// loop
	while (True)
	{
		// draw title screen
		DrawImg(TitleImg);
		MemPrint(buf, TOPBUFN, "max %06d", MaxScore);
		DrawText2(buf, 8, 480-26, COL_DKGREEN);
		DispUpdate();

		// request to do screenshot
		if (ReqScreenShot)
		{
			ReqScreenShot = False;
			ScreenShot();
		}

		// keyboard
		switch (KeyGet())
		{
		case KEY_PAD_A:
			return True;

		case KEY_SCREENSHOT:
			ScreenShot();
			break;

		// LCD display rezoom
		case KEY_ZOOM:
			LCDRezoom();
			break;

		case KEY_PAD_Y:
			if (!GameMenu()) return False;
			break;
		}
	}
}
