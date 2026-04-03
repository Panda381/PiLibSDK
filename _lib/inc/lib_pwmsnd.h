
// ****************************************************************************
//
//                                PWM sound output
//
// ****************************************************************************
// Audio output on Pi 3:
//  audio range 20Hz to 20kHz, PWM with sample rate 50MHz
//  GPIO40 right channel, GPIO41 left channel
//  RC filter, cut-out frequency 17.9 kHz, divider 3.3V to 1.1V output
//	- resistor 270R from GPIO
//	- RC to GND 33nF + 150R
//	- output via 10uF
//	- 2x protection duo-diodes BAV99
// https://learn.adafruit.com/adding-basic-audio-ouput-to-raspberry-pi-zero/overview

// The usable outputs are: left channel PWMSND_GPIO0 = GPIO12 (default), GPIO18, or GPIO40;
// right channel PWMSND_GPIO1 = GPIO13 (default), GPIO19, GPIO41, or GPIO45.
// The PWMSND_SWAP parameter can be used to swap the left and right channels.

// All functions should be called from core 0.
// Correct functioning is not guaranteed from other cores.

#if USE_PWMSND		// 1=use PWM sound output, 0=not used (lib_pwmsnd.*)

#ifndef _LIB_PWMSND_H
#define _LIB_PWMSND_H

#ifndef PWMSND_CHANNUM
#define PWMSND_CHANNUM	4	// number of sound channels of sound mixer
#endif

#ifndef PWMSND_DMACHAN
#define PWMSND_DMACHAN	7	// sound DMA channel
#endif

#ifndef PWMSND_DMAIRQ
#define PWMSND_DMAIRQ	IRQ_DMA7 // sound DMA channel IRQ
#endif

#ifndef PWMSND_SWAP
#define PWMSND_SWAP	0	// 1=swap channels (PWM1 left, PWM0 right), 0=no swap (PWM0 left, PWM1 right)
#endif

#ifndef PWMSND_GPIO0
#define PWMSND_GPIO0	12	// GPIO pin for PWM0 (12 alt0, 18 alt5, 40 alt0)
#endif

#ifndef PWMSND_GPIO1
#define PWMSND_GPIO1	13	// GPIO pin for PWM1 (13 alt0, 19 alt5, 41 alt0, 45 alt0)
#endif

// PWM sound uses GPIOCLOCK_SRC_PLLD clock source with frequency 500 MHz (Pi 1-3) or 750 MHz (Pi 4)
#if RASPPI<4
#define PWMSND_PRESC	2	// PWM sound integer prescaler (Pi 1-3)
#else
#define PWMSND_PRESC	3	// PWM sound integer prescaler (Pi 4)
#endif

#define PWMSND_CLOCK	250000000 // PWM sound clock frequency
#define PWMSND_PERIOD	5000	// PWM sound sample period
#define PWMSND_RATE	(PWMSND_CLOCK/PWMSND_PERIOD) // real playback sound rate (= 50000 Hz)

#define SOUNDRATE	50000	// reference sound rate [Hz]

#define SNDFRAC	16		// number of fraction bits
#define SNDINT (1<<SNDFRAC)	// integer part of sound fraction

#define SND_BUFSIZE	512	// sound buffer size in bytes (64 samples, delay 1 ms, 1 sample is two u32 words)
#define SND_BUFNUM	15	// number of sound buffers (total time 15 ms)

// sound formats
#define SNDFORM_PCM8		0	// PCM 8-bit mono, unsigned u8, middle at 0x80, 1 byte per sample
#define SNDFORM_PCM16		1	// PCM 16-bit mono, signed s16, middke at 0, 2 bytes per sample
#define SNDFORM_PCM8S		2	// PCM 8-bit stereo, unsigned u8, middle at 0x80, 2 bytes per sample
#define SNDFORM_PCM16S		3	// PCM 16-bit stereo, signed s16, middle at 0, 4 bytes per sample
//#define SNDFORM_ADPCM		4	// ADPCM 4-bit mono (currently not supported)
//#define SNDFORM_ADPCMS	5	// ADPCM 4-bit stereo (currently not supported)
#define SNDFORM_MELODY		6	// melody

