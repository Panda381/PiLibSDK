
// ****************************************************************************
//
//                                Game engine
//
// ****************************************************************************

#include "../include.h"

// buffers
u8 Board[MAPSIZE];	// game board
u8 Dir[MAPSIZE];	// direction map DIR_*
u8 PswBuf[PSWLEN];	// password buffer

// current game state
u8 Level;		// current level
u8 HeadX, HeadY;	// head (loco) coordinates
u8 GateX, GateY;	// gate coordinates
u16 Length;		// length of the (including loco)
u16 ItemNum;		// number of items on game board
u8 State;		// game state S_*
u8 Phase;		// current animation phase (0, 1, 2)
u8 CurDir;		// current direction DIR_*
int Score;		// current score
u32 LastTime;		// time of last step

// key buffers for 2 keys (NOKEY=no key)
char KeyBuf1, KeyBuf2;

// texts
const char LevelTxtEN[] = "LEVEL";
const char LevelTxtCZ[] = "SCENA";

const char PswTxtEN[] = "pswrd";
const char PswTxtCZ[] = "heslo";

const char LevelHlpEN[] = "KEY-A:PSWRD";
const char LevelHlpCZ[] = "KEY-A:HESLO";

const char OkHlp[] = "KEY-A:OK";

const char EscHlpEN[] = "KEY-B:CANCEL";
const char EscHlpCZ[] = "KEY-B:STORNO";

const char CongTxtEN[] = "CONGRATULATIONS!";
const char Cong2TxtEN[] = "You became absolute";
const char Cong3TxtEN[] = "winner of the game!";

const char CongTxtCZ[] = "  BLAHOPREJI !  ";
const char Cong2TxtCZ[] = "Stal ses absolutnim";
const char Cong3TxtCZ[] = " vitezem teto hry! ";

const char ScoreHlpEN[] = "SCORE";
const char ScoreHlpCZ[] = "SKORE";

