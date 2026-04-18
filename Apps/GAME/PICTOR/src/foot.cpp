
// ****************************************************************************
//
//                                   Foot
//
// ****************************************************************************

#include "../include.h"

#define LIFE_X	(5*16+4+2)
#define LIFE_Y	(FOOT_Y+4+2)
#define LIFE_W	140
#define LIFE_H	(FOOT_HEIGHT-8-4)

#define LOAD_X	328
#define LOAD_Y	LIFE_Y
#define LOAD_W	60
#define LOAD_H	LIFE_H

#define TIME_Y	LIFE_Y
#define TIME_W	140
#define TIME_H	LIFE_H
#define TIME_X	(WIDTH-4-TIME_W-4)

// life (0..LIFE_MAX)
int Life = 0;

// life colors
const u32 LifeCol[] = {
	COL_BLACK,
	COL_GREEN,
	COL_CYAN,
	COL_YELLOW,
	COL_WHITE,
	COL_MAGENTA,
	COL_MAGENTA
};

// display life
void DispLife()
{
	// prepare life
	int life = Life;
	if (life > LIFE_MAX) life = LIFE_MAX;
	int segm = life/100;
	life -= segm*100;

	// rescale
	life = (life*LIFE_W + 50) / 100;
	if (life > LIFE_W) life = LIFE_W;

	// draw
	DrawRect(LIFE_X, LIFE_Y, life, LIFE_H, LifeCol[segm+1]);
	DrawRect(LIFE_X+life, LIFE_Y, LIFE_W-life, LIFE_H, LifeCol[segm]);
}

// display load weapon
void DispReload()
{
	// prepare time
	int load = ActorReload;
	int reload = MissileTemp[ActInx].reload;
	if (load > reload) load = reload;
	load = reload - load;

	// rescale
	load = (load*LOAD_W + reload/2) / reload;
	if (load > LOAD_W) load = LOAD_W;

	// draw
	DrawRect(LOAD_X, LOAD_Y, load, LOAD_H, (ActorReload == 0) ? COL_GREEN : COL_RED);
	DrawRect(LOAD_X+load, LOAD_Y, LOAD_W-load, LOAD_H, COL_BLACK);
}

// display time
void DispTime()
{
	// rescale
	int frame = (CurrentFrame*TIME_W + MaxFrame/2) / MaxFrame;
	if (frame > TIME_W) frame = TIME_W;

	// draw
	DrawRect(TIME_X, TIME_Y, frame, TIME_H, COL_GREEN);
	DrawRect(TIME_X+frame, TIME_Y, TIME_W-frame, TIME_H, COL_BLACK);
}

// display foot
void DispFoot()
{
	// background
	DrawRect(1, FOOT_Y+1, WIDTH-2, FOOT_HEIGHT-2, HEADER_COL);

	// frame
	DrawFrame(0, FOOT_Y, WIDTH, FOOT_HEIGHT, HEADER_FRAMECOL);
	
	// title LIFE
	DrawText2("LIFE", 4, FOOT_Y+4, COL_YELLOW);

	// title LOAD
	DrawText2("LOAD", LOAD_X-5*16, FOOT_Y+4, COL_YELLOW);

	// title TIME
	DrawText2("TIME", TIME_X-5*16, FOOT_Y+4, COL_YELLOW);

	// life frame
	DrawFrame(LIFE_X-1, LIFE_Y-1, LIFE_W+2, LIFE_H+2, COL_WHITE);

	// load frame
	DrawFrame(LOAD_X-1, LOAD_Y-1, LOAD_W+2, LOAD_H+2, COL_WHITE);

	// time frame
	DrawFrame(TIME_X-1, TIME_Y-1, TIME_W+2, TIME_H+2, COL_WHITE);

	// display life
	DispLife();

	// display load weapon
	DispReload();

	// display time
	DispTime();
}
