
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

#define CONFIG_VOLUME_FULL	VOLDEF		// sound volume default level (100% full volume, = 9)
#define CONFIG_VOLUME_MAX	VOLMAX		// max. sound volume level (= 15)

// MP3 player
sMP3Player MP3Player;
u8 ALIGNED MP3PlayerInBuf[MP3PLAYER_INSIZE];
u8 ALIGNED MP3PlayerOutBuf[MP3PLAYER_OUTSIZE];

// get sound volume level (returns 0..CONFIG_VOLUME_MAX)
INLINE int ConfigGetVolume() { return GlobalSoundVolume; }

// increase sound volume level (limits range, updates playing sound, does not save configuration)
INLINE void ConfigIncVolume() { if (GlobalSoundVolume < CONFIG_VOLUME_MAX) GlobalVolumeSet(GlobalSoundVolume+1); }

// decrease sound volume level (limits range, updates playing sound, does not save configuration)
INLINE void ConfigDecVolume() { if (GlobalSoundVolume > 0) GlobalVolumeSet(GlobalSoundVolume-1); }

Bool InfoMode; // info mode (no image, display MP3 info)

#define MAXFILES 1000
#define ROWN	(HEIGHT/16)	// number of rows (= 240/16 = 15)
#define COLN	(WIDTH/(10*8))	// number of columns (= 320/80 = 4)
#define PAGEN	(ROWN*COLN)	// number of entries per page (= 15*4 = 60)

// Sound file (name without .MP3 extension)
typedef struct {
	char	name[12+1];	// Sound file name
	u8	namelen;	// length of file name
	Bool	dir;		// directory flag
} sSoundFile;

// subdirectory
#define MAXDIR 10
#define APPPATH_PATHMAX	(256 - 8 - 1) // max. length of the path, without terminating 0 (= 247)
#define SOUNDPATH_MAX	(APPPATH_PATHMAX+10)
char SoundPath[SOUNDPATH_MAX+1]; // path with sounds (without tailing '/')
int SoundPathLen;	// length of the path
char SoundPathFile[9];
u32 LastMount; // last mount time
Bool Remount; // remount request

// list of MP3 files
sSoundFile FileList[MAXFILES];
int FileListNum = 0;	// number of files
int FileListTop = 0;	// top file
int FileListSel = 0;	// selected file
int FileListMP3 = -1;	// first MP3 file (-1 = none)
s8 Volume = CONFIG_VOLUME_FULL; // current volume
sPic* JPGImage = NULL;	// JPG image (NULL = none)

// seek cursor to last name
char LastName[8+1];
int LastNameLen; // length of last name, 0 = not used
Bool LastNameDir; // flag of last name directory

// request to reload current directory
void Reload()
{
	// reset file list
	FileListNum = 0; // no entry
	FileListSel = 0; // reset cursor
	FileListTop = 0; // reset top entry

	// set flag - disk need to be remounted
	Remount = True;
}

// reset to root
void ResetRoot()
{
	// reset path to root
	SoundPath[0] = PATHCHAR;
	SoundPath[1] = 0;
	SoundPathLen = 1;

	// request to reload current directory
	Reload();
}

