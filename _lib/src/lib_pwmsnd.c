
// ****************************************************************************
//
//                                PWM sound output
//
// ****************************************************************************

#include "../../includes.h"	// includes

#if USE_PWMSND		// 1=use PWM sound output, 0=not used (lib_pwmsnd.*)

// global sound mute
Bool GlobalSoundMute = False;

// volume table
const int VolMulTab[VOLMAX+1] = {
	5,		// 0
	7,		// 1
	11,		// 2
	15,		// 3
	22,		// 4
	31,		// 5
	45,		// 6
	63,		// 7
	90,		// 8
	127,		// 9 ... default
	180,		// 10
	255,		// 11
	361,		// 12
	511,		// 13
	723,		// 14
	1023,		// 15
};

// global sound volume (0..VOLMAX, VOLDEF=default)
int GlobalSoundVolume = VOLDEF;			// current volume
int GlobalSoundVolMul = VolMulTab[VOLDEF];	// current volume multiplier

// PWM sound was initialized
Bool PWMSndWasInit = False;

// PWM sound channels
sPwmSnd PwmSound[PWMSND_CHANNUM];

// sound buffers
u32 PwmSoundBuf[SND_BUFSIZE/4*SND_BUFNUM]; // 1 stereo sample = 2 words u32

// DMA control blocks
DMA_CB_t PwmCb[SND_BUFNUM];

// next control block to generate
DMA_CB_t* PwmNextCb = PwmCb;

