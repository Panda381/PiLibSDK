
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#define DISP_FRAMEINFO	0	// 1=display MP3 frame info

#include "../include.h"

// for faster compilation (rather than compile with Makefile)
#include "../snd/001.cpp"
#include "../snd/002.cpp"
#include "../snd/003.cpp"
#include "../snd/004.cpp"
#include "../snd/005.cpp"
#include "../snd/006.cpp"
#include "../snd/007.cpp"
#include "../snd/008.cpp"
#include "../snd/009.cpp"
#include "../snd/010.cpp"
#include "../snd/011.cpp"
#include "../snd/012.cpp"
#include "../snd/013.cpp"
#include "../snd/014.cpp"
#include "../snd/015.cpp"
#include "../snd/016.cpp"
#include "../snd/017.cpp"
#include "../snd/018.cpp"
#include "../snd/019.cpp"
#include "../snd/020.cpp"
#include "../snd/021.cpp"
#include "../snd/022.cpp"
#include "../snd/023.cpp"
#include "../snd/024.cpp"
#include "../snd/025.cpp"
#include "../snd/026.cpp"
#include "../snd/027.cpp"
#include "../snd/028.cpp"
#include "../snd/029.cpp"
#include "../snd/030.cpp"
#include "../snd/031.cpp"
#include "../snd/032.cpp"
#include "../snd/033.cpp"
#include "../snd/034.cpp"
#include "../snd/035.cpp"
#include "../snd/036.cpp"
#include "../snd/037.cpp"
#include "../snd/038.cpp"
#include "../snd/039.cpp"
#include "../snd/040.cpp"
#include "../snd/041.cpp"
#include "../snd/042.cpp"
#include "../snd/043.cpp"
#include "../snd/044.cpp"
#include "../snd/045.cpp"
#include "../snd/046.cpp"
#include "../snd/047.cpp"
#include "../snd/048.cpp"
#include "../snd/049.cpp"
#include "../snd/050.cpp"
#include "../snd/051.cpp"
#include "../snd/052.cpp"
#include "../snd/053.cpp"
#include "../snd/054.cpp"
#include "../snd/055.cpp"
#include "../snd/056.cpp"
#include "../snd/057.cpp"
#include "../snd/058.cpp"
#include "../snd/059.cpp"
#include "../snd/060.cpp"
#include "../snd/061.cpp"
#include "../snd/062.cpp"
#include "../snd/063.cpp"
#include "../snd/064.cpp"
#include "../snd/065.cpp"
#include "../snd/066.cpp"
#include "../snd/067.cpp"
#include "../snd/068.cpp"
#include "../snd/069.cpp"
#include "../snd/070.cpp"
#include "../snd/071.cpp"
#include "../snd/072.cpp"
#include "../snd/073.cpp"
#include "../snd/074.cpp"
#include "../snd/075.cpp"
#include "../snd/076.cpp"
#include "../snd/077.cpp"
#include "../snd/078.cpp"
#include "../snd/079.cpp"
#include "../snd/080.cpp"
#include "../snd/081.cpp"
#include "../snd/082.cpp"
#include "../snd/083.cpp"
#include "../snd/084.cpp"
#include "../snd/085.cpp"
#include "../snd/086.cpp"
#include "../snd/087.cpp"
#include "../snd/088.cpp"
#include "../snd/089.cpp"
#include "../snd/090.cpp"
#include "../snd/091.cpp"
#include "../snd/092.cpp"
#include "../snd/093.cpp"
#include "../snd/094.cpp"
#include "../snd/095.cpp"
#include "../snd/096.cpp"
#include "../snd/097.cpp"
#include "../snd/098.cpp"
#include "../snd/099.cpp"
#include "../snd/100.cpp"
#include "../snd/101.cpp"
#include "../snd/102.cpp"
#include "../snd/103.cpp"
#include "../snd/104.cpp"
#include "../snd/105.cpp"
#include "../snd/106.cpp"
#include "../snd/107.cpp"
#include "../snd/108.cpp"
#include "../snd/109.cpp"
#include "../snd/110.cpp"
#include "../snd/111.cpp"
#include "../snd/112.cpp"
#include "../snd/113.cpp"
#include "../snd/114.cpp"
#include "../snd/115.cpp"
#include "../snd/116.cpp"
#include "../snd/117.cpp"
#include "../snd/118.cpp"
#include "../snd/119.cpp"
#include "../snd/120.cpp"
#include "../snd/121.cpp"
#include "../snd/122.cpp"
#include "../snd/123.cpp"
#include "../snd/124.cpp"
#include "../snd/125.cpp"
#include "../snd/126.cpp"
#include "../snd/127.cpp"
#include "../snd/128.cpp"
#include "../snd/129.cpp"
#include "../snd/130.cpp"

