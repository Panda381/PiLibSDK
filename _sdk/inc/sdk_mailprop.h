
// ****************************************************************************
//
//               Mailbox properties (to communicate with VideCore)
//
// ****************************************************************************
// https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface
// - The response overwrites the request; callee is not allowed to return different buffer address.
// - The buffer is 16-byte aligned. Only upper 28 bits of the address can be passed via mailbox.
// - All values are in host CPU endian order.
// - Unknown tags are ignored.

#ifndef _SDK_MAILPROP_H
#define _SDK_MAILPROP_H

#ifdef __cplusplus
extern "C" {
#endif

// ==== Mailbox property: Device ID (used in "Get power state" tag 0x00020001)

#define MP_DEV_SDCARD		0	// SD card
#define MP_DEV_UART0		1	// UART0
#define MP_DEV_UART1		2	// UART1
#define MP_DEV_USB		3	// USB HCD
#define MP_DEV_I2C0		4	// I2C0
#define MP_DEV_I2C1		5	// I2C1
#define MP_DEV_I2C2		6	// I2C2
#define MP_DEV_SPI		7	// SPI
#define MP_DEV_CCP2TX		8	// CCP2TX
#define MP_DEV_DEV9		9	// dev 9 (RPi4)
#define MP_DEV_DEV10		10	// dev 10 (RPi4)

// ==== Mailbox property: Clock ID (used in "Get clock state" tag 0x00030001)

#define MP_CLK_EMMC		1	// EMMC
#define MP_CLK_UART		2	// UART
#define MP_CLK_ARM		3	// ARM
#define MP_CLK_CORE		4	// CORE
#define MP_CLK_V3D		5	// V3D
#define MP_CLK_H264		6	// H264
#define MP_CLK_ISP		7	// ISP
#define MP_CLK_SDRAM		8	// SDRAM
#define MP_CLK_PIXEL		9	// PIXEL
#define MP_CLK_PWM		10	// PWM
#define MP_CLK_HEVC		11	// HEVC
#define MP_CLK_EMMC2		12	// EMMC2
#define MP_CLK_M2MC		13	// M2MC
#define MP_CLK_PIXEL_BVB	14	// PIXEL BVB

// ==== Mailbox property: Voltage ID (used in "Get voltage" tag 0x00030003)

#define MP_VOLT_CORE		1	// CORE
#define MP_VOLT_SDRAM_C		2	// SDRAM C
#define MP_VOLT_SDRAM_P		3	// SDRAM P
#define MP_VOLT_SDRAM_I		4	// SDRAM I

// ==== Mailbox property: Memory flags

#define MEM_FLAG_DISCARDABLE	(1<<0)	// Discardable, can be resized to 0 at any time. Use for cached data
#define MEM_FLAG_NORMAL		(0<<2)	// normal allocating alias. Don't use from ARM
#define MEM_FLAG_DIRECT		(1<<2)	// base 0xC.. alias, uncached
#define MEM_FLAG_COHERENT	(2<<2)	// base 0x8.. alias, non-allocating in L2 but coherent
#define MEM_FLAG_L1_NONALLOCATING (3<<2) // allocating in L2
#define MEM_FLAG_ZERO		(1<<4)	// initialise buffer to all zeros
#define MEM_FLAG_NO_INIT	(1<<5)	// don't initialise (default is initialise to all ones)
#define MEM_FLAG_HINT_PERMALOCK	(1<<6)	// Likely to be locked for long periods of time.

// ==== Mailbox tags
// Unused bits should be set to 0.

#define TAG_END				0		// end tag

// Videocore
#define TAG_GET_FIRMWARE_VERSION	0x00001		// get firmware revision (returns date and time of firmware build in Unix format)
							//	request: length 0
							//	response: length 4; value: u32 firmware version

// Hardware
#define TAG_GET_BOARD_MODEL		0x10001		// get board model (returns 0 for most models - use board revision instead)
							//	request: length 0
							//	response: length 4; value: u32 board model
#define TAG_GET_BOARD_REVISION		0x10002		// get board revision
							//	request: length 0
							//	response: length 4; value: u32 board revision
							//      https://github.com/AndrewFromMelbourne/raspberry_pi_revision
							//		bit 0..3: revision 1.x (0 -> 1.0, 1 -> 1.1, 2 -> 2.2)
							//		bit 4..11: board model
							//			0 = Pi 1 A
							//			1 = Pi 1 B
							//			2 = Pi 1 A+
							//			3 = Pi 1 B+
							//			4 = Pi 2 B
							//			5 = Alpha
							//			6 = Pi CM 1
							//			8 = Pi 3 B
							//			9 = Pi Zero
							//			0x0A: Pi CM 3
							//			0x0C = Pi Zero W
							//			0x0D = Pi 3 B+
							//			0x0E = Pi 3 A+
							//			0x10 = Pi CM 3+
							//			0x11 = Pi 4 B
							//			0x12 = Pi Zero 2 W
							//			0x13 = Pi 400
							//			0x14 = Pi CM 4
							//			0x15 = Pi CM 4S
							//			0x17 = Pi 5
							//			0x18 = Pi CM 5
							//			0x19 = Pi 500
							//			0x1A = Pi CM 5 Lite
							//			0x1B = Pi CM 0
							//		bit 12..15: SoC (0 -> BCM2835, 1 -> BCM2836, 2 -> BCM2837/BCM2710A1, 3 -> BCM2711)
							//		bit 16..19: Manufacturer (0 -> Sony UK, 1 -> Egoman, 2 -> Embest, 4 -> Sony Japan)
							//		bit 20..22: RAM size (0 -> 256 MB, 1 -> 512 MB, 2 -> 1 GB, 3 -> 2 GB, 4 -> 4 GB, 5 -> 8 GB)
							//		bit 23: 1=new style of the revision
							//		bit 25: Warranty 1=Bit violated (overvolting)
							//	old style revision: https://gist.github.com/apetrone/5850426
							//		2 = Pi 1 B rev. 1.0
							//		3 = Pi 1 B rev. 1.0 + Fuses mod and D14 removed
							//		4 = Pi 1 B rev. 2.0 256 MB (Sony)
							//		5 = Pi 1 B rev. 2.0 256 MB (Qisda)
							//		6 = Pi 1 B rev. 2.0 256 MB (Egoman)
							//		7 = Pi 1 A rev. 2.0 256 MB (Egoman)
							//		8 = Pi 1 A rev. 2.0 256 MB (Sony)
							//		9 = Pi 1 A rev. 2.0 256 MB (Qisda)
							//		0x0D = Pi 1 B rev. 2.0 512 MB (Egoman)
							//		0x0E = Pi 1 B rev. 2.0 512 MB (Sony)
							//		0x0F = Pi 1 B rev. 2.0 512 MB (Qisda)
#define TAG_GET_BOARD_MAC_ADDRESS	0x10003		// get board MAC address
							//	request: length 0
							//	response: length 6; value: u8... MAC address in network byte order
#define TAG_GET_BOARD_SERIAL		0x10004		// get board serial
							//	request: length 0
							//	response: length 8; value: u64 board serial
#define TAG_GET_ARM_MEMORY		0x10005		// get ARM memory
							//	request: length 0
							//	response: length 8; value: u32 base address, u32 size in bytes
							//		future formats may specify multiply base+size combinations
#define TAG_GET_VC_MEMORY		0x10006		// get VC memory
							//	request: length 0
							//	response: length 8; value: u32 base address, u32 size in bytes
							//		future formats may specify multiply base+size combinations
#define TAG_GET_CLOCKS			0x10007		// get clocks
							//	request: length 0
							//	response: length variable (multiply of 8); value: u32 parent clock id (0=root),
							//		 u32 clock id, ... repeated (parent and child)
							//	Returns all clocks that exist in top down breadth first order.
							//	Clocks that depend on another clock should be defined as children of that clock.
							//	Clocks that depend on no other clocks should have no parent.
							//	Clock ID: see MP_CLK_* list above

// Config
#define TAG_GET_COMMAND_LINE		0x50001		// get command line
							//	request: length 0
							//	response: length variable; value: u8... ASCII command line string
							//	Caller should not assume the string is null terminated.

// Shared resource management
#define TAG_GET_DMA_CHANNELS		0x60001		// get DMA channels
							//	request: length 0
							//	response: length 4; value: u32 mask of DMA channels (bit 0..15: 1=DMA channel 0..15 can be used)
							//	Caller assumes that the VC has enabled all the usable DMA channels.

// Power
#define TAG_GET_POWER_STATE		0x20001		// get power state
							//	request: length 4; value: u32 device id (see MP_DEV_* above)
							//	response: length 8; value: u32 device id, u32 state
							//			bit 0: 1=on, 0=off
							//			bit 1: 1=device does not exist, 0=device exists
#define TAG_GET_TIMING			0x20002		// get timing
							//	request: length 4; value: u32 device id (see MP_DEV_* above)
							//	response: length 8; value: u32 device id, u32 wait time in us
							//	Response indicates wait time required after turning a device on before
							//	power is stable. Returns 0 wait time if the device does not exist.
							//		
#define TAG_SET_POWER_STATE		0x28001		// set power state
							//	request: length 8; value: u32 device id (see MP_DEV_* above), u32 state
							//			bit 0: 1=on, 0=off
							//			bit 1: 1=wait, 0=do not wait (wait for stable)
							//	response: length 8; value: u32 device id, u32 state
							//			bit 0: 1=on, 0=off
							//			bit 1: 1=device does not exist, 0=device exists
							//	Response indicates new state, with/without waiting for the power to become stable.

// Clocks
// All clocks are the base clocks for those peripherals, e.g. 3MHz for UART, 50/100MHz for EMMC, not the dividers applied using the peripheral.
#define TAG_GET_CLOCK_STATE		0x30001		// get clock state
							//	request: length 4; value: u32 clock id (see MP_CLK_* above)
							//	response: length 8; value: u32 clock id, u32 state
							//			bit 0: 1=on, 0=off
							//			bit 1: 1=clock does not exist, 0=clock exists
#define TAG_SET_CLOCK_STATE		0x38001		// set clock state
							//	request: length 8; value: u32 clock id (see MP_CLK_* above), u32 state
							//			bit 0: 1=on, 0=off
							//			bit 1: 1=clock does not exist, 0=clock exists
							//	response: length 8; value: u32 clock id, u32 state
							//			bit 0: 1=on, 0=off
							//			bit 1: 1=clock does not exist, 0=clock exists
#define TAG_GET_CLOCK_RATE		0x30002		// get clock rate
							//	request: length 4; value: u32 clock id (see MP_CLK_* above)
							//	response: length 8; value: u32 clock id, u32 rate in Hz (0=clock not exists)
#define TAG_GET_LED_STATUS		0x30041		// get onboard LED status
							//	request: length 0
							//	response: length 8; value: u32 pin number, u32 status 0 or 1
							//		pin number: 42 status LED, 130 power LED
#define TAG_TEST_LED_STATUS		0x34041		// test onboard LED status
							//	request: length 0
							//	response: length 8; value: u32 pin number, u32 status 0 or 1
							//		pin number: 42 status LED, 130 power LED
#define TAG_SET_LED_STATUS		0x38041		// set onboard LED status
							//	request: length 8; value: u32 pin number, u32 status 0 or 1
							//	response: length 8; value: u32 pin number, u32 status 0 or 1
							//		pin number: 42 status LED, 130 power LED
#define TAG_GET_CLOCK_RATE_MEASURED	0x30047		// get clock rate measured
							//	request: length 4; value: u32 clock id (see MP_CLK_* above)
							//	response: length 8; value: u32 clock id, u32 rate in Hz (0=clock not exists)
							//	Get the true/actual clock rate (instead of the last requested value returned
							//	by "Get clock rate"/0x0003002) which respects clamping, throttling and clock
							//	divider limitations.
#define TAG_SET_CLOCK_RATE		0x38002		// set clock rate
							//	request: length 12; value: u32 clock id (see MP_CLK_* above), u32 rate in Hz, u32 skip setting turbo
							//			1=skip turbo, disable turbo on freq above default
							//	response: length 8; value: u32 clock id, u32 rate in Hz (0=clock not exists)
#define TAG_GET_MAX_CLOCK_RATE		0x30004		// get max clock rate
							//	request: length 4; value: u32 clock id (see MP_CLK_* above)
							//	response: length 8; value: u32 clock id, u32 max rate in Hz
#define TAG_GET_MIN_CLOCK_RATE		0x30007		// get min clock rate
							//	request: length 4; value: u32 clock id (see MP_CLK_* above)
							//	response: length 8; value: u32 clock id, u32 min rate in Hz (this may be used when idle)
#define TAG_GET_TURBO			0x30009		// get turbo
							//	request: length 4; value: u32 id (should be 0)
							//	response: length 8; value: u32 id, u32 level (1=turbo, 0=non-turbo)
#define TAG_SET_TURBO			0x38009		// set turbo
							//	request: length 8; value: u32 id (should be 0), u32 level (1=turbo, 0=non-turbo)
							//	response: length 8; value: u32 id, u32 level (1=turbo, 0=non-turbo)
							//	This will cause GPU clocks to be set to maximum when enabled and minimum when disabled.

// Voltage
#define TAG_GET_VOLTAGE			0x30003		// get voltage
							//	request: length 4; value: u32 voltage id (see MP_VOLT_* above)
							//	response: length 8; value: u32 voltage id, u32 value (voltage in microvolts; 0x80000000=invalid id)
#define TAG_SET_VOLTAGE			0x38003		// set voltage
							//	request: length 8; value: u32 voltage id (see MP_VOLT_* above), u32 value
							//		value <= 16: relative to platform-specific typical voltage, in 25mV steps
							//		16 < value < 500000: relative to platform-specific typical voltage, in microvolts
							//		value >= 500000: absolute voltage in microvolts
							//	response: length 8; value: u32 voltage id, u32 value (voltage in microvolts; 0x80000000=invalid id)
#define TAG_GET_MAX_VOLTAGE		0x30005		// get max voltage
							//	request: length 4; value: u32 voltage id (see MP_VOLT_* above)
							//	response: length 8; value: u32 voltage id, u32 max value (voltage in microvolts)
#define TAG_GET_MIN_VOLTAGE		0x30008		// get min voltage
							//	request: length 4; value: u32 voltage id (see MP_VOLT_* above)
							//	response: length 8; value: u32 voltage id, u32 min value (voltage in microvolts)

// Misc
#define TAG_GET_TEMPERATURE		0x30006		// get temperature
							//	request: length 4; value: u32 temperature id (should be 0)
							//	response: length 8; value: u32 temperature id, u32 temperature in thousands of °C
#define TAG_GET_MAX_TEMPERATURE		0x3000A		// get max temperature
							//	request: length 4; value: u32 temperature id (should be 0)
							//	response: length 8; value: u32 temperature id, u32 max safe temperature in thousands of °C
#define TAG_ALLOCATE_MEMORY		0x3000C		// allocate memory
							//	request: length 12; value: u32 size, u32 alignment, u32 flags (see MP_MEM_FLAG_* above)
							//	response: length 4; value: u32 handle (0=error)
							//	Allocates contiguous memory on the GPU. size and alignment are in bytes.
#define TAG_LOCK_MEMORY			0x3000D		// lock memory
							//	request: length 4; value: u32 handle
							//	response: length 4; value: u32 bus address
							//	Lock buffer in place, and return a bus address. Must be done before memory can be accessed
#define TAG_UNLOCK_MEMORY		0x3000E		// unlock memory (It retains contents, but may move.)
							//	request: length 4; value: u32 handle
							//	response: length 4; value: u32 status (0=success)
#define TAG_RELEASE_MEMORY		0x3000F		// release memory
							//	request: length 4; value: u32 handle
							//	response: length 4; value: u32 status (0=success)
#define TAG_EXECUTE_CODE		0x30010		// execute code
							//	request: length 28; value: u32 function pointer, 6x u32 r0..r5
							//	response: length 4; value: u32 r0
							//	Calls the function at given (bus) address and with arguments
							//	given. E.g. r0 = fn(r0, r1, r2, r3, r4, r5); It blocks until
							//	call completes. The (GPU) instruction cache is implicitly
							//	flushed. Setting the lsb of function pointer address will
							//	suppress the instruction cache flush if you know the buffer
							//	hasn't changed since last execution.
#define TAG_GET_DISPMANX_MEM_HANDLE	0x30014		// Get Dispmanx Resource mem handle
							//	request: length 4; value: u32 dispmanx resource handle
							//	response: length 8; value: u32 0 if successful, u32 mem handle for resource
							//	Gets the mem_handle associated with a created dispmanx resource.
							//	This can be locked and the memory directly written from the arm
							//	to avoid having to copy the image data to GPU.
#define TAG_GET_EDID_BLOCK		0x30020		// Get EDID block
							//	request: length 4; value: u32 block number
							//	response: length 136; value: u32 block number, u32 status, 128 bytes EDID block
							//	This reads the specified EDID block from attached HDMI/DVI device.
							//	There will always be at least one block of 128 bytes, but there
							//	may be additional blocks. You should keep requesting blocks (starting
							//	from 0) until the status returned is non-zero.

// Framebuffer
// - All tags in the request are processed in one operation.
// - It is not valid to mix Test tags with Get/Set tags in the same operation and no tags will be returned.
// - Get tags will be processed after all Set tags.
// - If an allocate buffer tag is omitted when setting parameters, then no change occurs unless
//   it can be accommodated without changing the buffer base or size.
// - When an allocate buffer response is returned, the old buffer area (if the base or size has changed) is implicitly freed.
#define TAG_ALLOCATE_BUFFER		0x40001		// allocate frame buffer
							//	request: length 4; value: u32 alignment in bytes
							//	response: length 8; value: u32 frame buffer address, u32 frame buffer size in bytes
							//		if base and size is 0, than alignment not supported
#define TAG_RELEASE_BUFFER		0x48001		// release frame buffer
							//	request: length 0
							//	response: length 0
#define TAG_BLANK_SCREEN		0x40002		// blank screen
							//	request: length 4; value: u32 state (bit 0: 1=on, 0=off)
							//	response: length 4; value: u32 state (bit 0: 1=on, 0=off)
#define TAG_GET_PHYSICAL_SIZE		0x40003		// get physical width/height of display (=size of allocated buffer, not resolution of video)
							//	request: length 0
							//	response: length 8; value: u32 width in pixels, u32 height in pixels
#define TAG_TEST_PHYSICAL_SIZE		0x44003		// test physical width/height of display (does not modify current hardware or frame buffer state)
							//	request: length 8; value: u32 width in pixels, u32 height in pixels
							//	response: length 8; value: u32 width in pixels, u32 height in pixels (same as request if supported)
#define TAG_SET_PHYSICAL_SIZE		0x48003		// set physical width/height of display
							//	request: length 8; value: u32 width in pixels, u32 height in pixels
							//	response: length 8; value: u32 width in pixels, u32 height in pixels
							//		response may not be the same as request, need to check (0=unsupported)
#define TAG_GET_VIRTUAL_SIZE		0x40004		// get virtual width/height of buffer (=portion of buffer that is sent to display)
							//	request: length 0
							//	response: lengt 8; value: u32 width in pixels, u32 height in pixels
#define TAG_TEST_VIRTUAL_SIZE		0x44004		// test virtual width/height of buffer (does not modify current hardware or frame buffer state)
							//	request: length 8; value: u32 width in pixels, u32 height in pixels
							//	response: length 8; value: u32 width in pixels, u32 height in pixels (same as request if supported)
#define TAG_SET_VIRTUAL_SIZE		0x48004		// set virtual width/height of buffer
							//	request: length 8; value: u32 width in pixels, u32 height in pixels
							//	response: length 8; value: u32 width in pixels, u32 height in pixels
							//		response may not be the same as request, need to check (0=unsupported)
#define TAG_GET_DEPTH			0x40005		// get depth (bits per pixel)
							//	request: length 0
							//	response: length 4; value: u32 bits per pixel
#define TAG_TEST_DEPTH			0x44005		// test depth (does not modify current hardware or frame buffer state)
							//	request: length 4; value: u32 bits per pixel
							//	response: length 4; value: u32 bits per pixel (same as request if supported)
#define TAG_SET_DEPTH			0x48005		// set depth (bits per pixel)
							//	request: length 4; value: u32 bits per pixel
							//	response: length 4; value: u32 bits per pixel
							//		response may not be the same as request, need to check (0=unsupported)
#define TAG_GET_PIXEL_ORDER		0x40006		// get pixel order
							//	request: length 0
							//	response: length 4; value: u32 state (0=BGR, 1=RGB)
#define TAG_TEST_PIXEL_ORDER		0x44006		// test pixel order (does not modify current hardware or frame buffer state)
							//	request: length 4; value: u32 state (0=BGR, 1=RGB)
							//	response: length 4; value: u32 state (0=BGR, 1=RGB) (same as request if supported)
#define TAG_SET_PIXEL_ORDER		0x48006		// set pixel roder
							//	request: length 4; value: u32 state (0=BGR, 1=RGB)
							//	response: length 4; value: u32 state (0=BGR, 1=RGB)
							//		response may not be the same as request, need to check
#define TAG_GET_ALPHA_MODE		0x40007		// get alpha mode
							//	request: length 0
							//	response: length 4; value: u32 alpha state
							//		0=alpha channel enabled (0=fully opaque)
							//		1=alpha channel reversed (0=fully transparent)
							//		2=alpha channel ignored
#define TAG_TEST_ALPHA_MODE		0x44007		// test alpha mode (does not modify current hardware or frame buffer state)
							//	request: length 4; value: u32 state (0=alpha, 1=reversed, 2=ignored)
							//	response: length 4; value: u32 state (0=alpha, 1=reversed, 2=ignored) (same as request if supported)
#define TAG_SET_ALPHA_MODE		0x48007		// set alpha mode
							//	request: length 4; value: u32 state (0=alpha, 1=reversed, 2=ignored)
							//	response: length 4; value: u32 state (0=alpha, 1=reversed, 2=ignored)
							//		response may not be the same as request, need to check
#define TAG_GET_PITCH			0x40008		// get pitch
							//	request: length 0
							//	response: length 4; value: u32 bytes per line
#define TAG_GET_VIRTUAL_OFFSET		0x40009		// get virtual offset
							//	request: length 0
							//	response: length 8; value: u32 X in pixels, u32 Y in pixels
#define TAG_TEST_VIRTUAL_OFFSET		0x44009		// test virtual offset (does not modify current hardware or frame buffer state)
							//	request: length 8; value: u32 X in pixels, u32 Y in pixels
							//	response: length 8; value: u32 X in pixels, u32 Y in pixels (same as request if supported)
#define TAG_SET_VIRTUAL_OFFSET		0x48009		// set virtual offset
							//	request: length 8; value: u32 X in pixels, u32 Y in pixels
							//	response: length 8; value: u32 X in pixels, u32 Y in pixels
							//		response may not be the same as request, need to check (0=unsupported)
#define TAG_GET_OVERSCAN		0x4000A		// get overscan
							//	request: length 0
							//	response: length 16; value: u32 top, u32 bottom, u32 left, u32 right (in pixels)
#define TAG_TEST_OVERSCAN		0x4400A		// test overscan (does not modify current hardware or frame buffer state)
							//	request: length 16; value: u32 top, u32 bottom, u32 left, u32 right (in pixels)
							//	response: length 16; value: u32 top, u32 bottom, u32 left, u32 right (in pixels)
							//		response may not be the same as request, need to check
#define TAG_SET_OVERSCAN		0x4800A		// set overscan
							//	request: length 16; value: u32 top, u32 bottom, u32 left, u32 right (in pixels)
							//	response: length 16; value: u32 top, u32 bottom, u32 left, u32 right (in pixels)
							//		response may not be the same as request, need to check (0=unsupported)
#define TAG_GET_PALETTE			0x4000B		// get palette
							//	request: length 0
							//	response: length 1024; value: u32.. RGBA palette values (index 0..255)
#define TAG_TEST_PALETTE		0x4400B		// test palette (does not modify current hardware or frame buffer state)
							//	request: length 24..1032; value: u32 offset - first palette 0..255, u32 length - number of
							//		entries 1..256, u32.. RGBA palette values (offset to offset+length-1)
							//	response: length 4; value: u32 state 0=valid, 1=invalid
#define TAG_SET_PALETTE			0x4800B		// set palette
							//	request: length 24..1032; value: u32 offset - first palette 0..255, u32 length - number of
							//		entries 1..256, u32.. RGBA palette values (offset to offset+length-1)
							//	response: length 4; value: u32 state 0=valid, 1=invalid
#define TAG_SET_CURSOR_INFO		0x8010		// set cursor info
							//	request: length 24; value: u32 width, u32 height, u32 unused, u32 pointer to pixels, u32 hotspotX, u32 hotspotY
							//	response: length 4; value: u32 status 0=valid, 1=invalid
							//		format is 32bpp (ARGB), width and height should be >= 16, width*height should be <= 64
							//		default cursor 64x64 with hotspot 0,0
#define TAG_SET_CURSOR_STATE		0x8011		// set cursor state
							//	request: length 16; value: u32 enable 1=visible 0=invisible, u32 x, u32 y,
							//		u32 flags (0=display coords, 1=framebuffer coords)
							//	response: length 4; value: u32 state 0=valid, 1=invalid
#define TAG_SET_SCREEN_GAMMA		0x8012		// set screen gamma (Pi 3 only)
							//	request: length 8; value: u32 display number, u32 address of 768-byte gamma table
							//		(256 bytes red, 256 bytes green, 256 blue)
							//	response: length 0

/*
// Mailbox property tag
typedef struct PACKED {
	u32	tagID;		// 0x00: tag identifier
	u32	bufsize;	// 0x04: value buffer size in bytes (multiple of 4 - aligned to 32-bit word)
	u32	vallen;		// 0x08: request codes:
				//	bit 0..30: reserved
				//	bit 31: 0 = request
				// response codes:
				//	bit 0..30: value length in bytes
				//	bit 31: 1 = response
#define VALLEN_RESP B31	// flag - set to mark response
	u32	valbuf[0];	// 0x0C: value buffer
	//u8	padding[x];	// padding to align the tag to 32 bits

} sMailPropTag;

STATIC_ASSERT(sizeof(sMailPropTag) == 0x0C, "Incorrect InsMailPropTagterr_t!");
*/

#define MAILPROP_SIZE	1024	// size of mailbox property buffer (4 KB)

// Mailbox property tag buffer (32-bit words aligned to 16 bytes)
//	index 0: buffer size in bytes
//	index 1: on request 0=request, on response 0x80000000=response OK, 0x80000001=response error
//	... tags
//	... end tag 0
extern u32 COHERENT MailPropBuf[MAILPROP_SIZE] __attribute__((aligned(16)));
extern int MailPropInx;	// index into mailbox property buffer

// initialize mailbox property buffer
void MailPropInit(void);

// Add mailbox property tag to property buffer
void MailPropAdd(u32 tag, ...);

// Process mailbox property buffer
u32 MailPropProcess(void);

// Search property response in property buffer (returns NULL on error)
//  Returns pointer into property buffer: u32 number of bytes, data follows
u32* MailPropGet(u32 tag);

#ifdef __cplusplus
}
#endif

#endif // _SDK_MAILPROP_H
