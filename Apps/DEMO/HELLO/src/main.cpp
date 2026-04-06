
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

int main()
{
	// draw text
	DrawText4("Hello World!", (WIDTH-12*8*4)/2, (HEIGHT-16*4)/2, COL_WHITE);
	DispUpdate();

	// main loop
	while (True)
	{
		// keyboard
		int key = KeyGet();
		if (key == KEY_ESC) Reboot();	// Program exit
		if (key == KEY_SCREENSHOT) ScreenShot(); //  Screenshot - This may take a few seconds to write.
	}

	return 0;
}