// repeat mode - use count of repeats or predefined constants
#define SNDREPEAT_NO		0	// no repeat
#define SNDREPEAT_REPEAT	-1	// repeat sound endlessly
#define SNDREPEAT_STREAM	-2	// streaming sound (resets 'nextcnt')

// note length in 1/60 sec
#define NOTE_LEN_BASE	(PWMSND_RATE/60) // = 50000/60 = 833.333 -> integer 833

#define NOTE_LEN1DOT	(NOTE_LEN_BASE*96)		// 1.
#define NOTE_LEN1	(NOTE_LEN_BASE*64)		// 1
#define NOTE_LEN2DOT	(NOTE_LEN_BASE*48)		// 1/2.
#define NOTE_LEN2	(NOTE_LEN_BASE*32)		// 1/2
#define NOTE_LEN4DOT	(NOTE_LEN_BASE*24)		// 1/4.
#define NOTE_LEN4	(NOTE_LEN_BASE*16)		// 1/4
#define NOTE_LEN8DOT	(NOTE_LEN_BASE*12)		// 1/8.
#define NOTE_LEN8	(NOTE_LEN_BASE*8)		// 1/8
#define NOTE_LEN16DOT	(NOTE_LEN_BASE*6)		// 1/16.
#define NOTE_LEN16	(NOTE_LEN_BASE*4)		// 1/16
#define NOTE_LEN32DOT	(NOTE_LEN_BASE*3)		// 1/32.
#define NOTE_LEN32	(NOTE_LEN_BASE*2)		// 1/32
#define NOTE_LEN64DOT	(NOTE_LEN_BASE+NOTE_LEN_BASE/2)	// 1/64
#define NOTE_LEN64	(NOTE_LEN_BASE*1)		// 1/64

#define NOTE_STOP	0		// stop mark of the melody

// get divider of the tone in 0.01 Hz (minimum 16 Hz), time base is 1 MHz
#define SND_TONE_INC(hz01) ((u32)(((u64)(hz01)<<32)/(100*PWMSND_RATE)))

#define NOTE_C0		SND_TONE_INC(1635)	// note=C0 (16.3516Hz)
#define NOTE_CS0	SND_TONE_INC(1732)	// note=C#0 (17.3239Hz)
#define NOTE_D0		SND_TONE_INC(1835)	// note=D0 (18.354Hz)
#define NOTE_DS0	SND_TONE_INC(1945)	// note=D#0 (19.4454Hz)
#define NOTE_E0		SND_TONE_INC(2060)	// note=E0 (20.6017Hz)
#define NOTE_F0		SND_TONE_INC(2183)	// note=F0 (21.8268Hz)
#define NOTE_FS0	SND_TONE_INC(2312)	// note=F#0 (23.1247Hz)
#define NOTE_G0		SND_TONE_INC(2450)	// note=G0 (24.4997Hz)
#define NOTE_GS0	SND_TONE_INC(2596)	// note=G#0 (25.9565Hz)
#define NOTE_A0		SND_TONE_INC(2750)	// note=A0 (27.5Hz)
#define NOTE_AS0	SND_TONE_INC(2914)	// note=A#0 (29.1352Hz)
#define NOTE_B0		SND_TONE_INC(3087)	// note=B0 (30.8677Hz)

#define NOTE_C1		SND_TONE_INC(3270)	// note=C1 (32.7032Hz)
#define NOTE_CS1	SND_TONE_INC(3465)	// note=C#1 (34.6478Hz)
#define NOTE_D1		SND_TONE_INC(3671)	// note=D1 (36.7081Hz)
#define NOTE_DS1	SND_TONE_INC(3889)	// note=D#1 (38.8909Hz)
#define NOTE_E1		SND_TONE_INC(4120)	// note=E1 (41.2034Hz)
#define NOTE_F1		SND_TONE_INC(4365)	// note=F1 (43.6535Hz)
#define NOTE_FS1	SND_TONE_INC(4625)	// note=F#1 (46.2493Hz)
#define NOTE_G1		SND_TONE_INC(4900)	// note=G1 (48.9994Hz)
#define NOTE_GS1	SND_TONE_INC(5191)	// note=G#1 (51.9131Hz)
#define NOTE_A1		SND_TONE_INC(5500)	// note=A1 (55Hz)
#define NOTE_AS1	SND_TONE_INC(5827)	// note=A#1 (58.2705Hz)
#define NOTE_B1		SND_TONE_INC(6174)	// note=B1 (61.7354Hz)

