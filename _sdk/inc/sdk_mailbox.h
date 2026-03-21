
// ****************************************************************************
//
//                       Mailbox - VideoCore communication
//
// ****************************************************************************
// https://bitbanged.com/posts/understanding-rpi/the-mailbox/
// https://github.com/raspberrypi/firmware/wiki/Accessing-mailboxes
// https://github.com/raspberrypi/firmware/wiki/Mailboxes
// https://www.valvers.com/open-software/raspberry-pi/bare-metal-programming-in-c-part-5/#mailboxes

// To read from Mailbox0:
//  1) Read STATUS until empty flag is not set
//  2) Read DATA
//  3) If lower 4 bits do not match the channel number, repeat 1)
//  4) Upper 28 bits are returned data (pointer to mailbox property tag buffer)

// To write to Mailbox1:
//  1) Read STATUS until full flag is not set
//  2) Write data (pointer to mailbox property tag buffer) combined with channel (in lower 4 bits)

// Address should be bus address as seen from the VC (VideoCore):
// - If MMU is not enabled, physical address and ARM address are the same.
// - If L2 is enabled, physical memory is mapped by MMU to base 0x40000000.
// - If L2 is disabled, physical memory is mapped by MMU to base 0xC0000000.
// Buffers must be 16-byte aligned, only upper 28 bits can be passed.

#ifndef _SDK_MAILBOX_H
#define _SDK_MAILBOX_H

#ifdef __cplusplus
extern "C" {
#endif

// Mailbox channels
#define MB_CHAN_POWER		0	// power management
#define MB_CHAN_FRAMEBUFFER	1	// frame buffer
#define MB_CHAN_VUART		2	// virtual UART
#define MB_CHAN_VCHIQ		3	// VCHIQ
#define MB_CHAN_LEDS		4	// LEDs
#define MB_CHAN_BUTTONS		5	// Buttons
#define MB_CHAN_TOUCH		6	// Touch screen
#define MB_CHAN_ARM_VC		8	// property tags ARM -> VC (request from ARM for response by VC)
#define MB_CHAN_VC_ARM		9	// property tags VC -> ARM (request from VC for response by ARM)

#define MB_CHAN_MASK		0x0f	// channel mask

// Mailbox status
#define MB_FULL			B31	// mailbox is full to write
#define MB_EMPTY		B30	// mailbox is empty to read

typedef struct {

	io32	DATA;		// 0x00: Read/write message (FIFO with capacity 8 words 32-bit)
				//  bit 0..3: channel
				//  bit 4..31: data
	io32	res[3];		// 0x04: reserved

	io32	PEEK;		// 0x10: Read message without removing it

	io32	SENDER;		// 0x14: Sender ID
				//  bit 0..1: sender ID

	io32	STATUS;		// 0x18: Status
				//  bit 30: 1=mailbox is empty to read
				//  bit 31: 1=mailbox is full to write

	io32	CONFIG;		// 0x1C: Config
				//	bit 0: 1=IRQ pending for space availability
				//	bit 1: 1=IRQ pending for new mails
				//	bit 2: 1=IRQ pending for opp is empty
				//	bit 3: Clear the mailbox
				//	bit 4: 1=Enable IRQ for has space availability
				//	bit 5: 1=Enable IRQ for new mails
				//	bit 6: 1=Enable IRQ for opp is empty
				//	bit 8: 1=Error Non-owner read attempt
				//	bit 9: 1=Error Attempt to write to filled mailbox
				//	bit 10: 1=Error Attempt to read from empty mailbox

} Mailbox_t;

STATIC_ASSERT(sizeof(Mailbox_t) == 0x20, "Incorrect Mailbox_t!");

// Mailbox peripheral
#define Mailbox0	((Mailbox_t*)MAILBOX_BASE)		// receive response from GPU to CPU
#define Mailbox1	((Mailbox_t*)(MAILBOX_BASE+0x20))	// send request from CPU to GPU

// Mailbox0 read flush
void MailboxFlush();

// Mailbox0 read (ch = channel)
u32 MailboxRead(int ch);

// Mailbox1 write (ch = channel)
void MailboxWrite(int ch, u32 val);

#ifdef __cplusplus
}
#endif

#endif // _SDK_MAILBOX_H
