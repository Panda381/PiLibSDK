
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

// images
#include "../img/computer.cpp"		// const u8 ComputerImgPNG[] 24x20
#include "../img/frame.cpp"		// const u8 FrameImgPNG[] 160*480
#include "../img/open.cpp"		// const u8 OpenImgJPG[] 640*480
#include "../img/select.cpp"		// const u8 SelectImgPNG[] 160*480
#include "../img/tiles.cpp"		// const u8 TilesImgPNG[] 60*900 (1x15 tiles, 1 tile is 60x60)

// sounds
#include "../snd/beep.cpp"		// const u8 BeepSnd[] PCM mono 16-bit 22050Hz
#include "../snd/beep2.cpp"		// const u8 Beep2Snd[] PCM mono 16-bit 22050Hz
#include "../snd/maple.cpp"		// const u8 MapleSnd[] MP3

// MP3 player
sMP3Player MP3Player;
u8 ALIGNED MP3PlayerOutBuf[MP3PLAYER_OUTSIZE];

// images
u8 *ComputerImg, *FrameImg, *OpenImg, *SelectImg, *TilesImg;

// tiles
#define TILEW	60	// tile width
#define TILEH	60	// tile height
#define TILESIZE (TILEW*TILEH) // tile size in pixels

// game board (size 480x480 pixels)
#define BOARDW	8	// game board width
#define BOARDH	8	// game board height
#define BOARDSIZE (BOARDW*BOARDH) // board size with fence

// frame
#define FRAMEW	160	// frame width
#define FRAMEH	480	// frame height
#define FRAMEX	480	// frame X coordinate
#define FRAMEY	240	// player 2 offset

// computer image
#define	COMPW	24	// computer width
#define COMPH	20	// computer height
#define COMPX	(16+FRAMEX) // computer X offset
#define COMPY	22	// computer Y offset

// select frame
#define SELECTW	160	// select width
#define SELECTH	100	// select height
#define SELECTX	FRAMEX	// select X offset
#define SELECTY	0	// select Y offset

// score
#define SCOREX	(70+FRAMEX) // score X offset
#define SCOREY	128	// score Y offset
#define WINX	(70+FRAMEX) // win X offset
#define WINY	192	// win Y offset
#define SCORECOL COL_WHITE // score color
#define SCOREBG COLOR(72,80,255) // score background

// tiles
#define TILE_BLACK	0	// black

#define TILE_EMPTY	1	// empty
#define TILE_DOT	2	// dot
#define TILE_EGG	3	// egg
#define TILE_CHICKEN	4	// chicken
#define TILE_HEN	5	// hen
#define TILE_EGGWIN	6	// egg win
#define TILE_HENWIN	7	// hen win

#define TILE_EMPTY2	8	// empty with frame
#define TILE_DOT2	9	// dot with frame
#define TILE_EGG2	10	// egg with frame
#define TILE_CHICKEN2	11	// chicken with frame
#define TILE_HEN2	12	// hen with frame
#define TILE_EGGWIN2	13	// egg win with frame
#define TILE_HENWIN2	14	// hen win with frame

#define TILE_NUM	15	// number of tiles

#define TILE_OFFMIN	TILE_EMPTY // first tile without cursor
#define TILE_OFFMAX	TILE_HENWIN // last tile without cursor
#define TILE_ONMIN	TILE_EMPTY2 // first tile without cursor
#define TILE_ONMAX	TILE_HENWIN2 // last tile without cursor

// game board
u8 Board[BOARDSIZE];

// directions
const s8 DirX[8] = {  1,  1,  0, -1, -1, -1,  0,  1 };
const s8 DirY[8] = {  0,  1,  1,  1,  0, -1, -1, -1 };

// players
int Stones[2];	// number of stones (player 1: number of eggs, player 2: number of hens)
int Wins[2];	// number of wins
Bool Comp[2];	// player is computer
u8 Player;	// active player (0 or 1, 2=no player)
u8 CurX, CurY;	// cursor position
u8 CurBlink;	// blinking cursor
u8 Pos;		// number of possible positions
u8 Free;	// number of free positions
Bool Demo = False; // demo mode

