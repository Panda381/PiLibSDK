
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

//#define AUTOSHOOT	1	// DEBUG: 1=autoshoot

// images
#include "../img/intro.cpp"		// const u8 IntroImgJPG[] 640x480
#include "../img/landscape.cpp"		// const u8 LandscapeImgJPG[] 640x640
#include "../img/shadows.cpp"		// const u8 ShadowsImg[] 32x192 (1x6 tiles, 1 tile 32x32)
#include "../img/sprites.cpp"		// const u8 SpritesImgPNG[] 64x3456 (1x54 tiles, 1 tile 64x64)
#include "../img/tiles.cpp"		// const u8 TilesImgPNG[] 32x416 (1x13 tiles, 1 tile 32x32)

// sounds
#include "../snd/artillery.cpp"		// const u8 ArtillerySnd[]
#include "../snd/blackhole.cpp"		// const u8 BlackholeSnd[]
#include "../snd/blackhole2.cpp"	// const u8 Blackhole2Snd[]
#include "../snd/explosion.cpp"		// const u8 ExplosionSnd[]
#include "../snd/fail.cpp"		// const u8 FailSnd[]
#include "../snd/greenrocket.cpp"	// const u8 GreenrocketSnd[]
#include "../snd/laser.cpp"		// const u8 LaserSnd[]
#include "../snd/levelup.cpp"		// const u8 LevelupSnd[]
#include "../snd/lost.cpp"		// const u8 LostSnd[]
#include "../snd/machinegun.cpp"	// const u8 MachinegunSnd[]
#include "../snd/silverrocket.cpp"	// const u8 SilverrocketSnd[]
#include "../snd/sonar.cpp"		// const u8 SonarSnd[]
#include "../snd/startup.cpp"		// const u8 StartupSnd[]
#include "../snd/Raptor.cpp"		// const u8 RaptorSnd[] ... MP3

// speed multiply (if using slow display)
int SpeedMul = 1;

#define SHIELD_MAX	7
#define ENERGY_MAX	30
#define SHIP_SPEED	10
#define WEAPON_NUM	7 	// number of weapons
#define ENEMY_NUM	5	// number of enemies

#define WEAPON_ARTILLERY 4	// weapon is artillery
#define ARTILLERY_NUM	15	// number of missiles of artillery
#define WEAPON_SONARBOMB 5	// weapon is sonar bomb
#define WEAPON_BLACKHOLE 6	// weapon is black hole

// sound channels
#define SNDCHAN_GUN	0	// sound channel: machine gun
#define SNDCHAN_WEAPON	1	// sound channel: weapon
#define SNDCHAN_MISC	2	// sound channel: misc
#define SNDCHAN_MUSIC	3	// sound channel: music

// flying missile
typedef struct {
	int type;	// missile type (-1 = entry not used, WEAPON_NUM = missile hit)
	float x,y;	// missile coordinates
	float dx, dy;	// missile delta
	int anim;	// missile animation
} sMissile;

// enemy
typedef struct {
	int type;	// enemy type (-1 = entry not used, ENEMY_NUM = enemy dead)
	float x, y;	// enemy coordinates
	float dx, dy;	// enemy delta
	int anim;	// enemy animation
	int energy;	// enemy energy
} sEnemy;

#define LANDSCAPEW	640
#define LANDSCAPEH	640
#define SHADOWW		64
#define SHADOWH		64
#define SHADOW_SHIFTX	60
#define SHADOW_SHIFTY	60

// shadow indices
#define SHADOW_SHIP	0	// spaceship
#define SHADOW_ENEMY1	1	// enemy 1
#define SHADOW_ENEMY2	2	// enemy 2
#define SHADOW_ENEMY3	3	// enemy 3
#define SHADOW_ENEMY4	4	// enemy 4
#define SHADOW_ENEMY5	5	// enemy 5

#define SPRITEW		64
#define SPRITEH		64

// sprite indices

