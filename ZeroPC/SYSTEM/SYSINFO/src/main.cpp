
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

// colors
#define COL_BG		COL_BLUE	// background color
#define COL_FRAME	COL_YELLOW	// frame color
#define COL_TEXT	COL_WHITE	// text color

// text of file list
//#define FONTW		8	// font width
//#define FONTH		16	// font height
#define TEXTW		(WIDTH/FONTW) // number of characters per row (=640/8=80)
#define TEXTH		(HEIGHT/FONTH) // number of rows (=480/16=30)

// home path and filename
#define PATHMAX		240	// max. length of path
char Path[PATHMAX+1];	// current path (with terminating 0)
int PathLen;		// length of path
char FileName[8+1];	// filename

// display
u32 FgCol; // foreground color
int DispXNext, DispYNext; // X and Y text coordinate of next frame
int DispX, DispY; // X and Y text coordinate (in characters)

// display text with length
void DispTextLen(const char* text, int len)
{
	DrawTextBgLen(text, len, DispX*FONTW, DispY*FONTH, FgCol, COL_BG);
	DispX += len;
}

// display text
void DispText(const char* text)
{
	DispTextLen(text, StrLen(text));
}

// print character
void DispChar(char ch)
{
	DrawCharBg(ch, DispX*FONTW, DispY*FONTH, FgCol, COL_BG);
	DispX++;
}

// print space
void DispSpc()
{
	DispChar(' ');
}

// display character repeated
void DispCharRep(char ch, int num)
{
	for (; num > 0; num--) DispChar(ch);
}

// display space repeated
void DispSpcRep(int num)
{
	for (; num > 0; num--) DispSpc();
}

// display frame with text
void DispFrame(const char* title, int h)
{
	// setup coordinates
	int x = DispXNext;
	int y = DispYNext;
	if (y >= TEXTH)
	{
		y = 0;
		x += TEXTW/2;
		DispXNext = x;
	}

	// clear background
	DrawRect(x*FONTW, y*FONTH, TEXTW/2*FONTW, h*FONTH, COL_BG);

	// draw frame
	DrawFrame(x*FONTW+FONTW/2, y*FONTH+FONTH/2, (TEXTW/2-1)*FONTW, (h-1)*FONTH, COL_FRAME, COL_FRAME);

	// draw title
	DispX = x + 2;
	DispY = y;
	FgCol = COL_FRAME;
	DispSpc();
	DispText(title);
	DispSpc();

	// prepare to draw text
	DispX = x + 1;
	DispY = y + 1;
	FgCol = COL_TEXT;

	// set next frame
	DispYNext = y + h;
}

// next row in current frame
void DispNL()
{
	DispX = DispXNext + 1;
	DispY++;
}

// Board
void FrameBoard()
{
	int i;

	DispFrame("Board", 8);

	DispText("       Model: 0x");
	DecHexNum(DecNumBuf, BoardModel, 8);
	DispText(DecNumBuf);
	DispNL();

	DispText("    Revision: 0x");
	DecHexNum(DecNumBuf, BoardRevision, 8);
	DispText(DecNumBuf);
	DispNL();

	DispText("       Board: ");
	DispText(BoardModelName);
	DispText(" rev.");
	DispChar(BoardModelRev + '0');
	DispNL();

	DispText("Manufacturer: ");
	DispText(BoardSocManuName);
	DispNL();

	DispText("      Serial: 0x");
	DecHexNum(DecNumBuf, (u32)(BoardSerial >> 32), 8);
	DispText(DecNumBuf);
	DispChar('-');
	DecHexNum(DecNumBuf, (u32)BoardSerial, 8);
	DispText(DecNumBuf);
	DispNL();

	DispText(" MAC Address: ");
	for (i = 0; i < MACADDR_LEN; i++)
	{
		if (i != 0) DispChar(':');
		DecHexNum(DecNumBuf, MACAddress[i], 2);
		DispText(DecNumBuf);
	}
}

