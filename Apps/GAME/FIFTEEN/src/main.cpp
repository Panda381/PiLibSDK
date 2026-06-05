
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

// images
#include "../img/01.cpp"		// const u8 Img01PNG[] 640x480
#include "../img/02.cpp"		// const u8 Img02JPG[] 640x480
#include "../img/03.cpp"		// const u8 Img03JPG[] 640x480
#include "../img/04.cpp"		// const u8 Img04JPG[] 640x480
#include "../img/05.cpp"		// const u8 Img05JPG[] 640x480
#include "../img/06.cpp"		// const u8 Img06JPG[] 640x480
#include "../img/07.cpp"		// const u8 Img07JPG[] 640x480
#include "../img/08.cpp"		// const u8 Img08JPG[] 640x480
#include "../img/09.cpp"		// const u8 Img09JPG[] 640x480
#include "../img/10.cpp"		// const u8 Img10JPG[] 640x480
#include "../img/11.cpp"		// const u8 Img11JPG[] 640x480
#include "../img/12.cpp"		// const u8 Img12JPG[] 640x480
#include "../img/13.cpp"		// const u8 Img13JPG[] 640x480
#include "../img/14.cpp"		// const u8 Img14JPG[] 640x480
#include "../img/15.cpp"		// const u8 Img15JPG[] 640x480
#include "../img/16.cpp"		// const u8 Img16JPG[] 640x480
#include "../img/frame.cpp"		// const u8 FrameImgPNG[] 160x120
#include "../img/intro.cpp"		// const u8 IntroImgJPG[] 640x480
#include "../img/select.cpp"		// const u8 SelectImgJPG[] 640x480
#include "../img/tiles.cpp"		// const u8 TilesImgPNG[] 160x480 (1x4 tiles, 1 tile 160x120)

// sounds
#include "../snd/bump.cpp"		// const u8 BumpSnd[] PCM mono 16-bit 22050Hz
#include "../snd/move.cpp"		// const u8 MoveSnd[] PCM mono 16-bit 22050Hz
#include "../snd/shuffling.cpp"		// const u8 ShufflingSnd[] PCM mono 16-bit 22050Hz
#include "../snd/toon.cpp"		// const u8 ToonSnd[] PCM mono 16-bit 22050Hz

#define TILEW	160	// tile width
#define TILEH	120	// tile height
#define TILESX	4	// game tiles in X direction
#define TILESY	4	// game tiles in Y direction
#define TILESNUM (TILESX*TILESY) // number of tiles (= 16)
#define TILE_EMPTY_INX	15	// empty tile index
#define TILE_EMPTY	1	// empty tile
#define TILE_WAIT	2	// wait tile
#define TILE_WAIT2	3	// wait tile 2

#define BOARDW	(TILESX*TILEW)	// board width
#define BOARDH	(TILESY*TILEH)	// board height
#define BOARDX	((WIDTH-BOARDW)/2) // board X coordinate
#define BOARDY	((HEIGHT-BOARDH)/2) // board Y coordinate

#define IMGW	640	// image width
#define IMGH	480	// image height

#define PUZZLE_NUM	16	// number of puzzles

// images (1 image is 1 MB, 16 images are 16 MB - ok we can decompress all images at once)
u8 *ImgList[PUZZLE_NUM], *FrameImg, *IntroImg, *SelectImg, *TilesImg;
u8* Img;

// game board
u8 Board[TILESNUM]; // visible board
//u8 BoardSave[TILESNUM]; // saved board during demo
u8 Pos[TILESNUM]; // positions of tiles (including hole)
u8 PosErr; // number of tiles on incorrect position
Bool Locked[TILESNUM]; // solved groups: locked tiles
Bool Solved[TILESNUM]; // solved groups: tiles to solve
int Dist; // current distance of numeric tiles from result position (0 = result is OK)

