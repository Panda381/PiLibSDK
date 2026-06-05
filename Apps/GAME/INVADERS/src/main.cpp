
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

// images
#include "../img/intro.cpp"	// const u8 IntroImg[] 320*240
#include "../img/sprites.cpp"	// const u8 SpritesImg[] 36*54

// sounds
#include "../snd/crash.cpp"	// const u8 CrashSnd[]
#include "../snd/hit.cpp"	// const u8 HitSnd[]
#include "../snd/introsnd.cpp"	// const u8 IntroSnd[]
#include "../snd/mother.cpp"	// const u8 MotherSnd[]
#include "../snd/shoot.cpp"	// const u8 ShootSnd[]
#include "../snd/tone1.cpp"	// const u8 Tone1Snd[]
#include "../snd/tone2.cpp"	// const u8 Tone2Snd[]
#include "../snd/tone3.cpp"	// const u8 Tone3Snd[]
#include "../snd/tone4.cpp"	// const u8 Tone4Snd[]

#define SPRITESIMGW	36	// width of image with sprites

// === aliens

#define ALIENW		12	// alien width
#define ALIENH		8	// alien height

#define ALIENNUMX	11	// number of aliens in X direction
#define ALIENNUMY	5	// number of aliens in Y direction
#define ALIENNUM	(ALIENNUMX*ALIENNUMY) // number of aliens (= 55)
#define ALIENSPACEX	4	// space between aliens in X direction
#define ALIENDX		(ALIENW+ALIENSPACEX) // alien delta X coordinate (= 16)
#define ALIENTOTW	(ALIENNUMX*ALIENDX - ALIENSPACEX) // alien total initial widh (= 172)
#define ALIENTOTX	(((WIDTH - ALIENTOTW)/2) & ~3) // alien block initial X coordinate, rounded down to 4 pixels (= 72)

#define ALIENSPACEY	8	// space between aliens in Y direction
#define ALIENDY		(ALIENH+ALIENSPACEY) // alien delta Y coordinate (= 16)
#define ALIENTOTH	(ALIENNUMY*ALIENDY - ALIENSPACEY) // alien total initial height (= 72)
#define ALIENTOTY	40	// alien block initial Y coordinate, must be multiply of 4
#define ALIENMAXY	112	// alien block initial max. Y coordinate, must be multiply of 4
#define ALIENMINX	0	// alien min. X coordinate
#define ALIENMAXX	WIDTH	// alien max. X coordinate

// === enemy missiles

#define MISSILENUM	10	// max. number of missiles
#define MISSILEW	4	// enemy missile width
#define MISSILEH	8	// enemy missile height
#define MISSILE1IMGX	24	// enemy missile 1 image X coordinate
#define MISSILE1IMGY	32	// enemy missile 1 image Y coordinate
#define MISSILE2IMGX	24	// enemy missile 2 image X coordinate
#define MISSILE2IMGY	40	// enemy missile 2 image Y coordinate

#define MISSILEEXPW	8	// enemy missile explosion width
#define MISSILEEXPH	8	// enemy missile explosion height
#define MISSILEEXPIMGX	28	// enemy missile explosion image X coordinate
#define MISSILEEXPIMGY	32	// enemy missile explosion image Y coordinate
#define MISSILEINVIMGX	28	// enemy missile inverted explosion image X coordinate
#define MISSILEINVIMGY	40	// enemy missile inverted explosion image Y coordinate

// === spaceship

#define SHIPW		12	// ship width
#define SHIPH		8	// ship height
#define SHIPIMGX	0	// ship image X coordinate in sprites
#define SHIPIMGY	24	// ship image Y coordinate in sprites
#define SHIPEXPIMGX	24	// ship explosion image X coordinate
#define SHIPEXPIMGY	24	// ship explosion image Y coordinate
#define SHIPY		(HEIGHT-16-8-SHIPH) // ship Y coordinate (= 208, must be multiply of 4)
#define SHIPX		(((WIDTH - SHIPW)/2) & ~3) // ship initial X coordinate (= 152, must be multiply of 4)
#define SHIPDX		16	// delta X of ship lives

// === spaceship shoots

