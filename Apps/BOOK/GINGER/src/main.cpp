
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

// include media (include for faster compilation rather than compile with Makefile)
#include "../img/img01.cpp"
#include "../img/img02.cpp"
#include "../img/img03.cpp"
#include "../img/img04.cpp"
#include "../img/img05.cpp"
#include "../img/img06.cpp"
#include "../img/img07.cpp"
#include "../img/img08.cpp"
#include "../img/img09.cpp"
#include "../img/img10.cpp"
#include "../img/img11.cpp"
#include "../img/img12.cpp"
#include "../img/img13.cpp"
#include "../img/img14.cpp"
#include "../img/img15.cpp"
#include "../img/img16.cpp"
#include "../img/img17.cpp"
#include "../img/img18.cpp"
#include "../img/img19.cpp"
#include "../img/img20.cpp"

#if VERSION_EN		// 1=use English version
#include "../snd/snd01.cpp"
#include "../snd/snd02.cpp"
#include "../snd/snd03.cpp"
#include "../snd/snd04.cpp"
#include "../snd/snd05.cpp"
#include "../snd/snd06.cpp"
#include "../snd/snd07.cpp"
#include "../snd/snd08.cpp"
#include "../snd/snd09.cpp"
#include "../snd/snd10.cpp"
#include "../snd/snd11.cpp"
#include "../snd/snd12.cpp"
#include "../snd/snd13.cpp"
#include "../snd/snd14.cpp"
#include "../snd/snd15.cpp"
#include "../snd/snd16.cpp"
#include "../snd/snd17.cpp"
#include "../snd/snd18.cpp"
#include "../snd/snd19.cpp"
#include "../snd/snd20.cpp"
#include "../snd/snd21.cpp"
#include "../snd/snd22.cpp"

#include "../img/img22.cpp"

#include "../img/text01.cpp"
#include "../img/text02.cpp"
#include "../img/text03.cpp"
#include "../img/text04.cpp"
#include "../img/text05.cpp"
#include "../img/text06.cpp"
#include "../img/text07.cpp"
#include "../img/text08.cpp"
#include "../img/text09.cpp"
#include "../img/text10.cpp"
#include "../img/text11.cpp"
#include "../img/text12.cpp"
#include "../img/text13.cpp"
#include "../img/text14.cpp"
#include "../img/text15.cpp"
#include "../img/text16.cpp"
#include "../img/text17.cpp"
#include "../img/text18.cpp"
#include "../img/text19.cpp"
#include "../img/text20.cpp"
#include "../img/text21.cpp"
#include "../img/text22.cpp"

#endif // VERSION_EN

#if VERSION_CZ		// 1=use Czech version
#include "../../GINGERCZ/snd/snd01.cpp"
#include "../../GINGERCZ/snd/snd02.cpp"
#include "../../GINGERCZ/snd/snd03.cpp"
#include "../../GINGERCZ/snd/snd04.cpp"
#include "../../GINGERCZ/snd/snd05.cpp"
#include "../../GINGERCZ/snd/snd06.cpp"
#include "../../GINGERCZ/snd/snd07.cpp"
#include "../../GINGERCZ/snd/snd08.cpp"
#include "../../GINGERCZ/snd/snd09.cpp"
#include "../../GINGERCZ/snd/snd10.cpp"
#include "../../GINGERCZ/snd/snd11.cpp"
#include "../../GINGERCZ/snd/snd12.cpp"
#include "../../GINGERCZ/snd/snd13.cpp"
#include "../../GINGERCZ/snd/snd14.cpp"
#include "../../GINGERCZ/snd/snd15.cpp"
#include "../../GINGERCZ/snd/snd16.cpp"
#include "../../GINGERCZ/snd/snd17.cpp"
#include "../../GINGERCZ/snd/snd18.cpp"
#include "../../GINGERCZ/snd/snd19.cpp"
#include "../../GINGERCZ/snd/snd20.cpp"
#include "../../GINGERCZ/snd/snd21.cpp"
#include "../../GINGERCZ/snd/snd22.cpp"

#include "../../GINGERCZ/img/img22.cpp"

#include "../../GINGERCZ/img/text01.cpp"
#include "../../GINGERCZ/img/text02.cpp"
#include "../../GINGERCZ/img/text03.cpp"
#include "../../GINGERCZ/img/text04.cpp"
#include "../../GINGERCZ/img/text05.cpp"
#include "../../GINGERCZ/img/text06.cpp"
#include "../../GINGERCZ/img/text07.cpp"
#include "../../GINGERCZ/img/text08.cpp"
#include "../../GINGERCZ/img/text09.cpp"
#include "../../GINGERCZ/img/text10.cpp"
#include "../../GINGERCZ/img/text11.cpp"
#include "../../GINGERCZ/img/text12.cpp"
#include "../../GINGERCZ/img/text13.cpp"
#include "../../GINGERCZ/img/text14.cpp"
#include "../../GINGERCZ/img/text15.cpp"
#include "../../GINGERCZ/img/text16.cpp"
#include "../../GINGERCZ/img/text17.cpp"
#include "../../GINGERCZ/img/text18.cpp"
#include "../../GINGERCZ/img/text19.cpp"
#include "../../GINGERCZ/img/text20.cpp"
#include "../../GINGERCZ/img/text21.cpp"
#include "../../GINGERCZ/img/text22.cpp"

