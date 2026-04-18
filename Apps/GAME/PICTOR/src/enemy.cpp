
// ****************************************************************************
//
//                               Enemies
//
// ****************************************************************************

#include "../include.h"

// move templates
const sMoveSeg Move1[] = { MOVEL(0), };		// straight left
const sMoveSeg Move1U[] = { MOVEL_SU(0), };	// straight left, slow up
const sMoveSeg Move1D[] = { MOVEL_SD(0), };	// straight left, slow down

const sMoveSeg Move2[] = { MOVEL(20), MOVELD(30), MOVEL(5), MOVELU(10), MOVEL(0), };	// left down
const sMoveSeg Move3[] = { MOVEL(20), MOVELU(30), MOVEL(5), MOVELD(10), MOVEL(0), };	// left up

const sMoveSeg Move4[] = { MOVEL(10), MOVELU(10), MOVEL(5), MOVELD(20), MOVEL(5), MOVELU(20), MOVEL(5), MOVELD(10), MOVEL(0), };	// saw
const sMoveSeg Move5[] = { MOVEL(10), MOVELD(10), MOVEL(5), MOVELU(20), MOVEL(5), MOVELD(20), MOVEL(5), MOVELU(10), MOVEL(0), };	// saw 2
const sMoveSeg Move6[] = { MOVEL(40), MOVEU(20), MOVEL(0), };	// up
const sMoveSeg Move7[] = { MOVEL(40), MOVED(20), MOVEL(0), };	// down
const sMoveSeg* Moves[] = { Move1, Move1U, Move1D,   Move2, Move3,   Move4, Move5, Move6, Move7 };

// enemy score
const s8 EnemyScore[3] = { 10, 20, 50 };

// current enemy template
sEnemyTemp EnemyNow[ENEMY_LEVNUM];

// enemies
sEnemy Enemy[ENEMY_MAX];

// explosions
sExplosion Explosion[EXPLOSIONS_MAX];