// CPU
void FrameCPU()
{
	DispFrame("Chip and CPU", 10);

	DispText("    SoC chip: ");
	DispText(BoardSocName);
	DispNL();

	DispText("         GPU: ");
	DispText(GpuName);
	DispNL();

	DispText(" Max Display: ");
	DispText(GpuMaxResName );
	DispNL();

	DispText("         CPU: ");
	DispText(ArmCpuTypeName);
	DispNL();

	DispText("       Cores: ");
	DecUNum(DecNumBuf, CORES, 0);
	DispText(DecNumBuf);
	DispNL();

	DispText("Architecture: ");
	DispText(ArmArchTypeName);
	DispNL();

	DispText("     RUNMODE: ");
	DispChar(RUNMODE + '0');
	DispNL();

	DispText("       AARCH: ");
#if AARCH==32
	DispText("32-bit");
#else
	DispText("64-bit");
#endif
	DispNL();
}

// Firmware
void FrameFirmware()
{
	DispFrame("Firmware", 5);

	DispText("       Build: ");
	DecUNum(DecNumBuf, FirmwareVersion, '\'');
	DispText(DecNumBuf);
	DispNL();

	DispText("   Build Hex: 0x");
	DecHexNum(DecNumBuf, FirmwareVersion, 8);
	DispText(DecNumBuf);
	DispNL();

	DispText("  mm/dd/yyyy: ");
	sDateTime dt;
	DateTimeUnpack(&dt, FirmwareVersion, 0, 0);
	DecUNum(DecNumBuf, dt.mon, 0);
	DispText(DecNumBuf);
	DispChar('/');
	DecUNum(DecNumBuf, dt.day, 0);
	DispText(DecNumBuf);
	DispChar('/');
	DecUNum(DecNumBuf, dt.year, 0);
	DispText(DecNumBuf);
	DispNL();
}

// memory
void FrameMemory()
{
	DispFrame("Memory", 7);

	DispText("      Memory: ");
	DecUNum(DecNumBuf, BoardMemSize, 0);
	DispText(DecNumBuf);
	DispText(" MB");
	DispNL();

	DispText(" ARM Address: 0x");
	DecHexNum(DecNumBuf, ARMMemAddr, 8);
	DispText(DecNumBuf);
	DispNL();

	DispText("    ARM Size: 0x");
	DecHexNum(DecNumBuf, ARMMemSize, 8);
	DispText(DecNumBuf);
	DispText(" (");
	DecUNum(DecNumBuf, ARMMemSize/MEGABYTE, 0);
	DispText(DecNumBuf);
	DispText(" MB)");
	DispNL();

	DispText("  VC Address: 0x");
	DecHexNum(DecNumBuf, VCMemAddr, 8);
	DispText(DecNumBuf);
	DispNL();

	DispText("     VC Size: 0x");
	DecHexNum(DecNumBuf, VCMemSize, 8);
	DispText(DecNumBuf);
	DispText(" (");
	DecUNum(DecNumBuf, VCMemSize/MEGABYTE, 0);
	DispText(DecNumBuf);
	DispText(" MB)");
	DispNL();
}

// Clock
void FrameClock()
{
	DispFrame("Frequency", 11);

	DispText("     Crystal: ");
#if RASPPI >= 4
	DispText("54 MHz");
#else
	DispText("19.2 MHz");
#endif
	DispNL();

	DispText("   ARM Clock: ");
	DecUNum(DecNumBuf, (ARMClock+500000)/1000000, '\'');
	DispText(DecNumBuf);
	DispText(" MHz");
	DispNL();

	DispText("  Core Clock: ");
	DecUNum(DecNumBuf, (CoreClock+500000)/1000000, '\'');
	DispText(DecNumBuf);
	DispText(" MHz");
	DispNL();

	DispText("  EMMC Clock: ");
	DecUNum(DecNumBuf, (EMMCClock+500000)/1000000, '\'');
	DispText(DecNumBuf);
	DispText(" MHz");
	DispNL();

	DispText("  UART Clock: ");
	DecUNum(DecNumBuf, (UARTClock+500000)/1000000, '\'');
	DispText(DecNumBuf);
	DispText(" MHz");
	DispNL();

	DispText("   V3D Clock: ");
	DecUNum(DecNumBuf, (V3DClock+500000)/1000000, '\'');
	DispText(DecNumBuf);
	DispText(" MHz");
	DispNL();

	DispText("  H264 Clock: ");
	DecUNum(DecNumBuf, (H264Clock+500000)/1000000, '\'');
	DispText(DecNumBuf);
	DispText(" MHz");
	DispNL();

	DispText("   ISP Clock: ");
	DecUNum(DecNumBuf, (ISPClock+500000)/1000000, '\'');
	DispText(DecNumBuf);
	DispText(" MHz");
	DispNL();

	DispText(" SDRAM Clock: ");
	DecUNum(DecNumBuf, (SDRAMClock+500000)/1000000, '\'');
	DispText(DecNumBuf);
	DispText(" MHz");
	DispNL();
}