// IRQ handler
void PwmHandler(void)
{
	int i;

	// clear IRQ request
	DMA_IntClr(PWMSND_DMACHAN);

	// dithering constant
	int di = SNDINT/4;

	// loop through control blocks, up to DMA current control block
	DMA_t* dma = DMA(PWMSND_DMACHAN);
	DMA_CB_t* cb = PwmNextCb;
	while ((void*)cb != DMA_ArmAddr(dma->CB))
	{
		// invalid CB
		if (dma->CB == 0) break;

		// buffer address
		u32* buf = DMA_ArmAddr(cb->SRC);
		u32* bufend = buf + SND_BUFSIZE/4;

		// loop addresses
		while (buf != bufend)
		{
			// default sample << SNDFRAC
			int sampL = (PWMSND_PERIOD/2)<<SNDFRAC;
			int sampR = sampL;

			// loop all channels
			sPwmSnd* snd = PwmSound;
			sPwmSnd* sndend = &PwmSound[PWMSND_CHANNUM];
			while (snd != sndend)
			{
				// get counter of remaining (double-)samples (0 = no sound)
				s32 cnt = snd->cnt;
				if (cnt > 0)	// check if this sound channel is valid
				{
					// preset - no sound sample
					int ssL = 0;
					int ssR = 0;

					// get position accumulator
					u32 acc = snd->acc;
					const u8* s = (const u8*)snd->snd; // sound pointer

					// PCM 8-bit mono
					int form = snd->form;
					if (form == SNDFORM_PCM8)
					{
						// load 2 samples
						int s0 = (int)s[0] - 128;
						int s1 = s0;
						if (cnt > 1) s1 = (int)s[1] - 128;

						// interpolate samples
						int ss = s0*SNDINT + (s1 - s0)*acc;

						// multiply by volume (range 0..PWMSND_PERIOD)
						ssL = (int)(((s64)ss * snd->volL) >> 8);
						ssR = (int)(((s64)ss * snd->volR) >> 8);
					}

					// PCM 8-bit stereo
					else if (form == SNDFORM_PCM8S)
					{
						// load 2*2 samples
						int s0L = (int)s[0] - 128;
						int s0R = (int)s[1] - 128;
						int s1L = s0L;
						int s1R = s0R;
						if (cnt > 1)
						{
							s1L = (int)s[2] - 128;
							s1R = (int)s[3] - 128;
						}

						// interpolate samples
						ssL = s0L*SNDINT + (s1L - s0L)*acc;
						ssR = s0R*SNDINT + (s1R - s0R)*acc;

						// multiply by volume (range 0..PWMSND_PERIOD)
						ssL = (int)(((s64)ssL * snd->volL) >> 8);
						ssR = (int)(((s64)ssR * snd->volR) >> 8);
					}

					// PCM 16-bit mono
					else if (form == SNDFORM_PCM16)
					{
						// load 2 samples
						const s16* p = (const s16*)s; // pointer to current sample
						int s0 = p[0];
						int s1 = s0;
						if (cnt > 1) s1 = p[1];

						// interpolate samples
						int ss = s0*SNDINT + (s1 - s0)*acc;

						// multiply by volume (range 0..PWMSND_PERIOD)
						ssL = (int)(((s64)ss * snd->volL) >> 16);
						ssR = (int)(((s64)ss * snd->volR) >> 16);
					}

					// PCM 16-bit stereo
					else if (form == SNDFORM_PCM16S)
					{
						// load 2*2 samples
						const s16* p = (const s16*)s; // pointer to current sample
						int s0L = p[0];
						int s0R = p[1];
						int s1L = s0L;
						int s1R = s0R;
						if (cnt > 1)
						{
							s1L = p[2];
							s1R = p[3];
						}

						// interpolate samples
						ssL = s0L*SNDINT + (s1L - s0L)*acc;
						ssR = s0R*SNDINT + (s1R - s0R)*acc;

						// multiply by volume (range 0..PWMSND_PERIOD)
						ssL = (int)(((s64)ssL * snd->volL) >> 16);
						ssR = (int)(((s64)ssR * snd->volR) >> 16);
					}

					// tones
					u32 inc = snd->inc;	// increment of the pointer
					if (form == SNDFORM_MELODY)
					{
						// prepare sample
						if (inc != 0) 		// check silence
						{
							ssL = snd->volL * (SNDINT/2);
							ssR = snd->volR * (SNDINT/2);
							if ((s32)acc < 0)
							{
								ssL = -ssL;
								ssR = -ssR;
							}

							// shift phase
							acc += inc;
						}

						// tone length counter
						cnt--;

						// end of tone
						if (cnt == 0)
						{
							const sMelodyTone* t = (const sMelodyTone*)s;
							t++;
							snd->snd = (const u8*)t;
							cnt = t->len;		// length (0=end of melody)
							snd->inc = t->inc;	// increment (0=silent)
							acc = 0;		// clear phase accumulator

							// end of melody
							if (cnt <= 0)
							{
								// repeat
								int rep = snd->repeat; // repeat counter
								t = (const sMelodyTone*)snd->next;
								if ((rep != 0) && (t != NULL) && (t->len != 0))
								{
									// decrement repeat counter
									if (rep > 0) // not infinity
									{
										rep--;
										snd->repeat = rep;
									}

									// get next pointer
									snd->snd = (const u8*)t;
									cnt = t->len;
									snd->inc = t->inc;

									// stream - this buffer was accepted
									if (rep == SNDREPEAT_STREAM)
									{
										snd->nextcnt = 0;
										snd->next = NULL;
									}
								}
							}
						}
					}

					// shift PCM sample
					else
					{
						acc += inc;		// shift accumulator
						int k = acc >> SNDFRAC;	 // integer increment
						snd->snd = s + snd->sampsize*k; // shift sound pointer
						cnt -= k;		// shift sample counter
						acc &= (SNDINT-1); 	// clear integer bits

						// end of sample
						if (cnt <= 0)
						{
							// repeat
							int rep = snd->repeat; // repeat counter
							u32 nextcnt = snd->nextcnt;
							if ((rep != 0) && (nextcnt != 0) && (snd->next != NULL))
							{
								// decrement repeat counter
								if (rep > 0) // not infinity
								{
									rep--;
									snd->repeat = rep;
								}

								// get next pointer ... at this place, 'cnt' can be negative
								snd->snd = snd->next - cnt;
								cnt = nextcnt + cnt;

								// stream - this buffer was accepted
								if (rep == SNDREPEAT_STREAM)
								{
									snd->nextcnt = 0;
									snd->next = NULL;
								}
							}
						}
					}
					snd->acc = acc;
					snd->cnt = cnt;

					// add to result sample
					sampL += ssL;
					sampR += ssR;
				}

				// next sound channel
				snd++;
			}

			// time dithering and rounding
			sampL += di;
			sampR += di;

			// shift dithering constant
			di ^= (SNDINT/4) ^ (SNDINT*3/4);
	
			// limit sound samples
			sampL >>= SNDFRAC;
			if ((uint)sampL > (uint)(PWMSND_PERIOD-1))
			{
				if (sampL < 0)
					sampL = 0;
				else
					sampL = PWMSND_PERIOD-1;
			}
			sampR >>= SNDFRAC;
			if ((uint)sampR > (uint)(PWMSND_PERIOD-1))
			{
				if (sampR < 0)
					sampR = 0;
				else
					sampR = PWMSND_PERIOD-1;
			}

			// save samples
#if PWMSND_SWAP	 // 1=swap channels (PWM1 left, PWM0 right), 0=no swap (PWM0 left, PWM1 right)
			buf[0] = sampR;
			buf[1] = sampL;
#else
			buf[0] = sampL;
			buf[1] = sampR;
#endif
			// shift buffer
			buf += 2;
		}

		// invalidate cache
		CleanAndInvalidateDataCacheRange((uintptr_t)DMA_ArmAddr(cb->SRC), SND_BUFSIZE);

		// next control block
		cb = (DMA_CB_t*)DMA_ArmAddr(cb->NEXT);
	}
	PwmNextCb = cb;
}