// load file list
void LoadFileList()
{
	FileListNum = 0;
	FileListSel = 0;
	FileListMP3 = -1;	// first MP3 file (-1 = none)
	sFile find;
	sFileInfo info;
	sSoundFile* snd;
	sSoundFile tmp;
	int i;

	// set current directory
	if (!SetDir(SoundPath)) return;

	// open search files
	if (!FindOpen(&find, "")) return;

	// find subdirectories
	for (; FileListNum < MAXFILES;)
	{
		// find next file
		if (!FindNext(&find, &info, ATTR_DIR | ATTR_RO | ATTR_ARCH, "*.")) break;

		// skip file
		if ((info.attr & ATTR_DIR) == 0) continue;

		// skip directory "."
		if ((info.namelen == 1) && (info.name[0] == '.')) continue;

		// fill up info
		snd = &FileList[FileListNum];
		memcpy(snd->name, info.name, 12+1);
		snd->namelen = info.namelen;
		snd->name[snd->namelen] = 0;
		snd->dir = True;
		FileListNum++;
	}

	// find re-open
	FindClose(&find);
	if (FindOpen(&find, ""))
	{
		// find files
		for (; FileListNum < MAXFILES;)
		{
			// find next file
			if (!FindNext(&find, &info, ATTR_ARCH | ATTR_RO, "*.MP3")) break;

			// first MP3 file
			if (FileListMP3 < 0) FileListMP3 = FileListNum;

			// fill up info
			snd = &FileList[FileListNum];
			memcpy(snd->name, info.name, 12+1);
			snd->namelen = info.namelen - 4; // cut .MP3
			snd->name[snd->namelen] = 0;
			snd->dir = False;
			FileListNum++;
		}

		// find close
		FindClose(&find);
	}

	// sort files
	for (i = 0; i < FileListNum-1;)
	{
		if (	(FileList[i+1].name[0] == '.') ||
			(!FileList[i].dir && FileList[i+1].dir) ||
			((FileList[i].dir == FileList[i+1].dir) &&
				(FileList[i].name[0] != '.') &&
				(strcmp(FileList[i].name, FileList[i+1].name) > 0)))
		{
			memcpy(&tmp, &FileList[i], sizeof(sSoundFile));
			memcpy(&FileList[i], &FileList[i+1], sizeof(sSoundFile));
			memcpy(&FileList[i+1], &tmp, sizeof(sSoundFile));
			if (i > 0)
				i--;
			else
				i++;
		}
		else
			i++;
	}
}

// view window correction
void ViewSetup()
{
	int num = FileListNum;

	// max. top entry
	int maxtop = num - PAGEN;	// max. top entry
	if (maxtop < 0) maxtop = 0;	// underflow max. top entry
	maxtop = (maxtop + ROWN-1)/ROWN*ROWN; // round max. top entry up

	// top entry
	int top = FileListTop;		// top entry
	if (top < 0) top = 0;		// underflow top entry
	top = (top + ROWN-1)/ROWN*ROWN; // round top entry up

	// limit top entry by max. top entry
	if (top > maxtop) top = maxtop;

	// shift top entry to containg selected entry
	int sel = FileListSel;		// selected entry
	if (sel >= num) sel = num-1;	// overflow selected entry
	if (sel < 0) sel = 0;		// underflow selected entry
	FileListSel = sel;

	// shift top entry, to view selected entry
	while (top > sel) top -= ROWN;
	while (top + PAGEN <= sel) top += ROWN;
	FileListTop = top;
}

// set cursor to last name
void SetLastName()
{
	sSoundFile* fd;
	int i;

	// no last name required
	if (LastNameLen == 0) return;

	// search last name in file list
	for (FileListSel = FileListNum-1; FileListSel > 0; FileListSel--)
	{
		fd = &FileList[FileListSel];
		if ((fd->namelen == LastNameLen) &&
			(fd->dir == LastNameDir) &&
			(memcmp(LastName, fd->name, LastNameLen) == 0)) break;
	}

	// view window correction
	ViewSetup();

	// delele last name request
	LastNameLen = 0;
}

// display file list
void DispFileList()
{
	int i, namelen;
	u32 fgcol, bgcol;
	char buf[31];
	sSoundFile* snd;

	// set font 8x16
	SelFont8x16();

	// no file
	if (FileListNum == 0)
	{
		DrawClear();
		DrawText("No files", 0, 0, COL_YELLOW);
		DispUpdate();
		return;
	}

	// display files
	int x = 0;
	int y = 0;
	for (i = FileListTop; i < FileListTop + PAGEN; i++)
	{
		// prepare color
		fgcol = COL_WHITE;
		bgcol = COL_BLACK;
		if ((i < FileListNum) && FileList[i].dir) fgcol = COL_YELLOW; // directory

		if (i == FileListSel)
		{
			bgcol = fgcol;
			fgcol = COL_BLACK;
		}

		// display name
		if (i < FileListNum)
		{
			snd = &FileList[i];
			if (snd->dir)
				MemPrint(buf, 30, "[%-8s]", snd->name);
			else
				MemPrint(buf, 30, " %-8s ", snd->name);
			DrawTextBg(buf, x, y, fgcol, bgcol);
		}
		else
			DrawTextBg("          ", x, y, fgcol, bgcol);

		y += 16;
		if (y > HEIGHT-16)
		{
			x += 10*8;
			y = 0;
		}		
	}
	DispUpdate();
}

