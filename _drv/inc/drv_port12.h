
// ****************************************************************************
//
//                       BarePi 12-channel PORT module
//
// ****************************************************************************

#if USE_PORT12		// 1=use BarePi 12-channel PORT module, 0=not used (drv_port12.*)

#ifndef _DRV_PORT12_H
#define _DRV_PORT12_H

#ifndef PORT12_I2C
#define PORT12_I2C	0	// PORT12 I2C bus
#endif

#ifndef PORT12_ADDR
#define PORT12_ADDR	0x38	// PORT12 I2C address
#endif

#ifndef PORT12_SPEED
#define PORT12_SPEED	100000	// PORT12 I2C speed
#endif

#if (PORT12_I2C != 1) && (!USE_I2CBUS || !USE_I2C0BUS)
#error "PORT12 requires I2C0 bus driver"
#endif

#if (PORT12_I2C != 0) && (!USE_I2CBUS || !USE_I2C1BUS)
#error "PORT12 requires I2C1 bus driver"
#endif

#define PORT12_NUM	12	// number of port channels

#define PORT12_ADCREF	PORT12_NUM // virtual port with ADCref, measure internal reference 1.2V

#define PORT12_ADC_MAX	0xfff	// ADC max. value (= 4095)

// PORT12 channels:
//  0 ... IN, OUT, ADC
//  1 ... IN, OUT, ADC
//  2 ... IN, OUT, ADC
//  3 ... IN, OUT, ADC
//  4 ... IN, OUT, ADC
//  5 ... IN, OUT, ADC
//  6 ... IN, OUT, PWM1A
//  7 ... IN, OUT, PWM1B
//  8 ... IN, OUT, PWM2A
//  9 ... IN, OUT, PWM2B
//  10 ... IN, OUT
//  11 ... IN, OUT
//  12 ... virtual port with ADCref, measure internal reference 1.2V

// IO function
#define PORT12_FUNC_IN		0	// input (no pulls, default), returns value LOW=0 or HIGH=1 ... default function
#define PORT12_FUNC_IN_PD	1	// input with pull-down, returns value LOW=0 or HIGH=1
#define PORT12_FUNC_IN_PU	2	// input with pull-up, returns value LOW=0 or HIGH=1
#define PORT12_FUNC_BTN		3	// button input with pull-up and noise filter, returns value OFF=0 or ON=1
#define PORT12_FUNC_ADC		4	// analog input (only IO0..IO5), returns value 0 to 4095
#define PORT12_FUNC_ADCF	5	// analog input filtered (only IO0..IO5), returns value 0 to 4095
#define PORT12_FUNC_OUT		6	// output, value LOW=0 or HIGH=1
#define PORT12_FUNC_OD		7	// open-drain output, value LOW=0 or HIGH=1
#define PORT12_FUNC_PWM		8	// PWM analog output (only IO6-IO9), value 0 to LOOP1-1 (IO6 or IO7) or LOOP2-1 (IO8 or IO9)

// The PWM1A (channel 6) and PWM1B (channel 7) signals use the same divider and loop settings. Default div=1, loop=480, frequency = 100kHz.
// The PWM2A (channel 8) and PWM2B (channel 9) signals use the same divider and loop settings. Default div=1, loop=480, frequency = 100kHz.

#define PORT12_DIV_DEF		1	// PWM timer default divider
#define PORT12_LOOP_DEF		480	// PWM timer default loop

// PORT12 entries
#define PORT12_NUM_INX		13	// index of number of ports
#define PORT12_DIV1_INX		14	// index of PWM1 divider-1
#define PORT12_LOOP1_INX	16	// index of PWM1 loop-1
#define PORT12_DIV2_INX		18	// index of PWM2 divider-1
#define PORT12_LOOP2_INX	20	// index of PWM2 loop-1
#define PORT12_FUNC_INX		22	// index of array u8[12] of port functions IO_FUNC_*
#define PORT12_DATA_INX		34	// index of array u16[12] of port data
//#define PORT12_REF_INX	58	// index of ADCref data u16 (measure internal reference 1.2V)

// PORT12 device
class cPORT12
{
public:
	int	i2c;		// I2C bus index (0 or 1)
	int	addr;		// I2C address (default 0x38)
	int 	speed;		// transfer speed in Hz (usually 100000 to 400000)

	// initialize
	//  i2c ... I2C bus index (0 or 1)
	//  addr ... I2C address (default PORT12_ADDR=0x38)
	//  speed ... transfer speed in Hz (usually 100000 to 400000)
	void Init(int i2c = PORT12_I2C, int addr = PORT12_ADDR, int speed = PORT12_SPEED);

	// Check if PORT12 module is connected (returns False on error)
	Bool Check();

// PWM sample rate frequency f=48000000/(div+1)/(loop+1)
//  100 kHz: div=1-1, loop=480-1 (f = 48000000/480 = 100000) ... default setup
//  20 kHz: div=1-1, loop=2400-1 (f = 48000000/2400 = 20000)
//  2 kHz: div=1-1, loop=24000-1 (f = 48000000/24000 = 2000)
//  50 Hz: div=16-1, loop=60000-1 (f = 48000000/16/60000 = 50)

	// Setup timer of PWM channels PWM1A (port channel 6) and PWM1B (port channel 7). Returns False on error.
	//  div ... prescaler-1 (= div1-1), value 0..65535
	//  loop ... loader-1 (= loop1-1), value 0..65535
	//  Base frequency of the PWM1 f = 48000000/(div+1)/(loop+1) = 48000000/div1/loop1
	Bool SetupPWM1(u16 div, u16 loop);
	
	// Setup timer of PWM channels PWM2A (port channel 8) and PWM2B (port channel 9). Returns False on error.
	//  div ... prescaler-1 (= div2-1), value 0..65535
	//  loop ... loader-1 (= loop2-1), value 0..65535
	//  Base frequency of the PWM2 f = 48000000/(div+1)/(loop+1) = 48000000/div2/loop2
	Bool SetupPWM2(u16 div, u16 loop);

	// setup functions of port channels (return False on error)
	//  chan ... first port channel 0..11
	//  func ... array of functions PORT12_FUNC_*
	//  num ... number of channels 1..12
	// Functions PORT12_FUNC_ADC and PORT12_FUNC_ADCF are supported only on channels 0 to 5.
	// Function PORT12_FUNC_PWM is supported only on channels 6 to 9.
	// PWM channels 6 and 7 require common timer setup SetupPWM1().
	// PWM channels 8 and 9 require common timer setup SetupPWM2().
	Bool SetupFunc(int chan, const u8* func, int num);

	// set data to port channels (returns False on error)
	//  chan ... first port channel 0..11
	//  data ... array of port values (output: LOW=0, HIGH=1; PWM: 0 to LOOP value)
	//  num ... number of channels 1..12
	Bool SetData(int chan, const u16* data, int num);

	// get data from port channels (returns False on error)
	//  chan ... first port channel 0..11, or 12 = ADCref measure internal reference 1.2V
	//  data ... array of port values (input: LOW=0, HIGH=1; button: OFF=0, ON=1; ADC: 0 to 4095)
	//  num ... number of channels 1..13 (last index 12 is ADCref)
	Bool GetData(int chan, u16* data, int num);
};

#endif // _DRV_PORT12_H

#endif // USE_PORT12