// initialize PWM sound output
void PWMSndInit()
{
	// terminate PWM sound output
	PWMSndTerm();

	// sound is OFF
	if (GlobalSoundMute) return;

	// initialize GPIOs
	GPIO_Func(PWMSND_GPIO0, (PWMSND_GPIO0 == 18) ? GPIO_FUNC_AF5 : GPIO_FUNC_AF0);
	GPIO_Func(PWMSND_GPIO1,  (PWMSND_GPIO1 == 19) ? GPIO_FUNC_AF5 : GPIO_FUNC_AF0);

	// preset sound buffers
	int i;
	for (i = 0; i < SND_BUFSIZE/4*SND_BUFNUM; i++) PwmSoundBuf[i] = PWMSND_PERIOD/2;

	// reset sound channel
	sPwmSnd* snd = PwmSound;
	for (i = 0; i < PWMSND_CHANNUM; i++)
	{
		// no current and no next sound
		snd->cnt = 0;
		snd->nextcnt = 0;
		snd->next = NULL;
		snd->repeat = 0;

		// next sound channel
		snd++;
	}

	// initialize DMA control blocks
	DMA_CB_t* cb = PwmCb;
	PwmNextCb = PwmCb; // next control block to generate
	u32* buf = PwmSoundBuf;
	for (i = 0; i < SND_BUFNUM; i++)
	{
		// transfer information
		u32 cfg =
			B0 |		// generate interrupt
			// B1 |		// 1=2D mode (only channels 0..6)
			B3 |		// 1=wait for AXI write response
			// B4 |		// 1=destination address incremented
			// B5 |		// 1=128-bit destination width
			B6 |		// 1=destination gated by DREQ
			// B7 |		// 1=ignore writes
			B8 |		// 1=source address incremented
			// B9 |		// 1=128-bit source width
			// B10 |	// 1=source gated by DREQ
			// B11 |	// 1=ignore reads
			(DMA_BURST_IO << 12) | // burst length
			(DREQ_PWM << 16) | // peripheral mapping
			// (0 << 21) |	// dummy wait cycles
			// B26 |	// 1=don't do wide writes
			0;
		cb->TI = cfg;

		// source address
		cb->SRC = DMA_BusAddr(buf);

		// destination address
		cb->DST = DMA_BusAddr((void*)&PWM->FIF1);

		// data length (number of bytes)
		cb->LEN = SND_BUFSIZE;

		// no stride
		cb->STRIDE = 0;

		// next control block
		cb->NEXT = DMA_BusAddr((i == SND_BUFNUM-1) ? PwmCb : (cb + 1));

		// reserved
		cb->res[0] = 0;
		cb->res[1] = 0;

		// next control block
		cb++;
		buf += SND_BUFSIZE/4;
	}

	// invalidate caches
	CleanAndInvalidateDataCacheRange((uintptr_t)PwmCb, SND_BUFNUM*sizeof(DMA_CB_t));
	CleanAndInvalidateDataCacheRange((uintptr_t)PwmSoundBuf, SND_BUFSIZE*SND_BUFNUM);

	// setup PWM clock
	GpioClockStart(GPIOCLOCK_PWM, GPIOCLOCK_SRC_PLLD, PWMSND_PRESC<<12, GPIOCLOCK_MASH_INT);

	// setup PWM
	PWM_t* pwm = PWM;
	pwm->RNG1 = PWMSND_PERIOD-1;
	pwm->RNG2 = PWMSND_PERIOD-1;
	pwm->CTL =		// enable PWM0 and PWM1, use N/M PWM algorithm, use FIFO, clear FIFO
			B0 |		// channel 1: 1=enable
			// B1 |		// channel 1: 1=use serialiser, 0=use PWM
			// B2 |		// channel 1: 1=repeat last FIFO data
			// B3 |		// channel 1: idle state of output
			// B4 |		// channel 1: invert polarity
			B5 |		// channel 1: 1=use FIFO, 0=use data
			B6 |		// 1=clear FIFO
			// B7 |		// channel 1: 1=M/S simple mode, 0=N/M algorithm
			B8 |		// channel 2: 1=enable
			// B9 |		// channel 2: 1=use serialiser, 0=use PWM
			// B10 |	// channel 2: 1=repeat last FIFO data
			// B11 |	// channel 2: idle state of output
			// B12 |	// channel 2: invert polarity
			B13 |		// channel 2: 1=use FIFO, 0=use data
			// B15 |	// channel 2: 1=M/S simple mode, 0=N/M algorithm
			0;
	pwm->DMAC = 7 | (7 << 8) | B31;	// DMA enable, DREQ threshold 7

	// set DMA interrupt handler
	IntHandler(PWMSND_DMAIRQ, PwmHandler);
	IntEnable(PWMSND_DMAIRQ);

	// enable DMA channel and start DMA transfer
	DMA_Start(PWMSND_DMACHAN, PwmCb);

	// was initialized
	PWMSndWasInit = True;
}

