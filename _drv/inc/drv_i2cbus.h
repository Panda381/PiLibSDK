
// ****************************************************************************
//
//                            I2C bus driver
//
// ****************************************************************************

/*
0x00 ........ broadcaast
0x01-0x03 ... reserved
0x04-0x07 ... high-speed masters codes
0x08-0x1F ... not widely used; suitable for small modules
0x20-0x27 ... I/O expanders for matrix keyboards or text displays
0x30-0x37 ... suitable for custom modules
0x38-0x3F ... acceletometers, sensors, keyboards, displays
0x3C-0x3D ... OLED SSD1306
0x40-0x4F ... PWM controllers, ADC, temperature sensors
0x50-0x57 ... EEPROM
0x68-0x69 ... RTC clock, gyroscopes
0x70-0x77 ... barometers, I2C multiplexers
0x78-0x7B ... 10-bit addressing
0x7C-0x7F ... reserved, ID mode

most available addresses: 0x30-0x37, 0x60-0x67, 0x70-0x77

BarePi modules:
---------------
0x30 ... LCD SPI graphics display (LCD320x240, LCD160x80)
0x31 ... LCD text display (LCD16x2)
0x32 ... LED 7-segment display (LED12)
0x34 ... alphanumeric keyboard (MiniKey)
0x35 ... calculator keyboard (CalcKey)
0x38 ... PORT interface (Port module)
0x3C ... OLED display SSD1306
0x50-0x56 ... EEPROMs
0x57 ... configuration EEPROM 4KB AT24C32
0x68 ... RTC clock DS3231


List of usual I2C addresses
---------------------------
0x00 ........ reserved, general call address (broadcast)
0x01 ........ reserved for CBUS compatibility
0x02 ........ reserved for I2C-compatible bus variants
0x03 ........ reserved for future use
0x04-0x07 ... Reserved for HS-mode controller
0x0B ........ LC709203F Fuel Gauge and Battery Monitor
0x0C-0x0F ... MLX90393 3-Axis Magnetometer
0x0E ........ MAG3110 3-Axis Magnetometer
0x10 ........ PA1010D GPS Module, VEML6075 UV Sensor, VEML7700 Ambient Light Sensor
0x11, 0x63 .. Si4713 FM Transmitter with RDS
0x12 ........ PMSA0031 Gas Sensor
0x13 ........ VCNL40x0 Proximity Sensor
0x18 ........ MPRLS Pressure Sensor
0x18-0x19 ... LIS331 3-Axis Accelerometer, LIS3DH 3-Axis Accelerometer
0x18-0x1F ... MCP9808 Temperature Sensor
0x19 ........ LSM303 Accelerometer
0x1A ........ AGS02MA TVOC Gas Sensor
0x1C-0x1D ... MMA7455L, MMA845x 3-Axis Accelerometer
0x1C, 0x1E .. LIS3MDL Magnetometer
0x1C-0x1F ... FXOS8700 Accelerometer/Magnetometer
0x1D-0x1E ... LSM9DS0 9-Axis IMU Accel/Mag
0x1D, 0x53 .. ADXL343 3-Axis Accelerometer, ADXL345 3-Axis Accelerometer
0x1E ........ LSM303 Magnetometer, HMC5883 Magnetometer, LIS2MDL Magnetometer
0x20 ........ Chirp! Water Sensor
0x20-0x21 ... FXAS21002 Gyroscope
0x20-0x27 ... MCP23008 GPIO Expander, MCP23017 GPIO Expander
0x23, 0x5C .. BH1750 Light Sensor
0x24 ........ PN532 NFC/RFID reader
0x26 ........ MSA301 3-Axis Accelerometer
0x28-0x29 ... BNO055 IMU, TSL2591 Light Sensor
0x28-0x2B ... DS1841 Digital Logarithmic Potentiometer, DS3502 Digital 10K Potentiometer
0x28-0x2D ... CAP1188 8-Channel Capacitive Touch
0x28-0x2E ... PCT2075 Temperature Sensor
0x29 ........ TCS34725 Color Sensor, VL53L0x ToF Sensor, VL6180X ToF Sensor
0x30-0x3F ... Adafruit NeoKey 1x4 Four Mechanical Key Switches with NeoPixels,
		Adafruit NeoSlider Slide Potentiometer with 4 NeoPixels
0x33 ........ MLX90640 IR Thermal Camera
0x36 ........ MAX17048 LiPoly/LiIon Fuel Gauge and Battery Monitor
0x36-0x3D ... Adafruit Stemma QT Rotary Encoder with NeoPixel
0x38 ........ AHT20 Humidity/Temperature Sensor, DHT20 Humidity/Temperature Sensor,
		FT6x06 Capacitive Touch Driver, NCP5623 RGB LED Driver,
		NEWOPTO XYC-ALS21C-K1 Ambient light sensor
0x38-0x39 ... VEML6070 UV Index Light Sensor
0x39 ........ TSL2561 Light Sensor, APDS-9960 IR/Color/Proximity Sensor, AS7341 Color Sensor
0x3A ........ PCF8577C LCD direct/duplex driver (with parallel adressing)
0x3C-0x3D ... OLED controllers CH1115, CH1116, SSD1305, SSD1306, SSD1309, SSD1315, SH1106, SSD1327
0x3C-0x3F ... ST75256 4-Level Grayscale LCD
0x40 ........ HTU21D-F Humidity/Temperature Sensor, MS8607 Humidity/Temperature/Pressure Sensor - Humidity,
		Si7021 Humidity/Temperature Sensor
0x40-0x41 ... HTU31D Humidity/Temperature Sensor
0x40-0x43 ... HDC1008 Humidity/Temperature Sensor
0x40-0x47 ... TMP007 IR Temperature Sensor, TMP006 IR Temperature Sensor
0x40-0x4F ... PCA9685, INA219 High-Side DC Current/Voltage Sensor, INA260 Precision DC Current/Power Sensor
0x40-0x7F ... PCA9685 16-Channel PWM Driver
0x41, 0x44 .. STMPE610/STMPE811 Resistive Touch Controller
0x44 ........ ISL29125 Color Sensor, SHT45 Humidity/Temperature Sensor, SHT40 Humidity/Temperature Sensor
0x44-0x45 ... SHT31 Humidity/Temperature Sensor
0x48-0x4F ... PCT2075 Temperature Sensor, PCF8591 Quad 8-Bit ADC + 8-Bit DAC
0x48-0x4B ... ADS1115 4-channel 16-Bit ADC, ADT7410 Temperature Sensor, ATSAMD09 Breakout with seesaw,
		TMP102 Temperature Sensor, TMP117 Temperature Sensor
0x49 ........ AS7262 Light/Color Sensor, TSL2561 Light Sensor
0x4A-0x4B ... BNO085 9-DoF IMU
0x4C ........ Designer Systems pHAT-NBIOT multi-band LTE Cat NB1 module, EMC2101 Fan Controller
0x4D ........ Designer Systems pHAT-GSM pHAT-GSM quad-band GSM/GPRS module
0x50-0x53 ... Adafruit Mini I2C Gamepad with seesaw
0x50-0x57 ... MB85RC FRAM
0x50-0x5F ... 24LCxx series EEPROMs of various sizes and makes
0x52 ........ Nintendo Nunchuck Controller
0x53 ........ LTR390 UV Sensor
0x57 ........ MAX3010x Pulse & Oximetry Sensor
0x58 ........ SGP30 Gas Sensor, TPA2016 Class-D Audio Amplifier
0x58-0x5B ... AW9523 GPIO Expander and LED Driver
0x59 ........ SGP40 Gas Sensor
0x5A ........ DRV2605 Haptic Motor Driver, MLX9061x IR Temperature Sensor
0x5A-0x5B ... CCS811 VOC Sensor
0x5A-0x5D ... MPR121 12-Point Capacitive Touch Sensor
0x5C ........ AM2315 Humidity/Temp Sensor, AM2320 Humidity/Temp Sensor
0x5C-0x5D ... LPS22 Pressure Sensor, LPS25 Pressure Sensor, LPS33HW Ported Pressure Sensor,
		LPS35HW Pressure Sensor
0x5E ........ TLV493D 3-Axis Magnetometer
0x5F ........ HTS221 Humidity/Temperature Sensor
0x60 ........ ATECC608 Cryptographic Co-Processor, MCP4728 Quad DAC, Si1145 Light/IR Sensor,
		TEA5767 Radio Receiver, VCNL4040 Proximity and Ambient Light Sensor,
		MPL115A2 Barometric Pressure, MPL3115A2 Barometric Pressure
0x60-0x61 ... MCP4725A0 12-Bit DAC, Si5351A Clock Generator
0x60-0x67 ... MCP9600 Temperature Sensor
0x60-0x6F ... PCA9685
0x61 ........ SCD30 Humidity/Temperature/CO2 Sensor
0x62-0x63 ... MCP4725A1 12-Bit DAC
0x64-0x65 ... MCP4725A2 12-Bit DAC
0x68 ........ This address is really popular with real time clocks, almost all of them use 0x68!
0x68 ........ DS1307 RTC, DS3231 RTC, PCF8523 RTC
0x68-0x69 ... AMG8833 IR Thermal Camera Breakout, ICM-20649 Accelerometer + Gyroscope,
		ITG3200 Gyroscope, MPU-9250 9-DoF IMU, MPU-60X0 Accelerometer + Gyroscope
0x6A-0x6B ... ICM330DHC 6-Axis IMU, L3GD20H Gyroscope, LSM6DS33 6-Axis IMU,
		LSM6DSOX 6-Axis IMU, LSM9DS0 9-Axis IMU Gyro
0x70 ........ SHTC3 Temp and Humidity Sensor
0x70-0x77 ... PCT2075 Temperature Sensor, HT16K33 LED Matrix Driver, TCA9548 1-to-8 I2C Multiplexer
0x70-0x7F ... PCA9685 Shields, HATs, Breakouts, Wings etc
0x74-0x77 ... IS31FL3731 144-LED CharliePlex driver
0x76 ........ MS8607 Humidity/Temperature/Pressure Sensor - Temperature/Pressure
0x76-0x77 ... BME280 Temp/Barometric/Humidity, BME680 Temp/Barometric/Humidity/Gas,
		BMP280 Temp/Barometric, BMP388 Temp/Barometric,
		BMP390 Temp/Barometric, DPS310 Barometric Sensor,
		MS5607/MS5611 Barometric Pressure
0x77 ........ BMA180 Accelerometer, BMP180 Temp/Barometric, BMP085 Temp/Barometric,
0x78-0x7B ... Reserved for 10-bit I2C addressing
0x7C-0x7F ... Reserved for future purposes
*/

