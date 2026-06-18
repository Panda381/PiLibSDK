
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

int main()
{
	int bus, i, key, addr;

	// wait
	while (True)
	{
		// clear screen
		DrawClear();

		// bus loop
		for (bus = 0; bus <= 1; bus++)
		{
			// prepare to scan I2C bus
			printf("I2C%d:", bus);

			// address loop (address 0x00 is broadcast address, 0x01-0x02 are reserved, 0x78-0x7F is 10bit prefix)
			for (addr = 3; addr <= 0x77; addr++)
			{
				// check I2C device address
				Bool res = I2Cbus_Check(bus, addr);

				// device was found
				if (res) printf(" 0x%02X", addr);
			}

			// end of list
			printf("\n");
		}

		// display update
		DispUpdate();

		// delay
		WaitMs(100);

		// keyboard
		key = KeyGet();
		if (key == KEY_Y) Reboot();	// Program exit
		if (key == KEY_SCREENSHOT) ScreenShot(); //  Screenshot - This may take a few seconds to write.
	}
}
