
// ****************************************************************************
//                                 
//                              All Includes
//
// ****************************************************************************

#ifndef _INCLUDE_H
#define _INCLUDE_H

// globals
#include "global.h"

// fonts includes
#include "_font/_include.h"

// SDK includes
#include "_sdk/_include.h"

// library includes
#include "_lib/_include.h"

// devices

// base device
#if USE_PIBASE
#include "_devices/pibase/_include.h"
#endif

// ZeroPC
#if USE_ZEROPC
#include "_devices/zeropc/_include.h"
#endif

// ZeroTiny
#if USE_ZEROTINY
#include "_devices/zerotiny/_include.h"
#endif

#endif // _INCLUDE_H