// Jokes ... max. 34 characters
const char* JokesCZ[] = {
	"Kdo jinemu jamu kopa...",
	"A zase vedle...",
	"Tak uz to konecne vzdej!",
	"Uz moc nechybelo!",
	"A uz zase?",
	"To snad delas schvalne?",
	"Co takhle se trochu soustredit?",
	"Dej si radeji pauzu.",
	"Nezoufej, to se stane.",
	"Netluc pocitac, nemuze za to!",
	"Nevolal nekdo o pomoc?",
	"Priznej si, ze na to uz nemas!",
	"I vyvalily se vlny zdola...",
	"Zivot je zivot...",
	"To vis, je to jako v zivote.",
	"Nebrec, mohlo to byt i horsi!",
	"HA, HA, HA. DOSTAL JSEM TE!",
	"A mas co sis zaslouzil!",
	"Na to jsem se jiz dlouho tesil!",
	"Nema tady nekdo naplast?",
	"A l a r m  !   H O R I  !",
	"Takhle to asi nemelo byt, ze?",
	"Tak kudy to asi bude?",
	"Tudy tedy cesta nevede.",
	"Chtelo to jen o kousek vedle.",
	"Neni to k zblazneni?",
	"Mne nenadavej, ja za to nemohu.",
	"Snad priste.",
	"Co takhle to zkusit trochu jinak?",
	"Neni skoda vse takhle nicit?",
	"Zkus si nechat zmerit IQ!",
	"Pozor na krevni tlak!",
	"A nedas si pokoj a nedas!",
	"Necitite kour?",
	"Zkus pouzit tlacitko RESET!",
	"To byla ale rana!",
	"Snad to nevzdavas?",
	"Pres eny kej tu kontinju...",
	"Chyba lavky, zase nic.",
	"A co jinak doma, vsichni zdravi?",
	"Pozvi si odborneho poradce.",
	"Tak dost legracek, a ted uz vazne!",
	"Tak kolik to bylo dnes rumiku?",
	"Chybami se clovek uci.",
	"Ma babicka by to zvladla lip!",
	"Jen malicky kousek chybel!",
	"To je ale smula!",
	"Jede jede masinka ... a uz dojela!",
	"Maminka ti to pofouka!",
	"Priste presneji: sin(a)*cos(b)+pi",
	"MU-HA-HA-HA-HA-HA-HA !!!",
	"To jsme se ale nasmali, co?",
	"Kam jedes Pepiku? Do doliku!",
	"Pozor, ZAKRUTA!",
	"Prezili to vsichni?",
	"Nebreci tu nekdo?",
	"Ceske drahy preji hezkou cestu!",
	"Co radeji zkusit bicykl?",
	"MAMIII ! JA MAM BEBICKO !",
	"Zlomene srdce je horsi.",
	"Potrebujes roztlacit?",
	"Potrebujes pohladit?",
	"Kam se vleces s tim srotem?",
	"Nekdo tu nerozezna vlevo a vpravo!",
	"Vlacek Kolejacek je Bouracek.",
	"Je cas udelat PA-PA!",
	"Tak asi to dnes uz nedas.",
	"Tesi te to takhle bourat?",
	"Jsem tu snad pro srandu kralikum?",
	"Jeste ze to bylo pojistene!",
	"Dobry, odnesl to jenom patnik.",
	"Necum do mobilu a davej pozor!",
	"Pekelne se soustred!",
	"To byla zkouska, a ted uz vazne.",
	"Tak uz vime jak to vypadat nema.",
	"Po 101. uz se to jiste podari!",
	"Uz toho fakt nech!",
	"Byl tam kopecek - asi krtecek!",
	"A jsme v CILI! ... Tak jeste ne.",
	"Dnes neni tvuj stastny den.",
	"Nerozptyluj se!",
	"Dame si to jeste jednou?",
	"Ne a ne! Fakt to nejde!",
	"Ta zed tu pred chvili nestala!",
	"Jeste ze to nemusis platit!",
	"Tudy NE!",
	"Ani tudy NE!",
	"Zkus to jinak!",
	"Vis to, ze tohle neni cilem hry?",
	"Ne do zdi, ale do brany!",
};

// Jokes ... max. 34 characters
const char* JokesEN[] = {
	"What goes around comes around...",
	"Missed again...",
	"Just give up already!",
	"You were so close!",
	"Again?",
	"Are you doing this on purpose?",
	"How about focusing a little?",
	"You'd better take a break.",
	"Don't despair, these things happen",
	"Don't smash the computer!",
	"Did someone call for help?",
	"You really can't handle this!",
	"And waves rolled in from below.",
	"Life is life...",
	"You know, it's just like life.",
	"Don't cry, it could've been worse!",
	"HA, HA, HA. I GOT YOU!",
	"And you got what you deserved!",
	"I was looking forward to this!",
	"Does anyone have a band-aid here?",
	"A l a r m  !   F I R E  !",
	"Wasn't this supposed to happen?",
	"So which way do you think it goes?",
	"The path doesn't go this way.",
	"It was just a little off the mark.",
	"Isn't this crazy?",
	"It's not my fault!",
	"Maybe next time.",
	"How about trying it differently?",
	"Why are you destroying it?",
	"Try getting your IQ tested!",
	"Watch your blood pressure!",
	"You just won't give it a rest!",
	"Can't you smell the smoke?",
	"Try pressing the RESET button!",
	"That was quite a blow!",
	"You're not giving up, are you?",
	"Press any key to continue...",
	"Oops, nothing again.",
	"Is everyone at home okay?",
	"You should call expert advisor.",
	"Enough joking around, be serious!",
	"How many drinks were there today?",
	"We learn from our mistakes.",
	"My grandma could've done better!",
	"You were just a tiny bit off!",
	"What bad luck!",
	"Little train ... already arrived!",
	"Mom will blow on it for you!",
	"Next time try: sin(a)*cos(b)+pi",
	"MU-HA-HA-HA-HA-HA-HA!!!",
	"We had a good laugh, didn't we?",
	"Where are you going, little Pedro?",
	"Watch out, SHARP TURN!",
	"Did everyone survive?",
	"Is someone crying here?",
	"We wish you a nice trip!",
	"Why not try a bike instead?",
	"MOMMM ! I HURT MYSELF !",
	"A broken heart is worse.",
	"Need a push?",
	"Do you need a hug?",
	"Where are you dragging that junk?",
	"You can't tell left from right!",
	"Railway train is a Crash-O-Man.",
	"It's time to say BYE-BYE!",
	"You won't make it today.",
	"Do you enjoy crashing like this?",
	"Am I here just for fun?",
	"Good thing it was insured!",
	"Only the curb took the hit.",
	"Stop staring at your phone!",
	"Focus like hell!",
	"That was test, and now be serious.",
	"So now we know what not to do.",
	"On the 101st try, you'll make it!",
	"Just give it up already!",
	"There was a bump - maybe a mole!",
	"And we're at the FINISH LINE!",
	"Today isn't your lucky day.",
	"Don't get distracted!",
	"Shall we try it one more time?",
	"No way! It really won't work!",
	"That wall wasn't there moment ago!",
	"Great you don't have to pay this!",
	"NOT this way!",
	"NOT that way either!",
	"Try a different approach!",
	"You know this isn't point of game?",
	"Not into the wall, try the gate!",
};

