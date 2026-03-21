
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                              Base Definitions                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#ifndef _DEF_H
#define _DEF_H

// base types
typedef __int8				s8;
typedef __int16				s16;
typedef __int32				s32;
typedef __int64				s64;
typedef unsigned __int8		u8;
typedef unsigned __int16	u16;
typedef unsigned __int32	u32;
typedef unsigned __int64	u64;

typedef unsigned long long QWORD;

#define NULL				0
#define FALSE				0
#define TRUE				1
typedef int					BOOL;
typedef unsigned long		DWORD;
typedef unsigned short		WORD;
typedef unsigned char		BYTE;

#pragma warning ( disable: 4996)		// warning - unsafe function

///////////////////////////////////////////////////////////////////////////////
// macros

// rotate left with wrap
#define ROL(val,bits) (unsigned int)((((unsigned int)(val))<<bits)| \
									(((unsigned int)(val))>>(32-bits)))

// conversion big-endian / little-endian
inline WORD BigLit2(WORD n) { return (WORD)(((n & 0xFF) << 8) | \
													((n & 0xFF00) >> 8)); }
inline DWORD BigLit4(DWORD n) { return (DWORD)(((n & 0xFF) << 24) | \
				((n & 0xFF00) << 8) | ((n & 0xFF0000) >> 8) | (n >> 24)); }

// conversion from 1 bit to 8 bits (255/1)
inline BYTE Col1To8(int val) { return (BYTE)(val*255); }

// conversion from 1 bit to 4 bits (15/1)
inline BYTE Col1To4(int val) { return (BYTE)(val*15); }

// conversion from 2 bits to 8 bits (255/3)
inline BYTE Col2To8(int val) { return (BYTE)(val*85); }

// conversion from 2 bits to 4 bits (15/3)
inline BYTE Col2To4(int val) { return (BYTE)(val*5); }

// conversion from 2 bits to 5 bits (31/3)
inline BYTE Col2To5(int val) { return (BYTE)((val*31+1)/3); }

// conversion from 3 bits to 4 bits (15/7)
inline BYTE Col3To4(int val) { return (BYTE)((val*15+3)/7); }

// conversion from 3 bits to 5 bits (31/7)
inline BYTE Col3To5(int val) { return (BYTE)((val*31+3)/7); }

// conversion from 3 bits to 6 bits (63/7)
inline BYTE Col3To6(int val) { return (BYTE)(val*9); }

// conversion from 3 bits to 8 bits (255/3)
inline BYTE Col3To8(int val) { return (BYTE)((val*255+3)/7); }

// conversion from 4 bits to 5 bits (31/15)
inline BYTE Col4To5(int val) { return (BYTE)((val*31+7)/15); }

// conversion from 4 bits to 6 bits (63/15)
inline BYTE Col4To6(int val) { return (BYTE)((val*63+7)/15); }

// conversion from 4 bits to 8 bits (255/15)
inline BYTE Col4To8(int val) { return (BYTE)(val*17); }

// conversion from 5 bits to 6 bits (63/31)
inline BYTE Col5To6(int val) { return (BYTE)((val*63+15)/31); }

// conversion from 5 bits to 8 bits (255/31)
inline BYTE Col5To8(int val) { return (BYTE)((val*255+15)/31); }

// conversion from 6 bits to 8 bits (255/63)
inline BYTE Col6To8(int val) { return (BYTE)((val*255+31)/63); }

#endif // _DEF_H
