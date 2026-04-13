
// ****************************************************************************
//
//                                 Game engine
//
// ****************************************************************************

#ifndef _GAME_H
#define _GAME_H

#define BLACKSREDSW	72			// black/red width
#define BLACKSREDSH	32			// black/red height
#define CARDW		64			// card width
#define CARDH		96			// card height
#define CASTLEW		128			// castle width
#define CASTLEH		232			// castle height
#define CLOUDW		32			// cloud width
#define CLOUDH		25			// cloud height
#define CURSORW		16			// cursor width
#define CURSORH		30			// cursor height
#define FENCEW		9			// fence width
#define FENCEH		216			// fence height
#define FLAGSW		32			// flag width
#define FLAGSH		32			// flag height
#define GRASSW		160			// grass width
#define GRASSH		32			// grass height
#define GRASSY		336			// grass Y coordinate
#define STATEW		72			// state width
#define STATEH		272			// state height
#define STATEY 		80			// state Y start coordinate
#define STATEDY		32			// state delta Y
#define TRUMPETW	32			// trumpet width
#define TRUMPETH	32			// trumpet height
#define WINW		128			// win width
#define WINH		32			// win height

#define CASTLEMAX	100			// castle and fence max. height
#define CASTLEY		352			// castle Y coordinate of bottom
#define CARDNUM		8			// number of cards of one player
#define CARDTYPES	30			// number of card types
#define MIDY		0			// middle card Y
#define MIDX1		(WIDTH/2-CARDW-4)	// middle card 1 X
#define MIDX2		(WIDTH/2+4)		// middle card 2 X
#define CARDY		(HEIGHT-CARDH-8)	// card Y coordinate
#define CARDX		36			// X coordinate of the player's first card
#define CARDDX		72			// increment of cards X coordinate
#define SHADOWOFF	4			// offset of card shadow

#define SOUNDCHAN_BIRD	3			// sound channel - birds
#define SOUNDCHAN_CARD	2			// sound channel - card animation
#define SOUNDCHAN_PAR	1			// sound channel - change parameters
#define SOUNDCHAN_WIN	0			// sound channel - win fanfares and applaus

// sound mode
#define SOUND_OFF	0		// sound is OFF
#define SOUND_SOME	1		// some sound
#define SOUND_ALL	2		// all sound
extern int SoundMode;

// player's parameters (order must be preserved - used to display status)
enum {
	PAR_BUILDERS = 0,	// 0: number of builders
	PAR_BRICKS,		// 1: number of bricks
	PAR_SOLDIERS,		// 2: number of soldiers
	PAR_WEAPONS,		// 3: number of weapons
	PAR_WIZARDS,		// 4: number of wizards
	PAR_CRYSTALS,		// 5: number of crystals
	PAR_CASTLE,		// 6: height of castle
	PAR_FENCE,		// 7: height of fence

	PAR_NUM			// 8: number of parameters
};

// player type
#define PLAYER_HUMAN	0	// human
#define PLAYER_COMP	1	// computer

// player descriptor
typedef struct {
	int	wins;		// total wins
	int	player;		// player type PLAYER_*
	int	par[PAR_NUM];	// parameter
	int	add[PAR_NUM];	// parameter addition
	int	cards[CARDNUM]; // cards (-1 none)
	Bool	disable[CARDNUM]; // cards disable
} sPlayer;

// card descriptor
typedef struct {
	int type;	// card type PAR_*
	int costs;	// card value "costs"
	int param;	// parameter to change PAR_* (PAR_NUM = extra)
	int value;	// change value (+ own, - enemy)
} sCard;

// player state
extern sPlayer Players[2];	// player's game parameters

// game
void Game(int player1, int player2);

#endif // _GAME_H
