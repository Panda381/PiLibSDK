
// ****************************************************************************
//
//                                  Actors
//
// ****************************************************************************

#include "../include.h"

// actor
int ActInx = 0;		// index of current actor (0=Jill)
sActorTemp Actor;	// current actor template
int ActorPhase;		// actor animation phase
int ActorX;		// actor middle X coordinate
int ActorY;		// actor middle Y coordinate
int ActorMinX, ActorMaxX; // actor's middle min/max X coordinate
int ActorMinY, ActorMaxY; // actor's middle min/max Y coordinate
int ActorJumpSpeed;	// actor's current jump speed
int ActorReload;	// counter to reload actor's weapon
int BloodTime;		// blood timer (0 = none)
int BloodX, BloodY;	// blood coordinate

// actor templates
sActorTemp ActorTemp[ACT_NUM] = {
	// name			missile name		imgpng		imgpng_size		img		w		h		walk
	{ "Bara",		"Shuriken",		Img00ActPNG,	sizeof(Img00ActPNG),	NULL,		JILL_W,		JILL_H,		True,	},
	{ "Bird",		"Seed",			Img01ActPNG,	sizeof(Img01ActPNG),	NULL,		BIRD_W,		BIRD_H,		False,	},
	{ "Parrot",		"Berry",		Img02ActPNG,	sizeof(Img02ActPNG),	NULL,		PARROT_W,	PARROT_H,	False,	},
	{ "Scarabeus",		"Sand Ball",		Img03ActPNG,	sizeof(Img03ActPNG),	NULL,		SCARABEUS_W,	SCARABEUS_H,	True,	},
	{ "Stingray",		"Lightning",		Img04ActPNG,	sizeof(Img04ActPNG),	NULL,		STINGRAY_W,	STINGRAY_H,	False,	},
	{ "Ice Bird",		"Ice Crystal",		Img05ActPNG,	sizeof(Img05ActPNG),	NULL,		ICEBIRD_W,	ICEBIRD_H,	False,	},
	{ "Dragon",		"Fire Ball",		Img06ActPNG,	sizeof(Img06ActPNG),	NULL,		DRAGON_W,	DRAGON_H,	False,	},
	{ "Ghost",		"Screaming",		Img07ActPNG,	sizeof(Img07ActPNG),	NULL,		GHOST_W,	GHOST_H,	False,	},
	{ "R2-D2",		"Laser",		Img08ActPNG,	sizeof(Img08ActPNG),	NULL,		R2D2_W,		R2D2_H,		True,	},
	{ "UFO",		"Phaser",		Img09ActPNG,	sizeof(Img09ActPNG),	NULL,		UFO_W,		UFO_H,		False,	},
	{ "Rocket Squirrel",	"Rocket",		Img10ActPNG,	sizeof(Img10ActPNG),	NULL,		SQUIRREL_W,	SQUIRREL_H,	False,	},
	{ "Gingerbread Man",	"Sugar Bomb",		Img11ActPNG,	sizeof(Img11ActPNG),	NULL,		GINGERBREAD_W,	GINGERBREAD_H,	True,	},
	{ "Devil Bird",		"Hole of Nothingness",	Img12ActPNG,	sizeof(Img12ActPNG),	NULL,		DEVIL_W,	DEVIL_H,	False,	},
};

// activate blood
void SetBlood(int x, int y)
{
	if (y < BG_Y_MIN + BLOOD_H/2 + 1) y = BG_Y_MIN + BLOOD_H/2 + 1;
	if (y > BG_Y_MAX - BLOOD_H/2 - 1) y = BG_Y_MAX - BLOOD_H/2 - 1;
	if (x < BLOOD_W/2 + 1) x = BLOOD_W/2 + 1;
	if (x > WIDTH - BLOOD_W/2 - 1) x = WIDTH - BLOOD_W/2 - 1;

	BloodX = x;
	BloodY = y;
	BloodTime = BLOOD_TIME;

	if (SoundMode != SOUNDMODE_OFF) PlaySound(FailSnd);
}

