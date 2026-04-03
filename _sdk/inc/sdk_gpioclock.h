
// ****************************************************************************
//
//                            GPIO Clock Manager
//
// ****************************************************************************
// General Purpose clocks have MASH noise-shaping dividers. The MASH can be
// programmed for 1, 2 or 3-stage filtering. When using the MASH filter, the
// frequency is spread around the requested frequency and the user must ensure
// that the module is not exposed to frequencies higher than 25MHz. 

// MASH=0: min DIVI=1, freq=clk/DIVI, min_freq=clk/DIVI, max_freq=clk/DIVI
// MASH=1: min DIVI=2, freq=clk/(DIVI+DIVF/4096), min_freq=clk/(DIVI+1), max_freq=clk/DIVI
// MASH=2: min DIVI=3, freq=clk/(DIVI+DIVF/4096), min_freq=clk/(DIVI+2), max_freq=clk/(DIVI-1)
// MASH=3: min DIVI=5, freq=clk/(DIVI+DIVF/4096), min_freq=clk/(DIVI+4), max_freq=clk/(DIVI-3)

// If you obtain the exact frequency you need by integer division, use MASH=0.
// If you need to specify the frequency for audio output using fractional division,
// the output may be affected by interference noise. In this case, the noise can
// be reduced by using MASH=2 instead of MASH=1. With MASH=3, the spectral lines
// may become too sharp.

#ifndef _SDK_GPIOCLOCK_H
#define _SDK_GPIOCLOCK_H

// GPIO clock password
#define GPIOCLOCK_PASSWD	(0x5A << 24)

// GPIO Clock index
#define GPIOCLOCK_CAM0		8
#define GPIOCLOCK_CAM1		9
#define GPIOCLOCK_0		14	// on GPIO4 Alt0, GPIO20 Alt5 (B+ and CM only), GPIO32 Alt0 (CM only), GPIO34 Alt0 (CM only)
#define GPIOCLOCK_1		15	// CLOCK1 is reserved for system use; on GPIO5 Alt0 (B+ and CM only), GPIO21 Alt5 (not B v2), GPIO42 Alt0 (CM only), GPIO44 Alt0 (CM only)
#define GPIOCLOCK_2		16	// on GPIO6 Alt0 (B+ and CM only), GPIO43 Alt0 (CM only)
#define GPIOCLOCK_PCM		19
#define GPIOCLOCK_PWM		20	// PWM
#define GPIOCLOCK_SMI		22

// GPIO Clock source
#define GPIOCLOCK_SRC_OSC	1	// crystal oscillator (Pi 1-3: 19.2 MHz, Pi 4: 54 MHz)
//#define GPIOCLOCK_SRC_PLLC	5	// PLL C variable (1000 MHz, but changes with overclock) .... do not use
#define GPIOCLOCK_SRC_PLLD	6	// PLL D (Pi 1-3: 500 MHz, Pi 4: 750 MHz)
//#define GPIOCLOCK_SRC_HDMI	7	// HDMI (Pi 1-3: 216 MHz, Pi 4: not used) ... do not use

// GPIO Clock MASH
#define GPIOCLOCK_MASH_INT	0	// integer division ... use if integer division gives a sufficiently good frequency
#define GPIOCLOCK_MASH_1	1	// 1-stage MASH (base of using fractional division, small frequency dispersion) ... use for precise frequency
#define GPIOCLOCK_MASH_2	2	// 2-stage MASH (fractional division with better interference noise) ... use for audio output
#define GPIOCLOCK_MASH_3	3	// 3-stage MASH (fractional division with sharp spectrum, large frequency dispersion) ... not recommended

// GPIO Clock Manager
typedef struct {
	io32	CTL;		// 0x00: Control register
				//	bit 0..3: "SRC" clock source GPIOCLOCK_SRC_*
				//	bit 4: "ENAB" request to start or stop clock without glitches - check "BUSY" flag
				//	bit 5: "KILL" 1=stop and reset clock generator (only debug - may cause glitch)
				//	bit 7: "BUSY" (read-only) read 1=clock is running (clock setup must not be changed if busy)
				//	bit 8: "FLIP" invert clock output (only debug - may cause glitch if busy)
				//	bit 9..10: "MASH" mash control GPIOCLOCK_MASH_* (do not change during BUSY and when enabling ENAB)
				//	bit 24..31: "PASSWD" (write-only) password "5A"

	io32	DIV;		// 0x04: divider
				//	bit 0..11: "DIVF" fractional part of divisor 0..4095 (do not change during BUSY)
				//	bit 12..23: "DIVI" integer part of divisor 2..4095 (do not change during BUSY)
				//		This value has a minimum limit determined by the MASH setting.
				//	bit 24..31: "PASSWD" (write-only) password "5A"

} GpioClock_t;
STATIC_ASSERT(sizeof(GpioClock_t) == 0x08, "Incorrect GpioClock_t!");

// get GPIO clock manager
#define GpioClock(inx)	((GpioClock_t*)(uintptr_t)(ARM_CM_BASE + (inx)*8))

// check if GPIO clock is running
//  inx ... GPIO clock index GPIOCLOCK_*
INLINE Bool GpioClockIsRunning(int inx) { return (GpioClock(inx)->CTL & B7) != 0; }

// get frequency in [Hz] of clock source (returns 0 if source is not valid)
//  src ... GPIO clock source GPIOCLOCK_SRC_*
u32 GpioClockSrcFreq(int src);

// Start GPIO clock
//  inx ... GPIO clock index GPIOCLOCK_*
//  src ... GPIO clock source GPIOCLOCK_SRC_*
//  div ... divisor * 4096
//  mash ... noise-shaping divider GPIOCLOCK_MASH_*
// Minimal divisor mash 0: 0x1000, mash 1: 0x2000, mash 2: 0x3000, mash 3: 0x5000
// When using the GPIOCLOCK output on the GPIO pin, it is
// necessary to activate the corresponding alternative function.
void GpioClockStart(int inx, int src, u32 div, int mash);

// Start GPIO clock with frequency in [Hz]
//  inx ... GPIO clock index GPIOCLOCK_*
//  src ... GPIO clock source GPIOCLOCK_SRC_*
//  freq ... required frequency in [Hz]
//  mash ... noise-shaping divider GPIOCLOCK_MASH_*
// When using the GPIOCLOCK output on the GPIO pin, it is
// necessary to activate the corresponding alternative function.
void GpioClockStartHz(int inx, int src, u32 freq, int mash);

// Stop GPIO clock
//  inx ... GPIO clock index GPIOCLOCK_*
void GpioClockStop(int inx);

// Get frequency in [Hz] of GPIO clock (returns 0 if clock is not running or clock source is unknown)
//  inx ... GPIO clock index GPIOCLOCK_*
u32 GpioClockGetHz(int inx);

#endif // _SDK_GPIOCLOCK_H
