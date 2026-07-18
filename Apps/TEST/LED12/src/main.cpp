
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

cLED12 Led12;
u8* DigImg;

int main()
{
	// initialize
	Led12.Init();

	// unpack image
	DigImg = (u8*)PNGLOAD(DigImgPNG);

	Bool k = True;
	int i, j, x;
	u8 b;
	
	while (True)
	{
		// send text
		Led12.TextBuf(k ? "3.14159265359" : "HELL0__WOR1D");
		Bool res = Led12.Update();
		k = !k;

#define YY 104
		// draw connection state
		DrawClear();
		DrawText(res ? "LED12 display OK" : "LED12 display not found", 0, 0, COL_GRAY);

		// display (size of 1 digit is 24x32 pixels)
		for (i = 0; i < 12; i++)
		{
			x = 16+i*24;
			DrawImg(DigImg, x, YY, 0, 0, 24, 32);
			b = Led12.buf[i];
			for (j = 1; j <= 8; j++)
			{
				if ((b & B0) == 0) DrawImg(DigImg, x, YY, j*24, 0, 24, 32);
				b >>= 1;
			}
		}
		DispUpdate();

		// keyboard
		for (i = 20; i > 0; i--)
		{
			WaitMs(100);
			u8 key = KeyGet();
			if (key == KEY_PAD_Y) Reboot();	// Program exit
			if (key == KEY_SCREENSHOT) ScreenShot(); //  Screenshot - This may take a few seconds to write.
		}
	}
}
