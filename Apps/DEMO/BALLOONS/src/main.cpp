
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

#define CLOUDSW	640	// clouds width
#define CLOUDSH	480	// clouds height

#define HOTAIRW 160	// hot-air balloon width
#define HOTAIRH	199	// hot-air balloon height
#define HOTAIR_NUM 4	// number of hot-air balloons
#define HOTAIR_MINSPEED 1 // min. move speed
#define HOTAIR_MAXSPEED 2 // max. move speed

#define BALLOONW 80	// balloon width
#define BALLOONH 170	// balloon height
#define BALLOON_NUM 40	// number of party balloons
#define BALLOON_SPEED 2	// balloon speed

u8* CloudsImg;
u8* HotairImg;
u8* BlueImg;
u8* GreenImg;
u8* OrangeImg;
u8* RedImg;

// hot-air balloons
int hotx[HOTAIR_NUM];
int hoty[HOTAIR_NUM];
s8 hotdx[HOTAIR_NUM];
s8 hotdy[HOTAIR_NUM];

// party balloons
const u8* balimg_src[4];
const u8* balimg[BALLOON_NUM];
int balx[BALLOON_NUM];
int baly[BALLOON_NUM];
int baldy[BALLOON_NUM];

int main()
{
	int i, j, x, y;

	// decompress Clouds from JPG format
	CloudsImg = (u8*)JPGLOAD(CloudsImgJpg);
	if (CloudsImg == NULL) Reboot(); // emergency exit

	// decompress images from PNG format
	HotairImg = (u8*)PNGLOAD(HotairImgPng);
	if (HotairImg == NULL) Reboot(); // emergency exit

	BlueImg = (u8*)PNGLOAD(BlueImgPng);
	if (BlueImg == NULL) Reboot(); // emergency exit
	balimg_src[0] = BlueImg;

	GreenImg = (u8*)PNGLOAD(GreenImgPng);
	if (GreenImg == NULL) Reboot(); // emergency exit
	balimg_src[1] = GreenImg;

	OrangeImg = (u8*)PNGLOAD(OrangeImgPng);
	if (OrangeImg == NULL) Reboot(); // emergency exit
	balimg_src[2] = OrangeImg;

	RedImg = (u8*)PNGLOAD(RedImgPng);
	if (RedImg == NULL) Reboot(); // emergency exit
	balimg_src[3] = RedImg;

	// current sky animation
	int skyx = 0;
	int skyy = 0;

	// hot-air balloons coordinates
	for (i = 0; i < HOTAIR_NUM; i++)
	{
		hotx[i] = RandU16Max(WIDTH-HOTAIRW);
		hoty[i] = RandU16Max(HEIGHT-HOTAIRH);
		hotdx[i] = RandU8MinMax(HOTAIR_MINSPEED, HOTAIR_MAXSPEED);
		if (RandU8() < 0x80) hotdx[i] = -hotdx[i];
		hotdy[i] = RandU8MinMax(HOTAIR_MINSPEED, HOTAIR_MAXSPEED);
		if (RandU8() < 0x80) hotdy[i] = -hotdy[i];
	}

	// prepare party balloons
	for (i = 0; i < BALLOON_NUM; i++)
	{
		j = RandU8Max(3);
		balimg[i] = balimg_src[j];
		balx[i] = RandU16Max(WIDTH-BALLOONW);
		baly[i] = RandS16MinMax(-BALLOONH, HEIGHT-1);
		baldy[i] = RandU8MinMax(BALLOON_SPEED-1, BALLOON_SPEED+1);
	}

	// main loop
	while (True)
	{
		// keyboard
		int key = KeyGet();
		if (key == KEY_ESC) Reboot();	// Program exit
		if (key == KEY_SCREENSHOT) ScreenShot(); //  Screenshot - This may take a few seconds to write.

	// ==== draw graphics

		// animate sky
		DrawImg(CloudsImg,	skyx - CLOUDSW,	skyy - CLOUDSH	);
		DrawImg(CloudsImg,	skyx,		skyy - CLOUDSH	);
		DrawImg(CloudsImg,	skyx - CLOUDSW,	skyy		);
		DrawImg(CloudsImg,	skyx,		skyy		);

		// animate hot-air balloons
		for (i = 0; i < HOTAIR_NUM; i++) DrawImg(HotairImg, hotx[i], hoty[i]);

		// animate party balloons
		for (i = 0; i < BALLOON_NUM; i++) DrawImg(balimg[i], balx[i], baly[i]);

		// update screen
		DispUpdate();

	// ==== shift graphics

		skyx += 4;
		if (skyx >= CLOUDSW) skyx -= CLOUDSW;
		skyy += 1;
		if (skyy >= CLOUDSH) skyy -= CLOUDSH;

		// shift hot-air balloons
		for (i = 0; i < HOTAIR_NUM; i++)
		{
			x = hotx[i] + hotdx[i];
			if (x < 0)
			{
				x = 0;
				hotdx[i] = RandU8MinMax(HOTAIR_MINSPEED, HOTAIR_MAXSPEED);
			}

			if (x > WIDTH - HOTAIRW)
			{			
				x = WIDTH - HOTAIRW;
				hotdx[i] = -RandU8MinMax(HOTAIR_MINSPEED, HOTAIR_MAXSPEED);
			}
			hotx[i] = x;

			y = hoty[i] + hotdy[i];
			if (y < 0)
			{
				y = 0;
				hotdy[i] = RandU8MinMax(HOTAIR_MINSPEED, HOTAIR_MAXSPEED);
			}

			if (y > HEIGHT - HOTAIRH)
			{			
				y = HEIGHT - HOTAIRH;
				hotdy[i] = -RandU8MinMax(HOTAIR_MINSPEED, HOTAIR_MAXSPEED);
			}
			hoty[i] = y;
		}

		// shift party balloons
		for (i = 0; i < BALLOON_NUM; i++)
		{
			y = baly[i] - baldy[i];
			if (y < -BALLOONH) y += HEIGHT+BALLOONH;
			baly[i] = y;
		}
	}

	return 0;
}
