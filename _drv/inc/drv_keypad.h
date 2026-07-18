
// ****************************************************************************
//
//                           BarePi keyboard KeyPad
//
// ****************************************************************************
// If you are using both the KeyPad and MiniKey keyboards at the same time,
// do not use this driver; instead, use the MiniKey driver (USE_MINIKEY).
// If you're only using the pad keys on your MiniKey keyboard, the KeyPad driver is sufficient.

#if USE_KEYPAD && !USE_MINIKEY	// 1=use BarePi KeyPad, 0=not used (drv_keypad.*)

#ifndef _DRV_KEYPAD_H
#define _DRV_KEYPAD_H

#ifndef KEY_REP_TIME1
#define KEY_REP_TIME1		400	// delta time of first press in [ms]
#endif

#ifndef KEY_REP_TIME2
#define KEY_REP_TIME2		100	// delta time of repeat press in [ms]
#endif

#ifndef KEY_REL_TIME
#define KEY_REL_TIME		50	// delta time of release in [ms]
#endif

// Keyboard GPIOs (GPIOs must be in range 0..31)
#ifndef KEY_PAD_A_GPIO
#define KEY_PAD_A_GPIO	22		// keypad [A] button GPIO
#endif

#ifndef KEY_PAD_B_GPIO
#define KEY_PAD_B_GPIO	23		// keypad [B] button GPIO
#endif

#ifndef KEY_PAD_X_GPIO
#define KEY_PAD_X_GPIO	24		// keypad [X] button GPIO
#endif

#ifndef KEY_PAD_Y_GPIO
#define KEY_PAD_Y_GPIO	27		// keypad [Y] button GPIO
#endif

#ifndef KEY_RIGHT_GPIO
#define KEY_RIGHT_GPIO	25		// RIGHT button GPIO
#endif

#ifndef KEY_UP_GPIO
#define KEY_UP_GPIO	6		// UP button GPIO
#endif

#ifndef KEY_LEFT_GPIO
#define KEY_LEFT_GPIO	26		// LEFT button GPIO
#endif

#ifndef KEY_DOWN_GPIO
#define KEY_DOWN_GPIO	5		// DOWN button GPIO
#endif

#ifndef KEY_ALT_GPIO
#define KEY_ALT_GPIO	4		// ALT button GPIO
#endif

// keyboard scan codes
#define KEY_PAD_A	1		// keypad [A] button ([A], Space, Zoom, Nul)
#define KEY_PAD_B	2		// keypad [B] button ([B], Enter, Insert, Edit)
#define KEY_PAD_X	3		// keypad [X] button ([X], Tab, PrtScr)
#define KEY_PAD_Y	4		// keypad [Y] button ([Y], Esc, Menu, Break)
#define KEY_RIGHT	5		// RIGHT button (Right, End)
#define KEY_UP		6		// UP button (Up, PgUp)
#define KEY_LEFT	7		// LEFT button (Left, Home)
#define KEY_DOWN	8		// DOWN button (Down, PgDn)
#define KEY_ALT		9		// ALT button, alternatove function

#define KEYPAD_NUM	9		// number of base keypad keys

// key flags
#define KEY_MASK	0x3f		// mask key scan code

#define KEYFLAG_ALT	B6		// Alt (= 0x40)

// alternative names of the keys
#define KEY_SPACE	KEY_PAD_A
#define KEY_ENTER	KEY_PAD_B
#define KEY_TAB		KEY_PAD_X
#define KEY_ESC		KEY_PAD_Y

// combined keys
#define KEY_END		(KEY_RIGHT | KEYFLAG_ALT)
#define KEY_PGUP	(KEY_UP    | KEYFLAG_ALT)
#define KEY_HOME	(KEY_LEFT  | KEYFLAG_ALT)
#define KEY_PGDN	(KEY_DOWN  | KEYFLAG_ALT)

#define KEY_ZOOM	(KEY_PAD_A | KEYFLAG_ALT)
#define KEY_INSERT	(KEY_PAD_B | KEYFLAG_ALT)
#define KEY_SCREENSHOT	(KEY_PAD_X | KEYFLAG_ALT)
#define KEY_MENU	(KEY_PAD_Y | KEYFLAG_ALT)

#define NOKEY		0	// no key

// keyboard is initialized
extern Bool KeyIsInit;

// key map
extern volatile Bool KeyMap[KEYPAD_NUM];	// key pressed flags
extern volatile u32 KeyPressTime[KEYPAD_NUM];	// time of next press
extern volatile u32 KeyRelTime[KEYPAD_NUM];	// time of last release

// keyboard buffer
#define KEYBUF_SIZE	8		// size of keyboard buffer
extern u8 KeyBuf[KEYBUF_SIZE];		// keyboard buffer
extern u8 KeyRetBuf;			// returned key to keyboard buffer (NOKEY = no key)
extern volatile int KeyWriteOff;	// write offset to keyboard buffer
extern volatile int KeyReadOff;		// read offset from keyboard buffer

// keyboard GPIOs
extern const u8 KeyGPIOList[KEYPAD_NUM];

// keyboard initialize
void KeyInit(void);

// keyboard terminate
void KeyTerm(void);

// check if key KEY_* is pressed (must be valid key code)
Bool KeyPressed(u8 key);

// scan keyboard (called from SysTick handler)
void KeyScan(void);

// get scan code from keyboard buffer (returns NOKEY if no scan code)
u8 KeyGet();

// flush keyboard buffer
void KeyFlush();

// return key to keyboard buffer (can hold only 1 key)
void KeyRet(u8 key);

// check no pressed key
Bool KeyNoPressed();

// wait for no key pressed
void KeyWaitNoPressed();

#endif // _DRV_KEYPAD_H

#endif // USE_KEYPAD && !USE_MINIKEY
