
// ****************************************************************************
//
//                           BarePi keyboard CalcKey
//
// ****************************************************************************

#include "../../includes.h"	// includes

#if USE_CALCKEY		// 1=use BarePi CalcKey, 0=not used (drv_calckey.*)

// keyboard is initialized
Bool CalcKeyIsInit = False;	// keyboard is initialized
u32 CalcKeyTime;		// last time of CalcKey service

// CalcKey message
sI2Cmsg CalcKeyWriteMsg;	// CalcKey write message
sI2Cmsg CalcKeyReadMsg;		// CalcKey read message
const u8 CalcKeyAddr[2] = { 0, 12 }; // CalcKey index of keyboard buffer
#define CALCKEY_BUF_LEN	4	// length of CalcKey key buffer
u8 CalcKeyBufNew[CALCKEY_BUF_LEN]; // CalcKey key read buffer
u8 CalcKeyBufOld[CALCKEY_BUF_LEN]; // old CalcKey key buffer
Bool CalcKeyWriteDir = True;	// CalcKey write direction (send data address)

// key map
volatile Bool CalcKeyMap[CALCKEY_NUM];	// key pressed flags
volatile u32 CalcKeyPressTime[CALCKEY_NUM]; // time of next press
int CalcKey2nd = CALCKEY_2ND_1;		// 2nd flag CALCKEY_2ND_*

// keyboard buffer
u8 CalcKeyBuf[CALCKEYBUF_SIZE]; 	// keyboard buffer
u8 CalcKeyRetBuf = CALC_NOKEY;		// returned key to keyboard buffer (CALC_NOKEY = no key)
volatile int CalcKeyWriteOff = 0;	// write offset to keyboard buffer
volatile int CalcKeyReadOff = 0;	// read offset from keyboard buffer

// remap numeric keys
const u8 CalcRemapDig[16] = {
	0x92, 	// '0'
	0x82, 	// '1'
	0x83, 	// '2'
	0x84, 	// '3'
	0x72, 	// '4'
	0x73, 	// '5'
	0x74, 	// '6'
	0x62, 	// '7'
	0x63, 	// '8'
	0x64, 	// '9'
	0x7b,	// '0A'
	0x7c,	// '0B'
	0x7d,	// '0C'
	0x6b,	// '0D'
	0x6c,	// '0E'
	0x6d,	// '0F'
};