#define PREDEPTH_MAX 14 // max. depth of pre-solver (must be <= DEPTH_MAX)
#define DEPTH_MAX 17 // max. depth of solver (real depth is max. 19)
s8 Moves[DEPTH_MAX]; // moves
int MovesNum;	// number of moves
int DepthMax;	// current max. depth

s8 BestMoves[DEPTH_MAX]; // best moves
int BestMovesNum;	// number of best moves
int BestDist;	// distance of best moves

#define Hole Pos[TILE_EMPTY_INX] // position of hole

// get hole X coordinate
INLINE u8 HoleX() { return Hole & 3; }

// get hole Y coordinate
INLINE u8 HoleY() { return Hole >> 2; }

// check goal state
INLINE Bool Check() { return PosErr == 0; }

// tile number
const char* TileNumTxt[15] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15" };

Bool DispTileNum = False;
Bool GameEnd = False;
int PuzzleInx = 0; // selected puzzle

// solved group table: number of tiles (0 = end mark), list of tiles 1..
const u8 SolverTab[] = {
	1, 1,
	1, 2,
	1, 5,
	1, 6,
	2, 3, 4,
	2, 7, 8,
	2, 9, 13,
	2, 10, 14,
	3, 11, 12, 15,
	0
};

// draw board
void DrawBoard()
{
	int i, j, x, y, b, bx, by;
	for (i = 0; i < TILESY; i++)
	{
		for (j = 0; j < TILESX; j++)
		{
			x = j*TILEW+BOARDX;
			y = i*TILEH+BOARDY;
			b = Board[i*TILESX + j];
			if ((b == TILE_EMPTY_INX) && !GameEnd) // empty tile in game
			{
				DrawImg(TilesImg, x, y, 0, TILE_EMPTY*TILEH, TILEW, TILEH);
			}
			else
			{
				// draw result tile
				bx = b & 3;
				by = b >> 2;
				DrawImg(Img, x, y, bx*TILEW, by*TILEH, TILEW, TILEH);

				// draw frame
				if ((PuzzleInx != 0) && DispTileNum && (b != TILE_EMPTY_INX) && !GameEnd)
				{
					DrawRect(x, y, 1, TILEH-1, COL_WHITE);
					DrawRect(x+1, y, TILEW-2, 1, COL_WHITE);
					DrawRect(x+TILEW-1, y+1, 1, TILEH-1, COL_BLACK);
					DrawRect(x+1, y+TILEH-1, TILEW-2, 1, COL_BLACK);
					DrawTextBg2(TileNumTxt[b], x+TILEW-((b < 9) ? 2*8 : 2*16), y+TILEH-2*16, COL_WHITE, COL_BLACK);
				}
			}
		}
	}

	// update screen
	DispUpdate();
}

// wait icon ON
void WaitOn()
{
	int x = HoleX()*TILEW+BOARDX;
	int y = HoleY()*TILEH+BOARDY;
	DrawImg(TilesImg, x, y, 0, TILE_WAIT*TILEH, TILEW, TILEH);
	DispUpdate();
}

// wait icon ON 2
void WaitOn2()
{
	int x = HoleX()*TILEW+BOARDX;
	int y = HoleY()*TILEH+BOARDY;
	DrawImg(TilesImg, x, y, 0, TILE_WAIT2*TILEH, TILEW, TILEH);
	DispUpdate();
}

// wait icon OFF
void WaitOff()
{
	int x = HoleX()*TILEW+BOARDX;
	int y = HoleY()*TILEH+BOARDY;
	DrawImg(TilesImg, x, y, 0, TILE_EMPTY*TILEH, TILEW, TILEH);
	DispUpdate();
}

// calculate position error
void CalcPos()
{
	int i, x, y, err, dist;
	u8 b;
	err = 0;
	dist = 0;
	for (i = 0; i < TILESNUM; i++)
	{
		b = Board[i];
		if (b != TILE_EMPTY_INX)
		{
			if (Solved[b] && (b != (u8)i)) err++;
			x = (i & 3) - (b & 3);
			y = (i >> 2) - (b >> 2);
			if (Solved[b])
			{
				x *= 4;
				y *= 4;
			}
			dist += x*x + y*y;
		}
		Pos[b] = i;
	}
	PosErr = err;
	Dist = dist;
}

