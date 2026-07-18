
// ****************************************************************************
//
//                           BarePi keyboard MiniKey
//
// ****************************************************************************

#include "../../includes.h"	// includes

#if USE_MINIKEY		// 1=use BarePi MiniKey, 0=not used (drv_minikey.*)

// keyboard is initialized
Bool KeyIsInit = False;		// keyboard is initialized
u32 MiniKeyTime;		// last time of MiniKey service

// MiniKey message
sI2Cmsg MiniKeyWriteMsg;	// MiniKey write message
sI2Cmsg MiniKeyReadMsg;		// MiniKey read message
const u8 MiniKeyAddr[2] = { 0, 12 }; // MiniKey index of keyboard buffer
#define MINIKEY_BUF_LEN	4	// length of MiniKey key buffer
u8 MiniKeyBuf[MINIKEY_BUF_LEN];	// MiniKey key read buffer
u8 MiniKeyBufOld[MINIKEY_BUF_LEN]; // old MiniKey key buffer
Bool MiniKeyWriteDir = True;	// MiniKey write direction (send data address)

// key map
volatile Bool KeyMap[KEY_NUM];		// key pressed flags
volatile u32 KeyPressTime[KEY_NUM];	// time of next press
volatile u32 KeyRelTime[KEYPAD_NUM];	// time of last release (only keypad keys)
Bool KeyCapsLock = False;		// CapsLock

// keyboard buffer
u8 KeyBuf[KEYBUF_SIZE]; 	// keyboard buffer
u8 KeyRetBuf = NOKEY;		// returned key to keyboard buffer (NOKEY = no key)
volatile int KeyWriteOff = 0;	// write offset to keyboard buffer
volatile int KeyReadOff = 0;	// read offset from keyboard buffer

// keyboard GPIOs
extern const u8 KeyGPIOList[KEYPAD_NUM] = {
	KEY_PAD_A_GPIO,
	KEY_PAD_B_GPIO,
	KEY_PAD_X_GPIO,
	KEY_PAD_Y_GPIO,
	KEY_RIGHT_GPIO,
	KEY_UP_GPIO,
	KEY_LEFT_GPIO,
	KEY_DOWN_GPIO,
	KEY_ALT_GPIO,
};