// spaceship
#define SPRITE_SHIP	0	// spaceship standing and animating (2 phases)
#define SPRITE_SHIPD	0	// spaceship moving down
#define SPRITE_SHIPU	1	// spaceship moving up
#define SPRITE_SHIPR	2	// spaceship moving right
#define SPRITE_SHIPL	3	// spaceship moving left
// weapon 0 - machine gun
#define SPRITE_WEAPON0_TILE	4	// weapon 0 tile
#define SPRITE_WEAPON0_MISSILE	5	// weapon 0 missile
#define SPRITE_WEAPON0_NUM	1	// weapon 0 animation phases
// weapon 1 - green rocket
#define SPRITE_WEAPON1_TILE	6	// weapon 1 tile
#define SPRITE_WEAPON1_MISSILE	7	// weapon 1 missile
#define SPRITE_WEAPON1_NUM	1	// weapon 1 animation phases
// weapon 2 - silver rocket
#define SPRITE_WEAPON2_TILE	8	// weapon 2 tile
#define SPRITE_WEAPON2_MISSILE	9	// weapon 2 missile
#define SPRITE_WEAPON2_NUM	1	// weapon 2 animation phases
// weapon 3 - laser
#define SPRITE_WEAPON3_TILE	10	// weapon 3 tile
#define SPRITE_WEAPON3_MISSILE	11	// weapon 3 missile
#define SPRITE_WEAPON3_NUM	1	// weapon 3 animation phases
// weapon 4 - artillery
#define SPRITE_WEAPON4_TILE	12	// weapon 4 tile
#define SPRITE_WEAPON4_MISSILE	13	// weapon 4 missile
#define SPRITE_WEAPON4_NUM	1	// weapon 4 animation phases
// weapon 5 - sonar bomb
#define SPRITE_WEAPON5_TILE	14	// weapon 5 tile
#define SPRITE_WEAPON5_MISSILE	15	// weapon 5 missile
#define SPRITE_WEAPON5_NUM	8	// weapon 5 animation phases
// weapon 6 - black hole
#define SPRITE_WEAPON6_TILE	23	// weapon 6 tile
#define SPRITE_WEAPON6_MISSILE	24	// weapon 6 missile
#define SPRITE_WEAPON6_NUM	6	// weapon 6 animation phases
// explosion
#define SPRITE_EXPLOSE1		30	// explosion 1
#define SPRITE_EXPLOSE2		31	// explosion 2
#define SPRITE_EXPLOSE3		32	// explosion 3
#define SPRITE_EXPLOSE4		33	// explosion 4
#define SPRITE_EXPLOSE5		34	// explosion 5
#define SPRITE_EXPLOSE6		35	// explosion 6
#define SPRITE_EXPLOSE7		36	// explosion 7
#define SPRITE_EXPLOSE8		37	// explosion 8
// enemy 1
#define SPRITE_ENEMY1		38	// enemy 1
#define SPRITE_ENEMY1_MISSILE	39	// enemy 1 missile
// enemy 2
#define SPRITE_ENEMY2		40	// enemy 2
#define SPRITE_ENEMY2_MISSILE	41	// enemy 2 missile
// enemy 3
#define SPRITE_ENEMY3		42	// enemy 3
#define SPRITE_ENEMY3_MISSILE	43	// enemy 3 missile
// enemy 4
#define SPRITE_ENEMY4		44	// enemy 4
#define SPRITE_ENEMY4_MISSILE	45	// enemy 4 missile
// enemy 5
#define SPRITE_ENEMY5		46	// enemy 5
#define SPRITE_ENEMY5_MISSILE	47	// enemy 5 missile
#define SPRITE_ENEMY5B		48	// enemy 5
#define SPRITE_ENEMY5C		49	// enemy 5
// hit missile
#define SPRITE_HIT0		50	// hit 0
#define SPRITE_HIT1		51	// hit 1
#define SPRITE_HIT2		52	// hit 2
#define SPRITE_HIT3		53	// hit 3

#define TILEW	32
#define TILEH	32

#define TILE_ENERGY2	0	// energy, 2 points
#define TILE_ENERGY1	1	// energy, 1 point
#define TILE_SHIELD	2	// shield
#define TILE_DIGIT0	3	// digit 0
#define TILE_DIGIT1	4	// digit 1
#define TILE_DIGIT2	5	// digit 2
#define TILE_DIGIT3	6	// digit 3
#define TILE_DIGIT4	7	// digit 4
#define TILE_DIGIT5	8	// digit 5
#define TILE_DIGIT6	9	// digit 6
#define TILE_DIGIT7	10	// digit 7
#define TILE_DIGIT8	11	// digit 8
#define TILE_DIGIT9	12	// digit 9

// ============================================================================
//                                 Data
// ============================================================================

// images
u8 *IntroImg, *LandscapeImg, *SpritesImg, *TilesImg;

// MP3 player
Bool MP3PlayerWasInit = False;
sMP3Player MP3Player;
u8 ALIGNED MP3PlayerOutBuf[MP3PLAYER_OUTSIZE];

// move direction
enum { MOVE_NO, MOVE_L, MOVE_R, MOVE_U, MOVE_D };

