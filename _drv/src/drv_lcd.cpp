
// ****************************************************************************
//
//                LCD display driver with SPI1 interface
//
// ****************************************************************************

#include "../../includes.h"	// includes

#if USE_LCD		// 1=use LCD SPI1 display driver, 0=not used (drv_lcd.*)

// ----------------------------------------------------------------------------
//                           Display base service
// ----------------------------------------------------------------------------

// === ST77XX base commands (reading commands not supported in this library)
#define ST77XX_NOP		0x00	// no operation
#define ST77XX_SWRESET		0x01	// software reset (wait 120ms)
#define ST77XX_RDDID		0x04	// read display ID (read: dummy read + 3 bytes)
#define ST77XX_RDDST		0x09	// read display status (read: dummy read + 4 bytes)
#define ST77XX_RDDPM		0x0A	// read display power mode (read: dummy read + 1 byte)
#define ST77XX_RDDMADCTL	0x0B	// read display MADCTL (read: dummy read + 1 byte)
#define ST77XX_RDDCOLMOD	0x0C	// read display pixel format (read: dummy read + 1 byte)
#define ST77XX_RDDIM		0x0D	// read display image mode (read: dummy read + 1 byte)
#define ST77XX_RDDSM		0x0E	// read display signal mode (read: dummy read + 1 byte)
#define ST77XX_RDDSDR		0x0F	// read display self-diagnostic result (read: dummy read + 1 byte) ... not on ST7735
#define ST77XX_SLPIN		0x10	// enable sleep mode - sleep IN (default state after reset; wait 5ms before next command)
#define ST77XX_SLPOUT		0x11	// disable sleep mode - sleep OUT (wait ST7789:5ms/ST7735:120ms before sending next command)
#define ST77XX_PTLON		0x12	// partial mode on
#define ST77XX_NORON		0x13	// partial mode off (normal display mode)
#define ST77XX_INVOFF		0x20	// disable inversion mode (default state after reset)
#define ST77XX_INVON		0x21	// enable inversion mode
#define ST77XX_GAMSET		0x26	// gamma set (write: 1 byte - select Gamma curve 0-15, default 1)
#define ST77XX_DISPOFF		0x28	// disable display (blank screen)
#define ST77XX_DISPON		0x29	// enable display
#define ST77XX_CASET		0x2A	// set start and end column of draw window (write: 4 bytes)
					//	- HIGH byte of X address start XS=0..XE
					//	- LOW byte of X address start
					//	- HIGH byte of X address end XE=XS..WIDTH-1 (239 oor 319)
					//	- LOW byte of X address end
#define ST77XX_RASET		0x2B	// set start and end row of draw window (write: 4 bytes)
					//	- HIGH byte of Y address start YS=0..YE
					//	- LOW byte of Y address start
					//	- HIGH byte of Y address end YE=YS..HEIGHT-1 (319 or 239)
					//	- LOW byte of Y address end
#define ST77XX_RAMWR		0x2C	// start write data to draw window (write: image data)
#define ST77XX_RAMRD		0x2E	// start read data from draw window (read: dummy read + image data)
#define ST77XX_PTLAR		0x30	// partial start/end address set (write: 4 bytes)
					//	- HIGH byte of partial start address 0..P
					//	- LOW byte of partial start address
					//	- HIGH byte of partial end address 0..P
					//	- LOW byte of partial end address
#define ST77XX_VSCRDEF		0x33	// vertical scrolling area definition (write: 6 bytes, top/area/bottom) ... not on ST7735
#define ST77XX_TEOFF		0x34	// tearing effect line off
#define ST77XX_TEON		0x35	// tearing effect line on (write: 1 byte)
#define ST77XX_MADCTL		0x36	// memory data access control (set rotation mode; write: 1 byte, bits ST77XX_MADCTL_*)

	// Bits of register ST77XX_MADCTL
	#define ST77XX_MADCTL_MY	0x80	// B7: page address order (0=top to bottom, 1=bottom to top)
	#define ST77XX_MADCTL_MX	0x40	// B6: column address order (0=left to right, 1=right to left)
	#define ST77XX_MADCTL_MV	0x20	// B5: page/column order (0=normal mode, 1=reverse mode)
	#define ST77XX_MADCTL_ML	0x10	// B4: line address order (LCD refresh 0=top to bottom, 1=bottom to top)
	#define ST77XX_MADCTL_RGB	0x08	// B3: RGB/BGR order (0=RGB, 1=BGR)
	#define ST77XX_MADCTL_MH	0x04	// B2: display data latch data order (LCD refresh 0=left to right, 1=right to left)

#define ST77XX_VSCRSADD		0x37	// vertical scrolling start address (write: 2 bytes) ... not on ST7735
					//	- HIGH vertical scrolling start address
					//	- LOW vertical scrolling start address
#define ST77XX_IDMOFF		0x38	// idle mode off (default after reset)
#define ST77XX_IDMON		0x39	// idle mode on (color expression is reduced to 8-color mode)
#define ST77XX_COLMOD		0x3A	// interface pixel format, set color mode (write: 1 byte, data COLOR_MODE_*)

	// color mode ST77XX_COLMOD (select both color space and data interface)
	#define COLOR_MODE_65K		0x50	// 65K-color space (RGB 5-6-5)
	#define COLOR_MODE_262K		0x60	// 262K-color space (RGB 6-6-6)
	#define COLOR_MODE_12BIT	0x03	// 12-bit data interface, 4K-color space (RGB 4-4-4)
	#define COLOR_MODE_16BIT	0x05	// 16-bit data interface (RGB 5-6-5)
	#define COLOR_MODE_18BIT	0x06	// 18-bit data interface (RGB 6-6-6)
	#define COLOR_MODE_16M		0x07	// 24-bit data interface (RGB 8-8-8)

#define ST77XX_RAMWRC		0x3C	// memory write continue (write: image data) ... not on ST7735
#define ST77XX_RAMRDC		0x3E	// memory read continue (read: dummy read + image data) ... not on ST7735
#define ST77XX_TESCAN		0x44	// set tear scanline (write: 2 bytes) ... not on ST7735
					//	- HIGH tear scanline
					//	- LOW tear scanline
#define ST77XX_RDTESCAN		0x45	// get scanline (read: dummy read + 2 bytes) ... not on ST7735
#define ST77XX_WRDISBV		0x51	// write display brightness (write: 1 byte 0..255) ... not on ST7735
#define ST77XX_RDDISBV		0x52	// read display brightness (read: dummy read + 1 byte) ... not on ST7735
#define ST77XX_WRCTRLD		0x53	// write CTRL display (default 0; write: 1 byte, bits ST77XX_WRCTRLD_*) ... not on ST7735

	// Bits of register ST77XX_WRCTRLD
	#define ST77XX_WRCTRLD_BL	0x04	// B2: 1=backlight control ON
	#define ST77XX_WRCTRLD_DD	0x08	// B3: 1=display dimming is ON
	#define ST77XX_WRCTRLD_BCTRL	0x20	// B5: 1=brightness registers are active

#define ST77XX_RDCTRLD		0x54	// read CTRL display (read: dummy read + 1 byte) ... not on ST7735
#define ST77XX_WRCACE		0x55	// write content adaptive brightness control and color enhancement (default 0; write: 1 byte, bits ST77XX_WRCACE_*) ... not on ST7735

	// Bits of register ST77XX_WRCACE
	#define ST77XX_WRCACE_C0	0x01	// B0: C0+C1: 0=off, 1=user interface, 2=still picture, 3=moving image
	#define ST77XX_WRCACE_C1	0x02	// B1:
	#define ST77XX_WRCACE_CE0	0x10	// B4: CE0+CE1: enhancement 0=low, 1=medium, 3=high
	#define ST77XX_WRCACE_CE1	0x20	// B5:
	#define ST77XX_WRCACE_CECTRL	0x80	// B7: 1=color enhancement ON

#define ST77XX_RDCABC		0x56	// read content adaptive brightness control (read: dummy read + 1 byte) ... not on ST7735
#define ST77XX_WRCABCMB		0x5E	// write CABC minimum brightness (write: 1 byte 0..255) ... not on ST7735
#define ST77XX_RDCABCMB		0x5F	// read CABC minimum brightness (read: dummy read + 1 byte) ... not on ST7735
#define ST77XX_RDABCSDR		0x68	// read automatic brightness control self-dignostic result (read: dummy read + 1 byte) ... not on ST7735

#define ST77XX_RDID1		0xDA	// read ID1 (read: dummy read + 1 byte)
#define ST77XX_RDID2		0xDB	// read ID2 (read: dummy read + 1 byte)
#define ST77XX_RDID3		0xDC	// read ID3 (read: dummy read + 1 byte)

// === ST7735 extended commands
#define ST77XX_FRMCTR1		0xB1	// Frame Rate Control (In normal mode/Full colors; write: 3 bytes)
#define ST77XX_FRMCTR2		0xB2	// Frame Rate Control (In Idle mode/8-colors; write: 3 bytes)
#define ST77XX_FRMCTR3		0xB3	// Frame Rate Control (In Partial mode/full colors; write: 6 bytes)
#define ST77XX_INVCTR		0xB4	// Display Inversion Control (write: 1 byte)
#define ST77XX_DISSET5		0xB6	// Display Function setting (write: 2 bytes)
#define ST77XX_PWCTR1		0xC0	// Power Control 1 (write: 2 bytes)
#define ST77XX_PWCTR2		0xC1	// Power Control 2 (write: 1 byte)
#define ST77XX_PWCTR3		0xC2	// Power Control 3 (in Normal mode/Full colors; write: 4 bytes)
#define ST77XX_PWCTR4		0xC3	// Power Control 4 (in Idle mode/8-colors; write: 4 bytes)
#define ST77XX_PWCTR5		0xC4	// Power Control 5 (in Partial mode/full-colors; write: 2 bytes)
#define ST77XX_VMCTR1		0xC5	// VCOM Control 1 (write: 2 bytes)
#define ST77XX_VMOFCTR		0xC7	// set VCOM offset control (write: 1 byte)
#define ST77XX_WRID2		0xD1	// set LCM version code (write: 1 byte)
#define ST77XX_WRID3		0xD2	// customer Project code (write: 1 byte)
#define ST77XX_NVCTR1		0xD9	// EEPROM control status (write: 1 byte)
#define ST77XX_NVCTR2		0xDE	// EEPROM Read Command (write: 1 byte)
#define ST77XX_NVCTR3		0xDF	// EEPROM Write Command (write: 3 bytes)
#define ST77XX_GAMCTRP1		0xE0	// set Gamma adjustment (+ polarity; write: 16 bytes)
#define ST77XX_GAMCTRN1		0xE1	// set Gamma adjustment (- polarity; write: 16 bytes)
#define ST77XX_EXTCTRL		0xF0	// extension command control (write: 1 byte)
#define ST77XX_PWCTR6		0xFC	// Power Control 6 (In partial mode + Idle; write: 2 bytes)
#define ST77XX_VCOM4L		0xFF	// Vcom 4 level control (write: 3 bytes)