// remap keys to ASCII characters - normal characters
const char KeyCharRemap[KEY_NUM+1] = {
	NOCHAR,		// NOKEY
	CH_SPC,		// #define KEY_PAD_A	1	// keypad [A] button ([A], Space, Zoom, Nul)
	CH_CR,		// #define KEY_PAD_B	2	// keypad [B] button ([B], Enter, Insert, Edit)
	CH_TAB,		// #define KEY_PAD_X	3	// keypad [X] button ([X], Tab, PrtScr)
	CH_ESC,		// #define KEY_PAD_Y	4	// keypad [Y] button ([Y], Esc, Menu, Break)
	CH_RIGHT,	// #define KEY_RIGHT	5	// RIGHT button (Right, End)
	CH_UP,		// #define KEY_UP	6	// UP button (Up, PgUp)
	CH_LEFT,	// #define KEY_LEFT	7	// LEFT button (Left, Home)
	CH_DOWN,	// #define KEY_DOWN	8	// DOWN button (Down, PgDn)
	NOCHAR,		// #define KEY_ALT	9	// ALT button, alternative function

	'1',		// #define KEY_1	10	// 0-0	1, !, F1
	'2',		// #define KEY_2	11	// 0-1	2, @, F2
	'3',		// #define KEY_3	12	// 0-2	3, #, F3
	'4',		// #define KEY_4	13	// 0-3	4, $, F4
	'5',		// #define KEY_5	14	// 0-4	5, %, F5
	'z',		// #define KEY_Z	15	// 0-5	Z, Disp-, Undo
	NOCHAR,		// #define KEY_RES1	16	// 0-6 ... reserved 1, currently not used

	'q',		// #define KEY_Q	17	// 1-0	Q, '\', Query
	'w',		// #define KEY_W	18	// 1-1	W, |, Close
	'e',		// #define KEY_E	19	// 1-2	E, `, Move
	'r',		// #define KEY_R	20	// 1-3	R, ~, Replace
	't',		// #define KEY_T	21	// 1-4	T, <, Insert
	'x',		// #define KEY_X	22	// 1-5	X, Disp+, Cut
	NOCHAR,		// #define KEY_RES2	23	// 1-6 ... reserved 2, currently not used

	'a',		// #define KEY_A	24	// 2-0	A, Vol-, All
	's',		// #define KEY_S	25	// 2-1	S, Vol+, Save
	'd',		// #define KEY_D	26	// 2-2	D, _, End
	'f',		// #define KEY_F	27	// 2-3	F, [, Find
	'g',		// #define KEY_G	28	// 2-4	G, ], Next
	'c',		// #define KEY_C	29	// 2-5	C, {, Copy
	'v',		// #define KEY_V	30	// 2-6	V, }, Paste

	'0',		// #define KEY_0	31	// 3-0	0, ), F10
	'9',		// #define KEY_9	32	// 3-1	9, (, F9
	'8',		// #define KEY_8	33	// 3-2	8, *, F8
	'7',		// #define KEY_7	34	// 3-3	7, &, F7
	'6',		// #define KEY_6	35	// 3-4	6, ^, F6
	NOCHAR,		// #define KEY_SHIFT	36	// 3-5	Shift, CapsLock
	'b',		// #define KEY_B	37	// 3-6	B, ',' , Block

	'p',		// #define KEY_P	38	// 4-0	P, -, Print
	'o',		// #define KEY_O	39	// 4-1	O, +, Open
	'i',		// #define KEY_I	40	// 4-2	I, '/', Tab
	'u',		// #define KEY_U	41	// 4-3	U, =, Home
	'y',		// #define KEY_Y	42	// 4-4	Y, >, Redo
	NOCHAR,		// #define KEY_CTRL	43	// 4-5	Ctrl
	'n',		// #define KEY_N	44	// 4-6	N, ., New

	CH_BS,		// #define KEY_BS	45	// 5-0	BS, Delete
	'l',		// #define KEY_L	46	// 5-1	L, ', PgDn
	'k',		// #define KEY_K	47	// 5-2	K, ", PgUp
	'j',		// #define KEY_J	48	// 5-3	J, :, Line
	'h',		// #define KEY_H	49	// 5-4	H, ;, Back
	NOCHAR,		// #define KEY_RES3	50	// 5-5 ... reserved 3, currently not used
	'm',		// #define KEY_M	51	// 5-6	M, ?, Enter
};