// enemy templates
sEnemyTemp EnemyTemp[ENEMY_NUM] = {
	// name			imgpng			imgpng_size		img	w		h		animmax		animshift	animmask
// 1:
	{ "Fly",		Img01En1PNG,		sizeof(Img01En1PNG),	NULL,	FLY_W,		FLY_H,		1,		0,		1,	},
	{ "Wasp",		Img01En2PNG,		sizeof(Img01En2PNG),	NULL,	WASP_W,		WASP_H,		1,		0,		1,	},
	{ "Hornet",		Img01En3PNG,		sizeof(Img01En3PNG),	NULL,	HORNET_W,	HORNET_H,	1,		0,		1,	},
// 2:
	{ "Dragonfly",		Img02En1PNG,		sizeof(Img02En1PNG),	NULL,	DRAGONFLY_W,	DRAGONFLY_H,	1,		0,		1,	},
	{ "Moth",		Img02En2PNG,		sizeof(Img02En2PNG),	NULL,	MOTH_W,		MOTH_H,		1,		0,		1,	},
	{ "Kestrel",		Img02En3PNG,		sizeof(Img02En3PNG),	NULL,	KESTREL_W,	KESTREL_H,	3,		1,		1,	},
// 3:
	{ "Maggot",		Img03En1PNG,		sizeof(Img03En1PNG),	NULL,	MAGGOT_W,	MAGGOT_H,	0,		0,		0,	},
	{ "Eagle",		Img03En2PNG,		sizeof(Img03En2PNG),	NULL,	EAGLE_W,	EAGLE_H,	3,		1,		1,	},
	{ "Antman",		Img03En3PNG,		sizeof(Img03En3PNG),	NULL,	ANTMAN_W,	ANTMAN_H,	3,		1,		1,	},
// 4:
	{ "Fish",		Img04En1PNG,		sizeof(Img04En1PNG),	NULL,	FISH_W,		FISH_H,		3,		1,		1,	},
	{ "Anglerfish",		Img04En2PNG,		sizeof(Img04En2PNG),	NULL,	ANGLERFISH_W,	ANGLERFISH_H,	3,		1,		1,	},
	{ "Shark",		Img04En3PNG,		sizeof(Img04En3PNG),	NULL,	SHARK_W,	SHARK_H,	3,		1,		1,	},
// 5:
	{ "Penguin",		Img05En1PNG,		sizeof(Img05En1PNG),	NULL,	PENGUIN_W,	PENGUIN_H,	1,		0,		1,	},
	{ "Polar Bear",		Img05En2PNG,		sizeof(Img05En2PNG),	NULL,	POLARBEAR_W,	POLARBEAR_H,	3,		1,		1,	},
	{ "Snowflake",		Img05En3PNG,		sizeof(Img05En3PNG),	NULL,	SNOWFLAKE_W,	SNOWFLAKE_H,	0,		0,		0,	},
// 6:
	{ "Phoenix",		Img06En1PNG,		sizeof(Img06En1PNG),	NULL,	PHOENIX_W,	PHOENIX_H,	3,		1,		1,	},
	{ "Fire Ball",		Img06En2PNG,		sizeof(Img06En2PNG),	NULL,	FIREBALL_W,	FIREBALL_H,	3,		1,		1,	},
	{ "Evil",		Img06En3PNG,		sizeof(Img06En3PNG),	NULL,	EVIL_W,		EVIL_H,		3,		1,		1,	},
// 7:
	{ "Pumpkin",		Img07En1PNG,		sizeof(Img07En1PNG),	NULL,	PUMPKIN_W,	PUMPKIN_H,	3,		1,		1,	},
	{ "Zombie",		Img07En2PNG,		sizeof(Img07En2PNG),	NULL,	ZOMBIE_W,	ZOMBIE_H,	0,		0,		0,	},
	{ "Death",		Img07En3PNG,		sizeof(Img07En3PNG),	NULL,	DEATH_W,	DEATH_H,	3,		1,		1,	},
// 8:
	{ "Camera",		Img08En1PNG,		sizeof(Img08En1PNG),	NULL,	CAMERA_W,	CAMERA_H,	0,		0,		0,	},
	{ "Explorer",		Img08En2PNG,		sizeof(Img08En2PNG),	NULL,	EXPLORER_W,	EXPLORER_H,	3,		1,		1,	},
	{ "War Dron",		Img08En3PNG,		sizeof(Img08En3PNG),	NULL,	DRON_W,		DRON_H,		0,		0,		0,	},
// 9:
	{ "Starship Enterprise",Img09En1PNG,		sizeof(Img09En1PNG),	NULL,	ENTERPRISE_W,	 ENTERPRISE_H,	0,		0,		0,	},
	{ "Klingon Warship",	Img09En2PNG,		sizeof(Img09En2PNG),	NULL,	KLINGON_W,	KLINGON_H,	0,		0,		0,	},
	{ "Death Star",		Img09En3PNG,		sizeof(Img09En3PNG),	NULL,	DEATHSTAR_W,	DEATHSTAR_H,	0,		0,		0,	},
// 10:
	{ "Alien",		Img10En1PNG,		sizeof(Img10En1PNG),	NULL,	ALIEN_W,	ALIEN_H,	1,		0,		1,	},
	{ "Xenomorph",		Img10En2PNG,		sizeof(Img10En2PNG),	NULL,	XENOMORPH_W,	XENOMORPH_H,	1,		0,		1,	},
	{ "Meteor",		Img10En3PNG,		sizeof(Img10En3PNG),	NULL,	METEOR_W,	METEOR_H,	3,		1,		1,	},
// 11:
	{ "Candy Cane",		Img11En1PNG,		sizeof(Img11En1PNG),	NULL,	CANDYCANE_W,	CANDYCANE_H,	3,		1,		1,	},
	{ "Lollipop",		Img11En2PNG,		sizeof(Img11En2PNG),	NULL,	LOLLIPOP_W,	LOLLIPOP_H,	3,		1,		1,	},
	{ "Fat Man",		Img11En3PNG,		sizeof(Img11En3PNG),	NULL,	FATMAN_W,	FATMAN_H,	3,		1,		1,	},
// 12:
	{ "Cloud",		Img12En1PNG,		sizeof(Img12En1PNG),	NULL,	CLOUD_W,	CLOUD_H,	0,		0,		0,	},
	{ "Star",		Img12En2PNG,		sizeof(Img12En2PNG),	NULL,	STAR_W,		STAR_H,		0,		0,		0,	},
	{ "Moon",		Img12En3PNG,		sizeof(Img12En3PNG),	NULL,	MOON_W,		MOON_H,		0,		0,		0,	},
};

// initialize enemies on new level
void InitEnemy()
{
	// prepare enemy templates
	int i = BackInx * ENEMY_LEVNUM;
	int j;
	for (j = 0; j < ENEMY_LEVNUM; j++)
	{
		memcpy(&EnemyNow[j], &EnemyTemp[i], sizeof(sEnemyTemp));
		i++;
	}

	// clear enemies
	for (i = 0; i < ENEMY_MAX; i++) Enemy[i].temp = NULL;

	// clear explosions
	for (i = 0; i < EXPLOSIONS_MAX; i++) Explosion[i].anim = EXPLOSION_PHASES;
}