// key names for display
const char* CalcDispName[0xa0] = {
	"0",		// 0x00 digit 0
	"1",		// 0x01 digit 1
	"2",		// 0x02 digit 2
	"3",		// 0x03 digit 3
	"4",		// 0x04 digit 4
	"5",		// 0x05 digit 5
	"6",		// 0x06 digit 6
	"7",		// 0x07 digit 7
	"8",		// 0x08 digit 8
	"9",		// 0x09 digit 9
	"0A",		// 0x0a digit 0A
	"0B",		// 0x0b digit 0B
	"0C",		// 0x0c digit 0C
	"0D",		// 0x0d digit 0D
	"0E",		// 0x0e digit 0E
	"0F",		// 0x0f digit 0F

	"E'",		// 0x10 label E'
	"A",		// 0x11 label A
	"B",		// 0x12 label B
	"C",		// 0x13 label C
	"D",		// 0x14 label D
	"E",		// 0x15 label E
	"A'",		// 0x16 label A'
	"B'",		// 0x17 label B'
	"C'",		// 0x18 label C'
	"D'",		// 0x19 label D'
	"A''",		// 0x1a label A''
	"B''",		// 0x1b label B''
	"C''",		// 0x1c label C''
	"D''",		// 0x1d label D''
	"E''",		// 0x1e label E''
	"F",		// 0x1f label F

	"OFF",		// 0x20 OFF
	"2nd",		// 0x21 2nd
	"INV",		// 0x22 INV
	"ln x",		// 0x23 Ln x
	"CE",		// 0x24 CE
	"CLR",		// 0x25 CLR
	"SBR Ind",	// 0x26 SBR Ind
	"HIR Ind",	// 0x27 HIR Ind
	"log",		// 0x28 log
	"CP",		// 0x29 CP
	"3rd",		// 0x2a 3rd
	"code",		// 0x2b code
	"log2",		// 0x2c log2
	"rand",		// 0x2d rand
	"",		// 0x2e
	"",		// 0x2f

	"tan",		// 0x30 tan
	"LRN",		// 0x31 LRN
	"x<->t",	// 0x32 x<>t
	"x^2",		// 0x33 x^2
	"Vx",		// 0x34 Vx
	"1/x",		// 0x35 1/x
	"Pgm",		// 0x36 Pgm
	"P->R",		// 0x37 P->R
	"sin",		// 0x38 sin
	"cos",		// 0x39 cos
	"Temp",		// 0x3a Temp
	"x<->y",	// 0x3b x<>y
	"sinh",		// 0x3c sinh
	"cosh",		// 0x3d cosh
	"tanh",		// 0x3e tanh
	"",		// 0x3f

	"Ind",		// 0x40 Ind
	"SST",		// 0x41 SST
	"STO",		// 0x42 STO
	"RCL",		// 0x43 RCL
	"SUM",		// 0x44 SUM
	"y^x",		// 0x45 y^x
	"Ins",		// 0x46 Ins
	"CMs",		// 0x47 CMs
	"Exc",		// 0x48 Exc
	"Prd",		// 0x49 Prd
	"Bat",		// 0x4a BAT
	"n!",		// 0x4b n!
	"ln n!",	// 0x4c ln n!
	"log n!",	// 0x4d log n!
	"mod2",		// 0x4e mod2 (floor)
	"",		// 0x4f

	"|x|",		// 0x50 |x|
	"BST",		// 0x51 BST
	"EE",		// 0x52 EE
	"(",		// 0x53 (
	")",		// 0x54 )
	":",		// 0x55 :
	"Del",		// 0x56 Del
	"Eng",		// 0x57 Eng
	"Fix",		// 0x58 Fix
	"Int",		// 0x59 Int
	"LCD",		// 0x5a LCD
	"<<",		// 0x5b <<
	">>",		// 0x5c >>
	"round",	// 0x5d round
	"mod",		// 0x5e mod (trunc)
	"",		// 0x5f

	"Deg",		// 0x60 Deg
	"GTO",		// 0x61 GTO
	"Pgm Ind",	// 0x62 Pgm Ind
	"Exc Ind",	// 0x63 Exc Ind
	"Prd Ind",	// 0x64 Prd Ind
	"x",		// 0x65 x
	"Pause",	// 0x66 Pause
	"x=t",		// 0x67 x=t
	"Nop",		// 0x68 Nop
	"Op",		// 0x69 Op
	"Rel",		// 0x6a REL
	"Inc Ind",	// 0x6b Inc Ind
	"Reg Ind",	// 0x6c Reg Ind
	"IF Ind",	// 0x6d If Ind
	"AND",		// 0x6e AND &
	"",		// 0x6f

	"Rad",		// 0x70 Rad
	"SBR",		// 0x71 SBR
	"STO Ind",	// 0x72 STO Ind
	"RCL Ind",	// 0x73 RCL Ind
	"SUM Ind",	// 0x74 SUM Ind
	"-",		// 0x75 -
	"Lbl",		// 0x76 Lbl
	"x>=t",		// 0x77 x>=t
	"Stat+",	// 0x78 Stat+
	"Mean",		// 0x79 Avrg x (Mean)
	"IF",		// 0x7a If
	"",		// 0x7b
	"",		// 0x7c
	"",		// 0x7d
	"XOR",		// 0x7e XOR ~
	"",		// 0x7f

	"Grad",		// 0x80 Grad
	"RST",		// 0x81 RST
	"HIR",		// 0x82 HIR
	"GTO Ind",	// 0x83 GTO Ind
	"Op Ind",	// 0x84 Op Ind
	"+",		// 0x85 +
	"St flg",	// 0x86 St Flg
	"If flg",	// 0x87 If Flg
	"D.MS",		// 0x88 D.MS
	"pi",		// 0x89 pi
	"Reg",		// 0x8a Reg
	"HEX",		// 0x8b HEX
	"BIN",		// 0x8c BIN
	"OCT",		// 0x8d OCT
	"OR",		// 0x8e OR |
	"",		// 0x8f

	"List",		// 0x90 List
	"R/S",		// 0x91 R/S
	"RTN",		// 0x92 RTN (Inv SBR)
	".",		// 0x93 .
	"+/-",		// 0x94 +/-
	"=",		// 0x95 =
	"Write",	// 0x96 Write
	"Dsz",		// 0x97 Dsz
	"Adv",		// 0x98 Adv
	"Prt",		// 0x99 Prt
	"phi",		// 0x9a phi
	"DEC",		// 0x9b DEC
	"Inc",		// 0x9c Inc
	"NOT",		// 0x9d NOT
	"%",		// 0x9e %
	"",		// 0x9f
};

