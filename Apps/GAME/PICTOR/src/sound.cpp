
// ****************************************************************************
//
//                                 Sounds
//
// ****************************************************************************

#include "../include.h"

#ifdef DEB_SOUND	// initial sound mode SOUNDMODE_OFF, SOUNDMODE_SOUND, SOUNDMODE_MUSIC
Bool SoundMode = DEB_SOUND; // sound mode
#else
Bool SoundMode = SOUNDMODE_MUSIC; // sound mode
#endif

// MP3 player
Bool MP3PlayerWasInit = False;
sMP3Player MP3Player;
u8 ALIGNED MP3PlayerOutBuf[MP3PLAYER_OUTSIZE];

// music samples
const u8* SndSamp[SND_NUM] = {
	Music1Snd,
	Music2Snd,
	Music3Snd,
	Music4Snd,
	Music5Snd,
	Music6Snd,
	Music7Snd,
	Music8Snd,
	Music9Snd,
	Music10Snd,
	Music11Snd,
	Music12Snd,
};

const int SndLen[SND_NUM] = {
	sizeof(Music1Snd),
	sizeof(Music2Snd),
	sizeof(Music3Snd),
	sizeof(Music4Snd),
	sizeof(Music5Snd),
	sizeof(Music6Snd),
	sizeof(Music7Snd),
	sizeof(Music8Snd),
	sizeof(Music9Snd),
	sizeof(Music10Snd),
	sizeof(Music11Snd),
	sizeof(Music12Snd),
};

// stop music
void MusicStop()
{
	sMP3Player* mp3 = &MP3Player;
	if (MP3PlayerWasInit)
	{
		MP3PlayerTerm(mp3);
		MP3PlayerWasInit = False;
	}
}

// poll music
void MusicPoll()
{
	sMP3Player* mp3 = &MP3Player;
	if (MP3PlayerWasInit)
	{
		MP3Poll(mp3);
	}
}

// play music
void MusicStart()
{
	MusicStop();
	if (SoundMode == SOUNDMODE_MUSIC)
	{
		sMP3Player* mp3 = &MP3Player;
		int r = MP3PlayerInit(mp3, NULL, SndSamp[BackInx], SndLen[BackInx], MP3PlayerOutBuf, MP3PLAYER_OUTSIZE, -1);
		if (r == ERR_MP3_OK)
		{
			MP3PlayerWasInit = True;
			MP3Play(mp3, SOUNDCHAN_MUSIC, True);
		}
	}
}
