
// ****************************************************************************
//
//                               Enemies
//
// ****************************************************************************

#define ENEMY_SPEED	6		// enemy speed
#define ENEMY_SLOWSPEED	2		// enemy slow speed
#define ENEMY_BORDERY	20		// enemy reserve in Y direction

#define ENEMY_HIT_DIST	40		// enemy hit distance
#define SUGAR_HIT_RADIUS 140		// sugar bomb radius

// move segment descriptor
typedef struct {
	s8		steps;		// number of frame steps (0=endless, stop mark)
	s8		dx;		// X increment
	s8		dy;		// Y increment
	s8		res;		// ... reserved (align)
} sMoveSeg;

// move segment macro (n=steps, 0=endless, stop mark)
#define MOVEL(n)	{ n, -ENEMY_SPEED, 0 }			// left
#define MOVEL_SU(n)	{ n, -ENEMY_SPEED, -ENEMY_SLOWSPEED }	// left, slow up
#define MOVEL_SD(n)	{ n, -ENEMY_SPEED, ENEMY_SLOWSPEED }	// left, slow down
#define MOVELD(n)	{ n, -ENEMY_SPEED, ENEMY_SPEED }	// left down
#define MOVED(n)	{ n, 0, ENEMY_SPEED }			// down
#define MOVERD(n)	{ n, ENEMY_SPEED, ENEMY_SPEED }		// right down
#define MOVER(n)	{ n, ENEMY_SPEED, 0 }			// right
#define MOVERU(n)	{ n, ENEMY_SPEED, -ENEMY_SPEED }	// right up
#define MOVEU(n)	{ n, 0, -ENEMY_SPEED }			// up
#define MOVELU(n)	{ n, -ENEMY_SPEED, -ENEMY_SPEED }	// left up

// move templates
extern const sMoveSeg* Moves[];

// enemy template descriptor
typedef struct {
	const char*	name;		// enemy name
	const u8*	imgpng;		// image source PNG
	int		imgpng_size;	// image source PNG size
	u8*		img;		// image
	int		w;		// image width
	int		h;		// image height
	int		animmax;	// max. value of animation phase (before shift)
	int		animshift;	// shift of animation phase counter
	int		animmask;	// mask of animation phase after shift
} sEnemyTemp;

// enemy score
extern const s8 EnemyScore[3];

extern sEnemyTemp	EnemyTemp[ENEMY_NUM]; // enemy templates
extern sEnemyTemp	EnemyNow[ENEMY_LEVNUM]; // current enemy template

// enemy descriptor
typedef struct {
	const sEnemyTemp* temp;		// enemy template, NULL = not used entry
	int		x, y;		// enemy coordinates - middle
	int		anim;		// animation phase
	const sMoveSeg*	move;		// pointer to move segments
	int		step;		// current step in move segment
	int		inx;		// enemy index (0..2)
} sEnemy;

extern sEnemy Enemy[ENEMY_MAX];		// enemies

// explosion descriptor
typedef struct {
	int		anim;		// animation phase (EXPLOSION_PHASES = not used)
	int		x, y;		// explosion coordinates - middle
} sExplosion;

#define EXPLOSIONS_MAX	20		// max. explosions

extern sExplosion Explosion[EXPLOSIONS_MAX];	// explosions

// Explosion
#define EXLPOSION_W		64		// explosion width
#define EXLPOSION_H		64		// explosion height

#define EXPLOSION_PHASES	8		// number of phases

// 01 Fly
#define FLY_W		100		// fly width
#define FLY_H		53		// fly height

// 01 Wasp
#define WASP_W		100		// wasp width
#define WASP_H		65		// wasp height

// 01 Hornet
#define HORNET_W	100		// hornet width
#define HORNET_H	86		// hornet height

// 02 Dragonfly
#define DRAGONFLY_W	120		// dragonfly width
#define DRAGONFLY_H	87		// dragonfly height

// 02 Moth
#define MOTH_W		80		// moth width
#define MOTH_H		91		// moth height

// 02 Kestrel
#define KESTREL_W	100		// kestrel width
#define KESTREL_H	89		// kestrel height