// display blood
void DispBlood()
{
	if (BloodTime > 0)
		DrawImg(BloodImg,			// image
			BloodX - BLOOD_W/2, BloodY - BLOOD_H/2, // destination X, Y
			0, 0,					// source X, Y
			BLOOD_W, BLOOD_H);		// width, height
}

// shift blood
void ShiftBlood()
{
	if (BloodTime > 0) BloodTime--;
}

// switch actor
void SetActor(int actinx)
{
	// prepare number of actors (including Jill)
#if DEB_ALLACTORS		// 1=all actors are available
	int actnum = ACT_NUM;
#else
	int actnum = Level + 2;
	if (actnum > ACT_NUM) actnum = ACT_NUM;
#endif

	// overflow index
	if (actinx < 0) actinx += actnum;
	if (actinx >= actnum) actinx -= actnum;
	if (actinx < 0) actinx = 0;
	if (actinx >= actnum) actinx = actnum-1;

	// reset animation phase
	ActorPhase = 0;

	// set maximum missile reload (to avoid cheating on actor switch)
	ActorReload = MissileTemp[actinx].reload;

	// set new actor
	ActInx = actinx;
	memcpy(&Actor, &ActorTemp[actinx], sizeof(Actor));

	// min/max X coordinate
	ActorMinX = ACT_MINX + Actor.w/2;
	ActorMaxX = ACT_MAXX - Actor.w/2;
	if (ActorX < ActorMinX) ActorX = ActorMinX;
	if (ActorX > ActorMaxX) ActorX = ActorMaxX;

	// min/max Y coordinate
	ActorMinY = ACT_MINY + Actor.h/2;
	ActorMaxY = ACT_MAXY - Actor.h/2;
	if (ActorY < ActorMinY) ActorY = ActorMinY;
	if (ActorY > ActorMaxY) ActorY = ActorMaxY;

	// not jumping
	ActorJumpSpeed = 0;
}

// initialize actor on a new game
void InitActor()
{
	// set currect actor
	SetActor(ActInx);

	// start position
	ActorX = ActorMinX;
	if (Actor.walk)
		ActorY = ActorMaxY; // Y of walking actor
	else
		ActorY = (ActorMinY + ActorMaxY)/2; // Y of flying actor
}

// display actor
void DispActor()
{
	// display shadow
	int x = ActorX - SHADOW_W/2;
	if (ActInx == 4) x -= 20; // correction X for Stingray
	DrawImgMask(ShadowImg,		// source 8-bit image
			x,			// destination X
			ACT_SHADOWY,		// destination Y
			0,			// source X
			0,			// source Y
			SHADOW_W,		// image width
			SHADOW_H,		// image height
			COLORA(0, 0, 0, 192));	// shadow intensity

	// display actor
	DrawImg(	Actor.img,		// source image data
			ActorX - Actor.w/2,	// destination X
			ActorY - Actor.h/2,	// destination Y
			ActorPhase*Actor.w,	// source X
			0,			// source Y
			Actor.w,		// image width
			Actor.h);		// image height
}

// shift actor animation
void ShiftActor()
{
	// actor is moving (if not jumping)
	if (!Actor.walk || (ActorY >= ActorMaxY))
	{
		ActorPhase++;
		if (ActorPhase >= ((ActInx == ACT_JILL) ? JILL_RUNNUM : 4)) ActorPhase = 0;
	}
}

// falling actor
void FallActor()
{
	if (Actor.walk)
	{
		ActorY += ActorJumpSpeed;
		if (ActorY >= ActorMaxY)
		{
			ActorY = ActorMaxY;
			ActorJumpSpeed = 0;
		}
		else
		{
			ActorJumpSpeed++;
			if (ActInx == ACT_JILL)
			{
				if (ActorJumpSpeed < 0)
					ActorPhase = JILL_JUMPUP;
				else
					ActorPhase = JILL_JUMPDN;
			}
			else
				ActorPhase = 0;
		}
	}
}