// shift hole (-4, -1, +1, +4), without checking coordinates
void Shift(s8 shift)
{
	int x, y;

	// old index of the hole (= tile new index)
	u8 oldinx = Hole;

	// new index of the hole (= tile old index)
	u8 newinx = oldinx + shift;

	// get tile
	u8 b = Board[newinx];
	if ((b == newinx) && Solved[b]) PosErr++;
	x = (b & 3) - (newinx & 3);
	y = (b >> 2) - (newinx >> 2);
	if (Solved[b])
	{
		x *= 4;
		y *= 4;
	}
	Dist -= x*x + y*y;

	// move tile
	Board[oldinx] = b;
	if ((b == oldinx) && Solved[b]) PosErr--;
	x = (b & 3) - (oldinx & 3);
	y = (b >> 2) - (oldinx >> 2);
	if (Solved[b])
	{
		x *= 4;
		y *= 4;
	}
	Dist += x*x + y*y;
	Pos[b] = oldinx;

	// set hole
	Board[newinx] = TILE_EMPTY_INX;
	Hole = newinx;
}

// initialize new game
void NewGame(Bool shuffle)
{
	int i;
	u8 r;

	GameEnd = False;

	// clear screen
	DrawClear();

	// initialize new state
	for (i = 0; i < TILESNUM; i++)
	{
		Locked[i] = False;
		Solved[i] = True;
		Board[i] = i;
		Pos[i] = i;
	}
	Dist = 0;
	PosErr = 0;

	// shuffle
	if (shuffle)
	{
		// start shuffling sound
		PlaySoundRep(ShufflingSnd);

		// shuffle board (2 seconds)
		for (i = 4000; i > 0; i--)
		{
			r = RandU8();
			if (r >= 0x80) // shift in X direction
			{
				if ((r & 1) == 0)
				{
					// shift hole left
					if (HoleX() > 0) Shift(-1);
				}
				else
				{
					// shift hole right
					if (HoleX() < TILESX-1) Shift(1);
				}
			}
			else // shift in Y direction
			{
				if ((r & 1) == 0)
				{
					// shift hole up
					if (HoleY() > 0) Shift(-TILESX);
				}
				else
				{
					// shift hole down
					if (HoleY() < TILESY-1) Shift(TILESX);
				}
			}

			// draw board
			if ((i & 0x7f) == 0)
			{
				DrawBoard();
				WaitMs(20);
			}
		}

		// stop shuffling sound
		StopSound();

		// flush keyboard
		KeyFlush();
	}

	// draw resulting state
	DrawBoard();
}

// pre-solve 1 level
void PreSolve1()
{
	// completed
	if (Dist == 0) return;

	// copy better solution
	if ((Dist < BestDist) || ((Dist == BestDist) && (MovesNum < BestMovesNum)))
	{
		memcpy(BestMoves, Moves, MovesNum);
		BestMovesNum = MovesNum;
		BestDist = Dist;
	}

	// check max. depth
	int movesnum = MovesNum;
	if (movesnum >= DepthMax) return;
	MovesNum = movesnum+1;

	// shift hole left
	if ((HoleX() > 0) && !Locked[Board[Hole - 1]])
	{
		Shift(-1);
		Moves[movesnum] = -1;
		PreSolve1();
		Shift(1);
	}

	// shift hole right
	if ((HoleX() < TILESX-1) && !Locked[Board[Hole + 1]])
	{
		Shift(1);
		Moves[movesnum] = 1;
		PreSolve1();
		Shift(-1);
	}

	// shift hole up
	if ((HoleY() > 0) && !Locked[Board[Hole - TILESX]])
	{
		Shift(-TILESX);
		Moves[movesnum] = -TILESX;
		PreSolve1();
		Shift(TILESX);
	}

	// shift hole down
	if ((HoleY() < TILESY-1) && !Locked[Board[Hole + TILESX]])
	{
		Shift(TILESX);
		Moves[movesnum] = TILESX;
		PreSolve1();
		Shift(-TILESX);
	}

	// return number of moves
	MovesNum = movesnum;
}

