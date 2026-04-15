
// ****************************************************************************
//
//                                 Game engine
//
// ****************************************************************************

#include "../include.h"

const int CastleX[2] = { WIDTH/2 - 144, WIDTH/2 + 144 }; // castle X middle coordinate
const int FenceX[2] = { WIDTH/2 - 44, WIDTH/2 + 44 }; // fence X middle coordinate
const int StateX[2] = { 8, WIDTH-STATEW-8}; // state X coordinate

// save screen content (including 8-byte sPic header)
u32 SaveCardBuf[CARDW*CARDH + SPIC_HEADER_SIZE/4];
u32 SaveCardBuf2[CARDW*CARDH + SPIC_HEADER_SIZE/4];
u32 SaveCloudBuf[CLOUDW*CLOUDH*PAR_NUM*2 + SPIC_HEADER_SIZE/4];
u32 SaveWinBuf[WINW*WINH + SPIC_HEADER_SIZE/4];

// player state
sPlayer Players[2];	// player's game parameters
Bool Demo;		// demo mode
int Player;		// active player (0 or 1)
int Last;		// last laid card (-1 = none)
Bool LastDiscard;	// last card was discarded
int SelCard;		// selected card (cursor)
int FreePos;		// free card position
int EndGame = -1;	// end game, player's victory (-1 = no win)
int SelectInc = 0;	// select cursor increment
int SelectIncOld = 0;	// old time of select cursor

// sound mode
int SoundMode = SOUND_ALL;

// parameter foreground color
const u32 ParFgCol[8] = { COL_YELLOW, COL_WHITE, COL_YELLOW, COL_WHITE,
			COL_YELLOW, COL_WHITE, COL_WHITE, COL_WHITE };

// card table
const sCard Card[CARDTYPES] = {

// bricks
	{	PAR_BRICKS,	1,	PAR_FENCE,	+3 },	// 0: wall
	{	PAR_BRICKS,	1,	PAR_CASTLE,	+2 },	// 1: base
	{	PAR_BRICKS,	3,	PAR_FENCE,	+6 },	// 2: defense
	{	PAR_BRICKS,	3,	PAR_NUM,	0 },	// 3: reserve (castle +8, fence -4)
	{	PAR_BRICKS,	5,	PAR_CASTLE,	+5 },	// 4: tower
	{	PAR_BRICKS,	8,	PAR_BUILDERS,	+1 },	// 5: school
	{	PAR_BRICKS,	10,	PAR_NUM,	0 },	// 6: wain (castle +8, enemy castle -4)
	{	PAR_BRICKS,	12,	PAR_FENCE,	+22 },	// 7: fence
	{	PAR_BRICKS,	18,	PAR_CASTLE,	+20 },	// 8: fort
	{	PAR_BRICKS,	39,	PAR_CASTLE,	+32 },	// 9: Babylon

// weapons
	{	PAR_WEAPONS,	1,	PAR_FENCE,	-2 },	// 10: archer
	{	PAR_WEAPONS,	2,	PAR_FENCE,	-3 },	// 11: knight
	{	PAR_WEAPONS,	2,	PAR_FENCE,	-4 },	// 12: rider
	{	PAR_WEAPONS,	4,	PAR_FENCE,	-6 },	// 13: platoon
	{	PAR_WEAPONS,	8,	PAR_SOLDIERS,	+1 },	// 14: recruit
	{	PAR_WEAPONS,	10,	PAR_FENCE,	-12 },	// 15: attack
	{	PAR_WEAPONS,	12,	PAR_NUM,	0 },	// 16: saboteur (enemy stocks -4)
	{	PAR_WEAPONS,	15,	PAR_NUM,	0 },	// 17: thief (transfer enemy stocks 5)
	{	PAR_WEAPONS,	18,	PAR_CASTLE,	-10 },	// 18: swat
	{	PAR_WEAPONS,	28,	PAR_FENCE,	-32 },	// 19: banshee

// crystals
	{	PAR_CRYSTALS,	4,	PAR_BRICKS,	+8 },	// 20: conjure bricks
	{	PAR_CRYSTALS,	4,	PAR_BRICKS,	-8 },	// 21: crush bricks
	{	PAR_CRYSTALS,	4,	PAR_WEAPONS,	+8 },	// 22: conjure weapons
	{	PAR_CRYSTALS,	4,	PAR_WEAPONS,	-8 },	// 23: crush weapons
	{	PAR_CRYSTALS,	4,	PAR_CRYSTALS,	+8 },	// 24: conjure crystals
	{	PAR_CRYSTALS,	4,	PAR_CRYSTALS,	-8 },	// 25: crush crystals
	{	PAR_CRYSTALS,	8,	PAR_WIZARDS,	+1 },	// 26: sorcerer
	{	PAR_CRYSTALS,	21,	PAR_FENCE,	-25 },	// 27: dragon
	{	PAR_CRYSTALS,	22,	PAR_CASTLE,	+22 },	// 28: pixies
	{	PAR_CRYSTALS,	25,	PAR_NUM,	0 },	// 29: curse (all +1, enemie's all -1)
};

