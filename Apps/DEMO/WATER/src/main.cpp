
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

#include "../img/forest.cpp"	// const u8 ForestImgJPG[]
#include "../img/duck.cpp"	// const u8 DuckImgPNG[]

#include "../snd/river.cpp"	// const u8 RiverSnd[]

#define IMGH	320	// image height
#define WATERH	(HEIGHT-IMGH) // water height

u8 *ForestImg, *DuckImg;

int main()
{
	u32 t0, t, dt;

	// decompress images
	ForestImg = (u8*)JPGLOAD(ForestImgJPG);
	if (ForestImg == NULL) Reboot(); // emergency exit
	DuckImg = (u8*)PNGLOAD(DuckImgPNG);
	if (DuckImg == NULL) Reboot(); // emergency exit

	float phase, duckx2;
	int y, y0, y2, duckx;

	// play sound
	PlaySoundRep(RiverSnd);

	// display top part of the screen
	DrawImg(ForestImg, 0, 0);

	// main loop
	phase = 0;
	duckx2 = 660;
	t0 = Time();
	while (True)
	{
		// animate water
		for (y = IMGH; y < HEIGHT; y++)
		{

#define WAVEAMP		0.2f	// wave amplitude
#define WAVEPERSP	20.0f	// perspective deformation
#define WAVELEN		200	// wave length
#define ANIMSPEED	0.02f	// animation speed

			y0 = y - IMGH; // Y relative to top of water
			y2 = (int)(IMGH - 1 - (sinf(phase + WAVELEN/sqrtf(y0/WAVEPERSP + 1))*WAVEAMP + 1)*y0);
			if (y2 < 0) y2 = 0;
			if (y2 >= IMGH) y2 = IMGH-1;
			DrawImg(ForestImg, 0, y, 0, y2, WIDTH, 1);
		}

		// display duck
		duckx = float2int(duckx2 + 0.5f);
		DrawImg(DuckImg, duckx, 360);

		// delta time
		t = Time();
		dt = t - t0;
		t0 = t;

		// increase animation phase
		phase += ANIMSPEED/10000*dt;

		// shift duck
		duckx2 -= 8*ANIMSPEED/10000*dt;
		if (duckx2 < -200) duckx2 = 660;

		// display update
		DispUpdate();

		// keyboard
		int key = KeyGet();
		if (key == KEY_PAD_Y) Reboot();	// Program exit
		if (key == KEY_SCREENSHOT) ScreenShot(); //  Screenshot - This may take a few seconds to write.
		if (key == KEY_ZOOM) LCDRezoom();	// LCD display rezoom
	}

	return 0;
}
