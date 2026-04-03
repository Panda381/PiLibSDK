
// ****************************************************************************
//
//                       GPIO - General Purpose I/O
//
// ****************************************************************************
// BCM2835, BCM2836, BCM2837: 54 GPIO pins
// BCM2711, BCM2712: 58 GPIO pins

#ifndef _SDK_GPIO_H
#define _SDK_GPIO_H

#if RASPPI <= 3
#define PIN_NUM		54	// number of pins (pin index = 0..PIN_NUM-1)
#else
#define PIN_NUM		58	// number of pins (pin index = 0..PIN_NUM-1)
#endif

// pin function select (registers GPFSELn)
#define GPIO_FUNC_IN	0	// input (default after reset)
#define GPIO_FUNC_OUT	1	// output
#define GPIO_FUNC_AF0	4	// alternate function 0
#define GPIO_FUNC_AF1	5	// alternate function 1
#define GPIO_FUNC_AF2	6	// alternate function 2
#define GPIO_FUNC_AF3	7	// alternate function 3
#define GPIO_FUNC_AF4	3	// alternate function 4
#define GPIO_FUNC_AF5	2	// alternate function 5

#define GPIO_FUNC_MASK	7	// mask bits of pin function select

#if RASPPI <= 3
// pull-up/pull-down setup (register GPPUD)
#define GPIO_PULL_OFF	0	// disable pull-up/down
#define GPIO_PULL_DOWN	1	// enable pull-down
#define GPIO_PULL_UP	2	// enable pull-up
#define GPIO_PULL_RES	3	// reserved
#else
// pull-up/pull-down setup (registers GPPUPPDN)
#define GPIO_PULL_OFF	0	// disable pull-up/down
#define GPIO_PULL_UP	1	// enable pull-up
#define GPIO_PULL_DOWN	2	// enable pull-down
#define GPIO_PULL_RES	3	// reserved
#endif

#define GPIO_PULL_MASK	3	// pull-up/down mask

// pin event detect mode
#define GPIO_EVENT_NONE		0	// no detection
#define GPIO_EVENT_RISE		B0	// rising edge
#define GPIO_EVENT_FALL		B1	// falling edge
#define GPIO_EVENT_HIGH		B2	// high value
#define GPIO_EVENT_LOW		B3	// low value
#define GPIO_EVENT_ARISE	B4	// asynchronous rising edge
#define GPIO_EVENT_AFALL	B5	// asynchronous falling edge