// generate new card
int NewCard()
{
	// unlinear randomness - cards with low number will be most plentiful
	int card = (int)(powf((RandFloat()*0.99f), 1.6f) * CARDTYPES);
	
	// change organization of card order - 3 groups from weakest to strongest cards
	card = (card/3) + (card % 3)*10;
	return card;
}

// test cards permission
void Permission()
{
	// initialize players
	int i, j, card, costs, par;
	sPlayer* p = Players;
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < CARDNUM; j++)
		{
			// check card cost
			card = p->cards[j];
			if (card < 0) // invalid card
				p->disable[j] = True;
			else
			{
				costs = Card[card].costs;
				par = Card[card].type;
				p->disable[j] = costs > p->par[par];
			}
		}

		// next player
		p++;
	}
}

char NumBuf[20];

// display state of one player
void DispState1(int player)
{
	int i, n;
	sPlayer* p;
	p = &Players[player];

	// display status background
	int x = StateX[player];
	DrawImg(StateImg, x, STATEY);

	// display status labels
	DrawImg(StateImg2, x, STATEY);

	// display values
	SelFont8x16();
	x += 43 + 12;
	int y = STATEY+10;
	for (i = 0; i < PAR_NUM; i++)
	{
		// decode parameter
		n = p->par[i];
		if (n > 999) n = 999;
		n = DecNum(NumBuf, n, 0);
		n = x - n*4;

		// display parameter
		DrawText(NumBuf, n, y, ParFgCol[i]);
		y += STATEDY;
		if (i == 5) y += STATEDY/2;
	}
}

// display states of players
void DispState()
{
	DispState1(0);
	DispState1(1);
}

// display grass
void DispGrass(int player)
{
	// display grass
	DrawImg(GrassImg, CastleX[player] - GRASSW/2, GRASSY);
}

// display castle of one player
void DispCastle(int player)
{
	// prepare castle height
	int h = Players[player].par[PAR_CASTLE]; // h is in range 0..100
	if (h > CASTLEMAX) h = CASTLEMAX; // castle size = 0..100

	// shadow height = 16..116 pixels
	int pixh = h + 16;

	// display shadow
	DrawImg(CastleShadowImg, CastleX[player] - CASTLEW/2, CASTLEY-pixh, CASTLEMAX-h, 0, IMG_MAXWH, pixh);

	// display castle
	pixh = pixh*2; // castle height
	DrawImg(CastleImg, CastleX[player] - CASTLEW/2, CASTLEY-pixh, player*CASTLEW, 0, CASTLEW, pixh);
}

// dispay fence of one player
void DispFence(int player)
{
	// prepare fence height
	int h = Players[player].par[PAR_FENCE]; // h is in range 0..100
	if (h > CASTLEMAX) h = CASTLEMAX;

	// shadow height = 0..100 pixels
	int pixh = h;

	// display shadow
	DrawImg(FenceShadowImg, FenceX[player] + FENCEW/2, CASTLEY-116, 0, 0, pixh);

	// display fence
	pixh = pixh*2 + 16; // fence height
	DrawImg(FenceImg, FenceX[player] - FENCEW/2, CASTLEY-pixh, 0, 0, FENCEW, pixh);
}

