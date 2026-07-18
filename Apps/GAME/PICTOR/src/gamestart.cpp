
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#include "../include.h"

// display game start
void DispGameStart(Bool prompt)
{
	// start position, shift animation
	ActorX = ActorMinX;
	if (Actor.walk)
	{
		// Jill
		if (ActInx == ACT_JILL)
		{
			// Y of walking actor
			ActorY = ActorMaxY + 12;

			// set phase of standing actor animation
			ActorPhase = JILL_STAND;
		}

		// other walking actors
		else
		{
			// Y of walking actor
			ActorY = ActorMaxY;

			// set phase of standing actor animation
			ActorPhase = 0;
		}
	}
	else
	{
		ActorY = (ActorMinY + ActorMaxY)/2; // Y of flying actor

		// shift flying actor animation
		ShiftActor();
	}

	// display shadow backgrounds
	DrawImg(Back.img1,	 	// image data and palettes
		0, BG_Y,			// destination X, Y
		0, 0,				// source X, Y
		WIDTH, BG_HEIGHT);		// width, height

	DrawImg(Back.img2,	 	// image data and palettes
		0, BG2_Y+Back.dy,		// destination X, Y
		0, 0,				// source X, Y
		WIDTH, BG2_HEIGHT-Back.dy);	// width, height

	DrawImg(Back.img3,	 	// image data and palettes
		0, BG3_Y,			// destination X, Y
		0, 0,				// source X, Y
		WIDTH, BG3_HEIGHT);		// width, height

	// display header
	DispHeader();

	// display foot
	DispFoot();

	// shadow screen
	DispShadow();

	// display actor
	DispActor();

#define OPEN_X	380
	int y = (Level < BG_NUM) ? 40 : 120;

	// Title
	int len = DecNum(DecNumBuf, Level + 1, 0);
	len += 16; // add text "Entering Sector "
	int x = OPEN_X - len*16/2;
	DrawText2("Entering Sector", x, y, COL_WHITE);
	x += 16*16;
	DrawText2(DecNumBuf, x, y, COL_WHITE);
	y += 32;

	len = StrLen(Back.name);
	DrawText4(Back.name, OPEN_X - len*32/2, y, COL_AZURE);

	// new helper
	if (Level < BG_NUM)
	{
		y += 64;
		const sActorTemp* a = &ActorTemp[BackInx+1];
		y += (244 - (32 + 32 + a->h + 32))/2;

		// Helper
		DrawText2("New Helper", OPEN_X - 10*16/2, y, COL_WHITE);
		y += 32;
		len = StrLen(a->name);
		DrawTextW2(a->name, OPEN_X - len*32/2, y, COL_GREEN);
		y += 32;

		// draw helper
		DrawImg(	a->img,			// source image data
				OPEN_X - a->w/2,	// destination X
				y,			// destination Y
				0,			// source X
				0,			// source Y
				a->w,			// image width
				a->h);			// image height
		y += a->h;

		// Attack
		len = StrLen(a->missile) + 7;
		x = OPEN_X  - len*16/2;
		DrawText2("Attack:", x, y, COL_RED);
		DrawText2(a->missile, x+7*16, y, COL_RED);
	}

	// help
	if (prompt)
	{
		DrawText2("Press [B]/[X] to transform", OPEN_X - 26*16/2, 380, COL_YELLOW);
		DrawText2("Press [A] to start game", OPEN_X - 23*16/2, 410, COL_YELLOW);
	}

	// display update
	DispUpdate();
}

// open game start (return False to break)
Bool GameStart()
{
	// display game start
	DispGameStart(False);

	// wait 1 second
	WaitMs(1000);

	// flush keys
	KeyFlush();

	while (True)
	{
		// display game start
		DispGameStart(True);

		// key control
		switch (KeyGet())
		{
		// start game
		case KEY_PAD_A:
			while (KeyPressed(KEY_PAD_A)) {}
			return True;

		// next actor
		case KEY_PAD_B:
			SetActor(ActInx+1);
			ActorReload = 0;
			break;

		// previous actor
		case KEY_PAD_X:
			SetActor(ActInx-1);
			ActorReload = 0;
			break;

		case KEY_SCREENSHOT:
			ScreenShot();
			break;

		// LCD display rezoom
		case KEY_ZOOM:
			LCDRezoom();
			break;

		// menu
		case KEY_PAD_Y:
			if (!GameMenu()) return False;
			break;
		}

		WaitMs(45);
	}
}