#if USE_I2CBUS		// 1=use I2C bus driver, 0=not used (drv_i2cbus.*)

#ifndef _DRV_I2CBUS_H
#define _DRV_I2CBUS_H

#include "../../_lib/inc/lib_list.h"

// I2C message status
#define I2CMSG_PENDING	1	// waiting for transaction
#define I2CMSG_RUN	2	// transfer is in progress
#define I2CMSG_DONE	3	// transaction was successful (callback() can add next message)
#define I2CMSG_LOCKED	4	// cannot start transfer, bus is locked
				//  - In this case, callback() must not add another 
				//    message to prevent operations from re-entering
#define I2CMSG_ERR	5	// transaction ACK error (callback() can add next message)
#define I2CMSG_CLKT	6	// transaction CLKT Clock Stretch Timeout error (callback() can add next message)
#define I2CMSG_TIMEOUT	7	// transaction time-out error (callback() can add next message)

// I2C message
typedef struct sI2Cmsg_ {
	sListEntry	list;		// list of messages (must be the first entry of this structure)
	void		(*callback)(struct sI2Cmsg_*); // callback (NULL=not used) - can add a new message,
					//	except when the status is I2CMSG_LOCKED
					//	When callback() is called, the message msg is removed from
					//	the queue (it can be sent again), the IRQ is disabled, and
					//	the spinlock is unlocked.
	void*		cookie;		// user's data
	u8*		data;		// pointer to message data, part 1 (ignored if len=0)
	int		len;		// number of data bytes to transfer, part 1
	u8*		data2;		// pointer to message data, part 2 (ignored if len2=0)
	int		len2;		// number of data bytes to transfer, part 2
	int		speed;		// transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
	volatile u8	status;		// message status I2CMSG_*
	u8		addr;		// device address on I2C bus
	Bool		read;		// True=read data, False=write data
} sI2Cmsg;