// control actor (returns False to break game)
Bool CtrlActor()
{
	// key input
	switch (KeyGet())
	{
	// Space: shooting
	//case KEY_SPACE:
	//	{
	//		NewGame(Level+1);
	//	}
	//	break;

	// Enter: Next actor
	case KEY_ENTER:
		SetActor(ActInx+1);
		break;

	// Tab: previous actor
	case KEY_TAB:
		SetActor(ActInx-1);
		break;

	// Esc: game menu
	case KEY_ESC:
		if (!GameMenu()) return False;

		// display header
		DispHeader();

		// display foot
		DispFoot();

		// play music
		MusicStart();
		break;

	// Screenshot
	case KEY_SCREENSHOT:
		ScreenShot();
		break;
	}

	// Shooting
#if !DEB_AUTOSHOOT			// 1=autoshoot
	if (KeyPressed(KEY_SPACE))
#endif
	{
		// generate missile
		GenMissile();
	}

	// Key left
	if (KeyPressed(KEY_LEFT))
	{
		ActorX -= ACT_SPEEDX;
		if (ActorX < ActorMinX) ActorX = ActorMinX;
	}

	// Key right
	if (KeyPressed(KEY_RIGHT))
	{
		ActorX += ACT_SPEEDX;
		if (ActorX > ActorMaxX) ActorX = ActorMaxX;
	}

	// Key up
	if (KeyPressed(KEY_UP))
	{
		// jump
		if (Actor.walk)
		{
			if (ActorY == ActorMaxY) ActorJumpSpeed = -ACT_JUMPSPEED;
		}
		else
		{
			ActorY -= ACT_SPEEDY;
			if (ActorY < ActorMinY) ActorY = ActorMinY;
		}
	}

	// Key down
	if (!Actor.walk && KeyPressed(KEY_DOWN))
	{
		ActorY += ACT_SPEEDY;
		if (ActorY > ActorMaxY) ActorY = ActorMaxY;
	}

	// jumping/falling actor
	FallActor();

	return True;
}

// hit actor
void HitActor()
{
	int i, x, y, dx, dy;

	// hit by bullets
	const sBulletTemp* temp;
	sBullet* b = Bullet;
	for (i = 0; i < BULLET_MAX; i++)
	{
		temp = b->temp;
		if (temp != NULL)
		{
			// get bullet coordinates
			x = b->x;
			y = b->y;

			// check distance
			dx = x - ActorX;
			dy = y - ActorY;
			if (dx*dx + dy*dy <= ACT_HIT_DIST*ACT_HIT_DIST)
			{
				// destroy this bullet
				b->temp = NULL;

				// decrease life
				Life -= Actor.walk ? ACT_HIT_WALK : ACT_HIT_FLY;
				if (Life < 0) Life = 0;

				// activate blood
				SetBlood(x, y);
			}
		}
		b++;
	}

	// hit by enemies
	const sEnemyTemp* te;
	sEnemy* e = Enemy;
	for (i = 0; i < ENEMY_MAX; i++)
	{
		te = e->temp;
		if (te != NULL)
		{
			// get enemy coordinates
			x = e->x;
			y = e->y;

			// check distance
			dx = x - ActorX;
			dy = y - ActorY;
			if (dx*dx + dy*dy <= ACT_HIT_DIST*ACT_HIT_DIST)
			{
				// destroy this enemy
				e->temp = NULL;

				// increase score
				Score += EnemyScore[e->inx];
				AddPoint(EnemyScore[e->inx], x, y);

				// decrease life
				Life -= Actor.walk ? ACT_HIT_WALK : ACT_HIT_FLY;
				if (Life < 0) Life = 0;

				// activate blood
				SetBlood(x, y);
			}
		}
		e++;
	}
}