#define SHOOTW		4	// shoot width
#define SHOOTH		8	// shoot height
#define SHOOTIMGX	12	// shoot image X coordinate
#define SHOOTIMGY	24	// shoot image Y coordinate

#define SHOOTEXPW	4	// shoot explosion width
#define SHOOTEXPH	8	// shoot explosion height
#define SHOOTEXPX	16	// shoot explosion image X coordinate
#define SHOOTEXPY	24	// shoot explosion image Y coordinate

#define SHOOTEXP2X	20	// shoot explosion inverted image X coordinate
#define SHOOTEXP2Y	24	// shoot explosion inverted image Y coordinate

// === score

#define LINEY1		20 // line 1 Y coordinate
#define LINEY2		(HEIGHT-20) // line 2 Y coordinate
#define SCOREY		0	// score Y coordinate
#define SCOREX		0	// score X coordinate
#define MAXSCOREX	(WIDTH-15*8)	// max. score X coordinate
#define LEVELY		(HEIGHT-16) // level Y coordinate
#define LEVELX		(WIDTH-9*8) // level X coordinate

// === house

#define HOUSEW		24	// house width
#define HOUSEH		16	// house height
#define HOUSEIMGX	0	// house image X coordinate in sprites
#define HOUSEIMGY	32	// house image Y coordinate in sprites
#define HOUSEY		(SHIPY-8-HOUSEH) // house Y coordinate (= 184, must be multiply of 4)

#define HOUSENUM	4	// number of houses
#define HOUSEDX		(3*ALIENDX) // house delta X coordinate (= 48)
#define HOUSESPACEX	(HOUSEDX - HOUSEW) // space between houses (= 24)
#define HOUSETOTW	(HOUSENUM*HOUSEDX - HOUSESPACEX) // house total width (= 168)
#define HOUSEX		(((WIDTH-HOUSETOTW)/2) & ~3) // house X coordinate (= 76, must be multiply of 4)

//#define HOUSECOL	COLOR(255, 128, 0) // house color

// === mothership

#define MOTHERY		24	// alien mothership Y coordinate
#define MOTHERW		16	// alien mothership width
#define MOTHERH		8	// alien mothership height
#define MOTHERIMGX	0	// alien mothership image X coordinate in sprites
#define MOTHERIMGY	48	// alien mothership image Y coordinate in sprites

#define MOTHEREXPIMGX	16	// alien mothership explosion image X coordinate in sprites
#define MOTHEREXPIMGY	48	// alien mothership explosion image Y coordinate in sprites

#define NEXTMOTHER	500	// number of steps to generate new mothership
#define MOTHERCHAN	2	// mothership sound channel

u32 OldTime;

// spaceship
int Lives; // number of remaining lives
int ShipX; // spaceship X coordinate, multiply of 4
int Score; // current score
int MaxScore = 0; // max. score
int Level; // current level
u32 ColHouse; // house color
int NextLive; // score to next live

// aliens
Bool AlienLive[ALIENNUM]; // alien lives
int AlienNum; // number of aliens
int AlienX;	// X coordinate of alien block, multiply of 4
int AlienY;	// Y coordinate of alien block, multiply of 4
int AlienPhase; // alien animation phase (0 or 1)
const u8 AlienType[ALIENNUMY] = { 0, 1, 1, 2, 2 }; // types of aliens
const u8 AlienScore[ALIENNUMY] = { 30, 20, 20, 10, 10 }; // score of aliens
int AlienMinX;	// alien min. X coordinate
int AlienMaxX;	// alien max. X coordinate
Bool AlienLeft; // aliens are moving left
int AlienTimer; // alien timer to next step
int AlienTone; // current alien tone

// alien missiles
Bool MissileOn[MISSILENUM]; // missile is active
int MissileX[MISSILENUM]; // missile X coordinate
int MissileY[MISSILENUM]; // missile Y coordinate
Bool EnemyLast[ALIENNUMX]; // last enemy
int MissilePhase; // missile animation phase

// spaceship shoot
int ShootX, ShootY; // shoot coordinate
Bool ShootOn; // shoot is ON

// mothership
Bool MotherOn; // mothership is on
Bool MotherLeft; // moving left
int MotherX; // mothership X coordinate
int NextMother; // counter to next mothership

