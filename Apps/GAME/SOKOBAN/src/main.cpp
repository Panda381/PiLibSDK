
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#include "../include.h"

u8* IntroImg;
u8* Tiles4Img;
u8* Tiles8Img;
u8* Tiles12Img;
u8* Tiles16Img;
u8* Tiles20Img;
u8* Tiles24Img;
u8* Tiles28Img;
u8* Tiles32Img;

// wait for a character
char WaitChar()
{
	char c;
	while ((c = KeyGet()) == NOKEY) {}
	return c;
}

#if AUTOMODE		// 1=automode, autorun all levels to check solutions, 0=normal
// DEBUG: auto mode, to check all levels
void AutoMode()
{
	int abslevel = 1;	// absolute level
	char buf[12];

	// select first author
	AutInx = 0;
	AutName = Author[0].author; // author's name
	Collect = Author[0].collect; // pointer to list of collections
	CollNum = Author[0].collnum; // number of collections

	// select first collection
	CollInx = 0;
	CollName = Collect[0].name; // name of collection
	Levels = Collect[0].levels; // pointer to list of levels
	LevNum = Collect[0].levnum; // number of levels

	// select first level
	Level = 0;
	LevDef = Levels[0]; // current level definition
	LevSolve = Levels[1]; // current level solve

	// check all levels	
	for (;;)
	{
		// display level info
		LevelInfo();

		// display absolute level number
		DecNum(buf, abslevel, 0);
		DRAWTEXT(buf, 0, 5*FONTH*DISPZOOM, COL_YELLOW);
		DispUpdate();
		WaitMs(100);

		// solve level
		PlaySolve();
		WaitMs(100);

		// check if solved
		if (MarkNum != 0)
		{
			LevelInfo();
			DRAWTEXT("ERROR!", 0, 5*FONTH*DISPZOOM, COL_RED);
			DispUpdate();
			break;
		}

		// increase scene level
		Level++;
		if (Level >= LevNum)
		{
			// next collection of current author
			int c = CollInx + 1;
			if (c < CollNum)
			{
				CollInx = c; // select next collection
				Level = 0; // selected level
				CollName = Collect[c].name; // name of collection
				Levels = Collect[c].levels; // pointer to list of levels
				LevNum = Collect[c].levnum; // number of levels
			}

			// else next author
			else
			{
				// next author
				int a = AutInx + 1;
				if (a < AutNum)
				{
					AutInx = a;
					CollInx = 0; // selected collection
					AutName = Author[a].author; // author's name
					Collect = Author[a].collect; // pointer to list of collections
					CollNum = Author[a].collnum; // number of collections

					Level = 0; // selected level
					CollName = Collect[0].name; // name of collection
					Levels = Collect[0].levels; // pointer to list of levels
					LevNum = Collect[0].levnum; // number of levels
				}
				// else stop
				else
				{
					SetTileMode(0);
					SelFont8x16();
					DecNum(buf, abslevel, 0);
					DRAWTEXT(buf, 0, 0, COL_YELLOW);
					DrawText2("ALL OK", 100*DISPZOOM, 6*FONTH*DISPZOOM, COL_GREEN);
					DispUpdate();
					break;
				}
			}
		}
		abslevel++;
	}
}
#endif // AUTOMODE

int main()
{
	int i, key;

	// decompress images
#if DISPZOOM == 1			// display zoom: 1=use 320x240, 2=use 640x480
	IntroImg = (u8*)JPGLOAD(IntroImg320JPG); if (IntroImg == NULL) Reboot();
#else
	IntroImg = (u8*)JPGLOAD(IntroImgJPG); if (IntroImg == NULL) Reboot();
#endif
	Tiles4Img = (u8*)PNGLOAD(Tiles4ImgPNG); if (Tiles4ImgPNG == NULL) Reboot();
	Tiles8Img = (u8*)PNGLOAD(Tiles8ImgPNG); if (Tiles8ImgPNG == NULL) Reboot();
	Tiles12Img = (u8*)PNGLOAD(Tiles12ImgPNG); if (Tiles12ImgPNG == NULL) Reboot();
	Tiles16Img = (u8*)PNGLOAD(Tiles16ImgPNG); if (Tiles16ImgPNG == NULL) Reboot();
	Tiles20Img = (u8*)PNGLOAD(Tiles20ImgPNG); if (Tiles20ImgPNG == NULL) Reboot();
	Tiles24Img = (u8*)PNGLOAD(Tiles24ImgPNG); if (Tiles24ImgPNG == NULL) Reboot();
	Tiles28Img = (u8*)PNGLOAD(Tiles28ImgPNG); if (Tiles28ImgPNG == NULL) Reboot();
	Tiles32Img = (u8*)PNGLOAD(Tiles32ImgPNG); if (Tiles32ImgPNG == NULL) Reboot();

	// display intro image
	PlaySound(ShiftSnd);
	DrawImg(IntroImg);
	u32 t1 = Time();
	DispUpdate();
	WaitMs(200);

	// wait a key (max. 5 seconds)
	KeyFlush();
	while ((u32)(Time() - t1) < 5000000)
	{
		key = KeyGet();
		if (key == KEY_PAD_Y)
			Reboot();
		else if (key == KEY_SCREENSHOT)
			ScreenShot();
		else if (key == KEY_ZOOM)
			LCDRezoom();
		else if ((key == KEY_PAD_B) || (key == KEY_PAD_A) || (key == KEY_PAD_X))
			break;
	}

	// clear screen
	DrawClear();
	DispUpdate();

	// DEBUG: auto mode, to check all levels
#if AUTOMODE		// 1=automode, autorun all levels to check solutions, 0=normal
	AutoMode();
#else
	// set text mode
	SetTileMode(0);
	DispUpdate();

	// main loop
	AutInx = 0;
	while (True)
	{
		int oldaut = AutInx;

		// play demo level
		while (True)
		{
			// play demo scene
			LevNum = 0;
			Levels = IntroLevel;
			Level = 0;
			if (PlaySolve()) break;
			SolvedAnim(False);

			// play random scene
			do {
				i = RandU8Max(AutNum-1);
				AutInx = i;
				AutName = Author[i].author; // author's name
				Collect = Author[i].collect; // pointer to list of collections
				CollNum = Author[i].collnum; // number of collections

				i = RandU8Max(CollNum-1);
				CollInx = i;
				CollName = Collect[i].name; // name of collection
				Levels = Collect[i].levels; // pointer to list of levels
				LevNum = Collect[i].levnum; // number of levels

				i = RandU16Max(LevNum-1);
				Level = i;
				LevDef = Levels[i*2]; // current level definition
				LevSolve = Levels[i*2+1]; // current level solve

				BoardDim();

			} while ((LevelW > 15) || (LevelH > 12));
			if (PlaySolve()) break;
			SolvedAnim(False);
		}

		if (KeyPressed(KEY_PAD_Y)) Reboot();

		AutInx = oldaut;

		// select author
		while (AuthorSelect())
		{
			// select collection
			while (CollSelect())
			{
				// select level
				while (LevSelect())
				{
					// game loop
					GameLoop();
				}
			}
		}
	}
#endif // AUTOMODE
}
