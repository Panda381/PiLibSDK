
// ****************************************************************************
//
//                           BarePi keyboard KeyPad
//
// ****************************************************************************

#include "../../includes.h"	// includes

#if USE_KEYPAD && !USE_MINIKEY	// 1=use BarePi KeyPad, 0=not used (drv_keypad.*)

// keyboard is initialized
Bool KeyIsInit = False;

// key map
volatile Bool KeyMap[KEYPAD_NUM];	// key pressed flags
volatile u32 KeyPressTime[KEYPAD_NUM];	// time of next press
volatile u32 KeyRelTime[KEYPAD_NUM];	// time of last release

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

// keyboard initialize
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
	for (i = 0; i < KEYPAD_NUM; i++) if (KeyMap[i]) return False;
	return True;
}

// wait for no key pressed
void KeyWaitNoPressed()
{
	while (!KeyNoPressed()) {}
}

#endif // USE_KEYPAD && !USE_MINIKEY