// wait step
void WaitStep()
{
	DispUpdate();
	u32 t;
	for (;;)
	{
		t = Time();
		if ((u32)(t - LastTime) >= GAMESPEED*1000) break;
	}
	LastTime = t;
}

// clear info row
void InfoClear()
{
	DrawRect(0, MAPY+MAPH*TILESIZE, WIDTH, 32, COL_BLACK);
}

// display text to info row
void InfoDispTextBg(int x, const char* txt, u32 col, u32 bgcol)
{
	SelFont8x16();
	DrawTextBg2(txt, x, MAPY+MAPH*TILESIZE, col, bgcol);
}

// display text to info row
void InfoDispText(int x, const char* txt, u32 col)
{
	InfoDispTextBg(x, txt, col, COL_BLACK);
}

// display info bottom row
void InfoRow()
{
	// clear info row
	InfoClear();

	// display score
	InfoDispText(0, (Lang == LANG_CZ) ? ScoreHlpCZ : ScoreHlpEN, COL_GREEN);
	char buf[10];
	DecUNum(buf, Score, 0);
	InfoDispText(6*8*2, buf, COL_YELLOW);
	
	// display text ENTER: PSW
	InfoDispText(14*8*2, (Lang == LANG_CZ) ? LevelHlpCZ : LevelHlpEN, COL_RED);

	// display level
	InfoDispText(WIDTH - 8*8*2, (Lang == LANG_CZ) ? LevelTxtCZ : LevelTxtEN, COL_GREEN);
	DecUNum(buf, Level, 0);
	InfoDispText(WIDTH - 2*8*2, buf, COL_YELLOW);
}

