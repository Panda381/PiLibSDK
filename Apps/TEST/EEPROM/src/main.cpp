
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************
// 0x50: data EEPROM 32KB AT24C256
// 0x57: configuration EEPROM 4KB AT24C32

#include "../include.h"

#define DOWRITE		0		// 1=do test write
#define DOCLEAR		0		// 1=clear EEPROMs with pattern
#define DOFORMAT	0		// 1=format EEPROM to config 'BarePi' storage format
#define DODETECT	1		// 1=detect EEPROM from configuration header
#define LOADCFG		0		// 1=load test config
#define SAVECFG		0		// 1=save test config

#define CLEAR_BYTE	0xff		// byte to clear EEPROMs

#define EEPROM1_ADDR	0x50		// EEPROM1 address
#define EEPROM1_MODEL	EEPROM_MODEL_32K // EEPROM1 model
#define EEPROM1_SIZE	(32*1024)	// EEPROM1 size
#define EEPROM1_SPEED	100000		// EEPROM1 speed

#define EEPROM2_ADDR	0x57		// EEPROM2 address
#define EEPROM2_MODEL	EEPROM_MODEL_4K // EEPROM2 model
#define EEPROM2_SIZE	(4*1024)	// EEPROM2 size
#define EEPROM2_SPEED	100000		// EEPROM2 speed

// EEPROM
cEEPROM EEPROM1;	// EEPROM 32KB
cEEPROM EEPROM2;	// EEPROM 4KB

u8 Buf1[EEPROM1_SIZE];
u8 Buf2[EEPROM2_SIZE];

#define CFG1ID	0x12
#define CFG1LEN	4
u32 Cfg1 = 0x87654321;

#define CFG2ID	0x13
#define CFG2LEN	4
u32 Cfg2 = 0x12345678;

// keyboard
void Keyboard()
{
	// keyboard
	int key = KeyGet();
	if (key == KEY_PAD_Y) Reboot();	// Program exit
	if (key == KEY_SCREENSHOT) ScreenShot(); //  Screenshot - This may take a few seconds to write.
	if (key == KEY_ZOOM) LCDRezoom();	// LCD display rezoom
}

// display memory (n = number of rows)
void DispMem(const u8* buf, int n)
{
	int i;
	int off = 0;
	u8 ch;
	for (; n > 0; n--)
	{
		printf("%04X:", off);
		for (i = 0; i < 16; i++)
		{
			printf((i == 8) ? "-" : " ");
			printf("%02X", buf[off+i]);
		}
		printf(" ");

		for (i = 0; i < 16; i++)
		{
			ch = buf[off+i];
			if (ch < 32) ch = '.';
			if ((ch >= 128) && (ch < 128+32)) ch = '.' + 128;
			printf("%c", ch);
		}

		printf("\n");
		off += 16;
	}
}