// tones
const u8* const Tones[4] = { Tone1Snd, Tone2Snd, Tone3Snd, Tone4Snd, };
const int TonesLen[4] = { sizeof(Tone1Snd), sizeof(Tone2Snd), sizeof(Tone3Snd), sizeof(Tone4Snd), };

// display sprite image
void DispSprite(int xs, int ys, int x, int y, int w, int h)
{
	DrawImg(SpritesImg, x, y, xs, ys, w, h);
}

// display ship
void DispShip()
{
	DispSprite(SHIPIMGX, SHIPIMGY, ShipX, SHIPY, SHIPW, SHIPH);
}

// clear ship
void ClearShip()
{
	DrawRect(ShipX, SHIPY, SHIPW, SHIPH, COL_BLACK);
}

// display remaining lives
void DispLives()
{
	int i;
	DrawRect(0, HEIGHT-SHIPH-4, 10*SHIPDX, SHIPH, COL_BLACK);
	i = Lives;
	if (i > 10) i = 10;
	for (i--; i >= 0; i--)
		DispSprite(SHIPIMGX, SHIPIMGY, i*SHIPDX, HEIGHT-SHIPH-4, SHIPW, SHIPH);
}

// lost live
void Lost()
{
	DispSprite(SHIPEXPIMGX, SHIPEXPIMGY, ShipX, SHIPY, SHIPW, SHIPH);
	PlaySound(CrashSnd);
	DispUpdate();
	WaitMs(1000);
	DrawRect(ShipX, SHIPY, SHIPW, SHIPH, COL_BLACK);
	Lives--;
	ShipX = SHIPX;
	DispShip();
	DispLives();
	DispUpdate();
}

// mothership move
void MoveMother()
{
	// generate new mothership
	NextMother--;
	if ((NextMother <= 0) && !MotherOn)
	{
		NextMother = NEXTMOTHER;
		MotherOn = True;
		MotherLeft = !MotherLeft;
		MotherX = MotherLeft ? WIDTH : -MOTHERW;
		PlaySoundRep(MotherSnd, MOTHERCHAN);
	}

	// move mothership
	if (MotherOn)
	{
		DrawRect(MotherX, MOTHERY, MOTHERW, MOTHERH, COL_BLACK);
		if (MotherLeft)
		{
			MotherX -= 2;
			if (MotherX < -MOTHERW)
			{
				MotherOn = False;
				StopSound(MOTHERCHAN);
				return;
			}
		}
		else
		{
			MotherX += 2;
			if (MotherX >= WIDTH)
			{
				MotherOn = False;
				StopSound(MOTHERCHAN);
				return;
			}
		}
		DispSprite(MOTHERIMGX, MOTHERIMGY, MotherX, MOTHERY, MOTHERW, MOTHERH);
		DispUpdate();
	}
}

// display bottom line
void DispLine()
{
	DrawRect(0, LINEY1, WIDTH, 1, COL_WHITE);
	DrawRect(0, LINEY2, WIDTH, 1, COL_WHITE);
}

// prepare alien limits
void LimitAlien()
{
	int x, y, xmin, xmax, i;
	xmin = WIDTH;
	xmax = 0;
	for (i = 0; i < ALIENNUM; i++)
	{
		if (AlienLive[i])
		{
			y = i / ALIENNUMX;
			x = i - y*ALIENNUMX;
			y = y*ALIENDY;
			x = x*ALIENDX;
			if (x < xmin) xmin = x;
			if (x > xmax) xmax = x;	
		}
	}
	AlienMinX = ALIENMINX - xmin;
	AlienMaxX = ALIENMAXX - ALIENDX - xmax + ALIENSPACEX;
}

// clear one alien
void ClearAlien(int inx)
{
	int x, y;
	y = inx / ALIENNUMX;
	x = inx - y*ALIENNUMX;
	y = AlienY + y*ALIENDY;
	x = AlienX + x*ALIENDX;
	DrawRect(x, y, ALIENW, ALIENH, COL_BLACK);
}

// clear aliens
void ClearAliens()
{
	int i;
	for (i = 0; i < ALIENNUM; i++) if (AlienLive[i]) ClearAlien(i);
}

