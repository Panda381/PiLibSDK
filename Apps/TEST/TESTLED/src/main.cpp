
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

#include "../img/testled.cpp"	// const u8 TestLedImgJpg[]

u8 *TestLedImg;

// draw one LED on the screen
void DrawLed(int inx, Bool on)
{
	inx += 2;
	int y = inx & 0x03;
	int x = inx/4*63 + y*16 + 44;
	y = y*69 + 147;
	DrawRound(x, y, 24, DRAW_ROUND_ALL, COL_BLACK);
	if (on) DrawRound(x, y, 22, DRAW_ROUND_ALL, COL_RED);
	DispUpdate();
}

int main()
{
	int i;

	// decompress JPG image
	TestLedImg = (u8*)JPGLOAD(TestLedImgJpg);
	if (TestLedImg == NULL) Reboot(); // emergency exit

	// keyboard service terminate
	KeyTerm();

	// setup all GPIOs
	for (i = 27; i != 0; i--)
	{
		GPIO_Func(i, GPIO_FUNC_IN);
		GPIO_Pull(i, GPIO_PULL_UP);
		DrawLed(i, False);
	}

	// draw image (format: CF_A8)
	DrawClear();
	DrawImgMask(TestLedImg);

	// display update
	DispUpdate();

	// wait
	while (True)
	{
		// set new pin ON
		GPIO_Func(i, GPIO_FUNC_OUT);
		GPIO_Out0(i);
		DrawLed(i, True);

		// delay
		WaitMs(500);

		// set last pin OFF
		GPIO_Func(i, GPIO_FUNC_IN);
		GPIO_Pull(i, GPIO_PULL_UP);
		DrawLed(i, False);

		// increase pin index
		i++;
		if (i >= 28) i = 0;
	}
}
