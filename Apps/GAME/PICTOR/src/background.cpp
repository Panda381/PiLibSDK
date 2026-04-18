
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#include "../include.h"

// background
int BackInx;		// index of background
sBackTemp Back;		// current background template
int BackPhase1, BackPhase2, BackPhase3; // background phases

// background templates
sBackTemp BackTemp[BG_NUM] = {
	// name			img1jpg		img1jpg_size		img2jpg		img2jpg_size		img3jpg		img3jpg_size		img1	img2	img3	dy
	{ "Meadow",		Img01Bg1JPG,	sizeof(Img01Bg1JPG),	Img01Bg2PNG,	sizeof(Img01Bg2PNG),	Img01Bg3PNG,	sizeof(Img01Bg3PNG),	NULL,	NULL,	NULL,	BG2_DY_MEADOW		},
	{ "Jungle",		Img02Bg1JPG,	sizeof(Img02Bg1JPG),	Img02Bg2PNG,	sizeof(Img02Bg2PNG),	Img02Bg3PNG,	sizeof(Img02Bg3PNG),	NULL,	NULL,	NULL,	BG2_DY_JUNGLE		},
	{ "Sandy Beach",	Img03Bg1JPG,	sizeof(Img03Bg1JPG),	Img03Bg2PNG,	sizeof(Img03Bg2PNG),	Img03Bg3PNG,	sizeof(Img03Bg3PNG),	NULL,	NULL,	NULL,	BG2_DY_SANDBEACH	},
	{ "Underwater",		Img04Bg1JPG,	sizeof(Img04Bg1JPG),	Img04Bg2PNG,	sizeof(Img04Bg2PNG),	Img04Bg3PNG,	sizeof(Img04Bg3PNG),	NULL,	NULL,	NULL,	BG2_DY_UNDERWATER	},
	{ "Ice Land",		Img05Bg1JPG,	sizeof(Img05Bg1JPG),	Img05Bg2PNG,	sizeof(Img05Bg2PNG),	Img05Bg3PNG,	sizeof(Img05Bg3PNG),	NULL,	NULL,	NULL,	BG2_DY_ICELAND		},
	{ "Fiery Land",		Img06Bg1JPG,	sizeof(Img06Bg1JPG),	Img06Bg2PNG,	sizeof(Img06Bg2PNG),	Img06Bg3PNG,	sizeof(Img06Bg3PNG),	NULL,	NULL,	NULL,	BG2_DY_FIRELAND		},
	{ "Haunted Hill",	Img07Bg1JPG,	sizeof(Img07Bg1JPG),	Img07Bg2PNG,	sizeof(Img07Bg2PNG),	Img07Bg3PNG,	sizeof(Img07Bg3PNG),	NULL,	NULL,	NULL,	BG2_DY_HAUNTED		},
	{ "Spacecraft",		Img08Bg1JPG,	sizeof(Img08Bg1JPG),	Img08Bg2PNG,	sizeof(Img08Bg2PNG),	Img08Bg3PNG,	sizeof(Img08Bg3PNG),	NULL,	NULL,	NULL,	BG2_DY_SPACECRAFT	},
	{ "Galaxy",		Img09Bg1JPG,	sizeof(Img09Bg1JPG),	Img09Bg2PNG,	sizeof(Img09Bg2PNG),	Img09Bg3PNG,	sizeof(Img09Bg3PNG),	NULL,	NULL,	NULL,	BG2_DY_GALAXY		},
	{ "Alien Planet",	Img10Bg1JPG,	sizeof(Img10Bg1JPG),	Img10Bg2PNG,	sizeof(Img10Bg2PNG),	Img10Bg3PNG,	sizeof(Img10Bg3PNG),	NULL,	NULL,	NULL,	BG2_DY_PLANET		},
	{ "Candy Land",		Img11Bg1JPG,	sizeof(Img11Bg1JPG),	Img11Bg2PNG,	sizeof(Img11Bg2PNG),	Img11Bg3PNG,	sizeof(Img11Bg3PNG),	NULL,	NULL,	NULL,	BG2_DY_CANDYLAND	},
	{ "Surreal Land",	Img12Bg1JPG,	sizeof(Img12Bg1JPG),	Img12Bg2PNG,	sizeof(Img12Bg2PNG),	Img12Bg3PNG,	sizeof(Img12Bg3PNG),	NULL,	NULL,	NULL,	BG2_DY_SURREAL		},
};

