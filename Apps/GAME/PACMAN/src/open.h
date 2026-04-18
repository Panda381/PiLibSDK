
// ****************************************************************************
//
//                                 Open screen
//
// ****************************************************************************

#ifndef _OPEN_H
#define _OPEN_H

extern u32 LastTime;

// update display and wait (returns True to exit)
Bool DispWait(u32 ms);

// open screen (break with a valid key)
void OpenScreen();

#endif // _OPEN_H