// === ST7789 extended commands (system function command table 2)
// - to access registers, enable it with command ST77XX_CMD2EN
#define ST77XX_RAMCTRL		0xB0	// RAM control (write: 2 bytes, bits ST77XX_RAMCTRL_1_* and ST77XX_RAMCTRL_2_*)

	// Bits of 1st byte of command ST77XX_RAMCTRL (unused bits are 0, default 0x00)
	#define ST77XX_RAMCTRL_1_DM0	0x01	// B0: DM0+DM1: interface 0=MCU, 1=RGB, 2=VSYNC
	#define ST77XX_RAMCTRL_1_DM1	0x02	// B1:
	#define ST77XX_RAMCTRL_1_RM	0x10	// B4: RAM access from 0=MCU, 1=RGB

	// Bits of 2nd byte of command ST77XX_RAMCTRL (bits 6 and 7 must be set to 1, default 0xF0)
	#define ST77XX_RAMCTRL_2_MDT0	0x01	// B0: MDT0+MDT1: method of pixel data transfer (see section 8.8.11)
	#define ST77XX_RAMCTRL_2_MDT1	0x02	// B1:
	#define ST77XX_RAMCTRL_2_RIM	0x04	// B2: RGB interface bus width 0=18 bits, 1=6 bits
	#define ST77XX_RAMCTRL_2_ENDIAN	0x08	// B3: 0=MSB first Big Endian (default) - Motorola, 1=LSB first Little Endian - Intel (only in 65K 8/9-bit mode)
	#define ST77XX_RAMCTRL_2_EPF0	0x10	// B4: EPF0+EPF1: align 65K data with bit 0: 0=equ 0, 1=equ 1, 2=equ high bit, 3=equ green bit 0
	#define ST77XX_RAMCTRL_2_EPF1	0x20	// B5:

#define ST77XX_RGBCTRL		0xB1	// RGB interface control (write: 3 bytes, signal polarity etc.)
#define ST77XX_PORCTRL		0xB2	// Porch setting (write: 5 bytes)
#define ST77XX_FRCTRL1		0xB3	// Frame rate control 1 (write: 3 bytes)
#define ST77XX_PARCTRL		0xB5	// Partial control (write: 1 byte)
#define ST77XX_GCTRL		0xB7	// Gate control (write: 1 byte)
#define ST77XX_GTADJ		0xB8	// Gate on timing adjustment (write: 4 bytes)
#define ST77XX_DGMEN		0xBA	// Digital gamma enable (write: 1 byte, digital gamma 0x00=disable, 0x04=enable)
#define ST77XX_VCOMS		0xBB	// VCOM setting (write: 1 byte, 0..64)
#define ST77XX_POWSAVE		0xBC	// Power saving mode (write: 1 byte)
#define ST77XX_DLPOFFSAVE	0xBD	// Display off power save (write: 1 byte)
#define ST77XX_LCMCTRL		0xC0	// LCM control (write: 1 byte, bits ST77XX_LCMCTRL_*)

	// Bits of register ST77XX_LCMCTRL_* (bit 7 = 0; default value 0x2C)
	#define ST77XX_LCMCTRL_XGS	0x01	// B0: 1=XOR GS setting in command 0xE4
	#define ST77XX_LCMCTRL_XMV	0x02	// B1: 1=XOR MV setting in command 0x36
	#define ST77XX_LCMCTRL_XMH	0x04	// B2: 1=reverse source output order for RGB interface
	#define ST77XX_LCMCTRL_XMX	0x08	// B3: 1=XOR MX setting in command 0x36
	#define ST77XX_LCMCTRL_XINV	0x10	// B4: 1=XOR inverse setting in command 0x21
	#define ST77XX_LCMCTRL_XBGR	0x20	// B5: 1=XOR RGB setting in command 0x36
	#define ST77XX_LCMCTRL_XMY	0x40	// B6: XOR MY setting in command Ox36

#define ST77XX_IDSET		0xC1	// ID code setting (write: 3 bytes)
#define ST77XX_VDVVRHEN		0xC2	// VDV and VRH command enable (write: 2 bytes, 0x01/0xFF default)
#define ST77XX_VRHS		0xC3	// VRH set (write: 1 byte, 0x0B default)
#define ST77XX_VDVS		0xC4	// VDV set (write: 1 byte, 0x20 default)
#define ST77XX_VCMOFSET		0xC5	// VCOM offset set (write: 1 byte, 0x20 default)
#define ST77XX_FRCTRL2		0xC6	// Frame rate control in normal mode (write: 1 byte, 0x0F default)
#define ST77XX_CABCCTRL		0xC7	// CABC control (write: 1 byte, 0x00 default)
#define ST77XX_REGSEL1		0xC8	// Register value selection 1 (write: 1 byte, value 0x08)
#define ST77XX_REGSEL2		0xCA	// Register value selection 2 (write: 1 byte, value 0x0F)
#define ST77XX_PWMFRSEL		0xCC	// PWM frequency selection (write: 1 byte, 0x02 default)
#define ST77XX_PWCTRL1		0xD0	// Power control 1 (write: 2 bytes, default 0xA4/0xA1)
#define ST77XX_VAPVANEN		0xD2	// Enable VAP/VAN signal output (write: 1 byte, 0x4C)
#define ST77XX_CMD2EN		0xDF	// Command 2 enable (write: 4 bytes, default disabled)
					//   write 0x5A 0x69 0x02 0x01 to enable Commands in Command table 2 when EXTC is LOW
#define ST77XX_PVGAMCTRL	0xE0	// Positive voltage gamma control (write: 14 bytes)
#define ST77XX_NVGAMCTRL	0xE1	// Negative voltage gamma control (write: 14 bytes)
#define ST77XX_DGMLUTR		0xE2	// Digital gamma look-up table for red (write: 64 bytes)
#define ST77XX_DGMLUTB		0xE3	// Digital gamma look-up table for blue (write: 64 bytes)
#define ST77XX_GATECTRL		0xE4	// Gate control (write: 3 bytes)
#define ST77XX_SPI2EN		0xE7	// SPI2 enable (write: 1 byte)
#define ST77XX_PWCTRL2		0xE8	// Power control 2 (write: 1 byte)
#define ST77XX_EQCTRL		0xE9	// Equaliza time control (write: 3 bytes)
#define ST77XX_PROMCTRL		0xEC	// Program control (write: 1 byte)
#define ST77XX_PROMEN		0xFA	// Program mode enable (write: 4 bytes)
#define ST77XX_NVMSET		0xFC	// NVM setting (write: 2 bytes)
#define ST77XX_PROMACT		0xFE	// Program action (write: 2 bytes)

// GPIO functions
#define LCD_CS_ON	{ GPIO_Out0(LCD_CS_GPIO); cb(); }	// activate chip selection
#define LCD_CS_OFF	{ cb(); GPIO_Out1(LCD_CS_GPIO); }	// deactivate chip selection

#if LCD_RES_GPIO >= 0 // if RES signal is used
#define LCD_RES_ON	{ GPIO_Out0(LCD_RES_GPIO); cb(); }	// activate reset (must be > 9us)
#define LCD_RES_OFF	{ cb(); GPIO_Out1(LCD_RES_GPIO); }	// deactivate reset
#endif

#define LCD_DC_CMD	{ cb(); GPIO_Out0(LCD_DC_GPIO); cb(); }	// set command mode
#define LCD_DC_DATA	{ cb(); GPIO_Out1(LCD_DC_GPIO); cb(); }	// set data mode

#if USE_LCD320x240==2		// 1=enable output to LCD SPI display ST7789 320x240, 2=use core3
u32 LCD_FrameBuf[320*240/2];	// LCD temporary frame buffer
volatile Bool LCD_FrameBufDirty = False; // flag - update frame buffer from core3
#endif

// wait for the transmission to complet
void cLCD::WaitBusy()
{
	while (!SPI1_TxIsEmpty() || SPI1_IsBusy()) {}
	WaitUs(10); // wait for last shift
}

// display connect (activate chip selection)
void cLCD::Connect()
{
	LCD_CS_ON;
	WaitUs(5);
}

// display disconnect (deactivate chip selection)
void cLCD::Disconnect()
{
	this->WaitBusy();	// wait for the transmission to complet
	LCD_CS_OFF;
}

// write command byte
void cLCD::WriteCmd(u8 cmd)
{
	this->WaitBusy();	// wait for the transmission to complet
	LCD_DC_CMD;		// set command mode
	this->Write8(cmd);	// send byte
	this->WaitBusy();	// wait for the transmission to complet
	WaitUs(10);		// wait command to process
}

// write data byte
void cLCD::WriteData(u8 data)
{
	LCD_DC_DATA;		// set data mode
	this->Write8(data);	// send byte
}

// set display rotation LCD_ROT_*
void cLCD::SetRot(int rot)
{
	// prepare roration
	rot &= 0x03;
	this->rot = rot;
	int k;

	// prepare coordinated
	switch (rot)
	{
	// case LCD_ROT_0:
	default:
		k = 0;
		this->xstart = this->offx;
		this->ystart = this->offy;
		this->w = this->dispw;
		this->h = this->disph;
		break;	

	case LCD_ROT_90:
		k = ST77XX_MADCTL_MY + ST77XX_MADCTL_MV,
		this->xstart = this->maxh - this->disph - this->offy;
		this->ystart = this->offx;
		this->w = this->disph;
		this->h = this->dispw;
		break;	

	case LCD_ROT_180:
		k = ST77XX_MADCTL_MX + ST77XX_MADCTL_MY,
		this->xstart = this->maxw - this->dispw - this->offx;
		this->ystart = this->maxh - this->disph - this->offy;
		this->w = this->dispw;
		this->h = this->disph;
		break;	

	case LCD_ROT_270:
		k = ST77XX_MADCTL_MX + ST77XX_MADCTL_MV;
		this->xstart = this->offy;
		this->ystart = this->maxw - this->dispw - this->offx;
		this->w = this->disph;
		this->h = this->dispw;
		break;	
	}

	// width in bytes
	this->wb = this->w*4;

	// send command
	this->Connect();
	this->WriteCmd(ST77XX_MADCTL);
	if (model == LCD_MODEL_ST7789) k |= ST77XX_MADCTL_RGB;
	this->WriteData((u8)k);
	this->Disconnect();
}

// setup drawing window (and connect display; image data should follow)
void cLCD::SetWindow(int x, int y, int w, int h)
{
	// connect display
	this->Connect();

	// prepare coordinates
	int x1 = x + this->xstart;
	int x2 = x1 + w - 1;
	int y1 = y + this->ystart;
	int y2 = y1 + h - 1;

	// setup X coordinates
	this->WriteCmd(ST77XX_CASET);
	this->WriteData((u8)(x1 >> 8));
	this->WriteData((u8)x1);
	this->WriteData((u8)(x2 >> 8));
	this->WriteData((u8)x2);

	// setup Y coordinates
	this->WriteCmd(ST77XX_RASET);
	this->WriteData((u8)(y1 >> 8));
	this->WriteData((u8)y1);
	this->WriteData((u8)(y2 >> 8));
	this->WriteData((u8)y2);

	// command to start sending data
	this->WriteCmd(ST77XX_RAMWR);

	// set data mode
	LCD_DC_DATA;		// set data mode
}

// set backlight control (range 0..10)
void cLCD::Backlight(int bl)
{
	// limit range to 0..10
	if (bl < LCD_BL_MIN) bl = LCD_BL_MIN;
	if (bl > LCD_BL_MAX) bl = LCD_BL_MAX;
	this->bl = bl;

#if LCD_BL_GPIO >= 0 // if BL signal is used
#if LCD_BL_GPIO == 19 // use PCM_FS

	// full brightness
	if (bl == LCD_BL_MAX)
	{
		GPIO_Out1(LCD_BL_GPIO);
		GPIO_Func(LCD_BL_GPIO, GPIO_FUNC_OUT);
	}

	// display off
	else if (bl == LCD_BL_MIN)
	{
		GPIO_Out0(LCD_BL_GPIO);
		GPIO_Func(LCD_BL_GPIO, GPIO_FUNC_OUT);
	}

	// setup PCM
	else
	{
		// select PCM_FS
		GPIO_Func(LCD_BL_GPIO, GPIO_FUNC_AF0);	// PCM_FS

		// stop PCM
		PCM->CS = 0;			// reset PCM
		WaitUs(200);

		// prepare PCM registers (PWM cycle 512, PWM frequency 9600000/512=18750 Hz)
		int flen = 512;
		int fslen = flen >> (LCD_BL_MAX - bl); // bl=1..9, shifts=9..1, fslen=1..256

		// setup mode register
		PCM->MODE = ((flen-1) << 10) | fslen; // master mode FSM=0

		// disable data channels
		PCM->TXC = 0;			// transmit is disabled
		PCM->RXC = 0;			// receive is disabled

		// start PCM
		PCM->CS = B0;			// enable PCM, clear STBY
		WaitUs(50);

		// start TX
		PCM->CS = B0 | B2;		// start transmission
	}

#else // LCD_BL_GPIO == 19 ... do not use PCM_FS - only enable or disable backlight
	if (bl == LCD_BL_MIN)
		GPIO_Out0(LCD_BL_GPIO);	// backlight off
	else
		GPIO_Out1(LCD_BL_GPIO); // backlight on
#endif // LCD_BL_GPIO == 19
#endif // LCD_BL_GPIO >= 0
}

