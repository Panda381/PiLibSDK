
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

// title font: Jokerman Regular https://www.azfonts.net/fonts/jokerman/regular-230489#google_vignette
// title2 font: Arial round

// images
#include "../img/background.cpp"	// const u8 BackgroundImgJPG[] 640x480
#include "../img/blacksredsCZ.cpp"	// const u8 BlacksRedsCZImgPNG[] 144x32 (2x1 tiles, 1 tile 72x32)
#include "../img/blacksredsEN.cpp"	// const u8 BlacksRedsENImgPNG[] 144x32 (2x1 tiles, 1 tile 72x32)
#include "../img/cardbase.cpp"		// const u8 CardBaseImgPNG[] 320x96 (5x1 tiles, 1 tile 64x96)
#include "../img/cardimage.cpp"		// const u8 CardImagePNG[] 1920x96 (30x1 tiles, 1 tile 64x96)
#include "../img/cardtextCZ.cpp"	// const u8 CardTextCZImgPNG[] 1920x96 (30x1 tiles, 1 tile 64x96)
#include "../img/cardtextEN.cpp"	// const u8 CardTextENImgPNG[] 1920x96 (30x1 tiles, 1 tile 64x96)
#include "../img/castle.cpp"		// const u8 CastleImgPNG[] 256x232 (2x1 tiles, 1 tile 128x232)
#include "../img/castle_shadow.cpp"	// const u8 CastleShadowImgPNG[] 228x116
#include "../img/cloud.cpp"		// const u8 CloudImgPNG[] 32x25
#include "../img/cursor.cpp"		// const u8 CursorImgPNG[] 16x30
#include "../img/discardCZ.cpp"		// const u8 DiscardCZImgPNG[] 64x96
#include "../img/discardEN.cpp"		// const u8 DiscardENImgPNG[] 64x96
#include "../img/fence.cpp"		// const u8 FenceImgPNG[] 9x216
#include "../img/fence_shadow.cpp"	// const u8 FenceShadowImgPNG[] 100x116
#include "../img/flags.cpp"		// const u8 FlagsImgPNG[] 256x32 (8x1 tiles, 1 tile 32x32)
#include "../img/grass.cpp"		// const u8 GrassImgPNG[] 160x32
#include "../img/lang.cpp"		// const u8 LangImgPNG[] 100x30 (2x1 tiles, 1 tile 50x30)
#include "../img/open.cpp"		// const u8 OpenImgPNG[] 384x192 (2x1 tiles, 1 tile 192x192)
#include "../img/open_sel.cpp"		// const u8 OpenSelImgPNG[] 320x40 (4x1 tiles, 1 tile 80x40)
#include "../img/open_sel2.cpp"		// const u8 OpenSel2ImgPNG[] 80x40
#include "../img/opentextCZ.cpp"	// const u8 OpenTextCZImgPNG[] 428x144
#include "../img/opentextEN.cpp"	// const u8 OpenTextENImgPNG[] 428x144
#include "../img/select.cpp"		// const u8 SelectImgPNG[] 240x40 (3x1 tiles, 1 tile 80x40)
#include "../img/state.cpp"		// const u8 StateImgPNG[] 72x272
#include "../img/stateCZ.cpp"		// const u8 StateCZImgPNG[] 48x272
#include "../img/stateEN.cpp"		// const u8 StateENImgPNG[] 48x272
#include "../img/title2CZ.cpp"		// const u8 Title2CZImgPNG[] 56x24
#include "../img/title2EN.cpp"		// const u8 Title2ENImgPNG[] 56x24
#include "../img/title3_1.cpp"		// const u8 Title3_1ImgPNG[] 110x52
#include "../img/title3_3.cpp"		// const u8 Title3_3ImgPNG[] 110x52
#include "../img/title3_4.cpp"		// const u8 Title3_4ImgPNG[] 110x52
#include "../img/titleCZ.cpp"		// const u8 TitleCZImgPNG[] 448x100
#include "../img/titleEN.cpp"		// const u8 TitleENImgPNG[] 448x100
#include "../img/trumpet.cpp"		// const u8 TrumpetImgPNG[] 64x32 (2x1 tiles, 1 tile 32x32)
#include "../img/win.cpp"		// const u8 WinImgPNG[] 1024x32 (8x1 tiles, 1 tile 128x32)

