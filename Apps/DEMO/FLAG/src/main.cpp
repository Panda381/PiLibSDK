
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

#define FLAGW	360	// flag width
#define FLAGH	240	// flag height
#define FLAGX	18	// flag X coordinate relative to mast
#define FLAGY	90	// flag Y base coordinate

#define MASTW	24	// mast width
#define MASTH	420	// mast height
#define MASTX	110	// mast X coordinate

u8* CloudsImg;
u8* FlagImg;

int main()
{
	int x, x2, y, key;
	float phase;

	// decompress images from JPG format
	CloudsImg = (u8*)JPGLOAD(CloudsImgJpg);
	if (CloudsImg == NULL) Reboot(); // emergency exit
	FlagImg = (u8*)JPGLOAD(FlagImgJpg);
	if (FlagImg == NULL) Reboot(); // emergency exit

	// draw sky and mast
	DrawImg(CloudsImg, 0, 0);
	DrawImg(MastImg, MASTX, HEIGHT - MASTH);
	DispUpdate();

#define WAVELEN	0.03f	// wave length
#define WAVESPEED 0.04f	// animation speed
#define WAVEAMP 0.1f	// wave amplitude

	// main loop
	phase = 0;
	while (True)
	{
		// keyboard
		key = KeyGet();
		if (key == KEY_ESC) Reboot();	// Program exit
		if (key == KEY_SCREENSHOT) ScreenShot(); //  Screenshot - This may take a few seconds to write.

		// draw flag
		for (x = 0; x < FLAGW; x++)
		{
			// calculate vertical coordinate of the flag
			y = (int)(FLAGY + WAVEAMP*sin(phase - x*WAVELEN)*x);
			x2 = x + FLAGX + MASTX;

			// draw flag
			DrawImg(FlagImg, x2, y, x, 0, 1, FLAGH);

			// redraw sky
			if (x2 >= MASTX + MASTW) // to avoid overwrite top part of the mast
			{
				DrawImg(CloudsImg, x2, 0, x2, 0, 1, y);
				DrawImg(CloudsImg, x2, y + FLAGH, x2, y + FLAGH, 1, HEIGHT - y - FLAGH);
			}
		}

		// increase phase
		phase += WAVESPEED;
		if (phase > PI*2) phase -= PI*2;

		DispUpdate();
	}

	return 0;
}