// check connection between two pins (returns False on error)
Bool AutoInitCheck(int snd, int rcv)
{
	// setup receiver to pull-down
	GPIO_Func(rcv, GPIO_FUNC_IN);
	GPIO_Pull(rcv, GPIO_PULL_DOWN);

	// setup sender to HIGH output
	GPIO_Out1(snd);
	GPIO_Func(snd, GPIO_FUNC_OUT);

	// wait settle time
	WaitUs(100);

	// get input
	cb();
	Bool res = (GPIO_In(rcv) == 1);
	cb();

	// setup receiver to pull-up
	GPIO_Pull(rcv, GPIO_PULL_UP);

	// setup sender to LOW output
	GPIO_Out0(snd);

	// wait settle time
	WaitUs(100);

	// get input
	cb();
	res &= (GPIO_In(rcv) == 0);
	cb();

	// restore default state
	GPIO_Pull(rcv, GPIO_PULL_OFF);
	GPIO_Func(snd, GPIO_FUNC_IN);
	return res;
}

// Detect display module (returns LCD_MODEL_*, or LCD_MODEL_NONE on error)
// Supported module and their detection:
// - Display module ST7789 320x240, rotation LCD_ROT_270: resistor 10K between signals DC and CS
// - Display module ST7735 160x80, rotation LCD_ROT_270: resistor 10K between signals RES and CS
// After detection, the display must be fully initialized, because modifying the pin states may have caused the display to reset.
int cLCD::Detect()
{
	// check ST7789 - resistor 10K between signals DC and CS
	Bool res = AutoInitCheck(LCD_DC_GPIO, LCD_CS_GPIO);
	res &= AutoInitCheck(LCD_CS_GPIO, LCD_DC_GPIO);
	if (res) return LCD_MODEL_ST7789;

#if LCD_RES_GPIO >= 0 // if RES signal is used
	// check ST7735 - resistor 10K between signals RES and CS
	res = AutoInitCheck(LCD_RES_GPIO, LCD_CS_GPIO);
	res &= AutoInitCheck(LCD_CS_GPIO, LCD_RES_GPIO);
	if (res) return LCD_MODEL_ST7735;
#endif

	// display not detected
	return LCD_MODEL_NONE;
}

// Auto-initialize display module (returns False on error)
// Supported module and their detection:
// - Display module ST7789 320x240, rotation LCD_ROT_270: resistor 10K between signals DC and CS
// - Display module ST7735 160x80, rotation LCD_ROT_270: resistor 10K between signals RES and CS
// Initialization is performed with framebuf=NULL. The image can be output from the
// main frame buffer using the UpdateMain() function. To use a custom frame buffer,
// set its address in the variable this->framebuf.
// To check later whether the initialization was successful, check whether
// the "model" variable contains the value "LCD_MODEL_NONE".
Bool cLCD::AutoInit()
{
	// detect display module
	int model = this->Detect();

	// ST7789 320x240
	if (model == LCD_MODEL_ST7789)
	{
		this->Init(LCD_MODEL_ST7789, 240, 320, 0, 0, NULL, LCD_ROT_270, LCD_DEF_SPEED);
		return True;
	}

	// ST7735 160x80
	if (model == LCD_MODEL_ST7735)
	{
		this->Init(LCD_MODEL_ST7735, 80, 160, 26, 1, NULL, LCD_ROT_270, LCD_DEF_SPEED);
		return True;
	}

	// display not detected
	this->model = LCD_MODEL_NONE;
	return False;
}

// initialize
//  model ... display model LCD_MODEL_*
//  dispw ... display physical width in pixels (columns) - viewport width
//  disph ... display physical height in pixels (rows) - viewport height
//  offx ... display physical start offset X (columns) - viewport X
//  offy ... display physical start offset Y (rows) - viewport Y
//  framebuf ... pointer to frame buffer with format 32-bits per pixel, size dispw*disph*4 bytes (can be shared with main display frame buffer)
//  rot ... image rotation LCD_ROT_*
//  speed ... transfer speed in Hz (30517..125000000 Hz)
// Frame buffer can be shared with the main screen (uses the same graphics format).
// The frame buffer is not accessed during initialization, it can be set up later.
// The display will remain OFF until the first Update().
// The physical dimensions and offset apply to the default rotation of 0.
// The framebuf can be NULL if you are updating using the UpdateMain() function from the main frame buffer.
void cLCD::Init(int model, int dispw, int disph, int offx, int offy, u32* framebuf, int rot /* = LCD_ROT_0 */, int speed /* = LCD_DEF_SPEED */)
{
	// base parameters
	this->model = model;
	this->dispw = dispw;
	this->disph = disph;
	this->offx = offx;
	this->offy = offy;
	this->framebuf = framebuf;
	this->rot = rot;
	this->speed = speed;
	this->on = False;
	this->bl = LCD_BL_DEF;

	// max. size
	if (model == LCD_MODEL_ST7735)
	{
		// ST7735
		this->maxw = 132;
		this->maxh = 162;
	}
	else
	{
		// ST7789
		this->maxw = 240;
		this->maxh = 320;
	}

	// font
	this->font = FontBold8x16; // draw font (cells 8 pixels, 128 characters)
	this->fontw = 8;	// font width (5 to 8 pixels)
	this->fonth = 16;	// font height
	this->printinv = False;

	// initialize GPIO pins
	GPIO_Out1(LCD_DC_GPIO);
	GPIO_Func(LCD_DC_GPIO, GPIO_FUNC_OUT);
#if LCD_RES_GPIO >= 0 // if RES signal is used
	GPIO_Out1(LCD_RES_GPIO);
	GPIO_Func(LCD_RES_GPIO, GPIO_FUNC_OUT);
#endif
	GPIO_Out1(LCD_CS_GPIO);
	GPIO_Func(LCD_CS_GPIO, GPIO_FUNC_OUT);
#if LCD_BL_GPIO >= 0 // if BL signal is used
#if LCD_BL_GPIO	== 19 // use PCM_FS
	GPIO_Func(LCD_BL_GPIO, GPIO_FUNC_AF0);	// PCM_FS
#else
	GPIO_Out1(LCD_BL_GPIO);
	GPIO_Func(LCD_BL_GPIO, GPIO_FUNC_OUT);
#endif
#endif
	GPIO_Func(LCD_MOSI_GPIO, GPIO_FUNC_AF4);
	GPIO_Func(LCD_SCK_GPIO, GPIO_FUNC_AF4);

	// inicialize PCM PWM
#if LCD_BL_GPIO	== 19 // use PCM_FS
	PCM->CS = 0;			// reset PCM
	PCM_ClockStart(9600000);	// use 9.6 MHz PCM clock, result frequency will be:
					//  19.2MHz/2=9600000 Hz, PWM cycle (/512) = 18.75 kHz
	this->Backlight(this->bl);	// setup backlight level
#endif

	// enable SPI1
	SPI1_Enable();

	// clear FIFO
	AUX->spi1.CTRL1 = 0;	// disable interrupts
	AUX->spi1.CTRL0 = B9;	// reset FIFO,
	WaitMs(1);
	if (model == LCD_MODEL_ST7789)
		AUX->spi1.CTRL0 = 8|B6|B7|B8|B17|B18|B19; // 8 data bits, start MSB first, reset FIFO, no hold time
	else
		AUX->spi1.CTRL0 = 8|B6|B17|B18|B19; // 8 data bits, start MSB first, reset FIFO, no hold time

	// set transfer speed in Hz
	SPI1_SetSpeed(speed);

	// enable FIFO
	SPI1_FlushDisable();

	// soft enable SPI1
	SPI1_SoftEnable();

	// display connect (activate chip selection)
	this->Connect();

	// hard reset LCD controller
#if LCD_RES_GPIO >= 0 // if RES signal is used
	LCD_RES_ON;	// activate reset
	WaitMs(1);	// reset delay (must be > 9us)
	LCD_RES_OFF;	// deactivate reset
	WaitMs(5);	// wait for initialization (required min. 5 ms)
#endif

	// soft reset LCD controller
	this->WriteCmd(ST77XX_SWRESET);
	WaitMs(120);	// wait for initialization (required min. 120 ms)

	// disable sleep mode
	this->WriteCmd(ST77XX_SLPOUT);
	WaitMs(120);	// wait for initialization (required min. 120ms)

	// set color mode
	this->WriteCmd(ST77XX_COLMOD);
	this->WriteData(COLOR_MODE_65K | COLOR_MODE_16BIT); // set color mode to RGB 16-bit 565
	WaitMs(1);

	// set display rotation
	this->Disconnect();
	this->SetRot(rot);
	this->Connect();

	// enable inversion
	this->WriteCmd(ST77XX_INVON);
	WaitMs(1);

	if (model == LCD_MODEL_ST7735)
	{
		this->WriteCmd(ST77XX_INVCTR);
		this->WriteData(0);
	}

	// disable partial mode
	this->WriteCmd(ST77XX_NORON);
	WaitMs(1);

	// disable idle mode
	this->WriteCmd(ST77XX_IDMOFF);

	// select gamma correction
	if (model == LCD_MODEL_ST7735)
	{
		this->WriteCmd(ST77XX_GAMSET);
		this->WriteData(2);
	}
	WaitMs(1);
	this->Disconnect();
}

// re-initialize to another viewport
//  dispw ... display physical width in pixels (columns) - viewport width
//  disph ... display physical height in pixels (rows) - viewport height
//  offx ... display physical start offset X (columns) - viewport X
//  offy ... display physical start offset Y (rows) - viewport Y
// If you need to change the frame buffer address, set the framebuf variable.
// The physical dimensions and offset apply to the default rotation of 0.
void cLCD::ReInit(int dispw, int disph, int offx, int offy)
{
	this->dispw = dispw;
	this->disph = disph;
	this->offx = offx;
	this->offy = offy;
	this->SetRot(this->rot);
}

// terminate
void cLCD::Term()
{
	this->model = LCD_MODEL_NONE;

	SPI1_Term();

	// terminate GPIO pins
	GPIO_Func(LCD_DC_GPIO, GPIO_FUNC_IN);
#if LCD_RES_GPIO >= 0 // if RES signal is used
	GPIO_Func(LCD_RES_GPIO, GPIO_FUNC_IN);
#endif
	GPIO_Func(LCD_CS_GPIO, GPIO_FUNC_IN);
#if LCD_BL_GPIO >= 0 // if BL signal is used
#if LCD_BL_GPIO	== 19 // use PCM_FS
	PCM->CS = 0; // stop PCM
#endif
	GPIO_Func(LCD_BL_GPIO, GPIO_FUNC_IN);
#endif
	GPIO_Func(LCD_MOSI_GPIO, GPIO_FUNC_IN);
	GPIO_Func(LCD_SCK_GPIO, GPIO_FUNC_IN);
}

