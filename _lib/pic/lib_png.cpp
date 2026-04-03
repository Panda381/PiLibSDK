
// ****************************************************************************
//
//                              PNG images
//
// ****************************************************************************

#include "../../includes.h"	// includes

#if USE_PNG		// 1=use PNG support, 0=not used (_lib/pic/lib_png*)

INLINE u32 BigLit4(u32 n) { return (u32)(((n & 0xFF) << 24) | \
				((n & 0xFF00) << 8) | ((n & 0xFF0000) >> 8) | (n >> 24)); }

// conversion from 2 bits to 8 bits (255/3)
INLINE u8 Col2To8(int val) { return (u8)(val*85); }

// conversion from 4 bits to 8 bits (255/15)
INLINE u8 Col4To8(int val) { return (u8)(val*17); }

#define TEXMAXWH	8192	// texture maximal width and height

// chunk identifiers
#define CHUNK_IEND	0x444e4549		// end of file 'IEND'
#define CHUNK_IHDR	0x52444849		// header 'IHDR'
#define CHUNK_PLTE	0x45544c50		// palette 'PLTE'
#define CHUNK_IDAT	0x54414449		// data 'IDAT'
#define CHUNK_tRNS	0x534e5274		// transparency 'tRNS'

// PNG IHDR header format (size 13 bytes)
#pragma pack(push,1)
typedef struct {
	u32	width;			// image width
	u32	height;			// image height
	u8	bitdepth;		// number of bits per color sample or index
	u8	colortype;		// color type (see below)
	u8	compression;
	u8	filter;
	u8	interlace;
} PNGHeader;
#pragma pack(pop)

// 3D color as QUADRUPLE (palette entry)
typedef struct _D3COLRGBA {
	u8	b;
	u8	g;
	u8	r;
	u8	a;
} D3COLRGBA;

// color type
#define COLTYPE_GRAY	0		// 1 grayscale sample, 1/2/4/8/16 bits
#define COLTYPE_RGB	2		// RGB triple of samples, 8/16 bits
#define COLTYPE_PAL	3		// palette index, 1/2/4/8 bits
#define COLTYPE_GRAYA	4		// 1 grayscale + 1 alpha sample, 8/16 bits
#define COLTYPE_RGBA	6		// RGB triple + 1 alpha sample, 8/16 bits

// CRC table
u32 PNG_CRC[256];
Bool CRC_Init = False;

// buffers
sStream* PNGInStream = NULL;		// input stream
u8*	PNGInBuf = NULL;		// data input buffer (=chunk data)
u8*	PNGInData = NULL;		// input data buffer
int	PNGInDataSize;			// input data buffer size
u8*	PNGInSrc;			// source pointer
u8*	PNGInEnd;			// end of input buffer
u32	PNGInSize;			// size of input buffer (=chunk length)

u8*	PNGRow[2];			// row buffers
int	PNGRowSize;			// size of image row
int	PNGRowInx;			// index of current row buffer

sPic*	PNGOutPic = NULL;		// output image

D3COLRGBA* PNGPal = NULL;		// palette buffer

int	PNGW;				// width
int	PNGH;				// height
int	PNGBits;			// color bits per sample (1, 2, 4, 8, 16)
int	PNGBytes;			// color bytes per pixel (1, 2, 3, 4)
int	PNGColType;			// color type (COLTYPE_*)
D3COLRGBA PNGTransCol;			// transparent color
Bool	PNGTransPal;			// use transparent palettes
Bool	PNGUseTrans;			// use transparent color
Bool	PNGInter;			// interlaced mode

int	PNGInterPass;			// current interlace pass

// LZ77 inflate decoder
u32	LZAdler32;			// Adler32 checksum
u8*	LZWin = NULL;			// LZ77 sliding window
u32	LZWinPos;			// output window position
u32	LZCopyPos;			// input copy position
u32	LZCopyCount;			// remaining LZ77 copy bytes
Bool	LZFinalDataSet;			// final data set is read
int	LZWinSize;			// LZ77 sliding window size

int	LZBitPos;			// current bit position
Bool	LZBitMode;			// reader is in bit mode

u16	LZLitCount;			// remaining literal bytes

// Huffman decoder
class HuffmanDecoder
{
public:
	u32	m_maxcodesize;		// maximum code size
	u32	m_valuecount;		// number of values
	int*	m_maxcode;		// maximum Huffman code values
	int*	m_mincode;		// minimum Huffman code values
	u32*	m_valinx;		// indexes into minimum codes table
	u32*	m_values;		// values

	// constructor
	HuffmanDecoder()
	{
		m_maxcodesize = 0;
		m_valuecount = 0;
		m_maxcode = NULL;
		m_mincode = NULL;
		m_valinx = NULL;
		m_values = NULL;
	}

	// delete buffers
	void Delete()
	{
		MemFree(m_maxcode);	m_maxcode = NULL;
		MemFree(m_mincode);	m_mincode = NULL;
		MemFree(m_valinx);	m_valinx = NULL;
		MemFree(m_values);	m_values = NULL;
	}

	// destructor
	~HuffmanDecoder() { Delete(); }

	// initialize Huffman table (returns False on error)
	Bool MakeTable(u32 maxcodesize, u32 valuecount, const u32* codelengths);

	// decode next value
	int Decode();

	// read Huffman length table (returns False on error)
	Bool ReadLengths(u32* lengths, u32 count);
};

HuffmanDecoder	LZLit;		// Huffman literal
HuffmanDecoder	LZDist;		// Huffman distances
HuffmanDecoder	LZTemp;		// temporary decoder

#define INTERPASSES 7				// count of interlace passes

#define LZWINSIZE	(1 << 15)
#define LZWINMASK	(LZWINSIZE-1)
#define BADCODE		0xfffffff

