
// ****************************************************************************
//
//                                Lives
//
// ****************************************************************************

#ifndef _LIVES_H
#define _LIVES_H

extern int	Lives;		// number of lives

// image width: 22 pixels
// image height: 6 lines
extern u8* LiveImg;
#define LIVE_IMG_W	22
#define LIVE_IMG_H	6

extern const u8 LivesSnd[];

// display number of lives
void DispLives();

#endif // _LIVES_H
