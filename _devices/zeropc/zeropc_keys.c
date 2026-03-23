
// ****************************************************************************
//
//                           ZeroPC - Keyboard
//
// ****************************************************************************

#include "../../includes.h"

// keyboard is initialized
Bool KeyIsInit = False;

// current row
volatile int KeyCurRow = 0;

// key map, counters of pressed keys - key is pressed if counter > 0
volatile u8 KeyMap[KEY_NUM];	// key pressed counters
volatile u8 KeyRelMap[KEY_NUM];	// key release counters
volatile Bool KeyShiftLock = False;	// key Shift CapsLock
volatile Bool KeyCtrlLock = False;	// key Ctrl CapsLock
volatile Bool KeyAltLock = False;	// key Alt CapsLock
volatile Bool KeyShiftPrefix = False;	// Shift was pressed
volatile Bool KeyCtrlPrefix = False;	// Ctrl was pressed
volatile Bool KeyAltPrefix = False;	// Alt was pressed

// keyboard buffer
int KeyBuf[KEYBUF_SIZE]; 	// keyboard buffer
int KeyRetBuf = NOKEY;		// returned key to keyboard buffer (NOKEY = no key)
volatile int KeyWriteOff = 0;	// write offset to keyboard buffer
volatile int KeyReadOff = 0;	// read offset from keyboard buffer

// keyboard rows
const u8 KeyRowList[KEY_ROW_NUM] = {
	KEY_ROW0_GPIO,		// ROW0 GPIO
	KEY_ROW1_GPIO,		// ROW1 GPIO
	KEY_ROW2_GPIO,		// ROW2 GPIO
	KEY_ROW3_GPIO,		// ROW3 GPIO
	KEY_ROW4_GPIO,		// ROW4 GPIO
	KEY_ROW5_GPIO,		// ROW5 GPIO
	KEY_ROW6_GPIO,		// ROW6 GPIO
};

// keyboard colummns
const u8 KeyColList[KEY_COL_NUM] = {
	KEY_COL0_GPIO,		// COL0 GPIO
	KEY_COL1_GPIO,		// COL1 GPIO
	KEY_COL2_GPIO,		// COL2 GPIO
	KEY_COL3_GPIO,		// COL3 GPIO
	KEY_COL4_GPIO,		// COL4 GPIO
	KEY_COL5_GPIO,		// COL5 GPIO
	KEY_COL6_GPIO,		// COL6 GPIO
};

// key remap table - base
const char KeyRemapBase[KEY_NUM] = {
	NOCHAR,		// KEY_ALT
	NOCHAR,		// KEY_SHIFT
	NOCHAR,		// KEY_CTRL
	CH_LEFT,	// KEY_LEFT
	CH_DOWN,	// KEY_DOWN
	CH_UP,		// KEY_UP
	CH_RIGHT,	// KEY_RIGHT
	CH_ESC,		// KEY_ESC
	CH_SPC,		// KEY_SPACE
	'z',		// KEY_Z
	CH_CR,		// KEY_ENTER
	CH_TAB,		// KEY_TAB
	CH_BS,		// KEY_BS
	'.',		// KEY_DOT
	'a',		// KEY_A
	'x',		// KEY_X
	'c',		// KEY_C
	'v',		// KEY_V
	'b',		// KEY_B
	'n',		// KEY_N
	'm',		// KEY_M
	'd',		// KEY_D
	'f',		// KEY_F
	'g',		// KEY_G
	'h',		// KEY_H
	'j',		// KEY_J
	'k',		// KEY_K
	'l',		// KEY_L
	's',		// KEY_S
	'w',		// KEY_W
	'e',		// KEY_E
	'u',		// KEY_U
	'i',		// KEY_I
	'o',		// KEY_O
	'p',		// KEY_P
	'q',		// KEY_Q
	'1',		// KEY_1
	'2',		// KEY_2
	'3',		// KEY_3
	'r',		// KEY_R
	't',		// KEY_T
	'y',		// KEY_Y
	'4',		// KEY_4
	'5',		// KEY_5
	'6',		// KEY_6
	'7',		// KEY_7
	'8',		// KEY_8
	'9',		// KEY_9
	'0',		// KEY_0
};

