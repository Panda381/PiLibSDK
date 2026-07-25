
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#include "../include.h"

#include "../img/bomb.cpp"	// const u8 BombImgPNG[] ... PNG
#include "../img/boss.cpp"	// const u8 BossImgPNG[] ... PNG
#include "../img/city1.cpp"	// const u8 City1ImgPNG[] ... PNG
#include "../img/city2.cpp"	// const u8 City2ImgPNG[] ... PNG
#include "../img/digits.cpp"	// const u8 DigitsImgPNG[] ... PNG
#include "../img/explosion.cpp"	// const u8 ExplosionImgPNG[] ... PNG
#include "../img/gateimg.cpp"	// const u8 GateImgPNG[] ... PNG
#include "../img/goalimg.cpp"	// const u8 GoalImgPNG[] ... PNG
#include "../img/grass1.cpp"	// const u8 Grass1ImgPNG[] ... PNG
#include "../img/grass2.cpp"	// const u8 Grass2ImgPNG[] ... PNG
#include "../img/grass3.cpp"	// const u8 Grass3ImgPNG[] ... PNG
#include "../img/grass4.cpp"	// const u8 Grass4ImgPNG[] ... PNG
#include "../img/intro.cpp"	// const u8 IntroImgPNG[] ... PNG
#include "../img/liveimg.cpp"	// const u8 LiveImgPNG[] ... PNG
#include "../img/shipimg.cpp"	// const u8 ShipImgPNG[] ... PNG
#include "../img/shotimg.cpp"	// const u8 ShotImgPNG[] ... PNG
#include "../img/skyline.cpp"	// const u8 SkylineImgPNG[] ... PNG
#include "../img/title.cpp"	// const u8 TitleImgPNG[] ... PNG
#include "../img/ufo.cpp"	// const u8 UfoImgPNG[] ... PNG
#include "../img/ufo2.cpp"	// const u8 Ufo2ImgPNG[] ... PNG

#include "../snd/entering.cpp"	// const u8 EnteringSnd[]
#include "../snd/explos.cpp"	// const u8 ExplosionSnd[]
#include "../snd/fuelsnd.cpp"	// const u8 FuelSnd[]
#include "../snd/gateok.cpp"	// const u8 GateSnd[]
#include "../snd/hit.cpp"	// const u8 HitSnd[]
#include "../snd/introsnd.cpp"	// const u8 IntroSnd[]
#include "../snd/laser.cpp"	// const u8 LaserSnd[]
#include "../snd/livessnd.cpp"	// const u8 LivesSnd[]

u8 *BombImg, *BossImg, *City1Img, *City2Img, *DigitsImg, *ExplosionImg;
u8 *GateImg, *GoalImg, *Grass1Img, *Grass2Img, *Grass3Img, *Grass4Img;
u8 *IntroImg, *LiveImg, *ShipImg, *ShotImg, *SkylineImg, *TitleImg;
u8 *UfoImg, *Ufo2Img;

// game variables
int	Level;		// current game level (0..)
int	SubLevel;	// current game sub-level (0..)

// delay with corrections
void Delay(int ms)
{
	int i;

	// delay
	WaitMs(ms);

	// corrections
	u32 t = ms*1000;
	LastStarTime += t;
	TimeLastDisk += t;
	TimeLastBig += t;
	ShipNewTime += t;
	ShipCrashTime += t;
	LastShotTime += t;
	GoalTime += t;

	sObj* o = Obj;
	for (i = OBJ_MAX; i > 0; i--)
	{
		o->time += t;
		o->starttime += t;
		o->nextbomb += t;
		o++;
	}
}

// initialize new game
void NewGame()
{
	ShootSpeed = 8;	// shoot speed

#ifdef DEBUG_LIVES	// number of start lives (default 3)
	Lives = DEBUG_LIVES;	// number of lives
#else
	Lives = 3;	// number of lives
#endif

#ifdef DEBUG_LEVEL	// start level 0..4 (default 0)
	Level = DEBUG_LEVEL; // current game level (0..)
#else
	Level = 0;	// current game level (0..)
#endif

#ifdef DEBUG_SUBLEVEL	// start sublevel 0..3 (default 0)
	SubLevel = DEBUG_SUBLEVEL; // current game sub-level (0..3)
	Fuel = (FUEL_MAX*3/4) << FRAC; // fuel to maximum
#else
	SubLevel = 0;	// current game sub-level (0..3)
#endif

	Score = 0;	// current score
	LastScore = 0;	// score at last life addition
	Speed = SHIP_SPEEDDEF; // ship speed
	LastShotTime = Time() - SHOTTIME; // time of last shoot
}