// display sound position
void DispSoundPos(Bool pause)
{
	char buf[20];
	sMP3Player* mp3 = &MP3Player;

	// get total length and position in seconds
	int len = MP3GetTimeLen(mp3);
	int pos = MP3GetTimePos(mp3);
#define MAXSOUNDPOS (99*60+59)
	if (len > MAXSOUNDPOS) len = MAXSOUNDPOS;
	if (pos > MAXSOUNDPOS) pos = MAXSOUNDPOS;

	// display current position (11 characters, width 88 pixels) 
	if (pause)
		MemPrint(buf, 20, "%02d:%02d/PAUSE", pos/60, pos%60);
	else
		MemPrint(buf, 20, "%02d:%02d/%02d:%02d", pos/60, pos%60, len/60, len%60);
	SelFont8x8();
	DrawTextBg(buf, 0, HEIGHT-8, COL_WHITE, COL_BLACK);

	// progress frame
	int x = 11*8;
	int w = WIDTH - 5*8 - x;
	DrawFrame(x, HEIGHT-8, w, 8, COL_BLACK);
	DrawFrame(x+1, HEIGHT-7, w-2, 6, COL_WHITE);
	x += 2;
	w -= 4;

	pos = mp3->pos;
	len = mp3->frames;
	if (pos > len) pos = len;
	if (len <= 0)
		pos = 0;
	else
		pos = (pos*w + len/2) /  len;
	DrawRect(x, HEIGHT-6, pos, 4, COL_AZURE);
	DrawRect(x + pos, HEIGHT-6, w - pos, 4, COL_BLACK);
}

// display volume
void DispVol()
{
	char buf[20];
	u8 vol = GlobalSoundVolume;
	MemPrint(buf, 20, "\12%03d%%", (vol*100 + CONFIG_VOLUME_FULL/2) / CONFIG_VOLUME_FULL);
	SelFont8x8();
	u32 col = COL_WHITE;
	if (vol > CONFIG_VOLUME_FULL) col = COL_YELLOW;
	if (vol > (CONFIG_VOLUME_FULL+CONFIG_VOLUME_MAX)/2) col = COL_RED;
	DrawTextBg(buf, WIDTH-5*8, HEIGHT-8, col, COL_BLACK);
}

// display title
void DispTitle()
{
	sMP3Player* mp3 = &MP3Player;

	// prepare artist name
	char* art = mp3->id3_artist;
	int artlen = mp3->id3_artist_len;

	// prepare song name
	char* nam = mp3->id3_title;
	int namlen = mp3->id3_title_len;

	// prepare title
	char tit[53+1];
	int titlen = artlen;
	memcpy(tit, art, artlen); // add artist
	if ((artlen != 0) && (namlen != 0))
	{
		memcpy(tit + titlen, " - ", 3); // add " - " separator
		titlen += 3;
	}
	if (titlen + namlen > 53) namlen = 53 - titlen;
	memcpy(tit + titlen, nam, namlen); // add song name
	titlen += namlen;
	if (titlen == 0)
	{
		memcpy(tit, FileList[FileListSel].name, FileList[FileListSel].namelen);
		titlen = FileList[FileListSel].namelen;
	}
	tit[titlen] = 0;

	// display title
	if (titlen <= 38)
	{
		SelFont8x8();
		int x = (WIDTH - titlen*8)/2;
		DrawRect(x - 2, 0, titlen*8+4, 8, COL_BLACK);
		DrawText(tit, x, 0, COL_GREEN);
	}
	else
	{
		SelFont6x8();
		int x = (WIDTH - titlen*6)/2;
		DrawRect(x - 2, 0, titlen*6+4, 8, COL_BLACK);
		DrawText(tit, x, 0, COL_GREEN);
	}
}

// display frame
void DispFrame(int y, int h, const char* title, u32 col)
{
	SelFont8x8();
	DrawFrame(4, y + 4, WIDTH - 8, h - 8, col);
	DrawTextBg(title, 8, y, col, COL_BLACK);
}

// MPEG version
const char* MpegVerTxt[4] = {
	"MPEG1",
	"MPEG2",
	"MPEG2.5",
	"-"
};