// weapon reload time
const int ReloadList[WEAPON_NUM] = { 4, 12, 10, 6, 24, 24, 100, };

// weapon speed
const float SpeedList[WEAPON_NUM] = { 30, 15, 15, 30, 15, 5, 1.5f, };

// weapon energy
const int WeaponEnergy[WEAPON_NUM] = { 26, 60, 120, 330, 80, 10000, 10000, };

// enemy speed list
const float EnemySpeed[ENEMY_NUM] = { 2, 3, 4, 3, 2, };

// enemy energy
const int EnemyEnergyList[ENEMY_NUM] = { 125, 250, 500, 1000, 2000, };

// enemy points
const int EnemyPoints[ENEMY_NUM] = { 20, 50, 100, 200, 500, };

// enemy missile speed list
const float EnemyMissileSpeed[ENEMY_NUM] = { 4, 6, 8, 6, 5, };

// enemy missile energy
const int EnemyMissileEnergy[ENEMY_NUM] = { 3, 6, 12, 24, 40, };

// weapon sounds
const u8* const SoundList[WEAPON_NUM] = {
	MachinegunSnd,		// weapon 0 - machine gun
	GreenrocketSnd,		// weapon 1 - green rocket
	SilverrocketSnd,	// weapon 2 - silver rocket
	LaserSnd,		// weapon 3 - laser
	ArtillerySnd,		// weapon 4 - artillery
	SonarSnd,		// weapon 5 - sonar bomb
	BlackholeSnd,		// weapon 6 - black hole
};

// weapon volume
const float SoundVolList[WEAPON_NUM] = {
	1.8f,		// weapon 0 - machine gun
	0.5f,		// weapon 1 - green rocket
	0.8f,		// weapon 2 - silver rocket
	0.6f,		// weapon 3 - laser
	0.8f,		// weapon 4 - artillery
	0.9f,		// weapon 5 - sonar bomb
	1.8f,		// weapon 6 - black hole
};

int LandscapeY = 0;
u32 LastTime;
int Level;	// current level (1..)
int Weapon;	// current weapon (0=machine gun)
int WeaponMax;	// max. weapon (0=machine gun)
int Shields;	// number of shields (-1 = spaceship is dead)
int Energy;	// current energy
int Score;	// current score
int ShipX, ShipY; // spaceship current position
u8 Move = MOVE_NO; // spaceship moving state
int GameFrame;	// frame counter (to do animations)
int NextShield;	// score to get next shield
int ReloadNext0, ReloadNext; // reload time of weapon 0 (machine gun) and current weapon

// my flying missiles
#define MYMISSILE_MAX	100	// max. my missiles
sMissile	MyMissile[MYMISSILE_MAX];

// enemy flying missiles
#define ENMISSILE_MAX	100	// max. enemy missiles
sMissile	EnMissile[ENMISSILE_MAX];

// enemy list
#define ENEMY_MAX	100	// max. enemies
sEnemy		Enemy[ENEMY_MAX];

// weapon base index
const u8 WeaponList[WEAPON_NUM + 1] = {
	SPRITE_WEAPON0_TILE,
	SPRITE_WEAPON1_TILE,
	SPRITE_WEAPON2_TILE,
	SPRITE_WEAPON3_TILE,
	SPRITE_WEAPON4_TILE,
	SPRITE_WEAPON5_TILE,
	SPRITE_WEAPON6_TILE,
	SPRITE_HIT0-1, // hit
};

// ============================================================================
//                                 Music
// ============================================================================

// stop music
void MusicStop()
{
	sMP3Player* mp3 = &MP3Player;
	if (MP3PlayerWasInit)
	{
		MP3PlayerTerm(mp3);
		MP3PlayerWasInit = False;
	}
}

// poll music
void MusicPoll()
{
	sMP3Player* mp3 = &MP3Player;
	if (MP3PlayerWasInit)
	{
		MP3Poll(mp3);
	}
}

// play music
void MusicStart()
{
	MusicStop();
	sMP3Player* mp3 = &MP3Player;
	int r = MP3PlayerInit(mp3, NULL, RaptorSnd, sizeof(RaptorSnd), MP3PlayerOutBuf, MP3PLAYER_OUTSIZE, -1);
	if (r == ERR_MP3_OK)
	{
		MP3PlayerWasInit = True;
		MP3Play(mp3, SNDCHAN_MUSIC, True);
	}
}

// ============================================================================
//                               Initialize
// ============================================================================