// add new enemy (temp = enemy template index 0..2)
void AddEnemy(const sEnemyTemp* temp, int x, int y, const sMoveSeg* move, int inx)
{
	int i;
	sEnemy* e = Enemy;
	for (i = 0; i < ENEMY_MAX; i++)
	{
		if (e->temp == NULL)
		{
			e->temp = temp;
			e->x = x;
			e->y = y;
			e->anim = 0;
			e->move = move;
			e->step = 0;
			e->inx = inx;
			break;
		}
		e++;
	}
}

// add explosion
void AddExplosion(int x, int y)
{
	int i;
	sExplosion* e = Explosion;
	for (i = 0; i < EXPLOSIONS_MAX; i++)
	{
		if (e->anim >= EXPLOSION_PHASES) // if entry is not used
		{
			e->anim = 0;
			e->x = x;
			e->y = y;
			break;
		}
		e++;
	}
}

// generate new enemy
void NewEnemy()
{
	// prepare current randomness
	int gen = f2i(((ENEMY_GEN_END - ENEMY_GEN_BEG)*CurrentFrame/MaxFrame + ENEMY_GEN_BEG) * powf(ENEMY_GEN_LEVEL, Level));

	// check randomness
	if (EnemyRand() <= (u16)gen)
	{
		// randomness of enemy 3 (range 0..65535, starting at 1/2 of the level)
		gen = 0;
		if (CurrentFrame*2 >= MaxFrame) gen = (CurrentFrame*2 - MaxFrame)*65536/MaxFrame;

		// generate enemy 3
		int inx;
		if (EnemyRand() < gen)
			inx = 2;
		else
		{
			// randomness of enemy 2 (range 0..65535)
			gen = CurrentFrame*50000/MaxFrame;

			// generate enemy 2, or 1 otherwise
			if (EnemyRand() < gen)
				inx = 1;
			else
				inx = 0;
		}

		// enemy template
		const sEnemyTemp* e = &EnemyNow[inx];

		// enemy coordinate Y
		int ymin = BG_Y_MIN + e->h/2 + ENEMY_BORDERY;
		int ymax = BG_Y_MAX - e->h/2 - ENEMY_BORDERY;
		int y = EnemyRandMinMax(ymin, ymax);

		// move
		const sMoveSeg* move;
		int n;
		if (inx == 0) // enemy 1: move straight left
			n = 2;
		else if (inx == 1) // enemy 2: move diagonally
			n = 4;
		else // enemy 3: random movement
			n = count_of(Moves)-1;

		move = Moves[EnemyRandMax(n)];

		// add enemy to the list
		AddEnemy(e, WIDTH + e->w/2 + 4, y, move, inx);
	}
}

// shift enemies, shoot bullets
void EnemyShift()
{
	int i, a;
	const sEnemyTemp* temp;
	sEnemy* e = Enemy;
	for (i = 0; i < ENEMY_MAX; i++)
	{
		temp = e->temp;
		if (temp != NULL)
		{
			// shift enemy animation
			a = e->anim + 1;
			if (a > temp->animmax) a = 0;
			e->anim = a;

			// move enemy
			const sMoveSeg* move = e->move;
			e->x += move->dx;
			e->y += move->dy;

			// limit Y coordinate
			int ymin = BG_Y_MIN + temp->h/2 + ENEMY_BORDERY;
			int ymax = BG_Y_MAX - temp->h/2 - ENEMY_BORDERY;

			if (e->y < ymin)
			{
				e->y = ymin;
				e->step = move->steps; // next move segment
				if (move == Move1U) e->move = Move1D; // flip direction
			}

			if (e->y > ymax)
			{
				e->y = ymax;
				e->step = move->steps; // next move segment
				if (move == Move1D) e->move = Move1U; // flip direction
			}

			// shift move step
			if (move->steps != 0)
			{
				e->step++;

				// next move segment
				if (e->step >= move->steps)
				{
					e->step = 0;
					e->move = move + 1;
				}
			}

			// delete enemy
			if (e->x + temp->w/2 <= -2)
				e->temp = NULL;
			else
			{
				// generate bullet
				if (RandU16() < ENEMY_GEN_BUL)
				{
					AddBullet(&BulletTemp[e->inx], e->x - temp->w/2, e->y);
				}
			}
		}
		e++;
	}
}

// shift explosions
void ExplosionShift()
{
	int i;
	sExplosion* e = Explosion;
	for (i = 0; i < EXPLOSIONS_MAX; i++)
	{
		if (e->anim < EXPLOSION_PHASES)
		{
			e->anim++;
		}
		e++;
	}
}

