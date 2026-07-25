
// ****************************************************************************
//
//                               Ground
//
// ****************************************************************************

#ifndef _GROUND_H
#define _GROUND_H

#define GROUND_DY	15	// delta of distance of ground lines

#define GRASS_Y_MIN	HORIZON	// minimal Y coordinate of the grass (= 56)
#define GRASS_H		GRASS_IMG_H // grass field height (= 184)
#define GRASS_Y_MAX	(GRASS_Y_MIN+GRASS_H-1)	// maximal Y coordinate of the grass (= 239)
#define HORIZ_LOW	50	// lowering horizon

#define DIST_MIN	0	// minimal distance of objects from projection plane
#define DIST_MAX	255	// maximal distance of objects from projection plane
#define DIST_VIEWER	40	// distance of viewer before projection plane

extern short GrassPhase;		// grass phase, with FRAC fraction

// image width: 320 pixels
// image height: 184 lines
extern u8* Grass1Img;
extern u8* Grass2Img;
extern u8* Grass3Img;
extern u8* Grass4Img;
#define GRASS_IMG_W	320
#define GRASS_IMG_H	184

// perspective projection table (conversion table from object distance to Y coordinate on the screen)
extern short PerspTab[DIST_MAX+1-DIST_MIN];

// calculate perspective projection table
void CalcPerspTab();

extern u8* GrassImgList[4];
extern const u32 GrassColList[4];

// get Y coordinate from perspective table
//  z ... distance with FRAC fraction
int GetPerspTab(int z);

// display ground
void DispGround();

#endif // _GROUND_H
