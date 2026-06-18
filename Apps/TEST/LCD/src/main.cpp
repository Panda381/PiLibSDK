
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

// display driver
cLCD LCD;

// display ST7780 320x240 (default)
#define LCD_MODEL	LCD_MODEL_ST7789
#define LCD_WIDTH	240
#define LCD_HEIGHT	320
#define LCD_OFFX	0
#define LCD_OFFY	0

// display ST7735 160x80
#define LCD_MODEL2	LCD_MODEL_ST7735
#define LCD_WIDTH2	80
#define LCD_HEIGHT2	160
#define LCD_OFFX2	26
#define LCD_OFFY2	1

// frame buffer
u32 DispBuf[320*240];

int main()
{
	int key, rot, i, model;
	int step = 0;

	// On start of power - wait for at least 5ms to stabilize power supply of the display
	WaitMs(10);

	// main loop
	model = LCD_MODEL_NONE;
	rot = LCD_ROT_270; // current rotation
	while (True)
	{
		// step counter
		DrawClear();
		printf("step %d\n", step);
		step++;

		// initialization
		if (rot == LCD_ROT_270)
		{
			// auto-detect
			model = cLCD::Detect();

			// initialize display
			if (model == LCD_MODEL_ST7735)
				LCD.Init(LCD_MODEL2, LCD_WIDTH2, LCD_HEIGHT2, LCD_OFFX2, LCD_OFFY2, DispBuf, rot, LCD_DEF_SPEED);
			else
				LCD.Init(LCD_MODEL, LCD_WIDTH, LCD_HEIGHT, LCD_OFFX, LCD_OFFY, DispBuf, rot, LCD_DEF_SPEED);
		}

		// set rotation
		LCD.SetRot(rot);

		// print info
		if (model == LCD_MODEL_ST7789)
			printf("Auto-detect: ST7789 240x320\n");
		else if (model == LCD_MODEL_ST7735)
			printf("Auto-detect: ST7735 80x160\n");
		else
			printf("Not detected, using default ST7789 240x320\n");

		// draw image
		if (LCD.w == 320)
			LCD.DrawImg(Img320x240); 
		else if (LCD.w == 240)
			LCD.DrawImg(Img240x320); 
		else if (LCD.h == 80)
			LCD.DrawImg(Img160x80);
		else
			LCD.DrawImg(Img80x160);

		// display update
		LCD.Update();

		// clear screen
		DrawRect(0, 50, WIDTH, HEIGHT-50, COL_BLACK);			

		// copy to screen
		LCD.CopyToScreen((WIDTH - LCD.w)/2, (HEIGHT - LCD.h)/2);
		DispUpdate();

		// keyboard
		key = KeyGet();
		if (key == KEY_Y) Reboot();	// Program exit
		if (key == KEY_SCREENSHOT) ScreenShot(); //  Screenshot - This may take a few seconds to write.

		// delay
		WaitMs(1000);

		// rotate
		rot++;
		if (rot >= 4) rot = 0;
	}
}