// sounds
#include "../snd/applause.cpp"		// const u8 ApplauseSnd[] PCM mono 8-bit 11025Hz
#include "../snd/birds.cpp"		// const u8 BirdsSnd[] PCM mono 8-bit 11025Hz
#include "../snd/build_castle.cpp"	// const u8 BuildCastleSnd[] PCM mono 16-bit 11025Hz
#include "../snd/build_fence.cpp"	// const u8 BuildFenceSnd[] PCM mono 16-bit 11025Hz
#include "../snd/card.cpp"		// const u8 CardSnd[] PCM mono 8-bit 11025Hz
#include "../snd/curse.cpp"		// const u8 CurseSnd[] PCM mono 16-bit 11025Hz
#include "../snd/decrease_stocks.cpp"	// const u8 DecreaseStocksSnd[] PCM mono 16-bit 11025Hz
#include "../snd/fanfares.cpp"		// const u8 FanfaresSnd[] PCM mono 8-bit 11025Hz
#include "../snd/increase_power.cpp"	// const u8 IncreasePowerSnd[] PCM mono 16-bit 11025Hz
#include "../snd/increase_stocks.cpp"	// const u8 IncreaseStocksSnd[] PCM mono 16-bit 11025Hz
#include "../snd/ruin_castle.cpp"	// const u8 RuinCastleSnd[] PCM mono 16-bit 11025Hz
#include "../snd/ruin_fence.cpp"	// const u8 RuinFenceSnd[] PCM mono 16-bit 11025Hz

const char* SoundText = "Sound: ";
const char* SoundTextSel[3]  = { "OFF ", "SOME", "ALL " };

// decompressed images
u8 *BackgroundImg;
u8 *BlacksRedsCZImg, *BlacksRedsENImg, *CardBaseImg, *CardImage, *CardTextCZImg;
u8 *CardTextENImg, *CastleImg, *CastleShadowImg, *CloudImg, *CursorImg, *DiscardCZImg;
u8 *DiscardENImg, *FenceImg, *FenceShadowImg, *FlagsImg, *GrassImg, *LangImg, *OpenImg;
u8 *OpenSelImg, *OpenSel2Img, *OpenTextCZImg, *OpenTextENImg, *SelectImg, *StateImg;
u8 *StateCZImg, *StateENImg, *Title2CZImg, *Title2ENImg, *Title3_1Img, *Title3_3Img;
u8 *Title3_4Img, *TitleCZImg, *TitleENImg, *TrumpetImg, *WinImg;

// language selection
int Lang = LANG_EN;		// current selected language
u8 *BlacksRedsImg, *CardTextImg, *DiscardImg, *OpenText, *StateImg2, *Title2Img, *TitleImg;

// open selection
int OpenSelInx = 1;

