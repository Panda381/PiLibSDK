
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

#define PWM1_DIV	1	// timer prescaler of PWM1 (to get 100 kHz from 48 MHz)
#define PWM1_LOOP	480	// timer looper of PWM1
#define PWM2_DIV	16	// timer prescaler of PWM2 (to get 50 Hz from 48 MHz)
#define PWM2_LOOP	60000	// timer looper of PWM2

cPORT12 Port12;

// channel functions
const u8 ChanFunc[PORT12_NUM] = {
	PORT12_FUNC_ADC,	//  0 ... IN, OUT, ADC (we will use ADC)
	PORT12_FUNC_ADC,	//  1 ... IN, OUT, ADC (we will use ADC)
	PORT12_FUNC_ADCF,	//  2 ... IN, OUT, ADC (we will use filtered ADC)
	PORT12_FUNC_ADCF,	//  3 ... IN, OUT, ADC (we will use filtered ADC)
	PORT12_FUNC_IN_PU,	//  4 ... IN, OUT, ADC (we will use INPUT with pull-up)
	PORT12_FUNC_IN_PD,	//  5 ... IN, OUT, ADC (we will use INPUT with pull-down)

	PORT12_FUNC_PWM,	//  6 ... IN, OUT, PWM1A (we will use PWM 100kHz)
	PORT12_FUNC_PWM,	//  7 ... IN, OUT, PWM1B (we will use PWM 100kHz)
	PORT12_FUNC_PWM,	//  8 ... IN, OUT, PWM2A (we will use PWM 50 Hz)
	PORT12_FUNC_PWM,	//  9 ... IN, OUT, PWM2B (we will use PWM 50 Hz)
	PORT12_FUNC_BTN,	//  10 ... IN, OUT (we will use BUTTON pull-up input)
	PORT12_FUNC_OUT,	//  11 ... IN, OUT (we will use OUTPUT)
				//  12 ... virtual port with ADCref, measure internal reference 1.2V
};

int main()
{
	Bool res;
	u8 out = 1;
	int i, vdd;
	u8 key;
	u16 adcref, data[6];

	// initialize (use default setup)
	Port12.Init();

	while (True)
	{
		// clear screen
		DrawClear();

		// check if port module is connected
		res = Port12.Check();
		if (res)
			printf("PORT12 module connected OK\n");
		else
			printf("PORT12 not found\n");

		// setup PWM Timers (PWM1: 100kHz, PWM2: 50Hz)
		res &= Port12.SetupPWM1(PWM1_DIV-1, PWM1_LOOP-1);	// 48000000/1/480 = 100000
		res &= Port12.SetupPWM2(PWM2_DIV-1, PWM2_LOOP-1);	// 48000000/16/60000 = 50

		// setup channel functions
		res &= Port12.SetupFunc(0, ChanFunc, PORT12_NUM);

		// read ADCref channel 1.2V
		adcref = 1489;
		res &= Port12.GetData(PORT12_ADCREF, &adcref, 1);
		vdd = 3300; // power supply voltage
		if ((adcref >= 1000) && (adcref < 2000))
		{
			vdd = (1200*4095 + adcref/2)/adcref;
			printf("ADCref=%d, Vdd=%.3fV\n", adcref, vdd*0.001);
		}
		else
			printf("ADCref=%d (Vdd unknown)\n", adcref);

		// read inputs, channels 0..5
		memset(data, 0, sizeof(data));
		res &= Port12.GetData(0, data, 6);
		printf("CH0 (ADC): %d, %.3fV\n", data[0], (double)data[0]*vdd/4095*0.001);
		printf("CH1 (ADC): %d, %.3fV\n", data[1], (double)data[1]*vdd/4095*0.001);
		printf("CH2 (ADCF): %d, %.3fV\n", data[2], (double)data[2]*vdd/4095*0.001);
		printf("CH3 (ADCF): %d, %.3fV\n", data[3], (double)data[3]*vdd/4095*0.001);
		printf("CH4 (IN_PU): %d\n", data[4]);
		printf("CH5 (IN_PD): %d\n", data[5]);

		// write PWM outputs, channels 6..9
		data[0] = RandU16Max(PWM1_LOOP-1);
		data[1] = RandU16Max(PWM1_LOOP-1);
		data[2] = RandU16Max(PWM2_LOOP-1);
		data[3] = RandU16Max(PWM2_LOOP-1);
		res &= Port12.SetData(6, data, 4);

		printf("CH6 (PWM1A): %d, %.3fV\n", data[0], (double)data[0]*vdd/(PWM1_LOOP-1)*0.001);
		printf("CH7 (PWM1B): %d, %.3fV\n", data[1], (double)data[1]*vdd/(PWM1_LOOP-1)*0.001);
		printf("CH8 (PWM2A): %d, %.3fV\n", data[2], (double)data[2]*vdd/(PWM2_LOOP-1)*0.001);
		printf("CH9 (PWM2B): %d, %.3fV\n", data[3], (double)data[3]*vdd/(PWM2_LOOP-1)*0.001);

		// read button input, channel 10
		data[0] = 0;
		res &= Port12.GetData(10, data, 1);
		printf("CH10 (BTN): %d=%s\n", data[0], (data[0] == 0) ? "OFF" : "ON");

		// write OUT, channel 11
		data[0] = out;
		res &= Port12.SetData(11, data, 1);
		printf("CH11 (OUT): %d\n", out);
		out = out ^ 1;

		// display update
		DispUpdate();

		// keyboard
		for (i = 20; i > 0; i--)
		{
			WaitMs(100);
			key = KeyGet();
			if (key == KEY_PAD_Y) Reboot();	// Program exit
			if (key == KEY_SCREENSHOT) ScreenShot(); //  Screenshot - This may take a few seconds to write.
		}
	}
}