// remap keys to ASCII characters - ALT characters
const char KeyCharRemapAlt[KEY_NUM+1] = {
	NOCHAR,		// NOKEY
	CH_ZOOM,	// #define KEY_PAD_A	1	// keypad [A] button ([A], Space, Zoom, Nul)
	CH_INS,		// #define KEY_PAD_B	2	// keypad [B] button ([B], Enter, Insert, Edit)
	CH_SCREENSHOT,	// #define KEY_PAD_X	3	// keypad [X] button ([X], Tab, PrtScr)
	CH_MENU,	// #define KEY_PAD_Y	4	// keypad [Y] button ([Y], Esc, Menu, Break)
	CH_END,		// #define KEY_RIGHT	5	// RIGHT button (Right, End)
	CH_PGUP,	// #define KEY_UP	6	// UP button (Up, PgUp)
	CH_HOME,	// #define KEY_LEFT	7	// LEFT button (Left, Home)
	CH_PGDN,	// #define KEY_DOWN	8	// DOWN button (Down, PgDn)
	NOCHAR,		// #define KEY_ALT	9	// ALT button, alternative function

	CH_F1,		// #define KEY_1	10	// 0-0	1, !, F1
	CH_F2,		// #define KEY_2	11	// 0-1	2, @, F2
	CH_F3,		// #define KEY_3	12	// 0-2	3, #, F3
	CH_F4,		// #define KEY_4	13	// 0-3	4, $, F4
	CH_F5,		// #define KEY_5	14	// 0-4	5, %, F5
	CH_DISPM,	// #define KEY_Z	15	// 0-5	Z, Disp-, Undo
	NOCHAR,		// #define KEY_RES1	16	// 0-6 ... reserved 1, currently not used

	'\\',		// #define KEY_Q	17	// 1-0	Q, '\', Query
	'|',		// #define KEY_W	18	// 1-1	W, |, Close
	'`',		// #define KEY_E	19	// 1-2	E, `, Move
	'~',		// #define KEY_R	20	// 1-3	R, ~, Replace
	'<',		// #define KEY_T	21	// 1-4	T, <, Insert
	CH_DISPP,	// #define KEY_X	22	// 1-5	X, Disp+, Cut
	NOCHAR,		// #define KEY_RES2	23	// 1-6 ... reserved 2, currently not used

	CH_VOLM,	// #define KEY_A	24	// 2-0	A, Vol-, All
	CH_VOLP,	// #define KEY_S	25	// 2-1	S, Vol+, Save
	'_',		// #define KEY_D	26	// 2-2	D, _, End
	'[',		// #define KEY_F	27	// 2-3	F, [, Find
	']',		// #define KEY_G	28	// 2-4	G, ], Next
	'{',		// #define KEY_C	29	// 2-5	C, {, Copy
	'}',		// #define KEY_V	30	// 2-6	V, }, Paste

	CH_F10,		// #define KEY_0	31	// 3-0	0, ), F10
	CH_F9,		// #define KEY_9	32	// 3-1	9, (, F9
	CH_F8,		// #define KEY_8	33	// 3-2	8, *, F8
	CH_F7,		// #define KEY_7	34	// 3-3	7, &, F7
	CH_F6,		// #define KEY_6	35	// 3-4	6, ^, F6
	NOCHAR,		// #define KEY_SHIFT	36	// 3-5	Shift, CapsLock
	',',		// #define KEY_B	37	// 3-6	B, ',' , Block

	'-',		// #define KEY_P	38	// 4-0	P, -, Print
	'+',		// #define KEY_O	39	// 4-1	O, +, Open
	'/',		// #define KEY_I	40	// 4-2	I, '/', Tab
	'=',		// #define KEY_U	41	// 4-3	U, =, Home
	'>',		// #define KEY_Y	42	// 4-4	Y, >, Redo
	NOCHAR,		// #define KEY_CTRL	43	// 4-5	Ctrl
	'.',		// #define KEY_N	44	// 4-6	N, ., New

	CH_DEL,		// #define KEY_BS	45	// 5-0	BS, Delete
	'\'',		// #define KEY_L	46	// 5-1	L, ', PgDn
	'"',		// #define KEY_K	47	// 5-2	K, ", PgUp
	':',		// #define KEY_J	48	// 5-3	J, :, Line
	';',		// #define KEY_H	49	// 5-4	H, ;, Back
	NOCHAR,		// #define KEY_RES3	50	// 5-5 ... reserved 3, currently not used
	'?',		// #define KEY_M	51	// 5-6	M, ?, Enter
};

