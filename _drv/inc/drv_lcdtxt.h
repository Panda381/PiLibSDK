
// ****************************************************************************
//
//                LCD TEXT display driver with I2C interface
//
// ****************************************************************************
// Controller: HD44780
// Standard formats:
//  1x8, 1x10, 1x16, 1x20, 1x24, 1x40
//  2x8, 2x10, 2x16, 2x20, 2x40
//  4x8, 4x16, 4x20
// Maximum is 80 characters (DDRAM range 0x00..0x27, 0x40..67)

#if USE_LCDTXT		// 1=use LCD TEXT I2C display driver, 0=not used (drv_lcdtxt.*)

#ifndef _DRV_LCDTXT_H
#define _DRV_LCDTXT_H

#ifndef LCDTXT_I2C
#define LCDTXT_I2C		0	// I2C bus
#endif

#ifndef LCDTXT_ADDR
#define LCDTXT_ADDR		0x31	// I2C slave address
#endif

#ifndef LCDTXT_SPEED
#define LCDTXT_SPEED		100000	// I2C speed
#endif

#ifndef LCDTXT_COLS
#define LCDTXT_COLS		16	// number of characters per row
#endif

#ifndef LCDTXT_ROWS
#define LCDTXT_ROWS		2	// number of rows
#endif

// display interface
#define LCDTXT_TEXT_LEN		(LCDTXT_COLS*LCDTXT_ROWS) // total text length (2 rows of 16 characters = 32 characters)
#define LCDTXT_TEXT_MAX		80	// max. length of the text
#define LCDTXT_FONT_LEN		64	// number of bytes per custom font (8 characters with height 8 lines)
#define LCDTXT_COLS_INX		12	// index of number of columns
#define LCDTXT_ROWS_INX		13	// index of number of rows
#define LCDTXT_VO_INX		14	// selected contrast index
#define LCDTXT_FONTSEL_INX	15	// selected font index
#define LCDTXT_TEXT_INX		16	// text index (max. text length LCDTXT_TEXT_LEN characters)
#define LCDTXT_FONT_INX		96	// user font index (font size LCDTXT_FONT_LEN=64 bytes)

// font indices
#define LCDTXT_FONT_DEF		0	// default font (remapping characters '\' 0x5C, '~' 0x7E and micro 0x7F)
#define LCDTXT_FONT_USER	1	// user font
#define LCDTXT_FONT_COLL	2	// columns from left
#define LCDTXT_FONT_COLR	3	// columns from right
#define LCDTXT_FONT_LINE	4	// lines
#define LCDTXT_FONT_PIX		5	// pixel graphics

#define LCDTXT_FONT_NUM		6	// number of fonts

// substitute characters on default font LCD_FONT_DEF
// - The characters are mirrored in indexes 0x00–0x07 and 0x08–0x0F.
#define LCDTXT_CHAR_BACKSLASH	1	// backslash '\' (character 0x5C)
#define LCDTXT_CHAR_WAVE	2	// wave ~ (character 0x7E)
#define LCDTXT_CHAR_PI		3	// pi
#define LCDTXT_CHAR_ROOT	4	// root V
#define LCDTXT_CHAR_MICRO	5	// micro (character 0x7F)
#define LCDTXT_CHAR_OMEGA	6	// omega
#define LCDTXT_CHAR_FULL	7	// full
#define LCDTXT_CHAR_SUM		8	// sum

#define LCDTXT_CHAR_NUM		8	// number of custom characters

// VO contrast table (clock frequency 1 MHz)
#define LCDTXT_VOTAB_MAX	11	// max. value of VOTab
#define LCDTXT_VOTAB_DEF	5	// middle contrast

// LCD TEXT device
class cLCDTXT
{
public:
	int	i2c;		// I2C bus index (0 or 1)
	int	addr;		// I2C address (default 0x31)
	int 	speed;		// transfer speed in Hz (usually 100000 to 400000)
	int	cols;		// number of columns (8, 10, 16, 20, 24 or 40)
	int	rows;		// number of rows (1, 2 or 4)
	int	printx;		// print X position
	int	printy;		// print Y position
	char	buf[LCDTXT_TEXT_MAX]; // text buffer

	// initialize (returns False on error)
	//  cols ... number of columns (8, 10, 16, 20, 24 or 40; or 0 = auto-detect from the device)
	//  rows = number of columns (8, 10, 16, 20, 24 or 40; or 0 = auto-detect from the device)
	//  i2c ... I2C bus index (0 or 1)
	//  addr ... I2C address (default LCDTXT_ADDR=0x31)
	//  speed ... transfer speed in Hz (usually 100000 to 400000)
	// After powering on, wait at least 150 ms for the module to initialize.
	Bool Init(int cols = 0, int rows = 0, int i2c = LCDTXT_I2C, int addr = LCDTXT_ADDR, int speed = LCDTXT_SPEED);

	// clear whole text buffer and print position
	void Clear();

	// clear one row
	void ClearRow(int row);

	// home print position 
	void Home();
	void HomeRow(int row);

	// print character
	void PrintCh(char ch);

	// print text width length
	void PrintTextLen(char* text, int len);

	// print ASCIIZ text
	void PrintText(char* text);

	// send print buffer to the display (returns False on error)
	Bool Update();

	// select contrast 0..LCDTXT_VOTAB_MAX-1 (returns False on error)
	//  On some devices, the contrast setting may be locked and
	//  can only be changed using the buttons on the display.
	//  Only the contrast setting selected using the buttons is
	//  stored in the device's flash memory. The contrast setting
	//  from the main processor is not saved as the default.
	Bool SetVO(int vo);
	
	// select font LCDTXT_FONT_* (returns False on error)
	//  If you want to redefine a user font, first send the font
	//  definition, and then send the command to switch fonts.
	Bool SelFont(int font);

	// send user font (returns False on error)
	//  buf ... 64-byte buffer with user font (8 characters with 8 lines, 1=pixel is black, only bits 0..4 are used)
	//  Buffer this->buf[] can be used for temporary store font definition.
	//  If you want to redefine a user font, first send the font
	//  definition, and then send the command to switch fonts.
	Bool UserFont(const u8* buf);
};

#endif // _DRV_LCDTXT_H

#endif // USE_LCDTXT
