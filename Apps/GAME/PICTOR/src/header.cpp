
// ****************************************************************************
//
//                                  Header
//
// ****************************************************************************

#include "../include.h"

// current and max score
int Score = 0;
int MaxScore = 0;

// display current score
void DispScore()
{
	int len = MemPrint(DecNumBuf, DECNUMBUF_SIZE, "%07d", Score);
	DrawTextBg2(DecNumBuf, 6*16+4, TIT_Y+4, (Score > MaxScore) ? COL_GREEN : COL_WHITE, HEADER_COL);
}

// display max. score
void DispMaxScore()
{
	int len = MemPrint(DecNumBuf, DECNUMBUF_SIZE, "%07d", (Score > MaxScore) ? Score : MaxScore);
	DrawTextBg2(DecNumBuf, WIDTH-7*16-4, TIT_Y+4, (Score > MaxScore) ? COL_GREEN : COL_WHITE, HEADER_COL);
}

// display Picopad
void DispPico()
{
	int mask = BonusMask;

	// Picopad is complete
	if (mask == BONUS_MASK_ALL)
	{
		DrawImg(PicopadImg,	// image data and palettes
			(WIDTH - PICOPAD_W - (BONUS_BIGSCORE_LEN+1)*16)/2, TIT_Y + (TIT_HEIGHT - PICOPAD_H)/2, // destination X, Y
			0, 0,					// source X, Y
			PICOPAD_W, PICOPAD_H);			// width, height

		// display points
		DrawText(BONUS_BIGSCORE_TEXT,
			(WIDTH - PICOPAD_W - (BONUS_BIGSCORE_LEN+1)*16)/2 + PICOPAD_W + 16, TIT_Y + 4,
			COL_YELLOW);
	}
	else
	{
		int x = 6*16+4 + 7*16+4 + 30;
		int i = BONUS_COLFIRST;
		const sBonusTemp* b = &BonusTemp[i];
		for (; i <= BONUS_COLLAST; i++)
		{
			if ((mask & BIT(i)) != 0)
			{
				DrawImg(b->img, 	// image data and palettes
					x, TIT_Y + (TIT_HEIGHT - b->h)/2, // destination X, Y
					0, 0,					// source X, Y
					b->w, b->h);			// width, height
				x += b->w + 4;
			}
			b++;
		}
	}
}

// display header
void DispHeader()
{
	// background
	DrawRect(1, TIT_Y+1, WIDTH-2, TIT_HEIGHT-2, HEADER_COL);

	// frame
	DrawFrame(0, TIT_Y, WIDTH, TIT_HEIGHT, HEADER_FRAMECOL);

	// title SCORE
	DrawText2("SCORE", 4, TIT_Y+4, COL_YELLOW);

	// title MAX
	DrawText2("MAX", WIDTH-11*16-4, TIT_Y+4, COL_YELLOW);

	// display current score
	DispScore();

	// display max. score
	DispMaxScore();

	// display Picopad
	DispPico();
}