// enter password
void Psw()
{
	// display help
	InfoClear();

	// display help
	InfoDispText(0, OkHlp, COL_GREEN);
	InfoDispText(WIDTH - (sizeof(EscHlpEN)-1)*8*2, (Lang == LANG_CZ) ? EscHlpCZ : EscHlpEN, COL_GREEN);

	// load password
	char buf[PSWLEN+1];
	memcpy(buf, &LevelPsw[Level*PSWLEN], PSWLEN);
	buf[PSWLEN] = 0;

	// display password
	const char* pswtxt = (Lang == LANG_CZ) ? PswTxtCZ : PswTxtEN;
	int pswlen = sizeof(PswTxtEN);
	int x = (WIDTH - (pswlen - 1 + 1 + 5)*8*2)/2 - 16*2;
	InfoDispText(x, pswtxt, COL_RED);
	x += (pswlen - 1 + 1)*8*2;

	// edit password
	int pos = 0;
	char c;
	char buf2[2];
	buf2[1] = 0;
	for (;;)
	{
		// display password
		InfoDispText(x, buf, COL_YELLOW);
		buf2[0] = buf[pos];
		InfoDispTextBg(x + pos*8*2, buf2, COL_BLACK, COL_YELLOW);
		DispUpdate();

		// input keys
		c = KeyGet();

		if (c == KEY_SCREENSHOT) { ScreenShot(); c = NOKEY; }
		if (c == KEY_ZOOM) { LCDRezoom(); c = NOKEY; }

		// Esc break
		if ((c == KEY_PAD_B) || (c == KEY_PAD_X) || (c == KEY_PAD_Y))
		{
			InfoRow();
			return;
		}

		// OK enter
		if (c == KEY_PAD_A)
		{
			// search password
			int i;
			for (i = 1; i <= LEVNUM; i++)
			{
				// check password
				if (memcmp(buf, &LevelPsw[i*PSWLEN], PSWLEN) == 0)
				{
					// set new level
					Level = i;
					break;
				}
			}

			// restart this scene
			InfoRow();
			OpenLevel();
			InitLevel();
			return;
		}

		// Left
		if (c == KEY_LEFT)
		{
			pos--;
			if (pos < 0) pos = PSWLEN-1;
		}

		// right
		if (c == KEY_RIGHT)
		{
			pos++;
			if (pos >= PSWLEN) pos = 0;
		}

		// up
		if (c == KEY_UP)
		{
			buf[pos]++;
			if ((u8)buf[pos] > (u8)'Z') buf[pos] = 'A';
		}

		// down
		if (c == KEY_DOWN)
		{
			buf[pos]--;
			if ((u8)buf[pos] < (u8)'A') buf[pos] = 'Z';
		}
	}
}

// display one tile of the board
void DispTile(u8 x, u8 y)
{
	u8 tile = Board[x + y*MAPW];
	int row = tile / TILESNUMX;
	int col = tile - row*TILESNUMX;
	DrawImg(TilesImg, x*TILESIZE, y*TILESIZE + MAPY, col*TILESIZE, row*TILESIZE, TILESIZE, TILESIZE);
}

// display all tiles
void DispBoard()
{
	int x, y;
	for (y = 0; y < MAPH; y++)
	{
		for (x = 0; x < MAPW; x++) DispTile(x, y);
	}
}

// put tile on the board
void PutTile(u8 x, u8 y, u8 tile)
{
	Board[x + y*MAPW] = tile;
	DispTile(x, y);
}

// get tile from board
u8 GetTile(u8 x, u8 y) { return Board[x + y*MAPW]; }

// clear level screen
void ClearLevel(u8 tile)
{
	u8 x, y;

	// clear screen with bricks
	for (x = 0; x < MAPW; x++)
	{
		for(y = 0; y < MAPH; y++) PutTile(x, y, tile);
		DispUpdate();
		WaitMs(20);
	}
}

