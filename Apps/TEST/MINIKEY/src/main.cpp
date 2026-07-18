
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

// Keys
const u8 Keys[] = {
	KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, 255,
	KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, 255,
	KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, KEY_BS, 255,
	KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, NOKEY, KEY_UP, NOKEY, KEY_ENTER, 255,
	KEY_ALT, KEY_SHIFT, KEY_CTRL, KEY_SPACE, KEY_ESC, KEY_TAB, KEY_LEFT, NOKEY, KEY_DOWN, NOKEY, KEY_RIGHT, 255,
};

// Key labels
const char* Lab[] = {
	" 1 ", " 2 ", " 3 ", " 4 ", " 5 ", " 6 ", " 7 ", " 8 ", " 9 ", " 0 ", "",
	" Q ", " W ", " E ", " R ", " T ", " Y ", " U ", " I ", " O ", " P ", "",
	" A ", " S ", " D ", " F ", " G ", " H ", " J ", " K ", " L ", "BS ", "",
	" Z ", " X ", " C ", " V ", " B ", " N ", " M ", "", " \15 ", "", "Ent", "",
	"Alt", "Shf", "Ctr", "Spc", "Esc", "Tab", " \16 ", "", " \17 ", "", " \14 ", "",
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
	int i, x, y;
	const u8* k;
	const char** t;
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
		k = Keys;
		t = Lab;
		x = 4;
		y = 4;
		for (i = 0; i < count_of(Keys); i++)
		{
			key = *k++;
			if (key == NOKEY)
			{
				x += 16;
			}
			else if (key == 255)
			{
				x = 4;
				y += 35;
			}
			else
			{
				DrawText(*t, x, y, COL_BLACK);
				DrawRound(x+12, y+16+8, 16, DRAW_ROUND_ALL, KeyMap[key-1] ? COL_RED : COL_BLACK);
				x += 32;
			}
			t++;
		}

		// read key
		key = KeyGet();
		if (key != NOKEY)
		{
			if (key == KEY_PAD_Y)
			{
				if (lastesc) Reboot();	// Program exit
				lastesc = True;
			}
			else
				lastesc = False;
			if (key == KEY_SCREENSHOT) ScreenShot(); //  Screenshot - This may take a few seconds to write.

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
		KeyRet(key);
		ch = KeyGetChar();
		if (ch != NOCHAR)
		{
			if (AscBufPos >= 40) AscBufPos = 0;
			AscBuf[AscBufPos++] = ch;
			AscBuf[AscBufPos] = 0;
		}

		// display buffers
		DrawText(HexBuf1, 0, 192, COL_YELLOW);
		DrawText(HexBuf2, 0, 192+16, COL_YELLOW);
		DrawText(AscBuf, 0, 224, COL_YELLOW);

		// display update
		DispUpdate();
	}
}