// remap keys to ASCII characters - Shift characters
const char KeyCharRemapShift[KEY_NUM+1] = {
	NOCHAR,		// NOKEY
	CH_SPC,		// #define KEY_PAD_A	1	// keypad [A] button ([A], Space, Zoom, Nul)
	CH_CR,		// #define KEY_PAD_B	2	// keypad [B] button ([B], Enter, Insert, Edit)
	CH_TAB,		// #define KEY_PAD_X	3	// keypad [X] button ([X], Tab, PrtScr)
	CH_ESC,		// #define KEY_PAD_Y	4	// keypad [Y] button ([Y], Esc, Menu, Break)
	CH_RIGHT,	// #define KEY_RIGHT	5	// RIGHT button (Right, End)
	CH_UP,		// #define KEY_UP	6	// UP button (Up, PgUp)
	CH_LEFT,	// #define KEY_LEFT	7	// LEFT button (Left, Home)
	CH_DOWN,	// #define KEY_DOWN	8	// DOWN button (Down, PgDn)
	NOCHAR,		// #define KEY_ALT	9	// ALT button, alternative function

	'!',		// #define KEY_1	10	// 0-0	1, !, F1
	'@',		// #define KEY_2	11	// 0-1	2, @, F2
	'#',		// #define KEY_3	12	// 0-2	3, #, F3
	'$',		// #define KEY_4	13	// 0-3	4, $, F4
	'%',		// #define KEY_5	14	// 0-4	5, %, F5
	'Z',		// #define KEY_Z	15	// 0-5	Z, Disp-, Undo
	NOCHAR,		// #define KEY_RES1	16	// 0-6 ... reserved 1, currently not used

	'Q',		// #define KEY_Q	17	// 1-0	Q, '\', Query
	'W',		// #define KEY_W	18	// 1-1	W, |, Close
	'E',		// #define KEY_E	19	// 1-2	E, `, Move
	'R',		// #define KEY_R	20	// 1-3	R, ~, Replace
	'T',		// #define KEY_T	21	// 1-4	T, <, Insert
	'X',		// #define KEY_X	22	// 1-5	X, Disp+, Cut
	NOCHAR,		// #define KEY_RES2	23	// 1-6 ... reserved 2, currently not used

	'A',		// #define KEY_A	24	// 2-0	A, Vol-, All
	'S',		// #define KEY_S	25	// 2-1	S, Vol+, Save
	'D',		// #define KEY_D	26	// 2-2	D, _, End
	'F',		// #define KEY_F	27	// 2-3	F, [, Find
	'G',		// #define KEY_G	28	// 2-4	G, ], Next
	'C',		// #define KEY_C	29	// 2-5	C, {, Copy
	'V',		// #define KEY_V	30	// 2-6	V, }, Paste

	')',		// #define KEY_0	31	// 3-0	0, ), F10
	'(',		// #define KEY_9	32	// 3-1	9, (, F9
	'*',		// #define KEY_8	33	// 3-2	8, *, F8
	'&',		// #define KEY_7	34	// 3-3	7, &, F7
	'^',		// #define KEY_6	35	// 3-4	6, ^, F6
	NOCHAR,		// #define KEY_SHIFT	36	// 3-5	Shift, CapsLock
	'B',		// #define KEY_B	37	// 3-6	B, ',' , Block

	'P',		// #define KEY_P	38	// 4-0	P, -, Print
	'O',		// #define KEY_O	39	// 4-1	O, +, Open
	'I',		// #define KEY_I	40	// 4-2	I, '/', Tab
	'U',		// #define KEY_U	41	// 4-3	U, =, Home
	'Y',		// #define KEY_Y	42	// 4-4	Y, >, Redo
	NOCHAR,		// #define KEY_CTRL	43	// 4-5	Ctrl
	'N',		// #define KEY_N	44	// 4-6	N, ., New

	CH_BS,		// #define KEY_BS	45	// 5-0	BS, Delete
	'L',		// #define KEY_L	46	// 5-1	L, ', PgDn
	'K',		// #define KEY_K	47	// 5-2	K, ", PgUp
	'J',		// #define KEY_J	48	// 5-3	J, :, Line
	'H',		// #define KEY_H	49	// 5-4	H, ;, Back
	NOCHAR,		// #define KEY_RES3	50	// 5-5 ... reserved 3, currently not used
	'M',		// #define KEY_M	51	// 5-6	M, ?, Enter
};

