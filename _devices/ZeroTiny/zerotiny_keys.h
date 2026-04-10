
// ****************************************************************************
//
//                           ZeroTiny - Keyboard
//
// ****************************************************************************

#ifndef _ZEROTINY_KEYS_H
#define _ZEROTINY_KEYS_H

#ifndef KEY_REP_TIME1
#define KEY_REP_TIME1		80	// delta time of first press in multiple of SYSTICK_MS (=5 ms, *80 = 400 ms)
#endif

#ifndef KEY_REP_TIME2
#define KEY_REP_TIME2		20	// delta time of repeat press in multiple of SYSTICK_MS (5 ms, *20 = 100 ms)
#endif

#ifndef KEY_REL_TIME
#define KEY_REL_TIME		15	// delta time of release in multiple of SYSTICK_MS (5 ms, *15 = 45 ms)
#endif

// Keyboard GPIOs (GPIOs must be in range 0..31)
#define KEY_A_GPIO	23
#define KEY_B_GPIO	27
#define KEY_X_GPIO	22
#define KEY_Y_GPIO	24
#define KEY_RIGHT_GPIO	5
#define KEY_UP_GPIO	25
#define KEY_LEFT_GPIO	26
#define KEY_DOWN_GPIO	6

// keyboard scan codes
#define KEY_A		0
#define KEY_B		1
#define KEY_X		2
#define KEY_Y		3
#define KEY_RIGHT	4
#define KEY_UP		5
#define KEY_LEFT	6
#define KEY_DOWN	7

#define KEY_NUM		8	// number of keys

// key flags
#define KEY_MASK	0x07	// mask key scan code

// alternative keys
#define KEY_ESC		KEY_Y
#define KEY_TAB		KEY_X
#define KEY_SPACE	KEY_B
#define KEY_ENTER	KEY_A
#define KEY_BS		KEY_X

// unused keys (just so the compiler doesn't report an error)
#define KEY_PGDN	8
#define KEY_PGUP	9
#define KEY_HOME	10
#define KEY_END		11
#define KEY_SCREENSHOT	12

// key map, counters of pressed keys - key is pressed if counter > 0
extern volatile u8 KeyMap[KEY_NUM];	// key pressed counters
extern volatile u8 KeyRelMap[KEY_NUM];	// key release counters

// keyboard buffer
#define KEYBUF_SIZE	8		// size of keyboard buffer
extern int KeyBuf[KEYBUF_SIZE];		// keyboard buffer
extern int KeyRetBuf;			// returned key to keyboard buffer (NOKEY = no key)
extern volatile int KeyWriteOff;	// write offset to keyboard buffer
extern volatile int KeyReadOff;		// read offset from keyboard buffer

// keyboard GPIOs
extern const u8 KeyGPIOList[KEY_NUM];

// keyboard initialize
void KeyInit(void);

// keyboard terminate
void KeyTerm(void);

// check if key is pressed (key must be in range 0..KEY_NUM-1)
//  key ... key scan code KEY_*
INLINE Bool KeyPressed(int key) { return KeyMap[key] != 0; }

// scan keyboard (called from SysTick handler)
void KeyScan(void);

// get scan code from keyboard buffer (returns NOKEY if no scan code)
int KeyGet();

// flush keyboard buffer
void KeyFlush();

// return key to keyboard buffer (can hold only 1 key)
void KeyRet(char key);

// check no pressed key
Bool KeyNoPressed();

// wait for no key pressed
void KeyWaitNoPressed();

#endif // _ZEROTINY_KEYS_H