// 03 Maggot
#define MAGGOT_W	74		// kestrel width
#define MAGGOT_H	100		// kestrel height

// 03 Eagle
#define EAGLE_W		100		// eagle width
#define EAGLE_H		105		// eagle height

// 03 Antman
#define ANTMAN_W	100		// antman width
#define ANTMAN_H	92		// antman height

// 04 Fish
#define FISH_W		100		// fish width
#define FISH_H		49		// fish height

// 04 Anglerfish
#define ANGLERFISH_W	120		// anglerfish width
#define ANGLERFISH_H	74		// anglerfish height

// 04 Shark
#define SHARK_W		100		// shark width
#define SHARK_H		55		// shark height

// 05 Penguin
#define PENGUIN_W	60		// penguin width
#define PENGUIN_H	70		// penguin height

// 05 Polar Bear
#define POLARBEAR_W	120		// polar bear width
#define POLARBEAR_H	79		// polar bear height

// 05 Snowflake
#define SNOWFLAKE_W	60		// snowflake width
#define SNOWFLAKE_H	65		// snowflake height

// 06 Phoenix
#define PHOENIX_W	100		// phoenix width
#define PHOENIX_H	70		// phoenix height

// 06 Fire Ball
#define FIREBALL_W	60		// fire ball width
#define FIREBALL_H	71		// fire ball height

// 06 Evil
#define EVIL_W		120		// evil width
#define EVIL_H		54		// evil height

// 07 Pumpkin
#define PUMPKIN_W	60		// pumpkin width
#define PUMPKIN_H	55		// pumpkin height

// 07 Zombie
#define ZOMBIE_W	60		// zombie width
#define ZOMBIE_H	69		// zombie height

// 07 Death
#define DEATH_W		130		// death width
#define DEATH_H		69		// death height

// 08 Camera
#define CAMERA_W	70		// camera width
#define CAMERA_H	33		// camera height

// 08 Explorer
#define EXPLORER_W	60		// explorer width
#define EXPLORER_H	50		// explorer height

// 08 War Dron
#define DRON_W		100		// war dron width
#define DRON_H		71		// war dron height

// 09 Starship Enterprise
#define ENTERPRISE_W		120		// enterprise width
#define ENTERPRISE_H		34		// enterprise height

// 09 Klingon Warship
#define KLINGON_W		100		// klingon width
#define KLINGON_H		53		// klingon height

// 09 Death Star
#define DEATHSTAR_W		80		// death star width
#define DEATHSTAR_H		79		// death star height

// 10 Alien
#define ALIEN_W			80		// alien width
#define ALIEN_H			67		// alien height

// 10 Xenomorph
#define XENOMORPH_W		90		// xenomorph width
#define XENOMORPH_H		97		// xenomorph height

// 10 Meteor
#define METEOR_W		100		// meteor width
#define METEOR_H		54		// meteor height

// 11 Candy Cane
#define CANDYCANE_W		100		// candy cane width
#define CANDYCANE_H		51		// candy cane height

// 11 Lollipop
#define LOLLIPOP_W		100		// lollipop width
#define LOLLIPOP_H		65		// lollipop height

// 11 Fat Man
#define FATMAN_W		100		// fat man width
#define FATMAN_H		53		// fat man height

// 12 Cloud
#define CLOUD_W			70		// cloud width
#define CLOUD_H			57		// cloud height

// 12 Star
#define STAR_W			70		// star width
#define STAR_H			65		// star height

// 12 Moon
#define MOON_W			70		// moon width
#define MOON_H			72		// moon height

// initialize enemies on new level
void InitEnemy();

// add new enemy (temp = enemy template index 0..2)
void AddEnemy(const sEnemyTemp* temp, int x, int y, const sMoveSeg* move, int inx);

// add explosion
void AddExplosion(int x, int y);

// generate new enemy
void NewEnemy();

// shift enemies
void EnemyShift();

// shift explosions
void ExplosionShift();

// display enemies
void EnemyDisp();

// display explosions
void ExplosionDisp();

// hit enemy by missile
void EnemyHit();
