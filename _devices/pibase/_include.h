
// ****************************************************************************
//
//                             PiBase - includes
//
// ****************************************************************************

#ifndef _PIBASE_DEVINIT_H
#define _PIBASE_DEVINIT_H

// key codes - not used, only not report errors
#define KEY_MASK		0x3f	// mask key scan code
#define KEYFLAG_SHIFT		B8	// Shift (= 0x100)
#define KEYFLAG_CTRL		B9	// Ctrl (= 0x200)
#define KEYFLAG_ALT		B10	// Alt (= 0x400)
#define KEY_ALT			0
#define KEY_SHIFT		1
#define KEY_CTRL		2
#define KEY_LEFT		3
#define KEY_DOWN		4
#define KEY_UP			5
#define KEY_RIGHT		6
#define KEY_ESC			7
#define KEY_SPACE		8
#define KEY_ENTER		10
#define KEY_TAB			11
#define KEY_BS			12
#define KEY_HOME	(KEY_LEFT | KEYFLAG_ALT) // key Home
#define KEY_END		(KEY_RIGHT | KEYFLAG_ALT) // key End
#define KEY_PGUP	(KEY_UP | KEYFLAG_ALT) // key PageUp
#define KEY_PGDN	(KEY_DOWN | KEYFLAG_ALT) // key PageDown
#define KEY_SCREENSHOT	(KEY_TAB | KEYFLAG_ALT)	// key Screenshot

// Initialize device
void DevInit();

// Terminate device
void DevTerm();

// get scan code from keyboard buffer - without CapsLock flags (returns NOKEY if no scan code)
INLINE int KeyGet() { return NOKEY; }

#endif // _PIBASE_DEVINIT_H