#define LZMAXDISTANCECODESIZE	15
#define LZMAXLENGTHCODESIZE	15
#define LZMAXLENGTHLENGTHCODES	19
#define LZMAXLENGTHLENGTHCODESIZE 7

#define LZMAXLENGTHCODES	286
#define LZFIRSTLENGTHCODE	257
#define LZLENGTHCODECOUNT	(LZMAXLENGTHCODES-LZFIRSTLENGTHCODE)
#define LZMAXDISTANCECODES	30
#define LZLITERALCOUNT		256
#define LZENDCODE		LZLITERALCOUNT

// row filter type
#define FILTERNONE	0
#define FILTERSUB	1
#define FILTERUP	2
#define FILTERAVERAGE	3
#define FILTERPATH	4

// order of length codes
const u8 LZDeflOrder[LZMAXLENGTHLENGTHCODES] = 
{
	16, 17, 18, 0,  8, 7,  9, 6, 10, 5,
	11,  4, 12, 3, 13, 2, 14, 1, 15,
};

const u32 LZBitMask[9] = { 0, 1, 3, 7, 0xf, 0x1f, 0x3f, 0x7f, 0xff };

// lenths of fixed Huffman table
const u32 LZLenTab[288] = 
{
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
	9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
	9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
	9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
	9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
	9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
	9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8,
};

// distances of fixed Huffman table
const u32 LZDistTab[32] = 
{
	5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5,
};

// number of extra bits for code - 257
const int LZLenExt[LZLENGTHCODECOUNT] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2,
	2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0,
};

// base length for code - 257
const int LZLenBase[LZLENGTHCODECOUNT] =
{
	 3,   4,   5,   6,   7,   8,   9,  10,  11,  13,
	15,  17,  19,  23,  27,  31,  35,  43,  51,  59,
	67,  83,  99, 115, 131, 163, 195, 227, 258
};

// number of extra bits for distance
const int LZDistExt[LZMAXDISTANCECODES] =
{
	0,  0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,
	6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13,
};

// base distance
const int LZDistBase[LZMAXDISTANCECODES] =
{
	   1,    2,     3,     4,     5,    7,    9,    13,    17,    25,
	  33,   49,    65,    97,   129,  193,  257,   385,   513,   769,
	1025, 1537,  2049,  3073,  4097, 6145, 8193, 12289, 16385, 24577,
};

// Adam7 interlace pattern
struct InterlaceInfo
{
	u32	rowinterval;
	u32	colinterval;
	u32	startrow;
	u32	startcol;
};

const InterlaceInfo LZInterlace[INTERPASSES] =
{
	{8, 8, 0, 0, },
	{8, 8, 0, 4, },
	{8, 4, 4, 0, },
	{4, 4, 0, 2, },
	{4, 2, 2, 0, },
	{2, 2, 0, 1, },
	{2, 1, 1, 0, },
};

// generate PNG CRC table
static Bool PNGGenCRC()
{
	for(u32 i = 0; i < 256; i++)
	{
		PNG_CRC[i] = i;
		for (int j = 0; j < 8; j++)
		{
			if ((PNG_CRC[i] & 1) == 0)
				PNG_CRC[i] >>= 1;
			else
				PNG_CRC[i] = 0xedb88320 ^ (PNG_CRC[i] >> 1);
		}
	}

	PNGRow[0] = NULL;
	PNGRow[1] = NULL;
	return True;
}

// delete PNG buffers on error
static void PNGDelBuf()
{
	// data input buffer (=chunk data)
	MemFree(PNGInBuf);
	PNGInBuf = NULL;

	// row buffers
	MemFree(PNGRow[0]);
	PNGRow[0] = NULL;
	MemFree(PNGRow[1]);
	PNGRow[1] = NULL;

	// data output buffer
	MemFree(PNGOutPic);
	PNGOutPic = NULL;

	// palette buffer
	MemFree(PNGPal);
	PNGPal = NULL;

	// LZ77 sliding window
	MemFree(LZWin);
	LZWin = NULL;

	// input data buffer
	MemFree(PNGInData);
	PNGInData = NULL;
	PNGInDataSize = 0;
}

// calculate CRC of a chunk
static u32 PNGGetCRC(long type, u8* data, int len)
{
	u32 crc = (u32)-1;
	int i;

	// get CRC of chunk type
	for (i = 0; i < 4; i++)
	{
		crc = PNG_CRC[(crc ^ ((u8*)&type)[i]) & 0xff] ^ (crc >> 8);
	}

	// get CRC of chunk data
	for (i = 0; i < len; i++)
	{
		crc = PNG_CRC[(crc ^ data[i]) & 0xff] ^ (crc >> 8);
	}
	return ~crc;
}

// read data into buffer (returns False on error)
static Bool PNGReadData(void* buf, int size)
{
	size_t n = PNGInStream->read(PNGInStream, buf, size);
	return (int)n == size;
}

// read LONG number (returns False on error)
inline Bool PNGReadLong(u32* data)
{
	if (!PNGReadData(data, sizeof(u32))) return False;
	*data = BigLit4(*data);
	return True;
}

// read one byte from input buffer
static u8 LZReadByte()
{
	if (PNGInSrc < PNGInEnd)
		return *PNGInSrc++;
	else
		return 0;
}

// read one word from input buffer
static u16 LZReadWord()
{
	u8 b1 = LZReadByte();
	u8 b2 = LZReadByte();
	return (u16)(b1 | (b2 << 8));
}

// read u32 from input buffer
static u32 LZReadDWord()
{
	u8 b1 = LZReadByte();
	u8 b2 = LZReadByte();
	u8 b3 = LZReadByte();
	u8 b4 = LZReadByte();
	return (u32)(b1 | (b2 << 8) | (b3 << 16) | (b4 << 24));
}