// initialize backgrounds on new level
void InitBack()
{
	// prepare background template
	BackInx = Level % BG_NUM;
	memcpy(&Back, &BackTemp[BackInx], sizeof(Back));

	// clear background phase
	BackPhase1 = 0;
	BackPhase2 = 0;
	BackPhase3 = 0;
}

// display backgrounds
void DispBack()
{
	// draw background 1
	if (BackPhase1 < BG_WIDTH - WIDTH)
	{
		DrawImg(Back.img1,	 	// image data and palettes
			0, BG_Y,			// destination X, Y
			BackPhase1, 0,			// source X, Y
			WIDTH, BG_HEIGHT);		// width, height
	}
	else
	{
		DrawImg(Back.img1, 	 	// image data and palettes
			0, BG_Y,			// destination X, Y
			BackPhase1, 0,			// source X, Y
			BG_WIDTH - BackPhase1, BG_HEIGHT); // width, height

		DrawImg(Back.img1, 	 	// image data and palettes
			BG_WIDTH - BackPhase1, BG_Y,	// destination X, Y
			0, 0,				// source X, Y
			WIDTH - (BG_WIDTH - BackPhase1), BG_HEIGHT); // width, height
	}

	// draw background 2
	if (BackPhase2 < BG2_WIDTH - WIDTH)
	{
		DrawImg(Back.img2, 	 	// image data and palettes
			0, BG2_Y+Back.dy,		// destination X, Y
			BackPhase2, 0,			// source X, Y
			WIDTH, BG2_HEIGHT-Back.dy);	// width, height
	}
	else
	{
		DrawImg(Back.img2, 	 	// image data and palettes
			0, BG2_Y+Back.dy,		// destination X, Y
			BackPhase2, 0,			// source X, Y
			BG2_WIDTH - BackPhase2, BG2_HEIGHT-Back.dy); // width, height

		DrawImg(Back.img2, 		// image data and palettes
			BG2_WIDTH - BackPhase2, BG2_Y+Back.dy,	// destination X, Y
			0, 0,				// source X, Y
			WIDTH - (BG2_WIDTH - BackPhase2), BG2_HEIGHT-Back.dy); // width, height
	}

	// draw background 3
	if (BackPhase3 < BG3_WIDTH - WIDTH)
	{
		DrawImg(Back.img3, 	 	// image data and palettes
			0, BG3_Y,			// destination X, Y
			BackPhase3, 0,			// source X, Y
			WIDTH, BG3_HEIGHT);		// width, height
	}
	else
	{
		DrawImg(Back.img3, 	 	// image data and palettes
			0, BG3_Y,			// destination X, Y
			BackPhase3, 0,			// source X, Y
			BG3_WIDTH - BackPhase3, BG3_HEIGHT); // width, height

		DrawImg(Back.img3,  	// image data and palettes
			BG3_WIDTH - BackPhase3, BG3_Y,	// destination X, Y
			0, 0,				// source X, Y
			WIDTH - (BG3_WIDTH - BackPhase3), BG3_HEIGHT); // width, height
	}
}

// shift backgrounds
void ShiftBack()
{
	// background layer 1
	BackPhase1 += BG_SPEED1;
	if (BackPhase1 >= BG_WIDTH) BackPhase1 -= BG_WIDTH;

	// background layer 2
	BackPhase2 += BG_SPEED2;
	if (BackPhase2 >= BG2_WIDTH) BackPhase2 -= BG2_WIDTH;

	// background layer 3
	BackPhase3 += BG_SPEED3;
	if (BackPhase3 >= BG3_WIDTH) BackPhase3 -= BG3_WIDTH;
}
