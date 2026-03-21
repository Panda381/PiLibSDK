
// ****************************************************************************
//                                 
//                         Library Default Configuration
//
// ****************************************************************************

#ifndef _CONFIG_DEF_H
#define _CONFIG_DEF_H

// base device
#if USE_PIBASE
#include "_devices/pibase/_config.h"
#endif

// ZeroPC
#if USE_ZEROPC
#include "_devices/zeropc/_config.h"
#endif

#endif // _CONFIG_DEF_H