/*
GPIO alternate functions, RASPPI <= 3
------------------------------------
https://pinout.xyz/
* = pin is not accessible at the pin header

GPIO0 and GPIO1 pins: These pins are reserved for ID EEPROM. At boot time this I2C interface will be
interrogated to look for an EEPROM that identifes the attached board and allows automagic setup of
the GPIOs (and optionally, Linux drivers). DO NOT USE these pins for anything other than attaching
an I2C ID EEPROM. Leave unconnected if ID EEPROM not required. 

	Pull	ALT0		ALT1		ALT2		ALT3		ALT4		ALT5		usual function
GPIO0	High	SDA0		SA5		-		-		-		-		I2C0 SDA, ID EEPROM HAT
GPIO1	High	SCL0		SA4		-		-		-		-		I2C0 SCL, ID EEPROM HAT
GPIO2	High	SDA1		SA3		-		-		-		-		I2C1 SDA
GPIO3	High	SCL1		SA2		-		-		-		-		I2C1 SCL
GPIO4	High	GPCLK0		SA1		-		-		-		ARM_TDI		clock0
GPIO5	High	GPCLK1		SA0		-		-		-		ARM_TDO		clock1
GPIO6	High	GPCLK2		SOE_N/SE	-		-		-		ARM_RTCK	clock2
GPIO7	High	SPI0_CE1_N	SWE_N/SRW_N	-		-		-		-		SPI0 CE1
GPIO8	High	SPI0_CE0_N	SD0		-		-		-		-		SPI0 CE0
GPIO9	Low	SPI0_MISO	SD1		-		-		-		-		SPI0 MISO
GPIO10	Low	SPI0_MOSI	SD2		-		-		-		-		SPI0 MOSI
GPIO11	Low	SPI0_SCLK	SD3		-		-		-		-		SPI0 SCLK
GPIO12	Low	PWM0		SD4		-		-		-		ARM_TMS		PWM0 audio L
GPIO13	Low	PWM1		SD5		-		-		-		ARM_TCK		PWM1 audio R
GPIO14	Low	TXD0		SD6		-		-		-		TXD1		UART0 TX, UART1 TX
GPIO15	Low	RXD0		SD7		-		-		-		RXD1		UART0 RX, UART1 RX
GPIO16	Low	-		SD8		-		CTS0		SPI1_CE2_N	CTS1		SPI1 CE2
GPIO17	Low	-		SD9		-		RTS0		SPI1_CE1_N	RTS1		SPI1 CE1
GPIO18	Low	PCM_CLK		SD10		-		BSCSL_SDA/MOSI	SPI1_CE0_N	PWM0		SPI1 CE0, PWM0 audio L
GPIO19	Low	PCM_FS		SD11		-		BSCSL_SCL/SCLK	SPI1_MISO	PWM1		SPI1 MISO, PWM1 audio R
GPIO20	Low	PCM_DIN		SD12		-		BSCSL/MISO	SPI1_MOSI	GPCLK0		SPI1 MOSI, clock0
GPIO21	Low	PCM_DOUT	SD13		-		BSCSL/CE_N	SPI1_SCLK	GPCLK1		SPI1 SCLK, clock1
GPIO22	Low	-		SD14		-		SD1_CLK		ARM_TRST	-		SD card CLK
GPIO23	Low	-		SD15		-		SD1_CMD		ARM_RTCK	-		SD card CMD
GPIO24	Low	-		SD16		-		SD1_DAT0	ARM_TDO		-		SD card DAT0
GPIO25	Low	-		SD17		-		SD1_DAT1	ARM_TCK		-		SD card DAT1
GPIO26	Low	-		-		-		SD1_DAT2	ARM_TDI		-		SD card DAT2
GPIO27	Low	-		-		-		SD1_DAT3	ARM_TMS		-		SD card DAT3
*GPIO28	-	SDA0		SA5		PCM_CLK		-		-		-		Ethernet
*GPIO29	-	SCL0		SA4		PCM_FS		-		-		-		Ethernet
*GPIO30	Low	-		SA3		PCM_DIN		CTS0		-		CTS1		Bluetooth UART0
*GPIO31	Low	-		SA2		PCM_DOUT	RTS0		-		RTS1		Bluetooth UART0
*GPIO32	Low	GPCLK0		SA1		-		TXD0		-		TXD1		Bluetooth UART0
*GPIO33	Low	-		SA0		-		RXD0		-		RXD1		Bluetooth UART0
*GPIO34	High	GPCLK0		SOE_N/SE	-		-		-		-		WiFi SD1
*GPIO35	High	SPI0_CE1_N	SWE_N/SRW_N	-		-		-		-		WiFi SD1
*GPIO36	High	SPI0_CE0_N	SD0		TXD0		-		-		-		WiFi SD1
*GPIO37	Low	SPI0_MISO	SD1		RXD0		-		-		-		WiFi SD1
*GPIO38	Low	SPI0_MOSI	SD2		RTS0		-		-		-		WiFi SD1
*GPIO39	Low	SPI0_SCLK	SD3		CTS0		-		-		-		WiFi SD1
*GPIO40	Low	PWM0		SD4		-		-		SPI2_MISO	TXD1		Flash SPI2, audio jack right channel
*GPIO41	Low	PWM1		SD5		-		-		SPI2_MOSI	RXD1		Flash SPI2, audio jack left channel
*GPIO42	Low	GPCLK1		SD6		-		-		SPI2_SCLK	RTS1		Flash SPI2
*GPIO43	Low	GPCLK2		SD7		-		-		SPI2_CE0_N	CTS1		Flash SPI2
*GPIO44	-	GPCLK1		SDA0		SDA1		-		SPI2_CE1_N	-
*GPIO45	-	PWM1		SCL0		SCL1		-		SPI2_CE2_N	-
*GPIO46	High	-		-		-		-		-		-		eMMC SD0 memory
*GPIO47	High	-		-		-		-		-		-               eMMC SD0 memory
*GPIO48	High	-		-		-		-		-		-		Ethernet, SD card CLK
*GPIO49	High	-		-		-		-		-		-		Ethernet, SD card CMD
*GPIO50	High	-		-		-		-		-		-		Ethernet, SD card DAT0
*GPIO51	High	-		-		-		-		-		-		Ethernet, SD card DAT1
*GPIO52	High	-		-		-		-		-		-		Ethernet, SD card DAT2
*GPIO53	High	-		-		-		-		-		-		Ethernet, SD card DAT3


GPIO alternate functions, RASPPI >= 4
------------------------------------
* = pin is not accessible at the pin header
	Pull	ALT0		ALT1		ALT2		ALT3		ALT4		ALT5
GPIO0	High	SDA0		SA5		PCLK		SPI3_CE0_N	TXD2		SDA6
GPIO1	High	SCL0		SA4		DE		SPI3_MISO	RXD2		SCL6
GPIO2	High	SDA1		SA3		LCD_VSYNC	SPI3_MOSI	CTS2		SDA3
GPIO3	High	SCL1		SA2		LCD_HSYNC	SPI3_SCLK	RTS2		SCL3
GPIO4	High	GPCLK0		SA1		DPI_D0		SPI4_CE0_N	TXD3		SDA3
GPIO5	High	GPCLK1		SA0		DPI_D1		SPI4_MISO	RXD3		SCL3
GPIO6	High	GPCLK2		SOE_N/SE	DPI_D2		SPI4_MOSI	CTS3		SDA4
GPIO7	High	SPI0_CE1_N	SWE_N/SRW_N	DPI_D3		SPI4_SCLK	RTS3		SCL4
GPIO8	High	SPI0_CE0_N	SD0		DPI_D4		BSCSL/CE_N	TXD4		SDA4
GPIO9	Low	SPI0_MISO	SD1		DPI_D5		BSCSL/MISO	RXD4		SCL4
GPIO10	Low	SPI0_MOSI	SD2		DPI_D6		BSCSL_SDA/MOSI	CTS4		SDA5
GPIO11	Low	SPI0_SCLK	SD3		DPI_D7		BSCSL_SCL/SCLK	RTS4		SCL5
GPIO12	Low	PWM0_0		SD4		DPI_D8		SPI5_CE0_N	TXD5		SDA5
GPIO13	Low	PWM0_1		SD5		DPI_D9		SPI5_MISO	RXD5		SCL5
GPIO14	Low	TXD0		SD6		DPI_D10		SPI5_MOSI	CTS5		TXD1
GPIO15	Low	RXD0		SD7		DPI_D11		SPI5_SCLK	RTS5		RXD1
GPIO16	Low	-		SD8		DPI_D12		CTS0		SPI1_CE2_N	CTS1
GPIO17	Low	-		SD9		DPI_D13		RTS0		SPI1_CE1_N	RTS1
GPIO18	Low	PCM_CLK		SD10		DPI_D14		SPI6_CE0_N	SPI1_CE0_N	PWM0_0
GPIO19	Low	PCM_FS		SD11		DPI_D15		SPI6_MISO	SPI1_MISO	PWM0_1
GPIO20	Low	PCM_DIN		SD12		DPI_D16		SPI6_MOSI	SPI1_MOSI	GPCLK0
GPIO21	Low	PCM_DOUT	SD13		DPI_D17		SPI6_SCLK	SPI1_SCLK	GPCLK1
GPIO22	Low	SD0_CLK		SD14		DPI_D18		SD1_CLK		ARM_TRST	SDA6
GPIO23	Low	SD0_CMD		SD15		DPI_D19		SD1_CMD		ARM_RTCK	SCL6
GPIO24	Low	SD0_DAT0	SD16		DPI_D20		SD1_DAT0	ARM_TDO		SPI3_CE1_N
GPIO25	Low	SD0_DAT1	SD17		DPI_D21		SD1_DAT1	ARM_TCK		SPI4_CE1_N
GPIO26	Low	SD0_DAT2	-		DPI_D22		SD1_DAT2	ARM_TDI		SPI5_CE1_N
GPIO27	Low	SD0_DAT3	-		DPI_D23		SD1_DAT3	ARM_TMS		SPI6_CE1_N
*GPIO28	-	SDA0		SA5		PCM_CLK		-		MII_A_RX_ERR	RGMII_MDIO
*GPIO29	-	SCL0		SA4		PCM_FS		-		MII_A_TX_ERR	RGMII_MDC
*GPIO30	Low	-		SA3		PCM_DIN		CTS0		MII_A_CRS	CTS1
*GPIO31	Low	-		SA2		PCM_DOUT	RTS0		MII_A_COL	RTS1
*GPIO32	Low	GPCLK0		SA1		-		TXD0		SD_CARD_PRE	TXD1
*GPIO33	Low	-		SA0		-		RXD0		SD_CARD_WRPROT	RXD1
*GPIO34	High	GPCLK0		SOE_N/SE	-		SD1_CLK		SD_CARD_LED	RGMII_IRQ
*GPIO35	High	SPI0_CE1_N	SWE_N/SRW_N	-		SD1_CMD		RGMII_START_STOP -
*GPIO36	High	SPI0_CE0_N	SD0		TXD0		SD1_DAT0	RGMII_RX_OK	MII_A_RX_ERR
*GPIO37	Low	SPI0_MISO	SD1		RXD0		SD1_DAT1	RGMII_MDIO	MII_A_TX_ERR
*GPIO38	Low	SPI0_MOSI	SD2		RTS0		SD1_DAT2	RGMII_MDC	MII_A_CRS
*GPIO39	Low	SPI0_SCLK	SD3		CTS0		SD1_DAT3	RGMII_IRQ	MII_A_COL
*GPIO40	Low	PWM1_0		SD4		-		SD1_DAT4	SPI0_MISO	TXD1
*GPIO41	Low	PWM1_1		SD5		-		SD1_DAT5	SPI0_MOSI	RXD1
*GPIO42	Low	GPCLK1		SD6		-		SD1_DAT6	SPI0_SCLK	RTS1
*GPIO43	Low	GPCLK2		SD7		-		SD1_DAT7	SPI0_CE0_N	CTS1
*GPIO44	-	GPCLK1		SDA0		SDA1		-		SPI0_CE1_N	SD_CARD_COLT
*GPIO45	-	PWM0_1		SCL0		SCL1		-		SPI0_CE2_N	SD_CARD_PWR0
*GPIO46	High	-		-		-		-		-		-
*GPIO47	High	-		-		-		-		-		-
*GPIO48	High	-		-		-		-		-		-
*GPIO49	High	-		-		-		-		-		-
*GPIO50	High	-		-		-		-		-		-
*GPIO51	High	-		-		-		-		-		-
*GPIO52	High	-		-		-		-		-		-
*GPIO53	High	-		-		-		-		-		-
*GPIO54	High	-		-		-		-		-		-
*GPIO55	High	-		-		-		-		-		-
*GPIO56	High	-		-		-		-		-		-
*GPIO57	High	-		-		-		-		-		-

SDAn, SCLn:		BSC master
GPCLKn:			General purpose clock (TBD)
SPIn_*:			SPI
PWMn:			Pulse Width Modulator
TXDn, RXDn, CTSn, RTSn:	UART
PCM_*:			PCM Audio 
SAn, SOE_N/SE, S...:	Secondary memory interface
BSCSCL*:		BSC/SPI slave
ARM_*:			ARM JTAG (TBD)
PCLK, DE, LCD*, DPI_Dn:	Display parallel interface

*/

