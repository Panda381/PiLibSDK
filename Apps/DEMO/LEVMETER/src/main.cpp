
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

#include "../img/repro1.cpp"	// const u8 Repro1Img[]
#include "../img/repro1b.cpp"	// const u8 Repro1BImg[]
#include "../img/repro2.cpp"	// const u8 Repro2Img[]
#include "../img/repro2b.cpp"	// const u8 Repro2BImg[]

// sound format: PCM mono 8-bit 22050Hz
// sound format: SNDFORM_PCM8
// sound speed relative to 50000Hz: 0.44100f
//const u8 TestSnd[154524] __attribute__ ((aligned(4))) = {
//	0x90, 0x5B, 0x02, 0x00,	// data size=154512
//	12, 0,			// data offset
//	0x22, 0x56,		// sample rate=22050
//	0x00, 0x00,		// ADPCM samples per block=0
//	8,			// bits per sample
//	1,			// number of channels
#include "../snd/test.cpp"	// const u8 TestSnd[]

#define FRAMECOL 0	// frame color

// repro image
#define REPROW	132	// repro width
#define REPROH	192	// repro height
#define REPROGAP 40	// gap between boxes
#define REPROY	0	// repro Y
#define REPRO1X 8	// repro 1 X
#define REPRO2X	(REPRO1X+REPROW+REPROGAP) // repro 2 X (= 180, right gap 8)

// graph
#define GRAPHGAP 48	// horizontal gap
#define GRAPHW	128	// graph width (16 samples by 8 pixels)
#define GRAPHH	48	// graph height (16 rows by 3 lines)
#define GRAPH1X	8	// graph 1 X
#define GRAPH2X 182	// graph 2 X
#define GRAPHY	(REPROY+REPROH)	// graph Y (= 192)

// data
#define SAMPNUM 16	// number of samples
#define SAMPMAX 16	// max. range of samples

#define BEATNUM	16	// number of sound beats
#define BEATINT	((sizeof(TestSnd)-12)/BEATNUM) // beat interval (= 9657)
#define BEATON (BEATINT/8) // beat on state

// spectrum data line
u8 Samp1[SAMPNUM]; // real data samples 0..15
u8 Samp2[SAMPNUM]; // real data samples 0..15

s8 Samp1Dir[SAMPNUM];
s8 Samp2Dir[SAMPNUM];

u8 Samp1Filt[SAMPNUM]; // filtered samples
u8 Samp2Filt[SAMPNUM]; // filtered samples

// colors
const u32 Colors[SAMPMAX] = {
	COL_CYAN,
	COL_CYAN,

	COL_GREEN,
	COL_GREEN,
	COL_GREEN,
	COL_GREEN,
	COL_GREEN,
	COL_GREEN,

	COL_YELLOW,
	COL_YELLOW,
	COL_YELLOW,
	COL_YELLOW,

	COL_RED,
	COL_RED,
	COL_RED,
	COL_RED,
};

int main()
{
	int i, j, off;
	int a, b;

	// initialize sound output
	PlaySoundRep(TestSnd);

	// draw reproboxes
	DrawImg(Repro1Img, REPRO1X, REPROY);
	DrawImg(Repro2Img, REPRO2X, REPROY);

	// main loop
	while (True)
	{
		// membranes update (by sound offset)
		int off = (sizeof(TestSnd) - 12 - PwmSound[0].cnt) % BEATINT;
		if (off < BEATON)
		{
			DrawImg(Repro1BImg, REPRO1X, REPROY);
			DrawImg(Repro2BImg, REPRO2X, REPROY);
		}
		else
		{
			DrawImg(Repro1Img, REPRO1X, REPROY);
			DrawImg(Repro2Img, REPRO2X, REPROY);
		}

		// generate samples
		for (i = 0; i < SAMPNUM; i++)
		{
			// generate new direction
			if (RandU8() < 100)
			{
				Samp1Dir[i] = RandS8MinMax(-2, +2);
				Samp2Dir[i] = RandS8MinMax(-2, +2);
			}

			// change sample 1 value
			a = Samp1[i] + Samp1Dir[i];
			if (a < 0) a = 0;
			if (a >= SAMPMAX) a = SAMPMAX-1;
			Samp1[i] = a;

			// change sample 2 value
			a = Samp2[i] + Samp2Dir[i];
			if (a < 0) a = 0;
			if (a >= SAMPMAX) a = SAMPMAX-1;
			Samp2[i] = a;
		}

		// display samples
		for (i = 0; i < SAMPNUM; i++)
		{
			 // filter samples for more realistic progress
			a = Samp1[i];
			b = Samp2[i];
			if ((i > 0) && (i < SAMPNUM-1))
			{
				a = (a*2 + Samp1[i-1] + Samp1[i+1]) >> 2;
				b = (b*2 + Samp2[i-1] + Samp2[i+1]) >> 2;
			}

			for (j = 0; j < SAMPMAX; j++)
			{

				if (a >= (SAMPMAX-1-j))
					DrawRect(GRAPH1X+i*8, GRAPHY+j*3, 7, 2, Colors[SAMPMAX-1-j]);
				else
					DrawRect(GRAPH1X+i*8, GRAPHY+j*3, 7, 2, COL_DKBLUE);

				if (b >= (SAMPMAX-j))
					DrawRect(GRAPH2X+i*8, GRAPHY+j*3, 7, 2, Colors[SAMPMAX-1-j]);
				else
					DrawRect(GRAPH2X+i*8, GRAPHY+j*3, 7, 2, COL_DKBLUE);
			}
		}

		// update display
		DispUpdate();

		// short delay
		WaitMs(20);

		// keyboard
		int key = KeyGet();
		if (key == KEY_ESC) Reboot();	// Program exit
		if (key == KEY_SCREENSHOT) ScreenShot(); //  Screenshot - This may take a few seconds to write.
	}

	return 0;
}
