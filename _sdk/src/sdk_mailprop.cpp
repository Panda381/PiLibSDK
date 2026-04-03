
// ****************************************************************************
//
//               Mailbox properties (to communicate with VideCore)
//
// ****************************************************************************

#include "../../includes.h"	// includes

// Mailbox property tag buffer (32-bit words aligned to 16 bytes)
//	index 0: buffer size in bytes
//	index 1: on request 0=request, on response 0x80000000=response OK, 0x80000001=response error
//	... tags
//	... end tag 0
u32 COHERENT MailPropBuf[MAILPROP_SIZE] __attribute__((aligned(16)));
int MailPropInx;	// index into mailbox property buffer

// initialize mailbox property buffer
void MailPropInit()
{
	// reserve index 0 and index 1
	MailPropInx = 2;
}

// Add mailbox property tag to property buffer
//  Mailbox tag (32-bit words)
//	index 0: tag indentifier
//	index 1: buffer size in bytes (aligned to 32-bit word)
//	index 2: 0=request, or response 0x80000000 + value length in bytes
//	index 3: value buffer
void MailPropAdd(u32 tag, ...)
{
	va_list vl;
	va_start(vl, tag);

	// store tag
	MailPropBuf[MailPropInx++] = tag;

	// save data
	switch (tag)
	{

// 8-byte buffer (2 words), no input argument
		case TAG_GET_FIRMWARE_VERSION:		// get firmware revision
							//	request: length 0
							//	response: length 4; value: u32 firmware version
		case TAG_GET_BOARD_MODEL:		// get board model
							//	request: length 0
							//	response: length 4; value: u32 board model
		case TAG_GET_BOARD_REVISION:		// get board revision
							//	request: length 0
							//	response: length 4; value: u32 board revision
		case TAG_GET_BOARD_MAC_ADDRESS:		// get board MAC address
							//	request: length 0
							//	response: length 6; value: u8... MAC address in network byte order
		case TAG_GET_BOARD_SERIAL:		// get board serial
							//	request: length 0
							//	response: length 8; value: u64 board revision
		case TAG_GET_ARM_MEMORY:		// get ARM memory
							//	request: length 0
							//	response: length 8; value: u32 base address, u32 size in bytes
							//		future may specify multiply base+size
		case TAG_GET_VC_MEMORY:			// get VC memory
							//	request: length 0
							//	response: length 8; value: u32 base address, u32 size in bytes
							//		future may specify multiply base+size
		case TAG_GET_DMA_CHANNELS:		// get DMA channels
							//	request: length 0
							//	response: length 4, value u32 used mask (1=DMA channel 0..15 used)
		case TAG_GET_LED_STATUS:		// get onboard LED status
							//	request: length 0
							//	response: length 8; value: u32 pin number, u32 status 0 or 1
							//		pin number: 42 status LED, 130 power LED
		case TAG_TEST_LED_STATUS:		// test onboard LED status
							//	request: length 0
							//	response: length 8; value: u32 pin number, u32 status 0 or 1
							//		pin number: 42 status LED, 130 power LED
		case TAG_RELEASE_BUFFER:		// release frame buffer
							//	request: length 0
							//	response: length 0
		case TAG_GET_PHYSICAL_SIZE:		// get physical width/heigth of display (=size of allocated buffer, not resolution of video)
							//	request: length 0
							//	response: lengt 8, u32 width in pixels, u32 height in pixels
		case TAG_GET_VIRTUAL_SIZE:		// get virtual width/height of buffer (=portion of buffer that is sent to display)
							//	request: length 0
							//	response: lengt 8, u32 width in pixels, u32 height in pixels
		case TAG_GET_DEPTH:			// get depth
							//	request: length 0
							//	response: length 4, u32 bits per pixel
		case TAG_GET_PIXEL_ORDER:		// get pixel order
							//	request: length 0
							//	response: length 4, u32 state (0=BGR, 1=RGB)
		case TAG_GET_ALPHA_MODE:		// get alpha mode
							//	request: length 0
							//	response: length 4, u32 alpha state
							//		0=alpha channel enabled (0=fully opaque)
							//		1=alpha channel reversed (0=fully transparent)
							//		2=alpha channel ignored
		case TAG_GET_PITCH:			// get pitch
							//	request: length 0
							//	response: length 4, u32 bytes per line
		case TAG_GET_VIRTUAL_OFFSET:		// get virtual offset
							//	request: length 0
							//	response: length 8, u32 X in pixels, u32 Y in pixels
			MailPropBuf[MailPropInx++] = 8;	// data buffer size
			MailPropBuf[MailPropInx++] = 0;	// request flag
			MailPropInx += 2;		// skip data buffer
			break;

// 8-byte buffer (2 words), 1 input argument
		case TAG_GET_POWER_STATE:		// get power state
							//	request: length 4, u32 device id (DEV_*)
							//	response: length 8, u32 device id, u32 state
							//			bit 0: 1=on, 0=off
							//			bit 1: 1=device not exist, 0=device exists
		case TAG_GET_TIMING:			// get timing
							//	request: length 4, u32 device id (DEV_*)
							//	response: length 8, u32 device id, u32 wait time in us
							//		- wait time required after turning device on
							//			before power is stable (0=device not exist)
		case TAG_GET_CLOCK_STATE:		// get clock state
							//	request: length 4, u32 clock id (CLK_*)
							//	response: length 8, u32 clock id, u32 state
							//			bit 0: 1=on, 0=off
							//			bit 1: 1=clock does not exist, 0=clock exists
		case TAG_GET_CLOCK_RATE:		// get clock rate
							//	request: length 4, u32 clock id (CLK_*)
							//	response: length 8, u32 clock id, u32 rate in Hz (0=clock not exists)
		case TAG_GET_CLOCK_RATE_MEASURED:	// get clock rate measured
							//	request: length 4; value: u32 clock id (see MP_CLK_* above)
							//	response: length 8; value: u32 clock id, u32 rate in Hz (0=clock not exists)
							//	Get the true/actual clock rate (instead of the last requested value returned
							//	by "Get clock rate"/0x0003002) which respects clamping, throttling and clock
							//	divider limitations.
		case TAG_GET_MAX_CLOCK_RATE:		// get max clock rate
							//	request: length 4, u32 clock id (CLK_*)
							//	response: length 8, u32 clock id, u32 max rate in Hz
		case TAG_GET_MIN_CLOCK_RATE:		// get min clock rate
							//	request: length 4, u32 clock id (CLK_*)
							//	response: length 8, u32 clock id, u32 min rate in Hz (idle)
		case TAG_GET_TURBO:			// get turbo
							//	request: length 4, u32 id (should be 0)
							//	response: length 8, u32 id, u32 level (1=turbo, 0=non-turbo)
		case TAG_GET_VOLTAGE:			// get voltage
							//	request: length 4, u32 voltage id (VOLT_*)
							//	response: length 8, u32 voltage id, u32 value 
							//		(offset from 1.2V in units of 0.025V, 0x80000000 invalid id)
		case TAG_GET_MAX_VOLTAGE:		// get max voltage
							//	request: length 4, u32 voltage id (VOLT_*)
							//	response: length 8, u32 voltage id, u32 max value 
							//		(offset from 1.2V in units of 0.025V, 0x80000000 invalid id)
		case TAG_GET_MIN_VOLTAGE:		// get min voltage
							//	request: length 4, u32 voltage id (VOLT_*)
							//	response: length 8, u32 voltage id, u32 min value 
							//		(offset from 1.2V in units of 0.025V, 0x80000000 invalid id)
		case TAG_GET_TEMPERATURE:		// get temperature
							//	request: length 4, u32 temperature id (should be 0)
							//	response: length 8, u32 temperature id, u32 temperature in thousands of °C
		case TAG_GET_MAX_TEMPERATURE:		// get max temperature
							//	request: length 4, u32 temperature id (should be 0)
							//	response: length 8, u32 temperature id, u32 max temperature in thousands of °C
		case TAG_LOCK_MEMORY:			// lock memory
							//	request: length 4, u32 handle
							//	response: length 4, u32 bus address
							//	Lock buffer in place, and return a bus address. Must be done before memory can be accessed
		case TAG_UNLOCK_MEMORY:			// unlock memory (It retains contents, but may move.)
							//	request: length 4, u32 handle
							//	response: length 4, u32 status (0=success)
		case TAG_RELEASE_MEMORY:		// release memory
							//	request: length 4, u32 handle
							//	response: length 4, u32 status (0=success)
		case TAG_GET_DISPMANX_MEM_HANDLE:	// Get Dispmanx Resource mem handle
							//	request: length 4, u32 dispmanx resource handle
							//	response: length 8, u32 0 if successful, u32 mem handle for resource
		case TAG_ALLOCATE_BUFFER:		// allocate frame buffer
							//	request: length 4, u32 alignment in bytes
							//	response: length 8, u32 frame buffer address, u32 frame buffer size
							//		0=alignment not supported
		case TAG_BLANK_SCREEN:			// blank screen
							//	request: length 4, u32 state (bit 0: 1=on, 0=off)
							//	response: length 4, u32 state (bit 0: 1=on, 0=off)
		case TAG_TEST_DEPTH:			// test depth
							//	request: length 4, u32 bits per pixel
							//	response: length 4, u32 bits per pixel (same as request if supported)
		case TAG_SET_DEPTH:			// set depth
							//	request: length 4, u32 bits per pixel
							//	response: length 4, u32 bits per pixel
							//		response may not be the same as request, need to check (0=unsupported)
		case TAG_TEST_PIXEL_ORDER:		// test pixel order
							//	request: length 4, u32 state (0=BGR, 1=RGB)
							//	response: length 4, u32 state (0=BGR, 1=RGB) (same as request if supported)
		case TAG_SET_PIXEL_ORDER:		// set pixel roder
							//	request: length 4, u32 state (0=BGR, 1=RGB)
							//	response: length 4, u32 state (0=BGR, 1=RGB)
							//		response may not be the same as request, need to check
		case TAG_TEST_ALPHA_MODE:		// test alpha mode
							//	request: length 4, u32 state (0=alpha, 1=reversed, 2=ignored)
							//	response: length 4, u32 state (0=alpha, 1=reversed, 2=ignored) (same as request if supported)
		case TAG_SET_ALPHA_MODE:		// set alpha mode
							//	request: length 4, u32 state (0=alpha, 1=reversed, 2=ignored)
							//	response: length 4, u32 state (0=alpha, 1=reversed, 2=ignored)
							//		response may not be the same as request, need to check
			MailPropBuf[MailPropInx++] = 8;	// data buffer size
			MailPropBuf[MailPropInx++] = 0;	// request flag
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save input argument
			MailPropInx++;			// skip rest of data buffer
			break;

// 8-byte buffer (2 words), 2 input arguments
		case TAG_SET_POWER_STATE:		// set power state
							//	request: length 8, u32 device id (DEV_*), u32 state
							//			bit 0: 1=on, 0=off
							//			bit 1: 1=wait, 0=do not wait (wait for stable)
							//	response: length 8, u32 device id, u32 state
							//			bit 0: 1=on, 0=off
							//			bit 1: 1=device not exist, 0=device exists
		case TAG_SET_CLOCK_STATE:		// set clock state
							//	request: length 8, u32 clock id (CLK_*), u32 state
							//			bit 0: 1=on, 0=off
							//			bit 1: 1=clock does not exist, 0=clock exists
							//	response: length 8, u32 clock id, u32 state
							//			bit 0: 1=on, 0=off
							//			bit 1: 1=clock does not exist, 0=clock exists
		case TAG_SET_LED_STATUS:		// set onboard LED status
							//	request: length 8; value: u32 pin number, u32 status 0 or 1
							//	response: length 8; value: u32 pin number, u32 status 0 or 1
							//		pin number: 42 status LED, 130 power LED
		case TAG_SET_TURBO:			// set turbo
							//	request: length 8, u32 id (should be 0), u32 level (1=turbo, 0=non-turbo)
							//	response: length 8, u32 id, u32 level (1=turbo, 0=non-turbo)
							//	This will cause GPU clocks to be set to maximum when enabled and minimum when disabled.
		case TAG_SET_VOLTAGE:			// set voltage
							//	request: length 8, u32 voltage id (VOLT_*), u32 value
							//	response: length 8, u32 voltage id, u32 value 
							//		(offset from 1.2V in units of 0.025V, 0x80000000 invalid id)
		case TAG_TEST_PHYSICAL_SIZE:		// test physical width/height of display
							//	request: length 8, u32 width in pixels, u32 height in pixels
							//	response: length 8, u32 width in pixels, u32 height in pixels (same as request if supported)
		case TAG_SET_PHYSICAL_SIZE:		// set physical width/height of display
							//	request: length 8, u32 width in pixels, u32 height in pixels
							//	response: length 8, u32 width in pixels, u32 height in pixels
							//		response may not be the same as request, need to check (0=unsupported)
		case TAG_TEST_VIRTUAL_SIZE:		// test virtual width/height of buffer
							//	request: length 8, u32 width in pixels, u32 height in pixels
							//	response: length 8, u32 width in pixels, u32 height in pixels (same as request if supported)
		case TAG_SET_VIRTUAL_SIZE:		// set virtual width/height of buffer
							//	request: length 8, u32 width in pixels, u32 height in pixels
							//	response: length 8, u32 width in pixels, u32 height in pixels
							//		response may not be the same as request, need to check (0=unsupported)
		case TAG_TEST_VIRTUAL_OFFSET:		// test virtual offset
							//	request: length 8, u32 X in pixels, u32 Y in pixels
							//	response: length 8, u32 X in pixels, u32 Y in pixels (same as request if supported)
		case TAG_SET_VIRTUAL_OFFSET:		// set virtual offset
							//	request: length 8, u32 X in pixels, u32 Y in pixels
							//	response: length 8, u32 X in pixels, u32 Y in pixels
							//		response may not be the same as request, need to check (0=unsupported)
		case TAG_SET_SCREEN_GAMMA:		// set screen gamma (Pi 3 only)
							//	request: length 8; value: u32 display number, u32 address of 768-byte gamma table
							//		(256 bytes red, 256 bytes green, 256 blue)
							//	response: length 0
			MailPropBuf[MailPropInx++] = 8;	// data buffer size
			MailPropBuf[MailPropInx++] = 0;	// request flag
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save 1st input argument
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save 2nd input argument
			break;

// 12-byte buffer (3 words), 3 input arguments
		case TAG_SET_CLOCK_RATE:		// set clock rate
							//	request: length 12, u32 clock id (CLK_*), u32 rate in Hz, u32 skip setting turbo
							//			1=skip turbo, disable turbo on freq above default
							//	response: length 8, u32 clock id, u32 rate in Hz (0=clock not exists)
		case TAG_ALLOCATE_MEMORY:		// allocate memory
							//	request: length 12, u32 size, u32 alignment, u32 flags (MEM_FLAG_*)
							//	response: length 4, u32 handle
			MailPropBuf[MailPropInx++] = 12;	// data buffer size
			MailPropBuf[MailPropInx++] = 0;	// request flag
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save 1st input argument
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save 2nd input argument
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save 3rd input argument
			break;

// 16-byte buffer (4 words), 4 input parameters
		case TAG_TEST_OVERSCAN:			// test overscan
							//	request: length 16, u32 top, u32 bottom, u32 left, u32 right (in pixels)
							//	response: length 16, u32 top, u32 bottom, u32 left, u32 right (in pixels)
							//		response may not be the same as request, need to check
		case TAG_SET_OVERSCAN:			// set overscan
							//	request: length 16, u32 top, u32 bottom, u32 left, u32 right (in pixels)
							//	response: length 16, u32 top, u32 bottom, u32 left, u32 right (in pixels)
							//		response may not be the same as request, need to check (0=unsupported)
		case TAG_SET_CURSOR_STATE:		// set cursor state
							//	request: length 16, u32 enable 1=visible 0=invisible, u32 x, u32 y,
							//		u32 flags (0=display coords, 1=framebuffer coords)
							//	response: length 4, u32 state 0=valid, 1=invalid
			MailPropBuf[MailPropInx++] = 16; // data buffer size
			MailPropBuf[MailPropInx++] = 0;	// request flag
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save 1st input argument
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save 2nd input argument
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save 3rd input argument
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save 4th input argument
			break;

// 16-byte buffer (4 words), no input argument
		case TAG_GET_OVERSCAN:			// get overscan
							//	request: length 0
							//	response: length 16, u32 top, u32 bottom, u32 left, u32 right (in pixels)
			MailPropBuf[MailPropInx++] = 16; // data buffer size
			MailPropBuf[MailPropInx++] = 0;	// request flag
			MailPropInx += 4;		// skip data buffer
			break;

// 24-byte buffer (6 words), 6 input parameters
		case TAG_SET_CURSOR_INFO:		// set cursor info
							//	request: length 24, u32 width, u32 height, u32 unused, u32 pointer to pixels, u32 hotspotX, u32 hotspotY
							//	response: length 4, u32 status 0=valid, 1=invalid
							//		format is 32bpp (ARGB), width and height 16..64
							//		default cursor 64x64 with hotspot 0,0
			MailPropBuf[MailPropInx++] = 24; // data buffer size
			MailPropBuf[MailPropInx++] = 0;	// request flag
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save 1st input argument
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save 2nd input argument
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save 3rd input argument
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save 4th input argument
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save 5th input argument
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save 6th input argument
			break;

// 256-byte buffer, no input arguments
		case TAG_GET_CLOCKS:			// get clocks
							//	request: length 0
							//	response: length multiply of 8; value: u32 parent clock id (0=root),
							//		 u32 clock id, ... repeated (parent and child)
		case TAG_GET_COMMAND_LINE:		// get command line
							//	request: length 0
							//	response: length variable, u8... ASCII command line string
			MailPropBuf[MailPropInx++] = 256; // data buffer size
			MailPropBuf[MailPropInx++] = 0;	// request flag
			MailPropInx += 256/4;		// skip data buffer
			break;

// 256-byte buffer, 1 input argument
		case TAG_GET_EDID_BLOCK:		// Get EDID block
							//	request: length 4, u32 block number
							//	response: length 136, u32 block number, u32 status, 128 bytes EDID block
			MailPropBuf[MailPropInx++] = 256; // data buffer size
			MailPropBuf[MailPropInx++] = 0;	// request flag
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save input argument
			MailPropInx += 256/4-1;		// skip data buffer
			break;

// 1024-byte buffer, no input argument
		case TAG_GET_PALETTE:			// get palette
							//	request: length 0
							//	response: length 1024, u32.. RGBA palette values (index 0..255)
			MailPropBuf[MailPropInx++] = 1024; // data buffer size
			MailPropBuf[MailPropInx++] = 0;	// request flag
			MailPropInx += 1024/4;		// skip data buffer
			break;

// 1032-byte buffer, 2 input arguments
//   To test and set palette: allocate tag first and then set entries
		case TAG_TEST_PALETTE:			// test palette
							//	request: length 24..1032, u32 offset first palette 0..255, u32 length number of
							//		entries 1..256, u32.. RGBA palette values (offset..offset+length-1)
							//	response: length 4, u32 state 0=valid, 1=invalid
		case TAG_SET_PALETTE:			// set palette
							//	request: length 24..1032, u32 offset first palette 0..255, u32 length number of
							//		entries 1..256, u32.. RGBA palette values (offset..offset+length-1)
							//	response: length 4, u32 state 0=valid, 1=invalid
			MailPropBuf[MailPropInx++] = 1032; // data buffer size
			MailPropBuf[MailPropInx++] = 0;	// request flag
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save 1st input argument
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save 2nd input argument
			MailPropInx += 1032/4-2;	// skip data buffer
			break;

// 28-byte buffer (7 words), 7 input arguments
		case TAG_EXECUTE_CODE:			// execute code
							//	request: length 28, u32 function pointer, 6x u32 r0..r5
							//	response: length 4, u32 r0
							//	Calls the function at given (bus) address and with arguments
							//	given. E.g. r0 = fn(r0, r1, r2, r3, r4, r5); It blocks until
							//	call completes. The (GPU) instruction cache is implicitly
							//	flushed. Setting the lsb of function pointer address will
							//	suppress the instruction cache flush if you know the buffer
							//	hasn't changed since last execution.
			MailPropBuf[MailPropInx++] = 28; // data buffer size
			MailPropBuf[MailPropInx++] = 0;	// request flag
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save 1st input argument
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save 2nd input argument
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save 3rd input argument
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save 4th input argument
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save 5th input argument
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save 6th input argument
			MailPropBuf[MailPropInx++] = va_arg(vl, u32); // save 7th input argument
			break;

// invalid tag, only ignore it
		default:
			MailPropInx--;
			break;
	}

	va_end(vl);
}

// Process mailbox property buffer
u32 MailPropProcess()
{
	// Mailbox0 read flush
	MailboxFlush();

	// set end tag
	MailPropBuf[MailPropInx] = TAG_END;

	// set size of property buffer (in bytes)
	MailPropBuf[0] = MailPropInx << 2;

	// set request flag
	MailPropBuf[1] = 0;

	// send buffer to mailbox
	MailboxWrite(MB_CHAN_ARM_VC, (u32)(uintptr_t)BUS_ADDRESS(MailPropBuf));

	// receive response
	return MailboxRead(MB_CHAN_ARM_VC);
}

// Search property response in property buffer (returns NULL on error)
//  Returns pointer into property buffer: u32 number of bytes, data follows
u32* MailPropGet(u32 tag)
{
	u32* res;

	// max. index
	int maxinx = MailPropBuf[0] >> 2;

	// search tag
	int i = 2;
	while (i < maxinx)
	{
		// check tag
		if (MailPropBuf[i] == tag)
		{
			res = &MailPropBuf[i+2];
			*res &= 0xffff;	// data size
			return res;
		}

		// next tag
		i += (MailPropBuf[i+1] >> 2) + 3;
	}

	// tag not found
	return NULL;			
}