// GPIO port
typedef struct {
	// GPIO function select (3 bits per pin, upper 2 bits are reserved)
	union {
		io32	GPFSEL[6];		// 0x00: GPIO function select #0..#5
		struct {
			io32	GPFSEL0;	// 0x00: GPIO function select #0, pin 0..9 (bits 30..31 are reserved)
			io32	GPFSEL1;	// 0x04: GPIO function select #1, pin 10..19 (bits 30..31 are reserved)
			io32	GPFSEL2;	// 0x08: GPIO function select #2, pin 20..29 (bits 30..31 are reserved)
			io32	GPFSEL3;	// 0x0C: GPIO function select #3, pin 30..39 (bits 30..31 are reserved)
			io32	GPFSEL4;	// 0x10: GPIO function select #4, pin 40..49 (bits 30..31 are reserved)
			io32	GPFSEL5;	// 0x14: GPIO function select #5, pin 50..57 (bits 24..31 are reserved)
		};
	};
	io32	res1;			// 0x18: ... reserved

	// GPIO pin output set (write only; write 1 to set pin to HIGH state, write 0 has no effect)
	io32	GPSET0;			// 0x1C: GPIO pin output set #0, pins 0..31
	io32	GPSET1;			// 0x20: GPIO pin output set #1, pins 32..57
	io32	res2;			// 0x24: ... reserved

	// GPIO pin output clear (write only; write 1 to set pin to LOW state, write 0 has no effect)
	io32	GPCLR0;			// 0x28: GPIO pin output clear #0, pins 0..31
	io32	GPCLR1;			// 0x2C: GPIO pin output clear #1, pins 32..57
	io32	res3;			// 0x30: ... reserved

	// GPIO pin input level (read only)
	io32	GPLEV0;			// 0x34: GPIO pin input level #0, pins 0..31
	io32	GPLEV1;			// 0x38: GPIO pin input level #1, pins 32..57
	io32	res4;			// 0x3C: ... reserved

	// GPIO pin event detect status (read, write 1 to clear)
	// - set to 1 by hw in case of edge/level event detection
	// - write 1 by sw to clear
	io32	GPEDS0;			// 0x40: GPIO pin event detect status #0, pins 0..31
	io32	GPEDS1;			// 0x44: GPIO pin event detect status #1, pins 32..57
	io32	res5;			// 0x48: ... reserved

	// GPIO pin rising edge detect enable (1=rising edge will set bit in GPEDSn)
	io32	GPREN0;			// 0x4C: GPIO pin rising edge detect enable #0, pins 0..31
	io32	GPREN1;			// 0x50: GPIO pin rising edge detect enable #1, pins 32..57
	io32	res6;			// 0x54: ... reserved
	
	// GPIO pin falling edge detect enable (1=falling edge will set bit in GPEDSn)
	io32	GPFEN0;			// 0x58: GPIO pin falling edge detect enable #0, pins 0..31
	io32	GPFEN1;			// 0x5C: GPIO pin falling edge detect enable #1, pins 32..57
	io32	res7;			// 0x60: ... reserved

	// GPIO pin high level detect enable (1=high level will set bit in GPEDSn)
	io32	GPHEN0;			// 0x64: GPIO pin high level detect enable #0, pins 0..31
	io32	GPHEN1;			// 0x68: GPIO pin high level detect enable #1, pins 32..57
	io32	res8;			// 0x6C: ... reserved

	// GPIO pin low level detect enable (1=low level will set bit in GPEDSn)
	io32	GPLEN0;			// 0x70: GPIO pin low level detect enable #0, pins 0..31
	io32	GPLEN1;			// 0x74: GPIO pin low level detect enable #1, pins 32..57
	io32	res9;			// 0x78: ... reserved

	// GPIO pin asynchronous rising edge detect enable (1=asynchronous rising edge will set bit in GPEDSn)
	// - Asynchronous means the incoming signal is not sampled by the system clock.
	//   As such rising edges of very short duration can be detected.
	io32	GPAREN0;		// 0x7C: GPIO pin asynchronous rising edge detect enable #0, pins 0..31
	io32	GPAREN1;		// 0x80: GPIO pin asynchronous rising edge detect enable #1, pins 32..57
	io32	res10;			// 0x84: ... reserved

	// GPIO pin asynchronous falling edge detect enable (1=asynchronous falling edge will set bit in GPEDSn)
	// - Asynchronous means the incoming signal is not sampled by the system clock.
	//   As such falling edges of very short duration can be detected.
	io32	GPAFEN0;		// 0x88: GPIO pin asynchronous falling edge detect enable #0, pins 0..31
	io32	GPAFEN1;		// 0x8C: GPIO pin asynchronous falling edge detect enable #1, pins 32..57
	io32	res11;			// 0x90: ... reserved

#if RASPPI <= 3

	// GPIO pins global pull-up/pull-down (bit 0..1: set GPIO_PULL_* value)
	io32	GPPUD;			// 0x94: bit 0..1: global pulls setup - set GPIO_PULL_OFF value
					//	 bit 2..31: reserved

	// GPIO pin pull-up/pull-down clock (1=assert clockd on line 'n')
	// Controls actuation of internal pull-down of GPIO pins. Use sequence:
	//	1. Write GPPUD to set requied control signal (to set pull-up/pull-down)
	//	2. Wait 150 cycles
	//	3. Write GPPUDCLK to clock control signal into GPIO pads to be modified
	//		- Only pins which receive clock will be modified. All others will retain old state.
	//	4. Wait 150 cycles
	//	5. Write GPPUD to remove control signal
	//	6. Write GPPUDCLK to remove clock.
	io32	GPPUDCLK0;		// 0x98: GPIO pin pull-up/pull-down clock #0, pins 0..31
	io32	GPPUDCLK1;		// 0x9C: GPIO pin pull-up/pull-down clock #1, pins 32..53
	io32	res12;			// 0xA0: ... reserved

	// Test
	io32	TEST;			// 0xA4: test (4 bits)
#else //  RASPPI <= 3
	io32	res13[15];		// 0x94: ... reserved
	io32	GPPINMUXSD;		// 0xD0:
	io32	res14[4];		// 0xD4: ... reserved

	// GPIO setup pull-up/pull-down (2 bits per pin: set GPIO_PULL_* value)
	union {
		io32	GPPUPPDN[4];	// 0xE4: GPIO pull-up/pull-down setup #0..#3
		struct {
			io32	GPPUPPDN0;	// 0xE4: GPIO pull-up/pull-down setup #0, pin 0..15
			io32	GPPUPPDN1;	// 0xE8: GPIO pull-up/pull-down setup #1, pin 16..31
			io32	GPPUPPDN2;	// 0xEC: GPIO pull-up/pull-down setup #2, pin 32..47
			io32	GPPUPPDN3;	// 0xF0: GPIO pull-up/pull-down setup #3, pin 48..57
		};
	};
#endif //  RASPPI <= 3

} GPIO_t;

