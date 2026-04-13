
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#ifndef _MAIN_H
#define _MAIN_H

// language selection
#define LANG_CZ		0
#define LANG_EN		1
extern int Lang;		// current selected language

// decompressed images
extern u8 *BackgroundImg;
extern u8 *BlacksRedsCZImg, *BlacksRedsENImg, *CardBaseImg, *CardImage, *CardTextCZImg;
extern u8 *CardTextENImg, *CastleImg, *CastleShadowImg, *CloudImg, *CursorImg, *DiscardCZImg;
extern u8 *DiscardENImg, *FenceImg, *FenceShadowImg, *FlagsImg, *GrassImg, *LangImg, *OpenImg;
extern u8 *OpenSelImg, *OpenSel2Img, *OpenTextCZImg, *OpenTextENImg, *SelectImg, *StateImg;
extern u8 *StateCZImg, *StateENImg, *Title2CZImg, *Title2ENImg, *Title3_1Img, *Title3_3Img;
extern u8 *Title3_4Img, *TitleCZImg, *TitleENImg, *TrumpetImg, *WinImg;

extern u8 *BlacksRedsImg, *CardTextImg, *DiscardImg, *OpenText, *StateImg2, *Title2Img, *TitleImg;

// sounds
extern const u8 ApplauseSnd[], BirdsSnd[], BuildCastleSnd[], BuildFenceSnd[];
extern const u8 CardSnd[], CurseSnd[], DecreaseStocksSnd[], FanfaresSnd[];
extern const u8 IncreasePowerSnd[], IncreaseStocksSnd[], RuinCastleSnd[], RuinFenceSnd[];

#endif // _MAIN_H
