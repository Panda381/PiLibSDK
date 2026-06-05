
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#include "../include.h"

// - we include *.cpp files for faster compilation, rather than to link it
// images
#include "../img/00act.cpp"
#include "../img/00shot.cpp"
#include "../img/01act.cpp"
#include "../img/01bg1.cpp"
#include "../img/01bg2.cpp"
#include "../img/01bg3.cpp"
#include "../img/01en1.cpp"
#include "../img/01en2.cpp"
#include "../img/01en3.cpp"
#include "../img/01shot.cpp"
#include "../img/02act.cpp"
#include "../img/02bg1.cpp"
#include "../img/02bg2.cpp"
#include "../img/02bg3.cpp"
#include "../img/02en1.cpp"
#include "../img/02en2.cpp"
#include "../img/02en3.cpp"
#include "../img/02shot.cpp"
#include "../img/03act.cpp"
#include "../img/03bg1.cpp"
#include "../img/03bg2.cpp"
#include "../img/03bg3.cpp"
#include "../img/03en1.cpp"
#include "../img/03en2.cpp"
#include "../img/03en3.cpp"
#include "../img/03shot.cpp"
#include "../img/04act.cpp"
#include "../img/04bg1.cpp"
#include "../img/04bg2.cpp"
#include "../img/04bg3.cpp"
#include "../img/04en1.cpp"
#include "../img/04en2.cpp"
#include "../img/04en3.cpp"
#include "../img/04shot.cpp"
#include "../img/05act.cpp"
#include "../img/05bg1.cpp"
#include "../img/05bg2.cpp"
#include "../img/05bg3.cpp"
#include "../img/05en1.cpp"
#include "../img/05en2.cpp"
#include "../img/05en3.cpp"
#include "../img/05shot.cpp"
#include "../img/06act.cpp"
#include "../img/06bg1.cpp"
#include "../img/06bg2.cpp"
#include "../img/06bg3.cpp"
#include "../img/06en1.cpp"
#include "../img/06en2.cpp"
#include "../img/06en3.cpp"
#include "../img/06shot.cpp"
#include "../img/07act.cpp"
#include "../img/07bg1.cpp"
#include "../img/07bg2.cpp"
#include "../img/07bg3.cpp"
#include "../img/07en1.cpp"
#include "../img/07en2.cpp"
#include "../img/07en3.cpp"
#include "../img/07shot.cpp"
#include "../img/08act.cpp"
#include "../img/08bg1.cpp"
#include "../img/08bg2.cpp"
#include "../img/08bg3.cpp"
#include "../img/08en1.cpp"
#include "../img/08en2.cpp"
#include "../img/08en3.cpp"
#include "../img/08shot.cpp"
#include "../img/09act.cpp"
#include "../img/09bg1.cpp"
#include "../img/09bg2.cpp"
#include "../img/09bg3.cpp"
#include "../img/09en1.cpp"
#include "../img/09en2.cpp"
#include "../img/09en3.cpp"
#include "../img/09shot.cpp"
#include "../img/10act.cpp"
#include "../img/10bg1.cpp"
#include "../img/10bg2.cpp"
#include "../img/10bg3.cpp"
#include "../img/10en1.cpp"
#include "../img/10en2.cpp"
#include "../img/10en3.cpp"
#include "../img/10shot.cpp"
#include "../img/11act.cpp"
#include "../img/11bg1.cpp"
#include "../img/11bg2.cpp"
#include "../img/11bg3.cpp"
#include "../img/11en1.cpp"
#include "../img/11en2.cpp"
#include "../img/11en3.cpp"
#include "../img/11shot.cpp"
#include "../img/12act.cpp"
#include "../img/12bg1.cpp"
#include "../img/12bg2.cpp"
#include "../img/12bg3.cpp"
#include "../img/12en1.cpp"
#include "../img/12en2.cpp"
#include "../img/12en3.cpp"
#include "../img/12shot.cpp"
#include "../img/blood.cpp"
#include "../img/bonuscloud.cpp"
#include "../img/bullet1.cpp"
#include "../img/bullet2.cpp"
#include "../img/bullet3.cpp"
#include "../img/explosion.cpp"
#include "../img/heart.cpp"
#include "../img/picopad.cpp"
#include "../img/picopad1.cpp"
#include "../img/picopad2.cpp"
#include "../img/picopad3.cpp"
#include "../img/picopad4.cpp"
#include "../img/shadow.cpp"
#include "../img/title_barepi.cpp"

