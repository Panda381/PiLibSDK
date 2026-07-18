
// ****************************************************************************
//
//                               Game end
//
// ****************************************************************************

#include "../include.h"

char PlayerName[TOPNAME_LEN+1] = "        ";
int PlayerNameInx = 0;

// display game end
void DispGameEnd(Bool prompt)
{
#define TOPBUFN 30
	char buf[TOPBUFN];

	// clear screen
	DrawClear();

	// display title
	DrawText4("Game Over", (WIDTH - 9*32)/2, 0, COL_RED);

	// display top list
	DispTop();

	// high score
	if (Score > MaxScore)
	{
		int y = 70;
		MemPrint(buf, TOPBUFN, "New MaxScore %07d", Score);
		DrawTextW2(buf, (WIDTH - 20*32)/2, y, COL_MAGENTA);
		if (prompt)
		{
			y += 30;
			DrawText2("Enter name and press [A] to continue", (WIDTH - 36*16)/2, y, COL_YELLOW);
			y += 30;
			DrawText4(PlayerName, (WIDTH - 8*32)/2, y, COL_AZURE);
			if (((Time() >> 18) & B0) == 0) DrawText4("_", (WIDTH - 8*32)/2 + PlayerNameInx*32, y + 8, COL_AZURE);
		}
	}
	else
	{
		MemPrint(buf, TOPBUFN, "Your Score %07d", Score);
		DrawTextW2(buf, (WIDTH - 18*32)/2, 70, COL_MAGENTA);
		if (prompt) DrawText2("Press any key...", (WIDTH - 16*16)/2, 140, COL_YELLOW);
	}

	// display update
	DispUpdate();
}

// game end
void GameEnd()
{
	int key;
	char ch;
	int strip;

	// get name of best player
	memcpy(PlayerName, Top[0].name, TOPNAME_LEN);

	// display game end
	DispGameEnd(False);

	// stop sounds
	MusicStop();
	StopAllSound();

	// wait 1 second
	WaitMs(1000);

	// flush keys
	KeyFlush();

	while (True)
	{
		// display game end
		DispGameEnd(True);

		// get key
		key = KeyGet();

		// high score
		if (Score > MaxScore)
		{
			switch (key)
			{
			case KEY_LEFT:
				if (PlayerNameInx > 0) PlayerNameInx--;
				break;

			case KEY_RIGHT:
				if (PlayerNameInx < TOPNAME_LEN-1) PlayerNameInx++;
				break;

			case KEY_UP:
				ch = PlayerName[PlayerNameInx] + 1;
				if (ch == 33)
					ch = '0';
				else if (ch == '9'+1)
					ch = 'A';
				else if (ch == 'Z'+1)
					ch = ' ';
				PlayerName[PlayerNameInx] = ch;
				break;

			case KEY_DOWN:
				ch = PlayerName[PlayerNameInx] - 1;
				if (ch == 31)
					ch = 'Z';
				else if (ch == '0'-1)
					ch = ' ';
				else if (ch == 'A'-1)
					ch = '9';
				PlayerName[PlayerNameInx] = ch;
				break;

			case KEY_SCREENSHOT:
				ScreenShot();
				break;

			// LCD display rezoom
			case KEY_ZOOM:
				LCDRezoom();
				break;

			case KEY_PAD_A:
				// move top table
				memmove(&Top[1], &Top[0], (TOP_NUM-1)*sizeof(sTop));

				// insert new score
				Top[0].score = Score;
				MaxScore = Score;
				memcpy(Top[0].name, PlayerName, TOPNAME_LEN);

				// save table
				while (!SaveTop())
				{
					// error
					DrawClear();
					DrawText4("Error writing score", 0, 100, COL_RED);
					DrawText4("to card. Press [A]", 0, 160, COL_RED);
					DrawText4("to repeat or [Y] to", 0, 220, COL_RED);
					DrawText4("ignore the error.", 0, 280, COL_RED);
					DispUpdate();

					while (True)
					{
						key = KeyGet();
						if (key == KEY_SCREENSHOT) ScreenShot();
						if (key == KEY_ZOOM) LCDRezoom();
						if (key == KEY_PAD_Y) return;
						if (key == KEY_PAD_A) break;
					}
				}
				return;

			case KEY_PAD_Y:
				return;
			}		
		}

		// no high score
		else
		{
			if (key != NOKEY) return;
		}
	}
}
