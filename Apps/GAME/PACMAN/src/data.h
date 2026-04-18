
// ****************************************************************************
//
//                                 Data and tables
//
// ****************************************************************************

#ifndef _DATA_H
#define _DATA_H

// images
extern const u8 LogoImg[14696] __attribute__ ((aligned(4)));
extern const u8 SpritesImg[81608] __attribute__ ((aligned(4)));
extern const u8 SpritesTransImg[108808] __attribute__ ((aligned(4)));
extern const u8 StatusImg[3275] __attribute__ ((aligned(4)));
extern const u8 TilesImg[12908] __attribute__ ((aligned(4)));

#define LOGOW 144
#define LOGOH 34

#define SPRITEW 20
#define SPRITEH 20
#define SPRITEIMGW 1360

#define STATUSW 11
#define STATUSH 11
#define STATUSIMGW 99

#define TILEW 10
#define TILEH 10
#define TILESIZE 10
#define TILESIMGW 430

// sounds
extern const u8 DeathSnd[67676] __attribute__ ((aligned(4)));		// PCM mono 16-bit 22050Hz
extern const u8 EatFruitSnd[19392] __attribute__ ((aligned(4)));	// PCM mono 16-bit 22050Hz
extern const u8 EatGhostSnd[25770] __attribute__ ((aligned(4)));	// PCM mono 16-bit 22050Hz
extern const u8 ExtraPacSnd[83772] __attribute__ ((aligned(4)));	// PCM mono 16-bit 22050Hz
extern const u8 FrightenSnd[11756] __attribute__ ((aligned(4)));	// PCM mono 16-bit 22050Hz
extern const u8 IntermissionSnd[114766] __attribute__ ((aligned(4)));	// PCM mono 8-bit 22050Hz
extern const u8 SirenSnd[18932] __attribute__ ((aligned(4)));		// PCM mono 16-bit 22050Hz
extern const u8 StartSnd[94864] __attribute__ ((aligned(4)));		// PCM mono 8-bit 22050Hz
extern const u8 WakkaSnd[12794] __attribute__ ((aligned(4)));		// PCM mono 16-bit 22050Hz

// game board
extern ALIGNED u8 Board[MAPSIZE];

// animation phase 0..3
extern u8 Phase;

// pacman next direction
extern u8 NextDir;

// current and max score
extern u32 Score;
extern u32 MaxScore;
extern int Level; // current level 0...
extern s8 Lives; // remaining lives
extern int FrightenCnt; // frighten counter
extern int Eaten; // next eaten points
extern int ScatterCnt; // scatter counter
extern int ChaseCnt; // chase counter
extern u8 ScatterTabNext; // next entry in scatter-chasse table
extern int DotCnt; // eaten dot counter
extern int FruitCnt; // fruit counter
extern u32 DispNextTime; // display next time

// level scatter-chasse time table: 5 levels x 4 pairs of 2 entries (scatter and chasse in number of seconds, 0=permanently)
extern const u8 ScatterTab[5*4*2];

// pellet blink counter
extern u8 Blink;

// level fruit table
extern const u8 FruitTab[FRUITTABMAX];

// bonus table
extern const u16 BonusTab[8];

// level frighten time table (number of seconds)
extern const u8 FrightenTab[FRIGHTENTABMAX];

// game board template (to edit, view tiles_num.bmp with tile indices)
extern const u8 BoardTemp[MAPSIZE];

// bonus fruit sprite
extern int FruitSpriteImg; // -1 = not visible
extern int FruitSpriteX;
extern int FruitSpriteY;

// character descriptor
extern sChar Chars[5];

// direction vector
extern const s8 DirX[4];
extern const s8 DirY[4];

// sprite characters indices: 9 characters * 4 directions * 4 animation phase
extern const u8 SpriteInx[CHAR_NUM * DIR_NUM * PHASENUM];

// decode number buffer
extern char NumBuf[15];

#endif // _DATA_H