// congratulate
void CongLevel()
{
	// clear screen with bricks
	ClearLevel(WALL);

	// set info box ON
	DrawRect(BOXX-3*8*2, BOXY, BOXW+6*8*2, BOXH, COL_BLACK);

	// display frame
	DrawFrame(BOXX-1-3*8*2, BOXY-1, BOXW+2+6*8*2, BOXH+2, COL_WHITE);

	// display text
	if (Lang == LANG_CZ)
	{
		DrawText2(CongTxtCZ, (WIDTH - (sizeof(CongTxtCZ)-1)*8*2)/2, BOXY + 16*2, COL_YELLOW);
		DrawText2(Cong2TxtCZ, (WIDTH - (sizeof(Cong2TxtCZ)-1)*8*2)/2, BOXY + 2*16*2, COL_GREEN);
		DrawText2(Cong3TxtCZ, (WIDTH - (sizeof(Cong3TxtCZ)-1)*8*2)/2, BOXY + 3*16*2, COL_GREEN);
	}
	else
	{
		DrawText2(CongTxtEN, (WIDTH - (sizeof(CongTxtEN)-1)*8*2)/2, BOXY + 16*2, COL_YELLOW);
		DrawText2(Cong2TxtEN, (WIDTH - (sizeof(Cong2TxtEN)-1)*8*2)/2, BOXY + 2*16*2, COL_GREEN);
		DrawText2(Cong3TxtEN, (WIDTH - (sizeof(Cong3TxtEN)-1)*8*2)/2, BOXY + 3*16*2, COL_GREEN);
	}

	// display info row
	InfoRow();

	// update display
	DispUpdate();

	// wait
	WaitMs(250);
	KeyFlush();
	int key;
	do {
		key = KeyGet();
		if (key == KEY_SCREENSHOT) { ScreenShot(); key = NOKEY; }
		if (key == KEY_ZOOM) { LCDRezoom(); key = NOKEY; }
	} while ((key == NOKEY) || (key == KEY_ALT));

	// clear screen with empty
	ClearLevel(EMPTY);
}

// open level
void OpenLevel()
{
	// clear screen with bricks
	ClearLevel(WALL);

	// set info box ON
	DrawRect(BOXX, BOXY, BOXW, BOXH, COL_BLACK);

	// display frame
	DrawFrame(BOXX-1, BOXY-1, BOXW+2, BOXH+2, COL_WHITE);

	// set font
	SelFont8x16();

	// display current scene
	int x, x2;
	char buf[10];
	int n = DecUNum(buf, Level, 0);
	const char* leveltxt = (Lang == LANG_CZ) ? LevelTxtCZ : LevelTxtEN;
	x = (WIDTH - (sizeof(LevelTxtEN)+n)*8*2)/2;
	DrawText2(leveltxt, x, BOXY + 16*2, COL_GREEN);
	x += sizeof(LevelTxtEN)*8*2;
	DrawText2(buf, x, BOXY + 16*2, COL_YELLOW);

	// display password
	const char* pswtxt = (Lang == LANG_CZ) ? PswTxtCZ : PswTxtEN;
	int pswlen = sizeof(PswTxtEN);
	x = (WIDTH - (pswlen+PSWLEN)*8*2)/2;
	DrawText2(pswtxt, x, BOXY + 3*16*2, COL_GREEN);
	memcpy(buf, &LevelPsw[Level*PSWLEN], PSWLEN);
	buf[PSWLEN] = 0;
	x += pswlen*8*2;
	DrawText2(buf, x, BOXY + 3*16*2, COL_YELLOW);

	// display info row
	InfoRow();

	// update display
	DispUpdate();

	// wait
	WaitMs(250);
	KeyFlush();
#if !AUTOMODE	// 1=automode - run levels to check solutions (help completes scene), 0=normal
	int key;
	do {
		key = KeyGet();
		if (key == KEY_SCREENSHOT) { ScreenShot(); key = NOKEY; }
		if (key == KEY_ZOOM) { LCDRezoom(); key = NOKEY; }
	} while ((key == NOKEY) || (key == KEY_ALT));
#endif

	// clear screen with empty
	ClearLevel(EMPTY);
}

// initialize current level
void InitLevel()
{
	// copy items to game board
	int lev = Level;
	if ((lev == 0) && (Lang == LANG_CZ))
	{
		lev = LEVNUM+1;
	}
	memcpy(Board, &Levels[MAPSIZE*lev], MAPSIZE);

	// analyse board
	u8 b;
	int x, y;
	ItemNum = 0;
	for (y = 0; y < MAPH; y++)
	{
		for (x = 0; x < MAPW; x++)
		{
			// get item from board
			b = Board[x+y*MAPW];

			// locomotive
			if ((b >= LOCOMIN) && (b <= LOCOMAX))
			{
				HeadX = x;
				HeadY = y;
			}

			// gate
			else if (b == GATE)
			{
				GateX = x;
				GateY = y;
			}

			// item
			else if ((b >= ITEMMIN) && (b <= ITEMMAX))
			{
				ItemNum++;
			}
		}
	}

	// reset variables
	State = S_WAIT;
	Length = 1;
	CurDir = DIR_R;
	Phase = 0;
	KeyBuf1 = NOKEY;
	KeyBuf2 = NOKEY;

	// display info row
	if (Level == 0)
		InfoClear();
	else
		InfoRow();

	// display board
	DispBoard();

	// time of last step
	LastTime = Time();
}