// initialize new game
void NewGame()
{
	int i;
	LastTime = Time();

	// setup new game
	Level = 1; // current level
	Weapon = 0; // current weapon
	WeaponMax = 0; // max. weapon
	Shields = 3; // number of shields
	Energy = ENERGY_MAX; // energy
	Score = 0; // current score
	ShipX = (WIDTH - SPRITEW)/2; // spaceship X coordinate
	ShipY = HEIGHT - SPRITEH; // spaceship Y coordinate
	Move = 0; // spaceship moving mode
	GameFrame = 0; // frame counter
	NextShield = 10000; // score to get next shield
	ReloadNext0 = 0; // reload time of weapon 0 (machine gun)
	ReloadNext = 0; // reload time of current weapon

	// clear missile list
	for (i = 0; i < MYMISSILE_MAX; i++) MyMissile[i].type = -1;
	for (i = 0; i < ENMISSILE_MAX; i++) EnMissile[i].type = -1;

	// clear enemy list
	for (i = 0; i < ENEMY_MAX; i++) Enemy[i].type = -1;
}

// ============================================================================
//                               Controls
// ============================================================================

// move landscape
void DoLandscape()
{
	// shift landscape
	LandscapeY += SpeedMul;
	if (LandscapeY >= LANDSCAPEH) LandscapeY -= LANDSCAPEH;
}

// serve keys, select weapon (returns True to quit the game)
Bool DoKeys()
{
	int ch;

	while (True)
	{
		ch = KeyGet();
		if (ch == NOKEY) break;

		// quit the game
		if (ch == KEY_PAD_Y) return True;

		// screenshot
		if (ch == KEY_SCREENSHOT) ScreenShot();

		// LCD display rezoom
		if (ch == KEY_ZOOM) LCDRezoom();

		// select weapon
		if ((ch == KEY_PAD_B) && (WeaponMax > 0) && (Shields >= 0))
		{
			Weapon++;
			if (Weapon > WeaponMax) Weapon = 1;
			ReloadNext = 0;
		}

		// select weapon
		if ((ch == KEY_PAD_X) && (WeaponMax > 0) && (Shields >= 0))
		{
			Weapon--;
			if (Weapon < 1) Weapon = WeaponMax;
			ReloadNext = 0;
		}
	}
	return False;
}

// serve spaceship moving
void DoShip()
{
	int inx;
	if (Shields < 0) return; // dead

	// moving spaceship
	Move = MOVE_NO;
	if (KeyPressed(KEY_UP))
	{
		ShipY -= SHIP_SPEED*SpeedMul;
		if (ShipY < TILEH)
			ShipY = TILEH;
		else
			Move = MOVE_U;
	}

	if (KeyPressed(KEY_DOWN))
	{
		ShipY += SHIP_SPEED*SpeedMul;
		if (ShipY > HEIGHT-SPRITEH)
			ShipY = HEIGHT-SPRITEH;
		else
			Move = MOVE_D;
	}

	if (KeyPressed(KEY_LEFT))
	{
		ShipX -= SHIP_SPEED*SpeedMul;
		if (ShipX < TILEW)
			ShipX = TILEW;
		else
			Move = MOVE_L;
	}

	if (KeyPressed(KEY_RIGHT))
	{
		ShipX += SHIP_SPEED*SpeedMul;
		if (ShipX > WIDTH-TILEW-SPRITEW)
			ShipX = WIDTH-TILEW-SPRITEW;
		else
			Move = MOVE_R;
	}
}

