
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

#include "../img/back.cpp"	// BackImgPNG 320x240
#include "../img/bird.cpp"	// BirdImgPNG 46x16
#include "../img/cat.cpp"	// CatImgPNG 82x55
#include "../img/comet.cpp"	// CometImgPNG 30x9
#include "../img/digit.cpp"	// DigitImgPNG 200x24
#include "../img/dog.cpp"	// DogImgPNG 156x57
#include "../img/frame.cpp"	// FrameImgPNG 96x48
#include "../img/hare.cpp"	// HareImgPNG 111x61
#include "../img/mole.cpp"	// MoleImgPNG 120x66
#include "../img/moon.cpp"	// MoonImgPNG 96x38
#include "../img/mouse.cpp"	// MouseImgPNG 117x49
#include "../img/pf.cpp"	// PfImgPNG 36x24
#include "../img/pfall.cpp"	// PfAllImgPNG 120x24
#include "../img/spark.cpp"	// SparkImgPNG 33x11
#include "../img/star.cpp"	// StarImgPNG 33x11

#include "../snd/silent.cpp"	// SilentSnd mp3

// sprite
typedef struct  {
	int		x, y;	// screen coordinates
	int		w;	// width of one image
	u8**		img;	// pointer to image
	int		num;	// number of sprite phases
	int		anim;	// number of animation loops
	int		delmin;	// minimal delay
	int		delmax;	// minimal delay
} sSprite;

u8 *BackImg, *BirdImg, *CatImg, *CometImg, *DigitImg, *DogImg, *FrameImg;
u8 *HareImg, *MoleImg, *MoonImg, *MouseImg, *PfImg, *PfAllImg, *SparkImg, *StarImg;

// sprites
sSprite Sprite[] = {
	// Moon
	{ 9, 19, 48, &MoonImg, 2, 1, 5, 45 },

	// dog
	{ 38, 128, 52, &DogImg, 3, 10, 10, 100 },
	
	// mouse
	{ 10, 175, 39, &MouseImg, 3, 10, 10, 100 },

	// mole
	{ 212, 134, 40, &MoleImg, 3, 10, 10, 100 },

	// hare
	{ 262, 164, 37, &HareImg, 3, 1, 5, 80 },

	// bird
	{ 221, 212, 23, &BirdImg, 2, 1, 5, 45 },

	// stars
	{ 23, 79, 11, &StarImg, 3, 1, 20, 120 },
	{ 63, 72, 11, &StarImg, 3, 1, 20, 120 },
	{ 88, 88, 11, &StarImg, 3, 1, 20, 120 },
	{ 87, 23, 11, &StarImg, 3, 1, 20, 120 },
	{ 119, 56, 11, &StarImg, 3, 1, 20, 120 },
	{ 136, 12, 11, &StarImg, 3, 1, 20, 120 },
	{ 184, 25, 11, &StarImg, 3, 1, 20, 120 },
	{ 200, 72, 11, &StarImg, 3, 1, 20, 120 },
	{ 231, 91, 11, &StarImg, 3, 1, 20, 120 },
	{ 279, 75, 11, &StarImg, 3, 1, 20, 120 },

	// sparks
	{ 152, 71, 11, &SparkImg, 3, 1, 10, 60 },
	{ 159, 95, 11, &SparkImg, 3, 1, 10, 60 },
	{ 144, 119, 11, &SparkImg, 3, 1, 10, 60 },
	{ 119, 152, 11, &SparkImg, 3, 1, 10, 60 },
	{ 176, 141, 11, &SparkImg, 3, 1, 10, 60 },
};

#define SPRITENUM	count_of(Sprite)

// time of next animation
u32 NextAnim[SPRITENUM];

// counter of animation loops
int AnimLoop[SPRITENUM];
int AnimStep[SPRITENUM];

#define FRAME_X		210
#define FRAME_Y		10
#define FRAME_INX	(FRAME_X + FRAMEIMG_INX)
#define FRAME_INY	(FRAME_Y + FRAMEIMG_INY + 2)

#define PFALLIMG_TW	120

#define FRAMEIMG_INX	10
#define FRAMEIMG_INY	10
#define FRAMEIMG_INW	76
#define FRAMEIMG_INH	28

#define PFALLIMG_W	120
#define PFALLIMG_H	24
#define PFALLIMG_TW	120

#define COMETIMG_W	15

