
// ****************************************************************************
//
//                           BarePi keyboard MiniKey
//
// ****************************************************************************
// If you are using both the KeyPad and MiniKey keyboards at the same time,
// do not use this driver; instead, use the MiniKey driver (USE_MINIKEY).
// If you're only using the pad keys on your MiniKey keyboard, the KeyPad driver is sufficient.

#if USE_MINIKEY		// 1=use BarePi MiniKey, 0=not used (drv_minikey.*)

#ifndef _DRV_MINIKEY_H
#define _DRV_MINIKEY_H

#ifndef KEY_REP_TIME1
#define KEY_REP_TIME1	400	// delta time of first press in [ms]
#endif

#ifndef KEY_REP_TIME2
#define KEY_REP_TIME2	100	// delta time of repeat press in [ms]
#endif

#ifndef KEY_REL_TIME
#define KEY_REL_TIME	50	// delta time of release in [ms]
#endif

#ifndef MINIKEY_TIME
#define MINIKEY_TIME	30	// interval of MiniKey service in [ms]
#endif

#ifndef MINIKEY_I2C
#define MINIKEY_I2C	0	// MiniKey I2C bus
#endif

#ifndef MINIKEY_ADDR
#define MINIKEY_ADDR	0x34	// MiniKey I2C address
#endif

#ifndef MINIKEY_SPEED
#define MINIKEY_SPEED	100000	// MiniKey I2C speed
#endif

#if (MINIKEY_I2C != 1) && (!USE_I2CBUS || !USE_I2C0BUS)
#error "MiniKey requires I2C0 bus driver"
#endif

#if (MINIKEY_I2C != 0) && (!USE_I2CBUS || !USE_I2C1BUS)
#error "MiniKey requires I2C1 bus driver"
#endif

// Keyboard GPIOs (GPIOs must be in range 0..31)
#ifndef KEY_PAD_A_GPIO
#define KEY_PAD_A_GPIO	22	// keypad [A] button GPIO
#endif

#ifndef KEY_PAD_B_GPIO
#define KEY_PAD_B_GPIO	23	// keypad [B] button GPIO
#endif

#ifndef KEY_PAD_X_GPIO
#define KEY_PAD_X_GPIO	24	// keypad [X] button GPIO
#endif

#ifndef KEY_PAD_Y_GPIO
#define KEY_PAD_Y_GPIO	27	// keypad [Y] button GPIO
#endif

#ifndef KEY_RIGHT_GPIO
#define KEY_RIGHT_GPIO	25	// RIGHT button GPIO
#endif

#ifndef KEY_UP_GPIO
#define KEY_UP_GPIO	6	// UP button GPIO
#endif

#ifndef KEY_LEFT_GPIO
#define KEY_LEFT_GPIO	26	// LEFT button GPIO
#endif

#ifndef KEY_DOWN_GPIO
#define KEY_DOWN_GPIO	5	// DOWN button GPIO
#endif

#ifndef KEY_ALT_GPIO
#define KEY_ALT_GPIO	4	// ALT button GPIO
#endif

// keyboard scan codes
#define KEY_PAD_A	1	// keypad [A] button ([A], Space, Zoom, Nul)
#define KEY_PAD_B	2	// keypad [B] button ([B], Enter, Insert, Edit)
#define KEY_PAD_X	3	// keypad [X] button ([X], Tab, PrtScr)
#define KEY_PAD_Y	4	// keypad [Y] button ([Y], Esc, Menu, Break)
#define KEY_RIGHT	5	// RIGHT button (Right, End)
#define KEY_UP		6	// UP button (Up, PgUp)
#define KEY_LEFT	7	// LEFT button (Left, Home)
#define KEY_DOWN	8	// DOWN button (Down, PgDn)
#define KEY_ALT		9	// ALT button, alternative function

#define KEYPAD_NUM	9	// number of base keypad keys

// Keyboard scan codes of character keys (row-column)
#define KEY_1		10	// 0-0	1, !, F1
#define KEY_2		11	// 0-1	2, @, F2
#define KEY_3		12	// 0-2	3, #, F3
#define KEY_4		13	// 0-3	4, $, F4
#define KEY_5		14	// 0-4	5, %, F5
#define KEY_Z		15	// 0-5	Z, Disp-, Undo
#define KEY_RES1	16	// 0-6 ... reserved 1, currently not used