// display info (info mode)
void DispInfo()
{
	int i;
	int y = 8 + 4;
	SelFont8x16();
	char buf[45];
	sMP3Player* mp3 = &MP3Player;

	int x1 = 8;

// ---- File

	// File frame
	DispFrame(y, 5*8, "File", COL_CYAN); y += 8;
	SelFont8x8();
	int x2 = 6*8+2;

	// File path
	DrawText("Path:", x1, y, COL_YELLOW);
	DrawText(SoundPath, x2, y, COL_WHITE);
	y += 8;

	// File name
	DrawText("Name:", x1, y, COL_YELLOW);
	sSoundFile* s = &FileList[FileListSel];
	const char* filename = s->name;
	i = s->namelen;
	memcpy(buf, filename, i);
	memcpy(buf+i, ".MP3", 5);
	DrawText(buf, x2, y, COL_WHITE);
	y += 8;

	// File size
	DrawText("Size:", x1, y, COL_YELLOW);
	MemPrint(buf, 12, "%dKB", mp3->file.size/1024);
	DrawText(buf, x2, y, COL_WHITE);
	y += 8+8;

// ---- ID3

	// ID3 frame
	DispFrame(y, 7*16, "ID3", COL_MAGENTA); y += 8;
	SelFont8x16();
	x2 = 9*8+2;

	// title
	DrawText("  Title:", x1, y, COL_YELLOW);
	DrawText(mp3->id3_title, x2, y, COL_WHITE);
	y += 16;
	
	// artist
	DrawText(" Artist:", x1, y, COL_YELLOW);
	DrawText(mp3->id3_artist, x2, y, COL_WHITE);
	y += 16;
	
	// album
	DrawText("  Album:", x1, y, COL_YELLOW);
	DrawText(mp3->id3_album, x2, y, COL_WHITE);
	y += 16;

	// comment
	DrawText("Comment:", x1, y, COL_YELLOW);
	DrawText(mp3->id3_comment, x2, y, COL_WHITE);
	y += 16;

	// genre
	DrawText("  Genre:", x1, y, COL_YELLOW);
	i = mp3->id3v1.genre;
	if (i < MP3_GENRELIST_NUM) DrawText(MP3GenreList[i], x2, y, COL_WHITE);
	y += 16;

	// year
	DrawText("   Year:", x1, y, COL_YELLOW);
	DrawText(mp3->id3_year, x2, y, COL_WHITE);

	// track
	DrawText("Track:", 18*8, y, COL_YELLOW);
	i = mp3->id3v1.track;
	if (i > 99) i = 99;
	MemPrint(buf, 12, "%02d", mp3->id3v1.track);
	DrawText(buf, 18*8+6*8+2, y, COL_WHITE);

	// Mono
	DrawText((mp3->info.nChans == 1) ? "MONO" : "STEREO", 32*8, y, COL_WHITE);
	y += 16+8;

// ---- Format

	// Format frame
	DispFrame(y, 8*8, "Format", COL_GRAY); y += 8;
	SelFont8x8();

	// Version
	DrawText("Version:", x1, y, COL_YELLOW);
	DrawText(MpegVerTxt[mp3->info.version], x2, y, COL_WHITE);

	// Layer
	DrawText("Layer:", 23*8, y, COL_YELLOW);
	DrawChar('0' + mp3->info.layer, 23*8+6*8+2, y, COL_WHITE);
	y += 8;

	// Bitrate
	DrawText("Bitrate:", x1, y, COL_YELLOW);
	MemPrint(buf, 12, "%dkbps", mp3->bitrateavg/1000);
	DrawText(buf, x2, y, COL_WHITE);

	// Sample rate
	DrawText("SampRate:", 20*8, y, COL_YELLOW);
	MemPrint(buf, 12, "%dHz", mp3->info.samprate);
	DrawText(buf, 20*8+9*8+2, y, COL_WHITE);
	y += 8;

	// Frames
	DrawText(" Frames:", x1, y, COL_YELLOW);
	MemPrint(buf, 12, "%d", mp3->frames);
	DrawText(buf, x2, y, COL_WHITE);

	// Frame time
	DrawText("FrameTime:", 19*8, y, COL_YELLOW);
	MemPrint(buf, 12, "%dus", mp3->frametime);
	DrawText(buf, 19*8+10*8+2, y, COL_WHITE);
	y += 8;

	// Frame In size	
	DrawText("FrameIn:", x1, y, COL_YELLOW);
	MemPrint(buf, 12, "%dB", mp3->framesizeavg);
	DrawText(buf, x2, y, COL_WHITE);

	// Frame Out size	
	DrawText("FrameOut:", 20*8, y, COL_YELLOW);
	MemPrint(buf, 12, "%dB", mp3->sampnumavg*2);
	DrawText(buf, 20*8+9*8+2, y, COL_WHITE);
	y += 8;

	// Poll load
	SelFont8x8();
	DrawText("   Poll:", x1, y, COL_YELLOW);
	DrawText("buf:", 30*8, y, COL_YELLOW);
	y += 8;

	// Decode load
	DrawText(" Decode:", x1, y, COL_YELLOW);
}