// display wins
void DispWins(int player)
{
	SelFont8x16();
	int n = DecNum(NumBuf, 	Players[player].wins, 0);
	int x = ((player == 0) ? 8 : (WIDTH-BLACKSREDSW-8)) + 32;
	x -= DrawTextPropLenDX(NumBuf, n)/2;
	DrawTextPropLen(NumBuf, n, x, 0, COLOR(0,160,0));
}

// display titles - player selection
void DispTitle()
{
	DrawImg(BlacksRedsImg, 8, 32, 0, 0, BLACKSREDSW, BLACKSREDSH);
	DrawImg(BlacksRedsImg, WIDTH-BLACKSREDSW-8, 32, BLACKSREDSW, 0, BLACKSREDSW, BLACKSREDSH);
}

// select cursor animation
void DispSelect()
{
	// get cursor phase
	int t = (Time() >> 17) & 1;
	int i = SelectInc;
	if (t != SelectIncOld)
	{
		SelectIncOld = t;
		i++;
		if (i > 2) i = 0;
		SelectInc = i;
	}

	// draw cursor
	int x = (Player == 0) ? 4 : WIDTH-BLACKSREDSW-8-4;
	DrawImg(SelectImg, x, 28, i*80, 0, 80);
}

// flag animation
void DispFlag()
{
	int t = (Time() >> 17) & 3;
	int player;
	int i, k;
	sPlayer* p;
	for (player = 0; player < 2; player++)
	{
		p = &Players[player];

		// get castle height
		int h = Players[player].par[PAR_CASTLE]; // h is in range 0..100
		if (h > CASTLEMAX) h = CASTLEMAX;
		int pixh = h*2 + 32; // castle height is in range 32..232

		// prepare flag coordinate
		int x = CastleX[player] - FLAGSW/2;
		int y = CASTLEY - pixh + 32 - FLAGSH;

		// draw background
		DrawImg(BackgroundImg, x, y, x, y, FLAGSW, FLAGSH);

		// draw flag
		DrawImg(FlagsImg, x, y, (t + player*4)*FLAGSW, 0, FLAGSW);
	}
}

// delay with flag animation (del = delay in multiply of 50ms)
void WaitFlag(int del)
{
	u32 t1 = Time();
	u32 t2 = del * 50000;
	while(True)
	{
		DispSelect();
		DispFlag();
		DispUpdate();
		if ((u32)(Time() - t1) >= t2) break;
	}
}

// display one card
void DispCard(int type, int x, int y, int shadow, Bool disable, Bool back, Bool discard)
{
	// invalid card
	if (type < 0) return;

	// display card shadow
	if (shadow != 0) DrawImg(CardBaseImg, x + shadow, y + shadow, 4*CARDW, 0, CARDW);

	// display card back side
	if (back)
	{
		// display card back side
		DrawImg(CardBaseImg, x, y, 3*CARDW, 0, CARDW);
	}

	else
	{
		// display card base
		DrawImg(CardBaseImg, x, y, (type/10)*CARDW, 0, CARDW);

		// display card image
		DrawImg(CardImage, x, y, type*CARDW, 0, CARDW);

		// display card text
		DrawImg(CardTextImg, x, y, type*CARDW, 0, CARDW);

		// card is disabled
		if (disable || discard) DrawImg(CardBaseImg, x, y, 4*CARDW, 0, CARDW);

		// card is discarded
		if (discard) DrawImg(DiscardImg, x, y);
	}
}

// save screen content under card
void SaveCard(u32* buf, int x, int y)
{
	int h, w;

	// initialize sPic header
	sPic* p = (sPic*)buf;
	p->w = CARDW;
	p->h = CARDH;
	p->wb = CARDW*4;
	p->colfmt = CF_A8B8G8R8;
	p->bits = 32;
	buf += SPIC_HEADER_SIZE/4;

	// copy data
	int wb = FrameBuffer.drawpitchpix;
	u32* s = &FrameBuffer.drawbuf[x + y*wb];
	wb -= CARDW;
	for (h = CARDH; h > 0; h--)
	{
		for (w = CARDW; w > 0; w--)
		{
			*buf = *s | 0xff000000;
			buf++;
			s++;
		}
		s += wb;
	}
}