u8* LogoImg = NULL;

int Loop = 0; // current loop
Bool Mute = False; // mute music

// Loop list
const u8* LoopList[LOOP_NUM] = {
	Loop001Snd,
	Loop002Snd,
	Loop003Snd,
	Loop004Snd,
	Loop005Snd,
	Loop006Snd,
	Loop007Snd,
	Loop008Snd,
	Loop009Snd,
	Loop010Snd,
	Loop011Snd,
	Loop012Snd,
	Loop013Snd,
	Loop014Snd,
	Loop015Snd,
	Loop016Snd,
	Loop017Snd,
	Loop018Snd,
	Loop019Snd,
	Loop020Snd,
	Loop021Snd,
	Loop022Snd,
	Loop023Snd,
	Loop024Snd,
	Loop025Snd,
	Loop026Snd,
	Loop027Snd,
	Loop028Snd,
	Loop029Snd,
	Loop030Snd,
	Loop031Snd,
	Loop032Snd,
	Loop033Snd,
	Loop034Snd,
	Loop035Snd,
	Loop036Snd,
	Loop037Snd,
	Loop038Snd,
	Loop039Snd,
	Loop040Snd,
	Loop041Snd,
	Loop042Snd,
	Loop043Snd,
	Loop044Snd,
	Loop045Snd,
	Loop046Snd,
	Loop047Snd,
	Loop048Snd,
	Loop049Snd,
	Loop050Snd,
	Loop051Snd,
	Loop052Snd,
	Loop053Snd,
	Loop054Snd,
	Loop055Snd,
	Loop056Snd,
	Loop057Snd,
	Loop058Snd,
	Loop059Snd,
	Loop060Snd,
	Loop061Snd,
	Loop062Snd,
	Loop063Snd,
	Loop064Snd,
	Loop065Snd,
	Loop066Snd,
	Loop067Snd,
	Loop068Snd,
	Loop069Snd,
	Loop070Snd,
	Loop071Snd,
	Loop072Snd,
	Loop073Snd,
	Loop074Snd,
	Loop075Snd,
	Loop076Snd,
	Loop077Snd,
	Loop078Snd,
	Loop079Snd,
	Loop080Snd,
	Loop081Snd,
	Loop082Snd,
	Loop083Snd,
	Loop084Snd,
	Loop085Snd,
	Loop086Snd,
	Loop087Snd,
	Loop088Snd,
	Loop089Snd,
	Loop090Snd,
	Loop091Snd,
	Loop092Snd,
	Loop093Snd,
	Loop094Snd,
	Loop095Snd,
	Loop096Snd,
	Loop097Snd,
	Loop098Snd,
	Loop099Snd,
	Loop100Snd,
	Loop101Snd,
	Loop102Snd,
	Loop103Snd,
	Loop104Snd,
	Loop105Snd,
	Loop106Snd,
	Loop107Snd,
	Loop108Snd,
	Loop109Snd,
	Loop110Snd,
	Loop111Snd,
	Loop112Snd,
	Loop113Snd,
	Loop114Snd,
	Loop115Snd,
	Loop116Snd,
	Loop117Snd,
	Loop118Snd,
	Loop119Snd,
	Loop120Snd,
	Loop121Snd,
	Loop122Snd,
	Loop123Snd,
	Loop124Snd,
	Loop125Snd,
	Loop126Snd,
	Loop127Snd,
	Loop128Snd,
	Loop129Snd,
	Loop130Snd,
};