#define NOTE_C2		SND_TONE_INC(6541)	// note=C2 (65.4064Hz)
#define NOTE_CS2	SND_TONE_INC(6930)	// note=C#2 (69.2957Hz)
#define NOTE_D2		SND_TONE_INC(7342)	// note=D2 (73.4162Hz)
#define NOTE_DS2	SND_TONE_INC(7778)	// note=D#2 (77.7817Hz)
#define NOTE_E2		SND_TONE_INC(8241)	// note=E2 (82.4069Hz)
#define NOTE_F2		SND_TONE_INC(8731)	// note=F2 (87.3071Hz)
#define NOTE_FS2	SND_TONE_INC(9250)	// note=F#2 (92.4986Hz)
#define NOTE_G2		SND_TONE_INC(9800)	// note=G2 (97.9989Hz)
#define NOTE_GS2	SND_TONE_INC(10383)	// note=G#2 (103.826Hz)
#define NOTE_A2		SND_TONE_INC(11000)	// note=A2 (110Hz)
#define NOTE_AS2	SND_TONE_INC(11654)	// note=A#2 (116.541Hz)
#define NOTE_B2		SND_TONE_INC(12347)	// note=B2 (123.471Hz)

#define NOTE_C3		SND_TONE_INC(13081)	// note=C3 (130.813Hz)
#define NOTE_CS3	SND_TONE_INC(13859)	// note=C#3 (138.591Hz)
#define NOTE_D3		SND_TONE_INC(14683)	// note=D3 (146.832Hz)
#define NOTE_DS3	SND_TONE_INC(15556)	// note=D#3 (155.563Hz)
#define NOTE_E3		SND_TONE_INC(16481)	// note=E3 (164.814Hz)
#define NOTE_F3		SND_TONE_INC(17461)	// note=F3 (174.614Hz)
#define NOTE_FS3	SND_TONE_INC(18500)	// note=F#3 (184.997Hz)
#define NOTE_G3		SND_TONE_INC(19600)	// note=G3 (195.998Hz)
#define NOTE_GS3	SND_TONE_INC(20765)	// note=G#3 (207.652Hz)
#define NOTE_A3		SND_TONE_INC(22000)	// note=A3 (220Hz)
#define NOTE_AS3	SND_TONE_INC(23308)	// note=A#3 (233.082Hz)
#define NOTE_B3		SND_TONE_INC(24694)	// note=B3 (246.942Hz)

#define NOTE_C4		SND_TONE_INC(26163)	// note=C4 (261.626Hz)
#define NOTE_CS4	SND_TONE_INC(27718)	// note=C#4 (277.183Hz)
#define NOTE_D4		SND_TONE_INC(29367)	// note=D4 (293.665Hz)
#define NOTE_DS4	SND_TONE_INC(31113)	// note=D#4 (311.127Hz)
#define NOTE_E4		SND_TONE_INC(32963)	// note=E4 (329.628Hz)
#define NOTE_F4		SND_TONE_INC(34923)	// note=F4 (349.228Hz)
#define NOTE_FS4	SND_TONE_INC(36999)	// note=F#4 (369.994Hz)
#define NOTE_G4		SND_TONE_INC(39200)	// note=G4 (391.995Hz)
#define NOTE_GS4	SND_TONE_INC(41531)	// note=G#4 (415.305Hz)
#define NOTE_A4		SND_TONE_INC(44000)	// note=A4 (440Hz)
#define NOTE_AS4	SND_TONE_INC(46616)	// note=A#4 (466.164Hz)
#define NOTE_B4		SND_TONE_INC(49388)	// note=B4 (493.883Hz)

