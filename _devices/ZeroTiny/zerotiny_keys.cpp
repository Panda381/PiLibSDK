
// ****************************************************************************
//
//                           ZeroTiny - Keyboard
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

// keyboard buffer
int KeyBuf[KEYBUF_SIZE]; 	// keyboard buffer
int KeyRetBuf = NOKEY;		// returned key to keyboard buffer (NOKEY = no key)
volatile int KeyWriteOff = 0;	// write offset to keyboard buffer
volatile int KeyReadOff = 0;	// read offset from keyboard buffer

// keyboard GPIOs
extern const u8 KeyGPIOList[KEY_NUM] = {
	KEY_A_GPIO,
	KEY_B_GPIO,
	KEY_X_GPIO,
	KEY_Y_GPIO,
	KEY_RIGHT_GPIO,
	KEY_UP_GPIO,
	KEY_LEFT_GPIO,
	KEY_DOWN_GPIO,
};

// keyboard initialize
void KeyInit(void)
{
	int i;

	// initialize rows - inputs with pull-ups
	for (i = 0; i < KEY_NUM; i++)
	{
		GPIO_Func(KeyGPIOList[i], GPIO_FUNC_IN);
		GPIO_Pull(KeyGPIOList[i], GPIO_PULL_UP);
	}

	// clear key map
	memset((void*)KeyMap, 0, sizeof(KeyMap));
	memset((void*)KeyRelMap, 0, sizeof(KeyRelMap));

	// keyboard buffer
	KeyRetBuf = NOKEY;	// returned key to keyboard buffer (NOKEY = no key)
	KeyWriteOff = 0;	// write offset to keyboard buffer
	KeyReadOff = 0;		// read offset from keyboard buffer

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

	// terminate pins
	for (i = 0; i < KEY_NUM; i++)
	{
		GPIO_Func(KeyGPIOList[i], GPIO_FUNC_IN);
		GPIO_Pull(KeyGPIOList[i], (KeyGPIOList[i] >= 9) ? GPIO_PULL_DOWN : GPIO_PULL_UP);
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

	// key map
	int key = 0;
	volatile u8* m = KeyMap;		// key pressed map
	volatile u8* r = KeyRelMap;		// key released map

	// scan keys
	for (; key < KEY_NUM; key++)
	{
		// load maps
		int mm = *m;	// load pressed map
		int rr = *r;	// load released map

		// check if key is pressed
		if (GPIO_In(KeyGPIOList[key]) == 0)
		{
			// first press
			if (mm == 0)
			{
				mm = KEY_REP_TIME1; // first press time
				KeyWriteKey(key);
			}

			// key is already pressed - check repeat interval
			else
			{
				// press counter
				mm--;
				if (mm == 0)
				{
					mm = KEY_REP_TIME2; // repeat press time
					KeyWriteKey(key);
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
				}
			}
		}

		// update map
		*m = mm;
		*r = rr;

		// next key
		m++;
		r++;
	}
}

// get scan code from keyboard buffer (returns NOKEY if no scan code)
int KeyGet()
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