// remap keys to ASCII characters - Ctrl characters
const char KeyCharRemapCtrl[KEY_NUM+1] = {
	NOCHAR,		// NOKEY
	CH_NUL,		// #define KEY_PAD_A	1	// keypad [A] button ([A], Space, Zoom, Nul)
	CH_EDIT,	// #define KEY_PAD_B	2	// keypad [B] button ([B], Enter, Insert, Edit)
	CH_TAB,		// #define KEY_PAD_X	3	// keypad [X] button ([X], Tab, PrtScr)
	CH_BREAK,	// #define KEY_PAD_Y	4	// keypad [Y] button ([Y], Esc, Menu, Break)
	CH_RIGHT,	// #define KEY_RIGHT	5	// RIGHT button (Right, End)
	CH_UP,		// #define KEY_UP	6	// UP button (Up, PgUp)
	CH_LEFT,	// #define KEY_LEFT	7	// LEFT button (Left, Home)
	CH_DOWN,	// #define KEY_DOWN	8	// DOWN button (Down, PgDn)
	NOCHAR,		// #define KEY_ALT	9	// ALT button, alternative function

	'1',		// #define KEY_1	10	// 0-0	1, !, F1
	'2',		// #define KEY_2	11	// 0-1	2, @, F2
	'3',		// #define KEY_3	12	// 0-2	3, #, F3
	'4',		// #define KEY_4	13	// 0-3	4, $, F4
	'5',		// #define KEY_5	14	// 0-4	5, %, F5
	CH_UNDO,	// #define KEY_Z	15	// 0-5	Z, Disp-, Undo
	NOCHAR,		// #define KEY_RES1	16	// 0-6 ... reserved 1, currently not used

	CH_QUERY,	// #define KEY_Q	17	// 1-0	Q, '\', Query
	CH_CLOSE,	// #define KEY_W	18	// 1-1	W, |, Close
	CH_MOVE,	// #define KEY_E	19	// 1-2	E, `, Move
	CH_REPLACE,	// #define KEY_R	20	// 1-3	R, ~, Replace
	CH_INS,		// #define KEY_T	21	// 1-4	T, <, Insert
	CH_CUT,		// #define KEY_X	22	// 1-5	X, Disp+, Cut
	NOCHAR,		// #define KEY_RES2	23	// 1-6 ... reserved 2, currently not used

	CH_ALL,		// #define KEY_A	24	// 2-0	A, Vol-, All
	CH_SAVE,	// #define KEY_S	25	// 2-1	S, Vol+, Save
	CH_END,		// #define KEY_D	26	// 2-2	D, _, End
	CH_FIND,	// #define KEY_F	27	// 2-3	F, [, Find
	CH_NEXT,	// #define KEY_G	28	// 2-4	G, ], Next
	CH_COPY,	// #define KEY_C	29	// 2-5	C, {, Copy
	CH_PASTE,	// #define KEY_V	30	// 2-6	V, }, Paste

	'0',		// #define KEY_0	31	// 3-0	0, ), F10
	'9',		// #define KEY_9	32	// 3-1	9, (, F9
	'8',		// #define KEY_8	33	// 3-2	8, *, F8
	'7',		// #define KEY_7	34	// 3-3	7, &, F7
	'6',		// #define KEY_6	35	// 3-4	6, ^, F6
	NOCHAR,		// #define KEY_SHIFT	36	// 3-5	Shift, CapsLock
	CH_BLOCK,	// #define KEY_B	37	// 3-6	B, ',' , Block

	CH_PRINT,	// #define KEY_P	38	// 4-0	P, -, Print
	CH_OPEN,	// #define KEY_O	39	// 4-1	O, +, Open
	CH_TAB,		// #define KEY_I	40	// 4-2	I, '/', Tab
	CH_HOME,	// #define KEY_U	41	// 4-3	U, =, Home
	CH_REDO,	// #define KEY_Y	42	// 4-4	Y, >, Redo
	NOCHAR,		// #define KEY_CTRL	43	// 4-5	Ctrl
	CH_NEW,		// #define KEY_N	44	// 4-6	N, ., New

	CH_BS,		// #define KEY_BS	45	// 5-0	BS, Delete
	CH_PGDN,	// #define KEY_L	46	// 5-1	L, ', PgDn
	CH_PGUP,	// #define KEY_K	47	// 5-2	K, ", PgUp
	CH_LF,		// #define KEY_J	48	// 5-3	J, :, Line
	CH_BS,		// #define KEY_H	49	// 5-4	H, ;, Back
	NOCHAR,		// #define KEY_RES3	50	// 5-5 ... reserved 3, currently not used
	CH_CR,		// #define KEY_M	51	// 5-6	M, ?, Enter
};

// keyboard initialize
// - Must be initialized after initialization of the I2C bus driver.
void KeyInit(void)
{
	int i;
	u8 gpio;

	// initialize keys - inputs with pull-ups
	for (i = 0; i < KEYPAD_NUM; i++)
	{
		gpio = KeyGPIOList[i];
		GPIO_Func(gpio, GPIO_FUNC_IN);
		GPIO_Pull(gpio, GPIO_PULL_UP);
	}

	// clear key map
	memset((void*)KeyMap, False, sizeof(KeyMap));

	// keyboard buffer
	KeyRetBuf = NOKEY;	// returned key to keyboard buffer (NOKEY = no key)
	KeyWriteOff = 0;	// write offset to keyboard buffer
	KeyReadOff = 0;		// read offset from keyboard buffer
	memset(MiniKeyBufOld, 0, sizeof(MiniKeyBufOld)); // clear old keyboard buffer

	// prepare MiniKey write message (send data address)
	sI2Cmsg* msg = &MiniKeyWriteMsg; // write message
	msg->callback = NULL;		// no callback
	msg->data = (u8*)MiniKeyAddr;	// pointer to message data
	msg->len = 2;			// length of message data
	msg->data2 = NULL;		// no data part 2
	msg->len2 = 0;			// no data part 2
	msg->speed = MINIKEY_SPEED;	// transfer speed in Hz
	msg->addr = MINIKEY_ADDR;	// device address
	msg->read = False;		// write direction

	// prepare MiniKey read message (read key buffer)
	msg = &MiniKeyReadMsg;		// read message
	msg->callback = NULL;		// no callback
	msg->data = MiniKeyBuf;		// pointer to message data
	msg->len = MINIKEY_BUF_LEN;	// length of message data
	msg->data2 = NULL;		// no data part 2
	msg->len2 = 0;			// no data part 2
	msg->speed = MINIKEY_SPEED;	// transfer speed in Hz
	msg->addr = MINIKEY_ADDR;	// device address
	msg->read = True;		// read direction

	// send next request - write address
	MiniKeyWriteDir = True;		// MiniKey write direction (send data address)
	I2Cbus_Add(MINIKEY_I2C, &MiniKeyWriteMsg);
	MiniKeyTime = Time();

	// keyboard is initialized
	cb();
	KeyIsInit = True;
}