// check if transaction is done (returns True if transaction is done)
INLINE Bool I2Cbus_IsDone(sI2Cmsg* msg) { return msg->status >= I2CMSG_DONE; }

// I2C bus descriptor
typedef struct {
	sList		list;		// list of I2C messages (transaction is always in progress if the list is not empty)
	int		inx;		// index of the data
	int		i2cinx;		// I2C bus index
	I2C_t*		i2c;		// I2C interface
	volatile u32	start;		// start time of last byte
	volatile u8	lock;		// spin lock
} sI2Cbus;

// flag - I2C bus driver is enabled
extern Bool I2Cbus_IsUse[I2C_NUM];

// I2C bus
extern sI2Cbus I2Cbus[I2C_NUM];

// initialize I2C bus driver
// - The user code can access unused I2C bus using polled access.
void I2Cbus_Init(int i2c);

// terminate I2C bus driver
void I2Cbus_Term(int i2c);

// auto-initialize I2C bus driver, if not initialized
void I2Cbus_AutoInit(int i2c);

// I2C bus driver watchdog (called from systick handler)
void I2Cbus_Watchdog();

// add I2C message to message queue
//    i2c ... I2C index 0 or 1
//  Before the transmission begins, the following items must be initialized in the msg structure:
//    callback ... pointer to the callback function called after the transfer is complete (NULL=not used)
//    data ... pointer to data buffer to receive or send data, part 1 (ignored if len=0)
//    len ... length of data buffer, part 1 (number of data bytes to transfer)
//    data2 ... pointer to data buffer to receive or send data, part 2 (ignored if len2=0)
//    len2 ... length of data buffer, part 2 (number of data bytes to transfer)
//    speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
//    addr ... device address on I2C bus
//    read ... True=read data, False=write data
//  optionaly: cookie ... user's data
// Only "list" and "status" entries will be modified in the I2C bus driver.
// The status of the operation can be viewed in the "status" entry.
// The structure of the message must remain valid until the transmission is complete.
// This function can also be called from an IRQ interrupt handler, but it must not be called from a FIQ fast interrupt.
// This function can call callback() if transaction cannot start (on error I2CMSG_LOCKED).
// I2C bus driver is auto-initialized, if not initialized yet.
void I2Cbus_Add(int i2c, sI2Cmsg* msg);