// display one alien
void DispAlien(int inx)
{
	int x, y, ys;

	y = inx / ALIENNUMX;
	x = inx - y*ALIENNUMX;
	ys = AlienType[y]*ALIENH;
	y = AlienY + y*ALIENDY;
	x = AlienX + x*ALIENDX;

	if (AlienLive[inx])
		DispSprite(AlienPhase*ALIENW, ys, x, y, ALIENW, ALIENH);
	else
		DispSprite(2*ALIENW, ys, x, y, ALIENW, ALIENH);

	// end game
	if (y >= SHIPY)
	{
		// lost live
		Lost();
		ClearAliens();
		AlienY -= 4*10;
	}
}

// display aliens
void DispAliens()
{
	int i;
	for (i = 0; i < ALIENNUM; i++) if (AlienLive[i]) DispAlien(i);
}

// check house collision (returns number of house pixels in area 8x4)
int CheckHouse(int x, int y)
{
	// check base Y cooordinate
	if ((y + 8 <= HOUSEY) || (y >= HOUSEY + HOUSEH)) return 0;

	// check base X coordinate
	if ((x + 4 <= HOUSEX) || (x >= HOUSEX + HOUSETOTW)) return 0;

	// check closer X coordinate
	int i = (x - HOUSEX) % HOUSEDX;
	if (i >= HOUSEW) return 0;

	// count pixels of house
	int j, num;
	u32* s = &FrameBuffer.drawbuf[x + y*WIDTH];
	num = 0;
	for (i = 8; i > 0; i--)
	{
		for (j = 4; j > 0; j--)
		{
			if (*s == ColHouse) num++;
			s++;
		}
		s += WIDTH - 4;
	}
	return num;
}

// shoot hit house
void ShootHitHouse()
{
	if (ShootOn)
	{
		if (CheckHouse(ShootX, ShootY) > 15)
		{
			ShootOn = False;
			DrawImg(SpritesImg, ShootX, ShootY, SHOOTEXP2X, SHOOTEXP2Y, SHOOTW, SHOOTH);
		}
	}
}

// missile hit house
void MissileHitHouse()
{
	int i;
	for (i = 0; i < MISSILENUM; i++)
	{
		if (MissileOn[i])
		{
			if (CheckHouse(MissileX[i], MissileY[i]) > 10)
			{
				MissileOn[i] = False;
				DrawImg(SpritesImg, MissileX[i]-2, MissileY[i], MISSILEINVIMGX, MISSILEINVIMGY, MISSILEEXPW, MISSILEEXPH);
			}
		}
	}
}

// display score
void DispScore()
{
	char buf[6];
	int n = Score;
	int i, x;
	u8 ch;
	for (i = 4; i >= 0; i--)
	{
		x = n/10;
		buf[i] = (n - x*10) + '0';
		n = x;
	}
	buf[5] = 0;
	DrawTextBg(buf, SCOREX+7*8, SCOREY, COL_WHITE, COL_BLACK);

	// next live
	if (Score >= NextLive)
	{
		Lives++;
		DispLives();
		NextLive *= 2;
	}
}

// display max. score
void DispMaxScore()
{
	char buf[6];
	int n = MaxScore;
	int i, x;
	u8 ch;
	for (i = 4; i >= 0; i--)
	{
		x = n/10;
		buf[i] = (n - x*10) + '0';
		n = x;
	}
	buf[5] = 0;
	DrawTextBg(buf, MAXSCOREX+10*8, SCOREY, ((MaxScore > 0) && (MaxScore == Score)) ? COL_GREEN : COL_WHITE, COL_BLACK);
}

// display level
void DispLevel()
{
	char buf[3];
	int n = Level+1;
	if (n > 99) n = 99;
	int i, x;
	u8 ch;
	for (i = 1; i >= 0; i--)
	{
		x = n/10;
		buf[i] = (n - x*10) + '0';
		n = x;
	}
	buf[2] = 0;
	DrawTextBg(buf, LEVELX+7*8, LEVELY, COL_WHITE, COL_BLACK);
}

