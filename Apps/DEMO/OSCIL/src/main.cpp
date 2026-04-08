
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

#include "../img/oscil.cpp"	// const u8 OscilImgJpg[] 640x348

#define OSCIL_X	0
#define OSCIL_Y	((HEIGHT-348)/2)

#define GRIDX	(OSCIL_X+66)
#define GRIDY	(OSCIL_Y+42)
#define GRIDW	260
#define GRIDH	180

u8* OscilImg;

int main()
{
	int i;
	float f1, f2, f3; // animation phase
	int lasty0, lasty, y;
	u32 t, t2, dt;

	DrawClear();

	// load image
	OscilImg = (u8*)JPGLOAD(OscilImgJpg);
	if (OscilImg == NULL) Reboot(); // emergency exit

	// main loop
	f1 = f2 = f3 = 0;
	t = Time();
	lasty0 = GRIDY+GRIDH/2;
	while (True)
	{
	// ==== draw graphics

		// draw oscilloscope
		DrawImg(OscilImg, OSCIL_X, OSCIL_Y);

		// draw samples
		lasty = lasty0;
		for (i = 1; i < GRIDW; i++)
		{
			y = GRIDY + (u8)(GRIDH*(
				sin(i*0.08 + f1)*0.25 + 
				sin(i*0.11 + f2)*0.13 + 
				sin(i*0.19 + f3)*0.08 + 
				0.5));
			DrawLine(GRIDX+i-1, lasty, GRIDX+i, y, COL_GREEN);
			lasty = y;
			if (i == 1) lasty0 = lasty;
		}

		// display update
		DispUpdate();

	// ==== shift graphics

		// delta time
		t2 = Time();
		dt = t2 - t;
		t = t2;

		// increase phases
		f1 += 0.3*dt/100000;
		f2 += 0.58*dt/100000;
		f3 += 1.22*dt/100000;

	// ==== control

		// keyboard
		int key = KeyGet();
		if (key == KEY_ESC) Reboot();	// Program exit
		if (key == KEY_SCREENSHOT) ScreenShot(); //  Screenshot - This may take a few seconds to write.
	}

	return 0;
}
