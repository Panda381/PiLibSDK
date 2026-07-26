// BarePi interface

#ifndef _PILIBSDK_H
#define _PILIBSDK_H

#define PROGMEM
INLINE u8 pgm_read_byte(const void* addr) { return *(u8*)addr; }

#define BLACK 0
#define WHITE 1

#define LOW 0
#define HIGH 1
typedef u8 uint8_t;
typedef s8 int8;
typedef s8 int8_t;
typedef u16 uint16_t;
typedef s16 int16_t;

INLINE u32 random(void ) { return RandU32(); }

// delay [ms]
INLINE void delay(int ms) { WaitMs(ms); }
INLINE void _delay_ms(int ms) { WaitMs(ms); }

// Frame buffer: 160x120
// Draw buffer: 128x64
//  X = 16, Y = 28

extern u32 Disp_OutCol;		// current output color (old: DispOutCol)
extern u8 Disp_OutPage;		// current output page (0..7)
extern u8 Disp_OutX;		// current output X (0..127)

// DispUpdateSetup(16, 8, 128, 64);
INLINE void DispUpdateSetup(int x, int y, int w, int h) {};

// start simulated I2C communication
INLINE void DispI2C_Start(void) {}

// stop simulated I2C communication
INLINE void DispI2C_Stop(void) {}

// Display select simulated I2C page
void DispI2C_SelectPage(int page);

// write a byte over simulated I2C (write to frame buffer), using Disp_OutCol color
// - After write all data, send image to display with DispUpdate().
void DispI2C_Write(u8 data);

// add byte over simulated I2C (write to frame buffer), write only '1' bits
// - After write all data, send image to display with DispUpdate().
void DispI2C_Add(int x, int y, u8 data, u32 col);

// set byte over simulated I2C (write to frame buffer)
// - After write all data, send image to display with DispUpdate().
void DispI2C_Set(int x, int y, u8 data, u32 col);

// clear byte over simulated I2C (write to frame buffer)
// - After write all data, send image to display with DispUpdate().
void DispI2C_Clr(int x, int y);

// play sound tone
void Sound(uint8_t freq, uint8_t dur);

#endif // PILIBSDK