// solve 1 level (returns True to completed)
Bool Solve1()
{
	Bool res;

	// completed
	if (PosErr == 0) return True;

	// check max. depth
	int movesnum = MovesNum;
	if (movesnum >= DepthMax) return False;
	MovesNum = movesnum+1;

	// shift hole left
	if ((HoleX() > 0) && !Locked[Board[Hole - 1]])
	{
		Shift(-1);
		Moves[movesnum] = -1;
		res = Solve1();
		Shift(1);
		if (res) return True;
	}

	// shift hole right
	if ((HoleX() < TILESX-1) && !Locked[Board[Hole + 1]])
	{
		Shift(1);
		Moves[movesnum] = 1;
		res = Solve1();
		Shift(-1);
		if (res) return True;
	}

	// shift hole up
	if ((HoleY() > 0) && !Locked[Board[Hole - TILESX]])
	{
		Shift(-TILESX);
		Moves[movesnum] = -TILESX;
		res = Solve1();
		Shift(TILESX);
		if (res) return True;
	}

	// shift hole down
	if ((HoleY() < TILESY-1) && !Locked[Board[Hole + TILESX]])
	{
		Shift(TILESX);
		Moves[movesnum] = TILESX;
		res = Solve1();
		Shift(-TILESX);
		if (res) return True;
	}

	// return number of moves
	MovesNum = movesnum;

	return False;
}

// solver
void Solver()
{
	Bool ok;
	const u8* t = SolverTab;
	int i, j;
	int faster1 = 7; // speed-up pre-solver
	int faster2 = 10; // speed-up solver
	Bool err;

	// clear solved group tabled
	for (i = 0; i < TILESNUM; i++)
	{
		Locked[i] = False;
		Solved[i] = False;
	}

	while (True)
	{
		// prepare next solved group
		MovesNum = 0;
		for (i = 0; i < TILESNUM; i++)
		{
			if (Solved[i]) Locked[i] = True; // lock already solved tiles
		}

		// update positions
		i = *t++; // number of tiles of next solved group
		if (i == 0) break; // end of table
		for (; i > 0; i--) Solved[*t++ - 1] = True;
		CalcPos();

PRE_AGAIN:
		// pre-solver
		err = True;
		WaitOn();

		// pre-solve
		BestMovesNum = 0;
		BestDist = Dist;
		DepthMax = PREDEPTH_MAX - faster1;
		PreSolve1();

		// play moves
		if (BestMovesNum > 0)
		{
			WaitOff();
			for (i = 0; i < BestMovesNum; i++)
			{
				// shift tile
				Shift(BestMoves[i]);
				PlaySound(MoveSnd);
				DrawBoard();

				// wait some time
				WaitMs(100);
				if (KeyGet() != NOKEY) return;
				WaitMs(100);
				if (KeyGet() != NOKEY) return;
			}
			WaitOn();
			err = False;

			// found some solution, so slower solver
			if (faster2 < 10) faster2++;
		}

		// quit
		if (KeyGet() != NOKEY) return;

		// solver
		WaitOn2();
		DepthMax = 0;
		do {
			DepthMax++;
			ok = Solve1();
		} while (!ok && (DepthMax < DEPTH_MAX - faster2));

		// not found, slower down all and repeat all again
		if (!ok && !Check())
		{
			if (err && (faster1 > 0)) faster1--;
			if (err && (faster2 > 0)) faster2--;
			goto PRE_AGAIN; // not found, try pre-solver again
		}

		// play moves
		if (MovesNum > 0)
		{
			WaitOff();
			for (i = 0; i < MovesNum; i++)
			{
				// shift tile
				Shift(Moves[i]);
				PlaySound(MoveSnd);
				DrawBoard();

				// wait some time
				WaitMs(100);
				if (KeyGet() != NOKEY) return;
				WaitMs(100);
				if (KeyGet() != NOKEY) return;
			}
			WaitOn2();
			err = False;

			// found some solution, so slower pre-solver
			if (faster1 < 7) faster1++;
		}

		// error of pre-solver and solver, slower down all
		if (err && (faster1 > 0)) faster1--;
		if (err && (faster2 > 0)) faster2--;
	}

	// initialize new state
	for (i = 0; i < TILESNUM; i++)
	{
		Locked[i] = False;
		Solved[i] = True;
	}
	WaitOff();
	CalcPos();
	DrawBoard();
}