// keyboard terminate
void KeyTerm(void)
{
	int i;
	u8 gpio;

	// keyboard is not initialized
	KeyIsInit = False;
	cb();

	// flush keyboard buffer
	KeyFlush();

	// terminate pins
	for (i = 0; i < KEYPAD_NUM; i++)
	{
		gpio = KeyGPIOList[i];
		GPIO_Func(gpio, GPIO_FUNC_IN);
		GPIO_Pull(gpio, (gpio >= 9) ? GPIO_PULL_DOWN : GPIO_PULL_UP);
	}
}

// check if key KEY_* is pressed (must be valid key code)
Bool KeyPressed(u8 key)
{
	// ALT flag
	if ((key & KEYFLAG_ALT) != 0)
	{
		key &= KEY_MASK;
		return KeyMap[key-1] && KeyMap[KEY_ALT-1];
	}
	return KeyMap[key-1];
}

// write key to keyboard buffer
void KeyWriteKey(u8 key)
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

		// write key
		KeyBuf[w] = key;
		dmb();
		KeyWriteOff = w2;
	}
}

// check if key is in keyboard buffer
Bool KeyIsInKeyBuf(u8* keybuf, u8 key)
{
	return ((keybuf[0] == key) ||
		(keybuf[1] == key) ||
		(keybuf[2] == key) ||
		(keybuf[3] == key));
}

// scan keyboard (called from SysTick handler)
void KeyScan(void)
{
	// check if keyboard is initialized
	if (!KeyIsInit) return;

	// key map
	u8 key = 1;
	volatile Bool* m = KeyMap;		// key pressed flags
	volatile u32* p = KeyPressTime;		// time of next press
	volatile u32* r = KeyRelTime;		// time of last release

	// scan keys
	for (; key <= KEYPAD_NUM; key++)
	{
		// check if key is pressed
		if (GPIO_In(KeyGPIOList[key-1]) == 0)
		{
			// first press
			if (!*m)
			{
				*m = True;			// key is pressed
				*p = Time() + KEY_REP_TIME1*1000; // time of next press
				KeyWriteKey(key);		// write key to buffer
			}

			// key is already pressed - repead press
			else
			{
				// Alt does not repeat
				if (key != KEY_ALT)
				{
					// check press time
					if ((s32)(Time() - *p) >= 0)
					{
						*p = Time() + KEY_REP_TIME2*1000; // time of next press
						KeyWriteKey(key);		// write key to buffer
					}
				}
			}

			// time of last release
			*r = Time();
		}

		// button is not pressed
		else
		{
			// if released
			if ((u32)(Time() - *r) >= KEY_REL_TIME*1000)
			{
				// clear pressed flag
				*m = False;
			}
		}

		// next key
		m++;
		p++;
		r++;
	}

	// check MiniKey service
	if ((u32)(Time() - MiniKeyTime) >= MINIKEY_TIME*1000)
	{
		MiniKeyTime = Time();

		// process last request - read keyboard buffer
		if (!MiniKeyWriteDir) // was read direction
		{
			// not done yet
			if (!I2Cbus_IsDone(&MiniKeyReadMsg)) return;

			// if OK
			if (MiniKeyReadMsg.status == I2CMSG_DONE)
			{
				// process keys
				int i;
				for (i = 0; i < MINIKEY_BUF_LEN; i++)
				{
					// key is pressed
					key = MiniKeyBuf[i];
					if ((key > KEYPAD_NUM) && (key <= KEY_NUM))
					{
						// flag - key is pressed
						KeyMap[key-1] = True;

						// check if key is first pressed
						if (!KeyIsInKeyBuf(MiniKeyBufOld, key))
						{
							KeyPressTime[key-1] = Time() + KEY_REP_TIME1*1000; // time of next press
							KeyWriteKey(key);		// write key to buffer
						}

						// key was pressed
						else
						{
							// Ctrl and Shift does not repeat
							if ((key != KEY_CTRL) && (key != KEY_SHIFT))
							{
								// check press time
								if ((s32)(Time() - KeyPressTime[key-1]) >= 0)
								{
									KeyPressTime[key-1] = Time() + KEY_REP_TIME2*1000; // time of next press
									KeyWriteKey(key);		// write key to buffer
								}
							}
						}
					}
				}

				// process old keys
				for (i = 0; i < MINIKEY_BUF_LEN; i++)
				{
					// key was pressed
					key = MiniKeyBufOld[i];
					if ((key > KEYPAD_NUM) && (key <= KEY_NUM))
					{
						// check if key is now not pressed
						if (!KeyIsInKeyBuf(MiniKeyBuf, key))
						{
							// flag - key is not pressed
							KeyMap[key-1] = False;
						}
					}
				}

				// backup current key buffer
				memcpy(MiniKeyBufOld, MiniKeyBuf, sizeof(MiniKeyBufOld));
			}

			// send next request - write address
			MiniKeyWriteDir = True;
			I2Cbus_Add(MINIKEY_I2C, &MiniKeyWriteMsg);
		}

		// process last request - write address
		else
		{
			// not done yet
			if (!I2Cbus_IsDone(&MiniKeyWriteMsg)) return;

			// error - repeat last write request
			if (MiniKeyWriteMsg.status != I2CMSG_DONE)
			{
				I2Cbus_Add(MINIKEY_I2C, &MiniKeyWriteMsg);
			}

			// send next request - read keyboard buffer
			else
			{
				MiniKeyWriteDir = False;
				I2Cbus_Add(MINIKEY_I2C, &MiniKeyReadMsg);
			}
		}
	}
}