// terminate PWM sound output
void PWMSndTerm()
{
	// was initialized
	if (!PWMSndWasInit) return;
	PWMSndWasInit = False;

	// disable DMA interrupt
	IntDisable(PWMSND_DMAIRQ);

	// reset DMA channel
	DMA_Reset(PWMSND_DMACHAN);
	dsb();

	// stop PWM
	PWM_Stop();

	// Stop GPIO clock
	GpioClockStop(GPIOCLOCK_PWM);

	// reset GPIOs
	GPIO_Func(PWMSND_GPIO0, GPIO_FUNC_IN);
	GPIO_Func(PWMSND_GPIO1, GPIO_FUNC_IN);
}

// stop playing sound
void StopSoundChan(int chan)
{
	IRQ_LOCK;
	dmb();
	sPwmSnd* snd = &PwmSound[chan];
	snd->nextcnt = 0;
	snd->next = NULL;
	snd->repeat = 0;
	dmb();
	snd->cnt = 0;
	dmb();
	IRQ_UNLOCK;
}

// stop playing sound of channel 0
void StopSound()
{
	StopSoundChan(0);
}

// stop playing sounds of all channels
void StopAllSound()
{
	int i;
	for (i = 0; i < PWMSND_CHANNUM; i++) StopSoundChan(i);
}

// Convert length of sound in bytes to number of samples
//  size = length of sound in bytes (use sizeof(array))
//  form = sound format SNDFORM_* (8-bit, 16-bit, mono or stereo)
// Returns length of sound in samples (or double-samples for stereo)
int SoundByteToLen(int size, int form)
{
	switch(form)
	{
	// 8-bits unsigned per sample, stereo
	// 16-bits signed per sample, mono
	case SNDFORM_PCM8S:
	case SNDFORM_PCM16: return size/2;

	// 16-bits signed per sample, stereo
	case SNDFORM_PCM16S: return size/4;

	// 8-bits unsigned per sample, mono
	case SNDFORM_PCM8:
	default: return size;
	}
}

// Convert length in number of samples to number of bytes
//  len = length of sound in samples (or double-samples for stereo)
//  form = sound format SNDFORM_* (8-bit, 16-bit, mono or stereo)
// Returns length of sound in bytes.
int SoundLenToByte(int len, int form)
{
	switch(form)
	{
	// 8-bits unsigned per sample, stereo
	// 16-bits signed per sample, mono
	case SNDFORM_PCM8S:
	case SNDFORM_PCM16: return len*2;

	// 16-bits signed per sample, stereo
	case SNDFORM_PCM16S: return len*4;

	// 8-bits unsigned per sample, mono
	case SNDFORM_PCM8:
	default: return len;
	}
}