// entering sector
void Enter()
{
	// clear screen
	DrawClear();

	// draw text
	DrawText("ENTERING SECTOR", (WIDTH-15*8)/2, 100, COL_WHITE);

	// display sector number
	MemPrint(DecNumBuf, DECNUMBUF_SIZE, "%d.%d", Level+1, SubLevel);
	DrawText(DecNumBuf, (WIDTH-3*8)/2, 112, COL_WHITE);

#ifdef DEBUG_SKIP_ENTER	// skip entering screen
	return;
#endif

	// play sound
	PlaySound(EnteringSnd, 0, SNDREPEAT_NO, 1, 0.5f);

	// display update
	DispUpdate();

	// wait
	WaitMs(3600);

	// flush keyboard
	KeyFlush();
}

// display all game screen
void DispAll()
{
	// sector 3 is in the space
	if (SubLevel == 3)
	{
		// clear screen
		DrawClear();

		// draw stars
		DispStars();
	}
	else
	{
		// display horizon
		DrawImg(SkylineImg, 0, 0, 0, 0, SKYLINE_IMG_W, SKYLINE_IMG_H);

		// display city
		if (Level == 1)
			DrawImg(City1Img, (WIDTH - CITY1_IMG_W)/2, HORIZON - CITY1_IMG_H,
				0, 0, CITY1_IMG_W, CITY1_IMG_H);
		else if (Level >= 2)
			DrawImg(City2Img, (WIDTH - CITY2_IMG_W)/2, HORIZON - CITY2_IMG_H,
				0, 0, CITY2_IMG_W, CITY2_IMG_H);

		// display ground
		DispGround();
	}

	// display number of lives
	DispLives();

	// display fuel
	DispFuel();

	// display goals
	DispGoals();

	// display score
	DispScore();

	// display objects
	DispObj();

	// display gates
	DispGate();

	// display shots
	DispShots();

	// display ship
	DispShip();

	// display update
	DispUpdate();
}

// initialize new level
void InitLevel()
{
	// fuel indicator to maximum, if it is first sublevel
	if (SubLevel == 0) Fuel = FUEL_MAX << FRAC;

	// entering sector
	Enter();
	
	// initialize ship
	ShipX = SHIP_DEFX;
	ShipInCrash = 0;	// >1 ship is in crash
	ShipNewTime = Time(); // ship protection

	// initialize stars
	InitStars();

	// reset shots on start of new scene
	ResetShots();

	// reset gates on start of new scene
	ResetGates();

	// initialize list of objects
	InitObj();
	TimeLastDisk = Time() - 5000000; // time of last generated small disk
	TimeLastBig = Time();	// time of last generated big enemy
	TimeLastBomb = Time();	// time of last generated bomb
	BossIsGen = False;	// boss is generated
	BossLives = ENEMY_BOSS_LIVES;	// boss lives counter

	// initialize goals
#ifdef DEBUG_GOALS // number of goals
	Goals = DEBUG_GOALS;
#else
	Goals = GoalsInit[SubLevel];
#endif

	// display all game screen
	DispAll();
}

// one game
void Game()
{
	u32 t, dt;
	int dz;

	// initialize new game
	NewGame();

	// initialize new level
	InitLevel();

	// game loop
	t = Time();
	while (True)
	{
		// display all graphics
		DispAll();

		// generate stars (only sublevel 3)
		if (SubLevel == 3)
		{
			// shift stars
			ShiftStars();

			// generate new stars
			NewStars();
		}

		// delta time
		dt = Time() - t;
		if (dt < 0) dt = 0;
		t += dt;
		if (dt > 150000) dt = 150000; // limit to 150 ms
		if (dt < 20000)
		{
			WaitUs(20000-dt);
			dt = 20000; // min. time 20 ms
		}

		// delta Z coordinate, with FRAC fraction
		if (SubLevel == 3)
			dz = (dt*(SHIP_SPEEDDEF>>FRAC))>>(21-FRAC);
		else
			dz = (dt*(Speed>>FRAC))>>(21-FRAC);

		// update score by distance
		AddScore(dz);

		// subtract fuel
		SubFuel(dt>>13);

		// shift ground
		GrassPhase -= dz;
		while (GrassPhase < 0) GrassPhase += GROUND_DY<<FRAC;

		// serve objects
		DoObj();

		// check hit ship by enemy
		ShipEnemyCrash();

		// ship service
		DoShip(dt, dz);

		// shooting service
		Shooting(dt);

		// gate service
		Gating(dz);

		// level completed
		if ((Goals <= 0) && ((int)(Time() - GoalTime) > 1000000))
		{
			// increase sublevel
			SubLevel++;
			if (SubLevel > 3)
			{
				SubLevel = 0;
				Level++;
			}

			// initialize new level
			InitLevel();
		}

		// end game
		if ((ShipInCrash > SHIP_CRASH_WAIT+2) && (Lives == 0)) return;

		// key
		switch (KeyGet())
		{
		// quit game
		case KEY_PAD_Y:
			return;

		case KEY_SCREENSHOT:
			ScreenShot();
			break;
		}
	}
}