// train step
void StepLevel()
{
	// next position
	s8 x = HeadX;
	s8 y = HeadY;
	u8 d = CurDir;
	if (d == DIR_L) x--;
	if (d == DIR_U) y--;
	if (d == DIR_R) x++;
	if (d == DIR_D) y++;
	u8* bold = &Board[HeadX + HeadY*MAPW]; // current position
	u8* bnew = &Board[x + y*MAPW]; // new position
	u8 b = *bnew; // item on new position

	// crash
	if ((x < 0) || (x >= MAPW) || // x is out of board
		(y < 0) || (y >= MAPH) || // y is out of board
		((b != EMPTY) && // not empty field
		((b <= GATEMIN) || (b > GATEMAX)) && // not open gate
		((b < ITEMMIN) || (b > ITEMMAX)))) // not valid item
	{
		State = S_CRASH;	// crash state
		b = CRASH;		// crash image

		// joke text
		const char** txt = JokesEN;
		int num = count_of(JokesEN);
		if (Lang == LANG_CZ)
		{
			txt = JokesCZ;
			num = count_of(JokesCZ);
		}
		const char* t = txt[RandU32Max(num-1)];
		int len = StrLen(t);

		SelFont8x14();

		int x = (WIDTH - (len+2)*16)/2;
		DrawCharBg2(' ', x, MAPY+2, COL_GREEN, COL_BLACK);
		x += 16;
		DrawTextBg2(t, x, MAPY+2, COL_GREEN, COL_BLACK);
		x += len*16;
		DrawCharBg2(' ', x, MAPY+2, COL_GREEN, COL_BLACK);

		SelFont8x16();

		// crash animation
		PlaySound(CrashSnd);

		while (True)
		{
			PutTile(HeadX, HeadY, b); // set locomotive crash image
			WaitStep();
			b++;
			if (b > CRASHMAX) b = CRASHMAX - 2;

			u8 key = KeyGet();
			if (key == KEY_SCREENSHOT) { ScreenShot(); key = NOKEY; }
			if (key == KEY_ZOOM) { LCDRezoom(); key = NOKEY; }
			if ((key != NOKEY) && (key != KEY_ALT)) break;
		}

		// restart new level
		InitLevel();
		return;
	}

	// set loco to new position
	u8 b2;
	s8 xold = HeadX;
	s8 yold = HeadY;
	if (d == DIR_L)
		b2 = LOCO_L; // direction left
	else if (d == DIR_U)
		b2 = LOCO_U; // direction up
	else if (d == DIR_R)
		b2 = LOCO_R; // direction right
	else b2 = LOCO_D; // direction down
	PutTile(x, y, b2); // store new locomotive image
	Dir[x + y*MAPW] = d; // store new direction at this point
	HeadX = x; // new locomotive position
	HeadY = y;

	// move wagons
	int i;
	for (i = Length-1; i > 0; i--)
	{
		x = xold;
		y = yold;

		// get position of previous wagon
		d = Dir[x + y*MAPW];
		if (d == DIR_L) x++;
		if (d == DIR_U) y++;
		if (d == DIR_R) x--;
		if (d == DIR_D) y--;

		// change direction of previous wagon
		b2 = GetTile(x, y); // get wagon on new (previous) position
		while (b2 >= WAGONMIN + 20) b2 -= 20;
		if (d == DIR_U) b2 += 20;
		if (d == DIR_R) b2 += 2*20;
		if (d == DIR_D) b2 += 3*20;

		// put wagon on new position
		PutTile(xold, yold, b2);

		xold = x;
		yold = y;
	}

	// collect item, increase train length
	if ((b >= ITEMMIN) && (b <= ITEMMAX))
	{
		// collect sound
		PlaySound(CollectSnd);

		// increase train length
		Length++;

		// new wagon
		b += WAGONMIN - ITEMMIN;
		while (b >= WAGONMIN + 20) b -= 20;
		d = Dir[xold + yold*MAPW];
		if (d == DIR_U) b += 20;
		if (d == DIR_R) b += 2*20;
		if (d == DIR_D) b += 3*20;
		PutTile(xold, yold, b);

		// decrease number of items
		ItemNum--;

		// start opening gate
		if (ItemNum == 0) PutTile(GateX, GateY, GATEMIN+1);

		// count score
		if (State == S_GO)
		{
			Score += 10;
			InfoRow();
		}
	}
	else
	{
		// step sound
		PlaySound(StepSnd);

		// empty last position
		PutTile(xold, yold, EMPTY);
	}

	// gate
	if ((HeadX == GateX) && (HeadY == GateY))
	{
		int s = State;
		State = S_FINISH;
		DispUpdate();
		WaitMs(500);
		KeyFlush();

#if !AUTOMODE	// 1=automode - run levels to check solutions (help completes scene), 0=normal
		if (s == S_HELP) return;
#endif

		// play sound
		PlaySound(SuccessSnd);
		WaitMs(1000);

		// open next level
		if (Level < LEVNUM)
		{
			Level++;
			OpenLevel();
		}
		else
			CongLevel();
		InitLevel();
	}
}