// play sound or music
//  chan = channel 0..PWMSND_CHANNUM-1
//  sound = pointer to sound in sSound format (speed>0), or music data in sMelodyTone format (speed=0)
//  rep = repeat counter (enter number of repeats, or flag SNDREPEAT_*)
//  speed = relative speed, 1=normal, or 0=play music
//  volume = volume 0..1 (1=normal, or can be > 1 to get higher volume)
//  panning = panning 0..1 (0=left, 0.5=middle, 1=right; panning outside range 0..1 will invert signal)
void PlaySoundChan(int chan, const void* sound, int rep, float speed, float volume, float panning)
{
	// global sound is OFF
	if (GlobalSoundMute) return;

	// stop playing sound
	StopSoundChan(chan);

	// pointer to sound channel
	sPwmSnd* s = &PwmSound[chan];

	// get sound info
	int form = SNDFORM_MELODY;
	int size = 0;
	const void* snd = sound;
	if (speed != 0)
	{
		const sSound* ssound = (const sSound*)sound;
		snd = (const void*)((const u8*)ssound + ssound->dataoff);
		size = ssound->datasize; // size of data in bytes
		int rate = ssound->rate; // sample rate
		int bits = ssound->bits; // number of bits per sample
		int ch = ssound->chan; // number of channels
		form = (bits == 8) ?
				((ch == 1) ? SNDFORM_PCM8 : SNDFORM_PCM8S) :
				((ch == 1) ? SNDFORM_PCM16 : SNDFORM_PCM16S);
		s->rate = (float)rate/SOUNDRATE;
		speed *= s->rate; // relative speed
	}

	// convert sound size to the length in (double-)samples
	int len = SoundByteToLen(size, form);

	// sound format
	s->form = form;
	s->sampsize = SoundLenToByte(1, form);

	// prepare volume
	s->volume = volume;
	s->panning = panning;
	s->volL = (int)(PWMSND_PERIOD * volume * cos(panning * PI/2) * GlobalSoundVolMul / VolMulTab[VOLDEF] + 0.5f);
	s->volR = (int)(PWMSND_PERIOD * volume * sin(panning * PI/2) * GlobalSoundVolMul / VolMulTab[VOLDEF] + 0.5f);

	// repeated sound
	s->next = NULL;
	s->nextcnt = 0;
	s->repeat = rep;
	if ((rep != SNDREPEAT_NO) && (rep != SNDREPEAT_STREAM))
	{
		s->next = (const u8*)snd;
		s->nextcnt = len;
	}

	// speed increment
	s->speed = speed;
	s->inc = (u32)(SNDINT * speed + 0.5f); // sample increment
	s->acc = 0;

	// tones
	if (form == SNDFORM_MELODY)
	{
		const sMelodyTone* t = (const sMelodyTone*)snd;
		len = t->len; // tone length counter
		s->inc = t->inc; // tone phase increment
	}

	// start current sound
	s->snd = (const u8*)snd;
	dmb();
	s->cnt = len;
	dmb();
}

// play sound at channel 0
//  sound = pointer to sound in sSound format
void PlaySound(const void* sound)
{
	PlaySoundChan(0, sound, SNDREPEAT_NO, 1.0f, 1.0f, 0.5f);
}

// play sound at channel 0 repeated
//  sound = pointer to sound in sSound format
void PlaySoundRep(const void* sound)
{
	PlaySoundChan(0, sound, SNDREPEAT_REPEAT, 1.0f, 1.0f, 0.5f);
}

// play music at channel 0
//  melody = pointer to table of tones sMelodyTone* (terminated with tone of len=0)
void PlayMelody(const sMelodyTone* melody)
{
	PlaySoundChan(0, (const void*)melody, SNDREPEAT_NO, 0, 1.0f, 1.0f);
}

// play music at channel 0 repeated
//  melody = pointer to table of tones sMelodyTone* (terminated with tone of len=0)
void PlayMelodyRep(const sMelodyTone* melody)
{
	PlaySoundChan(0, (const void*)melody, SNDREPEAT_REPEAT, 0, 1.0f, 1.0f);
}

// update sound relative speed
void SpeedSoundChan(int chan, float speed)
{
	speed *= PwmSound[chan].rate;
	PwmSound[chan].speed = speed;
	PwmSound[chan].inc = (u32)(SNDINT * speed + 0.5f);
}

// update sound relative speed of channel 0
void SpeedSound(float speed)
{
	SpeedSoundChan(0, speed);
}