// decompress images
void ImgInit()
{
	BackgroundImg = (u8*)JPGLOAD(BackgroundImgJPG);	if (BackgroundImg == NULL) Reboot();
	BlacksRedsCZImg = (u8*)PNGLOAD(BlacksRedsCZImgPNG); if (BlacksRedsCZImg == NULL) Reboot();
	BlacksRedsENImg = (u8*)PNGLOAD(BlacksRedsENImgPNG); if (BlacksRedsENImg == NULL) Reboot();
	CardBaseImg = (u8*)PNGLOAD(CardBaseImgPNG); if (CardBaseImg == NULL) Reboot();
	CardImage = (u8*)PNGLOAD(CardImagePNG); if (CardImage == NULL) Reboot();
	CardTextCZImg = (u8*)PNGLOAD(CardTextCZImgPNG); if (CardTextCZImg == NULL) Reboot();
	CardTextENImg = (u8*)PNGLOAD(CardTextENImgPNG); if (CardTextENImg == NULL) Reboot();
	CastleImg = (u8*)PNGLOAD(CastleImgPNG); if (CastleImg == NULL) Reboot();
	CastleShadowImg = (u8*)PNGLOAD(CastleShadowImgPNG); if (CastleShadowImg == NULL) Reboot();
	CloudImg = (u8*)PNGLOAD(CloudImgPNG); if (CloudImg == NULL) Reboot();
	CursorImg = (u8*)PNGLOAD(CursorImgPNG); if (CursorImg == NULL) Reboot();
	DiscardCZImg = (u8*)PNGLOAD(DiscardCZImgPNG); if (DiscardCZImg == NULL) Reboot();
	DiscardENImg = (u8*)PNGLOAD(DiscardENImgPNG); if (DiscardENImg == NULL) Reboot();
	FenceImg = (u8*)PNGLOAD(FenceImgPNG); if (FenceImg == NULL) Reboot();
	FenceShadowImg = (u8*)PNGLOAD(FenceShadowImgPNG); if (FenceShadowImg == NULL) Reboot();
	FlagsImg = (u8*)PNGLOAD(FlagsImgPNG); if (FlagsImg == NULL) Reboot();
	GrassImg = (u8*)PNGLOAD(GrassImgPNG); if (GrassImg == NULL) Reboot();
	LangImg = (u8*)PNGLOAD(LangImgPNG); if (LangImg == NULL) Reboot();
	OpenImg = (u8*)PNGLOAD(OpenImgPNG); if (OpenImg == NULL) Reboot();
	OpenSelImg = (u8*)PNGLOAD(OpenSelImgPNG); if (OpenSelImg == NULL) Reboot();
	OpenSel2Img = (u8*)PNGLOAD(OpenSel2ImgPNG); if (OpenSel2Img == NULL) Reboot();
	OpenTextCZImg = (u8*)PNGLOAD(OpenTextCZImgPNG); if (OpenTextCZImg == NULL) Reboot();
	OpenTextENImg = (u8*)PNGLOAD(OpenTextENImgPNG); if (OpenTextENImg == NULL) Reboot();
	SelectImg = (u8*)PNGLOAD(SelectImgPNG); if (SelectImg == NULL) Reboot();
	StateImg = (u8*)PNGLOAD(StateImgPNG); if (StateImg == NULL) Reboot();
	StateCZImg = (u8*)PNGLOAD(StateCZImgPNG); if (StateCZImg == NULL) Reboot();
	StateENImg = (u8*)PNGLOAD(StateENImgPNG); if (StateENImg == NULL) Reboot();
	Title2CZImg = (u8*)PNGLOAD(Title2CZImgPNG); if (Title2CZImg == NULL) Reboot();
	Title2ENImg = (u8*)PNGLOAD(Title2ENImgPNG); if (Title2ENImg == NULL) Reboot();
	Title3_1Img = (u8*)PNGLOAD(Title3_1ImgPNG); if (Title3_1Img == NULL) Reboot();
	Title3_3Img = (u8*)PNGLOAD(Title3_3ImgPNG); if (Title3_3Img == NULL) Reboot();
	Title3_4Img = (u8*)PNGLOAD(Title3_4ImgPNG); if (Title3_4Img == NULL) Reboot();
	TitleCZImg = (u8*)PNGLOAD(TitleCZImgPNG); if (TitleCZImg == NULL) Reboot();
	TitleENImg = (u8*)PNGLOAD(TitleENImgPNG); if (TitleENImg == NULL) Reboot();
	TrumpetImg = (u8*)PNGLOAD(TrumpetImgPNG); if (TrumpetImg == NULL) Reboot();
	WinImg = (u8*)PNGLOAD(WinImgPNG); if (WinImg == NULL) Reboot();
}

// select language LANG_* (does not update display)
void SetLang(int lang)
{
	Lang = lang;
	if (lang == LANG_CZ)	
	{
		BlacksRedsImg = BlacksRedsCZImg;
		CardTextImg = CardTextCZImg;
		DiscardImg = DiscardCZImg;
		OpenText = OpenTextCZImg;
		StateImg2 = StateCZImg;
		Title2Img = Title2CZImg;
		TitleImg = TitleCZImg;
	}
	else // lang == LANG_EN
	{
		BlacksRedsImg = BlacksRedsENImg;
		CardTextImg = CardTextENImg;
		DiscardImg = DiscardENImg;
		OpenText = OpenTextENImg;
		StateImg2 = StateENImg;
		Title2Img = Title2ENImg;
		TitleImg = TitleENImg;
	}
}