#define NOTE_C5		SND_TONE_INC(52325)	// note=C5 (523.251Hz)
#define NOTE_CS5	SND_TONE_INC(55437)	// note=C#5 (554.365Hz)
#define NOTE_D5		SND_TONE_INC(58733)	// note=D5 (587.33Hz)
#define NOTE_DS5	SND_TONE_INC(62225)	// note=D#5 (622.254Hz)
#define NOTE_E5		SND_TONE_INC(65926)	// note=E5 (659.255Hz)
#define NOTE_F5		SND_TONE_INC(69846)	// note=F5 (698.456Hz)
#define NOTE_FS5	SND_TONE_INC(73999)	// note=F#5 (739.989Hz)
#define NOTE_G5		SND_TONE_INC(78399)	// note=G5 (783.991Hz)
#define NOTE_GS5	SND_TONE_INC(83061)	// note=G#5 (830.609Hz)
#define NOTE_A5		SND_TONE_INC(88000)	// note=A5 (880Hz)
#define NOTE_AS5	SND_TONE_INC(93233)	// note=A#5 (932.328Hz)
#define NOTE_B5		SND_TONE_INC(98777)	// note=B5 (987.767Hz)

#define NOTE_C6		SND_TONE_INC(104650)	// note=C6 (1046.5Hz)
#define NOTE_CS6	SND_TONE_INC(110873)	// note=C#6 (1108.73Hz)
#define NOTE_D6		SND_TONE_INC(117466)	// note=D6 (1174.66Hz)
#define NOTE_DS6	SND_TONE_INC(124451)	// note=D#6 (1244.51Hz)
#define NOTE_E6		SND_TONE_INC(131851)	// note=E6 (1318.51Hz)
#define NOTE_F6		SND_TONE_INC(139691)	// note=F6 (1396.91Hz)
#define NOTE_FS6	SND_TONE_INC(147998)	// note=F#6 (1479.98Hz)
#define NOTE_G6		SND_TONE_INC(156798)	// note=G6 (1567.98Hz)
#define NOTE_GS6	SND_TONE_INC(166122)	// note=G#6 (1661.22Hz)
#define NOTE_A6		SND_TONE_INC(176000)	// note=A6 (1760Hz)
#define NOTE_AS6	SND_TONE_INC(186466)	// note=A#6 (1864.66Hz)
#define NOTE_B6		SND_TONE_INC(197553)	// note=B6 (1975.53Hz)

#define NOTE_C7		SND_TONE_INC(209300)	// note=C7 (2093Hz)
#define NOTE_CS7	SND_TONE_INC(221746)	// note=C#7 (2217.46Hz)
#define NOTE_D7		SND_TONE_INC(234932)	// note=D7 (2349.32Hz)
#define NOTE_DS7	SND_TONE_INC(248902)	// note=D#7 (2489.02Hz)
#define NOTE_E7		SND_TONE_INC(263702)	// note=E7 (2637.02Hz)
#define NOTE_F7		SND_TONE_INC(279383)	// note=F7 (2793.83Hz)
#define NOTE_FS7	SND_TONE_INC(295996)	// note=F#7 (2959.96Hz)
#define NOTE_G7		SND_TONE_INC(313596)	// note=G7 (3135.96Hz)
#define NOTE_GS7	SND_TONE_INC(332244)	// note=G#7 (3322.44Hz)
#define NOTE_A7		SND_TONE_INC(352000)	// note=A7 (3520Hz)
#define NOTE_AS7	SND_TONE_INC(372931)	// note=A#7 (3729.31Hz)
#define NOTE_B7		SND_TONE_INC(395107)	// note=B7 (3951.07Hz)

