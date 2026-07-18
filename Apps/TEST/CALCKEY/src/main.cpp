
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

// Key labels
const char* Lab[45] = {
	"  A", "  B", "  C", "  D", "  E",
	"2nd", "INV", "lnx", " CE", "CLR",
	"LRN", "x/t", " x2", " Vx", "1/x",
	"SST", "STO", "RCL", "SUM", " yx",
	"BST", " EE", "  (", "  )", "  +",
	"GTO", "  7", "  8", "  9", "  x",
	"SBR", "  4", "  5", "  6", "  -",
	"RST", "  1", "  2", "  3", "  +",
	"R/S", "  0", "  .", "+/-", "  =",
};

// HEX buffer
char HexBuf1[40+1] = { 0 };
char HexBuf2[40+1] = { 0 };
int HexBufRow = 0;
int HexBufPos = 0;

char AscBuf[40+1] = { 0 };
int AscBufPos = 0;

int main()
{
	u8 key;
	char ch;
	int i, j, x, y, len;
	const u8* k;
	const char** t;
	const char* txt;
	Bool lastesc;

	// clear
	DrawClear();

	// wait
	lastesc = False;
	while (True)
	{
		// Clear screen
		DrawRect(0, 0, WIDTH, 192, COL_WHITE);
		DrawRect(0, 192, WIDTH, 32, COL_BLUE);
		DrawRect(0, 224, WIDTH, 16, COL_DKGREEN);

		// draw keyboard
		t = Lab;
		y = 4;
		for (i = 1; i <= 9; i++)
		{
			x = 8;
			for (j = 1; j <= 5; j++)
			{
				key = (i << 4) | j;
				DrawText(*t, x, y, COL_BLACK);
				DrawRound(x+36, y+8, 16, DRAW_ROUND_ALL, CalcKeyMap[key] ? COL_RED : COL_BLACK);
				x += 60;
				t++;
			}
			y += 21;
		}

		// read key
		key = CalcKeyGetRaw();
		if (key != CALC_NOKEY)
		{
			// scroll
			if (HexBufPos > 40-3)
			{
				if (HexBufRow == 0)
				{
					HexBufRow = 1;
				}
				else
				{
					memcpy(HexBuf1, HexBuf2, 40+1);
				}
				HexBufPos = 0;
			}

			// add character to the buffer
			char* d = (HexBufRow == 0) ? HexBuf1 : HexBuf2;
			i = key >> 4;
			d[HexBufPos++] = (i > 9) ? (i - 10 + 'A') : (i + '0');
			i = key & 0x0f;
			d[HexBufPos++] = (i > 9) ? (i - 10 + 'A') : (i + '0');
			d[HexBufPos++] = ' ';
			d[HexBufPos] = 0;
		}

		// read character
		CalcKeyRet(key);
		key = CalcKeyGet();
		if ((key != CALC_NOKEY) && (key < 0xa0))
		{
			txt = CalcDispName[key];
			len = StrLen(txt);
			if (len > 0)
			{
				if (AscBufPos > 40 - len - 1) AscBufPos = 0;
				memcpy(AscBuf + AscBufPos, txt, len);
				AscBufPos += len;
				AscBuf[AscBufPos++] = ' ';
				AscBuf[AscBufPos] = 0;
			}
		}

		// display buffers
		DrawText(HexBuf1, 0, 192, COL_YELLOW);
		DrawText(HexBuf2, 0, 192+16, COL_YELLOW);
		DrawText(AscBuf, 0, 224, COL_YELLOW);

		// display update
		DispUpdate();

		// keyboard
		key = KeyGet();
		if (key == KEY_PAD_Y) Reboot();	// Program exit
		if (key == KEY_SCREENSHOT) ScreenShot(); //  Screenshot - This may take a few seconds to write.
	}
}