// enter bit mode
static void LZEnterBitMode(int pos)
{
	if (!LZBitMode)
	{
		LZBitMode = True;
		LZBitPos = pos;
	}
}

// exit bit mode
static void LZExitBitMode()
{
	if ((LZBitPos >= 0) && (PNGInSrc < PNGInEnd)) PNGInSrc++;
	LZBitMode = False;
	LZBitPos = -1;
}

// get bits from input buffer
static int LZGetBits(int bits)
{
	// no data in input buffer left
	if (PNGInSrc >= PNGInEnd) return 0;

	// there is enough bits in current byte
	int res;
	if (bits <= 8 - LZBitPos)
	{
		res = (*PNGInSrc >> LZBitPos) & LZBitMask[bits];
		LZBitPos += bits;
	}
	else
	{
		res = 0;
		int bitsok = 0;

		// get remaining bits in current byte
		if (LZBitPos < 8)
		{
			bitsok = 8 - LZBitPos;
			res = (*PNGInSrc >> LZBitPos) & LZBitMask[bitsok];
			bits -= bitsok;
		}

		// get rest of bits
		while (bits > 0)
		{
			// increase read pointer
			PNGInSrc++;
			if (PNGInSrc >= PNGInEnd) return res;

			if (bits > 8)
			{
				bits -= 8;
				res |= (*PNGInSrc << bitsok);
				bitsok += 8;
			}
			else
			{
				res |= (*PNGInSrc & LZBitMask[bits]) << bitsok;
				LZBitPos = bits;
				break;
			}
		}
	}
	return res;
}

// initialize Huffman table (returns False on error)
Bool HuffmanDecoder::MakeTable(u32 maxcodesize, u32 valuecount, const u32* codelengths)
{
	u32 i, j, k;
	m_maxcodesize = maxcodesize;
	m_valuecount = valuecount;

	// create buffers
	Delete();
	m_maxcode = (int*)MemAlloc(maxcodesize*sizeof(int));
	m_mincode = (int*)MemAlloc(maxcodesize*sizeof(int));
	m_valinx = (u32*)MemAlloc(valuecount*sizeof(u32));
	m_values = (u32*)MemAlloc(valuecount*sizeof(u32));
	if ((m_maxcode == NULL) || (m_mincode == NULL) || (m_valinx == NULL) || (m_values == NULL)) return False;

	// temporary buffer for code sizes
	u32* buffsizes = (u32*)MemAlloc((valuecount+1)*sizeof(u32));
	if (buffsizes == NULL) return False;

	// initialize table of values
	for (i = 0; i < valuecount; i++)
	{
		m_values[i] = i;
		buffsizes[i] = codelengths[i];
	}

	// sort table of values
	for (i = 0; i < valuecount-1; i++)
	{
		for (j = i+1; j < valuecount; j++)
		{
			if ((buffsizes[j] < buffsizes[i]) ||
				((buffsizes[j] == buffsizes[i]) &&
					(m_values[j] < m_values[i])))
			{
				k = buffsizes[j];
				buffsizes[j] = buffsizes[i];
				buffsizes[i] = k;
				k = m_values[j];
				m_values[j] = m_values[i];
				m_values[i] = k;
			}
		}
	}

	// temporary buffer for Huffman codes
	u32* buffcodes = (u32*)MemAlloc(valuecount*sizeof(u32));
	if (buffcodes == NULL) { MemFree(buffsizes); return False; }

	// initialize Huffman codes
	u32 lastlen = 0;
	u32 code = 0;
	for (i = 0; i < valuecount; i++)
	{
		while(lastlen != buffsizes[i])
		{
			lastlen++;
			code <<= 1;
		}

		if (lastlen != 0)
		{
			buffcodes[i] = code;
			code++;
		}
	}

	// initialize tables
	for (i = 0; i < maxcodesize; i++)
	{
		m_mincode[i] = BADCODE;
		m_maxcode[i] = (u32)-1;
	}

	for (i = 0; i < valuecount; i++) m_valinx[i] = 0;

	u32 last = 0;
	for (i = 0; i < valuecount; i++)
	{
		if (last != buffsizes[i])
		{
			last = buffsizes[i];
			m_valinx[last-1] = i;
			m_mincode[last-1] = buffcodes[i];
		}

		if (last != 0) m_maxcode[last-1] = buffcodes[i];
	}
	MemFree(buffsizes);
	MemFree(buffcodes);
	return True;
}

// decode next value
int HuffmanDecoder::Decode()
{
	// read code ("1" bit is prefix of longer code)
	int code = LZGetBits(1);
	u32 codelength;
	for (codelength = 0; code > m_maxcode[codelength]; codelength++)
	{
		if (codelength >= m_maxcodesize) return 0;
		code = ((code << 1) | LZGetBits(1));
	}
	int offset = code - m_mincode[codelength];
	int index = m_valinx[codelength] + offset;
	return m_values[index];
}

// read Huffman length table (returns False on error)
//	0..15 = literal length value
//	16 = repeat last code N-times, N = next 2 bits + 3
//	17 = set N length codes to 0, N = next 3 bits + 3
//	18 = set N length codes to 0, N = next 7 bits + 11
Bool HuffmanDecoder::ReadLengths(u32* lengths, u32 count)
{
	u32 i, k;
	for (i = 0; i < count;)
	{
		int cmd = Decode();

		// length value
		if (cmd < 16)
		{
			lengths[i] = cmd;
			i++;
		}

		// repeat last code
		else if (cmd == 16)
		{
			int n = LZGetBits(2) + 3;
			if (i == 0) return False;
			k = lengths[i-1];
			if ((i + n) > count) return False;
			for (; n > 0; n--)
			{
				lengths[i] = k;
				i++;
			}
		}

		// short fill with zero
		else if (cmd == 17)
		{
			int n = LZGetBits(3) + 3;
			if ((i + n) > count) return False;
			for (; n > 0; n--)
			{
				lengths[i] = 0;
				i++;
			}
		}

		// long fill with zero
		else if (cmd == 18)
		{
			int n = LZGetBits(7) + 11;
			if ((i + n) > count) return False;
			for (; n > 0; n--)
			{
				lengths[i] = 0;
				i++;
			}
		}

		// invalid code
		else
			return False;
	}
	return True;
}

