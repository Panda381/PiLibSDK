
// ****************************************************************************
//
//                                  Actors
//
// ****************************************************************************

// actor indices
#define ACT_JILL	0	// Jill's index
#define ACT_BIRD	1	// Bird's index
#define ACT_PARROT	2	// Parrot's index
#define ACT_SCARABEUS	3	// Scarabeus index
#define ACT_STINGRAY	4	// Stingray index
#define ACT_ICEBIRD	5	// Ice Bird's index
#define ACT_DRAGON	6	// Dragon's index
#define ACT_GHOST	7	// Ghost's index
#define ACT_R2D2	8	// R2D2's index
#define ACT_UFO		9	// UFO index
#define ACT_SQUIRREL	10	// Squirrel's index
#define ACT_GINGER	11	// Gingerbread Man's index
#define ACT_DEVILBIRD	12	// Devil Bird's index

#define ACT_MINX	6	// actor's left minimal X coordinate
#define ACT_MAXX	(WIDTH-40) // actor's right maximal X coordinate
#define ACT_MINY	(TIT_HEIGHT+6) // actor's top minimal Y coordinate
#define ACT_MAXY	(HEIGHT-FOOT_HEIGHT-6) // actor's bottom maximal Y coordinate
#define ACT_SPEEDX	10	// actor's speed in X direction
#define ACT_SPEEDY	10	// actor's speed in Y direction
#define ACT_SHADOWY	420	// actor's shadow Y
#define ACT_JUMPSPEED	20	// actor's jump speed (jump height = 10+9+8... = n*(n+1)/2 = 10*11/2 = 55)
#define ACT_ENEMYZONE	60	// Y zone range to search enemies

#define ACT_HIT_DIST	40	// actor hit distance
#define BLOOD_TIME	5	// blood time

#if DEB_SMALLHIT
#define ACT_HIT_FLY	2	// actor hit points - flying
#define ACT_HIT_WALK	1	// actor hit points - walking
#else
#define ACT_HIT_FLY	6	// actor hit points - flying
#define ACT_HIT_WALK	3	// actor hit points - walking
#endif

// actor
extern int ActInx;		// index of current actor (0=Jill)
extern int ActorPhase;		// actor animation phase
extern int ActorX;		// actor middle X coordinate
extern int ActorY;		// actor middle Y coordinate
extern int ActorMinX, ActorMaxX; // actor's middle min/max X coordinate
extern int ActorMinY, ActorMaxY; // actor's middle min/max Y coordinate
extern int ActorJumpSpeed;	// actor's current jump speed
extern int ActorReload;		// counter to reload actor's weapon
extern int BloodTime;		// blood timer (0 = none)
extern int BloodX, BloodY;	// blood coordinate

// actor template descriptor
typedef struct {
	const char*	name;		// actor name
	const char*	missile;	// missile name
	const u8*	imgpng;		// image PNG source
	int		imgpng_size;	// image PNG source size
	u8*		img;		// image
	int		w;		// image width
	int		h;		// image height
	Bool		walk;		// walking actor
} sActorTemp;

extern sActorTemp	ActorTemp[ACT_NUM]; // actor templates
extern sActorTemp	Actor;		// current actor template

// Actor shadow
#define SHADOW_W	64		// shadow width
#define SHADOW_H	14		// shadow height

// Blood
#define BLOOD_W		32		// blood width
#define BLOOD_H		32		// blood height

// 00 Jill
#define JILL_H		201		// Jill height
#define JILL_W		128		// Jill width
#define JILL_RUNNUM	8		// Jill run phases
#define JILL_JUMPUP	8		// Jill jump up phase
#define JILL_JUMPDN	9		// Jill jump down phase
#define JILL_STAND	10		// Jill stand phase

// 01 Bird
#define BIRD_W		200		// bird width
#define BIRD_H		112		// bird height

// 02 Parrot
#define PARROT_W	160		// parrot width
#define PARROT_H	119		// parrot height

// 03 Scarabeus
#define SCARABEUS_W	160		// scarabeus width
#define SCARABEUS_H	117		// scarabeus height

// 04 Stingray
#define STINGRAY_W	200		// stingray width
#define STINGRAY_H	97		// stingray height

// 05 Icebird
#define ICEBIRD_W	160		// icebird width
#define ICEBIRD_H	88		// icebird height

// 06 Dragon
#define DRAGON_W	200		// dragon width
#define DRAGON_H	117		// dragon height

// 07 Ghost
#define GHOST_W		100		// ghost width
#define GHOST_H		111		// ghost height

// 08 R2D2
#define R2D2_W		100		// R2D2 width
#define R2D2_H		121		// R2D2 height

// 09 UFO
#define UFO_W		160		// UFO width
#define UFO_H		100		// UFO height

// 10 Squirrel
#define SQUIRREL_W	120		// squirrel width
#define SQUIRREL_H	120		// squirrel height

// 11 Gingerbread Man
#define GINGERBREAD_W	110		// gingerbread man width
#define GINGERBREAD_H	135		// gingerbread man height

// 12 Devil Bird
#define DEVIL_W		200		// gingerbread man width
#define DEVIL_H		144		// gingerbread man height

// activate blood
void SetBlood(int x, int y);

// display blood
void DispBlood();

// shift blood
void ShiftBlood();

// switch actor
void SetActor(int actinx);

// initialize actor on a new game
void InitActor();

// display actor
void DispActor();

// shift actor animation
void ShiftActor();

// falling actor
void FallActor();

// control actor (returns False to break game)
Bool CtrlActor();

// hit actor
void HitActor();