// draw board
void DrawBoard()
{
	int i, j, x, y, b;
	for (i = 0; i < BOARDH; i++)
	{
		for (j = 0; j < BOARDW; j++)
		{
			b = Board[i*BOARDW + j];
			x = j*TILEW;
			y = i*TILEH;
			DrawImg(TilesImg, x, y, 0, b*TILEH, TILEW, TILEH);
		}
	}
	DispUpdate();
}

// get board on coordinates
u8 GetBoard(s8 x, s8 y)
{
	if ((x < 0) || (y < 0) || (x >= BOARDW) || (y >= BOARDH)) return TILE_BLACK;
	return Board[x + y*BOARDW];
}

// set board on coordinates (adds game start coordinates)
void SetBoard(s8 x, s8 y, u8 tile)
{
	Board[x + y*BOARDW] = tile;
}

// set cursor on
void CurOn()
{
	u8 c = GetBoard(CurX, CurY);
	if ((c >= TILE_OFFMIN) && (c <= TILE_OFFMAX))
	{
		SetBoard(CurX, CurY, c - TILE_OFFMIN + TILE_ONMIN);
	}
	CurBlink = 0;
}

// set cursor off
void CurOff()
{
	u8 c = GetBoard(CurX, CurY);
	if ((c >= TILE_ONMIN) && (c <= TILE_ONMAX))
	{
		SetBoard(CurX, CurY, c - TILE_ONMIN + TILE_OFFMIN);
	}
}

// discovery stones in one direction (returns number of stones; cursor must be off)
u8 GetStones1(s8 x, s8 y, s8 dx, s8 dy)
{
	u8 c;
	u8 steps = 0;
	u8 stones = 0;
	do {
		// do step
		x += dx;
		y += dy;

		// if chicken, number of steps is valid
		c = GetBoard(x, y);
		if (c == TILE_CHICKEN) stones = steps;

		// increase number of steps
		steps++;

	// while tile is valid
	} while ((c == TILE_EGG) || (c == TILE_CHICKEN) || (c == TILE_HEN));

	return stones;
}

// discovery stones in all directions (returns number of stones; cursor must be off)
u8 GetStones(s8 x, s8 y)
{
	u8 stones = 0;
	int i;
	for (i = 0; i < 8; i++) stones += GetStones1(x, y, DirX[i], DirY[i]);
	return stones;
}

// display possible positions (cursor must be off)
void DispPos()
{
	s8 x, y;
	u8 stones;
	u8 c;

	// clear number of possible positions
	Pos = 0;

	// loop through all game tiles
	for (y = 0; y < BOARDH; y++)
	{
		for (x = 0; x < BOARDW; x++)
		{
			// check if this tile is empty
			c = GetBoard(x, y);
			if ((c == TILE_EMPTY) || (c == TILE_DOT))
			{
				// count number of stones
				if (GetStones(x, y) > 0)
				{
					// increase number of possible positions
					Pos++;
					SetBoard(x, y, TILE_DOT);
				}
				else
					SetBoard(x, y, TILE_EMPTY);
			}
		}
	}
}

// start open page (returns key)
char OpenScreen()
{
	// display open image
	DrawImg(OpenImg);
	DispUpdate();

	// play music
	sMP3Player* mp3 = &MP3Player;
	int r = MP3PlayerInit(mp3, NULL, MapleSnd, sizeof(MapleSnd), MP3PlayerOutBuf, MP3PLAYER_OUTSIZE, -1);
	if (r == ERR_MP3_OK) MP3Play(mp3, 0, True);

	// wait for a key
	int i;
	int ch;
	u32 t1 = Time();
	while(True)
	{
		ch = KeyGet();

		// screenshot
		if (ch == KEY_SCREENSHOT)
		{
			ScreenShot();
		}

		// LCD display rezoom
		if (ch == KEY_INSERT)
		{
			LCDRezoom();
		}

		// demo
		if (Demo)
		{
			if ((u32)(Time() - t1) > 18000000) ch = KEY_UP;
		}

		switch(ch)
		{
		// start game
		case KEY_LEFT:
		case KEY_RIGHT:
		case KEY_UP:
		case KEY_DOWN:
		case KEY_Y:
			MP3PlayerTerm(mp3);
			return ch;

		// no key
		// invalid key
		default:
			break;
		}

		// music poll
		MP3Poll(mp3);
		WaitMs(5);
	}
}