u32 DispLoadInfoOldTime;

// display load info (info mode)
void DispLoadInfo()
{
	// check interval
	u32 t = Time();
	if ((u32)(t - DispLoadInfoOldTime) < 1000000) return;
	DispLoadInfoOldTime = t;

	// get load info
	sMP3Player* mp3 = &MP3Player;
	di();
	u32 decode_time = mp3->decode_time;
	int decode_num = mp3->decode_num;
	int decode_outrem = mp3->decode_outrem;
	int decode_inrem = mp3->decode_inrem;
	u32 poll_time = mp3->poll_time;
	int poll_len = mp3->poll_len;
	int poll_rem = mp3->poll_rem;
	mp3->decode_time = 0;
	mp3->decode_num = 0;
	mp3->decode_outrem = mp3->outsize;
	mp3->decode_inrem = mp3->insize;
	mp3->poll_time = 0;
	mp3->poll_len = 0;
	mp3->poll_rem = mp3->insize;
	ei();

	SelFont8x8();
	char buf[30];
	int y = 25*8 + 4;

	// average Poll time
	if (poll_len < 1) poll_len = 1;
	poll_time = (int)((s64)poll_time * mp3->framesizeavg / poll_len);
	int i = MemPrint(buf, 30, "%dus/frame (%d%%)", poll_time, poll_time*100/mp3->frametime);
	DrawRect(9*8+i*8, y, 21*8-i*8, 8, COL_BLACK);
	DrawTextBg(buf, 9*8+2, y, COL_WHITE, COL_BLACK);

	// Poll buffer
	i = MemPrint(buf, 30, "%d%%", poll_rem*100/mp3->insize);
	DrawRect(34*8+i*8, y, 5*8-i*8, 8, COL_BLACK);
	DrawTextBg(buf, 34*8+2, y, COL_WHITE, COL_BLACK);
	y += 8;

	// average Decode time
	if (decode_num < 1) decode_num = 1;
	decode_time = (decode_time + decode_num/2) / decode_num;
	i = MemPrint(buf, 30, "%dus/frame (%d%%)", decode_time, decode_time*100/mp3->frametime);
	DrawRect(9*8+i*8, y, 21*8-i*8, 8, COL_BLACK);
	DrawTextBg(buf, 9*8+2, y, COL_WHITE, COL_BLACK);

	// decode IN buffer, decode OUT buffer
	i = MemPrint(buf, 30, "%d%%,%d%%", decode_inrem*100/mp3->insize, decode_outrem*100/mp3->outsize);
	DrawRect(30*8+i*8, y, 9*8-i*8, 8, COL_BLACK);
	DrawTextBg(buf, 30*8, y, COL_WHITE, COL_BLACK);
}

// load JPG image
void PlayJPG()
{
	// delete old image
	MemFree(JPGImage);
	JPGImage = NULL;

	// selected file
	sSoundFile* s = &FileList[FileListSel];
	const char* filename = s->name;
	int len = s->namelen;

	// load image
	char buf[20];
	memcpy(buf, filename, len);
	memcpy(buf+len, ".JPG", 5);

	InfoMode = True; // info mode (no image, display MP3 info)

	sFile f;
	if (FileOpen(&f, buf)) // open JPG file
	{
		JPGImage = JPGLoadFile(&f);
		FileClose(&f);
		if (JPGImage != NULL) InfoMode = False;
	}
	if (InfoMode) DrawClear();
}

