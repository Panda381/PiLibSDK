
// ****************************************************************************
//
//                               Background
//
// ****************************************************************************

extern int BackInx;		// index of background
extern int BackPhase1, BackPhase2, BackPhase3; // background phases

// background template descriptors
typedef struct {
	const char*	name;		// background name
	const u8*	img1jpg;	// background 1 JPG source
	int		img1jpg_size;	// background 1 JPG source size
	const u8*	img2png;	// background 2 PNG source
	int		img2png_size;	// background 2 PNG source size
	const u8*	img3png;	// background 3 PNG source
	int		img3png_size;	// background 3 PNG source size
	u8*		img1;		// image of layer 1
	u8*		img2;		// image of layer 2
	u8*		img3;		// image of layer 3
	int		dy;		// Y correction of layer 2
} sBackTemp;

extern sBackTemp	BackTemp[BG_NUM]; // background templates
extern sBackTemp	Back;		// current background template

// background speed
#define BG_SPEED1	2		// layer 1 speed
#define BG_SPEED2	4		// layer 2 speed
#define BG_SPEED3	6		// layer 3 speed

// === Backgrounds, layers 1

#define BG_Y_MIN	TIT_HEIGHT	// background minimal Y coordinate

// background layer 1 (Y = 20..179)
#define BG_Y		BG_Y_MIN	// background Y coordinate
#define BG_WIDTH	1280		// width of background image
#define BG_HEIGHT	320		// height of background image

// - Overlap layer 1 and layer 2 by 70 lines
// - Layer 2 must add 10 lines

// === Backgrounds, layers 2

// background layer 2 (Y = 150..189)
#define BG2_Y		(BG_Y+BG_HEIGHT-140) // background Y coordinate
#define BG2_WIDTH	1280		// width of background image
#define BG2_HEIGHT	160		// max. height of background image

// Y correction
#define BG2_DY_MEADOW		(BG2_HEIGHT - 128)
#define BG2_DY_JUNGLE		(BG2_HEIGHT - 76)
#define BG2_DY_SANDBEACH	(BG2_HEIGHT - 80)
#define BG2_DY_UNDERWATER	(BG2_HEIGHT - 160)
#define BG2_DY_ICELAND		(BG2_HEIGHT - 76)
#define BG2_DY_FIRELAND		(BG2_HEIGHT - 96)
#define BG2_DY_HAUNTED		(BG2_HEIGHT - 128)
#define BG2_DY_SPACECRAFT	(BG2_HEIGHT - 112)
#define BG2_DY_GALAXY		(BG2_HEIGHT - 132)
#define BG2_DY_PLANET		(BG2_HEIGHT - 94)
#define BG2_DY_CANDYLAND	(BG2_HEIGHT - 70)
#define BG2_DY_SURREAL		(BG2_HEIGHT - 116)

// - Overlap layer 2 and layer 3 by 10 lines
// - Layer 3 must add 30 lines

// === Backgrounds, layers 3

// background layer 3 (Y = 180..219)
#define BG3_Y		(BG2_Y+BG2_HEIGHT-20) // background Y coordinate
#define BG3_WIDTH	1280		// width of background image
#define BG3_HEIGHT	80		// height of background image

// initialize backgrounds on new level
void InitBack();

// display backgrounds
void DispBack();

// shift backgrounds
void ShiftBack();
