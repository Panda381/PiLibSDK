
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#ifndef _MAIN_H
#define _MAIN_H

extern u8 *IntroImg, *TilesImg;

#define TILESIMGW	640
#define TILESNUMX	20	// number of tiles in X direction

// language
#define LANG_EN		0	// English language (default)
#define LANG_CZ		1	// Czech language
extern int Lang;		// current language LANG_*

// sounds
extern const u8 CollectSnd[];
extern const u8 CrashSnd[];
extern const u8 StepSnd[];
extern const u8 SuccessSnd[];
extern const u8 TrainSnd[];

#endif // _MAIN_H
