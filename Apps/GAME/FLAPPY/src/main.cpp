
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

// MP3 player
sMP3Player MP3Player;
u8 ALIGNED MP3PlayerOutBuf[MP3PLAYER_OUTSIZE];
Bool MP3WasInit = False;

// play music
void MusicPlay()
{
	MusicStop();
	MP3WasInit = (MP3PlayerInit(&MP3Player, NULL, FlappySnd, sizeof(FlappySnd), MP3PlayerOutBuf, MP3PLAYER_OUTSIZE, -1) == ERR_MP3_OK);
	if (MP3WasInit) MP3Play(&MP3Player, 0, True);
}

// stop music
void MusicStop()
{
	if (MP3WasInit)
	{
		MP3PlayerTerm(&MP3Player);
		MP3WasInit = False;
	}
}

// poll music
void MusicPoll()
{
	if (MP3WasInit) MP3Poll(&MP3Player);
}

// main function
int main()
{
	// intro screen
	DrawImg(IntroImg);
	DispUpdate();

	// wait a key (max. 5 seconds)
	u32 t1 = Time();
	WaitMs(200);
	KeyFlush();
	while ((u32)(Time() - t1) < 5000000)
	{
		int key = KeyGet();
		if (key == KEY_ESC)
			Reboot();
		else if (key == KEY_SCREENSHOT)
			ScreenShot();
		else if ((key == KEY_ENTER) || (key == KEY_SPACE) || (key == KEY_TAB))
			break;
	}

	// select scene set (returns True = OK, False = Esc)
	while (SetSelect())
	{
		// select scene (returns True = OK, False = Esc)
		while (LevSelect())
		{
			NewGame(True);

			// game loop
			while (GameLoop())
			{
				// continue to next scene
				SceneInx++;
				if (SceneInx >= SceneSetNum) break;
				NewGame(False);
			}
		}
	}

	Reboot();
	return 0;
}
