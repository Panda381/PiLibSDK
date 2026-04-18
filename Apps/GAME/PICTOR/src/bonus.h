
// ****************************************************************************
//
//                                 Bonus
//
// ****************************************************************************

#define BONUS_SCORE		200		// bonus points
#define BONUS_BIGSCORE		10000		// big bonus points
#define BONUS_BIGSCORE_TEXT	"10000"		// big bonus text
#define BONUS_BIGSCORE_LEN	5		// length of bit bonus text

// bonus index
#define BONUS_HEART	0		// heart
#define BONUS_TOP	1		// Picopad top cover
#define BONUS_CPU	2		// Picopad CPU
#define BONUS_DISP	3		// Picopad display
#define BONUS_BAT	4		// Picopad battery
#define BONUS_ON	5		// Picopad is ON

#define BONUS_NUM	6		// number of bonuses

#define BONUS_COLFIRST	1		// index of first collected bonus
#define BONUS_COLLAST	4		// index of last collected bonus

// bonus mask
#define BONUS_MASK_TOP	BIT(BONUS_TOP)	// Picopad top cover
#define BONUS_MASK_CPU	BIT(BONUS_CPU)	// Picopad CPU
#define BONUS_MASK_DISP	BIT(BONUS_DISP)	// Picopad display
#define BONUS_MASK_BAT	BIT(BONUS_BAT)	// Picopad battery

#define BONUS_MASK_ALL	(B1+B2+B3+B4)	// Picopad all components

// bonus template
typedef struct {
	const u8*	imgpng;		// image PNG source
	int		imgpng_size;	// image PNG source size
	u8*		img;		// image
	int		w;		// image width
	int		h;		// image height
} sBonusTemp;

// bonus templates
extern sBonusTemp BonusTemp[BONUS_NUM];

// current bonus
extern int BonusMask;			// mask of collected Picopad components (BONUS_MASK_*)
extern int BonusInx;			// index of current bonus (-1 if bonus is not active)
extern const sBonusTemp* Bonus;		// current bonus template
extern int BonusX;			// coordinate X of current bonus
extern int BonusY;			// coordinate Y of current bonus
extern int BonusPhase;			// bonus animation phase (0..2)

// initialize bonus on start of next level (requires initialized EnemyRandSeed)
void InitBonus();

// shift bonus
void BonusShift();

// display bonus
void BonusDisp();

// collect bonus
void BonusCollect();

// 0: Heart
#define HEART_W			32		// width
#define HEART_H			32		// height

// 1: Picopad top cover 
#define PICOPAD1_W		77		// width
#define PICOPAD1_H		32		// height

// 2: Picopad CPU
#define PICOPAD2_W		51		// width
#define PICOPAD2_H		20		// height

// 3: Picopad display
#define PICOPAD3_W		44		// width
#define PICOPAD3_H		27		// height

// 4: Picopad battery
#define PICOPAD4_W		18		// width
#define PICOPAD4_H		20		// height

// 5: Picopad complet on
#define PICOPAD_W		80		// width
#define PICOPAD_H		32		// height

// Bonus
#define BONUS_W			64		// width
#define BONUS_H			64		// height
#define BONUS_PHASES		6		// number of animation phases