#endif // VERSION_CZ

#define PAGENUM	22		// number of pages

// list of JPG images
const u8* PageImg[PAGENUM] = {
	Img01, Img02, Img03, Img04, Img05,
	Img06, Img07, Img08, Img09, Img10, 
	Img11, Img12, Img13, Img14, Img15,
	Img16, Img17, Img18, Img19, Img20, 
	Img01, Img22, };

// size of JPG images
const int PageImgSize[PAGENUM] = {
	sizeof(Img01), sizeof(Img02), sizeof(Img03), sizeof(Img04), sizeof(Img05),
	sizeof(Img06), sizeof(Img07), sizeof(Img08), sizeof(Img09), sizeof(Img10), 
	sizeof(Img11), sizeof(Img12), sizeof(Img13), sizeof(Img14), sizeof(Img15),
	sizeof(Img16), sizeof(Img17), sizeof(Img18), sizeof(Img19), sizeof(Img20), 
	sizeof(Img01), sizeof(Img22), };

// list of PNG texts
const u8* PageText[PAGENUM] = {
	Text01, Text02, Text03, Text04, Text05,
	Text06, Text07, Text08, Text09, Text10, 
	Text11, Text12, Text13, Text14, Text15,
	Text16, Text17, Text18, Text19, Text20, 
	Text21, Text22, };

// size of PNG texts
const int PageTextSize[PAGENUM] = {
	sizeof(Text01), sizeof(Text02), sizeof(Text03), sizeof(Text04), sizeof(Text05),
	sizeof(Text06), sizeof(Text07), sizeof(Text08), sizeof(Text09), sizeof(Text10), 
	sizeof(Text11), sizeof(Text12), sizeof(Text13), sizeof(Text14), sizeof(Text15),
	sizeof(Text16), sizeof(Text17), sizeof(Text18), sizeof(Text19), sizeof(Text20), 
	sizeof(Text21), sizeof(Text22), };

// list of MP3 sounds
const u8* PageSnd[PAGENUM] = {
	Snd01, Snd02, Snd03, Snd04, Snd05,
	Snd06, Snd07, Snd08, Snd09, Snd10, 
	Snd11, Snd12, Snd13, Snd14, Snd15,
	Snd16, Snd17, Snd18, Snd19, Snd20, 
	Snd21, Snd22, };

// size of MP3 sounds
const int PageSndSize[PAGENUM] = {
	sizeof(Snd01), sizeof(Snd02), sizeof(Snd03), sizeof(Snd04), sizeof(Snd05),
	sizeof(Snd06), sizeof(Snd07), sizeof(Snd08), sizeof(Snd09), sizeof(Snd10), 
	sizeof(Snd11), sizeof(Snd12), sizeof(Snd13), sizeof(Snd14), sizeof(Snd15),
	sizeof(Snd16), sizeof(Snd17), sizeof(Snd18), sizeof(Snd19), sizeof(Snd20), 
	sizeof(Snd21), sizeof(Snd22), };

Bool AutoPlay = True;
int Page = 0;
Bool MP3PlayerWasInit = False;
sMP3Player MP3Player;
u8 ALIGNED MP3PlayerOutBuf[MP3PLAYER_OUTSIZE];

// display current page
void DispPage()
{
	// display JPG image
	u8* pic = (u8*)JPGLoadBuf(PageImg[Page], PageImgSize[Page]);
	if (pic != NULL)
	{
		DrawImg(pic, 0, 0);
		MemFree(pic);
	}

	// display PNG text
	pic = (u8*)PNGLoadBuf(PageText[Page], PageTextSize[Page]);
	if (pic != NULL)
	{
		DrawImg(pic, 0, HEIGHT-64);
		MemFree(pic);
	}

	// display update
	DispUpdate();

	// stop sound
	sMP3Player* mp3 = &MP3Player;
	if (MP3PlayerWasInit) MP3PlayerTerm(mp3);

	if (AutoPlay)
	{
		// play sound of current page
		int r = MP3PlayerInit(mp3, NULL, PageSnd[Page], PageSndSize[Page], MP3PlayerOutBuf, MP3PLAYER_OUTSIZE, -1);
		if (r == ERR_MP3_OK)
		{
			MP3PlayerWasInit = True;
			MP3Play(mp3, 0, False);
			MP3Poll(mp3);
			WaitMs(10);
			if (!MP3Playing(mp3)) AutoPlay = False;
		}
		else
			AutoPlay = False;
	}
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

		case KEY_UP:
			AutoPlay = True;
			if (GlobalSoundVolume < VOLMAX) GlobalVolumeSet(GlobalSoundVolume+1);
			break;

		case KEY_DOWN:
			AutoPlay = True;
			if (GlobalSoundVolume > 0) GlobalVolumeSet(GlobalSoundVolume-1);
			break;

		case KEY_SPACE:
			AutoPlay = !AutoPlay;
			DispPage();
			break;
		}

		// polling MP3
		MP3Poll(&MP3Player);

		// auto play
		if (AutoPlay && (Page < PAGENUM-1) && !MP3Playing(&MP3Player))
		{
			Page++;
			if (Page >= PAGENUM) Page = 0;
			DispPage();
		}
	}

	return 0;
}