// number of bytes of the loops
const int LoopLen[LOOP_NUM] = {
	sizeof(Loop001Snd),
	sizeof(Loop002Snd),
	sizeof(Loop003Snd),
	sizeof(Loop004Snd),
	sizeof(Loop005Snd),
	sizeof(Loop006Snd),
	sizeof(Loop007Snd),
	sizeof(Loop008Snd),
	sizeof(Loop009Snd),
	sizeof(Loop010Snd),
	sizeof(Loop011Snd),
	sizeof(Loop012Snd),
	sizeof(Loop013Snd),
	sizeof(Loop014Snd),
	sizeof(Loop015Snd),
	sizeof(Loop016Snd),
	sizeof(Loop017Snd),
	sizeof(Loop018Snd),
	sizeof(Loop019Snd),
	sizeof(Loop020Snd),
	sizeof(Loop021Snd),
	sizeof(Loop022Snd),
	sizeof(Loop023Snd),
	sizeof(Loop024Snd),
	sizeof(Loop025Snd),
	sizeof(Loop026Snd),
	sizeof(Loop027Snd),
	sizeof(Loop028Snd),
	sizeof(Loop029Snd),
	sizeof(Loop030Snd),
	sizeof(Loop031Snd),
	sizeof(Loop032Snd),
	sizeof(Loop033Snd),
	sizeof(Loop034Snd),
	sizeof(Loop035Snd),
	sizeof(Loop036Snd),
	sizeof(Loop037Snd),
	sizeof(Loop038Snd),
	sizeof(Loop039Snd),
	sizeof(Loop040Snd),
	sizeof(Loop041Snd),
	sizeof(Loop042Snd),
	sizeof(Loop043Snd),
	sizeof(Loop044Snd),
	sizeof(Loop045Snd),
	sizeof(Loop046Snd),
	sizeof(Loop047Snd),
	sizeof(Loop048Snd),
	sizeof(Loop049Snd),
	sizeof(Loop050Snd),
	sizeof(Loop051Snd),
	sizeof(Loop052Snd),
	sizeof(Loop053Snd),
	sizeof(Loop054Snd),
	sizeof(Loop055Snd),
	sizeof(Loop056Snd),
	sizeof(Loop057Snd),
	sizeof(Loop058Snd),
	sizeof(Loop059Snd),
	sizeof(Loop060Snd),
	sizeof(Loop061Snd),
	sizeof(Loop062Snd),
	sizeof(Loop063Snd),
	sizeof(Loop064Snd),
	sizeof(Loop065Snd),
	sizeof(Loop066Snd),
	sizeof(Loop067Snd),
	sizeof(Loop068Snd),
	sizeof(Loop069Snd),
	sizeof(Loop070Snd),
	sizeof(Loop071Snd),
	sizeof(Loop072Snd),
	sizeof(Loop073Snd),
	sizeof(Loop074Snd),
	sizeof(Loop075Snd),
	sizeof(Loop076Snd),
	sizeof(Loop077Snd),
	sizeof(Loop078Snd),
	sizeof(Loop079Snd),
	sizeof(Loop080Snd),
	sizeof(Loop081Snd),
	sizeof(Loop082Snd),
	sizeof(Loop083Snd),
	sizeof(Loop084Snd),
	sizeof(Loop085Snd),
	sizeof(Loop086Snd),
	sizeof(Loop087Snd),
	sizeof(Loop088Snd),
	sizeof(Loop089Snd),
	sizeof(Loop090Snd),
	sizeof(Loop091Snd),
	sizeof(Loop092Snd),
	sizeof(Loop093Snd),
	sizeof(Loop094Snd),
	sizeof(Loop095Snd),
	sizeof(Loop096Snd),
	sizeof(Loop097Snd),
	sizeof(Loop098Snd),
	sizeof(Loop099Snd),
	sizeof(Loop100Snd),
	sizeof(Loop101Snd),
	sizeof(Loop102Snd),
	sizeof(Loop103Snd),
	sizeof(Loop104Snd),
	sizeof(Loop105Snd),
	sizeof(Loop106Snd),
	sizeof(Loop107Snd),
	sizeof(Loop108Snd),
	sizeof(Loop109Snd),
	sizeof(Loop110Snd),
	sizeof(Loop111Snd),
	sizeof(Loop112Snd),
	sizeof(Loop113Snd),
	sizeof(Loop114Snd),
	sizeof(Loop115Snd),
	sizeof(Loop116Snd),
	sizeof(Loop117Snd),
	sizeof(Loop118Snd),
	sizeof(Loop119Snd),
	sizeof(Loop120Snd),
	sizeof(Loop121Snd),
	sizeof(Loop122Snd),
	sizeof(Loop123Snd),
	sizeof(Loop124Snd),
	sizeof(Loop125Snd),
	sizeof(Loop126Snd),
	sizeof(Loop127Snd),
	sizeof(Loop128Snd),
	sizeof(Loop129Snd),
	sizeof(Loop130Snd),
};

