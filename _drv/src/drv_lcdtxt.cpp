
// ****************************************************************************
//
//                LCD TEXT display driver with I2C interface
//
// ****************************************************************************

#include "../../includes.h"	// includes

#if USE_LCDTXT		// 1=use LCD TEXT I2C display driver, 0=not used (drv_lcdtxt.*)

// initialize (returns False on error)
//  cols ... number of columns (8, 10, 16, 20, 24 or 40; or 0 = auto-detect from the device)
//  rows = number of columns (8, 10, 16, 20, 24 or 40; or 0 = auto-detect from the device)
//  i2c ... I2C bus index (0 or 1)
//  addr ... I2C address (default LCDTXT_ADDR=0x31)
//  speed ... transfer speed in Hz (usually 100000 to 400000)
// After powering on, wait at least 150 ms for the module to initialize.
Bool cLCDTXT::Init(int cols /*= 0*/, int rows /*= 0*/, int i2c /*= LCDTXT_I2C*/, int addr /*= LCDTXT_ADDR*/, int speed /*= LCDTXT_SPEED*/)
{
	int i;

	// base setup
	this->i2c = i2c;
	this->addr = addr;
	this->speed = speed;

	// load device setup
	Bool res = True;
	if ((cols <= 0) || (rows <= 0))
	{
		u8 reg[2];
		u32 t0 = Time();
		res = False;	// time-out error
		while ((u32)(Time() - t0) < 500000) // time-out 500ms
		{
			// read display dimension
			if (I2Cbus_ReadReg16(i2c, addr, LCDTXT_COLS_INX, reg, 2, speed))
			{
				res = True;

				// get number of columns
				if (cols <= 0) cols = reg[0];

				// get number of rows
				if (rows <= 0) rows = reg[1];
				break;
			}
		}

		// use default values on error
		if (cols <= 0) cols = 16;
		if (rows <= 0) rows = 2;
	}

	// check text size
	if ((cols < 8) || (cols > 40))
	{
		cols = 16;
		res = False;
	}

	if ((rows < 1) || (rows > 4))
	{
		rows = 2;
		res = False;
	}

	// check size
	if (cols*rows > LCDTXT_TEXT_MAX)
	{
		cols = 16;
		rows = 2;
		res = False;
	}
	this->cols = cols;
	this->rows = rows;

	// clear text buffer
	this->Clear();

	return res;
}

// clear text buffer and print position
void cLCDTXT::Clear()
{
	memset(this->buf, ' ', LCDTXT_TEXT_MAX);
	this->Home();
}

// clear one row
void cLCDTXT::ClearRow(int row)
{
	if ((uint)row < (uint)this->rows) memset(this->buf + row*this->cols, ' ', this->cols);
	this->HomeRow(row);
}

// home print position 
void cLCDTXT::Home()
{
	this->printx = 0;	// print X position
	this->printy = 0;	// print Y position
}

void cLCDTXT::HomeRow(int row)
{
	this->printx = 0;	// print X position
	this->printy = row;	// print Y position
}

// print character
void cLCDTXT::PrintCh(char ch)
{
	// CR
	if (ch == CH_CR)
		this->printx = 0;

	// LF
	else if (ch == CH_LF)
	{
		this->printx = 0;
		this->printy++;
	}

	// FF
	else if (ch == CH_FF)
	{
		this->Clear();
	}

	// print character
	else
	{
		int x = this->printx;
		int y = this->printy;
		if (	((uint)x < (uint)this->cols) &&
			((uint)y < (uint)this->rows))
		{
			this->buf[x + y*this->cols] = ch;
		}
		this->printx = x+1;
	}
}

// print text width length
void cLCDTXT::PrintTextLen(char* text, int len)
{
	for (; len > 0; len--) this->PrintCh(*text++);
}

// print ASCIIZ text
void cLCDTXT::PrintText(char* text)
{
	this->PrintTextLen(text, StrLen(text));
}

// send print buffer to the display (returns False on error)
Bool cLCDTXT::Update()
{
	return I2Cbus_WriteReg16(this->i2c, this->addr, LCDTXT_TEXT_INX, (const u8*)this->buf, this->cols*this->rows, this->speed);
}

// select contrast 0..LCDTXT_VOTAB_MAX-1 (returns False on error)
//  On some devices, the contrast setting may be locked and
//  can only be changed using the buttons on the display.
//  Only the contrast setting selected using the buttons is
//  stored in the device's flash memory. The contrast setting
//  from the main processor is not saved as the default.
Bool cLCDTXT::SetVO(int vo)
{
	if (vo < 0) vo = 0;
	if (vo >= LCDTXT_VOTAB_MAX) vo = LCDTXT_VOTAB_MAX-1;
	u8 reg = (u8)vo;
	return I2Cbus_WriteReg16(this->i2c, this->addr, LCDTXT_VO_INX, &reg, 1, this->speed);
}
	
// select font LCDTXT_FONT_* (returns False on error)
//  If you want to redefine a user font, first send the font
//  definition, and then send the command to switch fonts.
Bool cLCDTXT::SelFont(int font)
{
	if ((font < 0) || (font >= LCDTXT_FONT_NUM)) font = LCDTXT_FONT_DEF;
	u8 reg = (u8)font;
	return I2Cbus_WriteReg16(this->i2c, this->addr, LCDTXT_FONTSEL_INX, &reg, 1, this->speed);
}

// send user font (returns False on error)
//  buf ... 64-byte buffer with user font (8 characters with 8 lines, 1=pixel is black, only bits 0..4 are used)
//  Buffer this->buf[] can be used for temporary store font definition.
//  If you want to redefine a user font, first send the font
//  definition, and then send the command to switch fonts.
Bool cLCDTXT::UserFont(const u8* buf)
{
	return I2Cbus_WriteReg16(this->i2c, this->addr, LCDTXT_FONT_INX, buf, LCDTXT_FONT_LEN, this->speed);
}

#endif // USE_LCDTXT
