
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

// include media (include for faster compilation rather than compile with Makefile)
#include "../img/abc01.cpp"
#include "../img/abc02.cpp"
#include "../img/abc03.cpp"
#include "../img/abc04.cpp"
#include "../img/abc05.cpp"
#include "../img/abc06.cpp"
#include "../img/abc07.cpp"
#include "../img/abc08.cpp"
#include "../img/abc09.cpp"
#include "../img/abc10.cpp"
#include "../img/abc11.cpp"
#include "../img/abc12.cpp"
#include "../img/abc13.cpp"
#include "../img/abc14.cpp"
#include "../img/abc15.cpp"
#include "../img/abc16.cpp"
#include "../img/abc17.cpp"
#include "../img/abc18.cpp"
#include "../img/abc19.cpp"
#include "../img/abc20.cpp"
#include "../img/abc21.cpp"
#include "../img/abc22.cpp"
#include "../img/abc23.cpp"

#define PAGENUM	23		// number of pages

// list of PNG pages
const u8* PageAbc[PAGENUM] = {
	Abc01, Abc02, Abc03, Abc04, Abc05,
	Abc06, Abc07, Abc08, Abc09, Abc10, 
	Abc11, Abc12, Abc13, Abc14, Abc15,
	Abc16, Abc17, Abc18, Abc19, Abc20, 
	Abc21, Abc22, Abc23, };

// size of PNG pages
const int PageAbcSize[PAGENUM] = {
	sizeof(Abc01), sizeof(Abc02), sizeof(Abc03), sizeof(Abc04), sizeof(Abc05),
	sizeof(Abc06), sizeof(Abc07), sizeof(Abc08), sizeof(Abc09), sizeof(Abc10), 
	sizeof(Abc11), sizeof(Abc12), sizeof(Abc13), sizeof(Abc14), sizeof(Abc15),
	sizeof(Abc16), sizeof(Abc17), sizeof(Abc18), sizeof(Abc19), sizeof(Abc20), 
	sizeof(Abc21), sizeof(Abc22), sizeof(Abc23), };

int Page = 0;

// display current page
void DispPage()
{
	u8* pic = (u8*)PNGLoadBuf(PageAbc[Page], PageAbcSize[Page]);
	if (pic != NULL)
	{
		DrawImg(pic, 0, 0);
		MemFree(pic);
	}
	DispUpdate();
}

int main()
{
	// display current page
	DispPage();

	while (True)
	{
		// keyboard
		switch (KeyGet())
		{
		case KEY_ESC:
			Reboot();	// Program exit
			break;

		case KEY_SCREENSHOT:
			ScreenShot(); //  Screenshot - This may take a few seconds to write.
			break;

		case KEY_LEFT:
			Page--;
			if (Page < 0) Page = PAGENUM-1;
			DispPage();
			break;

		case KEY_RIGHT:
			Page++;
			if (Page >= PAGENUM) Page = 0;
			DispPage();
			break;
		}
	}

	return 0;
}