// get scan code from keyboard buffer (returns NOKEY if no scan code)
u8 KeyGet()
{
#if !SYSTICK_KEYSCAN	// call KeyScan() function from SysTick system timer
	// scan keyboard
	KeyScan();
	WaitMs(5);
#endif

	// get key from temporary 1-key buffer
	u8 key = KeyRetBuf;
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

// Get character (Remap keys to ASCII characters; returns NOCHAR of no character)
char KeyGetChar()
{
	u8 key, key2;
	char ch;

	do {
		while (True)
		{
			// key gey
			key = KeyGet();
			if (key == NOKEY) return NOCHAR;

			// CapsLock (Alt+Shift)
			if (key == KEY_CAPSLOCK) KeyCapsLock = !KeyCapsLock;

			// destroy Shift, Alt or Ctrl
			key2 = key & KEY_MASK;
			if ((key2 != KEY_SHIFT) && (key2 != KEY_CTRL) && (key2 != KEY_ALT)) break;
		}

		// Alt
		if ((key & KEYFLAG_ALT) != 0)
			ch = KeyCharRemapAlt[key2];

		// Ctrl
		else if (KeyPressed(KEY_CTRL))
			ch = KeyCharRemapCtrl[key2];

		// Shift
		else if (KeyPressed(KEY_SHIFT))
			ch = KeyCharRemapShift[key2];

		// normal
		else
			ch = KeyCharRemap[key2];

	} while (ch == NOCHAR);

	// CapsLock
	if (KeyCapsLock && (((ch >= 'A') && (ch <= 'Z')) || ((ch >= 'a') && (ch <= 'z')))) ch ^= 0x20;

	return ch;
}

// flush keyboard buffer
void KeyFlush()
{
	KeyReadOff = KeyWriteOff;
	KeyRetBuf = NOKEY;
}

// return key to keyboard buffer (can hold only 1 key)
void KeyRet(u8 key)
{
	KeyRetBuf = key;
}

// check no pressed key
Bool KeyNoPressed()
{
	int i;
	for (i = 0; i < KEY_NUM; i++) if (KeyMap[i]) return False;
	return True;
}

// wait for no key pressed
void KeyWaitNoPressed()
{
	while (!KeyNoPressed()) {}
}

#endif // USE_MINIKEY