// play selected sound
void PlayMedia()
{
	// set current directory
	if (!SetDir(SoundPath)) return;

PlayNextSound:

	// selected file
	sSoundFile* s = &FileList[FileListSel];
	const char* filename = s->name;
	int len = s->namelen;

	// load JPG file
	PlayJPG();

	// open MP3 file
	sMP3Player* mp3 = &MP3Player;
	char buf[20];
	memcpy(buf, filename, len);
	memcpy(buf+len, ".MP3", 5);
	int r = MP3PlayerInit(mp3, buf, MP3PlayerInBuf, MP3PLAYER_INSIZE, MP3PlayerOutBuf, MP3PLAYER_OUTSIZE, 500);

	// error
	if (r != ERR_MP3_OK)
	{
		DrawClear();
		SelFont8x16();
		DrawText("Error opening MP3 file", 0, 0, COL_YELLOW);
		DispUpdate();
		WaitMs(2000);
		return;
	}

	// start playing
	MP3Play(mp3, 0, False);
	Bool pause = False;

	// display info (info mode)
	if (InfoMode)
	{
		DispLoadInfoOldTime = Time() - 1000000;
		DispInfo();
		DispLoadInfo();
	}
	else
	{
		DrawClear();
		DrawImg((const u8*)JPGImage, 0, 0);
	}


	// display volume
	DispVol();

	// display title
	DispTitle();

	// display sound position
	DispSoundPos(pause);

	// display update
	DispUpdate();
	KeyFlush();

	// play sound
	while (MP3Playing(mp3) || pause)
	{
		// polling MP3
		MP3Poll(mp3);

		// display load info (info mode)
		if (InfoMode) DispLoadInfo();

		// display sound position
		DispSoundPos(pause);
		DispUpdate();

		// keyboard control
		switch (KeyGet())
		{
#if USE_ZEROPC
		// left (rewind back)
		case KEY_LEFT:
			MP3SeekTime(mp3, MP3GetTimePos(mp3) - 10);
			KeyFlush();
			break;

		// previous
		case KEY_HOME:
			MP3PlayerTerm(mp3);
			do {
				FileListSel--;
				if (FileListSel < 0) FileListSel = FileListNum-1;
			} while (FileList[FileListSel].dir);
			KeyFlush();
			goto PlayNextSound;

		// right (rewind forward)
		case KEY_RIGHT:
			MP3SeekTime(mp3, MP3GetTimePos(mp3) + 10);
			KeyFlush();
			break;

		// next
		case KEY_END:
			MP3PlayerTerm(mp3);
			do {
				FileListSel++;
				if (FileListSel >= FileListNum) FileListSel = 0;
			} while (FileList[FileListSel].dir);
			KeyFlush();
			goto PlayNextSound;
#endif // USE_ZEROPC

#if USE_ZEROTINY
		// left (rewind back)
		case KEY_LEFT:
			if (!KeyPressed(KEY_A))
			{
				MP3SeekTime(mp3, MP3GetTimePos(mp3) - 10);
				KeyFlush();
				break;
			}
			else
			{
				// previous
				MP3PlayerTerm(mp3);
				do {
					FileListSel--;
					if (FileListSel < 0) FileListSel = FileListNum-1;
				} while (FileList[FileListSel].dir);
				KeyFlush();
				goto PlayNextSound;
			}

		// right (rewind forward)
		case KEY_RIGHT:
			if (!KeyPressed(KEY_A))
			{
				MP3SeekTime(mp3, MP3GetTimePos(mp3) + 10);
				KeyFlush();
				break;
			}
			else
			{
				// next
				MP3PlayerTerm(mp3);
				do {
					FileListSel++;
					if (FileListSel >= FileListNum) FileListSel = 0;
				} while (FileList[FileListSel].dir);
				KeyFlush();
				goto PlayNextSound;
			}
#endif // USE_ZEROTINY

		// up (volume up)
		case KEY_UP:
			ConfigIncVolume();
			DispVol();
			DispUpdate();
			KeyFlush();
			break;

		// down (volume down)
		case KEY_DOWN:
			ConfigDecVolume();
			DispVol();
			DispUpdate();
			KeyFlush();
			break;

		// screen shot
		case KEY_SCREENSHOT:
			ScreenShot();
			break;

		// pause
		case KEY_SPACE:
			if (pause)
			{
				MP3Play(mp3, 0, False);
				pause = False;
			}
			else
			{
				MP3Stop(mp3);
				pause = True;
			}
			break;

		// info
		case KEY_TAB:
			DrawClear();
			if (InfoMode && (JPGImage != NULL))
			{
				InfoMode = False;
				DrawClear();
				DrawImg((const u8*)JPGImage, 0, 0);
			}
			else
			{
				InfoMode = True;
			}

			// display volume
			DispVol();

			// display title
			DispTitle();

			// display info (info mode)
			if (InfoMode)
			{
				DispLoadInfoOldTime = Time() - 1000000;
				DispInfo();
				DispLoadInfo();
			}
			DispSoundPos(pause);
			DispUpdate();
			KeyFlush();
			break;

		// quit
		case KEY_ESC:
			MP3PlayerTerm(mp3);
			KeyFlush();
			return;
		}
	}

	// flush buffers
	WaitMs(150);

	// close sound
	MP3PlayerTerm(mp3);

	// next sound
	do {
		FileListSel++;
		if (FileListSel >= FileListNum) FileListSel = 0;
	} while (FileList[FileListSel].dir);
	goto PlayNextSound;
}