// spaceship shooting
void DoShooting()
{
	int i, j, n;
	sMissile* m;

	if (Shields < 0) return; // dead

	// shoot next missile
	if (ReloadNext0 > 0) ReloadNext0 -= SpeedMul; // count machine gun
	if (ReloadNext > 0) ReloadNext -= SpeedMul; // count current weapon
#if AUTOSHOOT	// DEBUG: 1=autoshoot
	if (True)
#else
	if (KeyPressed(KEY_PAD_A))
#endif
	{
		// playing sound of machine gun
		if (!PlayingSound(SNDCHAN_GUN)) PlaySound(MachinegunSnd, SNDCHAN_GUN);

		// new missile of machine gun
		if (ReloadNext0 <= 0)
		{
			// search free entry
			m = MyMissile;
			for (i = 0; i < MYMISSILE_MAX; i++)
			{
				if (m->type < 0) // not used entry
				{
					m->type = 0;
					m->x = ShipX;
					m->y = ShipY;
					m->dx = 0;
					m->dy = SpeedList[0]*SpeedMul;
					m->anim = 0;
					ReloadNext0 = ReloadList[0];
					break;
				}
				m++;
			}
		}

		// new missile of current weapon
		if ((Weapon != 0) && (ReloadNext <= 0))
		{
			// search free entry
			m = MyMissile;
			n = (Weapon == WEAPON_ARTILLERY) ? ARTILLERY_NUM : 1; // artillery generates more missilies at once

			for (i = 0; (i < MYMISSILE_MAX) && (n > 0); i++)
			{
				if (m->type < 0) // not used entry
				{
					m->type = Weapon;
					m->x = ShipX;
					m->y = ShipY;
					m->dy = SpeedList[Weapon]*SpeedMul;
					m->dx = 0;
					m->anim = 0;

					// serve artillery
					if (Weapon == WEAPON_ARTILLERY)
					{
						float s = SpeedList[Weapon]*SpeedMul * RandFloatMinMax(0.9f, 1.1f); // random speed
						m->dx = 0.7f * RandFloatMinMax(-1.0f, +1.0f); // random direction
						m->dy = sqrtf(1 - m->dx*m->dx);
						m->dx *= s;
						m->dy *= s;
					}
					n--;
				}
				m++;
			}
			ReloadNext = ReloadList[Weapon];

			// playing sound of the missile
			PlaySound(SoundList[Weapon], SNDCHAN_WEAPON, SNDREPEAT_NO, 1, SoundVolList[Weapon]);
		}
	}
	else
	{
		// stop sound of machine gun
		StopSound(SNDCHAN_GUN);
	}
}

// move spaceship missile
void MoveMissile()
{
	int i, j, k, kbest;
	float dx, dy, a, distbest;
	sMissile *m, *m2;
	sEnemy* e;
	
	// move missiles
	m = MyMissile;
	for (i = 0; i < MYMISSILE_MAX; i++)
	{
		j = m->type;
		if ((j >= 0) && (j < WEAPON_NUM)) // active missile
		{
			// control sonar bomb
			if (j == WEAPON_SONARBOMB)
			{
				m->dx = 0;
				m->dy = 0;

				// find nearest enemy
				e = Enemy;
				kbest = -1;
				for (k = 0; k < ENEMY_MAX; k++)
				{
					if ((e->type >= 0) && (e->type < ENEMY_NUM))
					{
						dx = e->x - m->x;
						dy = e->y - m->y;
						if ((kbest < 0) || (dx*dx + dy*dy < distbest))
						{
							kbest = k;
							distbest = dx*dx + dy*dy;
						}
					}
					e++;
				}

				// navigate to enemy
				if (kbest >= 0)
				{
					e = &Enemy[kbest];
					a = atan2f(e->y - m->y, e->x - m->x);
					m->dx = cosf(a)*SpeedList[WEAPON_SONARBOMB]*SpeedMul;
					m->dy = -sinf(a)*SpeedList[WEAPON_SONARBOMB]*SpeedMul;
				}
			}

			m->x += m->dx;
			m->y -= m->dy;
			if ((m->y < -SPRITEH) || (m->y > HEIGHT) || (m->x < -SPRITEW) || (m->x > WIDTH)) // flying out of the screen
				m->type = -1; // destroy missile
			else
			{
				// check hit enemy
				e = Enemy;
				for (k = 0; k < ENEMY_MAX; k++)
				{
					if ((e->type >= 0) && (e->type < ENEMY_NUM))
					{
						dx = e->x - m->x;
						dy = e->y - m->y;
						if (dx*dx + dy*dy < SPRITEW*SPRITEH)
						{
							e->energy -= WeaponEnergy[j];

							// destroy enemy
							if (e->energy <= 0)
							{
								// increase score
								Score += EnemyPoints[e->type];

								if (j == WEAPON_BLACKHOLE) // black hole
								{
									// sucked through a black hole
									PlaySound(Blackhole2Snd, SNDCHAN_MISC);
									e->type = -1;
								}
								else
								{
									// change enemy to explosion
									PlaySound(ExplosionSnd, SNDCHAN_MISC);
									e->type = ENEMY_NUM;
									e->anim = 0;
								}

								// increase shields
								if ((Score >= NextShield) && (Shields >= 0))
								{
									NextShield += 10000;
									Shields++;
									PlaySound(LevelupSnd, SNDCHAN_MISC);
								}

								// increase level (at score: 1000, 2000, 5000, 10000, 17000,...)
								if (Score >= 1000*(1+(Level-1)*(Level-1)))
								{
									Level++;
									PlaySound(LevelupSnd, SNDCHAN_MISC);

									// add weapon
									if (Level <= WEAPON_NUM)
									{
										WeaponMax = Level - 1;
										Weapon = WeaponMax;
									}
								}
							}

							// destroy missile (not black hole)
							if (j != WEAPON_BLACKHOLE)
							{
								m->type = WEAPON_NUM; // change to missile hit
								m->anim = 0;
								break;
							}
						}
					}
					e++;
				}

				// black hole - destroys enemy missiles
				if (j == WEAPON_BLACKHOLE)
				{
					m2 = EnMissile;
					for (k = 0; k < ENMISSILE_MAX; k++)
					{
						// active missile
						if (m2->type >= 0)
						{
							// check distance
							dx = m2->x - m->x;
							dy = m2->y - m->y;
							if (dx*dx + dy*dy < SPRITEW*SPRITEH)
							{
								// sucked through a black hole
								PlaySound(Blackhole2Snd, SNDCHAN_MISC);
								m2->type = -1;
							}
						}
						m2++;
					}
				}
			}
		}
		m++;
	}	
}

