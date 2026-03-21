
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

// text of file list
//#define FONTW		8	// font width
//#define FONTH		16	// font height
#define TEXTW		(WIDTH/FONTW) // number of characters per row (=640/8=80)
#define TEXTH		(HEIGHT/FONTH) // number of rows (=480/16=30)

// kernel configuration file
#define KERNEL_CFG	"KERNEL.CFG"

// files
#define PATHMAX		240	// max. length of path
#define MAXFILES	1000	// max files in one directory
#define NAMELEN		9	// max. length of filename without extension and with terminating 0
#define FILEROWS	(TEXTH-2) // rows of files (=28)
#define FILECOLW	40	// width of file panel (in number of characters)

// file entry descriptor (10 bytes)
//#define ATTR_RO	B0	// Read only
//#define ATTR_HID	B1	// Hidden
//#define ATTR_SYS	B2	// System
//#define ATTR_VOL	B3	// Volume label
//#define ATTR_DIR	B4	// Directory
//#define ATTR_ARCH	B5	// Archive
#define ATTR_TXT	B6	// internal: text file TXT present
#define ATTR_BMP	B7	// internal: bitmap indicator BMP present
#define ATTR_MASK	0x3F	// mask of valid bits

// size of jump trampoline
#define TRAMP_SIZE	1000	// size of trampoline code

// file descriptor
typedef struct {
	u8	attr;		// attributes ATTR_*
	u8	len;		// file name length
	char	name[8];	// file name (without extension and without terminating zero)
	u32	size;		// file size
} sFileDesc;

// colors
#define COL_TITLEFG	COL_BLACK	// title foreground color
#define COL_TITLEBG	COL_WHITE	// title background color
#define COL_FILEFG	COL_CYAN	// file foreground color
#define COL_FILEBG	COL_BLUE	// file background color
#define COL_DIRFG	COL_WHITE	// directory foreground color
#define COL_DIRBG	COL_BLUE	// directory background color
#define COL_CURFG	COL_BLACK	// cursor coreground color
#define COL_CURBG	COL_CYAN	// cursor background color
#define COL_INFOFG	COL_GREEN	// info text foreground color
#define COL_INFOBG	COL_BLUE	// info text background color
#define COL_TEXTBG	COL_BLACK	// text background color
#define COL_TEXTFG	COL_GRAY	// text foreground color
#define COL_BIGINFOFG	COL_YELLOW	// big info text foreground color
#define COL_BIGINFOBG	COL_BLACK	// big info text background color
#define COL_BIGERRFG	COL_YELLOW	// big error foreground color
#define COL_BIGERRBG	COL_RED		// big error background color

// display
u32 FgCol, BgCol; // foreground and background color
int DispX, DispY; // X and Y text coordinate

// temporary buffer
#define TEMPBUF 2048 // size of temporary buffer (min. requires size: WIDTH/2*4 = 1280)
char TempBuf[TEMPBUF+1];
int TempBufNum; // number of bytes in temporary buffer
int TempBufInx; // current index in temporary buffer

// files
char Path[PATHMAX+1]; // current path (with terminating 0)
int PathLen; // length of path
u32 LastMount; // last mount time
Bool Remount; // remount request
sFileDesc FileDesc[MAXFILES]; // file descriptors
int FileNum; // number of files
int FileCur; // index of current file
int FileTop; // index of first visible file
sFile FileF; // search structure
sFileInfo FileI; // search file info

// seek cursor to last name
char LastName[8];
int LastNameLen; // length of last name, 0 = not used
u8 LastNameDir; // ATTR_DIR flag of last name

// preview
enum  {
	PREV_START,	// waiting for start
	PREV_TXT_LOAD,	// loading text
	PREV_BMP_START,	// waiting for bitmap start
	PREV_BMP_LOAD,	// loading bitmap
	PREV_STOP,	// preview not active
};	

int PrevState; // current preview state
int PrevBytes; // number of BMP bytes per pixel: 3 or 4
sFileDesc* PrevFD; // preview file descriptor
int PrevLine; // current preview line
sFile PrevFile; // preview file (name[0] = 0 if not open)
int PrevW, PrevH, PrevWB; // size of preview image

// set home position
void DispHome()
{
	DispX = 0;
	DispY = 0;
}

// set print colors
void DispCol(u32 fgcol, u32 bgcol)
{
	FgCol = fgcol;
	BgCol = bgcol;
}

