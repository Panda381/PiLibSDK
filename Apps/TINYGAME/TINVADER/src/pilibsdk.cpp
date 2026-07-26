
#include "../include.h"

// Frame buffer: 160x120
// Draw buffer: 128x64
//  X = 16, Y = 28

#define OFFSET_X	16
#define OFFSET_Y	28

u32 Disp_OutCol = COL_WHITE;	// current output color (old: DispOutCol)
u8 Disp_OutPage = 0;		// current output page (0..7)
u8 Disp_OutX = 0;		// current output X (0..127)

// Display select simulated I2C page
void DispI2C_SelectPage(int page)
{
	Disp_OutPage = page;
	Disp_OutX = 0;
}

// write a byte over simulated I2C (write to frame buffer), using Disp_OutCol color
// - After write all data, send image to display with DispUpdate().
void DispI2C_Write(u8 data)
{
	int i;
	int y = Disp_OutPage << 3;
	int x = Disp_OutX;
	u32 col = Disp_OutCol;
	u32* d = &FrameBuffer.drawbuf[x + OFFSET_X + (y + OFFSET_Y)*WIDTH];
	if ((data & B0) != 0) d[0*WIDTH] = col; else d[0*WIDTH] = COL_BLACK;
	if ((data & B1) != 0) d[1*WIDTH] = col; else d[1*WIDTH] = COL_BLACK;
	if ((data & B2) != 0) d[2*WIDTH] = col; else d[2*WIDTH] = COL_BLACK;
	if ((data & B3) != 0) d[3*WIDTH] = col; else d[3*WIDTH] = COL_BLACK;
	if ((data & B4) != 0) d[4*WIDTH] = col; else d[4*WIDTH] = COL_BLACK;
	if ((data & B5) != 0) d[5*WIDTH] = col; else d[5*WIDTH] = COL_BLACK;
	if ((data & B6) != 0) d[6*WIDTH] = col; else d[6*WIDTH] = COL_BLACK;
	if ((data & B7) != 0) d[7*WIDTH] = col; else d[7*WIDTH] = COL_BLACK;

	x++;
	if (x >= 128)
	{
		x = 0;
		Disp_OutPage = (Disp_OutPage + 1) & 7;
	}

	Disp_OutX = x;
}

// add byte over simulated I2C (write to frame buffer), write only '1' bits
// - After write all data, send image to display with DispUpdate().
void DispI2C_Add(int x, int y, u8 data, u32 col)
{
	y <<= 3;
	u32* d = &FrameBuffer.drawbuf[x + OFFSET_X + (y + OFFSET_Y)*WIDTH];
	if ((data & B0) != 0) d[0*WIDTH] = col;
	if ((data & B1) != 0) d[1*WIDTH] = col;
	if ((data & B2) != 0) d[2*WIDTH] = col;
	if ((data & B3) != 0) d[3*WIDTH] = col;
	if ((data & B4) != 0) d[4*WIDTH] = col;
	if ((data & B5) != 0) d[5*WIDTH] = col;
	if ((data & B6) != 0) d[6*WIDTH] = col;
	if ((data & B7) != 0) d[7*WIDTH] = col;
}

// set byte over simulated I2C (write to frame buffer)
// - After write all data, send image to display with DispUpdate().
void DispI2C_Set(int x, int y, u8 data, u32 col)
{
	y <<= 3;
	u32* d = &FrameBuffer.drawbuf[x + OFFSET_X + (y + OFFSET_Y)*WIDTH];
	if ((data & B0) != 0) d[0*WIDTH] = col; else d[0*WIDTH] = COL_BLACK;
	if ((data & B1) != 0) d[1*WIDTH] = col; else d[1*WIDTH] = COL_BLACK;
	if ((data & B2) != 0) d[2*WIDTH] = col; else d[2*WIDTH] = COL_BLACK;
	if ((data & B3) != 0) d[3*WIDTH] = col; else d[3*WIDTH] = COL_BLACK;
	if ((data & B4) != 0) d[4*WIDTH] = col; else d[4*WIDTH] = COL_BLACK;
	if ((data & B5) != 0) d[5*WIDTH] = col; else d[5*WIDTH] = COL_BLACK;
	if ((data & B6) != 0) d[6*WIDTH] = col; else d[6*WIDTH] = COL_BLACK;
	if ((data & B7) != 0) d[7*WIDTH] = col; else d[7*WIDTH] = COL_BLACK;
}

// clear byte over simulated I2C (write to frame buffer)
// - After write all data, send image to display with DispUpdate().
void DispI2C_Clr(int x, int y)
{
	y <<= 3;
	u32* d = &FrameBuffer.drawbuf[x + OFFSET_X + (y + OFFSET_Y)*WIDTH];
	u32 col = COL_BLACK;
	d[0*WIDTH] = col;
	d[1*WIDTH] = col;
	d[2*WIDTH] = col;
	d[3*WIDTH] = col;
	d[4*WIDTH] = col;
	d[5*WIDTH] = col;
	d[6*WIDTH] = col;
	d[7*WIDTH] = col;
}

// play sound tone
void Sound(uint8_t freq, uint8_t dur)
{
	sMelodyTone m[2];
	u32 n, f;

	if (freq == 0)
		WaitMs(dur);
	else
	{
// tone period = 510 - 2*freq [us]
// frequency in [Hz] = 1000000/(510-2*freq)
// divider = 1000000 / (1000000/(510-2*freq)) - 1 = 509 - 2*freq
// tone length = dur * (510-2*freq) [us]

		// tone length in [us]
		n = 510 - 2*freq;		// tone period in [us]
		f = 100000000/n;		// tone frequency in 0.01 Hz
		n *= dur;			// total tone length in [us]
		n += n/2; // /2 = a slight prolongation, because the original is slowed down by a 'for' loop

		// setup tone length
		m[0].len = (u32)(((u64)n*PWMSND_RATE+500000) / 1000000); // tone length in 'n' us

		// setup tone increment
		m[0].inc = SND_TONE_INC(f);

		// play tone
		m[1].len = 0;	// stop mark
		//PlayMelody(m);
		PlaySound((const void*)m, 0, SNDREPEAT_NO, 0, 0.5f);
		while (PlayingSound()) {}
	}
}
