
// ****************************************************************************
//
//                          Decode integer number
//
// ****************************************************************************

#ifndef _LIB_DECNUM_H
#define _LIB_DECNUM_H

#if USE_DECNUM		// 1=use Decode integer number, 0=not use (lib_decnum.*)

// temporary decode text buffer
#define DECNUMBUF_SIZE	16		// max. size of decode text buffer (9 digits + 1 sign + 3 separators + 1 zero = 14)
extern char DecNumBuf[DECNUMBUF_SIZE];

// decode unsigned number into ASCIIZ text buffer (returns number of digits)
//  sep = thousand separator, 0=none
int DecUNum(char* buf, u32 num, char sep);

// decode signed number into ASCIIZ text buffer (returns number of digits)
//  sep = thousand separator, 0=none
int DecNum(char* buf, s32 num, char sep);

// decode hex digit in least significant 4bits
char DecHexDig(int num);

// decode hex number (dig = number of digits)
void DecHexNum(char* buf, u32 num, u8 dig);

// decode 2 digits of number
void Dec2Dig(char* buf, u8 num);

// decode 2 digits of number with space character
void Dec2DigSpc(char* buf, u8 num);

#endif // USE_DECNUM

#endif // _LIB_DECNUM_H