// key remap table - Shift
const char KeyRemapShift[KEY_NUM] = {
	NOCHAR,		// KEY_ALT
	NOCHAR,		// KEY_SHIFT
	NOCHAR,		// KEY_CTRL
	CH_LEFT,	// KEY_LEFT
	CH_DOWN,	// KEY_DOWN
	CH_UP,		// KEY_UP
	CH_RIGHT,	// KEY_RIGHT
	CH_ESC,		// KEY_ESC
	CH_SPC,		// KEY_SPACE
	'Z',		// KEY_Z
	CH_LF,		// KEY_ENTER
	CH_TAB,		// KEY_TAB
	CH_DEL,		// KEY_BS
	',',		// KEY_DOT
	'A',		// KEY_A
	'X',		// KEY_X
	'C',		// KEY_C
	'V',		// KEY_V
	'B',		// KEY_B
	'N',		// KEY_N
	'M',		// KEY_M
	'D',		// KEY_D
	'F',		// KEY_F
	'G',		// KEY_G
	'H',		// KEY_H
	'J',		// KEY_J
	'K',		// KEY_K
	'L',		// KEY_L
	'S',		// KEY_S
	'W',		// KEY_W
	'E',		// KEY_E
	'U',		// KEY_U
	'I',		// KEY_I
	'O',		// KEY_O
	'P',		// KEY_P
	'Q',		// KEY_Q
	'!',		// KEY_1
	'@',		// KEY_2
	'#',		// KEY_3
	'R',		// KEY_R
	'T',		// KEY_T
	'Y',		// KEY_Y
	'$',		// KEY_4
	'%',		// KEY_5
	'^',		// KEY_6
	'&',		// KEY_7
	'*',		// KEY_8
	'(',		// KEY_9
	')',		// KEY_0
};

// key remap table - Alt
const char KeyRemapAlt[KEY_NUM] = {
	NOCHAR,		// KEY_ALT
	NOCHAR,		// KEY_SHIFT
	NOCHAR,		// KEY_CTRL
	CH_HOME,	// KEY_LEFT
	CH_PGDN,	// KEY_DOWN
	CH_PGUP,	// KEY_UP
	CH_END,		// KEY_RIGHT
	NOCHAR,		// KEY_ESC
	CH_INS,		// KEY_SPACE
	NOCHAR,		// KEY_Z
	NOCHAR,		// KEY_ENTER
	NOCHAR,		// KEY_TAB
	NOCHAR,		// KEY_BS
	NOCHAR,		// KEY_DOT
	NOCHAR,		// KEY_A
	NOCHAR,		// KEY_X
	NOCHAR,		// KEY_C
	NOCHAR,		// KEY_V
	'{',		// KEY_B
	'}',		// KEY_N
	'?',		// KEY_M
	'_',		// KEY_D
	'[',		// KEY_F
	']',		// KEY_G
	';',		// KEY_H
	':',		// KEY_J
	'"',		// KEY_K
	'\'',		// KEY_L
	NOCHAR,		// KEY_S
	'|',		// KEY_W
	'\x60',		// KEY_E
	'=',		// KEY_U
	'/',		// KEY_I
	'+',		// KEY_O
	'-',		// KEY_P
	'\\',		// KEY_Q
	NOCHAR,		// KEY_1
	NOCHAR,		// KEY_2
	NOCHAR,		// KEY_3
	'~',		// KEY_R
	'<',		// KEY_T
	'>',		// KEY_Y
	NOCHAR,		// KEY_4
	NOCHAR,		// KEY_5
	NOCHAR,		// KEY_6
	NOCHAR,		// KEY_7
	NOCHAR,		// KEY_8
	NOCHAR,		// KEY_9
	NOCHAR,		// KEY_0
};