// display enemies
void EnemyDisp()
{
	int i;
	const sEnemyTemp* temp;
	sEnemy* e = Enemy;
	for (i = 0; i < ENEMY_MAX; i++)
	{
		temp = e->temp;
		if (temp != NULL)
		{
			DrawImg(temp->img,		// image data and palettes
				e->x - temp->w/2, e->y - temp->h/2, // destination X, Y
				((e->anim >> temp->animshift) & temp->animmask)*temp->w, 0,	// source X, Y
				temp->w, temp->h);		// width, height
		}
		e++;
	}
}

// display explosions
void ExplosionDisp()
{
	int i;
	sExplosion* e = Explosion;
	for (i = 0; i < EXPLOSIONS_MAX; i++)
	{
		if (e->anim < EXPLOSION_PHASES)
		{
			DrawImg(ExplosionImg,		// image data and palettes
				e->x - EXLPOSION_W/2, e->y - EXLPOSION_H/2, // destination X, Y
				e->anim * EXLPOSION_W, 0,	// source X, Y
				EXLPOSION_W, EXLPOSION_H);	// width, height
		}
		e++;
	}
}

// hit enemy by missile
void EnemyHit()
{
	int i, j, k, xm, ym, xe, ye, dx, dy, dx2, dy2;
	const sMissileTemp* tm;
	const sEnemyTemp* te;
	sEnemy* e;
	sMissile* m;
	sBullet* b;

	// loop missiles
	m = Missile;
	for (i = 0; i < MISSILE_MAX; i++)
	{
		tm = m->temp;
		if (tm != NULL)
		{
			// get missile coordinates
			xm = m->x;
			ym = m->y;

			// loop enemies
			e = Enemy;
			for (j = 0; j < ENEMY_MAX; j++)
			{
				te = e->temp;
				if ((te != NULL) && (e->x - te->w/2 < WIDTH)) // cannot shot target out of screen
				{
					// get enemy coordinates
					xe = e->x;
					ye = e->y;

					// check distance
					dx = xe - xm;
					dy = ye - ym;
					if (dx*dx + dy*dy <= ENEMY_HIT_DIST*ENEMY_HIT_DIST)
					{
						// destroy this enemy
						e->temp = NULL;

						// increase score
						Score += EnemyScore[e->inx];
						AddPoint(EnemyScore[e->inx], xe, ye);

						// serve black hole
						if (m->actinx == ACT_DEVILBIRD)
						{
							// sound of suck
							if (SoundMode != SOUNDMODE_OFF) PlaySound(Blackhole2Snd, SOUNDCHAN_ENEMYHIT);
						}
						else
						{
							// create explosion
							AddExplosion(xe, ye);

							// sugar bomb
							if (m->actinx == ACT_GINGER)
							{
								// loop enemies
								for (k = 0; k < ENEMY_MAX; k++)
								{
									if (Enemy[k].temp != NULL)
									{
										dx2 = Enemy[k].x - xe;
										dy2 = Enemy[k].y - ye;
										if (dx2*dx2 + dy2*dy2 <= SUGAR_HIT_RADIUS*SUGAR_HIT_RADIUS)
										{
											// destroy this enemy
											Enemy[k].temp = NULL;

											// increase score
											Score += EnemyScore[Enemy[k].inx];
											AddPoint(EnemyScore[Enemy[k].inx], Enemy[k].x, Enemy[k].y);

											// create explosion
											AddExplosion(Enemy[k].x, Enemy[k].y);
										}
									}
								}
							}

							// sound of explosion
							if (SoundMode != SOUNDMODE_OFF) PlaySound(EnemyhitSnd, SOUNDCHAN_ENEMYHIT);
						}

						// destroy missile
						m->hits--;
						if (m->hits <= 0)
						{
							m->temp = NULL;
							break;
						}
					}
				}

				// next enemy
				e++;
			}

			// black hole - catch bullets
			if (m->actinx == ACT_DEVILBIRD)
			{
				// loop bullets
				b = Bullet;
				for (j = 0; j < BULLET_MAX; j++)
				{
					// check if bullet is valid
					if (b->temp != NULL)
					{
						// get bullet coordinates
						xe = b->x;
						ye = b->y;

						// check distance
						dx = xe - xm;
						dy = ye - ym;
						if (dx*dx + dy*dy <= ENEMY_HIT_DIST*ENEMY_HIT_DIST)
						{
							// destroy this bullet
							b->temp = NULL;

							// sound of suck
							if (SoundMode != SOUNDMODE_OFF) PlaySound(Blackhole2Snd, SOUNDCHAN_ENEMYHIT);
						}
					}

					// next bullet
					b++;
				}
			}
		}
		
		// next missile
		m++;
	}
}
