
// ****************************************************************************
//
//                                 Open screen
//
// ****************************************************************************

#include "../include.h"

const char* const OpenCharName[4] = { "SHADOW", "SPEEDY", "BASHFUL", "POKEY" };
const char* const OpenCharNick[4] = { "\"BLINKY\"", "\"PINKY\"", "\"INKY\"", "\"CLYDE\"" };
const char* const OpenScore[4] = { "200", "400", "800", "1600" };
const u32 OpenCharCol[4] = { COL_BLINKY, COL_PINKY, COL_INKY, COL_CLYDE };

u32 LastTime;

// update display and wait (returns True to exit)
Bool DispWait(u32 ms)
{
	ms *= 1000;
	u32 t;
	DispUpdate();
	do {
		t = Time(); 
		int key = KeyGet();
		if (key == KEY_PAD_Y) Reboot();
		if (key == KEY_SCREENSHOT) ScreenShot();
		if ((key == KEY_PAD_A) || (key == KEY_PAD_B) || (key == KEY_PAD_X))
		{
			StopAllSound();
			return True;
		}
	} while ((u32)(t - LastTime) < (u32)ms);
	LastTime = t;
	return False;
}

// animate pellet
void AnimPellet(int y1)
{
	Blink++;
	if (Blink <= 3)
	{
		// clear pellets
		DrawRect(120, y1, TILEW, TILEH, COL_BLACK);
	}
	else
	{
		// draw pellets
		if (Blink == 6) Blink = 0;
		DrawImg(TilesImg, 120, y1, TILE_PELLET*TILEW, 0, TILEW, TILEH);
	}
}