// update sound volume (1=normal volume)
void VolumeSoundChan(int chan, float volume)
{
	sPwmSnd* s = &PwmSound[chan];
	s->volume = volume;
	s->volL = (int)(PWMSND_PERIOD * volume * cos(s->panning * PI/2) * GlobalSoundVolMul / VolMulTab[VOLDEF] + 0.5f);
	s->volR = (int)(PWMSND_PERIOD * volume * sin(s->panning * PI/2) * GlobalSoundVolMul / VolMulTab[VOLDEF] + 0.5f);
}

// update sound volume of channel 0 (1=normal volume)
void VolumeSound(float volume)
{
	VolumeSoundChan(0, volume);
}

// update sound panning (0..1, 0=left, 0.5=middle, 1=right; panning outside range 0..1 will invert signal)
void PanningSoundChan(int chan, float panning)
{
	sPwmSnd* s = &PwmSound[chan];
	s->panning = panning;
	s->volL = (int)(PWMSND_PERIOD * s->volume * cos(panning * PI/2) * GlobalSoundVolMul / VolMulTab[VOLDEF] + 0.5f);
	s->volR = (int)(PWMSND_PERIOD * s->volume * sin(panning * PI/2) * GlobalSoundVolMul / VolMulTab[VOLDEF] + 0.5f);
}

// update sound panning of channel 0 (0..1, 0=left, 0.5=middle, 1=right; panning outside range 0..1 will invert signal)
void PanningSound(float panning)
{
	PanningSoundChan(0, panning);
}

// check if sound is playing
Bool PlayingSoundChan(int chan)
{
	if (GlobalSoundMute) return False;
	if ((uint)chan >= (uint)PWMSND_CHANNUM) return False;
	sPwmSnd* s = &PwmSound[chan];
	return s->cnt > 0;
}

// check if sound of channel 0 is playing
Bool PlayingSound()
{
	return PlayingSoundChan(0);
}

// set next repeated sound in the same format
//  snddata = pointer to sound data
//  size = length of sound in number of bytes
void SetNextSoundChan(int chan, const void* snddata, int size)
{
	sPwmSnd* s = &PwmSound[chan];

	// convert sound size to the length
	int len = SoundByteToLen(size, s->form);

	// check if this sound is already next sound
	if (PlayingSoundChan(chan) && ((const void*)s->next == snddata) && (s->nextcnt == len)) return;

	// disable next sound
	s->nextcnt = 0;
	s->next = NULL;
	dmb();

	// start sound if not playing
	if (s->cnt == 0)
	{
		s->snd = (const u8*)snddata;
		dmb();
		s->cnt = len;
		dmb();
	}

	// set next sound
	s->next = (const u8*)snddata;
	dmb();
	s->nextcnt = len;
}

// set next repeated sound of channel 0 in the same format
//  snddata = pointer to sound data
//  size = length of sound in number of bytes
void SetNextSound(const void* snddata, int size)
{
	SetNextSoundChan(0, snddata, size);
}

// check if streaming buffer is empty
Bool SoundStreamIsEmpty(int chan)
{
	sPwmSnd* s = &PwmSound[chan];
	return s->nextcnt == 0;
}

// global sound set OFF
void GlobalSoundSetOff()
{
	GlobalSoundMute = True;
	PWMSndTerm();
}

// global sound set ON
void GlobalSoundSetOn()
{
	PWMSndTerm();
	GlobalSoundMute = False;
	PWMSndInit();
}

// set new global volume 0..VOLMAX (= 0..15)
void GlobalVolumeSet(int vol)
{
	if (vol < 0) vol = 0;
	if (vol > VOLMAX) vol = VOLMAX;
	GlobalSoundVolume = vol;
	GlobalSoundVolMul = VolMulTab[vol];

	int i;
	sPwmSnd* s = PwmSound;
	for (i = 0; i < PWMSND_CHANNUM; i++)
	{
		s->volL = (int)(PWMSND_PERIOD * s->volume * cos(s->panning * PI/2) * GlobalSoundVolMul / VolMulTab[VOLDEF] + 0.5f);
		s->volR = (int)(PWMSND_PERIOD * s->volume * sin(s->panning * PI/2) * GlobalSoundVolMul / VolMulTab[VOLDEF] + 0.5f);
		s++;
	}
}

#endif // USE_PWMSND