#define NOTE_C8		SND_TONE_INC(418601)	// note=C8 (4186.01Hz)
#define NOTE_CS8	SND_TONE_INC(443492)	// note=C#8 (4434.92Hz)
#define NOTE_D8		SND_TONE_INC(469864)	// note=D8 (4698.64Hz)
#define NOTE_DS8	SND_TONE_INC(497803)	// note=D#8 (4978.03Hz)
#define NOTE_E8		SND_TONE_INC(527404)	// note=E8 (5274.04Hz)
#define NOTE_F8		SND_TONE_INC(558765)	// note=F8 (5587.65Hz)
#define NOTE_FS8	SND_TONE_INC(591991)	// note=F#8 (5919.91Hz)
#define NOTE_G8		SND_TONE_INC(627193)	// note=G8 (6271.93Hz)
#define NOTE_GS8	SND_TONE_INC(664488)	// note=G#8 (6644.88Hz)
#define NOTE_A8		SND_TONE_INC(704000)	// note=A8 (7040Hz)
#define NOTE_AS8	SND_TONE_INC(745862)	// note=A#8 (7458.62Hz)
#define NOTE_B8		SND_TONE_INC(790213)	// note=B8 (7902.13Hz)

#define NOTE_C9		SND_TONE_INC(837202)	// note=C9 (8372.02Hz)
#define NOTE_CS9	SND_TONE_INC(886984)	// note=C#9 (8869.84Hz)
#define NOTE_D9		SND_TONE_INC(939727)	// note=D9 (9397.27Hz)
#define NOTE_DS9	SND_TONE_INC(995606)	// note=D#9 (9956.06Hz)
#define NOTE_E9		SND_TONE_INC(1054810)	// note=E9 (10548.1Hz)
#define NOTE_F9		SND_TONE_INC(1117530)	// note=F9 (11175.3Hz)
#define NOTE_FS9	SND_TONE_INC(1183980)	// note=F#9 (11839.8Hz)
#define NOTE_G9		SND_TONE_INC(1254390)	// note=G9 (12543.9Hz)
#define NOTE_GS9	SND_TONE_INC(1328980)	// note=G#9 (13289.8Hz)
#define NOTE_A9		SND_TONE_INC(1408000)	// note=A9 (14080Hz)
#define NOTE_AS9	SND_TONE_INC(1491720)	// note=A#9 (14917.2Hz)
#define NOTE_B9		SND_TONE_INC(1580430)	// note=B9 (15804.3Hz)

#define NOTE_R		0			// pause

// tone of the melody
typedef struct {
	u32	len;			// tone length counter (0=end of melody)
	u32	inc;			// tone phase fraction increment (0=silent)
} sMelodyTone;

// PWM sound channel
typedef struct {
	int		form;		// sound format SNDFORM_*
	int		sampsize;	// (double-)sample size in bytes (1, 2 or 4)
	// current sound
	volatile const u8* snd;		// pointer to current playing sound or melody
	volatile s32	cnt;		// counter of remaining (double-)samples (0 = no sound)
	volatile u32	acc;		// position accumulator * SNDINT - fraction part; or u32 tone phase accumulator 32-bit fraction
	// speed
	u32		inc;		// increment of the pointer * SNDINT, or tone phase fraction increment (0=silent)
	float		speed;		// initial sound speed (relative to SOUNDRATE)
	float		rate;		// sound rate / SOUNDRATE
	// next sound
	const u8*	next;		// pointer to next sound to play repeated sound
	u32		nextcnt;	// length of next sound in number of (double-)samples (stream: 0=this sample was taken)
	volatile int	repeat;		// repeat counter or SNDREPEAT_* flag (0=no repeat, -1=repeat, -2=stream)
	// volume
	int		volL;		// sound volume LEFT (PWMSND_PERIOD = normal, negative volume inverts signal)
	int		volR;		// sound volume RIGHT (PWMSND_PERIOD = normal, negative volume inverts signal)
	float		volume;		// initial sound volume 0..1 (1=normal, or can be > 1 to get higher volume)
	float		panning;	// initial sound panning 0..1 (0=left, 0.5=middle, 1=right; panning outside range 0..1 will invert signal)
} sPwmSnd;

#define VOLMAX		15		// max. volume index
#define VOLDEF		9		// default volume "full"