// generate missiles
void GenMissile()
{
	int i, j, x, y;
	for (x = 0; x < ALIENNUMX; x++) EnemyLast[x] = True;

	i = ALIENNUM-1;
	for (y = ALIENNUMY-1; y >= 0; y--)
	{
		for (x = ALIENNUMX-1; x >= 0; x--)
		{
			if (AlienLive[i] && EnemyLast[x])
			{
				EnemyLast[x] = False;
				if ((RandU16() < 300) && (AlienY + y*ALIENDY + 8 < SHIPY))
				{
					for (j = 0; j < MISSILENUM; j++)
					{
						if (!MissileOn[j])
						{
							MissileOn[j] = True;
							MissileX[j] = AlienX + x*ALIENDX + 4;
							MissileY[j] = AlienY + y*ALIENDY + 8;
							break;
						}
					}
				}
			}

			i--;
		}
	}
}

// move missiles
void MoveMissile()
{
	int i;
	for (i = 0; i < MISSILENUM; i++)
	{
		if (MissileOn[i])
		{
			MissileY[i] += 4;
			if (MissileY[i] > SHIPY) MissileOn[i] = False;
		}
	}
}

// clear missiles
void ClearMissile()
{
	int i;
	for (i = 0; i < MISSILENUM; i++)
	{
		if (MissileOn[i])
		{
			DrawRect(MissileX[i], MissileY[i], MISSILEW, MISSILEH, COL_BLACK);
		}
	}
}

// display missiles
void DispMissile()
{
	int i;
	for (i = 0; i < MISSILENUM; i++)
	{
		if (MissileOn[i])
		{
			if (MissilePhase)
				DispSprite(MISSILE2IMGX, MISSILE2IMGY, MissileX[i], MissileY[i], MISSILEW, MISSILEH);
			else
				DispSprite(MISSILE1IMGX, MISSILE1IMGY, MissileX[i], MissileY[i], MISSILEW, MISSILEH);
		}
	}
	MissilePhase = (MissilePhase + 1) & 1;
}

// new game
void NewGame(int level)
{
	int i;

	// clear screen
	DrawClear();

	// draw houses
	for (i = 0; i < HOUSENUM; i++)
		DispSprite(HOUSEIMGX, HOUSEIMGY, HOUSEX + i*HOUSEDX, HOUSEY, HOUSEW, HOUSEH);
	ColHouse = FrameBuffer.drawbuf[HOUSEX + HOUSEW/2 + HOUSEY*WIDTH];

	// clear missiles
	for (i = 0; i < MISSILENUM; i++) MissileOn[i] = False;

	// display ship
	ShipX = SHIPX;
	ShootOn = False;
	DispShip();

	// display bottom line
	DispLine();

	// display remaining lives
	if (level == 0)
	{
		Lives = 3;
		NextLive = 1500; // score to next live
		Score = 0;
	}
	DispLives();

	// aliens
	AlienX = ALIENTOTX;
	AlienY = ALIENTOTY + level*8;
	if (AlienY > ALIENMAXY) AlienY = ALIENMAXY;
	for (i = 0; i < ALIENNUM; i++) AlienLive[i] = True;
	AlienNum = ALIENNUM;
	AlienPhase = 0;
	AlienLeft = True;
	LimitAlien();
	DispAliens();
	AlienTimer = 1;
	AlienTone = 0;
	MissilePhase = 0;

	// display score
	NextMother = NEXTMOTHER; // score of next mothership
	MotherOn = False;
	MotherLeft = True;
	Level = level;
	SelFont8x16();
	DrawText("SCORE:", SCOREX, SCOREY, COL_WHITE);
	DispScore();

	// display max. score
	DrawText("HI-SCORE:", MAXSCOREX, SCOREY, COL_WHITE);
	DispMaxScore();

	// display level
	DrawText("LEVEL:", LEVELX, LEVELY, COL_WHITE);
	DispLevel();

	// update display
	DispUpdate();

	// wait for a key
	WaitMs(500);
	KeyFlush();
	while (True)
	{
		DrawTextBg(" START ", (WIDTH-7*8)/2, 0, COL_RED, COL_WHITE);
		DispUpdate();
		WaitMs(250);
		i = KeyGet();
		if ((i == KEY_A) || (i == KEY_B)) break;

		DrawRect((WIDTH-7*8)/2, 0, 7*8, 16, COL_BLACK);
		DispUpdate();
		WaitMs(250);
		i = KeyGet();
		if ((i == KEY_A) || (i == KEY_B)) break;
	}
	DrawRect((WIDTH-7*8)/2, 0, 7*8, 16, COL_BLACK);
	DispUpdate();

	OldTime = Time();
}

