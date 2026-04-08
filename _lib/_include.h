
#ifndef _LIB_INCLUDE_H
#define _LIB_INCLUDE_H

#if USE_BIGINT		// 1=use BIT integers, 0=not used (_lib/bigint/*)
#include "bigint/bigint.h"	// big integers
#endif

#if USE_CALENDAR	// 1=use Calendar 32-bit (year range 1970..2099), 0=not used (lib_calendar.*)
#include "inc/lib_calendar.h"	// calendar 32-bit
#endif

#if USE_CALENDAR64	// 1=use Calendar 64-bit (year range -29227..+29227), 0=not used (lib_calendar64.*)
#include "inc/lib_calendar64.h"	// calendar 64-bit
#endif

#if USE_CRC		// 1=use CRC Cyclic Redundancy Check, 0=not used (lib_crc.*)
#include "inc/lib_crc.h"	// CRC check
#endif

#if USE_DECNUM		// 1=use Decode integer number, 0=not use (lib_decnum.*)
#include "inc/lib_decnum.h"	// decode number
#endif

#if USE_MAT2D			// 1=use 2D transformation matrix (lib_mat2d.*)
#include "inc/lib_mat2d.h"	// 2D Transformation Matrix
#endif

#if USE_DRAW		// 1=use Drawing library, 0=not used (lib_draw.*)
#include "inc/lib_draw.h"	// drawing
#endif

#if USE_STREAM		// 1=use Data stream, 0=not used (lib_stream.*)
#include "inc/lib_stream.h"	// data stream
#endif

#if USE_FAT		// 1=use FAT file system, 0=not used (lib_fat.*)
#include "inc/lib_fat.h"	// FAT file system
#endif

#if USE_JPG		// 1=use JPG support, 0=not used (_lib/pic/lib_jpg*)
#include "pic/lib_jpg.h"	// JPG images
#endif

#if USE_LIST		// 1=use Doubly Linked List, 0=not used (lib_list.*)
#include "inc/lib_list.h"	// Doubly Linked List
#endif

#if USE_MALLOC		// 1=use memory allocator, 0=not used (lib_malloc.*)
#include "inc/lib_malloc.h"	// Memory allocator
#endif

#if USE_MP3		// 1=use MP3 decoder, 0=not used (mp3/lib_mp3.*)
#include "mp3/lib_mp3.h"	// MP3 decoder
#endif

#if USE_PNG		// 1=use PNG support, 0=not used (_lib/pic/lib_png*)
#include "pic/lib_png.h"	// PNG images
#endif

#if USE_PRINT		// 1=use Formatted print, 0=not used (lib_print.*)
#include "inc/lib_print.h"	// Formatted print
#endif

#if USE_PWMSND		// 1=use PWM sound output, 0=not used (lib_pwmsnd.*)
#include "inc/lib_pwmsnd.h"	// PWM sound output
#endif

#if USE_RAND		// 1=use Random number generator, 0=not used (lib_rand.*)
#include "inc/lib_rand.h"	// random number generator
#endif

#if USE_REAL		// 1=use REAL numbers, 0=not used (_lib/real/*)
#define REAL_PATH	"real_h.h"
#include "real/real.h"		// all real numbers
#endif

#if USE_TEXT		// 1=use text strings, 0=not used (lib_text*)
#include "inc/lib_text.h"	// text strings
#endif

#if USE_TREE		// 1=use Tree list, 0=not used (lib_tree.*)
#include "inc/lib_tree.h"	// Tree List
#endif

#endif // _LIB_INCLUDE_H