// convert 32-bit color to 16-bit color
INLINE u32 Col32To16(u32 d)
{
	return ((d & 0xf8) >> 3) | ((d & 0xfc00) >> (8+2-5)) | ((d & 0xf80000) >> (16+3-5-6));
}

// clear display without using frame buffer and without enable display
void cLCD::UpdateClearCol(u32 col)
{
	u16 b = Col32To16(col);
	u8 bH = (u8)(b >> 8);
	u8 bL = (u8)b;

	// set drawing window
	this->SetWindow(0, 0, this->w, this->h);

	// send data	
	int i;
	for (i = this->w*this->h; i > 0; i--)
	{
		this->WriteData(bH);
		this->WriteData(bL);
	}

	// disconnect display
	this->Disconnect();
}

// display update
void cLCD::Update()
{
	int i;
	u32 b;

	// set drawing window
	this->SetWindow(0, 0, this->w, this->h);

	// send data	
	u32* s = this->framebuf;
	i = this->w*this->h;
	if ((i & 1) == 0) // size is aligned to 32-bits
	{
		this->SetLen32();	// set 32-bit transfer
		i >>= 1;
		for (; i > 0; i--)
		{
			b = Col32To16(s[0]) << 16;
			b |= Col32To16(s[1]);
			s += 2;
			this->Write32(b);
		}
	}
	else
	{
		this->SetLen16();	// set 16-bit transfer
		for (; i > 0; i--)
		{
			b = Col32To16(*s++);
			this->Write16(b);
		}
	}
	this->WaitBusy();	// wait for the transmission to complet
	this->SetLen8();	// set 8-bit transfer

	// flush received bytes
	while (!SPI1_RxIsEmpty()) (void)SPI1_Read();

	// enable display
	if (!this->on)
	{
		this->on = True;
		this->WriteCmd(ST77XX_DISPON);
	}

	// disconnect display
	this->Disconnect();
}

#if USE_LCD320x240==2		// 1=enable output to LCD SPI display ST7789 320x240, 2=use core3
// display udpate from core3 (returns time delta in [us])
int FASTCODE cLCD::UpdateCore(int core)
{
	// wait for update requrest
	dsb();
	while (!LCD_FrameBufDirty)
	{
		wfe();	// wait signal
		dsb();
		if (CoreStopReq(core)) return 0;
	}
	LCD_FrameBufDirty = False;
	dsb();

	// destination size of LCD display
	int wd = this->w;
	int hd = this->h;
	u32 t1 = Time();

	// set drawing window
	this->SetWindow(0, 0, wd, hd);

	// set 32-bit transfer
	this->SetLen32();

	// send data
	int i = wd*hd/2/4;
	const u32* s = LCD_FrameBuf;
	int j;
	for (; i > 0; i--)
	{
		// delay 2us - to minimize access to the peripheral bus, which would slow down core 0
		for (j = 1000; j > 0; j--) nop();

		// wait transmit FIFO to be empty
		while (!SPI1_TxIsEmpty()) {}

		// send 4 samples (batch processing puts less load on the bus)
		SPI1_Write(s[0]);
		SPI1_Write(s[1]);
		SPI1_Write(s[2]);
		SPI1_Write(s[3]);
		s += 4;
	}

	// end
	this->WaitBusy();	// wait for the transmission to complet
	this->SetLen8();	// set 8-bit transfer

	// flush received bytes
	while (!SPI1_RxIsEmpty()) (void)SPI1_Read();

	// disconnect display
	this->Disconnect();

	return Time() - t1;
}
#endif

// display update from main frame buffer - only if driver is valid (does not use its own frame buffer)
void cLCD::UpdateMain()
{
	int i, j, xs, ys, xd, yd, ws, hs, wd, hd, wbs;
	const u32 *s, *s2, *s0;
	u32 c1, c2, c3, c4, a, b;
	int d;
	Bool scale;

	// check if driver is valid
	if (!this->IsValid()) return;

	// clear display if not yet enabled - to clear unused parts of the display
	if (!this->on)
	{
		this->UpdateClearCol(COL_BLACK);

#if USE_LCD320x240==2		// 1=enable output to LCD SPI display ST7789 320x240, 2=use core3
		// enable display
		this->Connect();
		this->on = True;
		this->WriteCmd(ST77XX_DISPON);
		this->Disconnect();
#endif
	}

	// destination size of LCD display
	wd = this->w;
	hd = this->h;

	// prepare source buffer
	sFrameBuffer* f = &FrameBuffer;
	s0 = f->drawbuf;	// drawing bufer
	ws = f->drawwidth;	// width of drawing buffer
	hs = f->drawheight;	// height of drawing buffer
	wbs = f->drawpitchpix; // line width in pixels

#if USE_LCD320x240==2		// 1=enable output to LCD SPI display ST7789 320x240, 2=use core3
	u32* dst = LCD_FrameBuf;
	u16* dst16 = (u16*)dst;
#endif

	xs = 0;
	ys = 0;

	// zoom
	if ((this->zoom > 0) && ((ws > wd) || (hs > hd)) && ((ws/2 <= wd) && (hs/2 <= hd)))
	{
		if ((this->zoom == 2) || (this->zoom == 4)) xs = ws - ws/2;
		if ((this->zoom == 3) || (this->zoom == 4)) ys = hs - hs/2;
		s0 += xs + ys*wbs;
		ws /= 2;
		hs /= 2;
	}

	// scale if source FrameBuf > destination Disp
	if ((ws > wd) || (hs > hd))
	{
		// new source size
		ws /= 2;
		hs /= 2;

		// scaled source is greater than destination - limit coordinates
		if (ws > wd)
		{
			xs = ws - wd; // (ws*2 - wd*2)/2
			ws = wd;
		}
		if (hs > hd)
		{
			ys = hs - hd; // (hs*2 - hd*2)/2
			hs = hd;
		}

		// source buffer
		s0 = s0 + xs + ys*wbs;

		// set drawing window
		xd = (wd - ws)/2;
		yd = (hd - hs)/2;
#if USE_LCD320x240!=2		// 1=enable output to LCD SPI display ST7789 320x240, 2=use core3
		this->SetWindow(xd, yd, ws, hs);

		if ((ws & 1) == 0) // width is aligned to 32-bits
			this->SetLen32();	// set 32-bit transfer
		else
			this->SetLen16();	// set 16-bit transfer
#endif

		// send data
		for (i = hs; i > 0; i--)
		{
			s = s0;
			s2 = s0 + wbs;
			j = ws;
			if ((j & 1) == 0) // width is aligned to 32-bits
			{
				j >>= 1;
				for (; j > 0; j--)
				{
					c1 = s[0];
					c2 = s[1];
					c3 = s2[0];
					c4 = s2[1];
					a = ((c1 & 0xfc00fc) + (c2 & 0xfc00fc) + (c3 & 0xfc00fc) + (c4 & 0xfc00fc)) >> 2;
					b = ((c1 & 0xfc00) + (c2 & 0xfc00) + (c3 & 0xfc00) + (c4 & 0xfc00)) >> 2;

					d = Col32To16(a|b) << 16;

					c1 = s[2];
					c2 = s[3];
					c3 = s2[2];
					c4 = s2[3];
					a = ((c1 & 0xfc00fc) + (c2 & 0xfc00fc) + (c3 & 0xfc00fc) + (c4 & 0xfc00fc)) >> 2;
					b = ((c1 & 0xfc00) + (c2 & 0xfc00) + (c3 & 0xfc00) + (c4 & 0xfc00)) >> 2;

					s += 4;
					s2 += 4;

					d |= Col32To16(a|b);

#if USE_LCD320x240==2		// 1=enable output to LCD SPI display ST7789 320x240, 2=use core3
					*dst++ = d;
#else
					this->Write32(d);
#endif
				}
			}
			else
			{
				for (; j > 0; j--)
				{
					c1 = s[0];
					c2 = s[1];
					c3 = s2[0];
					c4 = s2[1];
					a = ((c1 & 0xfc00fc) + (c2 & 0xfc00fc) + (c3 & 0xfc00fc) + (c4 & 0xfc00fc)) >> 2;
					b = ((c1 & 0xfc00) + (c2 & 0xfc00) + (c3 & 0xfc00) + (c4 & 0xfc00)) >> 2;

					s += 2;
					s2 += 2;

					d = Col32To16(a|b);

#if USE_LCD320x240==2		// 1=enable output to LCD SPI display ST7789 320x240, 2=use core3
					*dst16++ = (u16)d;
#else
					this->Write16(d);
#endif
				}
			}
			s0 += 2*wbs;
		}
	}

	// destination Disp = 2*source FrameBuf
	else if ((2*ws == wd) && (2*hs == hd))
	{
		xd = 0;
		yd = 0;
		
		// set drawing window
#if USE_LCD320x240!=2		// 1=enable output to LCD SPI display ST7789 320x240, 2=use core3
		this->SetWindow(xd, yd, wd, hd);
		this->SetLen32();	// set 32-bit transfer
#endif

		// send data
		for (i = hd; i > 0; i--)
		{
			s = s0;
			for (j = ws; j > 0; j--)
			{
				d = Col32To16(s[0]);
				d |= d << 16;
				s++;

#if USE_LCD320x240==2		// 1=enable output to LCD SPI display ST7789 320x240, 2=use core3
				*dst++ = d;
#else
				this->Write32(d);
#endif
			}
			if ((i & 1) != 0) s0 += wbs;
		}
	}

	// source is less or equal to destination
	else
	{
		xd = (wd - ws)/2;
		yd = (hd - hs)/2;
		
		// set drawing window
#if USE_LCD320x240!=2		// 1=enable output to LCD SPI display ST7789 320x240, 2=use core3
		this->SetWindow(xd, yd, ws, hs);

		if ((ws & 1) == 0) // width is aligned to 32-bits
			this->SetLen32();	// set 32-bit transfer
		else
			this->SetLen16();	// set 16-bit transfer
#endif

		// send data
		for (i = hs; i > 0; i--)
		{
			s = s0;
			j = ws;
			if ((j & 1) == 0) // width is aligned to 32-bits
			{
				j >>= 1;
				for (; j > 0; j--)
				{
					d = Col32To16(s[0]) << 16;
					d |= Col32To16(s[1]);
					s += 2;

#if USE_LCD320x240==2		// 1=enable output to LCD SPI display ST7789 320x240, 2=use core3
					*dst++ = d;
#else
					this->Write32(d);
#endif
				}
			}
			else
			{
				for (; j > 0; j--)
				{
					d = Col32To16(*s++);

#if USE_LCD320x240==2		// 1=enable output to LCD SPI display ST7789 320x240, 2=use core3
					*dst16++ = (u16)d;
#else
					this->Write16(d);
#endif
				}
			}
			s0 += wbs;
		}
	}

#if USE_LCD320x240!=2		// 1=enable output to LCD SPI display ST7789 320x240, 2=use core3
	this->WaitBusy();	// wait for the transmission to complet
	this->SetLen8();	// set 8-bit transfer

	// flush received bytes
	while (!SPI1_RxIsEmpty()) (void)SPI1_Read();

	// enable display
	if (!this->on)
	{
		this->on = True;
		this->WriteCmd(ST77XX_DISPON);
	}

	// disconnect display
	this->Disconnect();

#else		// 1=enable output to LCD SPI display ST7789 320x240, 2=use core3
	dsb();
	LCD_FrameBufDirty = True; // flag - update frame buffer from core3
	dsb();
	sev();	// send signal
	dmb();
#endif
}