// keyboard initialize
// - Must be initialized after initialization of the I2C bus driver.
void CalcKeyInit(void)
{
	int i;
	u8 gpio;

	// clear key map
	memset((void*)CalcKeyMap, False, sizeof(CalcKeyMap));

	// keyboard buffer
	CalcKeyRetBuf = CALC_NOKEY;	// returned key to keyboard buffer (CALC_NOKEY = no key)
	CalcKeyWriteOff = 0;		// write offset to keyboard buffer
	CalcKeyReadOff = 0;		// read offset from keyboard buffer
	memset(CalcKeyBufOld, 0, sizeof(CalcKeyBufOld)); // clear old keyboard buffer

	// prepare CalcKey write message (send data address)
	sI2Cmsg* msg = &CalcKeyWriteMsg; // write message
	msg->callback = NULL;		// no callback
	msg->data = (u8*)CalcKeyAddr;	// pointer to message data
	msg->len = 2;			// length of message data
	msg->data2 = NULL;		// no data part 2
	msg->len2 = 0;			// no data part 2
	msg->speed = CALCKEY_SPEED;	// transfer speed in Hz
	msg->addr = CALCKEY_ADDR;	// device address
	msg->read = False;		// write direction

	// prepare CalcKey read message (read key buffer)
	msg = &CalcKeyReadMsg;		// read message
	msg->callback = NULL;		// no callback
	msg->data = CalcKeyBufNew;	// pointer to message data
	msg->len = CALCKEY_BUF_LEN;	// length of message data
	msg->data2 = NULL;		// no data part 2
	msg->len2 = 0;			// no data part 2
	msg->speed = CALCKEY_SPEED;	// transfer speed in Hz
	msg->addr = CALCKEY_ADDR;	// device address
	msg->read = True;		// read direction

	// send next request - write address
	CalcKeyWriteDir = True;		// CalcKey write direction (send data address)
	I2Cbus_Add(CALCKEY_I2C, &CalcKeyWriteMsg);
	CalcKeyTime = Time();

	// keyboard is initialized
	cb();
	CalcKeyIsInit = True;
}

// keyboard terminate
void CalcKeyTerm(void)
{
	int i;
	u8 gpio;

	// keyboard is not initialized
	CalcKeyIsInit = False;
	cb();

	// flush keyboard buffer
	CalcKeyFlush();
}

// write key to keyboard buffer
void CalcKeyWriteKey(u8 key)
{
	// get current and next write offset
	int w = CalcKeyWriteOff; // current write offset
	int w2 = w + 1;		// next write offset
	if (w2 >= CALCKEYBUF_SIZE) w2 = 0; // wrap
	dmb();

	// check empty space
	if (w2 != CalcKeyReadOff)
	{
		// write key
		CalcKeyBuf[w] = key;
		dmb();
		CalcKeyWriteOff = w2;
	}
}

// check if key is in keyboard buffer
Bool CalcKeyIsInKeyBuf(u8* keybuf, u8 key)
{
	return ((keybuf[0] == key) ||
		(keybuf[1] == key) ||
		(keybuf[2] == key) ||
		(keybuf[3] == key));
}