#if RASPPI <= 3
STATIC_ASSERT(sizeof(GPIO_t) == 0xA8, "Incorrect GPIO_t!");
#else
STATIC_ASSERT(sizeof(GPIO_t) == 0xF4, "Incorrect GPIO_t!");
#endif

// GPIO interface
#define GPIO	((GPIO_t*)ARM_GPIO_BASE)

// set GPIO function GPIO_FUNC_*
//  - This function is not thread-safe, but it is IRQ/FIQ interrupt save
void GPIO_Func(int pin, int func);

// set GPIO pull-up GPIO_PULL_*
//  - This function is not thread-safe
void GPIO_Pull(int pin, int pull);

// output 0 to GPIO
INLINE void GPIO_Out0(int pin)
{
	if (pin < 32)
		GPIO->GPCLR0 = BIT(pin);
	else
		GPIO->GPCLR1 = BIT(pin - 32);
}

// output 1 to GPIO
INLINE void GPIO_Out1(int pin)
{
	if (pin < 32)
		GPIO->GPSET0 = BIT(pin);
	else
		GPIO->GPSET1 = BIT(pin - 32);
}

// output value to GPIO (val = 0 or <>0)
INLINE void GPIO_Out(int pin, int val)
{
	if (val != 0)
		GPIO_Out1(pin);
	else
		GPIO_Out0(pin);
}