// Sound (exported by the PiLibSnd tool)
typedef struct {
	u32		datasize;	// data size in bytes (without header)
	u16		dataoff;	// offset of start of data (= size of this header = 12)
	u16		rate;		// sample rate (usually 22050, 44100)
	u16		sampblock;	// ADPCM samples per block
	u8		bits;		// number of bits per sample (4, 8, or 16)
	u8		chan;		// number of channels (1 or 2)
	// ... additional header data
	// ... data
} sSound;

// global sound mute
extern Bool GlobalSoundMute;

// global sound volume (0..VOLMAX, VOLDEF=default)
extern int GlobalSoundVolume;

// PWM sound channels
extern sPwmSnd PwmSound[PWMSND_CHANNUM];

// sound buffers
extern u32 PwmSoundBuf[SND_BUFSIZE/4*SND_BUFNUM]; // 1 stereo sample = 2 words u32

// initialize PWM sound output
void PWMSndInit();

// terminate PWM sound output
void PWMSndTerm();

// stop playing sound
void StopSoundChan(int chan);

// stop playing sound of channel 0
void StopSound();

// stop playing sounds of all channels
void StopAllSound();

// Convert length of sound in bytes to number of samples
//  size = length of sound in bytes (use sizeof(array))
//  form = sound format SNDFORM_* (8-bit, 16-bit, mono or stereo)
// Returns length of sound in samples (or double-samples for stereo)
int SoundByteToLen(int size, int form);

// play sound or music
//  chan = channel 0..PWMSND_CHANNUM-1
//  sound = pointer to sound in sSound format (speed>0), or music data in sMelodyTone format (speed=0)
//  rep = repeat counter (enter number of repeats, or flag SNDREPEAT_*)
//  speed = relative speed, 1=normal, or 0=play music
//  volume = volume 0..1 (1=normal, or can be > 1 to get higher volume)
//  panning = panning 0..1 (0=left, 0.5=middle, 1=right; panning outside range 0..1 will invert signal)
void PlaySoundChan(int chan, const void* sound, int rep, float speed, float volume, float panning);

// play sound at channel 0
//  sound = pointer to sound in sSound format
void PlaySound(const void* sound);

// play sound at channel 0 repeated
//  sound = pointer to sound in sSound format
void PlaySoundRep(const void* sound);

// play music at channel 0
//  melody = pointer to table of tones sMelodyTone* (terminated with tone of len=0)
void PlayMelody(const sMelodyTone* melody);

// play music at channel 0 repeated
//  melody = pointer to table of tones sMelodyTone* (terminated with tone of len=0)
void PlayMelodyRep(const sMelodyTone* melody);

// update sound relative speed
void SpeedSoundChan(int chan, float speed);

// update sound relative speed of channel 0
void SpeedSound(float speed);

// update sound volume (1=normal volume)
void VolumeSoundChan(int chan, float volume);

// update sound volume of channel 0 (1=normal volume)
void VolumeSound(float volume);

// update sound panning (0..1, 0=left, 0.5=middle, 1=right; panning outside range 0..1 will invert signal)
void PanningSoundChan(int chan, float panning);

// update sound panning of channel 0 (0..1, 0=left, 0.5=middle, 1=right; panning outside range 0..1 will invert signal)
void PanningSound(float panning);

// check if sound is playing
Bool PlayingSoundChan(int chan);

// check if sound of channel 0 is playing
Bool PlayingSound();

// set next repeated sound in the same format
//  snddata = pointer to sound data
//  size = length of sound in number of bytes
void SetNextSoundChan(int chan, const void* snddata, int size);

// set next repeated sound of channel 0 in the same format
//  snddata = pointer to sound data
//  size = length of sound in number of bytes
void SetNextSound(const void* snddata, int size);

// check if streaming buffer is empty
Bool SoundStreamIsEmpty(int chan);

// set next streaming buffer
//  snddata = pointer to sound data
//  size = length of sound in number of bytes
void SoundStreamSetNext(int chan, const void* snddata, int size);

// global sound set OFF
void GlobalSoundSetOff();

// global sound set ON
void GlobalSoundSetOn();

// set new global volume 0..VOLMAX (= 0..15)
void GlobalVolumeSet(int vol);

#endif // _LIB_PWMSND_H

#endif // USE_PWMSND