// display selected player
void DispSel()
{
	int play;
	int y = 0;
	for (play = 0; play < 2; play++)
	{
		// draw background
		DrawImg(FrameImg, FRAMEX, y, 0, y, FRAMEW, SELECTH);

		// draw computer
		if (Comp[play]) DrawImg(ComputerImg, COMPX, y+COMPY, 0, 0, COMPW, COMPH);

		// draw selection frame
		if (play == Player) DrawImg(SelectImg, SELECTX, y+SELECTY, 0, 0, SELECTW, SELECTH);

		// shift Y coordinate
		y += FRAMEY;
	}
	DispUpdate();
}

char NumBuf[20];

// display score
void DispScore()
{
	int play;
	int n;
	for (play = 0; play < 2; play++)
	{
		n = DecNum(NumBuf, Stones[play], 0);
		NumBuf[n++] = ' ';
		NumBuf[n++] = 0;
		DrawTextBg2(NumBuf, SCOREX, SCOREY+play*FRAMEY, SCORECOL, SCOREBG);
	}
	DispUpdate();
}

// display wins
void DispWin()
{
	int play;
	int n;
	for (play = 0; play < 2; play++)
	{
		n = DecNum(NumBuf, Wins[play], 0);
		NumBuf[n++] = ' ';
		NumBuf[n++] = ' ';
		NumBuf[n++] = ' ';
		NumBuf[n++] = 0;
		DrawTextBg2(NumBuf, WINX, WINY+play*FRAMEY, SCORECOL, SCOREBG);
	}
	DispUpdate();
}

// open game
void OpenGame(Bool comp1, Bool comp2)
{
	int i;

	// copy board template
	for (i = 0; i < BOARDSIZE; i++) Board[i] = 1;
	Board[3*8+3] = 4;
	Board[3*8+4] = 4;
	Board[4*8+3] = 4;
	Board[4*8+4] = 4;
	DrawBoard();

	// copy frame image
	DrawImg(FrameImg, FRAMEX, 0, 0, 0, FRAMEW, FRAMEH);

	// initialize players
	Stones[0] = 0;
	Stones[1] = 0;
	Comp[0] = comp1;
	Comp[1] = comp2;

	// start random player
	Player = RandU8() & 1;

	// cursor position
	CurX = 3;
	CurY = 3;

	// initialize number of free positions
	Free = BOARDW*BOARDH-4;
	Pos = 12;

	// display selected player
	DispSel();

	// display score
	DispScore();

	// display wins
	DispWin();
}

// update stones in one direction
void PutStone1(s8 x, s8 y, s8 dx, s8 dy)
{
	u8 c;

	// check number of stones in this direction
	u8 stones = GetStones1(x, y, dx, dy);
	for (; stones > 0; stones--)
	{
		DrawBoard();

		// play sound and short delay
		PlaySound(BeepSnd);
		WaitMs(100);

		// step
		x += dx;
		y += dy;

		// get tile
		c = GetBoard(x, y);

		// player 2 is active, direction egg -> chicken -> hen
		if (Player == 1)
		{
			// egg -> chicken
			if (c == TILE_EGG)
			{
				Stones[0]--; // decrease number of eggs of player 1
				c = TILE_CHICKEN; // change to chicken
			}

			// hen -> egg
			else if (c == TILE_HEN)
			{
				Stones[1]--; // decrease number of hens of player 2
				Stones[0]++; // increase number of eggs of player 1
				c = TILE_EGG;
			}

			// chicken -> hen
			else
			{
				Stones[1]++; // increase number of hens of player 2
				c = TILE_HEN;
			}
		}

		// player 1 is active, direction hen -> chicken -> egg
		else
		{
			// egg -> hen
			if (c == TILE_EGG)
			{
				Stones[0]--; // decrease number of effs of player 1
				Stones[1]++; // increase number of hens of player 2
				c = TILE_HEN;
			}

			// hen -> chicken
			else if (c == TILE_HEN)
			{
				Stones[1]--; // decrease number of hens of player 2
				c = TILE_CHICKEN;
			}

			// chicken -> egg
			else
			{
				Stones[0]++; // increase number of effs of player 1
				c = TILE_EGG;
			}
		}

		// set tile
		SetBoard(x, y, c);
		DrawBoard();

		// display score
		DispScore();
	}
}

