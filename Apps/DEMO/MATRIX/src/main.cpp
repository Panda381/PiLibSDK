
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

#define TEXTWIDTH	(WIDTH/FONTW)	// width of text buffer in number of characters
#define TEXTHEIGHT	(HEIGHT/FONTH)	// height of text buffer in number of characters
#define TEXTSIZE	(TEXTWIDTH*TEXTHEIGHT) // size of text buffer in number of characters

#define LEN_MIN	 	6	// minimal length of drop
#define LEN_MAX  	50	// maximal length of drop
#define OFF_MIN		0	// minimal offset
#define OFF_MAX		100	// maximal offset
#define SPEED_MIN	0.2f	// minimal speed
#define SPEED_MAX	0.5f	// maximal speed
#define CHAR_MIN	33	// minimal character
#define CHAR_MAX	126	// maximal character

// text screen
char TextBuf[TEXTSIZE];

// color screen
u32 ColBuf[TEXTSIZE];

// palette table
const u32 Pal[] = {
	COL_WHITE,
	COLOR(200,255,200),
	COLOR(127,255,127),
	COLOR(96,255,96),
	COLOR(32,255,32),
	COLOR(0,255,0),
	COLOR(0,255,0),
	COLOR(0,255,0),
	COLOR(0,240,0),
	COLOR(0,220,0),
	COLOR(0,200,0),
	COLOR(0,180,0),
	COLOR(0,160,0),
	COLOR(0,150,0),
	COLOR(0,140,0),
	COLOR(0,130,0),
	COLOR(0,120,0),
	COLOR(0,110,0),
	COLOR(0,100,0),
	COLOR(0,80,0),
	COLOR(0,70,0),
	COLOR(0,60,0),
	COLOR(0,50,0),
	COLOR(0,40,0),
	COLOR(0,30,0),
	COLOR(0,20,0),
	COLOR(0,10,0),
};
#define PALLEN count_of(Pal)	// length of palette table

// length of drops
u8 Len[TEXTWIDTH];

// offset of drops
float Off[TEXTWIDTH];

// falling speed
float Speed[TEXTWIDTH];

// generate new drop
void NewDrop(int i)
{
	Len[i] = RandU8MinMax(LEN_MIN, LEN_MAX);
	Off[i] = -RandFloatMinMax(OFF_MIN, OFF_MAX);
	Speed[i] = RandFloatMinMax(SPEED_MIN, SPEED_MAX);
}

int main()
{
	int i, x, y;
	u32 c;
	char* d;
	u32* dd;

	// generate new drops
	for (i = 0; i < TEXTWIDTH; i++)
	{
		NewDrop(i);
		Off[i] *= 2;
	}

	// generate random text screen
	d = TextBuf;
	for (i = TEXTSIZE; i > 0; i -= 3)
	{
		*d = RandU8MinMax(CHAR_MIN, CHAR_MAX);
		d++;
	}

	// main loop
	while (True)
	{
		// update color attributes
		dd = ColBuf;
		for (y = 0; y < TEXTHEIGHT; y++)
		{
			for (x = 0; x < TEXTWIDTH; x++)
			{
				i = (int)((Off[x] - y)/Len[x]*PALLEN + 0.5f);
				if ((i < 0) || (i >= PALLEN))
					c = COL_BLACK;
				else
					c = Pal[i];			
				*dd++ = c;
			}
		}

		// randomize some characters
		for (i = 80; i > 0; i--)
		{
			TextBuf[RandU16Max(TEXTWIDTH*TEXTHEIGHT-1)] = RandU8MinMax(CHAR_MIN, CHAR_MAX);
		}

		// shift drops
		for (x = 0; x < TEXTWIDTH; x++)
		{
			Off[x] += Speed[x];

			// generate new drop
			if (Off[x] - Len[x] > TEXTHEIGHT) NewDrop(x);
		}

		// display text screen
		d = TextBuf;
		dd = ColBuf;
		for (y = 0; y < TEXTHEIGHT; y++)
		{
			for (x = 0; x < TEXTWIDTH; x++)
			{
				DrawCharBg(*d, x*FONTW, y*FONTH, *dd);
				d++;
				dd++;
			}
		}

		// update display
		DispUpdate();
		WaitMs(20);

		// keyboard
		int key = KeyGet();
		if (key == KEY_ESC) Reboot();	// Program exit
		if (key == KEY_SCREENSHOT) ScreenShot(); //  Screenshot - This may take a few seconds to write.
	}

	return 0;
}