// start new LZ77 data set (return False on error)
static Bool LZStartNewDataSet()
{
	// enter bit mode
	LZEnterBitMode(0);

	// check final data set
	LZFinalDataSet = LZGetBits(1) != 0; 

	// compression type
	int compressiontype = LZGetBits(2);

	switch (compressiontype)
	{
	// uncompressed
	case 0:
		{
			LZExitBitMode();
			LZLitCount = LZReadWord();
			u16 testcount = LZReadWord();
			if ((LZLitCount != (u16)~testcount) || (LZLitCount == 0)) return False;
		}
		break;

	// fixed Huffman codes
	case 1:
		if (!LZLit.MakeTable(LZMAXLENGTHCODESIZE, 288, LZLenTab)) return False;
		if (!LZDist.MakeTable(LZMAXDISTANCECODESIZE, 32, LZDistTab)) return False;
		LZLitCount = 0;
		break;

	// dynamic Huffman codes
	case 2:
		{
			u32 hlit = LZGetBits(5);	// literal code count - 257
			u32 hdist = LZGetBits(5);	// distance codes - 1
			u32 hclen = LZGetBits(4);	// count of length codes - 4
			if ((hclen + 4) > LZMAXLENGTHLENGTHCODES) return False;

			// read length codes
			u32 lengths[LZMAXLENGTHLENGTHCODES];
			memset(lengths, 0, sizeof(lengths));
			u32 i;
			for (i = 0; i < hclen+4; i++)
			{
				lengths[LZDeflOrder[i]] = LZGetBits(3);
			}

			// make table in temporary decoder
			if (!LZTemp.MakeTable(LZMAXLENGTHLENGTHCODESIZE, LZMAXLENGTHLENGTHCODES, lengths)) return False;

			// read literal codes
			u32 literals [288];
			if (!LZTemp.ReadLengths(literals, hlit + 257)) return False;

			// read distance codes
			u32 distances[32];
			if (!LZTemp.ReadLengths(distances, hdist + 1)) return False;

			// make Huffman tables
			if (!LZLit.MakeTable(LZMAXLENGTHCODESIZE, hlit+257, literals)) return False;
			if (!LZDist.MakeTable(LZMAXDISTANCECODESIZE, hdist+1, distances)) return False;
			LZLitCount = 0;
			break;
		}
		break;

	// 3 is invalid mode
	default:
		return False;
	}
	return True;
}

// initialize LZ77 decoder (return False on error)
static Bool LZInit()
{
	LZFinalDataSet = False;
	LZBitMode = False;
	LZBitPos = -1;
	
	MemFree(LZWin);
	LZWin = (u8*)MemAlloc(LZWINSIZE);
	if (LZWin == NULL) return False;
	memset(LZWin, 0, LZWINSIZE);

	LZAdler32 = 1;
	LZWinPos = 0;
	LZCopyPos = 0;
	LZCopyCount = 0;

	// read compressed stream header u8 1
	u8 d1 = LZReadByte();
	if ((d1 & 0xf) != 8) return False; // compression method DEFLATED
	LZWinSize = (1 << (((d1 & 0xf0) >> 4) + 8)); // window size
	if (LZWinSize > LZWINSIZE) return False;

	// read compressed stream header u8 2
	u8 d2 = LZReadByte();
	if ((d2 & (1 << 5)) != 0) return False; // preset dictionary

	// corrupt compression header
	if (((d2 | (d1 << 8)) % 31) != 0) return False;

	// start new data set
	return LZStartNewDataSet();
}

// add Adler32 checksum
static void LZAdler(u8 value)
{
#define ADLER_PRIME 65521
	u32 low = LZAdler32 & 0xffff;
	u32 hgh = (LZAdler32 >> 16);
	low = (low + value) % ADLER_PRIME;
	hgh = (low + hgh) % ADLER_PRIME;
	LZAdler32 = (hgh << 16) | low;
}

// check Adler32 checksum (returns False on error)
static Bool LZCheckAddler()
{
	LZExitBitMode();
	u32 val = LZReadDWord();
	return val == LZAdler32;
}