// set font
//  font ... draw font (cells 8 pixels, 128 characters)
//  fontw ... font width (5 to 8 pixels)
//  fonth ... font height
void cLCD::SetFont(const u8* font, int fontw, int fonth)
{
	this->font = font; 	// draw font (cells 8 pixels, 128 characters)
	this->fontw = fontw;	// font width (5 to 8 pixels)
	this->fonth = fonth;	// font height
}

// clear screen with given color
void cLCD::DrawClearCol(u32 col)
{
	this->printinv = False;	// print invert
	int i;
	u32* d = this->framebuf;
	for (i = this->w * this->h; i > 0; i--) *d++ = col;
}

// clear screen with black color
void cLCD::DrawClear()
{
	this->DrawClearCol(COL_BLACK);
}

// copy frame buffer to drawing buffer of the main screen (It doesn't check
//   the validity of the coordinates; it just quickly copies the memory)
void cLCD::CopyToScreen(int x, int y)
{
	int wd = FrameBuffer.drawpitchpix;
	u32* d = &FrameBuffer.drawbuf[x + y*wd];
	const u32* s = this->framebuf;
	int w, h;
	w = this->w;
	for (h = this->h; h > 0; h--)
	{
		memcpy(d, s, w*4);
		d += wd;
		s += w;
	}
}

// copy frame buffer from drawing buffer of the main screen (It doesn't check
//   the validity of the coordinates; it just quickly copies the memory)
void cLCD::CopyFromScreen(int x, int y)
{
	int wd = FrameBuffer.drawpitchpix;
	const u32* s = &FrameBuffer.drawbuf[x + y*wd];
	u32* d = this->framebuf;
	int w, h;
	w = this->w;
	for (h = this->h; h > 0; h--)
	{
		memcpy(d, s, w*4);
		d += w;
		s += wd;
	}
}

// macro - prepare components for blending
#define DRAW_BLEND_PREP()				\
	u32 inv = 255 - a;				\
	u32 srb = (col & 0x00ff00ff)*a + 0x00ff00ff;	\
	u32 sg = (col & 0x0000ff00)*a + 0x0000ff00

// macro - blend one pixel
#define DRAW_BLEND_PIXEL()	{			\
	u32 dst = *d;					\
	u32 rb = (dst & 0x00ff00ff);			\
	u32 g = (dst & 0x0000ff00);			\
	rb = (srb + rb*inv) & 0xff00ff00;		\
	g = (sg + g*inv) & 0x00ff0000;			\
	*d = ((rb | g) >> 8) | 0xff000000; }

// invert value
#define DRAW_INV_VAL	0x00ffffff

// divide by 255
#define DIV255(nn) (((nn)*32897) >> 23)

// ----------------------------------------------------------------------------
//                            Draw point
// ----------------------------------------------------------------------------

// draw point (alpha 0=transparent, 255=opaque)
void cLCD::DrawPoint(int x, int y, u32 col)
{
	if (((u32)x < (u32)this->w) && ((u32)y < (u32)this->h))
	{
		// pointer to buffer
		u32* d = &this->framebuf[x + y*this->w];

		// get alpha
		u32 a = col >> 24;
		if (a == 0) return; // full transparent
		if (a == 255) // full opaque
		{
			*d = col;
			return;
		}

		// prepare components for blending
		DRAW_BLEND_PREP();

		// blend one pixel
		DRAW_BLEND_PIXEL();
	}
}

// get pixel (returns color COL_*)
u32 cLCD::GetPoint(int x, int y)
{
	if (((u32)x >= (u32)this->w) || ((u32)y >= (u32)this->h)) return COL_BLACK;
	return this->framebuf[x + y*this->w];
}

// ----------------------------------------------------------------------------
//                            Draw rectangle
// ----------------------------------------------------------------------------

// draw rectangle (w and h can be negative = flip rectangle; alpha 0=transparent, 255=opaque)
void cLCD::DrawRect(int x, int y, int w, int h, u32 col)
{
	int k;

	// flip rectangle
	if (w < 0)
	{
		x += w;
		w = -w;
	}

	if (h < 0)
	{
		y += h;
		h = -h;
	}

	// limit x
	if (x < 0)
	{
		w += x;
		x = 0;
	}

	// limit w
	k = this->w;
	if (x + w > k) w = k - x;
	if (w <= 0) return;

	// limit y
	if (y < 0)
	{
		h += y;
		y = 0;
	}

	// limit h
	k = this->h;
	if (y + h > k) h = k - y;
	if (h <= 0) return;

	// prepare destination
	int wb = this->w;
	u32* d = &this->framebuf[x + y*wb];
	wb -= w;
	int i;

	// get alpha
	u32 a = col >> 24;
	if (a == 0) return; // full transparent
	if (a == 255)
	{
		// full opaque
		for (; h > 0; h--)
		{
			for (i = w; i > 0; i--) *d++ = col;
			d += wb;
		}
		return;
	}

	// prepare components for blending
	DRAW_BLEND_PREP();

	// draw with blending
	for (; h > 0; h--)
	{
		for (i = w; i > 0; i--)
		{
			// blend one pixel
			DRAW_BLEND_PIXEL();
			d++;
		}
		d += wb;
	}
}

// ----------------------------------------------------------------------------
//                          Draw horizontal line
// ----------------------------------------------------------------------------

// draw horizontal line (w can be negative = flip line; alpha 0=transparent, 255=opaque)
void cLCD::DrawHLine(int x, int y, int w, u32 col)
{
	this->DrawRect(x, y, w, 1, col);
}

// ----------------------------------------------------------------------------
//                          Draw vertical line
// ----------------------------------------------------------------------------

// draw vertical line (h can be negative = flip line; alpha 0=transparent, 255=opaque)
void cLCD::DrawVLine(int x, int y, int h, u32 col)
{
	this->DrawRect(x, y, 1, h, col);
}

// ----------------------------------------------------------------------------
//                           Draw frame
// ----------------------------------------------------------------------------

// draw frame (dimensions must be > 0; alpha 0=transparent, 255=opaque)
void cLCD::DrawFrame(int x, int y, int w, int h, u32 col)
{
	// check size
	if ((w <= 0) || (h <= 0)) return;

	// bottom line
	this->DrawHLine(x, y+h-1, w, col);
	if (h > 1)
	{
		// right line
		this->DrawVLine(x+w-1, y, h-1, col);

		// top line
		this->DrawHLine(x, y, w-1, col);

		// left line (light)
		if (h > 2) this->DrawVLine(x, y+1, h-2, col);
	}
}

// draw frame 3D (dimensions must be > 0; alpha 0=transparent, 255=opaque)
void cLCD::DrawFrame2(int x, int y, int w, int h, u32 col_light, u32 col_dark)
{
	// check size
	if ((w <= 0) || (h <= 0)) return;

	// bottom line (dark)
	this->DrawHLine(x, y+h-1, w, col_dark);
	if (h > 1)
	{
		// right line (dark)
		this->DrawVLine(x+w-1, y, h-1, col_dark);

		// top line (light)
		this->DrawHLine(x, y, w-1, col_light);

		// left line (light)
		if (h > 2) this->DrawVLine(x, y+1, h-2, col_light);
	}
}

// draw thick frame (dimensions must be > 0; alpha 0=transparent, 255=opaque)
void cLCD::DrawFrameW(int x, int y, int w, int h, int thick, u32 col)
{
	// check size
	if ((w <= 0) || (h <= 0)) return;

	// full frame
	if ((w <= 2*thick) || (h <= 2*thick))
	{
		this->DrawRect(x, y, w, h, col);
	}

	// thick frame
	else
	{
		// top line
		this->DrawRect(x, y, w, thick, col);

		// bottom line
		this->DrawRect(x, y+h-thick, w, thick, col);

		// left line
		h -= 2*thick;
		y += thick;
		this->DrawRect(x, y, thick, h, col);

		// right line
		this->DrawRect(x+w-thick, y, thick, h, col);
	}
}

// ----------------------------------------------------------------------------
//                           Draw line
// ----------------------------------------------------------------------------

// Internal function - draw line with overlapped pixels (alpha 0=transparent, 255=opaque)
void cLCD::DrawLineOver(int x1, int y1, int x2, int y2, Bool over, u32 col)
{
	// difference of coordinates
	int dx = x2 - x1;
	int dy = y2 - y1;

	// increment X
	int sx = 1;
	if (dx < 0)
	{
		sx = -1;
		dx = -dx;
	}

	// increment Y
	int sy = 1;
	int wb = this->w;
	int ddy = wb;
	if (dy < 0)
	{
		sy = -1;
		ddy = -ddy;
		dy = -dy;
	}

	// destination address (pointer can be out of clipping range, but it is OK for now)
	u32* d = &this->framebuf[x1 + y1*wb];

	// clipping
	int maxx = wb;
	int maxy = this->h;

	// get alpha
	u32 a = col >> 24;
	if (a == 0) return; // full transparent
	if (a == 255) // full opaque
	{
		// draw first pixel
		if (((u32)x1 < (u32)maxx) && ((u32)y1 < (u32)maxy)) *d = col;

		// steeply in X direction, X is prefered as base
		if (dx > dy)
		{
			int m = 2*dy;
			int p = m - dx;
			dx = 2*dx;
			while (x1 != x2)
			{
				x1 += sx;
				d += sx;
				if (p > 0)
				{
					// draw overlapped pixel
					if (over && ((u32)x1 < (u32)maxx) && ((u32)y1 < (u32)maxy)) *d = col;
					y1 += sy;
					d += ddy;
					p -= dx;
				}
				p += m;
				if (((u32)x1 < (u32)maxx) && ((u32)y1 < (u32)maxy)) *d = col;
			}
		}

		// steeply in Y direction, Y is prefered as base
		else
		{
			int m = 2*dx;
			int p = m - dy;
			dy = 2*dy;
			while (y1 != y2)
			{
				y1 += sy;
				d += ddy;
				if (p > 0)
				{
					// draw overlapped pixel
					if (over && ((u32)x1 < (u32)maxx) && ((u32)y1 < (u32)maxy)) *d = col;
					x1 += sx;
					d += sx;
					p -= dy;
				}
				p += m;
				if (((u32)x1 < (u32)maxx) && ((u32)y1 < (u32)maxy)) *d = col;
			}
		}
	}

	// transparency
	else
	{
		// prepare components for blending
		DRAW_BLEND_PREP();

		// draw first pixel
		if (((u32)x1 < (u32)maxx) && ((u32)y1 < (u32)maxy))
		{
			// blend one pixel
			DRAW_BLEND_PIXEL();
		}

		// steeply in X direction, X is prefered as base
		if (dx > dy)
		{
			int m = 2*dy;
			int p = m - dx;
			dx = 2*dx;
			while (x1 != x2)
			{
				x1 += sx;
				d += sx;
				if (p > 0)
				{
					// draw overlapped pixel
					if (over && ((u32)x1 < (u32)maxx) && ((u32)y1 < (u32)maxy))
					{
						// blend one pixel
						DRAW_BLEND_PIXEL();
					}
					y1 += sy;
					d += ddy;
					p -= dx;
				}
				p += m;
				if (((u32)x1 < (u32)maxx) && ((u32)y1 < (u32)maxy))
				{
					// blend one pixel
					DRAW_BLEND_PIXEL();
				}
			}
		}

		// steeply in Y direction, Y is prefered as base
		else
		{
			int m = 2*dx;
			int p = m - dy;
			dy = 2*dy;
			while (y1 != y2)
			{
				y1 += sy;
				d += ddy;
				if (p > 0)
				{
					// draw overlapped pixel
					if (over && ((u32)x1 < (u32)maxx) && ((u32)y1 < (u32)maxy))
					{
						// blend one pixel
						DRAW_BLEND_PIXEL();
					}
					x1 += sx;
					d += sx;
					p -= dy;
				}
				p += m;
				if (((u32)x1 < (u32)maxx) && ((u32)y1 < (u32)maxy))
				{
					// blend one pixel
					DRAW_BLEND_PIXEL();
				}
			}
		}
	}
}