// draw PF and update (pfshift = +FRAMEIMG_INW ... -PFALLIMG_TW)
void DrawPf(int pfshift)
{
	// draw frame
	DrawImg(FrameImg, FRAME_X, FRAME_Y);

	// draw PF text
	if (pfshift >= 0)
		DrawImg(PfAllImg, FRAME_INX+pfshift, FRAME_INY, 0, 0, FRAMEIMG_INW-pfshift, PFALLIMG_H);
	else
		if (pfshift >= FRAMEIMG_INW - PFALLIMG_W)
			DrawImg(PfAllImg, FRAME_INX, FRAME_INY, -pfshift, 0, FRAMEIMG_INW, PFALLIMG_H);
		else
			DrawImg(PfAllImg, FRAME_INX, FRAME_INY, -pfshift, 0, PFALLIMG_W+pfshift, PFALLIMG_H);

	// update display
	DispUpdate();
}

// MP3 player
sMP3Player MP3Player;
u8 ALIGNED MP3PlayerOutBuf[MP3PLAYER_OUTSIZE];

int main()
{
	int i;
	const sSprite* s;
	int comx = -400;
	int comy = -110;
	int pfshift = FRAMEIMG_INW;

	// decompress images
	BackImg = (u8*)PNGLOAD(BackImgPNG);
	BirdImg = (u8*)PNGLOAD(BirdImgPNG);
	CatImg = (u8*)PNGLOAD(CatImgPNG);
	CometImg = (u8*)PNGLOAD(CometImgPNG);
	DigitImg = (u8*)PNGLOAD(DigitImgPNG);
	DogImg = (u8*)PNGLOAD(DogImgPNG);
	FrameImg = (u8*)PNGLOAD(FrameImgPNG);
	HareImg = (u8*)PNGLOAD(HareImgPNG);
	MoleImg = (u8*)PNGLOAD(MoleImgPNG);
	MoonImg = (u8*)PNGLOAD(MoonImgPNG);
	MouseImg = (u8*)PNGLOAD(MouseImgPNG);
	PfImg = (u8*)PNGLOAD(PfImgPNG);
	PfAllImg = (u8*)PNGLOAD(PfAllImgPNG);
	SparkImg = (u8*)PNGLOAD(SparkImgPNG);
	StarImg = (u8*)PNGLOAD(StarImgPNG);

	// play sound
	sMP3Player* mp3 = &MP3Player;
	int r = MP3PlayerInit(mp3, NULL, SilentSnd, sizeof(SilentSnd), MP3PlayerOutBuf, MP3PLAYER_OUTSIZE, -1);
	if (r == ERR_MP3_OK) MP3Play(mp3, 0, True);

#define ANIM_DELTA	100	// animation delta time in [ms]

	// time of first animation
	s = Sprite;
	for (i = 0; i < SPRITENUM; i++)
	{
		NextAnim[i] = (RandU16MinMax(s->delmin, s->delmax) * ANIM_DELTA)*1000 + Time();
		AnimLoop[i] = s->anim;
		AnimStep[i] = 0;
		s++;
	}

	// main loop
	while (True)
	{
		// music poll
		MP3Poll(mp3);

		// display background
		DrawImg(BackImg, 0, 0);

		// draw sprites
		s = Sprite;
		for (i = 0; i < SPRITENUM; i++)
		{
			DrawImg(*s->img, s->x, s->y, AnimStep[i] * s->w, 0, s->w);
			s++;
		}

		// draw comet
		DrawImg(CometImg, comx, comy, (comy & 1) * COMETIMG_W, 0, COMETIMG_W);

		// draw PF and update
		DrawPf(pfshift);

		// animation
		s = Sprite;
		for (i = 0; i < SPRITENUM; i++)
		{
			// animation time reacher?
			if ((s32)(Time() - NextAnim[i]) >= 0)
			{
				// increase animation step
				AnimStep[i]++;
				if (AnimStep[i] >= s->num)
				{
					// one animation loop completed
					AnimStep[i] = 1;
					AnimLoop[i]--;
					if (AnimLoop[i] <= 0)
					{
						// prepare for next animation
						NextAnim[i] = (RandU16MinMax(s->delmin, s->delmax) * ANIM_DELTA)*1000 + Time();
						AnimLoop[i] = s->anim;
						AnimStep[i] = 0;
					}
				}
			}

			s++;
		}

		// comet animation
		comx += 4;
		comy += 1;
		if (comx > 450)
		{
			comx = -400;
			comy = -110;
		}

		// frame animation
		pfshift -= 2;
		if (pfshift < -PFALLIMG_TW) pfshift = FRAMEIMG_INW;
		WaitMs(30);

		// draw PF and update
		DrawPf(pfshift);

		// frame animation
		pfshift -= 2;
		if (pfshift < -PFALLIMG_TW) pfshift = FRAMEIMG_INW;

		// delay
		WaitMs(ANIM_DELTA-40);

		// keyboard
		int key = KeyGet();
		if (key == KEY_ESC) Reboot();	// Program exit
		if (key == KEY_SCREENSHOT) ScreenShot(); //  Screenshot - This may take a few seconds to write.
	}

	return 0;
}
