
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#ifndef _MAIN_H
#define _MAIN_H

#define SYMBOLW		128		// symbol width
#define SYMBOLH		96		// symbol height
#define SYMBOL_NUM	6		// number of symbol types

#define REEL_NUM	3		// number of reels
#define REEL_POS	32		// number of reel positions
#define REELW		SYMBOLW		// reel width
#define REELH		(3*SYMBOLH)	// reel height (= 288)
#define REELALLW	(REEL_NUM*REELW) // width of all reels
#define REEL1X		128		// X coordinate of first reel
#define REEL2X		(128+REELW)	// X coordinate of second reel (= 256)
#define REEL3X		(128+2*REELW)	// X coordinate of third reel (= 384)
#define REELY		96		// Y coordinate of reels

#define REELSTATE_IN	1		// reel state: turning in
#define REELSTATE_OUT	2		// reel state: turning out
#define REELSTATE_OVER	3		// reel state: turning overshoot

#define REEL_TIMEINC	80		// reel time increment of one animation step in [ms]
#define REEL_SLOWSPEED	100		// slow speed
#define REEL_STARTDEL	2		// start delay in number of steps
#define REEL_ACCEL	5		// acceleration/deceleration
#define REEL_OVERSTEPS	150		// overshoot steps
#define REEL_BLURSPEED	200		// speed for blur image

#define REELDIR_FORW	1		// reel direction forward (down), normal direction
#define REELDIR_BACK	0		// reel direction back (up), special direction

#define BET_HIGH	6		// bet max value = high game

#define MAXWIN		750		// max. win value

// reel descriptor
typedef struct {

// Reel position means "position of symbol on reel on line 0" * 256

	int	state;	// reel current state REELSTATE_*
	int	dir;	// reel turning direction REELDIR_*
	int	speed;	// current speed

	int	pos;	// reel current position * 256
	int	dest;	// destination position * 256
	int	rem;	// remaining steps * 256
	int	wait;	// wait time to start in [ms]
	int	over;	// overshoot steps * 256
	int	past;	// pasting steps * 256
} sReel;

#define DIGITW		32		// digit width
#define DIGITH		48		// digit height
#define DIGITY		32		// digit Y coordinate
#define BANKX		80		// BANK X coordinate
#define BANKNUM		4		// BANK number of digits
#define WINX		240		// WIN X coordinate
#define WINNUM		3		// WIN number of digits
#define BETX		368		// BET X coordinate
#define BETNUM		1		// BET number of digits
#define CREDITX		432		// CREDIT X coordinate
#define CREDITNUM	4		// CREDIT number of digits

// Symbols:
#define CHE		0	// cherry
#define APP		1	// apple
#define PLU		2	// plum
#define PEA		3	// pear
#define GRA		4	// grape
#define BEL		5	// bell

#define WIN_LINES	5	// number of winning lines
#define SYM_MASK	0x07	// symbol mask (3 bits, value 0..5)
#define POS_MASK	0x1f	// position mask (5 bits, value 0..31)
#define POSH_MASK	0x1fff	// position mask * 256

// winning item
typedef struct {
	u16		value;			// value of win prize (1 to 750)
	u16		type;			// winning type (which symbols are on winlines)
							//   bit 0..2 (3 bits): symbol 1
							//   bit 3..5 (3 bits): number of lines with symbol 1
							//   bit 6..8 (3 bits): symbol 2
							//   bit 9..11 (3 bits): number of lines with symbol 2
							//   bit 12..14 (3 bits): symbol 3
							//   bit 15 (1 bit): number of lines with symbol 3
							//		Note: If there are 3 different symbols on the winlines,
							//				the symbol 3 can only appear on 1 line, so the
							//				number of lines of the symbol 3 is determined
							//				by 1 bit only.
	u16		num;			// number of wins
	u16		rand;			// randomness (0 to 0xffff)
} sWinItem;

// Symbols on reels
//   CHE=cherry, APP=apple, PLU=plum, PEA=pear, GRA=grape, BEL=bell
extern const u8 ReelTab[REEL_NUM*REEL_POS];

// Winning values
extern const u16 WinValTab[SYMBOL_NUM];

// List of winning tables
extern const sWinItem* const WinTabs[WIN_LINES];

#endif // _MAIN_H
