
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#ifndef _MAIN_H
#define _MAIN_H

// text screen
#define TEXTW	(WIDTH/8)	// width of text screen (=40)
#define TEXTH	(HEIGHT/FONTH)	// height of text screen (=15)

// format: CF_B8G8R8
// image width: 320 pixels
// image height: 240 lines
// image pitch: 960 bytes
extern const u8 IntroImg[230408] __attribute__ ((aligned(4)));

// sound format: MP3
extern const u8 FlappySnd[286824];

// play music
void MusicPlay();

// stop music
void MusicStop();

// poll music
void MusicPoll();

#endif // _MAIN_H