// output more pins masked (pins 0..31)
INLINE void GPIO_OutMask(u32 val, u32 mask)
{
	GPIO->GPSET0 = val & mask;
	GPIO->GPCLR0 = (~val) & mask;
}

// output more pins masked (pins 32..57)
INLINE void GPIO_OutMask2(u32 val, u32 mask)
{
	GPIO->GPSET1 = val & mask;
	GPIO->GPCLR1 = (~val) & mask;
}

// input GPIO pin (returns 0 or 1)
INLINE u8 GPIO_In(int pin)
{
	if (pin < 32)
		return (u8)((GPIO->GPLEV0 >> pin) & 1);
	else
		return (u8)((GPIO->GPLEV1 >> (pin - 32)) & 1);
}

// flip GPIO output pin
//  - This function is not thread-safe
INLINE void GPIO_Flip(int pin)
{
	if (pin < 32)
	{
		u32 m = BIT(pin);
		u32 d = GPIO->GPLEV0 & m;
		GPIO->GPCLR0 = d;
		d ^= m;
		GPIO->GPSET0 = d;
	}
	else
	{
		u32 m = BIT(pin - 32);
		u32 d = GPIO->GPLEV1 & m;
		GPIO->GPCLR1 = d;
		d ^= m;
		GPIO->GPSET1 = d;
	}
}

// get pin event detect status (returns True of event is set)
INLINE Bool GPIO_Event(int pin)
{
	if (pin < 32)
		return (Bool)(((GPIO->GPEDS0 >> pin) & 1) != 0);
	else
		return (Bool)(((GPIO->GPEDS1 >> (pin - 32)) & 1) != 0);
}

// clear pin event detect status
INLINE void GPIO_ClrEvent(int pin)
{
	if (pin < 32)
		GPIO->GPEDS0 = BIT(pin);
	else
		GPIO->GPEDS1 = BIT(pin - 32);
}

// set pin detect mode (mode = combination of GPIO_EVENT_* flags)
//  - This function is not thread-safe
void GPIO_Detect(int pin, int mode);

#endif // _SDK_GPIO_H
