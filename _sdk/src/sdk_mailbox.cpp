
// ****************************************************************************
//
//                       Mailbox - VideoCore communication
//
// ****************************************************************************

#include "../../includes.h"	// includes

// Mailbox0 read flush
void MailboxFlush()
{
	u32 val;

	// flush loop
	while ((Mailbox0->STATUS & MB_EMPTY) == 0)
	{
		// read data
		val = Mailbox0->DATA;

		// small delay
		WaitMs(1);
	}
}

// Mailbox0 read (ch = channel)
u32 MailboxRead(int ch)
{
	u32 val;

	// read loop
	do {
		// wait for data
		while ((Mailbox0->STATUS & MB_EMPTY) != 0) {}

		// data synchronization barrier
		dsb();

		// read data
		val = Mailbox0->DATA;

	// wait for required channel
	} while ((val & MB_CHAN_MASK) != ch);

	// data synchronization barrier
	dsb();

	// return value
	return val & ~MB_CHAN_MASK;
}

// Mailbox1 write (ch = channel)
void MailboxWrite(int ch, u32 val)
{
	// wait for empty space
	while ((Mailbox1->STATUS & MB_FULL) != 0) {}

	// data synchronization barrier
	dsb();

	// write data
	Mailbox1->DATA = (val & ~MB_CHAN_MASK) | ch;

	// data synchronization barrier
	dsb();
}
