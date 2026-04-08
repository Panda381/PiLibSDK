
#include "../includes.h"	// includes

#if USE_BIGINT		// 1=use BIT integers, 0=not used (_lib/bigint/*)
#include "bigint/bigint.cpp"	// BIG integers
#if BIGINT_BERN_NUM > 0
#include "bigint/bernoulli.cpp"	// Bernoulli numbers
#endif // BIGINT_BERN_NUM
#endif // USE_BIGINT

#if USE_CALENDAR	// 1=use Calendar 32-bit (year range 1970..2099), 0=not used (lib_calendar.*)
#include "src/lib_calendar.cpp"	// calendar 32-bit
#endif

#if USE_CALENDAR64	// 1=use Calendar 64-bit (year range -29227..+29227), 0=not used (lib_calendar64.*)
#include "src/lib_calendar64.cpp" // calendar 64-bit
#endif

#if USE_CRC		// 1=use CRC Cyclic Redundancy Check, 0=not used (lib_crc.*)
#include "src/lib_crc.cpp"	// CRC check
#endif

#if USE_DECNUM		// 1=use Decode integer number, 0=not use (lib_decnum.*)
#include "src/lib_decnum.cpp"	// decode number
#endif

#if USE_DRAW		// 1=use Drawing library, 0=not used (lib_draw.*)
#include "src/lib_draw.cpp"	// drawing
#endif

#if USE_FAT		// 1=use FAT file system, 0=not used (lib_fat.*)
#include "src/lib_fat.cpp"	// FAT file system
#endif

#if USE_JPG		// 1=use JPG support, 0=not used (_lib/pic/lib_jpg*)
#include "pic/lib_jpg.cpp"	// JPG images
#endif

#if USE_LIST		// 1=use Doubly Linked List, 0=not used (lib_list.*)
#include "src/lib_list.cpp"	// Doubly Linked List
#endif

#if USE_MALLOC		// 1=use memory allocator, 0=not used (lib_malloc.*)
#include "src/lib_malloc.cpp"	// Memory allocator
#endif

#if USE_MAT2D			// 1=use 2D transformation matrix (lib_mat2d.*)
#include "src/lib_mat2d.cpp"	// 2D Transformation Matrix
#endif

#if USE_MP3		// 1=use MP3 decoder, 0=not used (mp3/lib_mp3.*)
#include "mp3/lib_mp3.cpp"	// MP3 decoder
#endif

#if USE_PNG		// 1=use PNG support, 0=not used (_lib/pic/lib_png*)
#include "pic/lib_png.cpp"	// PNG images
#endif

#if USE_PRINT		// 1=use Formatted print, 0=not used (lib_print.*)
#include "src/lib_print.cpp"	// Formatted print
#endif

#if USE_PWMSND		// 1=use PWM sound output, 0=not used (lib_pwmsnd.*)
#include "src/lib_pwmsnd.cpp"	// PWM sound output
#endif

#if USE_RAND		// 1=use Random number generator, 0=not used (lib_rand.*)
#include "src/lib_rand.cpp"	// random number generator
#endif

#if USE_REAL		// 1=use REAL numbers, 0=not used (_lib/real/*)
#include "real/real.cpp"	// REAL numbers
#endif

#if USE_STREAM		// 1=use Data stream, 0=not used (lib_stream.*)
#include "src/lib_stream.cpp"	// data stream
#endif

#if USE_TEXT		// 1=use text strings, 0=not used (lib_text*)
#include "src/lib_text.cpp"	// text strings
#include "src/lib_textlist.cpp"	// list of text strings
#include "src/lib_textnum.cpp"	// text strings - numeric conversions
#endif

#if USE_TREE		// 1=use Tree list, 0=not used (lib_tree.*)
#include "src/lib_tree.cpp"	// Tree List
#endif
