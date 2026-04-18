
// ****************************************************************************
//
//                               Actor Missiles
//
// ****************************************************************************

#define MISSILE_SPEEDX_MIN	10		// missile speed in X direction - min
#define MISSILE_SPEEDX_MID	20		// missile speed in X direction - middle
#define MISSILE_SPEEDX_MAX	40		// missile speed in X direction - max

#define MISSILE_SPEEDY_MIN	4		// missile speed in Y direction - min
#define MISSILE_SPEEDY_MID	8		// missile speed in Y direction - middle
#define MISSILE_SPEEDY_MAX	20		// missile speed in Y direction - max

#define MISSILE_SPEED_FLASH0	20		// missile speed of flash of Stingray
#define MISSILE_SPEED_FLASH1	18		// *cos(22.5)
#define MISSILE_SPEED_FLASH2	14		// *cos(45)
#define MISSILE_SPEED_FLASH3	8		// *cos(67.5)

#define MISSILE_SPEED_BLACKHOLE	10		// black hole speed

#define MISSILE_LIFETIME_FLASH	6		// flash lifetime
#define MISSILE_LIFETIME_SCREAM	12		// scream lifetime

// actor missile template
typedef struct {
	const u8*	imgpng;		// missile image PNG source
	int		imgpng_size;	// missile image PNG source size
	u8*		img;		// missile image
	const u8*	sound;		// missile sound
	int		w;		// image width
	int		h;		// image height
	int		phasenum;	// number of animation phases
	int		reload;		// number of frames to reload weapon
	int		hits;		// number of hits
	Bool		bounce;		// bounce from edges
} sMissileTemp;

// actor missile templates
extern sMissileTemp MissileTemp[ACT_NUM];

// actor missile descriptor
typedef struct {
	const sMissileTemp*	temp;	// missile template (NULL = not used)
	int		x, y;		// missile coordinate
	int		dx, dy;		// missile coordinate increment
	int		actinx;		// actor index
	int		phase;		// animation phase
	int		lifetime;	// lifetime counter
	int		hits;		// hit counter
	sEnemy*		target;		// target enemy (NULL = none)
} sMissile;

// actor missile list
#define MISSILE_MAX	30	// max. missiles
extern sMissile Missile[MISSILE_MAX];

// 00 Shuriken
#define SHURIKEN_W	64		// shuriken width
#define SHURIKEN_H	64		// shuriken height

// 01 Seed
#define SEED_W		16		// seed width
#define SEED_H		16		// seed height

// 02 Berry
#define BERRY_W		16		// berry width
#define BERRY_H		16		// berry height

// 03 Sand Ball
#define SANDBALL_W		32		// sand ball width
#define SANDBALL_H		32		// sand ball height

// 04 Flash
#define FLASH_W			32		// flash width
#define FLASH_H			32		// flash height

// 05 Crystal
#define CRYSTAL_W		32		// crystal width
#define CRYSTAL_H		32		// crystal height

// 06 Fire Ball
#define FIRE_W			32		// fire ball width
#define FIRE_H			32		// fire ball height

// 07 Screaming
#define SCREAM_W		16		// screaming width
#define SCREAM_H		32		// screaming height

// 08 Laser
#define LASER_W			16		// laser width
#define LASER_H			16		// laser height

// 09 Phaser
#define PHASER_W		16		// phaser width
#define PHASER_H		16		// phaser height

// 10 Rocket
#define ROCKET_W		64		// rocket width
#define ROCKET_H		32		// rocket height

// 11 Sugar Bomb
#define SUGAR_W			32		// sugar bomb width
#define SUGAR_H			32		// sugar bomb height

// 12 Hole of Nothingness
#define SPIRAL_W		64		// spiral width
#define SPIRAL_H		64		// spiral height

// initialize actor missiles on new level
void InitMissile();

// add new missile
void AddMissile(const sMissileTemp* temp, int actinx, int x, int y, int dx, int dy, sEnemy* target);

// shift missiles
void MissileShift();

// display missiles
void MissileDisp();

// generate missile
void GenMissile();