// serve enemies
void DoEnemy()
{
	int i, j;
	sEnemy* e;

	// randomness to generate new enemy
	if (RandU16() < 2000*SpeedMul + 400*Level)
	{
		// find free entry
		e = Enemy;
		for (i = 0; i < ENEMY_MAX; i++)
		{
			if (e->type < 0) // entry is not used
			{
				j = Level-1; // max. level
				if (j >= ENEMY_NUM) j = ENEMY_NUM-1; // limit to max. enemy
				e->type = RandU8Max(j); // generate enemy type
				e->x = RandU16MinMax(SPRITEW, WIDTH - TILEW - SPRITEW); // generate X coordinate
				e->y = -4*SPRITEH;
				e->dx = 0;
				e->dy = EnemySpeed[e->type]*SpeedMul; // enemy speed
				e->anim = 0;
				e->energy = EnemyEnergyList[e->type];
				if (e->type > 1) e->dx = (ShipX - e->x)*0.01f; // move towards spaceship
				break;
			}
			e++;
		}
	}

	// move enemy (or dead enemy)
	e = Enemy;
	for (i = 0; i < ENEMY_MAX; i++)
	{
		j = e->type;
		if (j >= 0)
		{
			e->x += e->dx;
			e->y += e->dy;
			if (e->y > HEIGHT) e->type = -1; // enemy is out of the screen
		}
		e++;
	}
}

// serve enemy shooting
void DoEnemyShooting()
{
	int i, j, k;
	sEnemy* e;
	sMissile* m;

	// find valid enemy
	e = Enemy;
	for (i = 0; i < ENEMY_MAX; i++)
	{
		j = e->type;
		if ((j >= 0) && (j < ENEMY_NUM)) // entry is valid and not dead
		{
			// randomness to generate missile
			if (RandU16() < 500*SpeedMul)
			{
				// find unused missile entry
				m = EnMissile;
				for (k = 0; k < ENMISSILE_MAX; k++)
				{
					if (m->type < 0) // not used entry
					{
						m->type = j; // missile type
						m->x = e->x;
						m->y = e->y;
						m->dy = EnemyMissileSpeed[j]*SpeedMul;
						m->dx = 0;
						if (j > 1) m->dx = (ShipX - m->x)*0.02f; // move towards spaceship
						break;
					}
					m++;
				}
			}
		}
		e++;
	}
}

// move enemy missiles
void MoveEnemyMissile()
{
	int i, j;
	sMissile* m;
	float dx, dy;

	// find valid missile
	m = EnMissile;
	for (i = 0; i < ENMISSILE_MAX; i++)
	{
		j = m->type;
		if (j >= 0) // used entry
		{
			m->x += m->dx;
			m->y += m->dy;
			if ((m->y < -SPRITEH) || (m->y > HEIGHT) || (m->x < -SPRITEW) || (m->x > WIDTH)) // flying out of the screen
				m->type = -1; // destroy missile
			else
			{
				// check distance
				dx = ShipX - m->x;
				dy = ShipY - m->y;
				if ((dx*dx + dy*dy < SPRITEW*SPRITEH*0.4) && (Shields >= 0))
				{
					// lost energy
					Energy -= EnemyMissileEnergy[j];
					m->type = -1;
					PlaySound(FailSnd, SNDCHAN_MISC);

					// next shield
					if (Energy < 0)
					{
						Shields--;
						PlaySound(LostSnd, SNDCHAN_MISC);
						if (Shields >= 0) Energy = ENERGY_MAX; else Energy = 0;
					}
				}
			}
		}
		m++;
	}
}