// display shoot
void DispShoot()
{
	DispSprite(SHOOTIMGX, SHOOTIMGY, ShootX, ShootY, SHOOTW, SHOOTH);
}

// clear shoot collision
void ClearShoot()
{
	DrawRect(ShootX, ShootY, SHOOTW, SHOOTH, COL_BLACK);
}

// shoot collision service
void ShootCollision()
{
	int i, x, y, k;
	if (!ShootOn) return;

	// shoot enemy
	if ((ShootX >= AlienX) && (ShootY >= AlienY) &&
		(ShootX < AlienX + ALIENTOTW) && (ShootY < AlienY + ALIENTOTH))
	{
		x = (ShootX - AlienX)/ALIENDX;
		y = (ShootY - AlienY)/ALIENDY;
		k = y;
		i = x + y*ALIENNUMX;
		x = AlienX + x*ALIENDX - ShootX;
		y = AlienY + y*ALIENDY - ShootY;
		if (AlienLive[i] && (x < ALIENW) && (y < ALIENH))
		{
			AlienLive[i] = False;
			AlienNum--;
			ShootOn = False;
			ClearShoot();
			PlaySound(HitSnd);
			DispAlien(i);
			Score += AlienScore[k];
			DispScore();
			if (Score > MaxScore)
			{
				MaxScore = Score;
				DispMaxScore();
			}			
			DispUpdate();
			WaitMs(100);
			ClearAlien(i);
			DispUpdate();
			LimitAlien();
			return;
		}
	}

	// missile collision
	for (i = 0; i < MISSILENUM; i++)
	{
		if (MissileOn[i] && (MissileX[i] < ShootX+SHOOTW) && (MissileX[i] + MISSILEW >= ShootX) &&
			(MissileY[i] < ShootY+2*SHOOTH) && (MissileY[i] + MISSILEH >= ShootY))
		{
			MissileOn[i] = False;
			ClearShoot();
			DrawRect(MissileX[i], MissileY[i], MISSILEW, MISSILEH, COL_BLACK);
			ShootOn = False;
			DispSprite(SHOOTEXPX, SHOOTEXPY, ShootX, ShootY, SHOOTEXPW, SHOOTEXPH);
			DispUpdate();
			WaitMs(150);
			ClearShoot();
			DispUpdate();
		}
	}
	if (!ShootOn) return;

	// shoot mothership
	if (MotherOn && (ShootY < MOTHERY+MOTHERH) && (ShootX+4 >= MotherX) && (ShootX < MotherX+MOTHERW))
	{
		MotherOn = False;
		ShootOn = False;
		ClearShoot();
		DispSprite(MOTHEREXPIMGX, MOTHEREXPIMGY, MotherX, MOTHERY, MOTHERW, MOTHERH);
		StopSound(MOTHERCHAN);
		PlaySound(HitSnd);
		Score += 1000;
		DispScore();
		if (Score > MaxScore)
		{
			MaxScore = Score;
			DispMaxScore();
		}			
		DispUpdate();
		WaitMs(100);
		DrawRect(MotherX, MOTHERY, MOTHERW, MOTHERH, COL_BLACK);
		DispUpdate();
		return;
	}
}

// move shoot
void MoveShoot()
{
	// move shoot
	if (ShootOn)
	{
		ClearShoot();
		ShootY -= 8;
		if (ShootY < MOTHERY)
			ShootOn = False;
		else
		{
			ShootHitHouse();
			if (ShootOn)
			{
				DispShoot();

				// shoot collision service
				ShootCollision();
			}
		}
		DispUpdate();
	}
}