// level animation - called every single game step
void AnimLevel()
{
	// set phase of the objects
	int x, y;
	u8 b;
	for (y = 0; y < MAPH; y++)
	{
		for (x = 0; x < MAPW; x++)
		{
			b = GetTile(x, y);

			// animate items
			if ((b >= ITEMMIN) && (b <= ITEMMAX))
			{
				while (b >= ITEMMIN+20) b -= 20;
				b += Phase*20;
				PutTile(x, y, b);
			}

			// animate locomotive
			else if ((b >= LOCOMIN) && (b <= LOCOMAX))
			{
				while (b >= LOCOMIN+4) b -= 4;
				b += Phase*4;
				PutTile(x, y, b);
			}

			// animate gate
			else if ((b > GATEMIN) && (b < GATEMAX))
			{
				PutTile(x, y, b + 1);
			}
		}
	}

	// increase animation phase
	Phase++;
	if (Phase >= 3)
	{
		Phase = 0;

		// change direction by keyboard (and potentially start the game)
		char c = KeyBuf1;
		KeyBuf1 = KeyBuf2;
		KeyBuf2 = NOKEY;
		if (c != NOKEY)
		{
			if (State == S_WAIT) State = S_GO;
			if (c == KEY_DOWN) CurDir = DIR_D;
			if (c == KEY_RIGHT) CurDir = DIR_R;
			if (c == KEY_LEFT) CurDir = DIR_L;
			if (c == KEY_UP) CurDir = DIR_U;
		}

		// one step
		if ((State == S_GO) || (State == S_HELP)) StepLevel();
	}
}

