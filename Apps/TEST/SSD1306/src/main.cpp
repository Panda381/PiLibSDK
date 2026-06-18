
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

// display driver
cSSD1306 Disp;

#define DISP_I2C	0		// I2C bus (0 or 1)
#define DISP_ADDR	SSD1306_ADDR	// display I2C address (SSD1306_ADDR=0x3C default)

// SSD1306 128x64, Pajenicko https://pajenicko.cz/graficky-oled-displej-096-128x64-i2c-bily
#define DISP_W		128		// display physical width (max. 128, or set 132 for SH1106 controller)
#define DISP_H		64		// display physical height (max. 64)

// SSD1306 128x32, Hadex M509 https://www.hadex.cz/p/m509-displej-oled-0-91-x22-128x32-znaky-iic-i2c-4piny-modry
//#define DISP_W	128		// display physical width (max. 128, or set 132 for SH1106 controller)
//#define DISP_H	32		// display physical height (max. 64)

// SH1106 128x32, Hadex M510 https://www.hadex.cz/p/m510-displej-oled-1-3-x22-128x64-znaky-iic-i2c-4piny-modry
//#define DISP_W	132		// display physical width (max. 128, or set 132 for SH1106 controller)
//#define DISP_H	64		// display physical height (max. 64)

// SSD1306 64x48, Hadex M510A https://www.hadex.cz/p/m510a-displej-oled-0-66-x22-mini-64x48-znaku-iic-i2c
//  Connect pins on OLED shield to I2C bus:
//	3V3 -> VDD
//	GND -> GND
//	D1 -> SCL
//	D2 -> SDA
//#define DISP_W	64		// display physical width (max. 128, or set 132 for SH1106 controller)
//#define DISP_H	48		// display physical height (max. 64)

// SSD1306 70x40, GMe https://www.gme.cz/v/1509011/oled-displej-042-70x40-i2c-bily
//#define DISP_W	70		// display physical width (max. 128, or set 132 for SH1106 controller)
//#define DISP_H	40		// display physical height (max. 64)

int main()
{
	int key, rot, i;

	// On start of power - wait for at least 5ms to stabilize power supply of OLED.
	WaitMs(10);

	// main loop
	rot = SSD1306_ROT_0;
	while (True)
	{
		// initialize display driver
		DrawClear();
		Bool res = Disp.Init(0, DISP_ADDR, DISP_W, DISP_H, rot, I2C_DEF_SPEED);
		printf("SSD1306 initialized ");
		if (res)
			printf("OK\n");
		else
			printf("ERROR\n");

		// After Init() delay 2ms to guarantee initialization.
		WaitMs(2);

		// draw frame
		Disp.DrawClear();
		int w = Disp.w;
		int h = Disp.h;
		Disp.DrawFrameSet(0, 0, w, h);
		Disp.DrawRectSet(2, 2, w-4, h-4);

		// draw text
		Disp.SetFont(FontTiny5x8, 5, 8);
		Disp.DrawText("HELLO", (w-5*5)/2, h/2-13, Disp.ColBlack);
		Disp.DrawText("world", (w-5*5)/2, h/2-4, Disp.ColBlack);

		// draw mono image with background
		Disp.DrawImg(Img, (w-21)/2, h/2+5, Disp.ColBlack);

		// display update	
		Disp.Update();

		// copy frame buffer to the main screen
		Disp.CopyScreen((WIDTH-w)/2, (HEIGHT-h)/2);
		DispUpdate();

		// delay some time
		for (i = 20; i > 0; i--)
		{
			// keyboard
			key = KeyGet();
			if (key == KEY_Y) Reboot();	// Program exit
			if (key == KEY_SCREENSHOT) ScreenShot(); //  Screenshot - This may take a few seconds to write.
			WaitMs(100);
		}

		// rotate
		rot++;
		if (rot >= 4) rot = 0;
	}
}