// names of loops
const char* LoopName[LOOP_NUM] = {
	"3xOSC fun",
	"C_HC",
	"C_kick",
	"Dance_X_B",
	"dnbautoscratch",
	"Dreamzofluxury",
	"DrumCircle",
	"ElvisInADress",
	"FruityExample-alpharisc",
	"Fuego-BigKilling",
	"Fuego-FenderBender",
	"Fuego-GraySunshine",
	"H441-GOA",
	"Halfwaytonowhere",
	"ISuckAtTitles",
	"JasonC-Ebb_and_Flow",
	"JasonC-Faded@4am",
	"JasonC-Glimmer",
	"JasonC-Highpass",
	"JasonC-Stasis",
	"JasonC-Tranced@4am",
	"Land of rock",
	"Light already (remixed)",
	"MentalFusion",
	"Myopic-DepartureLounge",
	"Native soil",
	"Oscar22",
	"Oscarparty",
	"Oscillations",
	"Pilchard-KickingBeat",
	"Pilchard-NewFruity",
	"Reegz-Loungin",
	"Reegz-Vibe",
	"Roof garden_B",
	"SpaceAce",
	"speecher",
	"spookytriangle",
	"Sympathy",
	"Tears",
	"Techmatik",
	"TheAmericanPropertyNetwork",
	"threeosc",
	"Toby-Falling",
	"Toby-Hydrosphere",
	"Toby-Industro",
	"Toby-RainDrops",
	"Toby-Reaper",
	"Vlad-She'saRican",
	"XOR-Chop my slice please",
	"XOR-Osmosis",
	"XOR-Reggophany",
	"asche",
	"drwho",
	"Fuego96-NashvilleWest",
	"404lament",
	"hERETIC-Tubular",
	"Try Again (remixed)",
	"Try Again (remixed) 2",
	"JasonC-Fubar",
	"JasonC-Greyscale",
	"MakeMeFeelLove",
	"Ori'sBminors",
	"Vlad-FeveriaMagma4-InstrumentalRemix",
	"FruityTechTrance",
	"JasonC-Shear",
	"Arpeggiator",
	"NewStuff",
	"Daft Punk - Revolution 909",
	"CWCrisman-Real Mellow",
	"DanR-Taken",
	"DanR-WhatDoesThatKnobDoAgain",
	"Duda-Drumsynth Trigger",
	"Fuego+Reegz-Aura",
	"Fuego-AHintOfReggae",
	"Fuego-Behaviour 9",
	"Fuego-FL303",
	"Fuego-Skyhigh",
	"gol-Random",
	"Inginer-Housy Shuffle",
	"Inginer-Jungle Fever",
	"JasonC-After Hours",
	"JasonC-Dark Corners",
	"JasonC-Glint",
	"JasonC-Groundswell",
	"JasonC-Kinetic",
	"Pilchard-D-S-L",
	"Pilchard-Goathing",
	"Reegz-Foxy Sista",
	"Reegz-IFI",
	"Reegz-Messin Around",
	"Reegz-The Way (Instrumental)",
	"Toby-Carrier",
	"Toby-Striker",
	"XOR-Eliminator",
	"AmbushInTheBush",
	"BeatMachine",
	"StringLoop",
	"NewStuff Rap",
	"Korben Dallas",
	"Aknot! Wot",
	"Holymen - The Last Universe",
	"Holymen - Area 51 (Shiva Shidapu)",
	"Holymen - Seven'th Heaven",
	"Holymen - Sicko Leako",
	"Holymen - Time for Peace",
	"Holymen - Romeo 4 Juliete",
	"prodigy-out_of_space_(techno_remix)",
	"D.O.N.S feat. Technotronic - Pump Up",
	"punk city - mission-(sample.b6)",
	"Supa-Dupa-Fly (Sezam Remix)",
	"a-reflective-dbu",
	"Aphex Twins - Star Wars Theme (Trance)",
	"beethoven - 9th symphony (techno)",
	"beethoven - 9th symphony (techno) 2",
	"carl cox - TECHNO SETS (Live)",
	"Crystal Techno",
	"Disco Kandi 3 - DJD Hydraulic Dogs",
	"Dj Disco - Let's Dance(Extended Rmx)",
	"Dj Quicksilver - Techno Macht Spass",
	"DJ Tiesto - Forbidden Paradise",
	"DJ Tiesto - Forbidden Paradise 2",
	"robert miles - x files (techno mix)",
	"X-Perience - Land of Tomatoes",
	"Holymen - Monkey Buieness",
	"Astral Projection - Analog Voyager",
	"Astral Projection - Aurora Borealis",
	"Astral Projection - Dancing Galaxy",
	"Astral Projection - Enlightened",
	"Astral Projection - Power Gen",
	"Astral Projection - Still on Mars",
};