#define KEY_Q		17	// 1-0	Q, '\', Query
#define KEY_W		18	// 1-1	W, |, Close
#define KEY_E		19	// 1-2	E, `, Move
#define KEY_R		20	// 1-3	R, ~, Replace
#define KEY_T		21	// 1-4	T, <, Insert
#define KEY_X		22	// 1-5	X, Disp+, Cut
#define KEY_RES2	23	// 1-6 ... reserved 2, currently not used

#define KEY_A		24	// 2-0	A, Vol-, All
#define KEY_S		25	// 2-1	S, Vol+, Save
#define KEY_D		26	// 2-2	D, _, End
#define KEY_F		27	// 2-3	F, [, Find
#define KEY_G		28	// 2-4	G, ], Next
#define KEY_C		29	// 2-5	C, {, Copy
#define KEY_V		30	// 2-6	V, }, Paste

#define KEY_0		31	// 3-0	0, ), F10
#define KEY_9		32	// 3-1	9, (, F9
#define KEY_8		33	// 3-2	8, *, F8
#define KEY_7		34	// 3-3	7, &, F7
#define KEY_6		35	// 3-4	6, ^, F6
#define KEY_SHIFT	36	// 3-5	Shift, CapsLock
#define KEY_B		37	// 3-6	B, ',' , Block

#define KEY_P		38	// 4-0	P, -, Print
#define KEY_O		39	// 4-1	O, +, Open
#define KEY_I		40	// 4-2	I, '/', Tab
#define KEY_U		41	// 4-3	U, =, Home
#define KEY_Y		42	// 4-4	Y, >, Redo
#define KEY_CTRL	43	// 4-5	Ctrl
#define KEY_N		44	// 4-6	N, ., New

#define KEY_BS		45	// 5-0	BS, Delete
#define KEY_L		46	// 5-1	L, ', PgDn
#define KEY_K		47	// 5-2	K, ", PgUp
#define KEY_J		48	// 5-3	J, :, Line
#define KEY_H		49	// 5-4	H, ;, Back
#define KEY_RES3	50	// 5-5 ... reserved 3, currently not used
#define KEY_M		51	// 5-6	M, ?, Enter

#define KEY_NUM		51	// number of scan codes (9+39=48 real keys, 3 keys are not used)

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

#define KEY_CAPSLOCK	(KEY_SHIFT | KEYFLAG_ALT)

#define KEY_ZOOM	(KEY_PAD_A | KEYFLAG_ALT)
#define KEY_INSERT	(KEY_PAD_B | KEYFLAG_ALT)
#define KEY_SCREENSHOT	(KEY_PAD_X | KEYFLAG_ALT)
#define KEY_MENU	(KEY_PAD_Y | KEYFLAG_ALT)

#define NOKEY		0	// no key

// keyboard is initialized
extern Bool KeyIsInit;		// keyboard is initialized
extern u32 MiniKeyTime;		// last time of MiniKey service

// key map
extern volatile Bool KeyMap[KEY_NUM];		// key pressed flags
extern volatile u32 KeyPressTime[KEY_NUM];	// time of next press
extern volatile u32 KeyRelTime[KEYPAD_NUM];	// time of last release (only keypad keys)
extern Bool KeyCapsLock;			// CapsLock

// keyboard buffer
#define KEYBUF_SIZE	8		// size of keyboard buffer
extern u8 KeyBuf[KEYBUF_SIZE];		// keyboard buffer
extern u8 KeyRetBuf;			// returned key to keyboard buffer (NOKEY = no key)
extern volatile int KeyWriteOff;	// write offset to keyboard buffer
extern volatile int KeyReadOff;		// read offset from keyboard buffer

// keyboard GPIOs
extern const u8 KeyGPIOList[KEYPAD_NUM];

// keyboard initialize
// - Must be initialized after initialization of the I2C bus driver.
void KeyInit(void);

// keyboard terminate
void KeyTerm(void);

// check if key KEY_* is pressed (must be valid key code)
Bool KeyPressed(u8 key);

// scan keyboard (called from SysTick handler)
void KeyScan(void);

// get scan code from keyboard buffer (returns NOKEY if no scan code)
u8 KeyGet();

// Get character (Remap keys to ASCII characters; returns NOCHAR of no character)
char KeyGetChar();

// flush keyboard buffer
void KeyFlush();

// return key to keyboard buffer (can hold only 1 key)
void KeyRet(u8 key);

// check no pressed key
Bool KeyNoPressed();

// wait for no key pressed
void KeyWaitNoPressed();

#endif // _DRV_MINIKEY_H

#endif // USE_MINIKEY