// key remap table - Ctrl
const char KeyRemapCtrl[KEY_NUM] = {
	NOCHAR,		// KEY_ALT
	NOCHAR,		// KEY_SHIFT
	NOCHAR,		// KEY_CTRL
	CH_LEFT,	// KEY_LEFT
	CH_DOWN,	// KEY_DOWN
	CH_UP,		// KEY_UP
	CH_RIGHT,	// KEY_RIGHT
	CH_ESC,		// KEY_ESC
	CH_NUL,		// KEY_SPACE
	CH_UNDO,	// KEY_Z
	NOCHAR,		// KEY_ENTER
	NOCHAR,		// KEY_TAB
	NOCHAR,		// KEY_BS
	NOCHAR,		// KEY_DOT
	CH_ALL,		// KEY_A
	CH_CUT,		// KEY_X
	CH_COPY,	// KEY_C
	CH_PASTE,	// KEY_V
	CH_BLOCK,	// KEY_B
	CH_NEW,		// KEY_N
	CH_CR,		// KEY_M
	CH_END,		// KEY_D
	CH_FIND,	// KEY_F
	CH_NEXT,	// KEY_G
	CH_BS,		// KEY_H
	CH_LF,		// KEY_J
	CH_PGUP,	// KEY_K
	CH_PGDN,	// KEY_L
	CH_SAVE,	// KEY_S
	CH_CLOSE,	// KEY_W
	CH_MOVE,	// KEY_E
	CH_HOME,	// KEY_U
	CH_TAB,		// KEY_I
	CH_OPEN,	// KEY_O
	CH_PRINT,	// KEY_P
	CH_QUERY,	// KEY_Q
	NOCHAR,		// KEY_1
	NOCHAR,		// KEY_2
	NOCHAR,		// KEY_3
	CH_REPLACE,	// KEY_R
	CH_INS,		// KEY_T
	CH_REDO,	// KEY_Y
	NOCHAR,		// KEY_4
	NOCHAR,		// KEY_5
	NOCHAR,		// KEY_6
	NOCHAR,		// KEY_7
	NOCHAR,		// KEY_8
	NOCHAR,		// KEY_9
	NOCHAR,		// KEY_0
};

// keyboard initialize
void KeyInit(void)
{
	int i;

	// initialize rows - now inputs without pulls, later output LOW
	for (i = 0; i < KEY_ROW_NUM; i++)
	{
		GPIO_Func(KeyRowList[i], GPIO_FUNC_IN);
		GPIO_Pull(KeyRowList[i], GPIO_PULL_OFF);
		GPIO_Out0(KeyRowList[i]);
	}

	// initialize columns - inputs with pull-ups
	for (i = 0; i < KEY_COL_NUM; i++)
	{
		GPIO_Func(KeyColList[i], GPIO_FUNC_IN);
		GPIO_Pull(KeyColList[i], GPIO_PULL_UP);
	}

	// clear key map
	memset((void*)KeyMap, 0, sizeof(KeyMap));
	memset((void*)KeyRelMap, 0, sizeof(KeyRelMap));

	// keyboard buffer
	KeyRetBuf = NOKEY;	// returned key to keyboard buffer (NOKEY = no key)
	KeyWriteOff = 0;	// write offset to keyboard buffer
	KeyReadOff = 0;		// read offset from keyboard buffer
	KeyShiftLock = False;	// key Shift CapsLock
	KeyCtrlLock = False;	// key Ctrl CapsLock
	KeyAltLock = False;	// key Alt CapsLock
	KeyShiftPrefix = False;	// Shift was pressed
	KeyCtrlPrefix = False;	// Ctrl was pressed
	KeyAltPrefix = False;	// Alt was pressed

	// keyboard is initialized
	cb();
	KeyIsInit = True;
}