// decode LZ77 data (returns number of bytes or 0 on error)
static int LZDecode(u8* buf, u32 count)
{
	u32 i = 0;
	u32 val, len, dist;
	int ext;
	u8 res;
	while (i < count)
	{
		// transfer data without compression
		if (LZLitCount > 0)
		{
			while ((LZLitCount > 0) && (i < count))
			{
				LZLitCount--;
				res = LZReadByte();
				LZWin[LZWinPos] = res;
				LZWinPos = (LZWinPos + 1) & LZWINMASK;
				LZAdler(res);
				buf[i] = res;
				i++;
			}

			// start new data set or stop
			if (LZLitCount == 0)
			{
				if (LZFinalDataSet)
				{
					if (!LZCheckAddler()) return 0;
				}
				else
				{
					if (!LZStartNewDataSet()) return 0;
				}
			}
		}

		// copy old data stream
		else if (LZCopyCount > 0)
		{
			while ((LZCopyCount > 0) && (i < count))
			{
				LZCopyCount--;
				res = LZWin[LZCopyPos];
				LZWin[LZWinPos] = res;
				LZCopyPos = (LZCopyPos + 1) & LZWINMASK;
				LZWinPos = (LZWinPos + 1) & LZWINMASK;
				LZAdler(res);
				buf[i] = res;
				i++;
			}
		}

		// deflate compression 
		else
		{
			// read next code
			val = LZLit.Decode();

			// data value
			if (val < LZLITERALCOUNT)
			{
				res = (u8)val;
				LZWin[LZWinPos] = res;
				LZWinPos = (LZWinPos + 1) & LZWINMASK;
				LZAdler(res);
				buf[i] = res;
				i++;
			}

			// end marker - start new data set or stop
			else if (val == LZENDCODE)
			{
				if (LZFinalDataSet)
				{
					if (!LZCheckAddler()) return 0;
					return i;
				}
				else
				{
					if (!LZStartNewDataSet()) return 0;
				}
			}

			// code to copy old string
			else if (val < LZMAXLENGTHCODES)
			{
				// prepare length of string
				ext = LZLenExt[val - LZFIRSTLENGTHCODE];
				len = LZLenBase[val - LZFIRSTLENGTHCODE];
				if (ext > 0) len += LZGetBits(ext);

				// prepare distance of string
				val = LZDist.Decode();
				if (val > 29) return 0;
				ext = LZDistExt[val];
				dist = LZDistBase[val];
				if (ext > 0) dist += LZGetBits(ext);

				// prepare state variables
				LZCopyPos = (LZWINSIZE + LZWinPos - dist) & LZWINMASK;
				LZCopyCount = len;

				// get first byte
				res = LZWin[LZCopyPos];
				LZWin[LZWinPos] = res;
				LZCopyPos = (LZCopyPos + 1) & LZWINMASK;
				LZWinPos = (LZWinPos + 1) & LZWINMASK;
				LZCopyCount--;
				LZAdler(res);
				buf[i] = res;
				i++;
			}

			// invalid code
			else return 0;
		}
	}
	return i;
}

// path predictor
static u8 LZPathPredictor(u8 left, u8 up, u8 upleft)
{
	int pp = left + up - upleft;
	int pa, pb, pc;

	if (pp > left)
		pa = pp - left;
	else
		pa = left - pp;

	if (pp > up)
		pb = pp - up;
	else
		pb = up - pp;

	if (pp > upleft)
		pc = pp - upleft;
	else
		pc = upleft - pp;

	if ((pa <= pb) && (pa <= pc)) return left;
	if (pb <= pc) return up;
	return upleft;
}

// filter row
static void LZFilter(int filter, int size)
{
	int lastrow = PNGRowInx ^ 1;	// index of previous raw
	int off = PNGBytes;		// pixel width
	int i;
	u8 up, left, upleft;

	switch(filter)
	{
	// no filter
	case FILTERNONE:
		break;

	// subtract previous value
	case FILTERSUB:
		for (i = off; i < size; i++)
		{
			PNGRow[PNGRowInx][i] = (u8)(PNGRow[PNGRowInx][i] + PNGRow[PNGRowInx][i-off]);
		}
		break;

	// subtract previous row
	case FILTERUP:
		for (i = 0; i < size; i++)
		{
			PNGRow[PNGRowInx][i] = (u8)(PNGRow[PNGRowInx][i] + PNGRow[lastrow][i]);
		}
		break;

	// subtract average of previous value and previous row
	case FILTERAVERAGE:
		for (i = 0; i < size; i++)
		{
			up = PNGRow[lastrow][i];
			if (i < off)
				left = 0;
			else
				left = PNGRow[PNGRowInx][i-off];
			PNGRow[PNGRowInx][i] = (u8)(PNGRow[PNGRowInx][i] + (left+up)/2);
		}
		break;

	// path predictor 
	case FILTERPATH:
		for (i = 0; i < size; i++)
		{
			up = PNGRow[lastrow][i];
			if (i < off)
			{
				left = 0;
				upleft = 0;
			}
			else
			{
				left = PNGRow[PNGRowInx][i-off];
				upleft = PNGRow[lastrow][i-off];
			}
			PNGRow[PNGRowInx][i] = (u8)(PNGRow[PNGRowInx][i] + LZPathPredictor(left, up, upleft));
		}
		break;
	}
}

// copy non-interlaced row to image buffer
static void LZCopyNonint(int row)
{
	int i;
	u8* dst = &PNGOutPic->data[row*PNGW*PNGBytes];
	u8* src = PNGRow[PNGRowInx];

	switch (PNGColType)
	{
	case COLTYPE_GRAY:
		switch (PNGBits)
		{
		case 1:
			for (i = 0; i < PNGW; i++)
			{
				*dst++ = (u8)((src[i/8] & (1 << (7 - (i&7)))) == 0 ? 0:255);
			}
			break;

		case 2:
			for (i = 0; i < PNGW; i++)
			{
				*dst++ = Col2To8((src[i/4] >> (6 - 2*(i & 3))) & 3);
			}
			break;

		case 4:
			for (i = 0; i < PNGW; i++)
			{
				if ((i & 1) == 0)
					*dst++ = Col4To8((src[i/2] >> 4) & 0xf);
				else
					*dst++ = Col4To8(src[i/2] & 0xf);
			}
			break;
		default:
			memcpy(dst, src, PNGW);
		}
		break;

	case COLTYPE_RGB:
		for (i = 0; i < PNGW; i++)
		{
			*dst++ = src[2];
			*dst++ = src[1];
			*dst++ = src[0];
			src += 3;
		}
		break;

	case COLTYPE_PAL:
		switch (PNGBits)
		{
		case 1:
			for (i = 0; i < PNGW; i++)
			{
				*dst++ = (u8)((src[i/8] >> (7 - (i & 7))) & 1);
			}
			break;

		case 2:
			for (i = 0; i < PNGW; i++)
			{
				*dst++ = (u8)((src[i/4] >> (6 - 2*(i & 3))) & 3);
			}
			break;

		case 4:
			for (i = 0; i < PNGW; i++)
			{
				if ((i & 1) == 0)
					*dst++ = (u8)((src[i/2] >> 4) & 0xf);
				else
					*dst++ = (u8)(src[i/2] & 0xf);
			}
			break;
		default:
			memcpy(dst, src, PNGW);
		}
		break;

	case COLTYPE_GRAYA:
		memcpy(dst, src, PNGW*2);
		break;

	case COLTYPE_RGBA:
		for (i = 0; i < PNGW; i++)
		{
			*dst++ = src[2];
			*dst++ = src[1];
			*dst++ = src[0];
			*dst++ = src[3];
			src += 4;
		}
		break;
	}
}