// draw line (alpha 0=transparent, 255=opaque)
void cLCD::DrawLine(int x1, int y1, int x2, int y2, u32 col)
{
	this->DrawLineOver(x1, y1, x2, y2, False, col);
}

// Draw thick line (alpha 0=transparent, 255=opaque)
//  thick ... thick of line in pixels
//  round ... draw round ends
// Do not use "round ends" and "transparent" at the same time - round ends incorrectly redraw transparency.
void cLCD::DrawLineW(int x1, int y1, int x2, int y2, int thick, Bool round, u32 col)
{
	int i, k, dx, dy, dxabs, dyabs;
	Bool over;

	// thick is 1
	if (thick <= 1)
	{
		if (thick == 1) this->DrawLineOver(x1, y1, x2, y2, False, col);
		return;
	}

	// difference of coordinates
	dxabs = x2 - x1;
	dyabs = y2 - y1;
	if (dxabs < 0) dxabs = -dxabs;
	if (dyabs < 0) dyabs = -dyabs;

	// draw round ends
	if (round && (thick > 2))
	{
		int thick2 = (thick-1) | 1; // a circle of odd size is needed to be placed exactly on the coordinate
		this->DrawRound(x1, y1, thick2, DRAW_ROUND_ALL, col);
		this->DrawRound(x2, y2, thick2, DRAW_ROUND_ALL, col);
	}

	// difference of coordinates
	//   X and Y coordinates are "swapped" because they express
	//   a direction orthogonal to the direction of the line
	dy = x2 - x1;
	dx = y2 - y1;
	Bool swap = True;

	// increment X
	int sx = 1;
	if (dx < 0)
	{
		sx = -1;
		dx = -dx;
		swap = !swap;
	}

	// increment Y
	int sy = 1;
	if (dy < 0)
	{
		sy = -1;
		dy = -dy;
		swap = !swap;
	}

	// thick correction (otherwise the diagonal lines would be thicker than the perpendicular lines)
	if (thick >= 4)
	{
		if ((dxabs >= dyabs*3/4) && (dxabs*3/4 <= dyabs))
		{
			if (thick >= 17) thick--;
			if (thick >= 14) thick--;
			if (thick >= 11) thick--;
			if (((thick & 1) == 0) || (thick >= 7)) thick--;
		}
	}

	// prepare adjust to shift to the middle of the line
	int dx2 = dx*2;
	int dy2 = dy*2;
	int adj = thick/2;

	// steeply in X direction, X is prefered as base
	if (dx >= dy)
	{
		// swapped direction
		if (swap)
		{
			adj = thick - 1 - adj;
			sy = -sy;
		}
		else
			sx = -sx;

		// shift to the middle of the line
		int err = dy2 - dx;
		for (i = adj; i > 0; i--)
		{
			x1 -= sx;
			x2 -= sx;
			if (err >= 0)
			{
				y1 -= sy;
				y2 -= sy;
				err -= dx2;
			}
			err += dy2;
		}

		// draw first line
		this->DrawLineOver(x1, y1, x2, y2, False, col);

		// draw other lines
		err = dy2 - dx;
		for (i = thick; i > 1; i--)
		{
			x1 += sx;
			x2 += sx;
			over = False;
			if (err >= 0)
			{
				y1 += sy;
				y2 += sy;
				err -= dx2;
				over = True;
			}

			// draw line
			this->DrawLineOver(x1, y1, x2, y2, over, col);
			err += dy2;
		}
	}

	// steeply in Y direction, Y is prefered as base
	else
	{
		// swapped direction
		if (swap)
			sx = -sx;
		else
		{
			adj = thick - 1 - adj;
			sy = -sy;
		}

		// shift to the middle of the line
		int err = dx2 - dy;
		for (i = adj; i > 0; i--)
		{
			y1 -= sy;
			y2 -= sy;
			if (err >= 0)
			{
				x1 -= sx;
				x2 -= sx;
				err -= dy2;
			}
			err += dx2;
		}

		// draw first line
		this->DrawLineOver(x1, y1, x2, y2, False, col);

		// draw other lines
		err = dx2 - dy;
		for (i = thick; i > 1; i--)
		{
			y1 += sy;
			y2 += sy;
			over = False;
			if (err >= 0)
			{
				x1 += sx;
				x2 += sx;
				err -= dy2;
				over = True;
			}

			// draw line
			this->DrawLineOver(x1, y1, x2, y2, over, col);
			err += dx2;
		}
	}
}

// ----------------------------------------------------------------------------
//                          Draw round (Filled circle)
// ----------------------------------------------------------------------------

#define DRAW_ROUND_HIDE()	{				\
	if ((mask & DRAW_ROUND_NOTOP) != 0) y1 = 0;		\
	if ((mask & DRAW_ROUND_NOBOTTOM) != 0) y2 = 0;		\
	if ((mask & DRAW_ROUND_NOLEFT) != 0) x1 = 0;		\
	if ((mask & DRAW_ROUND_NORIGHT) != 0) x2 = 0; }

#define DRAW_ROUND_CLIP()	{			\
		if (x+x1 < 0) x1 = - x;			\
		int k = this->w;			\
		if (x+x2 >= k) x2 = k - 1 - x;		\
		if (y+y1 < 0) y1 = - y;			\
		k = this->h;				\
		if (y+y2 >= k) y2 = k - 1 - y;		\
		if ((x2 < x1) || (y2 < y1)) return; }

// Draw round (alpha 0=transparent, 255=opaque)
//  diam ... diameter of the round (radius = d/2)
//  mask ... hide parts of the round with DRAW_ROUND_* (or their combination); use DRAW_ROUND_ALL or 0 to draw whole round
//		DRAW_ROUND_NOTOP	= hide top part of the round
//		DRAW_ROUND_NOBOTTOM	= hide bottom part of the round
//		DRAW_ROUND_NOLEFT	= hide left part of the round
//		DRAW_ROUND_NORIGHT	= hide right part of the round
//		DRAW_ROUND_ALL		= draw whole round
void cLCD::DrawRound(int x, int y, int diam, int mask, u32 col)
{
	if (diam < 1) diam = 1;
	int r = (diam+1)/2;
	int x1 = -r;
	int x2 = +r;
	int y1 = -r;
	int y2 = +r;
	int wb = this->w;
	u32 *d0, *d;
	int r2;

	// hide parts of the round
	DRAW_ROUND_HIDE();

	// get alpha
	u32 a = col >> 24;
	if (a == 0) return; // full transparent

	// odd diameter (1, 3, 5, ...)
	if ((diam & 1) != 0)
	{
		// clipping
		DRAW_ROUND_CLIP();

		// prepare buffer address
		d0 = &this->framebuf[(x+x1) + (y+y1)*wb];
		r2 = r*(r-1);
		if (diam == 3) r2--;

		// full opaque
		if (a == 255)
		{
			// draw round
			for (y = y1; y <= y2; y++)
			{
				d = d0;
				for (x = x1; x <= x2; x++)
				{
					if ((x*x + y*y) <= r2) *d = col;
					d++;
				}
				d0 += wb;
			}
		}

		// transparent
		else
		{
			// prepare components for blending
			DRAW_BLEND_PREP();

			// draw round
			for (y = y1; y <= y2; y++)
			{
				d = d0;
				for (x = x1; x <= x2; x++)
				{
					if ((x*x + y*y) <= r2)
					{
						// blend one pixel
						DRAW_BLEND_PIXEL();
					}
					d++;
				}
				d0 += wb;
			}
		}
	}

	// even diameter (2, 4, 6, ...)
	else
	{
		// to correspond to the center of DrawCircle [x0,y0] = [x+d/2,y+d/2]
		x--;
		y--;
		x1++;
		y1++;

		// clipping
		DRAW_ROUND_CLIP();

		// prepare buffer address
		d0 = &this->framebuf[(x+x1) + (y+y1)*wb];
		r2 = r*r;

		// full opaque
		if (a == 255)
		{
			// draw round
			for (y = y1; y <= y2; y++)
			{
				d = d0;
				for (x = x1; x <= x2; x++)
				{
					if ((x*(x-1) + y*(y-1)) < r2) *d = col;
					d++;
				}
				d0 += wb;
			}
		}

		// transparent
		else
		{
			// prepare components for blending
			DRAW_BLEND_PREP();

			// draw round
			for (y = y1; y <= y2; y++)
			{
				d = d0;
				for (x = x1; x <= x2; x++)
				{
					if ((x*(x-1) + y*(y-1)) < r2)
					{
						// blend one pixel
						DRAW_BLEND_PIXEL();
					}
					d++;
				}
				d0 += wb;
			}
		}
	}
}

// ----------------------------------------------------------------------------
//                               Draw circle
// ----------------------------------------------------------------------------
// Using Mid-Point Circle Drawing Algorithm

// draw circle arcs
//#define DRAW_CIRCLE_ARC0	B0		// draw arc 0..45 deg
//#define DRAW_CIRCLE_ARC1	B1		// draw arc 45..90 deg
//#define DRAW_CIRCLE_ARC2	B2		// draw arc 90..135 deg
//#define DRAW_CIRCLE_ARC3	B3		// draw arc 135..180 deg
//#define DRAW_CIRCLE_ARC4	B4		// draw arc 180..225 deg
//#define DRAW_CIRCLE_ARC5	B5		// draw arc 225..270 deg
//#define DRAW_CIRCLE_ARC6	B6		// draw arc 270..315 deg
//#define DRAW_CIRCLE_ARC7	B7		// draw arc 315..360 deg
//#define DRAW_CIRCLE_ALL	0xff		// draw whole circle

#define DRAW_CIRCLE_PIX(xxx,yyy,mmm)		\
	if ((mask & mmm) != 0)			\
	{	int x2 = x+(xxx);		\
		int y2 = y+(yyy);		\
		if (((u32)x2 < (u32)maxx) &&	\
		    ((u32)y2 < (u32)maxy)) {	\
		d = &d0[x2 + y2*wb];		\
		*d = col; } }

#define DRAW_CIRCLE_PIX_BLEND(xxx,yyy,mmm)	\
	if ((mask & mmm) != 0)			\
	{	int x2 = x+(xxx);		\
		int y2 = y+(yyy);		\
		if (((u32)x2 < (u32)maxx) &&	\
		    ((u32)y2 < (u32)maxy)) {	\
		d = &d0[x2 + y2*wb];		\
		DRAW_BLEND_PIXEL(); } }

#define DRAW_CIRCLE_PIXINV(xxx,yyy,mmm)		\
	if ((mask & mmm) != 0)			\
	{	int x2 = x+(xxx);		\
		int y2 = y+(yyy);		\
		if (((u32)x2 < (u32)maxx) &&	\
		    ((u32)y2 < (u32)maxy)) {	\
		d = &d0[x2 + y2*wb];		\
		*d = ~*d; } }