// display open screen
void DispOpen()
{
	// clear screen
	DrawClear();

	// display ants
	DrawImg(OpenImg, 0, 0, 0, 0, 192);
	DrawImg(OpenImg, WIDTH-192, 250, 192, 0, 192);

	// display title
	DrawImg(TitleImg, 192, 0);

/*
Text CZ (use Windows 1250 ANSI):
Nikdo z èerných ani èervených mravencù si již nepamatuje, kdo
a kdy zaèal jejich odvìkou válku o mraveništì "U dvou smrkù".
Aby vìèným soubojùm uèinili konec, rozhodli se postavit hrady,
s jejichž pomocí by získali nadvládu nad celým územím.

Cílem hry je postavit hrad o výšce 100 nebo znièit hrad soupeøe.
Každá karta spotøebuje urèité množství surovin (èíslo vpravo
nahoøe). Rychlost tvorby surovin závisí na množství dìlníkù.

Text EN:
None of the black or red ants can remember who and when started
their age-old war over the anthill "By the Two Spruces." To put
an end to their endless battles, they decided to build castles,
which would help them gain control over the entire territory.

Goal of the game is to build a castle with a height of 100 or
to destroy the opponent's castle. Each card consumes a certain
amount of resources (number in the upper right). The rate at
which resources are produced depends on the number of workers.
*/

	// display text
	DrawImg(OpenText, 192, 105);

	// display title 2
	DrawImg(Title2Img, 230+27, 275);
#if RUNMODE == 1
	DrawImg(Title3_1Img, 230, 300);
#elif RUNMODE == 3
	DrawImg(Title3_3Img, 230, 300);
#else // RUNMODE == 4
	DrawImg(Title3_4Img, 230, 300);
#endif

	// language selection
	DrawImg(LangImg, 20, 320, 0, 0, 50);
	DrawImg(LangImg, 20, 370, 50, 0, 50);
	if (Lang == LANG_EN)
		DrawImg(OpenSel2Img, 5, 370-5);
	else
		DrawImg(OpenSel2Img, 5, 320-5);

	// sound selection
	SelFont8x16();
	DrawText(SoundText, 0, HEIGHT-16, COL_GRAY);
	DrawText(SoundTextSel[SoundMode], 7*8, HEIGHT-16, COL_GRAY);

	// draw selection box
	DrawImg(OpenSelImg, 200, HEIGHT-40);
	DrawImg(OpenSel2Img, 200+OpenSelInx*80, HEIGHT-40);

	// display update
	DispUpdate();

	// key flush
	KeyFlush();
}

int main()
{
	// clear wins
	Players[0].wins = 0;
	Players[1].wins = 0;

	// decompress images
	ImgInit();

	// select language
	SetLang(Lang);

	// display open screen
	DispOpen();

	// main loop
	while (True)
	{
		// keyboard
		int key = KeyGet();
		switch (key)
		{
		// LEFT
		case KEY_LEFT:
			OpenSelInx--;
			if (OpenSelInx < 0) OpenSelInx = 3;
			DispOpen();
			break;

		// RIGHT
		case KEY_RIGHT:
			OpenSelInx++;
			if (OpenSelInx > 3) OpenSelInx = 0;
			DispOpen();
			break;

		// UP
		case KEY_UP:
			SetLang(LANG_CZ);
			DispOpen();
			break;

		// DOWN
		case KEY_DOWN:
			SetLang(LANG_EN);
			DispOpen();
			break;

		// Tab
		case KEY_PAD_X:
			SoundMode--;
			if (SoundMode < 0) SoundMode = 2;
			DispOpen();
			break;

		// Enter, Space
		case KEY_PAD_A:
		case KEY_PAD_B:
			switch (OpenSelInx)
			{
			case 0: Game(PLAYER_HUMAN, PLAYER_HUMAN); break;
			case 1: Game(PLAYER_HUMAN, PLAYER_COMP); break;
			case 2: Game(PLAYER_COMP, PLAYER_HUMAN); break;
			case 3: Game(PLAYER_COMP, PLAYER_COMP); break;
			}
			StopAllSound();
			DispOpen();
			break;

		// ESC
		case KEY_PAD_Y:
			Reboot();	// Program exit
			break;

		// Screenshot
		case KEY_SCREENSHOT:
			ScreenShot(); //  Screenshot - This may take a few seconds to write.
			break;

		// LCD display rezoom
		case KEY_ZOOM:
			LCDRezoom();
			break;
		}
	}

	return 0;
}