// help level (returns 0=continue, 1=start game, 2=exit)
int HelpLevel()
{
	u8 key;
	char b;

	// re-initialize current scene
	InitLevel();

	// help mode
	State = S_HELP;

	// prepare pointer to level solution
	const char* s = Solution[Level];

	// macro loop
	while (State == S_HELP)
	{
		// get next key from solution macro
		b = *s++;
		if (b == 0)
		{
			State = S_FINISH;

			// wait 1.6 seconds on end of scene
			for (b = 10; b > 0; b--)
			{
				key = KeyGet();
				if (key == KEY_SCREENSHOT) { ScreenShot(); key = NOKEY; }
				if (key == KEY_ZOOM) { LCDRezoom(); key = NOKEY; }
				if (key == KEY_PAD_Y) return 2;
				if ((key != NOKEY) && (key != KEY_ALT)) return 1;
				AnimLevel();
				// wait step
				WaitStep();
			}
			return False;
		}
		if (b == 'R') CurDir = DIR_R;
		if (b == 'U') CurDir = DIR_U;
		if (b == 'L') CurDir = DIR_L;
		if (b == 'D') CurDir = DIR_D;

		key = KeyGet();
		if (key == KEY_SCREENSHOT) { ScreenShot(); key = NOKEY; }
		if (key == KEY_ZOOM) { LCDRezoom(); key = NOKEY; }
		if (key == KEY_PAD_Y) return 2;
		if ((key != NOKEY) && (key != KEY_ALT)) return 1;
		AnimLevel();

		WaitStep();

		key = KeyGet();
		if (key == KEY_SCREENSHOT) { ScreenShot(); key = NOKEY; }
		if (key == KEY_ZOOM) { LCDRezoom(); key = NOKEY; }
		if (key == KEY_PAD_Y) return 2;
		if ((key != NOKEY) && (key != KEY_ALT)) return 1;
		AnimLevel();

		WaitStep();

		key = KeyGet();
		if (key == KEY_SCREENSHOT) { ScreenShot(); key = NOKEY; }
		if (key == KEY_ZOOM) { LCDRezoom(); key = NOKEY; }
		if (key == KEY_PAD_Y) return 2;
		if ((key != NOKEY) && (key != KEY_ALT)) return 1;
		AnimLevel();

		// text on demo screen
		if (Level == 0)
		{
			if (HeadY == 9)
			{
				if ((HeadX >= 3) && (HeadX < 18))
					DrawImg(TilesImg, 3*32, 9*32+MAPY, 0, (Lang == LANG_CZ) ? (9*32) : (10*32), (HeadX-3)*TILESIZE, 32);
			}
		}

		// step
		WaitStep();

		// break help
		key = KeyGet();
		if (key == KEY_SCREENSHOT) { ScreenShot(); key = NOKEY; }
		if (key == KEY_ZOOM) { LCDRezoom(); key = NOKEY; }
		if (key == KEY_PAD_Y) return 2;
		if ((key != NOKEY) && (key != KEY_ALT)) return 1;
	}

	// no break from keyboard
	return 0;
}

// game loop - called every single game step (returns True on break Esc)
Bool GameLoop()
{
	// animation
	AnimLevel();

	// key input
	char c = KeyGet();

#if AUTOMODE	// 1=automode - run levels to check solutions (help completes scene), 0=normal
	c = KEY_PAD_X;
#endif

	if (c == NOKEY) return False;

	switch (c)
	{
	// enter password
	case KEY_PAD_A:
		Psw();
		break;

	// restart
	case KEY_PAD_Y:
		// break
		if (State != S_GO) return True; 

		// restart scene
		InitLevel();
		break;

	case KEY_SCREENSHOT:
		ScreenShot();
		break;

	case KEY_ZOOM:
		LCDRezoom();
		break;

	// help
	case KEY_PAD_X:
#if AUTOMODE	// 1=automode - run levels to check solutions (help completes scene), 0=normal
		if (HelpLevel() == 2) return True;
#else
		HelpLevel();
#endif
		InitLevel();
		break;

	// direction
	case KEY_RIGHT:
	case KEY_UP:
	case KEY_LEFT:
	case KEY_DOWN:
		// save key into key buffer
		if (KeyBuf1 == NOKEY)
			KeyBuf1 = c;
		else
		{
			if (KeyBuf2 == NOKEY)
				KeyBuf2 = c;
			else
			{
				KeyBuf1 = KeyBuf2;
				KeyBuf2 = c;
			}
		}
		break;

	// unknown key
	default:
		KeyFlush();
	}

	// no break
	return False;
}