// card animation
void AnimCard(int type, int x1, int y1, int x2, int y2, Bool back, Bool discard)
{
	// play sound
	if (SoundMode > 0) PlaySound(CardSnd, SOUNDCHAN_CARD);

#define ANIMSTEPS 12 // number of steps of animation

	int i, x, y, xs, ys, s;
	u32 t;
	for (i = 0; i < ANIMSTEPS; i++)
	{
		// time of step begin
		t = Time();

		// current card position
		x = (x2 - x1)*i/ANIMSTEPS + x1;
		y = (y2 - y1)*i/ANIMSTEPS + y1;

		// shadow position
		s = (int)(SHADOWOFF + 64*sinf(PI*i/ANIMSTEPS));
		xs = x + s;
		ys = y + s;

		// selection animation
		DispSelect();

		// flag animation
		DispFlag();

		// save shadow screen content
		SaveCard(SaveCardBuf, xs, ys);

		// save card screen content
		SaveCard(SaveCardBuf2, x, y);

		// draw card
		DispCard(type, x, y, s, False, back, discard);

		// display update
		DispUpdate();

		// restore screen content
		DrawImg((const u8*)SaveCardBuf2, x, y);
		DrawImg((const u8*)SaveCardBuf, xs, ys);

		// wait step time 25ms
		while ((u32)(Time() - t) < 25000) {}
	}
}

// display cards of active player
void DispCards()
{
	int x = CARDX;
	sPlayer* p = &Players[Player];
	int i = 0;
	for (i = 0; i < CARDNUM; i++)
	{
		if (p->cards[i] < 0) // no card
			DrawImg(BackgroundImg, x, CARDY, x, CARDY, CARDW+SHADOWOFF, CARDH+SHADOWOFF);
		else
			DispCard(p->cards[i], x, CARDY, SHADOWOFF, p->disable[i],
				(p->player != PLAYER_HUMAN) && !Demo, False);
		x += CARDDX;
	}
}

// cursor ON
void CursorOn()
{
	if (Players[Player].player == PLAYER_HUMAN)
	{
		DrawImg(CursorImg, CARDX + SelCard*CARDDX + CARDW/2 - CURSORW/2,
			HEIGHT-CARDH-8 + CARDH/2 - CURSORH/2);
	}
}

// cursor OFF
void CursorOff()
{
	sPlayer* p = &Players[Player];
	DispCard(p->cards[SelCard], CARDX + SelCard*CARDDX, HEIGHT-CARDH-8, 0,
		p->disable[SelCard], (p->player != PLAYER_HUMAN) && !Demo, False);
}

// change values animation (anim = length)
void ChangeAnim(Bool comp)
{
	int i, player, x, y, n, par, save, w, h;
	u32 *s, *d;

	// prepare save cloud buffer
	sPic* pic = (sPic*)SaveCloudBuf;	
	pic->w = CLOUDW;
	pic->h = CLOUDH*PAR_NUM*2;
	pic->wb = CLOUDW*4;
	pic->colfmt = CF_A8B8G8R8;
	pic->bits = 32;

	// display values
	SelFont8x16();

	// length of animation
	i = (comp) ? 6 : 3;
	for (; i > 0; i--)
	{
		// display values
		save = 0;
		for (player = 0; player < 2; player++)
		{
			// prepare coordinates
			x = (player == 0) ? (StateX[0]+STATEW-5) : (StateX[1]-CLOUDW+7);
			y = STATEY + 6;
			sPlayer* p = &Players[player];

			// loop parameters
			for (par = 0; par < PAR_NUM; par++)
			{
				// parameter changed
				if (p->add[par] != 0)
				{
					// push screen
					s = &FrameBuffer.drawbuf[x + y*WIDTH];
					d = &SaveCloudBuf[save + SPIC_HEADER_SIZE/4];
					for (h = CLOUDH; h > 0; h--)
					{
						for (w = CLOUDW; w > 0; w--)
						{
							*d = *s | 0xff000000;
							d++;
							s++;
						}
						s += FrameBuffer.drawpitchpix - CLOUDW;
					}

					// draw cloud
					DrawImg(CloudImg, x, y);

					// draw value
					if (p->add[par] >= 0)
					{
						NumBuf[0] = '+';
						n = DecNum(NumBuf+1, p->add[par], 0)+1;
						DrawText(NumBuf, x+13-n*4, y+3, COLOR(0, 180, 0));
					}
					else
					{
						n = DecNum(NumBuf, p->add[par], 0);
						DrawText(NumBuf, x+13-n*4, y+3, COLOR(255, 0, 0));
					}
					save += CLOUDW*CLOUDH;
				}
				y += STATEDY;
				if (par == 5) y += STATEDY/2;
			}
		}

		// display update
		DispUpdate();

		// delay with flag animation
		WaitFlag(2);

		// pop screen
		save = 0;
		for (player = 0; player < 2; player++)
		{
			// prepare coordinates
			x = (player == 0) ? (StateX[0]+STATEW-5) : (StateX[1]-CLOUDW+7);
			y = STATEY + 6;
			sPlayer* p = &Players[player];

			// loop parameters
			for (par = 0; par < PAR_NUM; par++)
			{
				// parameter changed
				if (p->add[par] != 0)
				{
					// pop screen
					DrawImg((const u8*)SaveCloudBuf, x, y, 0, save, CLOUDW, CLOUDH);
					save += CLOUDH;
				}
				y += STATEDY;
				if (par == 5) y += STATEDY/2;
			}
		}

		// display update
		DispUpdate();

		// delay with flag animation
		WaitFlag(1);
	}
}