// ============================================================================
//                               Display
// ============================================================================

// display sprite
void DispSprite(int sprite, int x, int y)
{
	DrawImg(SpritesImg, x, y, 0, sprite*SPRITEH, SPRITEW, SPRITEH);
}

// display tile
void DispTile(int tile, int x, int y)
{
	DrawImg(TilesImg, x, y, 0, tile*TILEH, TILEW, TILEH);
}

// display shadow
void DispShadow(int shadow, int x, int y)
{
	DrawImgMask(ShadowsImg, x+SHADOW_SHIFTX, y+SHADOW_SHIFTY, 0, shadow*SHADOWH, SHADOWW, SHADOWH, COLORA(0,0,0,80));
}

// display landscape
void DispLandscape()
{
	// display landscape
	DrawImg(LandscapeImg, 0, LandscapeY, 0, 0, LANDSCAPEW, LANDSCAPEH);
	DrawImg(LandscapeImg, 0, LandscapeY - LANDSCAPEH, 0, 0, LANDSCAPEW, LANDSCAPEH);
}

// display spaceship shadow
void DispShipShadow()
{
	// display shadow
	if (Shields >= 0) DispShadow(SHADOW_SHIP, ShipX, ShipY);
}

// display enemy shadows
void DispEnemyShadow()
{
	int i, j;
	sEnemy* e;

	// display shadow
	e = Enemy;
	for (i = 0; i < ENEMY_MAX; i++)
	{
		j = e->type;
		if ((j >= 0) && (j < ENEMY_NUM)) // entry is valid and not dead
		{
			DispShadow(SHADOW_ENEMY1+j, (int)e->x, (int)e->y);
		}
		e++;
	}
}

// display spaceship shooting
void DispShooting()
{
	int i, j, n;
	sMissile* m;

	// display missiles
	m = MyMissile;
	for (i = 0; i < MYMISSILE_MAX; i++)
	{
		j = m->type;
		if (j >= 0) // active missile (including hit)
		{
			n = WeaponList[j]+1;

			// serve sonar bomb
			if (j == WEAPON_SONARBOMB)
			{
				m->anim++;
				if (m->anim >= SPRITE_WEAPON5_NUM) m->anim = 0;
				n += m->anim;
			}

			// serve black hole
			if (j == WEAPON_BLACKHOLE)
			{
				m->anim++;
				if (m->anim >= SPRITE_WEAPON6_NUM) m->anim = 0;
				n += (SPRITE_WEAPON6_NUM - 1 - m->anim);
			}

			// serve hit
			if (j == WEAPON_NUM)
			{
				n += m->anim;
				m->anim++;
				if (m->anim >= 4) m->type = -1; // destroy missile
			}

			// display missile
			DispSprite(n, (int)m->x, (int)m->y);
		}
		m++;
	}	
}

// display enemies
void DispEnemy()
{
	int i, j;
	sEnemy* e;

	// display enemy
	e = Enemy;
	for (i = 0; i < ENEMY_MAX; i++)
	{
		j = e->type;
		if (j >= 0) // active enemy (including explosion)
		{
			// explosion
			if (j >= ENEMY_NUM)
			{
				DispSprite(SPRITE_EXPLOSE1 + e->anim, (int)e->x, (int)e->y);
				e->anim++;
				if (e->anim >= 8) e->type = -1;
			}
			else
			{
				j *= 2;
				if (j == 4*2) // animate enemy 5
				{
					e->anim++;
					if (e->anim >= 3) e->anim = 0;
					if (e->anim != 0) j += e->anim+1;
				}
				DispSprite(SPRITE_ENEMY1+j, (int)e->x, (int)e->y);
			}
		}
		e++;
	}
}

// display spaceship
void DispShip()
{
	int inx;

	// dead
	if (Shields < 0)
	{
		DispSprite(SPRITE_EXPLOSE5 + (GameFrame & 3), ShipX, ShipY);
	}
	else
	{
		// display ship
		if (Move == MOVE_L) // left
			inx = SPRITE_SHIPL;
		else if (Move == MOVE_R) // right
			inx = SPRITE_SHIPR;
		else if (Move == MOVE_D) // down
			inx = SPRITE_SHIPD;
		else if (Move == MOVE_U) // up
			inx = SPRITE_SHIPU;
		else
			inx = SPRITE_SHIP + (GameFrame & 1);
		DispSprite(inx, ShipX, ShipY);
	}
}