// put stone (cursor must be off and current tile must be 'dot')
void PutStone()
{
	int i;

	// short blink cursor (must leave cursor off)
	for (i = 5; i > 0; i--)
	{
		DrawBoard();
		WaitMs(50);
		CurOn();
		DrawBoard();
		WaitMs(50);
		CurOff();
	}
	DrawBoard();

	// put chicken into current position
	SetBoard(CurX, CurY, TILE_CHICKEN);

	// decrease free positions
	Free--;

	// update stones in all directions
	for (i = 0; i < 8; i++) PutStone1(CurX, CurY, DirX[i], DirY[i]);
}

// get profit in one direction
s8 Profit1(s8 x, s8 y, s8 dx, s8 dy)
{
	u8 c;
	s8 profit = 0;

	// check number of stones in this direction
	u8 stones = GetStones1(x, y, dx, dy);
	for (; stones > 0; stones--)
	{
		// step
		x += dx;
		y += dy;

		// get tile
		c = GetBoard(x, y);

		// player 2 is active (egg -> chicken -> hen)
		if (Player == 1)
		{
			// this player loses hen, opposite gains egg
			if (c == TILE_HEN)
				profit -= 2;

			// this player gains hen or opposite loses egg
			else
				profit++;
		}

		// player 1 is active (hen -> chicken -> egg)
		else
		{
			// this player loses egg, opposite gains hen
			if (c == TILE_EGG)
				profit -= 2;

			// this player gains egg or opposite loses hen
			else
				profit++;
		}
	}
	return profit;
}

// get tile profit
s8 Profit(s8 x, s8 y)
{
	s8 profit = 0;
	int i;
	for (i = 0; i < 8; i++) profit += Profit1(x, y, DirX[i], DirY[i]);
	return profit;
}

// find best position (cursor must be off)
void FindPos()
{
	s8 x, y;
	u8 c;
	s8 bestprofit = -125; // profit of founded tiles
	s8 profit;

	// loop through all game tiles
	for (y = 0; y < BOARDH; y++)
	{
		for (x = 0; x < BOARDW; x++)
		{
			// check if this tile is valid
			c = GetBoard(x, y);
			if (c == TILE_DOT)
			{
				// get profit of this field
				profit = Profit(x, y);

				// check better profit
				if ((profit > bestprofit) ||
					((profit == bestprofit) && (RandU8() < 80)))
				{
					// save new best state
					bestprofit = profit;
					CurX = x;
					CurY = y;
				}
			}
		}
	}
}