// display whole game area
void DispAll(Bool update = True)
{
	// display background
	DrawImg(BackgroundImg);

	// display wins
	DispWins(0);
	DispWins(1);

	// display titles - player selection
	DispTitle();

	// display castles and fences
	DispGrass(0);
	DispCastle(0);
	DispFence(0);

	DispGrass(1);
	DispFence(1);
	DispCastle(1);

	// selection animation
	DispSelect();

	// display flags
	DispFlag();

	// display middle cards
	DispCard(0, MIDX1, MIDY, SHADOWOFF, False, True, False);
	DispCard(Last, MIDX2, MIDY, SHADOWOFF, False, False, LastDiscard);

	// display state
	DispState();

	// display cards
	DispCards();

	// update screen
	if (update) DispUpdate();
}

// start new game
void NewGame(int player1, int player2)
{
	// computer state
	Players[0].player = player1;
	Players[1].player = player2;
	Demo = ((player1 == PLAYER_COMP) && (player2 == PLAYER_COMP));

	// starting player is Black
	Player = 0;

	// none last laid card
	Last = -1;
	LastDiscard = False; // last card was discarded

	// initialize players
	int i, j;
	sPlayer* p = Players;
	for (i = 0; i < 2; i++)
	{
		// set player's parameters
		p->par[PAR_BUILDERS] = 2;
		p->par[PAR_BRICKS] = 5;
		p->par[PAR_SOLDIERS] = 2;
		p->par[PAR_WEAPONS] = 5;
		p->par[PAR_WIZARDS] = 2;
		p->par[PAR_CRYSTALS] = 5;
		p->par[PAR_CASTLE] = 30;
		p->par[PAR_FENCE] = 10;

		// generate cards
		for (j = 0; j < CARDNUM; j++) p->cards[j] = NewCard();

		// next player
		p++;
	}

	// selected card (cursor)
	SelCard = 3;

	// test cards permission
	Permission();

	// display whole game area
	DispAll();

	// display cursor
	CursorOn();
}

// display win game
void WinGame(int player)
{
	EndGame = player;

	// prepare castle height
	sPlayer* p = &Players[player];

	// get castle height
	int h = Players[player].par[PAR_CASTLE]; // h is in range 0..100
	if (h > CASTLEMAX) h = CASTLEMAX;
	int pixh = h*2 + 32; // castle height

	// prepare trumpet coordinate
	int x = CastleX[player] - CASTLEW/2 + 32;
	int y = CASTLEY - pixh + 32 - TRUMPETH;

	// play fanfare
	if (SoundMode > 0) PlaySound(FanfaresSnd, SOUNDCHAN_WIN);

	// delay - wait for sounds
	while (PlayingSound(SOUNDCHAN_WIN))
	{
		// selection animation
		DispSelect();

		// animate flags
		DispFlag();

		// draw trumpet image
		DrawImg(TrumpetImg, x, y, player*TRUMPETW, 0, TRUMPETW);

		// display update
		DispUpdate();

		// delay
		WaitMs(20);
	};
	Players[player].wins++;
	DispAll();

	// play applaus
	if (SoundMode > 0) PlaySoundRep(ApplauseSnd, SOUNDCHAN_WIN);

	// winning animation
	y = GRASSY;
	x = CastleX[player] - WINW/2;
	int anim = 0;
	KeyFlush();
	int i = Demo ? 200 : 1000000000;
	u32 *s, *d;
	for (; i > 0; i--)
	{
		// key
		if (KeyGet() != NOKEY) break;

		// draw castle and grass
		DispAll(False);

		// draw image
		DrawImg(WinImg, x, y, player*4*WINW + (anim>>2)*WINW, 0, WINW, WINH);
		anim++;
		if (anim >= 16) anim = 0;

		// display update
		DispUpdate();

		// delay
		WaitMs(30);
	}

	// stop sound
	StopSound(SOUNDCHAN_WIN);

	// start new game (returns True to quit game)
	NewGame(Players[0].player, Players[1].player);
}