// sounds
#include "../snd/bigbonus.cpp"
#include "../snd/blackhole.cpp"
#include "../snd/blackhole2.cpp"
#include "../snd/electricity.cpp"
#include "../snd/enemyhit.cpp"
#include "../snd/fail.cpp"
#include "../snd/flame.cpp"
#include "../snd/glass.cpp"
#include "../snd/gun.cpp"
#include "../snd/lasershot.cpp"
#include "../snd/minigun.cpp"
#include "../snd/music1.cpp"
#include "../snd/music2.cpp"
#include "../snd/music3.cpp"
#include "../snd/music4.cpp"
#include "../snd/music5.cpp"
#include "../snd/music6.cpp"
#include "../snd/music7.cpp"
#include "../snd/music8.cpp"
#include "../snd/music9.cpp"
#include "../snd/music10.cpp"
#include "../snd/music11.cpp"
#include "../snd/music12.cpp"
#include "../snd/noise.cpp"
#include "../snd/phasershot.cpp"
#include "../snd/rocketshot.cpp"
#include "../snd/screaming.cpp"
#include "../snd/swing.cpp"
#include "../snd/throw.cpp"
#include "../snd/zing.cpp"

// source codes
#include "actor.cpp"
#include "background.cpp"
#include "bonus.cpp"
#include "bullet.cpp"
#include "disp.cpp"
#include "enemy.cpp"
#include "foot.cpp"
#include "game.cpp"
#include "gameend.cpp"
#include "gamemenu.cpp"
#include "gamestart.cpp"
#include "header.cpp"
#include "missile.cpp"
#include "point.cpp"
#include "random.cpp"
#include "sound.cpp"
#include "titlescreen.cpp"

// common resources
u8 *BloodImg, *BonuscloudImg, *ExplosionImg, *TitleImg, *PicopadImg;

// ============================================================================
//                             Main function
// ============================================================================

// main function
int main()
{
	int i;

	DrawClear();
	DrawText2("Initialize...", 0, 0, COL_WHITE);
	DispUpdate();

	// get home path and filename
	HomePathLen = GetHomePath(HomePath, PATH_MAX, HomePathFilename);

	// unpack actor images
	sActorTemp* a = ActorTemp;
	for (i = 0; i < ACT_NUM; i++)
	{
		a->img = (u8*)PNGLoadBuf(a->imgpng, a->imgpng_size);
		if (a->img == NULL) Reboot();
		a++;
	}

	// unpack missile images
	sMissileTemp* m = MissileTemp;
	for (i = 0; i < ACT_NUM; i++)
	{
		m->img = (u8*)PNGLoadBuf(m->imgpng, m->imgpng_size);
		if (m->img == NULL) Reboot();
		m++;
	}

	// unpack background images
	sBackTemp* b = BackTemp;
	for (i = 0; i < BG_NUM; i++)
	{
		b->img1 = (u8*)JPGLoadBuf(b->img1jpg, b->img1jpg_size);
		if (b->img1 == NULL) Reboot();
		b->img2 = (u8*)PNGLoadBuf(b->img2png, b->img2png_size);
		if (b->img2 == NULL) Reboot();
		b->img3 = (u8*)PNGLoadBuf(b->img3png, b->img3png_size);
		if (b->img3 == NULL) Reboot();
		b++;
	}

	// unpack enemy images
	sEnemyTemp* e = EnemyTemp;
	for (i = 0; i < ENEMY_NUM; i++)
	{
		e->img = (u8*)PNGLoadBuf(e->imgpng, e->imgpng_size);
		if (e->img == NULL) Reboot();
		e++;
	}

	// unpack bonus images
	sBonusTemp* t = BonusTemp;
	for (i = 0; i < BONUS_NUM; i++)
	{
		t->img = (u8*)PNGLoadBuf(t->imgpng, t->imgpng_size);
		if (t->img == NULL) Reboot();
		t++;
	}

	// unpack enemy bullet images
	sBulletTemp* u = BulletTemp;
	for (i = 0; i < BULLET_TEMP_NUM; i++)
	{
		u->img = (u8*)PNGLoadBuf(u->imgpng, u->imgpng_size);
		if (u->img == NULL) Reboot();
		u++;
	}

	// unpack common resources
	PicopadImg = (u8*)PNGLOAD(ImgPicopadPNG); if (PicopadImg == NULL) Reboot();
	BloodImg = (u8*)PNGLOAD(ImgBloodPNG); if (BloodImg == NULL) Reboot();
	BonuscloudImg = (u8*)PNGLOAD(ImgBonusPNG); if (BonuscloudImg == NULL) Reboot();
	ExplosionImg = (u8*)PNGLOAD(ImgExplosionPNG); if (ExplosionImg == NULL) Reboot();
	TitleImg = (u8*)PNGLOAD(ImgTitleBarePiPNG); if (TitleImg == NULL) Reboot();

	// load top list and max. score
	MaxScore = LoadTop();

	// main loop
	while (True)
	{
		// Title screen
		if (!TitleScreen()) Reboot();

		// game
		Game();
		MusicStop();
		StopAllSound();

		// game end
		if ((Life <= 0) || (Score > MaxScore)) GameEnd();
	}
}