// copy interlaced row to image buffer
static void LZCopyInt(int row, int pixels, const InterlaceInfo* info)
{
	int i = 0;
	u8* dst = &PNGOutPic->data[row*PNGW*PNGBytes + info->startcol*PNGBytes];
	u8* src = PNGRow[PNGRowInx];

	switch (PNGColType)
	{
	case COLTYPE_GRAY:
		switch (PNGBits)
		{
		case 1:
			for (i = 0; i < pixels; i++)
			{
				*dst = (u8)((src[i/8] & (1 << (7 - (i & 7)))) == 0 ? 0:255);
				dst += info->colinterval;
			}
			break;

		case 2:
			for (i = 0; i < pixels; i++)
			{
				*dst = Col2To8((src[i/4] >> (6 - 2*(i & 3))) & 3);
				dst += info->colinterval;
			}
			break;

		case 4:
			for (i = 0; i < pixels; i++)
			{
				if ((i & 1) == 0)
					*dst = Col4To8((src[i/2] >> 4) & 0xf);
				else
					*dst = Col4To8(src[i/2] & 0xf);
				dst += info->colinterval;
			}
			break;
		default:
			for (i = 0; i < pixels; i++)
			{
				*dst = *src;
				src++;
				dst += info->colinterval;
			}
		}
		break;

	case COLTYPE_RGB:
		for (i = 0; i < pixels; i++)
		{
			dst[0] = src[2];
			dst[1] = src[1];
			dst[2] = src[0];
			dst += info->colinterval*3;
			src += 3;
		}
		break;

	case COLTYPE_PAL:
		switch (PNGBits)
		{
		case 1:
			for (i = 0; i < pixels; i++)
			{
				*dst = (u8)((src[i/8] >> (7 - (i & 7))) & 1);
				dst += info->colinterval;
			}
			break;

		case 2:
			for (i = 0; i < pixels; i++)
			{
				*dst = (u8)((src[i/4] >> (6 - 2*(i & 3))) & 3);
				dst += info->colinterval;
			}
			break;

		case 4:
			for (i = 0; i < pixels; i++)
			{
				if ((i & 1) == 0)
					*dst = (u8)((src[i/2] >> 4) & 0xf);
				else
					*dst = (u8)(src[i/2] & 0xf);
				dst += info->colinterval;
			}
			break;
		default:
			for (i = 0; i < pixels; i++)
			{
				*dst = *src;
				src++;
				dst += info->colinterval;
			}
		}
		break;

	case COLTYPE_GRAYA:
		for (i = 0; i < pixels; i++)
		{
			dst[0] = src[0];
			dst[1] = src[1];
			src += 2;
			dst += info->colinterval*2;
		}
		break;

	case COLTYPE_RGBA:
		for (i = 0; i < pixels; i++)
		{
			dst[0] = src[2];
			dst[1] = src[1];
			dst[2] = src[0];
			dst[3] = src[3];
			src += 4;
			dst += info->colinterval*4;
		}
		break;
	}
}

// read non-interlaced data (returns False on error)
static Bool LZReadNonint()
{
	// clear row buffers
	memset(PNGRow[0], 0, PNGRowSize);
	memset(PNGRow[1], 0, PNGRowSize);
	PNGRowInx = 0;

	// read all rows
	u32 i;
	u8 filter;
	for (i = 0; i < (u32)PNGH; i++)
	{
		// get filter mode
		if ((LZDecode(&filter, 1) != 1) || (filter > 4)) return False;

		// read one row
		if (LZDecode(PNGRow[PNGRowInx],PNGRowSize) != PNGRowSize) return False;

		// filter row
		LZFilter(filter, PNGRowSize);

		// copy row to image buffer
		LZCopyNonint(i);

		// switch buffer
		PNGRowInx ^= 1;
	}
	return True;
}

// read interlaced data (returns False on error)
static Bool LZReadInt()
{
	u32 i;
	u8 filter;

	for (PNGInterPass = 0; PNGInterPass < INTERPASSES; PNGInterPass++)
	{
		// interlace values
		const InterlaceInfo* info = LZInterlace + PNGInterPass;

		// check image size
		if (((int)info->startrow < PNGH) && ((int)info->startcol < PNGW))
		{
			// clear row buffers
			memset(PNGRow[0], 0, PNGRowSize);
			memset(PNGRow[1], 0, PNGRowSize);
			PNGRowInx = 0;

			// pixels per this row
			u32 pixels = (PNGW - info->startcol + info->colinterval - 1) / info->colinterval;

			// bytes per this row
			u32 rowbytes = (pixels*PNGBytes*PNGBits + 7)/8;

			// read all rows
			for (i = info->startrow; i < (u32)PNGH; i += info->rowinterval)
			{
				// get filter mode
				if ((LZDecode(&filter, 1) != 1) || (filter > 4)) return False;

				// read one row
				if (LZDecode(PNGRow[PNGRowInx], rowbytes) != (int)rowbytes) return False;

				// filter row
				LZFilter(filter, rowbytes);

				// copy row to image buffer
				LZCopyInt(i, pixels, info);

				// switch buffer
				PNGRowInx ^= 1;
			}
		}
	}
	return True;
}