// one game
void Game()
{
	int i;

	// new game
	NewGame(True);

	// game loop
	while (True)
	{
		// key
		switch(KeyGet())
		{
		// right
		case KEY_RIGHT:
			if (HoleX() > 0)
			{
				Shift(-1);
				PlaySound(MoveSnd);
				DrawBoard();
			}
			else
				PlaySound(BumpSnd);
			break;

		// left
		case KEY_LEFT:
			if (HoleX() < TILESX-1)
			{
				Shift(1);
				PlaySound(MoveSnd);
				DrawBoard();
			}
			else
				PlaySound(BumpSnd);
			break;

		// up
		case KEY_UP:
			if (HoleY() < TILESY-1)
			{
				Shift(TILESX);
				PlaySound(MoveSnd);
				DrawBoard();
			}
			else
				PlaySound(BumpSnd);
			break;

		// down
		case KEY_DOWN:
			if (HoleY() > 0)
			{
				Shift(-TILESX);
				PlaySound(MoveSnd);
				DrawBoard();
			}
			else
				PlaySound(BumpSnd);
			break;

		// numbers
		case KEY_A:
			DispTileNum = !DispTileNum;
			DrawBoard();
			break;

		// screenshot
		case KEY_SCREENSHOT:
			ScreenShot();
			break;

		// solver
		case KEY_B:
			Solver();
			for (i = 0; i < TILESNUM; i++)
			{
				Locked[i] = False;
				Solved[i] = True;
			}
			CalcPos();
			DrawBoard();
			break;

		case KEY_Y:
			return;
		}

		// check solution
		if (Check())
		{
			GameEnd = True;
			DrawBoard();

			// play fanfare
			PlaySound(ToonSnd);

			// winning animation
			for (i = 4; i > 0; i--)
			{
				WaitMs(150);
				DrawClear();
				DispUpdate();
				WaitMs(150);
				DrawBoard();
			}

			// flush keyboard
			KeyFlush();

			// wait for a key
			while (KeyGet() == NOKEY) {}
			return;
		}
	}
}

// select puzzle (returns False to exit)
Bool Select()
{
	// draw
	while (True)
	{
		// display all puzzles
		DrawImg(SelectImg);

		// display selection
		int x = (PuzzleInx & 3) * WIDTH/4;
		int y = (PuzzleInx >> 2) * HEIGHT/4;
		DrawFrameW(x, y, WIDTH/4, HEIGHT/4, 3, COL_RED);
		DispUpdate();

		// key
		int key = KeyGet();
		switch (key)
		{
		case KEY_Y:
			return False;

		case KEY_A:
		case KEY_B:
			Img = ImgList[PuzzleInx];
			return True;

		case KEY_SCREENSHOT:
			ScreenShot();
			break;

		case KEY_LEFT:
			PuzzleInx--;
			if (PuzzleInx < 0) PuzzleInx = PUZZLE_NUM-1;
			break;

		case KEY_RIGHT:
			PuzzleInx++;
			if (PuzzleInx >= PUZZLE_NUM) PuzzleInx = 0;
			break;

		case KEY_UP:
			PuzzleInx -= 4;
			if (PuzzleInx < 0) PuzzleInx += 16;
			break;

		case KEY_DOWN:
			PuzzleInx += 4;
			if (PuzzleInx >= PUZZLE_NUM) PuzzleInx -= 16;
			break;

		case KEY_HOME:
			PuzzleInx = 0;
			break;

		case KEY_END:
			PuzzleInx = PUZZLE_NUM-1;
			break;
		}
	}
}