// MP3 player
Bool MP3PlayerWasInit = False;
sMP3Player MP3Player;
u8 ALIGNED MP3PlayerOutBuf[MP3PLAYER_OUTSIZE];

// start new loop
void NewLoop()
{
	// clear screen
	DrawClearCol(COL_RED, COL_WHITE);

	// draw logo
	DrawImg(LogoImg, (WIDTH - LOGO_W)/2, (HEIGHT - LOGO_H)/2);

	// prepare loop number
	int len0 = DecNum(DecNumBuf, Loop+LOOP_FIRST, 0);
	DecNumBuf[len0++] = ':';
	DecNumBuf[len0] = 0;

	// draw loop name
	int len = len0 + StrLen(LoopName[Loop]);
	if (len > 40)
	{
		int x = (WIDTH - len*8)/2;
		DrawTextH(DecNumBuf, x, HEIGHT-32, COL_RED);
		x += len0*8;
		DrawTextH(LoopName[Loop], x, HEIGHT-32, COL_RED);
	}
	else
	{
		int x = (WIDTH - len*16)/2;
		DrawText2(DecNumBuf, x, HEIGHT-32, COL_RED);
		x += len0*16;
		DrawText2(LoopName[Loop], x, HEIGHT-32, COL_RED);
	}

	// display update
	DispUpdate();

	// start/stop playing loop
	sMP3Player* mp3 = &MP3Player;
	if (MP3PlayerWasInit) MP3PlayerTerm(mp3);

	if (!Mute)
	{
		int r = MP3PlayerInit(mp3, NULL, LoopList[Loop], LoopLen[Loop], MP3PlayerOutBuf, MP3PLAYER_OUTSIZE, -1);
		if (r == ERR_MP3_OK)
		{
			MP3PlayerWasInit = True;
			MP3Play(mp3, 0, True);

#if DISP_FRAMEINFO	// display MP3 frame info
			DrawRect(0, 0, WIDTH, 3*16, COL_BLUE);
			char bf[70];
			MemPrint(bf, 70, "FrameTime=%dus Frames=%d", mp3->frametime, mp3->frames);
			DrawText(bf, 0, 0, COL_YELLOW);
			MemPrint(bf, 70, "FrameIn=%dB FrameOut=%dB Size=%dB", mp3->framesizeavg, mp3->sampnumavg*2, LoopLen[Loop]);
			DrawText(bf, 0, 16, COL_YELLOW);
			MemPrint(bf, 70, "Bitrate=%dkbps SampRate=%dHz", mp3->bitrateavg/1000, mp3->info.samprate);
			DrawText(bf, 0, 32, COL_YELLOW);
			DispUpdate();
#endif
		}
		else
			Mute = True;
	}
}

int main()
{
	// load logo image
	LogoImg = (u8*)JPGLOAD(LogoImgJpg);

	Loop = 0;
	NewLoop();

	while(True)
	{
		switch (KeyGet())
		{
		// mute
		case KEY_SPACE:
			Mute = !Mute;
			NewLoop();
			break;

		// exit
		case KEY_ESC:
			Reboot();
			break;

		// screenshot
		case KEY_SCREENSHOT:
			ScreenShot();
			break;

		// previous
		case KEY_LEFT:
			Loop--;
			if (Loop < 0) Loop = LOOP_NUM-1;
			NewLoop();
			break;

		// next
		case KEY_RIGHT:
			Loop++;
			if (Loop >= LOOP_NUM) Loop = 0;
			NewLoop();
			break;
		}

		sMP3Player* mp3 = &MP3Player;
		if (!Mute && MP3Playing(mp3))
		{
			MP3Poll(mp3);

			int pos = mp3->pos * WIDTH / mp3->frames;
			DrawRect(0, HEIGHT-4, pos, 4, COL_GREEN);
			DrawRect(pos, HEIGHT-4, WIDTH - pos, 4, COL_BLACK);
			DispUpdate();
		}
	}
}