// Draw circle or arc (alpha 0=transparent, 255=opaque)
//  diam ... diameter of the circle (radius = d/2)
//  mask ... draw circle arcs, use combination of DRAW_CIRCLE_*; use DRAW_CIRCLE_ALL or 0xff to draw whole circle
void cLCD::DrawCircle(int x, int y, int diam, int mask, u32 col)
{
	// check diameter
	if (diam <= 1)
	{
		if (diam == 1) this->DrawPoint(x, y, col);
		return;
	}

	// prepare
	int r = diam/2;
	int xx = 0;
	int yy = r;
	int p = 1 - r;
	int maxx = this->w;
	int maxy = this->h;

	// pointer to middle of the circle
	int wb = maxx;
	u32* d0 = this->framebuf;
	u32* d;

	// get alpha
	u32 a = col >> 24;
	if (a == 0) return; // full transparent
	int c = -1; // even correction - on even diameter do not draw middle point
	if (a == 255) // full opaque
	{
		// odd diameter
		if ((diam & 1) != 0) // on odd diameter - draw middle point
		{
			// draw middle point (xx = 0)
			DRAW_CIRCLE_PIX(+yy,  0,DRAW_CIRCLE_ARC0);	// 0..45 deg, 315..360 deg
			DRAW_CIRCLE_PIX(  0,-yy,DRAW_CIRCLE_ARC1);	// 45..90 deg, 90..135 deg
			DRAW_CIRCLE_PIX(-yy,  0,DRAW_CIRCLE_ARC3);	// 135..180 deg, 180..225 deg
			DRAW_CIRCLE_PIX(  0,+yy,DRAW_CIRCLE_ARC5);	// 225..270 deg, 270..315 deg
			c = 0;
		}

		while (True)
		{
			// shift to next point
			xx++;
			if (p > 0)
			{
				yy--;
				p -= 2*yy;
			}
			p += 2*xx + 1;

			// stop drawing
			if (xx >= yy)
			{
				if (xx == yy)
				{
					// draw last point (xx == yy)
					DRAW_CIRCLE_PIX(+xx+c,-xx  ,DRAW_CIRCLE_ARC0);	// 0..45 deg, 45..90 deg
					DRAW_CIRCLE_PIX(-xx  ,-xx  ,DRAW_CIRCLE_ARC2);	// 90..135 deg, 135..180 deg
					DRAW_CIRCLE_PIX(-xx  ,+xx+c,DRAW_CIRCLE_ARC4);	// 180..225 deg, 225..270 deg
					DRAW_CIRCLE_PIX(+xx+c,+xx+c,DRAW_CIRCLE_ARC6);	// 270..315 deg, 315..360 deg
				}
				break;
			}

			// draw points
			DRAW_CIRCLE_PIX(+yy+c,-xx  ,DRAW_CIRCLE_ARC0);		// 0..45 deg
			DRAW_CIRCLE_PIX(+xx+c,-yy  ,DRAW_CIRCLE_ARC1);		// 45..90 deg
			DRAW_CIRCLE_PIX(-xx  ,-yy  ,DRAW_CIRCLE_ARC2);		// 90..135 deg
			DRAW_CIRCLE_PIX(-yy  ,-xx  ,DRAW_CIRCLE_ARC3);		// 135..180 deg
			DRAW_CIRCLE_PIX(-yy  ,+xx+c,DRAW_CIRCLE_ARC4);		// 180..225 deg
			DRAW_CIRCLE_PIX(-xx  ,+yy+c,DRAW_CIRCLE_ARC5);		// 225..270 deg
			DRAW_CIRCLE_PIX(+xx+c,+yy+c,DRAW_CIRCLE_ARC6);		// 270..315 deg
			DRAW_CIRCLE_PIX(+yy+c,+xx+c,DRAW_CIRCLE_ARC7);		// 315..360 deg
		}
	}

	// transparency
	else
	{
		// prepare components for blending
		DRAW_BLEND_PREP();

		// odd diameter
		if ((diam & 1) != 0) // on odd diameter - draw middle point
		{
			// draw middle point (xx = 0)
			DRAW_CIRCLE_PIX_BLEND(+yy,  0,DRAW_CIRCLE_ARC0);	// 0..45 deg, 315..360 deg
			DRAW_CIRCLE_PIX_BLEND(  0,-yy,DRAW_CIRCLE_ARC1);	// 45..90 deg, 90..135 deg
			DRAW_CIRCLE_PIX_BLEND(-yy,  0,DRAW_CIRCLE_ARC3);	// 135..180 deg, 180..225 deg
			DRAW_CIRCLE_PIX_BLEND(  0,+yy,DRAW_CIRCLE_ARC5);	// 225..270 deg, 270..315 deg
			c = 0;
		}

		while (True)
		{
			// shift to next point
			xx++;
			if (p > 0)
			{
				yy--;
				p -= 2*yy;
			}
			p += 2*xx + 1;

			// stop drawing
			if (xx >= yy)
			{
				if (xx == yy)
				{
					// draw last point (xx == yy)
					DRAW_CIRCLE_PIX_BLEND(+xx+c,-xx  ,DRAW_CIRCLE_ARC0);	// 0..45 deg, 45..90 deg
					DRAW_CIRCLE_PIX_BLEND(-xx  ,-xx  ,DRAW_CIRCLE_ARC2);	// 90..135 deg, 135..180 deg
					DRAW_CIRCLE_PIX_BLEND(-xx  ,+xx+c,DRAW_CIRCLE_ARC4);	// 180..225 deg, 225..270 deg
					DRAW_CIRCLE_PIX_BLEND(+xx+c,+xx+c,DRAW_CIRCLE_ARC6);	// 270..315 deg, 315..360 deg
				}
				break;
			}

			// draw points
			DRAW_CIRCLE_PIX_BLEND(+yy+c,-xx  ,DRAW_CIRCLE_ARC0);		// 0..45 deg
			DRAW_CIRCLE_PIX_BLEND(+xx+c,-yy  ,DRAW_CIRCLE_ARC1);		// 45..90 deg
			DRAW_CIRCLE_PIX_BLEND(-xx  ,-yy  ,DRAW_CIRCLE_ARC2);		// 90..135 deg
			DRAW_CIRCLE_PIX_BLEND(-yy  ,-xx  ,DRAW_CIRCLE_ARC3);		// 135..180 deg
			DRAW_CIRCLE_PIX_BLEND(-yy  ,+xx+c,DRAW_CIRCLE_ARC4);		// 180..225 deg
			DRAW_CIRCLE_PIX_BLEND(-xx  ,+yy+c,DRAW_CIRCLE_ARC5);		// 225..270 deg
			DRAW_CIRCLE_PIX_BLEND(+xx+c,+yy+c,DRAW_CIRCLE_ARC6);		// 270..315 deg
			DRAW_CIRCLE_PIX_BLEND(+yy+c,+xx+c,DRAW_CIRCLE_ARC7);		// 315..360 deg
		}
	}
}

// ----------------------------------------------------------------------------
//                              Draw image
// ----------------------------------------------------------------------------

// limit image coordinates
#define DRAW_IMG_LIMIT() {				\
	/* limit X */					\
	if (xs < 0) { w += xs; x -= xs; xs = 0; }	\
	if (x < 0) { w += x; xs -= x; x = 0; }		\
	/* limit w */					\
	int k = this->w - x; if (w > k) w = k;		\
	ws = pic->w;					\
	k = ws - xs; if (w > k) w = k;			\
	if (w <= 0) return;				\
	/* limit y */					\
	if (ys < 0) { h += ys; y -= ys; ys = 0; }	\
	if (y < 0) { h += y; ys -= y; y = 0; }		\
	/* limit h */					\
	k = this->h - y; if (h > k) h = k;		\
	hs = pic->h;					\
	k = hs - ys; if (h > k) h = k;			\
	if (h <= 0) return; }


// Draw image
//  img ... image in format sPic, must be in aligned CF_A8B8G8R8 or CF_B8G8R8 format
//  x ... destination X coordiate
//  y ... destination Y coordiate
//  xs ... source X coordinate
//  ys ... source Y coordinate
//  w ... width
//  h ... height
//  alpha ... transparency 0..255: 0=transparent, 255=opaque
void cLCD::DrawImg(const u8* img, int x, int y, int xs, int ys, int w, int h, int alpha)
{
	int ws, hs, i;

	// check source alpha
	if (alpha <= 0) return;
	if (alpha > 255) alpha = 255;

	// source image
	const sPic* pic = (const sPic*)img;
	if ((pic->colfmt != CF_A8B8G8R8) && (pic->colfmt != CF_B8G8R8)) return;

	// limit coordinates
	DRAW_IMG_LIMIT();

	// destination address
	int wb = this->w;
	u32* d = &this->framebuf[x + y*wb];
	wb -= w;

	// 24-bit format without alpha
	if (pic->colfmt == CF_B8G8R8)
	{
		// source address
		int wbs = pic->wb;
		const u8* s = pic->data + xs*3 + ys*wbs;
		wbs -= w*3;

		// full opaque
		if (alpha == 255)
		{
			for (; h > 0; h--)
			{
				for (i = w; i > 0; i--)
				{
					*d = s[0] | ((u32)s[1] << 8) | ((u32)s[2] << 16) | 0xff000000;
					d++;
					s += 3;
				}
				d += wb;
				s += wbs;
			}
		}

		// draw with blending
		else
		{
			u32 inv = 255 - alpha;

			for (; h > 0; h--)
			{
				for (i = w; i > 0; i--)
				{
					// get source pixel
					u8 red = s[0];
					u8 green = s[1];
					u8 blue = s[2];
					s += 3;
					u32 srb = (red | ((u32)blue << 16))*alpha + 0x00ff00ff;
					u32 sg = ((u32)green << 8)*alpha + 0x0000ff00;
					u32 dst = *d;
					u32 rb = (dst & 0x00ff00ff);
					u32 g = (dst & 0x0000ff00);
					rb = (srb + rb*inv) & 0xff00ff00;
					g = (sg + g*inv) & 0x00ff0000;
					*d = ((rb | g) >> 8) | 0xff000000;
					d++;
				}
				d += wb;
				s += wbs;
			}
		}
	}

	// 32-bit format CF_A8B8G8R8 with Alpha
	else
	{
		// source address
		int wbs = pic->wb/4;
		const u32* s = ((const u32*)pic->data) + xs + ys*wbs;
		wbs -= w;

		// draw with blending
		for (; h > 0; h--)
		{
			for (i = w; i > 0; i--)
			{
				// get source pixel
				u32 col = *s++;
			
				// get source alpha
				u32 a = col >> 24;
				a *= alpha;
				a = DIV255(a);

				// full opaque
				if (a == 255)
				{
					*d = col;
				}
				else if (a != 0)
				{
					DRAW_BLEND_PREP();
					DRAW_BLEND_PIXEL();
				}
				d++;
			}
			d += wb;
			s += wbs;
		}
	}
}

// ----------------------------------------------------------------------------
//                              Draw character
// ----------------------------------------------------------------------------

// draw character using system fixed font (alpha 0=transparent, 255=opaque; use DrawSelFont() to select font)
// - To invert text, use bit 7 of the characters, or FrameBuffer.inv flag (flags are XORed).
void cLCD::DrawCharScale(u8 ch, int x, int y, int scalex, int scaley, u32 col)
{
	int sx, sy;
	int b, m, i, x2, dwb;
	u32* d;

	// get alpha
	u32 a = col >> 24;
	if (a == 0) return;	// full transparent

	// get clipping
	int maxx = this->w;
	int maxy = this->h;

	// prepare inversion mask
	u32 binv = this->printinv ? (~0) : 0;
	if (ch >= 0x80)
	{
		ch &= 0x7f;
		binv = ~binv;
	}

	// pointer to the font
	const u8* s = &this->font[ch];

	// prepare destination (at this place, it can be out of screen)
	int wb = maxx;

	// font size
	int w = this->fontw;
	int h = this->fonth;

	// full opaque
	if (a == 255)
	{
		// loop through font height
		for (; h > 0; h--)
		{
			// loop through scale Y
			for (sy = scaley; sy > 0; sy--)
			{
				// check if Y is valid
				if ((u32)y < (u32)maxy)
				{
					// destination
					d = &this->framebuf[x + y*wb];

					// load font sample
					b = *s ^ binv;

					// loop through font width
					x2 = x;
					for (i = w; i > 0; i--)
					{
						// check if draw this pixel
						if ((b & B7) == 0)
						{
							// loop through scale X
							for (sx = scalex; sx > 0; sx--)
							{
								// check if X is valid
								if ((u32)x2 < (u32)maxx)
								{
									// draw pixel
									*d = col;
								}

								// shift X
								x2++;
								d++;
							}
						}
						else
						{
							x2 += scalex;
							d += scalex;
						}

						// shift 
						b <<= 1;
					}
				}

				// shift Y
				y++;
			}

			// shift source to next line
			s += 128;
		}
	}

	// transparent
	else
	{
		// prepare components for blending
		DRAW_BLEND_PREP();

		// loop through font height
		for (; h > 0; h--)
		{
			// loop through scale Y
			for (sy = scaley; sy > 0; sy--)
			{
				// check if Y is valid
				if ((u32)y < (u32)maxy)
				{
					// destination
					d = &this->framebuf[x + y*wb];

					// load font sample
					b = *s ^ binv;

					// loop through font width
					x2 = x;
					for (i = w; i > 0; i--)
					{
						// check if draw this pixel
						if ((b & B7) == 0)
						{
							// loop through scale X
							for (sx = scalex; sx > 0; sx--)
							{
								// check if X is valid
								if ((u32)x2 < (u32)maxx)
								{
									// blend pixel
									DRAW_BLEND_PIXEL();
								}

								// shift X
								x2++;
								d++;
							}
						}
						else
						{
							x2 += scalex;
							d += scalex;
						}

						// shift 
						b <<= 1;
					}
				}

				// shift Y
				y++;
			}

			// shift source to next line
			s += 128;
		}
	}
}

