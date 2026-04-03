
// ****************************************************************************
//
//                               Data stream
//
// ****************************************************************************

#if USE_STREAM		// 1=use Data stream, 0=not used (lib_stream.*)

#ifndef _LIB_STREAM_H
#define _LIB_STREAM_H

typedef struct sStream_ sStream;

// callback function to open stream (start transactions)
typedef void (*pStreamOpen)(sStream* str);

// callback function to close stream (stop transactions and flush buffers)
typedef void (*pStreamClose)(sStream* str);

// callback function to write data to the stream (returns number of bytes written)
typedef u32 (*pStreamWrite)(sStream* str, const void* buf, u32 num);

// callback function to read data from the stream (returns number of bytes read)
typedef u32 (*pStreamRead)(sStream* str, void* buf, u32 num);

// callback function to relative seek position (not all streams support seeking)
typedef void (*pStreamSeek)(sStream* str, int off);

// data stream
typedef struct sStream_
{
	// variables are used by callback functions, but should not be used by caller
	u32		pos;		// object seek position
	u32		size;		// object size
	size_t		cookie;		// object cookie (base address, file handle)

	// caller interacts with the stream through callback functions, not through variables 
	pStreamOpen	open;		// callback function to open stream (start transactions; NULL=none)
	pStreamClose	close;		// callback function to close stream (stop transactions and flush buffers; NULL=none)
	pStreamWrite	write;		// callback function to write data to the stream (returns number of bytes written)
	pStreamRead	read;		// callback function to read data from the stream (returns number of bytes read)
	pStreamSeek	seek;		// callback function to relative seek position (not all streams support seeking)
} sStream;

// callback - nul write stream function
u32 StreamWrite0(sStream* str, const void* buf, u32 num);

// callback - nul read stream function
u32 StreamRead0(sStream* str, void* buf, u32 num);

// callback - nul seek stream function
void StreamSeek0(sStream* str, int off);

// initialize nul stream (used to get number of characters)
void Stream0Init(sStream* str);

// callback - write data to memory buffer
u32 StreamWriteBuf(sStream* str, const void* buf, u32 num);

// callback - read data from memory buffer
u32 StreamReadBuf(sStream* str, void* buf, u32 num);

// callback - relative seek memory buffer
void StreamSeekBuf(sStream* str, int off);

// initialize stream to read from memory buffer
void StreamReadBufInit(sStream* str, const void* buf, u32 num);

// initialize stream to write to memory buffer
void StreamWriteBufInit(sStream* str, void* buf, u32 num);

#endif // _LIB_STREAM_H

#endif // USE_STREAM
