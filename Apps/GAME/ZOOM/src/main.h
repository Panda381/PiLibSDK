
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#ifndef _MAIN_H
#define _MAIN_H

// debug switchwes (comment all switches on release version!)
//#define DEBUG_SKIP_OPEN		// skip open screen
//#define DEBUG_SKIP_ENTER		// skip entering screen
//#define DEBUG_NOCRASH			// no crash spaceship
//#define DEBUG_LIVES		3	// number of start lives 1.. (default 3)
//#define DEBUG_LEVEL		0	// start level 0.. (default 0)
//#define DEBUG_SUBLEVEL	0	// start sublevel 0..3 (default 0)
//#define DEBUG_GOALS		10	// number of goals

// sound channels
#define SNDCHAN_SHOT	2	// shooting
#define SNDCHAN_FUEL	3	// fuel indicator

// game variables
extern int	Level;		// current game level (0..)
extern int	SubLevel;	// current game sub-level (0..)

#define FRAC	6	// fraction bits of coordinates and fuel

#define HORIZON	56	// horizon Y coordinate

extern u8* City1Img;
#define CITY1_IMG_W	48
#define CITY1_IMG_H	7

// image width: 82 pixels
// image height: 8 lines
extern u8* City2Img;
#define CITY2_IMG_W	82
#define CITY2_IMG_H	8

// image width: 104 pixels
// image height: 384 lines
extern u8* ExplosionImg;
#define EXPLOSION_IMG_W		104
#define EXPLOSION_IMG_H		48

// image width: 320 pixels
// image height: 56 lines
extern u8* SkylineImg;
#define SKYLINE_IMG_W	320
#define SKYLINE_IMG_H	56

// sound format: PCM mono 16-bit 22050Hz
extern const u8 EnteringSnd[];
extern const u8 ExplosionSnd[];
extern const u8 HitSnd[];

// delay with corrections
void Delay(int ms);

// initialize new game
void NewGame();

// entering sector
void Enter();

// display all game screen
void DispAll();

// initialize new level
void InitLevel();

// one game
void Game();

#endif // _MAIN_H