int main()
{
	FileListSel = 0;
	FileListNum = 0;
	sSoundFile* fd;
	u32 t;
	int i;

	// get path with sounds
	SoundPathLen = GetHomePath(SoundPath, SOUNDPATH_MAX, SoundPathFile);
	LastNameLen = 0; // no last name

	// initialize remount
	LastMount = Time() - 2000000; // invalidate last mount time
	Remount = True; // remount request

	// try to mount disk
	if (DiskMount())
	{
		FileListTop = 0;

		// load files
		Remount = False; // disk alreay mounted
		LoadFileList(); // load file list

		// set cursor to last name
		SetLastName();

		// display new directory
		DispFileList();
		DispUpdate();
	}

	while (True)
	{
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
					// clear
					DrawClear();

					// reset to root
					ResetRoot();

					// display info text
					SelFont8x16();
					DrawText2("Insert SD card", (WIDTH-14*8*2)/2, (HEIGHT-16*2)/2, COL_YELLOW);
					DispUpdate();
				}
			}
			else
			{
				// reload directories if disk need to be remounted
				if (Remount)
				{
					FileListTop = 0;

					// display info text
//					DrawClear();
//					SelFont8x16();
//					DrawText2("Loading files...", (WIDTH-16*8*2)/2, (HEIGHT-16*2)/2, COL_YELLOW);
//					DispUpdate();

					// load files
					Remount = False; // clear flag - disk already remounted
					LoadFileList(); // load file list

					// set cursor to last name
					SetLastName();

					// display new directory
					DispFileList();
					DispUpdate();
				}
			}
		}

		// sound selection
		while (True)
		{
			int key = KeyGet();
			if (key == NOKEY) break;

			switch (key)
			{
			// Screenshot
			case KEY_SCREENSHOT:
				ScreenShot();
				break;

			// Esc exit
			case KEY_ESC:
				Reboot();
				break;

			// UP
			case KEY_UP:
				if (FileListSel > 0)
				{
					FileListSel--;
					ViewSetup();	// view window correction
					DispFileList();
					DispUpdate();
				}
				break;

			// DOWN
			case KEY_DOWN:
				if (FileListSel < FileListNum-1)
				{
					FileListSel++;
					ViewSetup();	// view window correction
					DispFileList();
					DispUpdate();
				}
				break;

			// LEFT
			case KEY_LEFT:
				if (FileListSel > 0)
				{
					FileListSel -= ROWN;
					ViewSetup();	// view window correction
					DispFileList();
					DispUpdate();
				}
				break;

			// RIGHT
			case KEY_RIGHT:
				if (FileListSel < FileListNum-1)
				{
					FileListSel += ROWN;
					ViewSetup();	// view window correction
					DispFileList();
					DispUpdate();
				}
				break;

			// Home
			case KEY_HOME:
				if (FileListSel > 0)
				{
					FileListSel = 0;
					FileListTop = 0;
					DispFileList();
					DispUpdate();
				}
				break;

			// End
			case KEY_END:
				if (FileListSel < FileListNum-1)
				{
					FileListSel = FileListNum-1;
					ViewSetup();	// view window correction
					DispFileList();
					DispUpdate();
				}
				break;

			// PageUp
			case KEY_PGUP:
				if (FileListSel > 0)
				{
					if (FileListTop == 0)
					{
						FileListSel = 0;
					}
					else
					{
						FileListSel -= PAGEN;
						FileListTop -= PAGEN;
						while (FileListTop < 0)
						{
							FileListSel += ROWN;
							FileListTop += ROWN;
						}
					}
					ViewSetup();	// view window correction
					DispFileList();
					DispUpdate();
				}
				break;

			// PageDown
			case KEY_PGDN:
				if (FileListSel < FileListNum-1)
				{
					i = FileListNum - PAGEN;	// max. top entry
					if (i < 0) i = 0;		// underflow max. top entry
					i = (i + ROWN-1)/ROWN*ROWN;	// round max. top entry up
					if (FileListTop == i)
					{
						FileListSel = FileListNum-1;
					}
					else
					{
						FileListSel += PAGEN;
						FileListTop += PAGEN;
						while (FileListTop > i)
						{
							FileListSel -= ROWN;
							FileListTop -= ROWN;
						}
					}
					ViewSetup();	// view window correction
					DispFileList();
					DispUpdate();
				}
				break;

			// BackSpace - up directory
			case KEY_BS:

				// not root directory yet
				i = SoundPathLen;
				if (i > 1)
				{
					// delete last directory
					while (SoundPath[SoundPathLen-1] != PATHCHAR) SoundPathLen--;

					// save last name
					LastNameLen = i - SoundPathLen;
					memcpy(LastName, &SoundPath[SoundPathLen], LastNameLen);
					LastNameDir = True;

					// delete path separator if not root
					if (SoundPathLen > 1) SoundPathLen--;

					// set terminating 0
					SoundPath[SoundPathLen] = 0;

					// request to reload current directory
					FileListTop = 0;
					Reload();

					// invalidate last mount time = current time - 2 seconds
					LastMount = Time()-2000000;
				}
				KeyFlush();
				break;

			// play 1 or change directory
			case KEY_ENTER:
				if (FileListSel < FileListNum)
				{
					sSoundFile* fd = &FileList[FileListSel];

					// change directory
					if (fd->dir)
					{
						// going to up-directory
						if ((fd->namelen == 2) && (fd->name[0] == '.') && (fd->name[1] == '.'))
						{
							// delete last directory
							i = SoundPathLen;
							if (i > 1) while (SoundPath[SoundPathLen-1] != PATHCHAR) SoundPathLen--;

							// save last name
							LastNameLen = i - SoundPathLen;
							memcpy(LastName, &SoundPath[SoundPathLen], LastNameLen);
							LastNameDir = True;

							// delete path separator if not root
							if (SoundPathLen > 1) SoundPathLen--;
						}

						// going to sub-directory
						else
						{
							// check path length
							if (SoundPathLen + 1 + fd->namelen <= SOUNDPATH_MAX)
							{
								// add path separator if not root
								if (SoundPathLen != 1) SoundPath[SoundPathLen++] = PATHCHAR;

								// add path name
								memcpy(&SoundPath[SoundPathLen], fd->name, fd->namelen);
								SoundPathLen += fd->namelen;
							}
						}

						// set terminating 0
						SoundPath[SoundPathLen] = 0;

						// request to reload current directory
						FileListTop = 0;
						Reload();

						// invalidate last mount time = current time - 2 seconds
						LastMount = Time()-2000000;
					}
					else
					{
						// play MP3 file
						PlayMedia();
						ViewSetup();	// view window correction
						DispFileList();
						DispUpdate();
					}

					KeyFlush();
				}
				break;

			// Space play all
			case KEY_SPACE:
				if (FileListMP3 >= 0)
				{
					FileListSel = FileListMP3;
					PlayMedia();
					ViewSetup();	// view window correction
					DispFileList();
					DispUpdate();
				}
				KeyFlush();
				break;
			}
		}
	}

	return 0;
}
