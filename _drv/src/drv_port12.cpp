
// ****************************************************************************
//
//                       BarePi 12-channel PORT module
//
// ****************************************************************************

#include "../../includes.h"	// includes

#if USE_PORT12		// 1=use BarePi 12-channel PORT module, 0=not used (drv_port12.*)

// initialize
//  i2c ... I2C bus index (0 or 1)
//  addr ... I2C address (default PORT12_ADDR=0x38)
//  speed ... transfer speed in Hz (usually 100000 to 400000)
void cPORT12::Init(int i2c /*= PORT12_I2C*/, int addr /*= PORT12_ADDR*/, int speed /*= PORT12_SPEED*/)
{
	this->i2c = i2c;
	this->addr = addr;
	this->speed = speed;
}

// Check if PORT12 module is connected (returns False on error)
Bool cPORT12::Check()
{
//	'P','O','R','T',			// 8 (4): interface "PORT"
//	'1',					// 12 (1): version
//	PORT_NUM,				// 13 (1): number of ports

	// read ID array
	u8 buf[6];
	if (!I2Cbus_ReadReg16(this->i2c, this->addr, 8, buf, 6, this->speed)) return False;

	// check ID array
	return (buf[0] == 'P') && (buf[1] == 'O') && (buf[2] == 'R') && (buf[3] == 'T') &&
		(buf[4] == '1') && (buf[5] == PORT12_NUM);
}

// Setup timer of PWM channels PWM1A (port channel 6) and PWM1B (port channel 7). Returns False on error.
//  div ... prescaler-1 (= div1-1), value 0..65535
//  loop ... loader-1 (= loop1-1), value 0..65535
//  Base frequency of the PWM1 f = 48000000/(div+1)/(loop+1) = 48000000/div1/loop1
Bool cPORT12::SetupPWM1(u16 div, u16 loop)
{
//	(PORT_DIV_DEF-1)&0xff, (PORT_DIV_DEF-1)>>8, // 14 (2): PWM1 u16 divider-1 0..65535
//	(PORT_LOOP_DEF-1)&0xff, (PORT_LOOP_DEF-1)>>8, // 16 (2): PWM1 u16 loop-1 0..65535

	u16 buf[2];
	buf[0] = div;
	buf[1] = loop;
	return I2Cbus_WriteReg16(this->i2c, this->addr, PORT12_DIV1_INX, (const u8*)buf, 4, this->speed);
}

// Setup timer of PWM channels PWM2A (port channel 8) and PWM2B (port channel 9). Returns False on error.
//  div ... prescaler-1 (= div2-1), value 0..65535
//  loop ... loader-1 (= loop2-1), value 0..65535
//  Base frequency of the PWM2 f = 48000000/(div+1)/(loop+1) = 48000000/div2/loop2
Bool cPORT12::SetupPWM2(u16 div, u16 loop)
{
//	(PORT_DIV_DEF-1)&0xff, (PORT_DIV_DEF-1)>>8, // 18 (2): PWM2 u16 divider-1 0..65535
//	(PORT_LOOP_DEF-1)&0xff, (PORT_LOOP_DEF-1)>>8, // 20 (2): PWM2 u16 loop-1 0..65535

	u16 buf[2];
	buf[0] = div;
	buf[1] = loop;
	return I2Cbus_WriteReg16(this->i2c, this->addr, PORT12_DIV2_INX, (const u8*)buf, 4, this->speed);
}

// setup functions of port channels (return False on error)
//  chan ... first port channel 0..11
//  func ... array of functions PORT12_FUNC_*
//  num ... number of channels 1..12
// Functions PORT12_FUNC_ADC and PORT12_FUNC_ADCF are supported only on channels 0 to 5.
// Function PORT12_FUNC_PWM is supported only on channels 6 to 9.
// PWM channels 6 and 7 require common timer setup SetupPWM1().
// PWM channels 8 and 9 require common timer setup SetupPWM2().
Bool cPORT12::SetupFunc(int chan, const u8* func, int num)
{
//	[22 ... 33] = IO_FUNC_IN,		// 22 (12): port u8[12] function IO_FUNC_*

	return I2Cbus_WriteReg16(this->i2c, this->addr, PORT12_FUNC_INX+chan, func, num, this->speed);
}

// set data to port channels (returns False on error)
//  chan ... first port channel 0..11
//  data ... array of port values (output: LOW=0, HIGH=1; PWM: 0 to LOOP value)
//  num ... number of channels 1..12
Bool cPORT12::SetData(int chan, const u16* data, int num)
{
//	[34 ... 57] = 0,			// 34 (24): port data u16[12] (LOW and HIGH byte)

	return I2Cbus_WriteReg16(this->i2c, this->addr, PORT12_DATA_INX+2*chan, (const u8*)data, 2*num, this->speed);
}

// get data from port channels (returns False on error)
//  chan ... first port channel 0..11, or 12 = ADCref measure internal reference 1.2V
//  data ... array of port values (input: LOW=0, HIGH=1; button: OFF=0, ON=1; ADC: 0 to 4095)
//  num ... number of channels 1..13 (last index 12 is ADCref)
Bool cPORT12::GetData(int chan, u16* data, int num)
{
//	[34 ... 57] = 0,			// 34 (24): port data u16[12] (LOW and HIGH byte)
//	0, 0,					// 58 (2): ADCref data u16 (measure internal reference 1.2V; LOW and HIGH byte)

	return I2Cbus_ReadReg16(this->i2c, this->addr, PORT12_DATA_INX+2*chan, (u8*)data, 2*num, this->speed);
}

#endif // USE_PORT12
