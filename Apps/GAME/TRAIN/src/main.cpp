
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#include "../include.h"

#include "../img/intro_cz.cpp"	// const u8 IntroCzImgJPG[] ... JPG
#include "../img/intro_en.cpp"	// const u8 IntroEnImgJPG[] ... JPG
#include "../img/light1.cpp"	// const u8 Light1ImgPNG[] ... PNG
#include "../img/light2.cpp"	// const u8 Light2ImgPNG[] ... PNG
#include "../img/tiles.cpp"	// const u8 TilesImgPNG[] ... PNG

#include "../snd/collect.cpp"	// const u8 CollectSnd[]
#include "../snd/crash.cpp"	// const u8 CrashSnd[]
#include "../snd/step.cpp"	// const u8 StepSnd[]
#include "../snd/success.cpp"	// const u8 SuccessSnd[]
#include "../snd/train.cpp"	// const u8 TrainSnd[]

u8 *IntroCzImg, *IntroEnImg, *Light1Img, *Light2Img, *TilesImg;

// language
int Lang = LANG_EN;		// current language LANG_*

int main()
{
	int i;
	u8 ch;

	// decompress images
	IntroCzImg = (u8*)JPGLOAD(IntroCzImgJPG); if (IntroCzImg == NULL) Reboot();
	IntroEnImg = (u8*)JPGLOAD(IntroEnImgJPG); if (IntroEnImg == NULL) Reboot();
	Light1Img = (u8*)PNGLOAD(Light1ImgPNG); if (Light1Img == NULL) Reboot();
	Light2Img = (u8*)PNGLOAD(Light2ImgPNG); if (Light2Img == NULL) Reboot();
	TilesImg = (u8*)PNGLOAD(TilesImgPNG); if (TilesImg == NULL) Reboot();

	// set first scene
	Level = LEVFIRST;
	Score = 0;

PlayIntro:

	// intro screen
	PlaySoundRep(TrainSnd);
	DrawImg(IntroEnImg);
#define LANG_X	30
#define LANG_Y	(HEIGHT-32-4)
	if (Lang == LANG_EN)
	{
		DrawImg(TilesImg, LANG_X, LANG_Y, 448, 320, 4*32, 32);
	}
	else
	{
		DrawImg(IntroCzImg, 150, 30);
		DrawImg(TilesImg, LANG_X, LANG_Y, 448, 288, 4*32, 32);
	}
	DrawImg(Light1Img, 512, 320);
	DispUpdate();

	// wait a key (max. 20 seconds)
	u32 t1 = Time();
	WaitMs(200);
	KeyFlush();
	Bool light1 = True;
	u32 light_t = Time();
	while ((u32)(Time() - t1) < 15000000)
	{
		ch = KeyGet();

		// exit
		if (ch == KEY_PAD_Y)
			Reboot();

		// screenshot
		else if (ch == KEY_SCREENSHOT)
		{
			ScreenShot();
			t1 = Time();
		}

		// LCD zoom
		else if (ch == KEY_ZOOM)
		{
			LCDRezoom();
			t1 = Time();
		}

		// select language
		else if (ch == KEY_PAD_X)
		{
			DrawImg(IntroEnImg);
			if (Lang == LANG_EN)
			{
				Lang = LANG_CZ;
				DrawImg(IntroCzImg, 150, 30);
				DrawImg(TilesImg, LANG_X, LANG_Y, 448, 288, 4*32, 32);
			}
			else
			{
				Lang = LANG_EN;
				DrawImg(TilesImg, LANG_X, LANG_Y, 448, 320, 4*32, 32);
			}
			t1 = Time();
		}
		else if ((ch == KEY_PAD_B) || (ch == KEY_PAD_A) || (ch == KEY_PAD_X))
			break;

		// traffic lights
		if ((u32)(Time() - light_t) >= 500000)
		{
			light1 = !light1;
			light_t = Time();
		}
		DrawImg(light1 ? Light1Img : Light2Img, 512, 320);

		// display update
		DispUpdate();
	}
	StopAllSound();

	// clear screen
	DrawClear();
	DispUpdate();

	// loop with demo scene
	while (True)
	{
		// demo
		i = Level; // save current level

		while (True)
		{
			// play demo scene 0
			Level = 0;

			// help level (returns 0=continue, 1=start game, 2=exit)
			int res = HelpLevel();
			if (res == 2)
			{
				Level = i;
				goto PlayIntro;
			}
			if (res == 1) break; // break

#if AUTOMODE	// 1=automode - run levels to check solutions (help completes scene)
			break;
#endif

			// play random scene
			Level = RandU8MinMax(1, LEVNUM);
			HelpLevel();
		}

		// return current level
		Level = i;

		// game main loop
		InitLevel();
		while (True)
		{
			// game loop - called every single game step
			if (GameLoop()) break;

			// wait step
			WaitStep();
		}
	}
}
