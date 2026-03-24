
// ****************************************************************************
//                                 
//                        Project library configuration
//
// ****************************************************************************

#ifndef _CONFIG_H
#define _CONFIG_H

// Screen
#define WIDTH		640		// draw width (0=do not initialize default videomode)
#define HEIGHT		480		// draw height
#define SCREENSCALE	1		// 0 = no back buffer, or use back buffer with screen scale 1, 2 or 4

// Include default configuration
// - For device predefined options see _devices/{device}/_config.h.
// - For all available options see config_def.h in root directory.
#include CONFIG_DEF_H				// default configuration

#endif // _CONFIG_H
