
// ****************************************************************************
//
//                           BarePi - Initialize
//
// ****************************************************************************

#ifndef _BAREPI_DEVINIT_H
#define _BAREPI_DEVINIT_H

#if USE_LCD && USE_LCD320x240		// 1=enable output to LCD SPI display ST7789 320x240 (BarePi module LCD320x240), 2=use core3
extern cLCD LCDDisp;			// LCD display driver
extern volatile int LCDDispTime;	// time delta of LCD frame in [us], to get FPS
extern volatile int LCDDispSend;	// time to send LCD frame in [us]
extern volatile int LCDDispZoom;	// LCD display zoom (0..4, 0=full screen)
INLINE Bool LCDIsOn() { return LCDDisp.IsValid(); }	// check if LCD display is enabled
void LCDRezoom();			// shift LCD display zoom (on key Insert)
#else
INLINE Bool LCDIsOn() { return False; }	// check if LCD display is enabled
INLINE void LCDRezoom() {}		// shift LCD display zoom (on key Insert)
#endif

// Initialize device
void DevInit();

// terminate device
void DevTerm();

#endif // _BAREPI_DEVINIT_H
