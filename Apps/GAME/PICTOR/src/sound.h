
// ****************************************************************************
//
//                                 Sounds
//
// ****************************************************************************

#define SOUNDMODE_OFF	0	// sound is OFF
#define SOUNDMODE_SOUND	1	// only sound
#define SOUNDMODE_MUSIC	2	// sound + music

#define SOUNDCHAN_MISSILE	0	// sound channel - missiles thrown by actor
#define SOUNDCHAN_ENEMYHIT	1	// sound channel - enemy hit
#define SOUNDCHAN_ACTORHIT	2	// sound channel - actor hit
#define SOUNDCHAN_MUSIC		3	// sound channel - music

extern Bool SoundMode; // sound mode

// MP3 player
extern Bool MP3PlayerWasInit;
extern sMP3Player MP3Player;
extern u8 ALIGNED MP3PlayerOutBuf[MP3PLAYER_OUTSIZE];

// music samples
#define SND_NUM BG_NUM
extern const u8* SndSamp[SND_NUM];
extern const int SndLen[SND_NUM];

// stop music
void MusicStop();

// poll music
void MusicPoll();

// play music
void MusicStart();