// main function
int main()
{
	int key, step;
	int res;
	u32 t1, t2, seed;

	// initialize or detect EEPROM
#if DODETECT	// 1=detect EEPROM from configuration header
	WaitMs(1000);
	if (EEPROM1.Detect(0, EEPROM1_ADDR, EEPROM1_SPEED))
		printf("EEPROM1: detect %dB\n", EEPROM1.size);
	else
	{
		printf("EEPROM1: cannot detect\n");
		EEPROM1.Init(0, EEPROM1_ADDR, EEPROM1_MODEL, EEPROM1_SPEED);
	}

	if (EEPROM2.Detect(0, EEPROM2_ADDR, EEPROM2_SPEED))
		printf("EEPROM2: detect %dB\n", EEPROM2.size);
	else
	{
		printf("EEPROM2: cannot detect\n");
		EEPROM2.Init(0, EEPROM2_ADDR, EEPROM2_MODEL, EEPROM2_SPEED);
	}
	DispUpdate();
	WaitMs(2000);	
#else
	EEPROM1.Init(0, EEPROM1_ADDR, EEPROM1_MODEL, EEPROM1_SPEED);
	EEPROM2.Init(0, EEPROM2_ADDR, EEPROM2_MODEL, EEPROM2_SPEED);
#endif
	printf("Start reading...");
	DispUpdate();

	// main loop
	step = 0;
	while (True)
	{
		DrawClear();
		step++;
		printf("run %d\n\n", step);

// ----- EEPROM1

		printf("EEPROM1 ");

		// load config
#if LOADCFG // 1=load test config
		res = EEPROM1.LoadCfg(CFG1ID, CFG1LEN, &Cfg1);
		printf("Load(%d)=%08X ", res, Cfg1);
		res = EEPROM1.LoadCfg(CFG2ID, CFG2LEN, &Cfg2);
		printf("Load(%d)=%08X ", res, Cfg2);
#endif

		// save config
#if SAVECFG // 1=save test config
		Cfg1--;
		res = EEPROM1.SaveCfg(CFG1ID, CFG1LEN, &Cfg1);
		printf("Save(%d) ", res);
		Cfg2++;
		res = EEPROM1.SaveCfg(CFG2ID, CFG2LEN, &Cfg2);
		printf("Save(%d) ", res);
#endif

		// write EEPROM1
#if DOWRITE // 1=do test write
		seed = RandU32();
		t1 = Time();
		res = EEPROM1.WritePattern(0, EEPROM1_SIZE, seed);
		t2 = Time();
		printf("Write:%s %dms ", res ? "OK" : "ERR", (t2-t1)/1000);

		Keyboard();

		t1 = Time();
		res = EEPROM1.CheckPattern(0, EEPROM1_SIZE, seed);
		t2 = Time();
		printf("Check:%s %dms ", (res == EEPROM1_SIZE) ? "OK" : "ERR", (t2-t1)/1000);
#endif

		Keyboard();

		// clear EEPROM
#if DOCLEAR // 1=clear EEPROMs with pattern
		t1 = Time();
		res = EEPROM1.Clear(0, EEPROM1_SIZE, CLEAR_BYTE);
		t2 = Time();
		printf("Clear:%s %dms ", res ? "OK" : "ERR", (t2-t1)/1000);

		Keyboard();
#endif

		// format EEPROM
#if DOFORMAT // 1=format EEPROM to config 'BarePi' storage format
		t1 = Time();
		res = EEPROM1.Format();
		t2 = Time();
		printf("Format:%s %dms ", res ? "OK" : "ERR", (t2-t1)/1000);

		Keyboard();
#endif

		// read EEPROM1
		t1 = Time();
		res = EEPROM1.Read(0, Buf1, EEPROM1_SIZE);
		t2 = Time();
		printf("Read:%s %dms", res ? "OK" : "ERR", (t2-t1)/1000);
#if DOWRITE // 1=do test write
		res = EEPROM1.VerifyPattern(Buf1, EEPROM1_SIZE, seed);
		printf(" Verify:%s", (res == EEPROM1_SIZE) ? "OK" : "ERR");
#endif
		printf("\n");
		DispMem(Buf1, 12);
		printf("\n");

		Keyboard();

// ----- EEPROM1
		printf("EEPROM2 ");

		// load config
#if LOADCFG // 1=load test config
		res = EEPROM2.LoadCfg(CFG1ID, CFG1LEN, &Cfg1);
		printf("Cfg(%d)=%08X ", res, Cfg1);
		res = EEPROM2.LoadCfg(CFG2ID, CFG2LEN, &Cfg2);
		printf("Cfg(%d)=%08X ", res, Cfg2);
#endif

		// save config
#if SAVECFG // 1=save test config
		Cfg1--;
		res = EEPROM2.SaveCfg(CFG1ID, CFG1LEN, &Cfg1);
		printf("Save(%d) ", res);
		Cfg2++;
		res = EEPROM2.SaveCfg(CFG2ID, CFG2LEN, &Cfg2);
		printf("Save(%d) ", res);
#endif

		// write EEPROM2
#if DOWRITE // 1=do test write
		seed = RandU32();
		t1 = Time();
		res = EEPROM2.WritePattern(0, EEPROM2_SIZE, seed);
		t2 = Time();
		printf("Write:%s %dms ", res ? "OK" : "ERR", (t2-t1)/1000);

		Keyboard();

		t1 = Time();
		res = EEPROM2.CheckPattern(0, EEPROM2_SIZE, seed);
		t2 = Time();
		printf("Check:%s %dms ", (res == EEPROM2_SIZE) ? "OK" : "ERR", (t2-t1)/1000);
#endif

		Keyboard();

		// clear EEPROM
#if DOCLEAR // 1=clear EEPROMs with pattern
		t1 = Time();
		res = EEPROM2.Clear(0, EEPROM2_SIZE, CLEAR_BYTE);
		t2 = Time();
		printf("Clear:%s %dms ", res ? "OK" : "ERR", (t2-t1)/1000);

		Keyboard();
#endif

		// format EEPROM
#if DOFORMAT // 1=format EEPROM to config 'BarePi' storage format
		t1 = Time();
		res = EEPROM2.Format();
		t2 = Time();
		printf("Format:%s %dms ", res ? "OK" : "ERR", (t2-t1)/1000);

		Keyboard();
#endif

		// read EEPROM2
		t1 = Time();
		res = EEPROM2.Read(0, Buf2, EEPROM2_SIZE);
		t2 = Time();
		printf("Read:%s %dms", res ? "OK" : "ERR", (t2-t1)/1000);
#if DOWRITE // 1=do test write
		res = EEPROM2.VerifyPattern(Buf2, EEPROM2_SIZE, seed);
		printf(" Verify:%s", (res == EEPROM2_SIZE) ? "OK" : "ERR");
#endif
		printf("\n");
		DispMem(Buf2, 12);

		// display update
		DispUpdate();

		// keyboard
		Keyboard();
	}
}
