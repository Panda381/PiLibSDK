
// ****************************************************************************
//
//                                 Bonus
//
// ****************************************************************************

#include "../include.h"

// bonus templates
sBonusTemp BonusTemp[BONUS_NUM] = {
//	imgpng			imgpng_size		img	w		h
	{ ImgHeartPNG,		sizeof(ImgHeartPNG),	NULL,	HEART_W,	HEART_H,	},
	{ ImgPicopad1PNG,	sizeof(ImgPicopad1PNG),	NULL,	PICOPAD1_W,	PICOPAD1_H,	},
	{ ImgPicopad2PNG,	sizeof(ImgPicopad2PNG),	NULL,	PICOPAD2_W,	PICOPAD2_H,	},
	{ ImgPicopad3PNG,	sizeof(ImgPicopad3PNG),	NULL,	PICOPAD3_W,	PICOPAD3_H,	},
	{ ImgPicopad4PNG,	sizeof(ImgPicopad4PNG),	NULL,	PICOPAD4_W,	PICOPAD4_H,	},
	{ ImgPicopadPNG,	sizeof(ImgPicopadPNG),	NULL,	PICOPAD_W,	PICOPAD_H,	},
};

// bonuses in levels
int BonusLevel[BG_NUM] = {
	BONUS_TOP,		// 1 Meadow
	BONUS_CPU,		// 2 Jungle
	BONUS_HEART,		// 3 Sandy Beach
	BONUS_DISP,		// 4 Underwater
	BONUS_BAT,		// 5 Ice Land
	BONUS_HEART,		// 6 Fiery Land
	BONUS_TOP,		// 7 Haunted Hill
	BONUS_CPU,		// 8 Spacecraft
	BONUS_HEART,		// 9 Galaxy
	BONUS_DISP,		// 10 Alien Planet
	BONUS_BAT,		// 11 Candy Land
	BONUS_HEART,		// 12 Surreal Land
};

// current bonus
int BonusMask;			// mask of collected Picopad components (BONUS_MASK_*)
int BonusInx;			// index of current bonus (-1 if bonus is not active)
const sBonusTemp* Bonus;	// current bonus template
int BonusX;			// coordinate X of current bonus
int BonusY;			// coordinate Y of current bonus
int BonusPhase;			// bonus animation phase (0..2)

// initialize bonus on start of next level (requires initialized EnemyRandSeed)
void InitBonus()
{
	BonusInx = BonusLevel[BackInx];	// bonus index
	Bonus = &BonusTemp[BonusInx];	// bonus template
	BonusX = MaxFrame/2*BG_SPEED3 + WIDTH;	// X coordinate
	BonusY = EnemyRandMinMax(BG_Y_MIN + 200, BG_Y_MAX - 80); // random Y coordiate
	BonusPhase = 0;			// bonus animation phase
}

// shift bonus
void BonusShift()
{
	// shift animation phase
	BonusPhase++;
	if (BonusPhase >= BONUS_PHASES) BonusPhase = 0;

	// shift bonus X coordinate
	BonusX -= BG_SPEED3;
}

// display bonus
void BonusDisp()
{
	int x = BonusX;
	if ((x >= -60) && (BonusInx >= 0) && (x < WIDTH + 60))
	{
		int inx = BonusInx;
		int y = BonusY;
		const sBonusTemp* b = Bonus;
		
		// draw cloud
		DrawImg(BonuscloudImg, 			// image data and palettes
			x - BONUS_W/2, y - BONUS_H/2, 		// destination X, Y
			BonusPhase * BONUS_W, 0,		// source X, Y
			BONUS_W, BONUS_H);			// width, height

		// draw bonus
		DrawImg(b->img, 			// image data and palettes
			x - b->w/2, y - b->h/2,			// destination X, Y
			0, 0,					// source X, Y
			b->w, b->h);				// width, height
	}
}

// collect bonus
void BonusCollect()
{
	int x = BonusX;
	if ((x >= -60) && (BonusInx >= 0) && (x < WIDTH + 60))
	{
		// check bonus collision
		if (	(ActorX + Actor.w/2 >= x) &&
			(ActorX - Actor.w/2 < x) &&
			(ActorY + Actor.h/2 >= BonusY) &&
			(ActorY - Actor.h/2 < BonusY))
		{
			// add score
			if (BonusInx == BONUS_HEART)
				Life += 100;
			else
			{
				Score += BONUS_SCORE;
				AddPoint(BONUS_SCORE, BonusX, BonusY);
			}

			// sound
			if (SoundMode != SOUNDMODE_OFF) PlaySound(ZingSnd, SOUNDCHAN_ACTORHIT);

			if ((BonusInx >= BONUS_COLFIRST) && (BonusInx <= BONUS_COLLAST))
			{
				BonusMask |= BIT(BonusInx);
				if (BonusMask == BONUS_MASK_ALL)
				{
					// Picopad is complete
					Score += BONUS_BIGSCORE;
					DispHeader();
					DispUpdate();

					if (SoundMode != SOUNDMODE_OFF) PlaySound(BigbonusSnd, SOUNDCHAN_ACTORHIT);
					WaitMs(1000);

					BonusMask = 0;
					DispHeader();
					DispUpdate();
				}
			}

			// delete bonus
			BonusInx = -1;
		}
	}
}