// take new card
void TakeCard()
{
	// generate new card
	int card = NewCard();

	// card animation
	sPlayer* p = &Players[Player];
	AnimCard(card, MIDX1, MIDY, CARDX + FreePos*CARDDX, CARDY, (p->player != PLAYER_HUMAN) && !Demo, False);

	// display new card
	p->cards[FreePos] = card;
	p->disable[FreePos] = False;

	// test cards permission
	Permission();

	// redraw all cards
	DispAll();

	// delay
	WaitFlag(4);
}

// put card (pos = card position)
void PutCard(int pos, Bool discard)
{
	int i, j;

	// card is disabled
	sPlayer* p = &Players[Player];
	if (p->disable[pos]) discard = True;

	// free card position
	FreePos = pos;

	// card type
	int card = p->cards[pos];
	const sCard* c = &Card[card];

	// subtract card value (if not discarded)
	if (!discard)
	{
		// cost card
		p->par[c->type] -= c->costs;

		// display state
		DispState1(Player);

		// test cards permission
		Permission();
	}

	// delete card
	p->cards[pos] = -1; // no card
	DispAll();

	// card animation
	AnimCard(card, CARDX + pos*CARDDX, CARDY, MIDX2, MIDY, False, discard);
	Last = card;
	LastDiscard = discard;

	// dispay last card
	DispAll();

	// execute card
	if (!discard)
	{
		// save old state
		int castle1 = Players[0].par[PAR_CASTLE];
		int castle2 = Players[1].par[PAR_CASTLE];
		int fence1 = Players[0].par[PAR_FENCE];
		int fence2 = Players[1].par[PAR_FENCE];
		const u8* snd = NULL;

		// clear additions of both players
		for (i = 0; i < PAR_NUM; i++)
		{
			Players[0].add[i] = 0;
			Players[1].add[i] = 0;
		}

		// parameter change
		if (c->param != PAR_NUM) // not extra
		{
			// change to enemy
			if (c->value < 0) p = &Players[Player ^ 1];

			// play sound
			switch (c->param)
			{
			// increase power
			case PAR_BUILDERS:
			case PAR_SOLDIERS:
			case PAR_WIZARDS:
				snd = IncreasePowerSnd;
				break;

			// castle
			case PAR_CASTLE:
				if (c->value >= 0)
				{
					// build castle
					snd = BuildCastleSnd;
				}
				else
				{
					// ruin castle
					snd = RuinCastleSnd;
				}
				break;

			// fence
			case PAR_FENCE:
				if (c->value >= 0)
				{
					// build fence
					snd = BuildFenceSnd;
				}
				else
				{
					if (p->par[PAR_FENCE] < -c->value)
					{
						// ruin fence
						snd = RuinFenceSnd;
					}
					else
					{
						// ruin castle
						snd = RuinCastleSnd;
					}
				}
				break;

			// stocks
			// PAR_BRICKS
			// PAR_WEAPONS
			// PAR_CRYSTALS
			default:
				if (c->value >= 0)
				{
					// increase stock
					snd = IncreaseStocksSnd;
				}
				else
				{
					// decrease stock
					snd = DecreaseStocksSnd;
				}
				break;
			}

			// set parameter addition
			p->add[c->param] = c->value;
		}

		// extra service
		else
		{
			switch (card)
			{
			// reserve (castle +8, fence -4)
			case 3:
				p->add[PAR_CASTLE] = 8; // castle +8
				if (p->par[PAR_FENCE] >= 4) // fence -4
					p->add[PAR_FENCE] = -4;
				else
					p->add[PAR_FENCE] = - p->par[PAR_FENCE];

				// build castle
				snd = BuildCastleSnd;
				break;

			// wain (castle +8, enemy castle -4)
			case 6:
				p->add[PAR_CASTLE] = 8; // castle +8
				Players[Player ^ 1].add[PAR_CASTLE] = -4; // enemy castle -4

				// build castle
				snd = BuildCastleSnd;
				break;
				
			// saboteur (enemy stocks -4)
			case 16:
				p = &Players[Player ^ 1];
				p->add[PAR_BRICKS] = -4;
				p->add[PAR_WEAPONS] = -4;
				p->add[PAR_CRYSTALS] = -4;

				// decrease stock
				snd = DecreaseStocksSnd;
				break;

			// thief (transfer enemy stocks 5)
			case 17:
				{
					p = &Players[Player ^ 1];

					int bricks = p->par[PAR_BRICKS];
					if (bricks > 5) bricks = 5;
					p->add[PAR_BRICKS] = -bricks;

					int weapons = p->par[PAR_WEAPONS];
					if (weapons > 5) weapons = 5;
					p->add[PAR_WEAPONS] = -weapons;

					int crystals = p->par[PAR_CRYSTALS];
					if (crystals > 5) crystals = 5;
					p->add[PAR_CRYSTALS] = -crystals;

					p = &Players[Player];
					p->add[PAR_BRICKS] = bricks;
					p->add[PAR_WEAPONS] = weapons;
					p->add[PAR_CRYSTALS] = crystals;

					// increase stock
					snd = IncreaseStocksSnd;
				}
				break;

			// curse (all +1, enemie's all -1)
			case 29:
				for (i = 0; i < PAR_NUM; i++) p->add[i] = 1;

				p = &Players[Player ^ 1];
				for (i = 0; i < PAR_NUM; i++)
				{
					if (p->par[i] > 0) p->add[i] = -1;
				}

				snd = CurseSnd;
				break;
			}
		}

		// display state
		DispState();

		// change values animation
		ChangeAnim(Players[Player].player == PLAYER_COMP);

		// update parameters
		for (i = 0; i < 2; i++)
		{
			p = &Players[i];
			for (j = 0; j < PAR_NUM; j++)
			{
				p->par[j] += p->add[j];

				// check underflow
				switch (j)
				{
				case PAR_FENCE:
					if (p->par[j] < 0)
					{
						p->par[PAR_CASTLE] += p->par[j];
						if (p->par[PAR_CASTLE] < 0) p->par[PAR_CASTLE] = 0;
						p->par[j] = 0;
					}
					break;

				case PAR_BUILDERS:
				case PAR_SOLDIERS:
				case PAR_WIZARDS:
					if (p->par[j] < 1) p->par[j] = 1;
					break;

				default:
					if (p->par[j] < 0) p->par[j] = 0;
					break;
				}
			}
		}

		// play sound
		if ((snd != NULL) && (SoundMode > 0)) PlaySound(snd, SOUNDCHAN_PAR);
	}

	// test cards permission
	Permission();

	// redraw screen
	DispAll();

	// delay
	WaitFlag(5);

	// delay - wait for sounds
	while (PlayingSound(SOUNDCHAN_PAR)) (WaitFlag(1));

	// check player's 0 victory
	if ((Players[0].par[PAR_CASTLE] >= CASTLEMAX) ||
		(Players[1].par[PAR_CASTLE] <= 0))
	{
		WinGame(0); // True to quit game
		return;
	}

	// check player's 1 victory
	if ((Players[1].par[PAR_CASTLE] >= CASTLEMAX) ||
		(Players[0].par[PAR_CASTLE] <= 0))
	{
		WinGame(1); // True to quit game
		return;
	}

	// demo additional delay
	if (Demo) WaitFlag(8);

	// take new card
	TakeCard();

	// change active player
	Player ^= 1;

	// increase stocks
	p = &Players[Player];
	p->par[PAR_BRICKS] += p->par[PAR_BUILDERS];
	p->par[PAR_WEAPONS] += p->par[PAR_SOLDIERS];
	p->par[PAR_CRYSTALS] += p->par[PAR_WIZARDS];

	// test cards permission
	Permission();

	// redraw all cards
	DispAll();
}