void cLCD::DrawChar(u8 ch, int x, int y, u32 col) { this->DrawCharScale(ch, x, y, 1, 1, col); }
void cLCD::DrawCharW(u8 ch, int x, int y, u32 col) { this->DrawCharScale(ch, x, y, 2, 1, col); }
void cLCD::DrawCharH(u8 ch, int x, int y, u32 col) { this->DrawCharScale(ch, x, y, 1, 2, col); }
void cLCD::DrawChar2(u8 ch, int x, int y, u32 col) { this->DrawCharScale(ch, x, y, 2, 2, col); }
void cLCD::DrawChar3(u8 ch, int x, int y, u32 col) { this->DrawCharScale(ch, x, y, 3, 3, col); }
void cLCD::DrawChar4(u8 ch, int x, int y, u32 col) { this->DrawCharScale(ch, x, y, 4, 4, col); }

// draw character with background, using system fixed font (no alpha transparency; use SetFont() to select font)
// - To invert text, use bit 7 of the characters, or this->printinv flag (flags are XORed).
void cLCD::DrawCharBgScale(u8 ch, int x, int y, int scalex, int scaley, u32 colfg, u32 colbg)
{
	int sx, sy;
	int b, m, i, x2, dwb;
	u32* d;

	// get clipping
	int maxx = this->w;
	int maxy = this->h;

	// prepare inversion mask
	u32 binv = this->printinv ? (~0) : 0;
	if (ch >= 0x80)
	{
		ch &= 0x7f;
		binv = ~binv;
	}

	// pointer to the font
	const u8* s = &this->font[ch];

	// prepare destination (at this place, it can be out of screen)
	int wb = maxx;

	// font size
	int w = this->fontw;
	int h = this->fonth;

	// loop through font height
	for (; h > 0; h--)
	{
		// loop through scale Y
		for (sy = scaley; sy > 0; sy--)
		{
			// check if Y is valid
			if ((u32)y < (u32)maxy)
			{
				// destination
				d = &this->framebuf[x + y*wb];

				// load font sample
				b = *s ^ binv;

				// loop through font width
				x2 = x;
				for (i = w; i > 0; i--)
				{
					// loop through scale X
					for (sx = scalex; sx > 0; sx--)
					{
						// check if X is valid
						if ((u32)x2 < (u32)maxx)
						{
							// draw pixel
							*d = ((b & B7) == 0) ? colfg : colbg;
						}

						// shift X
						x2++;
						d++;
					}

					// shift 
					b <<= 1;
				}
			}

			// shift Y
			y++;
		}

		// shift source to next line
		s += 128;
	}
}

void cLCD::DrawCharBg(u8 ch, int x, int y, u32 colfg, u32 colbg) { this->DrawCharBgScale(ch, x, y, 1, 1, colfg, colbg); }
void cLCD::DrawCharBgW(u8 ch, int x, int y, u32 colfg, u32 colbg) { this->DrawCharBgScale(ch, x, y, 2, 1, colfg, colbg); }
void cLCD::DrawCharBgH(u8 ch, int x, int y, u32 colfg, u32 colbg) { this->DrawCharBgScale(ch, x, y, 1, 2, colfg, colbg); }
void cLCD::DrawCharBg2(u8 ch, int x, int y, u32 colfg, u32 colbg) { this->DrawCharBgScale(ch, x, y, 2, 2, colfg, colbg); }
void cLCD::DrawCharBg3(u8 ch, int x, int y, u32 colfg, u32 colbg) { this->DrawCharBgScale(ch, x, y, 3, 3, colfg, colbg); }
void cLCD::DrawCharBg4(u8 ch, int x, int y, u32 colfg, u32 colbg) { this->DrawCharBgScale(ch, x, y, 4, 4, colfg, colbg); }

// draw text with length, using system fixed font (alpha 0=transparent, 255=opaque; use DrawSelFont() to select font)
// - To invert text, use bit 7 of the characters, or this->printinv flag (flags are XORed).
// - Returns shift of X in pixels.
int cLCD::DrawTextLenScale(const char* text, int len, int x, int y, int scalex, int scaley, u32 col)
{
	int dx = this->fontw*scalex;
	int res = 0;
	for (; len > 0; len--)
	{
		this->DrawCharScale(*text++, x, y, scalex, scaley, col);
		x += dx;
		res += dx;
	}
	return res;
}

int cLCD::DrawTextLen(const char* text, int len, int x, int y, u32 col) { return this->DrawTextLenScale(text, len, x, y, 1, 1, col); }
int cLCD::DrawTextLenW(const char* text, int len, int x, int y, u32 col) { return this->DrawTextLenScale(text, len, x, y, 2, 1, col); }
int cLCD::DrawTextLenH(const char* text, int len, int x, int y, u32 col) { return this->DrawTextLenScale(text, len, x, y, 1, 2, col); }
int cLCD::DrawTextLen2(const char* text, int len, int x, int y, u32 col) { return this->DrawTextLenScale(text, len, x, y, 2, 2, col); }
int cLCD::DrawTextLen3(const char* text, int len, int x, int y, u32 col) { return this->DrawTextLenScale(text, len, x, y, 3, 3, col); }
int cLCD::DrawTextLen4(const char* text, int len, int x, int y, u32 col) { return this->DrawTextLenScale(text, len, x, y, 4, 4, col); }

// draw ASCIIZ text (terminated with zero), using system fixed font (alpha 0=transparent, 255=opaque; use DrawSelFont() to select font)
// - To invert text, use bit 7 of the characters, or this->printinv flag (flags are XORed).
// - Returns shift of X in pixels.
int cLCD::DrawTextScale(const char* text, int x, int y, int scalex, int scaley, u32 col)
{
	return this->DrawTextLenScale(text, StrLen(text), x, y, scalex, scaley, col);
}

int cLCD::DrawText(const char* text, int x, int y, u32 col) { return this->DrawTextScale(text, x, y, 1, 1, col); }
int cLCD::DrawTextW(const char* text, int x, int y, u32 col) { return this->DrawTextScale(text, x, y, 2, 1, col); }
int cLCD::DrawTextW2(const char* text, int x, int y, u32 col) { return this->DrawTextScale(text, x, y, 4, 2, col); }
int cLCD::DrawTextH(const char* text, int x, int y, u32 col) { return this->DrawTextScale(text, x, y, 1, 2, col); }
int cLCD::DrawText2(const char* text, int x, int y, u32 col) { return this->DrawTextScale(text, x, y, 2, 2, col); }
int cLCD::DrawText3(const char* text, int x, int y, u32 col) { return this->DrawTextScale(text, x, y, 3, 3, col); }
int cLCD::DrawText4(const char* text, int x, int y, u32 col) { return this->DrawTextScale(text, x, y, 4, 4, col); }

// draw text with length and background, using system fixed font (no alpha transparency; use DrawSelFont() to select font)
// - To invert text, use bit 7 of the characters, or FrameBuffer.inv flag (flags are XORed).
// - Returns shift of X in pixels.
int cLCD::DrawTextBgLenScale(const char* text, int len, int x, int y, int scalex, int scaley, u32 colfg, u32 colbg)
{
	int dx = this->fontw*scalex;
	int res = 0;
	for (; len > 0; len--)
	{
		this->DrawCharBgScale(*text++, x, y, scalex, scaley, colfg, colbg);
		x += dx;
		res += dx;
	}
	return res;
}

int cLCD::DrawTextBgLen(const char* text, int len, int x, int y, u32 colfg, u32 colbg) { return this->DrawTextBgLenScale(text, len, x, y, 1, 1, colfg, colbg); }
int cLCD::DrawTextBgLenW(const char* text, int len, int x, int y, u32 colfg, u32 colbg) { return this->DrawTextBgLenScale(text, len, x, y, 2, 1, colfg, colbg); }
int cLCD::DrawTextBgLenH(const char* text, int len, int x, int y, u32 colfg, u32 colbg) { return this->DrawTextBgLenScale(text, len, x, y, 1, 2, colfg, colbg); }
int cLCD::DrawTextBgLen2(const char* text, int len, int x, int y, u32 colfg, u32 colbg) { return this->DrawTextBgLenScale(text, len, x, y, 2, 2, colfg, colbg); }
int cLCD::DrawTextBgLen3(const char* text, int len, int x, int y, u32 colfg, u32 colbg) { return this->DrawTextBgLenScale(text, len, x, y, 3, 3, colfg, colbg); }
int cLCD::DrawTextBgLen4(const char* text, int len, int x, int y, u32 colfg, u32 colbg) { return this->DrawTextBgLenScale(text, len, x, y, 4, 4, colfg, colbg); }

// draw ASCIIZ text with background (terminated with zero), using system fixed font (no alpha transparency; use DrawSelFont() to select font)
// - To invert text, use bit 7 of the characters, or FrameBuffer.inv flag (flags are XORed).
// - Returns shift of X in pixels.
int cLCD::DrawTextBgScale(const char* text, int x, int y, int scalex, int scaley, u32 colfg, u32 colbg)
{
	return this->DrawTextBgLenScale(text, StrLen(text), x, y, scalex, scaley, colfg, colbg);
}

int cLCD::DrawTextBg(const char* text, int x, int y, u32 colfg, u32 colbg) { return this->DrawTextBgScale(text, x, y, 1, 1, colfg, colbg); }
int cLCD::DrawTextBgW(const char* text, int x, int y, u32 colfg, u32 colbg) { return this->DrawTextBgScale(text, x, y, 2, 1, colfg, colbg); }
int cLCD::DrawTextBgH(const char* text, int x, int y, u32 colfg, u32 colbg) { return this->DrawTextBgScale(text, x, y, 1, 2, colfg, colbg); }
int cLCD::DrawTextBg2(const char* text, int x, int y, u32 colfg, u32 colbg) { return this->DrawTextBgScale(text, x, y, 2, 2, colfg, colbg); }
int cLCD::DrawTextBg3(const char* text, int x, int y, u32 colfg, u32 colbg) { return this->DrawTextBgScale(text, x, y, 3, 3, colfg, colbg); }
int cLCD::DrawTextBg4(const char* text, int x, int y, u32 colfg, u32 colbg) { return this->DrawTextBgScale(text, x, y, 4, 4, colfg, colbg); }

#endif // USE_LCD