// read data from I2C, using I2C bus driver, and wait (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  data ... data buffer, part 1 (can be NULL if len = 0)
//  len ... number of bytes 0..65535, part 1
//  data2 ... data buffer, part 2 (can be NULL if len2 = 0)
//  len2 ... number of bytes 0..65535, part 2
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
// Do not check device presence reading 0 bytes, it does not work.
// I2C bus driver is auto-initialized, if not initialized yet.
Bool I2Cbus_Read(int i2c, int addr, void* data, int len, void* data2 = NULL, int len2 = 0, int speed = 0);

// write data to I2C, using I2C bus driver, and wait (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  data ... data buffer, part 1 (can be NULL if len = 0)
//  len ... number of bytes 0..65535, part 1
//  data2 ... data buffer, part 2 (can be NULL if len2 = 0)
//  len2 ... number of bytes 0..65535, part 2
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
// The presence of a device can be tested by writing a data length of 0.
// I2C bus driver is auto-initialized, if not initialized yet.
Bool I2Cbus_Write(int i2c, int addr, const void* data, int len, const void* data2 = NULL, int len2 = 0, int speed = 0);

// check presence of the I2C device, using I2C bus driver (to scan devices on the bus; returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
// I2C bus driver is auto-initialized, if not initialized yet.
Bool I2Cbus_Check(int i2c, int addr, int speed = I2C_DEF_SPEED);

// read registers with 8-bit addressing (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  inx ... 8-bit index of first register
//  reg ... pointer to array of registers
//  num ... number of registers
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
Bool I2Cbus_ReadReg8(int i2c, int addr, int inx, u8* reg, int num = 1, int speed = 0);

// read registers with 16-bit addressing (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  inx ... 16-bit index of first register
//  reg ... pointer to array of registers
//  num ... number of registers
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
Bool I2Cbus_ReadReg16(int i2c, int addr, int inx, u8* reg, int num = 1, int speed = 0);

// read registers with 24-bit addressing (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  inx ... 24-bit index of first register
//  reg ... pointer to array of registers
//  num ... number of registers
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
Bool I2Cbus_ReadReg24(int i2c, int addr, int inx, u8* reg, int num = 1, int speed = 0);

// read registers with 32-bit addressing (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  inx ... 32-bit index of first register
//  reg ... pointer to array of registers
//  num ... number of registers
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
Bool I2Cbus_ReadReg32(int i2c, int addr, int inx, u8* reg, int num = 1, int speed = 0);

// write registers with 8-bit addressing (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  inx ... 8-bit index of first register
//  reg ... pointer to array of registers
//  num ... number of registers
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
Bool I2Cbus_WriteReg8(int i2c, int addr, int inx, const u8* reg, int num = 1, int speed = 0);

// write registers with 16-bit addressing (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  inx ... 16-bit index of first register
//  reg ... pointer to array of registers
//  num ... number of registers
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
Bool I2Cbus_WriteReg16(int i2c, int addr, int inx, const u8* reg, int num = 1, int speed = 0);

// write registers with 24-bit addressing (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  inx ... 24-bit index of first register
//  reg ... pointer to array of registers
//  num ... number of registers
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
Bool I2Cbus_WriteReg24(int i2c, int addr, int inx, const u8* reg, int num = 1, int speed = 0);

// write registers with 32-bit addressing (returns False on error)
//  i2c ... I2C peripheral 0..1
//  addr ... slave address 0..127
//  inx ... 32-bit index of first register
//  reg ... pointer to array of registers
//  num ... number of registers
//  speed ... transfer speed in Hz (usually 100000 to 400000, 0 = use last speed)
Bool I2Cbus_WriteReg32(int i2c, int addr, int inx, const u8* reg, int num = 1, int speed = 0);

#endif // _DRV_I2CBUS_H

#endif // USE_I2CBUS