// display text with length
void DispTextLen(const char* text, int len)
{
	DrawTextBgLen(text, len, DispX*FONTW, DispY*FONTH, FgCol, BgCol);
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
	DrawCharBg(ch, DispX*FONTW, DispY*FONTH, FgCol, BgCol);
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

// display frame of file list
void FrameFileList()
{
	// reset cursor
	DispHome();

	// color
	DispCol(COL_FILEFG, COL_FILEBG);

	// top frame
	DispChar(0x1c); // left frame
	DispCharRep(0x15, FILECOLW-3); // row
	DispChar(0x19); // right frame
	DispY++;

	// inner rows
	do {
		DispX = 0; // reset column
		DispChar(0x1a); // left frame
		DispSpcRep(FILECOLW/2-2); // row
		DispChar(0x1a); // middle frame
		DispSpcRep(FILECOLW/2-2); // row
		DispChar(0x1a); // right frame
		DispY++; // increment row
	} while (DispY != TEXTH-1);

	// bottom frame
	DispX = 0; // reset column
	DispChar(0x16); // left frame
	DispCharRep(0x15, FILECOLW-3); // row
	DispChar(0x13); // right frame
}

// display current path
void PathFileList()
{
	// prepare path length
	int len = PathLen;

	// reset cursor
	DispY = 0;
	DispX = 1;

// len <= FILECOLW-1-4 ... | top_line + space + path + space + top_line |
// len == FILECOLW-1-3 ... | path + space |
// len == FILECOLW-1-2 ... | path |
// len > FILECOLW-1-2 ... | "..." short_path |

	// left part of top line, left space
	if (len <= FILECOLW-1-4)
	{
		// left part of top line
		DispCol(COL_FILEFG, COL_FILEBG);
		DispCharRep(0x15, (FILECOLW-1-4 - len)/2); // line

		// left space
		DispCol(COL_TITLEFG, COL_TITLEBG);
		DispSpc();
	}

	// path
	DispCol(COL_TITLEFG, COL_TITLEBG);
	if (len <= FILECOLW-1-2)
	{
		// full path
		DispText(Path);
	}
	else
	{
		// short path
		DispCharRep('.', 3);
		DispText(Path + len - (FILECOLW-1-2-3));
	}

	// right space
	if (len <= FILECOLW-1-3) DispSpc();
	
	// right part of top line
	if (len <= FILECOLW-1-4)
	{
		// right part of top line
		DispCol(COL_FILEFG, COL_FILEBG);
		DispCharRep(0x15, (FILECOLW-1-4 - len + 1)/2); // line
	}
}

// display current index of selected file
void InxFileList()
{
	if (FileNum == 0) return;

	// prepare text
	int n = DecNum(TempBuf, FileCur+1, 0);
	TempBuf[n++] = '/';
	n += DecNum(&TempBuf[n], FileNum, 0);

	// reset cursor
	DispY = TEXTH-1;
	DispX = 1;

	// left part of bottom line
	DispCol(COL_FILEFG, COL_FILEBG);
	DispCharRep(0x15, (FILECOLW-1-4 - n)/2); // line

	// left space
	DispSpc();

	// text
	DispText(TempBuf);

	// right space
	DispSpc();
	
	// right part of bottom line
	DispCharRep(0x15, (FILECOLW-1-4 - n + 1)/2); // line
}

// display file list
//  "|[12345678] TXT BMP  [12345678] TXT BMP|"
void DispFileList()
{
	// reset cursor
	DispY = 1;

	// display files
	int i, j;
	char ch;
	Bool dir;
	sFileDesc* fd = &FileDesc[FileTop];
	int x0 = 1;
	for (i = 0; i < 2*FILEROWS; i++)
	{
		DispX = x0;

		// set normal color
		DispCol(COL_FILEFG, COL_FILEBG);

		// entry is valid
		j = i + FileTop;
		if (j < FileNum)
		{
			// check directory
			dir = ((fd->attr & ATTR_DIR) != 0);

			// set directory color
			if (dir) DispCol(COL_DIRFG, COL_DIRBG);

			// set cursor color
			if (j == FileCur) DispCol(COL_CURFG, COL_CURBG);

			// directory mark '['
			ch = dir ? '[' : ' ';
			DispChar(ch);

			// decode entry name
			for (j = 0; j < fd->len; j++) DispChar(fd->name[j]);

			// directory mark ']'
			ch = dir ? ']' : ' ';
			DispChar(ch);

			// display spaces
			DispSpcRep(9 - fd->len);

			// TXT mark
			if ((fd->attr & ATTR_TXT) != 0)
			{
				DispText("TXT");
			}
			else
				DispSpcRep(3);

			// space
			DispSpc();

			// BMP mark
			if ((fd->attr & ATTR_BMP) != 0)
			{
				DispText("BMP");
			}
			else
				DispSpcRep(3);
		}
		else
			// clear invalid row
			DispSpcRep(FILECOLW/2-2);

		// increase file
		DispY++;
		if (DispY == TEXTH-1)
		{
			DispY = 1;
			x0 = 20;
		}
		fd++;
	}

	// display current index
	InxFileList();

	// restart preview state
	PrevState = PREV_START;
}

// load files (programs with UF2 extension, directory without extension)
void LoadFileList()
{
	// clear file list
	FileNum = 0;
	FileCur = 0;

	// set current directory
	if (!SetDir(Path)) return;

	// open search 
	if (!FindOpen(&FileF, "")) return;

	// load files
	int inx, i, len;
	u32 siz;
	sFileDesc* fd = FileDesc;
	char ch;
	char* name;
	Bool dir;
	for (inx = 0; inx < MAXFILES; inx++)
	{
		// find next file
		if (!FindNext(&FileF, &FileI, ATTR_DIR_MASK, "*.*")) break;

		// check directory
		dir = (FileI.attr & ATTR_DIR) != 0;

		// skip directory "."
		len = FileI.namelen;
		name = FileI.name;
		if (dir && (len == 1) && (name[0] == '.')) continue;

		// get attributes
		fd->attr = FileI.attr & ATTR_MASK;
		fd->size = 0;

		// copy directory ".."
		if (dir && (len == 2) && (name[0] == '.') && (name[1] == '.'))
		{
			fd->len = len;
			fd->name[0] = '.';
			fd->name[1] = '.';
			fd++;
			FileNum++;
		}
		else
		{
			// skip hidden entry
			if ((FileI.attr & ATTR_HID) != 0) continue;

			// directory
			if (dir)
			{
				// copy directory name (without extension)
				for (i = 0; (i < len) && (i < 8); i++)
				{
					ch = name[i];
					if (ch == '.') break;
					fd->name[i] = ch;
				}

				// directory is valid only if has no extension
				if (i == len)
				{
					fd->len = len;
					fd++;
					FileNum++;
				}
			}

			// file
			else
			{
				// check extension "IMG"
				if ((len > 4) && (name[len-4] == '.') && (name[len-3] == 'I') &&
					 (name[len-2] == 'M') && (name[len-1] == 'G'))
				{
					// copy file name (without extension)
					fd->len = len-4;
					memcpy(fd->name, name, len-4);
					fd->size = FileI.size;
					fd++;
					FileNum++;
				}
			}
		}
	}

	// open search again, to check info files
	if (FindOpen(&FileF, ""))
	{
		// find next file
		while (FindNext(&FileF, &FileI, ATTR_DIR_MASK, "*.*"))
		{
			// skip directory
			if ((FileI.attr & ATTR_DIR) != 0) continue;

			// check extension "TXT"
			len = FileI.namelen;
			if (len > 4)
			{
				len -= 4;

				name = FileI.name;
				if ((name[len] == '.') && (name[len+1] == 'T') &&
					 (name[len+2] == 'X') && (name[len+3] == 'T'))
				{
					// search this file name
					sFileDesc* fd = FileDesc;
					for (inx = 0; inx < FileNum; inx++)
					{
						// compare file name length
						if (fd->len == len)
						{
							// compare file names, set TXT flag
							if (memcmp(fd->name, name, len) == 0) fd->attr |= ATTR_TXT;
						}
						fd++;
					}
				}
				else
				{
					// check extension "BMP"
					if ((name[len] == '.') && (name[len+1] == 'B') &&
						 (name[len+2] == 'M') && (name[len+3] == 'P'))
					{
						// search this file name
						sFileDesc* fd = FileDesc;
						for (inx = 0; inx < FileNum; inx++)
						{
							// compare file name length
							if (fd->len == len)
							{
								// compare file names, set BMP flag
								if (memcmp(fd->name, name, len) == 0) fd->attr |= ATTR_BMP;
							}
							fd++;
						}
					}
				}
			}
		}
	}

	// sort files (using bubble sort)
	fd = FileDesc;
	for (inx = 0; inx < FileNum-1;)
	{
		Bool ok = True;

		// directory '..' must be at first place
		if ((fd[1].len == 2) && (fd[1].name[0] == '.') && (fd[1].name[1] == '.')) ok = False;

		// directory must be before the files
		if (((fd[0].attr & ATTR_DIR) == 0) && ((fd[1].attr & ATTR_DIR) != 0)) ok = False;

		// entry of the same group
		if (((fd[0].attr ^ fd[1].attr) & ATTR_DIR) == 0)
		{
			// compare names
			len = fd[0].len;
			if (fd[1].len < fd[0].len) len = fd[1].len;
			for (i = 0; i < len; i++)
			{
				if (fd[0].name[i] != fd[1].name[i]) break;
			}

			if (i < len) // names are different
			{
				if (fd[0].name[i] > fd[1].name[i]) ok = False;
			}
			else // names are equal, check name lengths
			{
				if (fd[1].len < fd[0].len) ok = False;
			}
		}

		// exchange files
		if (!ok)
		{
			ch = fd[0].attr;
			fd[0].attr = fd[1].attr;
			fd[1].attr = ch;

			ch = fd[0].len;
			fd[0].len = fd[1].len;
			fd[1].len = ch;

			siz = fd[0].size;
			fd[0].size = fd[1].size;
			fd[1].size = siz;

			for (i = 0; i < 8; i++)
			{
				ch = fd[0].name[i];
				fd[0].name[i] = fd[1].name[i];
				fd[1].name[i] = ch;
			}

			// shift index down
			if (inx > 0)
			{
				inx -= 2;
				fd -= 2;
			}
		}

		// shift index up
		inx++;
		fd++;
	}
}

// request to reload current directory
void Reload()
{
	// reset file list
	FileNum = 0; // no entry
	FileCur = 0; // reset cursor
	FileTop = 0; // reset top entry

	// preview state not active
	PrevState = PREV_STOP;

	// display frame of file list
	FrameFileList();

	// display current path
	PathFileList();

	// set flag - disk need to be remounted
	Remount = True;
}

// reset to root
void ResetRoot()
{
	// reset path to root
	Path[0] = PATHCHAR;
	Path[1] = 0;
	PathLen = 1;

	// request to reload current directory
	Reload();
}

// set cursor to last name
void SetLastName()
{
	sFileDesc* fd;
	int i;

	// no last name required
	if (LastNameLen == 0) return;

	// search last name in file list
	for (FileCur = FileNum-1; FileCur > 0; FileCur--)
	{
		fd = &FileDesc[FileCur];
		if ((fd->len == LastNameLen) &&
			((fd->attr & ATTR_DIR) == LastNameDir) &&
			(memcmp(LastName, fd->name, LastNameLen) == 0)) break;
	}

	// limit max. top entry
	if (FileTop > FileCur) FileTop = FileCur;

	// limit min. top entry
	i = FileCur - (FILEROWS-1);
	if (i < 0) i = 0;
	if (FileTop < i) FileTop = i;

	// delele last name request
	LastNameLen = 0;
}

// clear preview panel
void PreviewClr()
{
	DrawRect(WIDTH/2, 0, WIDTH/2, HEIGHT, COL_BLACK);
}

// display info text
void DispInfo(const char* text)
{
	// prepare length of into text
	int len = StrLen(text);

	// set text color and coordinated
	DispCol(COL_INFOFG, COL_INFOBG);
	DispY = 2;
	DispX = (FILECOLW/2 - len)/2;

	// display info text
	DispText(text);
}

// display big info text
void DispBigInfo(const char* text)
{
	DrawClear();
	int len = StrLen(text);
	DrawTextBg4(text, (WIDTH - len*32)/2, (HEIGHT - 4*16)/2, COL_BIGINFOFG, COL_BIGINFOBG);
	DispUpdate();
}

// display big error text
void DispBigErr(const char* text)
{
	FileClose(&PrevFile);

	DrawClear();
	int len = StrLen(text);
	DrawTextBg4(text, (WIDTH - len*32)/2, (HEIGHT-64)/2, COL_BIGERRFG, COL_BIGERRBG);
	DispUpdate();

	KeyFlush();
	while (KeyGet() == NOKEY) {}

	DrawClear();
	FrameFileList();
	PathFileList();
	DispFileList();
	DispUpdate();
}

// loading next byte from temporary buffer (returns 0 if no next byte)
u8 PrevChar()
{
	// check if buffer need to be loaded
	if (TempBufInx >= TempBufNum)
	{
		// check if file is open
		if (!FileIsOpen(&PrevFile)) return 0;

		// read next temporary buffer
		TempBufInx = 0;
		TempBufNum = FileRead(&PrevFile, TempBuf, TEMPBUF);

		// end of file
		if (TempBufNum < TEMPBUF) FileClose(&PrevFile);

		// no data
		if (TempBufNum == 0) return 0;
	}

	// get next byte
	return TempBuf[TempBufInx++];
}

// loading next byte from temporary buffer, skip CR (returns 0 if no next byte)
char PrevCharCR()
{
	char ch;
	do {
		ch = PrevChar();
	} while (ch == CH_CR);
	return ch;
}

// display preview
void Preview()
{
	int i, j;
	char ch;
	u8 inv;
	sBmp* bmp;
	u32* dst;
	u8* src;

	switch(PrevState)
	{
	// waiting for start
	case PREV_START:

		// close old preview file
		FileClose(&PrevFile);

		// clear preview panel
		PreviewClr();
		DispUpdate();

		// check if current file is valid
		if ((uint)FileCur >= (uint)FileNum)
		{
			// no current file
			PrevState = PREV_STOP;
			return;
		}

		// prepare file descriptor
		PrevFD = &FileDesc[FileCur];

		// no text file
		if ((PrevFD->attr & ATTR_TXT) == 0)
		{
			// waitting for bitmap start
			PrevState = PREV_BMP_START;
			return;
		}

		// prepare filename of text file
		memcpy(TempBuf, PrevFD->name, PrevFD->len);
		memcpy(&TempBuf[PrevFD->len], ".TXT", 5);

		// open text file
		SetDir(Path);
		if (!FileOpen(&PrevFile, TempBuf))
		{
			// cannot open text file, waitting for bitmap start
			PrevState = PREV_BMP_START;
			return;
		}
		TempBufNum = 0; // no data in temporary buffer
		TempBufInx = 0; // reset current index in temporary buffer
		PrevLine = 0; // reset current preview line

		// loading text file
		PrevState = PREV_TXT_LOAD;
		break;

	// loading text
	//	- text width = 26 characters
	//	- ^ is prefix of control characters
	//		^^ ... print ^ character
	//		^1..^9 ... print character with code 1..9
	//		^A..^V ... print character with code 10..31
	//		^0 ... normal gray text
	//		^W ... white text
	//		^X ... green text
	//		^Y ... yellow text
	//		^Z ... red text
	//		^[ ... start invert
	//		^] ... stop invert
	case PREV_TXT_LOAD:

		// set normal text color
		DispCol(COL_TEXTFG, COL_TEXTBG);
		inv = 0;

		// set coordinates
		DispX = TEXTW/2;
		DispY = PrevLine;

		// decode one row (i = relative character position)
		for (i = 0; i < TEXTW/2;)
		{
			// load next character, skip CR characters
			ch = PrevCharCR();

			// LF end of line or NUL end of file
			if ((ch == CH_LF) || (ch == 0)) break;

			// tabulator (modulo 8)
			if (ch == CH_TAB)
			{
				do {
					DispChar(' ' ^ inv);
					i++; // increase character position
				} while ((i < TEXTW/2) && ((i & 7) != 0));
			}
			else
			{
				// prefix character ^
				if (ch == '^')
				{
					// load next character
					ch = PrevCharCR();

					// LF end of line or NUL end of file
					if ((ch == CH_LF) || (ch == 0))
						break;

					// set white text
					else if (ch == 'W')
						DispCol(COL_WHITE, COL_TEXTBG);

					// set green text
					else if (ch == 'X')
						DispCol(COL_GREEN, COL_TEXTBG);

					// set yellow text
					else if (ch == 'Y')
						DispCol(COL_YELLOW, COL_TEXTBG);
						
					// set red text
					else if (ch == 'Z')
						DispCol(COL_RED, COL_TEXTBG);
						
					// set normal text
					else if (ch == '0')
						DispCol(COL_TEXTFG, COL_TEXTBG);

					// print character with code 1..9
					else if ((ch >= '1') && (ch <= '9'))
					{
						DispChar((ch - '0') ^ inv);
						i++; // increase character position
					}

					// print character with code 10..31
					else if ((ch >= 'A') && (ch <= 'V'))
					{
						DispChar((ch - 'A' + 10) ^ inv);
						i++; // increase character position
					}

					// start invert
					else if (ch == '[')
						inv = 0x80;

					// stop invert
					else if (ch == ']')
						inv = 0;

					// print ^ character or other normal character
					else
					{
						DispChar(ch ^ inv);
						i++; // increase character position
					}
				}
				else
				{
					// display one character
					DispChar(ch ^ inv);
					i++; // increase character position
				}
			}
		}

		// skip rest of the row
		if (i == TEXTW/2)
		{
			// find LF end of line or NUL end of file
			do {
				ch = PrevCharCR();
			} while ((ch != CH_LF) && (ch != 0));
		}

		// prepare number of rows
		i = ((PrevFD->attr & ATTR_BMP) == 0) ? TEXTH : (TEXTH/2);

		// increase curren row
		PrevLine++;
		if (PrevLine >= i)
		{
			DispUpdate();

			// close text file
			FileClose(&PrevFile);

			// start loading bitmap
			PrevState = PREV_BMP_START;
		}
		break;

	// waiting for bitmap start
	case PREV_BMP_START:

		// no bitmap file
		if ((PrevFD->attr & ATTR_BMP) == 0)
		{
			// stop loading
			PrevState = PREV_STOP;
			return;
		}

		// prepare filename of bitmap file
		memcpy(TempBuf, PrevFD->name, PrevFD->len);
		memcpy(&TempBuf[PrevFD->len], ".BMP", 5);

		// open bitmap file
		SetDir(Path);
		if (!FileOpen(&PrevFile, TempBuf))
		{
			// cannot open bitmap file, stop
			PrevState = PREV_STOP;
			return;
		}
		TempBufNum = 0; // no data in temporary buffer
		TempBufInx = 0; // reset current index in temporary buffer

		// load bitmap header
		i = FileRead(&PrevFile, TempBuf, sizeof(sBmp));

		// get size of preview image
		bmp = (sBmp*)TempBuf;
		PrevW = bmp->biWidth; // width aligned to DWORD
		PrevH = bmp->biHeight;
		if (PrevH < 0) PrevH = -PrevH; // negative height -> flip row order

		// check bitmap header
		if ((i != sizeof(sBmp)) ||
			(PrevW < 8) || (PrevW > 2048) ||
			(PrevH < 8) || (PrevH > 2048) ||
			(bmp->bfType != 0x4D42) ||
			(bmp->bfSize < 100) || (bmp->bfSize > 5000000) ||
			(bmp->bfOffBits < 54) || (bmp->bfOffBits > 2000) ||
			(bmp->biPlanes != 1) ||
			((bmp->biBitCount != 32) && (bmp->biBitCount != 24)))
		{
			FileClose(&PrevFile);

			// cannot open bitmap file, stop
			PrevState = PREV_STOP;
			return;
		}

		PrevBytes = bmp->biBitCount/8;
		PrevWB = (PrevW * PrevBytes + 3) & ~3;

		// set start of first line
		FileSeek(&PrevFile, bmp->bfOffBits);

		// prepare first video line
		PrevLine = ((PrevFD->attr & ATTR_TXT) == 0) ? 0 : (HEIGHT/2);

		// loading bitmap file
		PrevState = PREV_BMP_LOAD;
		break;

	// loading bitmap
	case PREV_BMP_LOAD:

		// prepare address in video memory
		dst = &FrameBuffer.drawbuf[PrevLine*WIDTH + WIDTH/2];

		// prepare size of data to read from one line
		i = (PrevW > (WIDTH/2)) ? (WIDTH/2) : PrevW;

		// read one video line
		FileRead(&PrevFile, TempBuf, i*PrevBytes); // max. 320*4 = 1280 bytes
		src = (u8*)TempBuf;
		j = i;
		if (PrevBytes == 4)
		{
			
			for (; j > 0; j--)
			{
				u8 b = src[0];
				u8 g = src[1];
				u8 r = src[2];
				src += 4;
				*dst++ = COLOR(r, g, b, 255);
			}
		}
		else
		{
			for (; j > 0; j--)
			{
				u8 b = src[0];
				u8 g = src[1];
				u8 r = src[2];
				src += 3;
				*dst++ = COLOR(r, g, b, 255);
			}
		}

		// skip rest of line
		if (PrevWB > i*PrevBytes) FileSeek(&PrevFile, FilePos(&PrevFile) + PrevWB - i*PrevBytes);

		// increase line
		PrevLine++;
		PrevH--;

		// end of image
		if ((PrevLine >= HEIGHT) || (PrevH <= 0))
		{
			DispUpdate();

			// close preview file
			FileClose(&PrevFile);

			// stop preview
			PrevState = PREV_STOP;
		}
		else
		{
			if ((PrevH & 0x1f) == 0) DispUpdate();
		}
		break;

	// preview not active
	// PREV_STOP
	default:
		break;
	}
}

// parse kernel configuration file
void ParseCfg()
{
	int i;
	char *d, *s;

	// set root directory
	SetDir("/");

	// open file
	if (FileOpen(&PrevFile, KERNEL_CFG))
	{
		// read file
		int n = FileRead(&PrevFile, TempBuf, TEMPBUF-1);
		FileClose(&PrevFile);
		if ((n > 2) && (n <= TEMPBUF-1))
		{
			// stop mark
			TempBuf[n] = 0;

			// parse path
			d = Path;
			s = TempBuf;
			for (i = 0; i <= PATHMAX; i++)
			{
				if ((u8)*s < (u8)' ') break;
				*d++ = *s++;
			}
			PathLen = i;
			Path[i] = 0;

			// skip line
			while ((*s == 13) || (*s == 10)) s++;

			// parse filename
			d = LastName;
			for (i = 0; i < 8; i++)
			{
				if ((u8)*s < (u8)' ') break;
				*d++ = *s++;
			}
			LastNameLen = i; // length of last name, 0 = not used
			LastNameDir = 0; // ATTR_DIR flag of last name

			// set directory
			SetDir(Path);

			// get directory
			PathLen = GetDir(Path, PATHMAX);

			// request to reload current directory
			if (PathLen == 0)
				ResetRoot();
			else			
				Reload();
		}
	}
}

// trampoline
extern unsigned char Trampoline;	// trampoline start
extern unsigned char Trampoline_End;	// trampoline end
typedef void (*tramp_fn_t)(void* src, int len, void* dest);

int main()
{
	int i, j, k, m, n;
	u32 t;
	sFileDesc* fd;

	// clear screen
	DrawClear();

	// reset to root
	LastNameLen = 0; // no last name
	FileInit(&PrevFile); // initialize file structure of preview file
	ResetRoot();

	// display frame of file list
	FrameFileList();
	DispInfo("Insert SD card");
	DispUpdate();

	// initialize remount
	LastMount = Time()-2000000; // invalidate last mount time = current time - 2 seconds
	Remount = True; // remount request

	// try to mount disk
	if (DiskMount())
	{
		FileTop = 0;

		// parse kernel configuration file
		ParseCfg();

		// display info text
		FrameFileList();
		PathFileList();
		DispInfo("Loading files...");
		DispUpdate();

		// load files
		Remount = False; // clear flag - disk already remounted
		LoadFileList(); // load file list

		// set cursor to last name
		SetLastName();

		// display new directory
		FrameFileList();
		PathFileList();
		DispFileList();
		DispUpdate();
	}

	while (True)
	{
		// display update
		//DispUpdate();

		// check last mount time (interval 1 second)
		t = Time();
		if ((int)(t - LastMount) > 1000000)
		{
			// save new last mount time
			LastMount = t;

			// disk removed, request to remount disk next time
			if (!DiskMount())
			{
				// clear directories if not done yet
				if (!Remount) // if remount request yet not set
				{
					// clear preview panel
					PreviewClr();

					// reset to root
					ResetRoot();

					// display info text
					DispInfo("Insert SD card");
					DispUpdate();
				}
			}
			else
			{
				// reload directories if disk need to be remounted
				if (Remount)
				{
					// clear preview panel
					PreviewClr();

					// display info text
					DispInfo("Loading files...");
					DispUpdate();

					// load files
					Remount = False; // clear flag - disk already remounted
					LoadFileList(); // load file list

					// set cursor to last name
					SetLastName();

					// display new directory
					FrameFileList();
					PathFileList();
					DispFileList();
					DispUpdate();
				}
			}
		}

		// keyboard service
		while (True)
		{
			int ch = KeyGet();
			if (ch == NOKEY) break;

			// key switch
			switch (ch & (KEY_MASK | KEYFLAG_ALT | KEYFLAG_CTRL))
			{
			// down
			case KEY_DOWN:
			case KEY_DOWN | KEYFLAG_CTRL:
				if (FileCur < FileNum-1)
				{
					// increase cursor
					FileCur++;

					// prepare max. top entry
					i = FileCur - (2*FILEROWS-1);
					if (i < 0) i = 0;

					// limit min. top entry
					if (FileTop < i) FileTop = i;

					// repaint display
					DispFileList();
					DispUpdate();
				}
				break;

			// PageDown and Right
			case KEY_RIGHT:
			case KEY_RIGHT | KEYFLAG_CTRL:
			case KEY_DOWN | KEYFLAG_ALT:
				if (FileCur < FileNum-1)
				{
					// max. top entry
					i = FileNum - 2*FILEROWS;
					if (i < 0) i = 0;
					j = FILEROWS; // page size

					// cursor is at left column, or max. top entry reached
					if ((FileCur - FileTop < j) || (FileTop >= i))
					{
						FileCur += j;
						if (FileCur > FileNum-1) FileCur = FileNum-1;
					}

					// max. top entry not reached - move page
					else
					{
						FileTop += j; // shift top file
						if (FileTop > i) // max. top entry exceeded?
						{
							j -= FileTop - i; // reduce page size
							FileTop = i; // reduce top entry
						}
						FileCur += j; // shift cursor
					}

					// repaint display
					DispFileList();
					DispUpdate();
				}
				break;

			// Up
			case KEY_UP:
			case KEY_UP | KEYFLAG_CTRL:
				if (FileCur > 0)
				{
					// decrease cursor
					FileCur--;

					// limit top entry
					if (FileTop > FileCur) FileTop = FileCur;

					// repaint display
					DispFileList();
					DispUpdate();
				}
				break;

			// PagUp and Left
			case KEY_LEFT:
			case KEY_LEFT | KEYFLAG_CTRL:
			case KEY_UP | KEYFLAG_ALT:
				if (FileCur > 0)
				{
					j = FILEROWS; // page size

					// cursor is at right column, or min. top entry reached
					if ((FileCur - FileTop >= j) || (FileTop <= 0))
					{
						FileCur -= j;
						if (FileCur < 0) FileCur = 0;
					}

					// min. top entry not reached - move page
					else
					{
						FileTop -= j; // shift top file
						if (FileTop < 0) // min. top entry exceeded?
						{
							j += FileTop; // reduce page size
							FileTop = 0; // reduce top entry
						}
						FileCur -= j; // shift cursor
					}

					// repaint display
					DispFileList();
					DispUpdate();
				}
				break;

			// Home
			case KEY_LEFT | KEYFLAG_ALT:
				if (FileCur != 0)
				{
					FileCur = 0;
					FileTop = 0;
					DispFileList();
					DispUpdate();
				}
				break;

			// End
			case KEY_RIGHT | KEYFLAG_ALT:
				i = FileNum - 2*FILEROWS;
				if (i < 0) i = 0;
				j = FileNum - 1;
				if (j < 0) j = 0;
				if ((FileTop != i) || (FileCur != j))
				{
					FileTop = i;
					FileCur = j;
					DispFileList();
					DispUpdate();
				}
				break;

			// Enter, Space
			case KEY_ENTER:
			case KEY_SPACE:
				if ((uint)FileCur < (uint)FileNum)
				{
					fd = &FileDesc[FileCur];

					// change directory
					if ((fd->attr & ATTR_DIR) != 0)
					{
						// going to up-directory
						if ((fd->len == 2) && (fd->name[0] == '.') && (fd->name[1] == '.'))
						{
							// delete last directory
							i = PathLen;
							while (Path[PathLen-1] != PATHCHAR) PathLen--;

							// save last name
							LastNameLen = i - PathLen;
							memcpy(LastName, &Path[PathLen], LastNameLen);
							LastNameDir = ATTR_DIR;

							// delete path separator if not root
							if (PathLen > 1) PathLen--;
						}

						// going to sub-directory
						else
						{
							// check path length
							if (PathLen + 1 + fd->len <= PATHMAX)
							{
								// add path separator if not root
								if (PathLen != 1) Path[PathLen++] = PATHCHAR;

								// add path name
								memcpy(&Path[PathLen], fd->name, fd->len);
								PathLen += fd->len;
							}
						}

						// set terminating 0
						Path[PathLen] = 0;

						// request to reload current directory
						FileTop = 0;
						Reload();

						// invalidate last mount time = current time - 2 seconds
						LastMount = Time()-2000000;
					}

					// run program
					else
					{
						// close old preview file
						FileClose(&PrevFile);

						// prepare filename of the file
						memcpy(TempBuf, fd->name, fd->len);
						memcpy(&TempBuf[fd->len], ".IMG", 5);

						// open file
						DispBigInfo("Loading...");
						SetDir(Path);
						if (!FileOpen(&PrevFile, TempBuf) || (fd->size < 10))
						{
							DispBigErr("Cannot open file");
						}
						else
						{
							// allocate memory block
							void* mem = MemAlloc(fd->size + 4 + TRAMP_SIZE);
							if (mem == NULL)
							{
								FileClose(&PrevFile);
								DispBigErr("File is too big");
							}
							else
							{
								// read program to the memory
								int num = FileRead(&PrevFile, mem, fd->size);
								FileClose(&PrevFile);
								if (num != fd->size)
								{
									MemFree(mem);
									DispBigErr("Read error");
								}
								else
								{
									// save kernel configuration file
									SetDir("/");
									FileDelete(KERNEL_CFG); // delete configuration file
									if (FileCreate(&PrevFile, KERNEL_CFG)) // create file
									{
										// write path
										FileWrite(&PrevFile, Path, PathLen);
										FileWrite(&PrevFile, "\r\n", 2);

										// write file name
										FileWrite(&PrevFile, fd->name, fd->len);
										FileWrite(&PrevFile, "\r\n", 2);

										// close file
										FileClose(&PrevFile);
									}

									// unmount disk
									DiskUnmount();

									// clear screen
									DrawClear();
									DispUpdate();

									// system terminate (cannot be run with multicore, used only by boot loader)
									SysTerm();

									// trampoline destination address
									void* tramp = (void*)(((size_t)mem + fd->size + 16 + 15) & ~0x0f); // trampoline address aligned to 16 bytes

									// trampoline real size
									size_t tramp_size = (size_t)&Trampoline_End - (size_t)&Trampoline;

									// copy trampoline
									// At this place, memcpy() cannot be used, it locks on non-cached unaligned memory.
									u8* d = (u8*)tramp;
									const u8* s = (const u8*)&Trampoline;
									for (; tramp_size > 0; tramp_size--) *d++ = *s++;
									cb();

									// invalidate instruction cache
									InvalidateInstructionCache();
									isb();

									// boot address
#if AARCH==32
									void* dest = (void*)0x8000;
#else
									void* dest = (void*)0x80000;
#endif

									// jump to trampoline
									tramp_fn_t fn = (tramp_fn_t)tramp;
									fn(mem, fd->size, dest);

									// --- trampoline will not return here
								}
							}
						}
					}
				}
				break;

			// BackSpace - out of directory
			case KEY_BS:
				// not root directory yet
				if (PathLen > 1)
				{
					// delete last directory
					i = PathLen;
					while (Path[PathLen-1] != PATHCHAR) PathLen--;

					// save last name
					LastNameLen = i - PathLen;
					memcpy(LastName, &Path[PathLen], LastNameLen);
					LastNameDir = ATTR_DIR;

					// delete path separator if not root
					if (PathLen > 1) PathLen--;

					// set terminating 0
					Path[PathLen] = 0;

					// request to reload current directory
					FileTop = 0;
					Reload();

					// invalidate last mount time = current time - 2 seconds
					LastMount = Time()-2000000;
				}
				break;
			}
		}

		// preview
		Preview();
	}
}