// keyboard terminate
void KeyTerm(void)
{
	// keyboard is not initialized
	KeyIsInit = False;
	cb();

	// flush keyboard buffer
	KeyFlush();

	int i;

	// terminate rows
	for (i = 0; i < KEY_ROW_NUM; i++)
	{
		GPIO_Func(KeyRowList[i], GPIO_FUNC_IN);
		GPIO_Pull(KeyRowList[i], (KeyRowList[i] >= 9) ? GPIO_PULL_DOWN : GPIO_PULL_UP);
	}

	// terminate columns
	for (i = 0; i < KEY_COL_NUM; i++)
	{
		GPIO_Func(KeyColList[i], GPIO_FUNC_IN);
		GPIO_Pull(KeyColList[i], (KeyColList[i] >= 9) ? GPIO_PULL_DOWN : GPIO_PULL_UP);
	}
}

// write key to keyboard buffer
void KeyWriteKey(int key)
{
	// get current and next write offset
	int w = KeyWriteOff;	// current write offset
	int w2 = w + 1;		// next write offset
	if (w2 >= KEYBUF_SIZE) w2 = 0; // wrap
	dmb();

	// check empty space
	if (w2 != KeyReadOff)
	{
		// add flags
		if (KeyPressed(KEY_ALT)) key |= KEYFLAG_ALT;
		if (KeyPressed(KEY_SHIFT)) key |= KEYFLAG_SHIFT;
		if (KeyPressed(KEY_CTRL)) key |= KEYFLAG_CTRL;
		if (KeyShiftLock) key |= KEYFLAG_SHIFTLOCK;
		if (KeyCtrlLock) key |= KEYFLAG_CTRLLOCK;
		if (KeyAltLock) key |= KEYFLAG_ALTLOCK;

		// write key
		KeyBuf[w] = key;
		dmb();
		KeyWriteOff = w2;
	}
}

// scan keyboard (called from SysTick handler)
void KeyScan(void)
{
	// check if keyboard is initialized
	if (!KeyIsInit) return;

	// get current row
	int row = KeyCurRow;

	// key map
	int key = row*KEY_COL_NUM;
	volatile u8* m = &KeyMap[key];		// key pressed map
	volatile u8* r = &KeyRelMap[key];	// key released map

	// get columns
	u32 lev = GPIO->GPLEV0;
	const u8* gpio = KeyColList;

	// scan columns
	int col;
	for (col = 0; col < KEY_COL_NUM; col++)
	{
		// load maps
		int mm = *m;	// load pressed map
		int rr = *r;	// load released map

		// check if key is pressed
		if ((lev & BIT(*gpio)) == 0)
		{
			// first press
			if (mm == 0)
			{
				mm = KEY_REP_TIME1; // first press time
				*m = mm;
				KeyWriteKey(key);

				// prefix
				KeyShiftPrefix = (key == KEY_SHIFT);
				KeyCtrlPrefix = (key == KEY_CTRL);
				KeyAltPrefix = (key == KEY_ALT);
			}

			// key is already pressed - check repeat interval
			else
			{
				// Alt, Shift and Ctrl do not repeat
				if ((key != KEY_ALT) && (key != KEY_SHIFT) && (key != KEY_CTRL))
				{
					// press counter
					mm--;
					if (mm == 0)
					{
						mm = KEY_REP_TIME2; // repeat press time
						*m = mm;
						KeyWriteKey(key);
					}
				}
			}

			// reset release time
			rr = KEY_REL_TIME;
		}

		// button is not pressed
		else
		{
			// if not released yet
			if (rr > 0)
			{
				rr--;

				// release
				if (rr == 0)
				{
					// reset pressed counter
					mm = 0;

					// Shift CapsLock
					if ((key == KEY_SHIFT) && KeyShiftPrefix)
					{
						KeyShiftLock = !KeyShiftLock;	// Shift CapsLock
						KeyShiftPrefix = False;		// Shift prefix
					}

					// Ctrl CapsLock
					if ((key == KEY_CTRL) && KeyCtrlPrefix)
					{
						KeyCtrlLock = !KeyCtrlLock;	// Ctrl CapsLock
						KeyCtrlPrefix = False;		// Ctrl prefix
					}

					// Alt CapsLock
					if ((key == KEY_ALT) && KeyAltPrefix)
					{
						KeyAltLock = !KeyAltLock;	// Alt CapsLock
						KeyAltPrefix = False;		// Alt prefix
					}
				}
			}
		}

		// update map
		*m = mm;
		*r = rr;

		// next column
		gpio++;
		m++;
		r++;
		key++;
	}

	// switch OFF current row
	GPIO_Func(KeyRowList[row], GPIO_FUNC_IN);

	// shift to next row
	row++;
	if (row >= KEY_ROW_NUM) row = 0;
	KeyCurRow = row;

	// switch ON next row
	GPIO_Func(KeyRowList[row], GPIO_FUNC_OUT);
}