// main function
int main()
{
	u32 fgcol, bgcol;
	int i, key;

	// decompress images
	ImgList[0] = (u8*)PNGLOAD(Img01PNG); if (ImgList[0] == NULL) Reboot();
	ImgList[1] = (u8*)JPGLOAD(Img02JPG); if (ImgList[1] == NULL) Reboot();
	ImgList[2] = (u8*)JPGLOAD(Img03JPG); if (ImgList[2] == NULL) Reboot();
	ImgList[3] = (u8*)JPGLOAD(Img04JPG); if (ImgList[3] == NULL) Reboot();
	ImgList[4] = (u8*)JPGLOAD(Img05JPG); if (ImgList[4] == NULL) Reboot();
	ImgList[5] = (u8*)JPGLOAD(Img06JPG); if (ImgList[5] == NULL) Reboot();
	ImgList[6] = (u8*)JPGLOAD(Img07JPG); if (ImgList[6] == NULL) Reboot();
	ImgList[7] = (u8*)JPGLOAD(Img08JPG); if (ImgList[7] == NULL) Reboot();
	ImgList[8] = (u8*)JPGLOAD(Img09JPG); if (ImgList[8] == NULL) Reboot();
	ImgList[9] = (u8*)JPGLOAD(Img10JPG); if (ImgList[9] == NULL) Reboot();
	ImgList[10] = (u8*)JPGLOAD(Img11JPG); if (ImgList[10] == NULL) Reboot();
	ImgList[11] = (u8*)JPGLOAD(Img12JPG); if (ImgList[11] == NULL) Reboot();
	ImgList[12] = (u8*)JPGLOAD(Img13JPG); if (ImgList[12] == NULL) Reboot();
	ImgList[13] = (u8*)JPGLOAD(Img14JPG); if (ImgList[13] == NULL) Reboot();
	ImgList[14] = (u8*)JPGLOAD(Img15JPG); if (ImgList[14] == NULL) Reboot();
	ImgList[15] = (u8*)JPGLOAD(Img16JPG); if (ImgList[15] == NULL) Reboot();
	FrameImg = (u8*)PNGLOAD(FrameImgPNG); if (FrameImg == NULL) Reboot();
	IntroImg = (u8*)JPGLOAD(IntroImgJPG); if (IntroImg == NULL) Reboot();
	SelectImg = (u8*)JPGLOAD(SelectImgJPG); if (SelectImg == NULL) Reboot();
	TilesImg = (u8*)PNGLOAD(TilesImgPNG); if (TilesImg == NULL) Reboot();

	// display intro image
	PlaySound(ToonSnd);
	DrawImg(IntroImg);
	u32 t1 = Time();
	DispUpdate();
	WaitMs(200);

	// wait a key (max. 5 seconds)
	KeyFlush();
	while ((u32)(Time() - t1) < 5000000)
	{
		key = KeyGet();
		if (key == KEY_Y)
			Reboot();
		else if (key == KEY_SCREENSHOT)
			ScreenShot();
		else if ((key == KEY_B) || (key == KEY_A) || (key == KEY_X))
			break;
	}

	// clear screen
	DrawClear();
	DispUpdate();

	// main loop
	while (True)
	{
		// select puzzle (returns False to exit)
		if (!Select()) Reboot();

		// game
		Game();
	}

	return 0;
}
