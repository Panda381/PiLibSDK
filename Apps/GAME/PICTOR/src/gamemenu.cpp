
// ****************************************************************************
//
//                               Game menu
//
// ****************************************************************************

#include "../include.h"

// request to do screenshot
Bool ReqScreenShot = False;

// home directory
char HomePath[PATH_MAX+1];
char HomePathFilename[13];
int HomePathLen;

sTop Top[TOP_NUM];	// top list

// load top list (returns max. score)
s32 LoadTop()
{
	// reset top list
	int i;
	memset(Top, ' ', sizeof(Top));
	for (i = 0; i < TOP_NUM; i++) Top[i].score = 0;

	// mount disk if not mounted
	DiskAutoMount();

	// get home path (and set is as current directory)
	SetDir(HomePath);

	// load top file
	sFile file;
	if (FileOpen(&file, TOPLIST_NAME))
	{
		FileRead(&file, Top, sizeof(Top));
		FileClose(&file);
	}

	// get max. score
	return Top[0].score;
}

// save top list (returns False on error)
Bool SaveTop()
{
	// mount disk if not mounted
	DiskAutoMount();

	// set home directory
	SetDir(HomePath);

	// write top file
	sFile file;
	if (!FileOpen(&file, TOPLIST_NAME) && !FileCreate(&file, TOPLIST_NAME)) return False;

	// write file
	int k = FileWrite(&file, Top, sizeof(Top));

	// close file
	FileClose(&file);

	return (k == sizeof(Top));
}

// display top score
void DispTop()
{
#define TOPBUFN 30
	char buf[TOPBUFN];
	char namebuf[TOPNAME_LEN+1];

	// display top list
	int y = 210;
	DrawTextW2("Top List", (WIDTH - 8*32)/2, y, COL_GREEN);
	y += 30;

	int i; // y=240-8*15 = 120
	for (i = 0; i < TOP_NUM; i++)
	{
		memcpy(namebuf, Top[i].name, TOPNAME_LEN);
		namebuf[TOPNAME_LEN] = 0;
		MemPrint(buf, TOPBUFN, "%d: %07d %s", i+1, Top[i].score, namebuf);
		DrawTextW2(buf, 0, y, COL_WHITE);
		y += 30;
	}
}

// game menu (returns False to exit)
Bool GameMenu()
{
	// stop sounds
	MusicStop();
	StopAllSound();

	// flush keys
	KeyFlush();

	while (True)
	{
		// clear screen
		DrawClear();

		// display title
		int y = 0;
		DrawText4("Game Menu", (WIDTH - 9*32)/2, y, COL_GREEN);
		y += 64;

		// display menu
#if USE_ZEROPC
		DrawTextW2("Esc", 0, y, MENU_COL_KEY);
#else
		DrawTextW2("Y", 0, y, MENU_COL_KEY);
#endif
		DrawTextW2("Continue", 192, y, MENU_COL_MENU);
		y += 30;

#if USE_ZEROPC
		DrawTextW2("Tab", 0, y, MENU_COL_KEY);
#else
		DrawTextW2("X", 0, y, MENU_COL_KEY);
#endif
		DrawTextW2("Exit", 192, y, MENU_COL_MENU);
		y += 30;

#if USE_ZEROPC
		DrawTextW2("Enter", 0, y, MENU_COL_KEY);
#else
		DrawTextW2("B", 0, y, MENU_COL_KEY);
#endif
		DrawTextW2("Sound is", 192, y, MENU_COL_MENU);
		DrawTextW2((SoundMode == SOUNDMODE_OFF) ? "off" : ((SoundMode == SOUNDMODE_SOUND) ? "ON" : "ALL"), 15*32, y, MENU_COL_MENU);
		y += 30;

#if USE_ZEROPC
		DrawTextW2("Space", 0, y, MENU_COL_KEY);
#else
		DrawTextW2("A", 0, y, MENU_COL_KEY);
#endif
		DrawTextW2("Screenshot", 192, y, MENU_COL_MENU);

		// display top list
		DispTop();

		// display update
		DispUpdate();

		// keyboard
		switch (KeyGet())
		{
		// exit
		case KEY_TAB:
			return False;

		// sound
		case KEY_ENTER:
			SoundMode++;
			if (SoundMode > 2) SoundMode = 0;
			break;

		// screen shot
		case KEY_SPACE:
			// request to do screenshot
			ReqScreenShot = True;
			while (KeyPressed(KEY_SPACE)) {}
			return True;

		case KEY_SCREENSHOT:
			ScreenShot();
			break;

		// continue
		case KEY_ESC:
			return True;
		}
	}
}