// remap scan code to ASCII character (returns NOCHAR on invalid key)
//  key ... key scan code KEY_* with KEYFLAG_* flags, also with CapsLock flags
char KeyToChar(int key)
{
	// get flags
	Bool shift = (key & KEYFLAG_SHIFT) != 0;
	Bool ctrl = (key & KEYFLAG_CTRL) != 0;
	Bool alt = (key & KEYFLAG_ALT) != 0;
	Bool shiftlock = (key & KEYFLAG_SHIFTLOCK) != 0;
	key &= KEY_MASK;	// mask scan code

	// get remap table
	char ch;
	if (ctrl)
		ch = KeyRemapCtrl[key];
	else if (alt)
		ch = KeyRemapAlt[key];
	else if (shift)
		ch = KeyRemapShift[key];
	else
		ch = KeyRemapBase[key];
	if (ch == NOCHAR) return NOCHAR;

	// CapsLock
	if (shiftlock && (((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z'))))
	{
		ch ^= 0x20;
	}
	return ch;
}

// get scan code from keyboard buffer - with CapsLock flags (returns NOKEY if no scan code)
int KeyGetCaps()
{
#if !SYSTICK_KEYSCAN	// call KeyScan() function from SysTick system timer
	// scan keyboard
	KeyScan();
	WaitMs(4);
#endif

	// get key from temporary 1-key buffer
	int key = KeyRetBuf;
	if (key != NOKEY)
	{
		KeyRetBuf = NOKEY;
		return key;
	}

	// no key, buffer is empty
	int r = KeyReadOff;
	dmb();
	if (r == KeyWriteOff) return NOKEY;

	// get key
	key = KeyBuf[r];

	// increase read offset
	r++;
	if (r >= KEYBUF_SIZE) r = 0;
	KeyReadOff = r;

	return key;
}

// get scan code from keyboard buffer - without CapsLock flags (returns NOKEY if no scan code)
int KeyGet()
{
	int key = KeyGetCaps();
	if (key == NOKEY) return NOKEY;
	return key & (KEY_MASK | KEYFLAG_SHIFT | KEYFLAG_CTRL | KEYFLAG_ALT);
}

// get character from keyboard buffer (returns NOCHAR if no valid character)
char KeyChar()
{
	char ch;
	int key;
	while (True)
	{
		// get key scan code
		key = KeyGetCaps();
		if (key == NOKEY) return NOCHAR; // no key

		// remap scan code to ASCII character
		ch = KeyToChar(key);
		if (ch != NOCHAR) return ch;
	}
}

// flush keyboard buffer
void KeyFlush()
{
	KeyReadOff = KeyWriteOff;
	KeyRetBuf = NOKEY;
}

// return key to keyboard buffer (can hold only 1 key)
void KeyRet(char key)
{
	KeyRetBuf = key;
}

// check no pressed key
Bool KeyNoPressed()
{
	int i;
	for (i = 0; i < KEY_NUM; i++) if (KeyMap[i] > 0) return False;
	return True;
}

// wait for no key pressed
void KeyWaitNoPressed()
{
	while (!KeyNoPressed()) {}
}