// find best card
int FindCard()
{
	int i;
	sPlayer* p = &Players[Player];

	// find best enabled card (expensive cards are better)
	int foundinx = -1; // found card position
	int foundval = 0; // found card value
	int card, testval;
	const sCard* c;
	for (i = 0; i < CARDNUM; i++)
	{
		if (!p->disable[i])
		{
			// get card
			card = p->cards[i];
			c = &Card[card];
			testval = c->costs; // card value

			// workers have preference
			if ((c->param == PAR_BUILDERS) ||
				(c->param == PAR_SOLDIERS) ||
				(c->param == PAR_WIZARDS))
			{
				testval = 100;
			}

			// found card with higher value
			if ((testval > foundval) || ((testval == foundval) && (RandU8() < 0x60)))
			{
				foundinx = i;
				foundval = testval;
			}
		}
	}

	// enabled card not found - find card for discard 
	//    (discard expensive cards, they cannot be use for a long time)
	if (foundinx < 0)
	{
		foundinx = 0;

		for (i = 0; i < CARDNUM; i++)
		{
			if (p->disable[i])
			{
				// get card
				card = p->cards[i];
				c = &Card[card];
				testval = c->costs; // card value

				// found card with higher value
				if (testval > foundval)
				{
					foundinx = i;
					foundval = testval;
				}
			}
		}
	}

	return foundinx;
}