// play game
void PlayGame(Bool comp1, Bool comp2)
{
	int i;
	int ch;
	int x, y;

	// demo mode
	Demo = comp1 && comp2;

	// open game
	OpenGame(comp1, comp2);

	// display possible positions
	DispPos();

	// display cursor on
	CurOn();
	DrawBoard();

	// while there is some possible position
	while ((Pos > 0) && (Free > 0))
	{
		// wait for a key
		ch = KeyGet();

		// screenshot
		if (ch == KEY_SCREENSHOT) ScreenShot();

		// LCD display rezoom
		if (ch == KEY_INSERT) LCDRezoom();

		// break demo
		if (Demo && (ch == KEY_Y))
		{
			Demo = False;
			return;
		}

		// player is computer
		if (Comp[Player])
		{
			// find position
			CurOff();
			FindPos();
			CurOn();
			DrawBoard();

			// put stone
			if (!Demo) KeyFlush();
			ch = KEY_A;
		}

		switch (ch)
		{
		// break game
		case KEY_Y:
			return;

		// right
		case KEY_RIGHT:
			if (CurX < BOARDW-1)
			{
				CurOff();
				CurX++;
				CurOn();
				DrawBoard();
			}
			break;

		// up
		case KEY_UP:
			if (CurY > 0)
			{
				CurOff();
				CurY--;
				CurOn();
				DrawBoard();
			}
			break;

		// left
		case KEY_LEFT:
			if (CurX > 0)
			{
				CurOff();
				CurX--;
				CurOn();
				DrawBoard();
			}
			break;

		// down
		case KEY_DOWN:
			if (CurY < BOARDH-1)
			{
				CurOff();
				CurY++;
				CurOn();
				DrawBoard();
			}
			break;

		// help
		case KEY_B:
			CurOff();
			FindPos();
			CurOn();
			DrawBoard();
			break;

		// put stone
		case KEY_A:
			CurOff();
			if (GetBoard(CurX, CurY) == TILE_DOT)
			{
				// put stone
				PutStone();

				// update possible positions
				DispPos();

				// change player
				Player ^= 1;

				// display selected player
				DispSel();

				// display score
				DispScore();
			}
			CurOn();
			DrawBoard();
			break;

		// no key, short delay
		case NOKEY:
			WaitMs(50);
			break;

		// invalid key
		default:
			break;
		}

		// blinking cursor
		CurBlink++;
		if (CurBlink == 5)
		{
			CurOff();
			DrawBoard();
		}

		if (CurBlink == 8)
		{
			CurOn();
			DrawBoard();
		}
	}

	// cursor off
	CurOff();

	// select no player
	Player = 2;
	DispSel();

	// max. time of blinking
	i = Demo ? 15 : 1000000000;

	// player 1 (eggs) wins
	if (Stones[0] > Stones[1])
	{
		// animate
		for (y = 0; y < 8; y++)
		{
			for (x = 0; x < 8; x++)
			{
				if (GetBoard(x, y) == TILE_EGG)
				{
					SetBoard(x, y, TILE_EGGWIN);
					PlaySound(Beep2Snd);
					DrawBoard();
					WaitMs(200);
				}
			}
		}

		// display wins
		Wins[0]++;
		DispWin();

		// blink winner
		for (; i > 0; i--)
		{
			Player = 0;
			DispSel();
			DrawBoard();
			WaitMs(200);
			if (KeyGet() != NOKEY) return;
			Player = 2;
			DispSel();
			DrawBoard();
			WaitMs(200);
			if (KeyGet() != NOKEY) return;
		}
	}

	// player 2 (hens) wins
	else if (Stones[1] > Stones[0])
	{
		// animate
		for (y = 0; y < 8; y++)
		{
			for (x = 0; x < 8; x++)
			{
				if (GetBoard(x, y) == TILE_HEN)
				{
					SetBoard(x, y, TILE_HENWIN);
					PlaySound(Beep2Snd);
					DrawBoard();
					WaitMs(200);
				}
			}
		}

		// display wins
		Wins[1]++;
		DispWin();

		// blink winner
		for (; i > 0; i--)
		{
			Player = 1;
			DispSel();
			DrawBoard();
			WaitMs(200);
			if (KeyGet() != NOKEY) return;
			Player = 2;
			DispSel();
			DrawBoard();
			WaitMs(200);
			if (KeyGet() != NOKEY) return;
		}
	}

	// no winner
	else
	{
		// blink winner
		for (; i > 0; i--)
		{
			Player = 0;
			DispSel();
			DrawBoard();
			WaitMs(200);
			if (KeyGet() != NOKEY) return;
			Player = 1;
			DispSel();
			DrawBoard();
			WaitMs(200);
			if (KeyGet() != NOKEY) return;
		}
	}
}

// main function
int main()
{
	// decompress images
	ComputerImg = (u8*)PNGLOAD(ComputerImgPNG); if (ComputerImg == NULL) Reboot();
	FrameImg = (u8*)PNGLOAD(FrameImgPNG); if (FrameImg == NULL) Reboot();
	OpenImg = (u8*)JPGLOAD(OpenImgJPG); if (OpenImg == NULL) Reboot();
	SelectImg = (u8*)PNGLOAD(SelectImgPNG); if (SelectImg == NULL) Reboot();
	TilesImg = (u8*)PNGLOAD(TilesImgPNG); if (TilesImg == NULL) Reboot();

	int ch;

	// clear players
	Wins[0] = 0;
	Wins[1] = 0;

	// main loop
	while (True)
	{
		// open screen
		ch = OpenScreen();

		// start new game
		switch (ch)
		{
		// player is EGG
		case KEY_LEFT:
			PlayGame(False, True);
			break;

		// player is HEN
		case KEY_RIGHT:
			PlayGame(True, False);
			break;

		// demo
		case KEY_UP:
			PlayGame(True, True);
			break;

		// 2 players
		case KEY_DOWN:
			PlayGame(False, False);
			break;

		// quit
		case KEY_Y:
			Reboot();
			break;
		}
	}

	return 0;
}
