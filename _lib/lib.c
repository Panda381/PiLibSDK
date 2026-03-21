
#include "../includes.h"	// includes

#if USE_CALENDAR	// 1=use Calendar 32-bit (year range 1970..2099), 0=not used (lib_calendar.*)
#include "src/lib_calendar.c"	// calendar 32-bit
#endif

#if USE_CALENDAR64	// 1=use Calendar 64-bit (year range -29227..+29227), 0=not used (lib_calendar64.*)
#include "src/lib_calendar64.c"	// calendar 64-bit
#endif

#if USE_CRC		// 1=use CRC Cyclic Redundancy Check, 0=not used (lib_crc.*)
#include "src/lib_crc.c"	// CRC check
#endif

#if USE_DECNUM		// 1=use Decode integer number, 0=not use (lib_decnum.*)
#include "src/lib_decnum.c"	// decode number
#endif

#if USE_DRAW		// 1=use Drawing library, 0=not used (lib_draw.*)
#include "src/lib_draw.c"	// drawing
#endif

#if USE_FAT		// 1=use FAT file system, 0=not used (lib_fat.*)
#include "src/lib_fat.c"	// FAT file system
#endif

#if USE_LIST		// 1=use Doubly Linked List, 0=not used (lib_list.*)
#include "src/lib_list.c"	// Doubly Linked List
#endif

#if USE_MALLOC		// 1=use memory allocator, 0=not used (lib_malloc.*)
#include "src/lib_malloc.c"	// Memory allocator
#endif

#if USE_PRINT		// 1=use Formatted print, 0=not used (lib_print.*)
#include "src/lib_print.c"	// Formatted print
#endif

#if USE_PWMSND		// 1=use PWM sound output, 0=not used (lib_pwmsnd.*)
#include "src/lib_pwmsnd.c"	// PWM sound output
#endif

#if USE_RAND		// 1=use Random number generator, 0=not used (lib_rand.*)
#include "src/lib_rand.c"	// random number generator
#endif

#if USE_STREAM		// 1=use Data stream, 0=not used (lin_stream.*)
#include "src/lib_stream.c"	// data stream
#endif

#if USE_TREE		// 1=use Tree list, 0=not used (lib_tree.*)
#include "src/lib_tree.c"	// Tree List
#endif