// open screen (break with a valid key)
void OpenScreen()
{
	int i, x, x2, y, y1, y2, phase;

	StopSound();

	// clear open screen
	DrawClear();

	// set font
	SelFont8x8();

	// draw logo
	y = 3;
	DrawImg(LogoImg, (WIDTH-LOGOW)/2, y, 0, 0, LOGOW, LOGOH);
	y += LOGOH;
	if (DispWait(200)) return;
	DrawText("for Raspberry Zero", (WIDTH-18*8)/2, y, COL_GREEN);
	y += 25;
	if (DispWait(400)) return;

	// draw title
	DrawText("CHARACTER / NICKNAME", 90, y, COL_WHITE);
	y += 20;
	if (DispWait(400)) return;

	// draw characters
	for (i = 0; i < 4; i++)
	{
		DrawImg(SpritesImg, 70, y-6, (SPRITE_BLINKY+4+i*8)*SPRITEW, 0, SPRITEW, SPRITEH);
		if (DispWait(400)) return;
		DrawText(OpenCharName[i], 110, y, OpenCharCol[i]);
		if (DispWait(200)) return;
		DrawText(OpenCharNick[i], 180, y, OpenCharCol[i]);
		if (DispWait(200)) return;
		y += 20;
	}
	if (DispWait(400)) return;
	y += 35;

	// draw dot points
	DrawImg(TilesImg, 120, y, TILE_DOT*TILEW, 0, TILEW, TILEH);
	DrawText("10 pts", 140, y+2, COL_WHITE);
	y += 20;

	// draw pellet points
	y1 = y;
	DrawImg(TilesImg, 120, y1, TILE_PELLET*TILEW, 0, TILEW, TILEH);
	DrawText("50 pts", 140, y+2, COL_WHITE);
	y -= 40;
	if (DispWait(400)) return;

	// destination pellet
	y2 = y;
	DrawImg(TilesImg, 73, y2, TILE_PELLET*TILEW, 0, TILEW, TILEH);
	if (DispWait(400)) return;

	// prepare sprites for animation
	x = 320;
	phase = 0;
	Blink = 0;

	// play sound (2-times)
	PlaySound(IntermissionSnd);

	// animate sprites to the left
	do {
		// shift sprites
		x -= CHARSPEED;

		// animate pellets
		AnimPellet(y1);
		if (Blink <= 3)
			// clear pellets
			DrawRect(73, y2, TILEW, TILEH, COL_BLACK);
		else
			// draw pellets
			DrawImg(TilesImg, 73, y2, TILE_PELLET*TILEW, 0, TILEW, TILEH);

		// draw sprites
		DrawImg(SpritesImg, x, y-6, (SPRITE_PACL+phase)*SPRITEW, 0, SPRITEW, SPRITEH);
		DrawImg(SpritesImg, x+SPRITEW, y-6, (SPRITE_BLINKY+phase)*SPRITEW, 0, SPRITEW, SPRITEH);
		DrawImg(SpritesImg, x+2*SPRITEW, y-6, (SPRITE_PINKY+phase)*SPRITEW, 0, SPRITEW, SPRITEH);
		DrawImg(SpritesImg, x+3*SPRITEW, y-6, (SPRITE_INKY+phase)*SPRITEW, 0, SPRITEW, SPRITEH);
		DrawImg(SpritesImg, x+4*SPRITEW, y-6, (SPRITE_CLYDE+phase)*SPRITEW, 0, SPRITEW, SPRITEH);
		DrawRect(x+5*SPRITEW, y-6, SPRITEW, SPRITEH, COL_BLACK);

		// animation delay
		if (DispWait(SPEED)) return;

		// increase animation phase
		phase = (phase+1) & 1;

	} while (x > 70);

	// clear eaten pellet
	DrawRect(120, y1, TILEW, TILEH, COL_BLACK);
	DrawRect(73, y2, TILEW, TILEH, COL_BLACK);

	// animate sprites to the right
	x2 = x;
	Bool eat1 = False;
	Bool eat2 = False;
	Bool eat3 = False;
	Bool eat4 = False;
	Bool wait;
	do {
		// shift sprites
		x += CHARSPEED;
		x2 += FRIGHSPEED;

		if (!PlayingSound()) PlaySound(IntermissionSnd);

		// animate pellets
		AnimPellet(y1);

		// eat ghosts
		wait = False;
		DrawRect(x-SPRITEW, y-6, 2*SPRITEW, SPRITEH, COL_BLACK);
		if ((x2 < x - SPRITEW/2) && !eat1)
		{
			DrawImg(SpritesImg, x2+SPRITEW, y-6, SPRITE_200*SPRITEW, 0, SPRITEW, SPRITEH);
			wait = True;
			eat1 = True;
		}
		else if ((x2 < x - SPRITEW/2-1*SPRITEW) && !eat2)
		{
			DrawImg(SpritesImg, x2+2*SPRITEW, y-6, SPRITE_400*SPRITEW, 0, SPRITEW, SPRITEH);
			wait = True;
			eat2 = True;
		}
		else if ((x2 < x - SPRITEW/2-2*SPRITEW) && !eat3)
		{
			DrawImg(SpritesImg, x2+3*SPRITEW, y-6, SPRITE_800*SPRITEW, 0, SPRITEW, SPRITEH);
			wait = True;
			eat3 = True;
		}
		else if ((x2 < x - SPRITEW/2-3*SPRITEW) && !eat4)
		{
			DrawImg(SpritesImg, x2+4*SPRITEW, y-6, SPRITE_1600*SPRITEW, 0, SPRITEW, SPRITEH);
			wait = True;
			eat4 = True;
		}

		// draw sprites
		if (!eat1) DrawImg(SpritesImg, x2+SPRITEW, y-6, (SPRITE_FRIGH+phase)*SPRITEW, 0, SPRITEW, SPRITEH);
		if (!eat2) DrawImg(SpritesImg, x2+2*SPRITEW, y-6, (SPRITE_FRIGH+phase)*SPRITEW, 0, SPRITEW, SPRITEH);
		if (!eat3) DrawImg(SpritesImg, x2+3*SPRITEW, y-6, (SPRITE_FRIGH+phase)*SPRITEW, 0, SPRITEW, SPRITEH);
		if (!eat4) DrawImg(SpritesImg, x2+4*SPRITEW, y-6, (SPRITE_FRIGH+phase)*SPRITEW, 0, SPRITEW, SPRITEH);

		// draw pacman
		if (!wait) DrawImg(SpritesImg, x, y-6, (SPRITE_PACR+phase)*SPRITEW, 0, SPRITEW, SPRITEH);

		// animation delay
		if (DispWait(SPEED)) return;
		if (wait)
		{
			for (i = 1000/SPEED; i > 0; i--)
			{
				// animate pellets
				AnimPellet(y1);
				if (DispWait(SPEED)) return;
			}
		}

		// increase animation phase
		phase = (phase+1) & 1;

	} while (x < WIDTH);

	// wait for sound
	while (PlayingSound())
	{
		// animate pellets
		AnimPellet(y1);
		if (DispWait(SPEED)) return;
	}
}