int main()
{
	int i;
	u8 ch;

	// decompress images
	BombImg = (u8*)PNGLOAD(BombImgPNG); if (BombImg == NULL) Reboot();
	BossImg = (u8*)PNGLOAD(BossImgPNG); if (BossImg == NULL) Reboot();
	City1Img = (u8*)PNGLOAD(City1ImgPNG); if (City1Img == NULL) Reboot();
	City2Img = (u8*)PNGLOAD(City2ImgPNG); if (City2Img == NULL) Reboot();
	DigitsImg = (u8*)PNGLOAD(DigitsImgPNG); if (DigitsImg == NULL) Reboot();
	ExplosionImg = (u8*)PNGLOAD(ExplosionImgPNG); if (ExplosionImg == NULL) Reboot();
	GateImg = (u8*)PNGLOAD(GateImgPNG); if (GateImg == NULL) Reboot();
	GoalImg = (u8*)PNGLOAD(GoalImgPNG); if (GoalImg == NULL) Reboot();
	Grass1Img = (u8*)PNGLOAD(Grass1ImgPNG); if (Grass1Img == NULL) Reboot();
	Grass2Img = (u8*)PNGLOAD(Grass2ImgPNG); if (Grass2Img == NULL) Reboot();
	Grass3Img = (u8*)PNGLOAD(Grass3ImgPNG); if (Grass3Img == NULL) Reboot();
	Grass4Img = (u8*)PNGLOAD(Grass4ImgPNG); if (Grass4Img == NULL) Reboot();
	IntroImg = (u8*)PNGLOAD(IntroImgPNG); if (IntroImg == NULL) Reboot();
	LiveImg = (u8*)PNGLOAD(LiveImgPNG); if (LiveImg == NULL) Reboot();
	ShipImg = (u8*)PNGLOAD(ShipImgPNG); if (ShipImg == NULL) Reboot();
	ShotImg = (u8*)PNGLOAD(ShotImgPNG); if (ShotImg == NULL) Reboot();
	SkylineImg = (u8*)PNGLOAD(SkylineImgPNG); if (SkylineImg == NULL) Reboot();
	TitleImg = (u8*)PNGLOAD(TitleImgPNG); if (TitleImg == NULL) Reboot();
	UfoImg = (u8*)PNGLOAD(UfoImgPNG); if (UfoImg == NULL) Reboot();
	Ufo2Img = (u8*)PNGLOAD(Ufo2ImgPNG); if (Ufo2Img == NULL) Reboot();

	GrassImgList[0] = Grass1Img;
	GrassImgList[1] = Grass2Img;
	GrassImgList[2] = Grass3Img;
	GrassImgList[3] = Grass4Img;

	// intro screen
	PlaySound(IntroSnd);
	DrawImg(IntroImg);
	DispUpdate();

	// calculate perspective projection table
	CalcPerspTab();

	// wait a key (max. 8 seconds)
	u32 t1 = Time();
	WaitMs(200);
	KeyFlush();
	while ((u32)(Time() - t1) < 8000000)
	{
		ch = KeyGet();

		// exit
		if (ch == KEY_PAD_Y)
			Reboot();

		// screenshot
		else if (ch == KEY_SCREENSHOT)
			ScreenShot();

		else if ((ch == KEY_PAD_B) || (ch == KEY_PAD_A) || (ch == KEY_PAD_X))
			break;
	}
	StopAllSound();

	// clear screen
	DrawClear();
	DispUpdate();

	// main loop
	while (True)
	{
		// open menu (returns False to quit the game)
		if (!Open()) Reboot();

		// play game
		Game();
	}
}