// SD card
void FrameSD()
{
	DiskAutoMount();
	PathLen = GetHomePath(Path, PATHMAX, FileName);

	DispFrame("SD Card", 10);

	DispText("   Card Type: ");
	DispText(SD_TypeName[SD_Type]);
	DispNL();

	DispText("    Card RCA: 0x");
	DecHexNum(DecNumBuf, SD_CardRCA, 4);
	DispText(DecNumBuf);
	DispNL();

	DispText("  Media Size: ");
	DecUNum(DecNumBuf, (SD_MediaSize + 1024) / (2*1024), '\'');
	DispText(DecNumBuf);
	DispText(" MB");
	DispNL();

	DispText(" File System: ");
	DispText(DiskFATName());
	DispNL();

	DispText("     Cluster: ");
	DecUNum(DecNumBuf, DiskClustSize(), '\'');
	DispText(DecNumBuf);
	DispNL();

	DispText("      Serial: 0x");
	DecHexNum(DecNumBuf, GetDiskSerial(), 8);
	DispText(DecNumBuf);
	DispNL();

	DispText("   Home Path: ");
	if (PathLen > 20)
	{
		DispText("...");
		DispText(&Path[PathLen - 20]);
	}
	else
		DispText(Path);
	DispNL();

	DispText("   File Name: ");
	DispText(FileName);
	DispText(".IMG");
	DispNL();
}

// Miscellaneous
void FrameMisc()
{
	DispFrame("Miscellaneous", 9);

	DispText(" Temperature: ");
	u32 temp = GetCoreTemp();
	MemPrint(DecNumBuf, 10, "%.3f", temp*0.001);
	DispText(DecNumBuf);
	DispText(" \02C");
	DispNL();

	DispText("Core Voltage: ");
	MemPrint(DecNumBuf, 10, "%.3f", CoreVoltage*0.000001);
	DispText(DecNumBuf);
	DispText(" mV");
	DispNL();

	DispText("     SDRAM-C: ");
	MemPrint(DecNumBuf, 10, "%.3f", SDRAMCVoltage*0.000001);
	DispText(DecNumBuf);
	DispText(" mV");
	DispNL();

	DispText("     SDRAM-P: ");
	MemPrint(DecNumBuf, 10, "%.3f", SDRAMPVoltage*0.000001);
	DispText(DecNumBuf);
	DispText(" mV");
	DispNL();

	DispText("     SDRAM-I: ");
	MemPrint(DecNumBuf, 10, "%.3f", SDRAMIVoltage*0.000001);
	DispText(DecNumBuf);
	DispText(" mV");
	DispNL();

	DispText("Frame Buffer: 0x");
	DecHexNum(DecNumBuf, (u32)(size_t)FrameBuffer.screenbuf, 8);
	DispText(DecNumBuf);
	DispNL();

	DispText("   Videomode: ");
	DecUNum(DecNumBuf, FrameBuffer.screenwidth, 0);
	DispText(DecNumBuf);
	DispText(" x ");
	DecUNum(DecNumBuf, FrameBuffer.screenheight, 0);
	DispText(DecNumBuf);
	DispText(", 32-bit");	
	DispNL();
}

int main()
{
	int key;

	// Left column
	DispXNext = 0;
	DispYNext = 0;

	FrameBoard();		// 8
	FrameFirmware();	// 5
	FrameMemory();		// 7
	FrameSD();		// 10

	// Right column
	DispXNext = TEXTW/2;
	DispYNext = 0;

	FrameCPU();		// 10
	FrameClock();		// 11
	FrameMisc();		// 9

	// display update
	DispUpdate();

	// wait for Esc key
	while (True)
	{
		key = KeyGet();

		// Program exit
		if (key == KEY_ESC) break;

		//  Screenshot - This may take a few seconds to write.
		if (key == KEY_SCREENSHOT) ScreenShot();
	}
}
