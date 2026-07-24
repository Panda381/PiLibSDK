
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#ifndef _MAIN_H
#define _MAIN_H

// text screen
#define TEXTW	(WIDTH/8)	// width of text screen (=80)
#define TEXTH	(HEIGHT/FONTH)	// height of text screen (=30)

// images
extern const u8 IntroImg320JPG[86994];
extern const u8 IntroImgJPG[113151];

extern const u8 Tiles4ImgPNG[3005];
extern const u8 Tiles8ImgPNG[3335];
extern const u8 Tiles12ImgPNG[3828];
extern const u8 Tiles16ImgPNG[4176];
extern const u8 Tiles20ImgPNG[4572];
extern const u8 Tiles24ImgPNG[4957];
extern const u8 Tiles28ImgPNG[5487];
extern const u8 Tiles32ImgPNG[5784];

extern u8* IntroImg;
extern u8* Tiles4Img;
extern u8* Tiles8Img;
extern u8* Tiles12Img;
extern u8* Tiles16Img;
extern u8* Tiles20Img;
extern u8* Tiles24Img;
extern u8* Tiles28Img;
extern u8* Tiles32Img;

// sound format: PCM mono 16-bit 22050Hz
// sound format: SNDFORM_PCM16
// sound speed relative to 50000Hz: 0.44100f
extern const u8 ShiftSnd[171672] __attribute__ ((aligned(4)));

// sound format: PCM mono 16-bit 22050Hz
// sound format: SNDFORM_PCM16
// sound speed relative to 50000Hz: 0.44100f
extern const u8 YippeeSnd[35816] __attribute__ ((aligned(4)));

// wait for a character
char WaitChar();

#endif // _MAIN_H