// decode PNG image from stream to image in CF_A8B8G8R8, CF_B8G8R8 or CF_A8 format
//  Returns sPic* image (allocated with MemAlloc()) or NULL on error.
sPic* PNGLoadStream(sStream* str)
{
	// input stream
	PNGInStream = str;

	// generate CRC table
	if (!CRC_Init)
	{
		PNGGenCRC();
		CRC_Init = True;
	}

	// check PNG signature
	u8 sig[8];
	size_t n = str->read(str, sig, 8);
	if ((n != 8) || (sig[0] != 0x89) || (sig[1] != 'P') || (sig[2] != 'N') ||
		(sig[3] != 'G') || (sig[4] != 13) || (sig[5] != 10) ||
		(sig[6] != 26) || (sig[7] != 10)) return NULL;

	// prepare data
	int i;
	PNGW = 0;		// width
	PNGH = 0;		// height
	PNGTransPal = False;	// use transparent palettes
	PNGUseTrans = False;	// use transparent color
	PNGInData = NULL;	// no input data buffer
	PNGInDataSize = 0;	// size of input data buffer
	PNGOutPic = NULL;	// no output data buffer

	// read all chunks
	for (;;)
	{
		// read chunk data length
		PNGInSize = 0;
		if (!PNGReadLong(&PNGInSize) || (PNGInSize > 50000000))
			{ PNGDelBuf(); return NULL; }

		// read chunk type
		u32 chunktype = 0;
		if (!PNGReadData(&chunktype, sizeof(u32)))
			{ PNGDelBuf(); return NULL; }

		// read chunk data
		MemFree(PNGInBuf);
		PNGInBuf = NULL;
		if (PNGInSize > 0)
		{
			PNGInBuf = (u8*)MemAlloc(PNGInSize);
			if (PNGInBuf == NULL) { PNGDelBuf(); return NULL; }
			if (!PNGReadData(PNGInBuf, PNGInSize))
				{ PNGDelBuf(); return NULL; }
		}

		// read and check CRC
		u32 crc = 0;
		if (!PNGReadLong(&crc)) { PNGDelBuf(); return NULL; }
		u32 crc2 = PNGGetCRC(chunktype, PNGInBuf, PNGInSize);
		if (crc2 != crc) { PNGDelBuf(); return NULL; }

		// end chunk 'IEND'
		if (chunktype == CHUNK_IEND)
		{
			MemFree(PNGInBuf);
			PNGInBuf = NULL;
			break;
		}

		// process chunk
		switch (chunktype)
		{
		// header 'IHDR'
		case CHUNK_IHDR:
			{
				PNGHeader* png = (PNGHeader*)PNGInBuf;
				PNGW = BigLit4(png->width);
				PNGH = BigLit4(png->height);
				PNGBits = png->bitdepth;
				PNGColType = png->colortype;
				PNGInter = (png->interlace == 1);

				// check color type and bit depth (16 bit sample not supported)
				switch(PNGColType)
				{
				case COLTYPE_GRAY:
				case COLTYPE_PAL:
					if ((PNGBits != 1) && (PNGBits != 2) && (PNGBits != 4) && (PNGBits != 8))
						{ PNGDelBuf(); return NULL; }
					PNGBytes = 1;
					break;

				case COLTYPE_RGB:
					if (PNGBits != 8) { PNGDelBuf(); return NULL;}
					PNGBytes = 3;
					break;

				case COLTYPE_GRAYA:
					if (PNGBits != 8) { PNGDelBuf(); return NULL;}
					PNGBytes = 2;
					break;

				case COLTYPE_RGBA:
					if (PNGBits != 8) { PNGDelBuf(); return NULL;}
					PNGBytes = 4;
					break;

				default:
					{ PNGDelBuf(); return NULL; }
				}

				// check PNG header
				if ((PNGW < 1) || (PNGW > TEXMAXWH) ||
					(PNGH < 1) || (PNGH > TEXMAXWH) ||
					(png->compression != 0) ||
					(png->filter != 0) ||
					(PNGInter > 1))
				{
					PNGDelBuf();
					return NULL;
				}
			}
			break;

		// palettes 'PLTE'
		case CHUNK_PLTE:
			if ((PNGPal == NULL) && (PNGColType == COLTYPE_PAL))
			{
				// check number of palettes
				i = PNGInSize/3;
				if ((i > (1<<PNGBits)) ||
					((int)PNGInSize != i*3) ||
					(PNGW == 0)||
					(PNGH == 0))
				{
					PNGDelBuf();
					return NULL;
				}

				// create palette buffer
				PNGPal = (D3COLRGBA*)MemAlloc(256*sizeof(D3COLRGBA));
				if (PNGPal == NULL) { PNGDelBuf(); return NULL; }

				// convert palettes
				for (i--; i >= 0; i--)
				{
					PNGPal[i].r = PNGInBuf[i*3+0];
					PNGPal[i].g = PNGInBuf[i*3+1];
					PNGPal[i].b = PNGInBuf[i*3+2];
					PNGPal[i].a = 255;
				}
			}
			break;

		// transparency 'tRNS'
		case CHUNK_tRNS:
			switch (PNGColType)
			{
			// transparent palettes
			case COLTYPE_PAL:
				if (PNGPal != NULL)
				{
					PNGTransPal = True;		// use transparent palettes
					if (PNGInSize > 256) PNGInSize = 256;
					for (i = 0; i < (int)PNGInSize; i++)
						PNGPal[i].a = PNGInBuf[i];
				}
				break;

			// transparent grayscale
			case COLTYPE_GRAY:
				if (PNGInSize >= 2)
				{
					PNGUseTrans = True;
					PNGTransCol.r = PNGInBuf[1];
					PNGTransCol.g = PNGInBuf[1];
					PNGTransCol.b = PNGInBuf[1];
					PNGTransCol.a = 0;
				}
				break;

			// transparent RGB
			case COLTYPE_RGB:
				if (PNGInSize >= 6)
				{
					PNGUseTrans = True;
					PNGTransCol.r = PNGInBuf[1];
					PNGTransCol.g = PNGInBuf[3];
					PNGTransCol.b = PNGInBuf[5];
					PNGTransCol.a = 0;
				}
				break;
			}
			break;

		// data 'IDAT'
		case CHUNK_IDAT:
			{
				u8* d = (u8*)MemResize(PNGInData, PNGInDataSize + PNGInSize);
				if (d == NULL) { PNGDelBuf(); return NULL; }
				memcpy(d + PNGInDataSize, PNGInBuf, PNGInSize);
				PNGInData = d;
				PNGInDataSize += PNGInSize;
			}
			break;
		}

		// free chunk buffer
		MemFree(PNGInBuf);
		PNGInBuf = NULL;
	}

	// header must be valid
	if ((PNGW == 0) || (PNGH == 0) || (PNGInDataSize == 0) ||
		((PNGPal == NULL) && (PNGColType == COLTYPE_PAL)))
	{
		PNGDelBuf();
		return NULL;
	}

	// input buffer
	PNGInSrc = PNGInData;			// source pointer
	PNGInEnd = PNGInData + PNGInDataSize; // end of input buffer

	// row size
	if (PNGBits >= 8)
		PNGRowSize = PNGW*PNGBytes;
	else
		PNGRowSize = (PNGW*PNGBits+7)/8;

	// raw buffers
	MemFree(PNGRow[0]);
	PNGRow[0] = (u8*)MemAlloc(PNGRowSize);
	if (PNGRow[0] == NULL) { PNGDelBuf(); return NULL; }
	MemFree(PNGRow[1]);
	PNGRow[1] = (u8*)MemAlloc(PNGRowSize);
	if (PNGRow[1] == NULL) { PNGDelBuf(); return NULL; }
	PNGRowInx = 0;

	// output buffer
	PNGOutPic=(sPic*)MemAlloc(SPIC_HEADER_SIZE + PNGW*PNGH*PNGBytes + 10); // output buffer
	if (PNGOutPic == NULL) { PNGDelBuf(); return NULL; }

	// initialize LZ77 decoder (return False on error)
	if (!LZInit()) { PNGDelBuf(); return NULL; }

	// read data interlaced or non-interlaced
	if (PNGInter)
	{
		if (!LZReadInt()) { PNGDelBuf(); return NULL; }
	}
	else
	{
		if (!LZReadNonint()) { PNGDelBuf(); return NULL; }
	}

	// delete input data buffer
	MemFree(PNGInData);
	PNGInData = NULL;
	PNGInDataSize = 0;

	// set output parameters
	sPic* pic = PNGOutPic;
	pic->w = PNGW;
	pic->h = PNGH;
	pic->wb = PNGW*PNGBytes;
	pic->bits = PNGBytes*8;
	switch (PNGBytes)
	{
	case 1:
		// Library PiLibSDK supports only CF_A8 format
		pic->colfmt = CF_A8;
		MemFree(PNGPal);
		PNGPal = NULL;

/*
		if (PNGPal == NULL)
			*fmt = TEXIFMT_L8;
		else
			if (PNGTransPal)
				*fmt = TEXIFMT_PAL8A;
			else
				*fmt = TEXIFMT_PAL8;
*/
		break;

	case 2:
		// Not supported in PiLibSDK library
		PNGDelBuf();
		return NULL;

//		*fmt = TEXIFMT_A8L8;
//		break;

	case 3:
		//*fmt = TEXIFMT_R8G8B8;
		pic->colfmt = CF_B8G8R8;

		// swap color components
		{
			int w, h, wbs;
			u8 b;
			h = pic->h;
			u8* s = pic->data;
			wbs = pic->wb - pic->w*3;
			for (; h > 0; h--)
			{
				for (w = pic->w; w > 0; w--)
				{
					b = s[0];
					s[0] = s[2];
					s[2] = b;
					s += 3;
				}
				s += wbs;
			}
		}
		break;

	case 4:
		// *fmt = TEXIFMT_A8R8G8B8;
		pic->colfmt = CF_A8B8G8R8;

		// swap color components
		{
			int w, h, wbs;
			u8 b;
			h = pic->h;
			u8* s = pic->data;
			wbs = pic->wb - pic->w*4;
			for (; h > 0; h--)
			{
				for (w = pic->w; w > 0; w--)
				{
					b = s[0];
					s[0] = s[2];
					s[2] = b;
					s += 4;
				}
				s += wbs;
			}
		}
		break;
	}

//	*pal = PNGPal;
//	PNGPal = NULL;
//	*trans = PNGUseTrans ? &PNGTransCol : NULL;

	PNGOutPic = NULL;	// no output image
	return pic;
}

// decode JPEG image from file to image in CF_A8B8G8R8, CF_B8G8R8 or CF_A8 format
//  Returns sPic* image (allocated with MemAlloc()) or NULL on error.
sPic* PNGLoadFile(sFile* file)
{
	// create file stream
	sStream str;
	StreamReadFileInit(&str, file);

	// decore PNG image
	return PNGLoadStream(&str);
}

// decode JPEG image from buffer to image in CF_A8B8G8R8, CF_B8G8R8 or CF_A8 format
//  Returns sPic* image (allocated with MemAlloc()) or NULL on error.
sPic* PNGLoadBuf(const void* buf, int size)
{
	// create memory stream
	sStream str;
	StreamReadBufInit(&str, buf, size);

	// decore PNG image
	return PNGLoadStream(&str);
}

#endif // USE_PNG