// scan keyboard (called from SysTick handler)
void CalcKeyScan(void)
{
	u8 key;

	// check if keyboard is initialized
	if (!CalcKeyIsInit) return;

	// check CalcKey service
	if ((u32)(Time() - CalcKeyTime) >= CALCKEY_TIME*1000)
	{
		CalcKeyTime = Time();

		// process last request - read keyboard buffer
		if (!CalcKeyWriteDir) // was read direction
		{
			// not done yet
			if (!I2Cbus_IsDone(&CalcKeyReadMsg)) return;

			// if OK
			if (CalcKeyReadMsg.status == I2CMSG_DONE)
			{
				// process keys
				int i;
				for (i = 0; i < CALCKEY_BUF_LEN; i++)
				{
					// key is pressed
					key = CalcKeyBufNew[i];
					if ((key >= 0x11) && (key <= 0x95))
					{
						// flag - key is pressed
						CalcKeyMap[key] = True;

						// check if key is first pressed
						if (!CalcKeyIsInKeyBuf(CalcKeyBufOld, key))
						{
							CalcKeyPressTime[key] = Time() + CALCKEY_REP_TIME1*1000; // time of next press
							CalcKeyWriteKey(key);		// write key to buffer
						}

						// key was pressed
						else
						{
							// 2nd and INV does not repeat
							if ((key != CALCKEY_2ND) && (key != CALCKEY_INV))
							{
								// check press time
								if ((s32)(Time() - CalcKeyPressTime[key]) >= 0)
								{
									CalcKeyPressTime[key] = Time() + CALCKEY_REP_TIME2*1000; // time of next press
									CalcKeyWriteKey(key);		// write key to buffer
								}
							}
						}
					}
				}

				// process old keys
				for (i = 0; i < CALCKEY_BUF_LEN; i++)
				{
					// key was pressed
					key = CalcKeyBufOld[i];
					if ((key >= 0x11) && (key <= 0x95))
					{
						// check if key is now not pressed
						if (!CalcKeyIsInKeyBuf(CalcKeyBufNew, key))
						{
							// flag - key is not pressed
							CalcKeyMap[key] = False;
						}
					}
				}

				// backup current key buffer
				memcpy(CalcKeyBufOld, CalcKeyBufNew, sizeof(CalcKeyBufOld));
			}

			// send next request - write address
			CalcKeyWriteDir = True;
			I2Cbus_Add(CALCKEY_I2C, &CalcKeyWriteMsg);
		}

		// process last request - write address
		else
		{
			// not done yet
			if (!I2Cbus_IsDone(&CalcKeyWriteMsg)) return;

			// error - repeat last write request
			if (CalcKeyWriteMsg.status != I2CMSG_DONE)
			{
				I2Cbus_Add(CALCKEY_I2C, &CalcKeyWriteMsg);
			}

			// send next request - read keyboard buffer
			else
			{
				CalcKeyWriteDir = False;
				I2Cbus_Add(CALCKEY_I2C, &CalcKeyReadMsg);
			}
		}
	}
}

// get raw key code from keyboard buffer (returns CALC_NOKEY if no key code)
u8 CalcKeyGetRaw()
{
	// get key from temporary 1-key buffer
	u8 key = CalcKeyRetBuf;
	if (key != CALC_NOKEY)
	{
		CalcKeyRetBuf = CALC_NOKEY;
		return key;
	}

	// no key, buffer is empty
	int r = CalcKeyReadOff;
	dmb();
	if (r == CalcKeyWriteOff) return CALC_NOKEY;

	// get key
	key = CalcKeyBuf[r];

	// increase read offset
	r++;
	if (r >= CALCKEYBUF_SIZE) r = 0;
	CalcKeyReadOff = r;

	return key;
}

// get key code from keyboard buffer (returns CALC_NOKEY if no key code)
u8 CalcKeyGet(void)
{
	u8 key;
	int k;

	// get all keys
	for (;;)
	{
		// get key
		key = CalcKeyGetRaw();
		if (key == CALC_NOKEY) return CALC_NOKEY;

		// 2nd
		if (key == CALCKEY_2ND)
		{
			CalcKey2nd++;
			if (CalcKey2nd > CALCKEY_2ND_3) CalcKey2nd = CALCKEY_2ND_1;
			continue;
		}

		// remap 2nd
		if (CalcKey2nd == CALCKEY_2ND_2)
		{
			CalcKey2nd = CALCKEY_2ND_1;	// reset 2nd flag
			k = key & 0xf0;			// row of key
			key &= 0x0f;			// column of key
			key += 5;			// shift column
			if (key >= 10) key = 0;		// change column 10 to 0
			key |= k;			// restore row
		}

		// remap 3rd
		else if (CalcKey2nd == CALCKEY_2ND_3)
		{
			CalcKey2nd = CALCKEY_2ND_1;	// reset 3rd flag
			k = key & 0xf0;			// row of key
			key &= 0x0f;			// column of key
			key += 9;			// shift column
			key |= k;			// restore row
		}
		
		// remap HIR
		if (key == 0x27) return 0x82;

		// remap numeric digits
		for (k = 0; k < 16; k++)
		{
			if (CalcRemapDig[k] == key)
			{
				key = k;
				break;
			}
		}

		return key;
	}
}

// flush keyboard buffer
void CalcKeyFlush()
{
	CalcKeyReadOff = CalcKeyWriteOff;
	CalcKeyRetBuf = CALC_NOKEY;
}

// return key to keyboard buffer (can hold only 1 key)
void CalcKeyRet(u8 key)
{
	CalcKeyRetBuf = key;
}

// check no pressed key
Bool CalcKeyNoPressed()
{
	int i;
	for (i = 0; i < CALCKEY_NUM; i++) if (CalcKeyMap[i]) return False;
	return True;
}

// wait for no key pressed
void CalcKeyWaitNoPressed()
{
	while (!CalcKeyNoPressed()) {}
}

#endif // USE_CALCKEY