// spaceship control
void SpaceCtrl()
{
	// move ship left
	if (KeyPressed(KEY_LEFT) && (ShipX >= 4))
	{
		ClearShip();
		ShipX -= 4;
		DispShip();
		DispUpdate();
	}

	// move ship right
	if (KeyPressed(KEY_RIGHT) && (ShipX < WIDTH-ALIENW))
	{
		ClearShip();
		ShipX += 4;
		DispShip();
		DispUpdate();
	}

	// shooting
	if (KeyPressed(KEY_A) && !ShootOn)
	{
		ShootOn = True;
		ShootX = ShipX + 4;
		ShootY = SHIPY - 8;
		PlaySound(ShootSnd);
		DispShoot();
		DispUpdate();
	}
}

// move aliens
void AlienMove()
{
	// move aliens
	AlienTimer--;
	if (AlienTimer <= 0)
	{
		// clear aliens
		ClearAliens();

		// shift aliens
		if (AlienLeft)
		{
			AlienX -= 8;
			if (AlienX < AlienMinX)
			{
				AlienX += 16;
				AlienLeft = False;
				AlienY += 8;
			}
		}
		else
		{
			AlienX += 8;
			if (AlienX > AlienMaxX)
			{
				AlienX -= 16;
				AlienLeft = True;
				AlienY += 8;
			}
		}

		DispAliens();	// display aliens
		DispUpdate();
		AlienTimer = AlienNum/4+2;
		AlienPhase = (AlienPhase+1) & 1;

		AlienTone = (AlienTone+1) & 3;
		PlaySound(Tones[AlienTone], 1);
	}
}

// crash collision service
void Crash()
{
	int i;
	for (i = 0; i < MISSILENUM; i++)
	{
		if (MissileOn[i])
		{
			// spaceship collision
			if ((MissileX[i] < ShipX+SHIPW) && (MissileX[i] + MISSILEW >= ShipX) &&
				(MissileY[i] < SHIPY+SHIPH) && (MissileY[i] + MISSILEH >= SHIPY))
			{
				MissileOn[i] = False;
				DrawRect(MissileX[i], MissileY[i], MISSILEW, MISSILEH, COL_BLACK);

				// lost live
				Lost();
			}
		}
	}
}

// game loop
void Game()
{
	u32 t;
	int x, y, i;

	while (True)
	{
		// move shoot
		MoveShoot();

		// spaceship control
		SpaceCtrl();

		// move aliens
		AlienMove();

		// serve missiles
		ClearMissile();
		MoveMissile();
		GenMissile();
		MissileHitHouse();
		DispMissile();
		DispUpdate();

		// mothership move
		MoveMother();

		// shoot collision service
		ShootCollision();

		// crash collision service
		Crash();

		// time step
		do t = Time(); while ((u32)(t - OldTime) < (u32)50000);
		OldTime = t;

		switch (KeyGet())
		{
		// reset to boot loader
		case KEY_Y:
			Reboot();
			break;

		case KEY_SCREENSHOT:
			ScreenShot();
			break;
		}

		// new level
		if (AlienNum == 0)
		{
			StopAllSound();
			DrawTextBg(" LEVEL UP ", (WIDTH-10*8)/2, (HEIGHT-16)/2, COL_GREEN, COL_BLACK);
			DispUpdate();
			WaitMs(1000);
			KeyFlush();
			NewGame(Level+1);
		}

		// end game
		if (Lives < 0)
		{
			StopAllSound();
			DrawTextBg(" GAME OVER ", (WIDTH-11*8)/2, (HEIGHT-16)/2, COL_RED, COL_WHITE);
			DispUpdate();
			WaitMs(1000);
			KeyFlush();
			while (KeyGet() == NOKEY) {}
			return;
		}
	}
}

// main function
int main()
{
	// intro screen
	DrawImg(IntroImg);
	DispUpdate();

	// wait a key (max. 8 seconds)
	PlaySoundRep(IntroSnd);
	u32 t1 = Time();
	WaitMs(200);
	KeyFlush();
	while ((u32)(Time() - t1) < 8000000)
	{
		int key = KeyGet();
		if (key == KEY_Y)
			Reboot();
		else if (key == KEY_SCREENSHOT)
			ScreenShot();
		else if ((key == KEY_B) || (key == KEY_A) || (key == KEY_X))
			break;
	}
	StopSound();
	DrawClear();
	DispUpdate();

	while (True)
	{
		// new game
		NewGame(0);

		// game loop
		Game();
	}
	return 0;
}