// auto play
void AutoPlay()
{
	// small delay
	WaitFlag(8);

	// find next card to play
	int foundinx = FindCard();

	// put card
	PutCard(foundinx, Players[Player].disable[foundinx]);
}

// game
void Game(int player1, int player2)
{
	int ch;

	// start new game
	EndGame = -1;
	NewGame(player1, player2);

	// play background sound
	if (SoundMode > 1) PlaySoundRep(BirdsSnd, SOUNDCHAN_BIRD);

	// main loop
	while (true)
	{
		// active player is computer
		if (Players[Player].player == PLAYER_COMP)
		{
			// auto play
			AutoPlay();

			// display cursor
			CursorOn();

			// flush keyboard
			if (!Demo)
				KeyFlush();
			else
			{
				ch = KeyGet();

				if (ch == KEY_SCREENSHOT)
					ScreenShot();
				else if (ch == KEY_ESC)
					return;
			}
		}
		else
		{
			// input from keyboard
			ch = KeyGet();

			// serve keyboard
			switch (ch)
			{
			// quit
			case KEY_ESC:
				return;

			// left
			case KEY_LEFT:
				CursorOff();
				SelCard--;
				if (SelCard < 0) SelCard = CARDNUM-1;
				CursorOn();
				KeyFlush();
				break;

			// right
			case KEY_RIGHT:
				CursorOff();
				SelCard++;
				if (SelCard >= CARDNUM) SelCard = 0;
				CursorOn();
				KeyFlush();
				break;

			// discard
			case KEY_UP:
			case KEY_ENTER:
				PutCard(SelCard, True); // put card
				CursorOn();
				KeyFlush();
				break;

			// select
			case KEY_DOWN:
			case KEY_SPACE:
				PutCard(SelCard, Players[Player].disable[SelCard]); // put card
				CursorOn();
				KeyFlush();
				break;

			// help
			case KEY_TAB:
				CursorOff();
				SelCard = FindCard();
				CursorOn();
				KeyFlush();
				break;

			// screenshot
			case KEY_SCREENSHOT:
				ScreenShot();
				break;

			// no key
			case NOKEY:
				if (!PlayingSound(SOUNDCHAN_BIRD) && (SoundMode > 1))
				{
					PlaySoundRep(BirdsSnd, SOUNDCHAN_BIRD);
				}
				break;

			// invalid key
			default:
				KeyFlush();
				break;
			}
		}

		// selection animation
		DispSelect();

		// flag animation
		DispFlag();

		// update display
		DispUpdate();
	}
}