// display enemy missiles
void DispEnemyShooting()
{
	int i, j;
	sMissile* m;

	// display missiles
	m = EnMissile;
	for (i = 0; i < ENMISSILE_MAX; i++)
	{
		j = m->type;
		if (j >= 0) // active missile
		{
			// display missile
			DispSprite(j*2 + SPRITE_ENEMY1_MISSILE, (int)m->x, (int)m->y);
		}
		m++;
	}	
}

// display shields
void DispShields()
{
	int i;
	for (i = 0; i < Shields; i++) DispTile(TILE_SHIELD, 0, i*TILEH);
}

// dispay score
void DispScore()
{
	int i, s, s2;
	s = Score;
	for (i = 5; (i >= 0) || (s > 0); i--)
	{
		s2 = s/10;
		DispTile(TILE_DIGIT0 + s - s2*10, (WIDTH - 6*TILEW - SPRITEW/2)/2 + i*TILEW, 0);
		s = s2;
	}
}

// display current selected weapon
void DispSelWeapon()
{
	DispSprite(WeaponList[Weapon], WIDTH-TILEW-SPRITEW, 2);
}

// display energy
void DispEnergy()
{
	int i;
	for (i = 0; i < Energy; i += 2)
	{
		if (i <= Energy - 2)
			DispTile(TILE_ENERGY2, WIDTH-TILEW, i*TILEH/2);
		else
			DispTile(TILE_ENERGY1, WIDTH-TILEW, i*TILEH/2);
	}
}

// ============================================================================
//                             Main function
// ============================================================================

// wait time and update
void WaitTime(u32 ms)
{
	u32 t;
	ms *= 1000;
	DispUpdate();
	do t = Time(); while ((u32)(t - LastTime) < ms);
	LastTime = t;
}

// main function
int main()
{
	// decompress images
	IntroImg = (u8*)JPGLOAD(IntroImgJPG); if (IntroImg == NULL) Reboot();
	LandscapeImg = (u8*)JPGLOAD(LandscapeImgJPG); if (LandscapeImg == NULL) Reboot();
	SpritesImg = (u8*)PNGLOAD(SpritesImgPNG); if (SpritesImg == NULL) Reboot();
	TilesImg = (u8*)PNGLOAD(TilesImgPNG); if (TilesImg == NULL) Reboot();

	// speed multiply (if using slow display)
#if RASPPI == 1
	SpeedMul = 2;
//#else
//	if (LCDIsOn()) SpeedMul = 2;
#endif

	// intro screen
	DrawImg(IntroImg);
	DispUpdate();

	// wait a key (max. 5 seconds)
	PlaySound(StartupSnd);
	u32 t1 = Time();
	WaitMs(200);
	KeyFlush();
	while ((u32)(Time() - t1) < 5000000)
	{
		int key = KeyGet();
		if (key == KEY_PAD_Y)
			Reboot();
		else if (key == KEY_SCREENSHOT)
			ScreenShot();
		else if (key == KEY_ZOOM)
			LCDRezoom();
		else if ((key == KEY_PAD_B) || (key == KEY_PAD_A) || (key == KEY_PAD_X))
			break;
	}
	StopSound();
	DrawClear();
	DispUpdate();

// ---- initialize

	// initialize new game
	NewGame();

	// start music
	MusicStart();

	// main loop
	while (True)
	{

// --- controls

		// move landscape
		DoLandscape();

		// serve keys, select weapon (returns True to quit the game)
		if (DoKeys())
		{
			MusicStop();
			Reboot();
		}

		// serve spaceship moving
		DoShip();

		// spaceship shooting
		DoShooting();

		// move spaceship missile
		MoveMissile();

		// serve enemies
		DoEnemy();

		// serve enemy shooting
		DoEnemyShooting();

		// move enemy missile
		MoveEnemyMissile();

// --- display

		// display landscape
		DispLandscape();

		// display spaceship shadow
		DispShipShadow();

		// display enemy shadows
		DispEnemyShadow();

		// display spaceship shooting
		DispShooting();

		// display enemies
		DispEnemy();

		// display spaceship
		DispShip();

		// display enemy missiles
		DispEnemyShooting();

		// display shields
		DispShields();

		// dispay score
		DispScore();

		// display current selected weapon
		DispSelWeapon();

		// display energy
		DispEnergy();

		// display update
		DispUpdate();

		// poll music
		MusicPoll();

// --- main loop

		// display update and wait
		WaitTime(50*SpeedMul);

		// frame counter (to serve some animations)
		GameFrame++;
	}
}
