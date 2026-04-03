
// ****************************************************************************
//
//                           ZeroPC - Keyboard
//
// ****************************************************************************

#ifndef _ZEROPC_KEYS_H
#define _ZEROPC_KEYS_H

#ifndef KEY_REP_TIME1
#define KEY_REP_TIME1		12	// delta time of first press in multiple of 7*SYSTICK_MS (=35 ms, *12 = 420 ms)
#endif

#ifndef KEY_REP_TIME2
#define KEY_REP_TIME2		3	// delta time of repeat press in multiple of 7*SYSTICK_MS (35 ms, *3 = 105 ms)
#endif

#ifndef KEY_REL_TIME
#define KEY_REL_TIME		2	// delta time of release in multiple of 7*SYSTICK_MS (35 ms, *2 = 70 ms)
#endif

// Keyboard GPIOs (GPIOs must be in range 0..31)
#define KEY_ROW0_GPIO	17	// ROW0 GPIO
#define KEY_ROW1_GPIO	22	// ROW1 GPIO
#define KEY_ROW2_GPIO	23	// ROW2 GPIO
#define KEY_ROW3_GPIO	24	// ROW3 GPIO
#define KEY_ROW4_GPIO	25	// ROW4 GPIO
#define KEY_ROW5_GPIO	26	// ROW5 GPIO
#define KEY_ROW6_GPIO	27	// ROW6 GPIO

#define KEY_COL0_GPIO	0	// COL0 GPIO
#define KEY_COL1_GPIO	1	// COL1 GPIO
#define KEY_COL2_GPIO	4	// COL2 GPIO
#define KEY_COL3_GPIO	5	// COL3 GPIO
#define KEY_COL4_GPIO	6	// COL4 GPIO
#define KEY_COL5_GPIO	7	// COL5 GPIO
#define KEY_COL6_GPIO	16	// COL6 GPIO

#define KEY_ROW_NUM	7	// number of rows
#define KEY_COL_NUM	7	// number of columns

// keyboard scan codes (row-column)
#define KEY_ALT		0	// 0-0
#define KEY_SHIFT	1	// 0-1
#define KEY_CTRL	2	// 0-2
#define KEY_LEFT	3	// 0-3
#define KEY_DOWN	4	// 0-4
#define KEY_UP		5	// 0-5
#define KEY_RIGHT	6	// 0-6

#define KEY_ESC		7	// 1-0
#define KEY_SPACE	8	// 1-1
#define KEY_Z		9	// 1-2
#define KEY_ENTER	10	// 1-3
#define KEY_TAB		11	// 1-4
#define KEY_BS		12	// 1-5
#define KEY_DOT		13	// 1-6

#define KEY_A		14	// 2-0
#define KEY_X		15	// 2-1
#define KEY_C		16	// 2-2
#define KEY_V		17	// 2-3
#define KEY_B		18	// 2-4
#define KEY_N		19	// 2-5
#define KEY_M		20	// 2-6

#define KEY_D		21	// 3-0
#define KEY_F		22	// 3-1
#define KEY_G		23	// 3-2
#define KEY_H		24	// 3-3
#define KEY_J		25	// 3-4
#define KEY_K		26	// 3-5
#define KEY_L		27	// 3-6

#define KEY_S		28	// 4-0
#define KEY_W		29	// 4-1
#define KEY_E		30	// 4-2
#define KEY_U		31	// 4-3
#define KEY_I		32	// 4-4
#define KEY_O		33	// 4-5
#define KEY_P		34	// 4-6

#define KEY_Q		35	// 5-0
#define KEY_1		36	// 5-1
#define KEY_2		37	// 5-2
#define KEY_3		38	// 5-3
#define KEY_R		39	// 5-4
#define KEY_T		40	// 5-5
#define KEY_Y		41	// 5-6

#define KEY_4		42	// 6-0
#define KEY_5		43	// 6-1
#define KEY_6		44	// 6-2
#define KEY_7		45	// 6-3
#define KEY_8		46	// 6-4
#define KEY_9		47	// 6-5
#define KEY_0		48	// 6-6

#define KEY_NUM		49	// number of scan codes

// key flags
#define KEY_MASK	0x3f	// mask key scan code

#define KEYFLAG_SHIFT		B8	// Shift (= 0x100)
#define KEYFLAG_CTRL		B9	// Ctrl (= 0x200)
#define KEYFLAG_ALT		B10	// Alt (= 0x400)
#define KEYFLAG_SHIFTLOCK	B11	// Shift CapsLock (= 0x800)
#define KEYFLAG_CTRLLOCK	B12	// Ctrl CapsLock (= 0x1000)
#define KEYFLAG_ALTLOCK		B13	// Alt CapsLock (= 0x2000)

// combined keys
#define KEY_HOME	(KEY_LEFT | KEYFLAG_ALT) // key Home
#define KEY_END		(KEY_RIGHT | KEYFLAG_ALT) // key End
#define KEY_PGUP	(KEY_UP | KEYFLAG_ALT) // key PageUp
#define KEY_PGDN	(KEY_DOWN | KEYFLAG_ALT) // key PageDown
#define KEY_SCREENSHOT	(KEY_TAB | KEYFLAG_ALT)	// key Screenshot

// key map, counters of pressed keys - key is pressed if counter > 0
extern volatile u8 KeyMap[KEY_NUM];	// key pressed counters
extern volatile u8 KeyRelMap[KEY_NUM];	// key release counters
extern volatile Bool KeyShiftLock;	// key Shift CapsLock
extern volatile Bool KeyCtrlLock;	// key Ctrl CapsLock
extern volatile Bool KeyAltLock;	// key Alt CapsLock

// keyboard buffer
#define KEYBUF_SIZE	20		// size of keyboard buffer (= 32)
extern int KeyBuf[KEYBUF_SIZE];		// keyboard buffer
extern int KeyRetBuf;			// returned key to keyboard buffer (NOKEY = no key)
extern volatile int KeyWriteOff;	// write offset to keyboard buffer
extern volatile int KeyReadOff;		// read offset from keyboard buffer

// keyboard rows
extern const u8 KeyRowList[KEY_ROW_NUM];

// keyboard colummns
extern const u8 KeyColList[KEY_COL_NUM];

// keyboard initialize
void KeyInit(void);

// keyboard terminate
void KeyTerm(void);

// check if key is pressed (key must be in range 0..KEY_NUM-1)
//  key ... key scan code KEY_* (without KEYFLAG_* flags)
INLINE Bool KeyPressed(int key) { return KeyMap[key] != 0; }

// scan keyboard (called from SysTick handler)
void KeyScan(void);

// remap scan code to ASCII character (returns NOCHAR on invalid key)
//  key ... key scan code KEY_* with KEYFLAG_* flags, also with CapsLock flags
char KeyToChar(int key);

// get scan code from keyboard buffer - with CapsLock flags (returns NOKEY if no scan code)
int KeyGetCaps();

// get scan code from keyboard buffer - without CapsLock flags (returns NOKEY if no scan code)
int KeyGet();

// get character from keyboard buffer (returns NOCHAR if no valid character)
char KeyChar();

// flush keyboard buffer
void KeyFlush();

// return key to keyboard buffer (can hold only 1 key)
void KeyRet(char key);

// check no pressed key
Bool KeyNoPressed();

// wait for no key pressed
void KeyWaitNoPressed();

#endif // _ZEROPC_KEYS_H
