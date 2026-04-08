
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

#include "../img/CobbleStone.cpp"	// const u8 CobbleStoneImgJPG[]
#include "../img/Fabric.cpp"		// const u8 FabricImgJPG[]
#include "../img/Soil.cpp"		// const u8 SoilImgJPG[]
#include "../img/Wood.cpp"		// const u8 WoodImgJPG[]

#define IMGWIDTH 256		// image width
#define IMGHEIGHT 256		// image height
#define IMGWIDTHB ((IMGWIDTH+1)/2) // bytes per line of image

#define TWISTW	200		// twister width

#define MIDX	(WIDTH/2) // middle X coordinate

u8 *CobbleStoneImg, *FabricImg, *SoilImg, *WoodImg;

int main()
{
	// decompress JPG images
	CobbleStoneImg = (u8*)JPGLOAD(CobbleStoneImgJPG);
	if (CobbleStoneImg == NULL) Reboot(); // emergency exit
	FabricImg = (u8*)JPGLOAD(FabricImgJPG);
	if (FabricImg == NULL) Reboot(); // emergency exit
	SoilImg = (u8*)JPGLOAD(SoilImgJPG);
	if (SoilImg == NULL) Reboot(); // emergency exit
	WoodImg = (u8*)JPGLOAD(WoodImgJPG);
	if (WoodImg == NULL) Reboot(); // emergency exit

	// variables
	float aa;
	float ang; // base angle
	float amp; // wave amplitude
	float yamp;
	int y, ys, x, x1, x2, x3, x4;

	// main loop
	aa = 0; // angle counter
	while (True)
	{
		ang = (sinf(aa)-1)*PI;
		amp = sinf(cosf(aa)*0.3)*2000 + 700;

		// draw twister
		for (y = 0; y < HEIGHT; y++)
		{
			yamp = y/amp + ang;
			ys = y & (IMGHEIGHT-1);

			// 4 edges
			x1 = sinf(yamp)*TWISTW + MIDX;
			x2 = sinf(yamp+PI/2)*TWISTW + MIDX;
			x3 = sinf(yamp+PI)*TWISTW + MIDX;
			x4 = sinf(yamp+PI*3/2)*TWISTW + MIDX;

			// draw 4 segment on one line
			if (x2 - x1 > 1) DrawImgLine(CobbleStoneImg, x1, y, x2-x1, 0, ys, IMGWIDTH);
			if (x3 - x2 > 1) DrawImgLine(FabricImg, x2, y, x3-x2, 0, ys, IMGWIDTH);
			if (x4 - x3 > 1) DrawImgLine(SoilImg, x3, y, x4-x3, 0, ys, IMGWIDTH);
			if (x1 - x4 > 1) DrawImgLine(WoodImg, x4, y, x1-x4, 0, ys, IMGWIDTH);

			// clear left part of the screen
			x = x1;
			if (x2 < x) x = x2;
			if (x3 < x) x = x3;
			if (x4 < x) x = x4;
			DrawRect(0, y, x, 1, COL_BLACK);

			// clear right part of the screen
			x = x1;
			if (x2 > x) x = x2;
			if (x3 > x) x = x3;
			if (x4 > x) x = x4;
			DrawRect(x, y, WIDTH-x, 1, COL_BLACK);
		}

		// increment angle of animation
		aa += 0.005f;

		// display update
		DispUpdate();

		// keyboard
		int key = KeyGet();
		if (key == KEY_ESC) Reboot();	// Program exit
		if (key == KEY_SCREENSHOT) ScreenShot(); //  Screenshot - This may take a few seconds to write.
	}

	return 0;
}
