
// ****************************************************************************
//
//                              JPG images
//
// ****************************************************************************
// >>> This code uses JPEG library from IJG group http://www.ijg.org/ <<<

#include "../../includes.h"	// includes

#if USE_JPG		// 1=use JPG support, 0=not used (_lib/pic/lib_jpg*)

//#include <setjmp.h>

jmp_buf JPG_JmpBuf;

#define JPEG_INTERNALS
#include "jpeg/jconfig.h"
#include "jpeg/jinclude.h"
#include "jpeg/jmorecfg.h"
#include "jpeg/jpeglib.h"
#include "jpeg/jerror.h"
#include "jpeg/cderror.h"
#include "jpeg/cdjpeg.h"
#include "jpeg/jpegint.h"
#include "jpeg/jdct.h"
#include "jpeg/jversion.h"
#include "jpeg/jmemsys.h"

#include "jpeg/jaricom.c"
#include "jpeg/jcomapi.c"
#include "jpeg/jdapimin.c"
#include "jpeg/jdapistd.c"
#include "jpeg/jdarith.c"
#include "jpeg/jdatasrc.c"
#include "jpeg/jdcoefct.c"
#include "jpeg/jdcolor.c"
#include "jpeg/jddctmgr.c"
#include "jpeg/jdhuff.c"
#include "jpeg/jdinput.c"
#include "jpeg/jdmainct.c"
#include "jpeg/jdmarker.c"
#include "jpeg/jdmaster.c"
#include "jpeg/jdmerge.c"
#include "jpeg/jdpostct.c"
#include "jpeg/jdsample.c"
#include "jpeg/jerror.c"
#include "jpeg/jfdctflt.c"
#include "jpeg/jfdctfst.c"
#include "jpeg/jfdctint.c"
#include "jpeg/jidctflt.c"
#include "jpeg/jidctfst.c"
#include "jpeg/jidctint.c"
#include "jpeg/jmemmgr.c"
#include "jpeg/jmemnobs.c"
#include "jpeg/jquant1.c"
#include "jpeg/jquant2.c"
#include "jpeg/jutils.c"

u8*	JPEGOutBuf = NULL;		// output buffer
u8*	JPEGOutDst = NULL;		// destination pointer into output buffer
u8*	JPEGOutEnd = NULL;		// end of output buffer
int	JPEGWidth;			// image width
int	JPEGHeight;			// image height
int	JPEGBytes;			// bytes per pixel
int	JPEGWidthBytes;			// bytes per image row

extern "C" void _exit(int status);
extern "C" int _kill(int pid, int sig);
extern "C" int _getpid(void);
void _exit(int status) { while (True) { } }
int _kill(int pid, int sig) { return -1; }
int _getpid(void) { return 1; }

// null error message table
static const char * const cdjpeg_message_table[] = {
	NULL
};

// read data from stream
size_t JPG_FileRead(void* buffer, size_t size, size_t count, void* file)
{
	sStream* str = (sStream*)file;
	return (size_t)str->read(str, buffer, (u32)(size*count));
}

// write data to stream
size_t JPG_FileWrite(const void* buffer, size_t size, size_t count, void* file)
{
	sStream* str = (sStream*)file;
	return (size_t)str->write(str, buffer, (u32)(size*count));
}

// decode JPG image from stream to image in CF_B8G8R8 or CF_A8 format
//  Returns sPic* image (allocated with MemAlloc()) or NULL on error.
sPic* JPGLoadStream(sStream* str)
{
	sPic* pic = NULL;

	// check JPG header
	u8 hd[3];
	size_t n = str->read(str, hd, 3);
	str->seek(str, -3);
	if ((n!=3)||(hd[0]!=0xff)||(hd[1]!=0xd8)||(hd[2]!=0xff)) return NULL;

	// local variables
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	// error exit
	if (setjmp(JPG_JmpBuf)) return NULL;

	// create decompression object
	cinfo.client_data = NULL;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	// error exit
	if (setjmp(JPG_JmpBuf))
	{
		jpeg_destroy_decompress(&cinfo);
		return NULL;
	}

	// default error messages
	jerr.addon_message_table = cdjpeg_message_table;
	jerr.first_addon_message = 0;
	jerr.last_addon_message = 0;
	cinfo.err->trace_level = 0;

	// data source for decompression
	cinfo.dct_method = JDCT_FLOAT;
	cinfo.dither_mode = JDITHER_NONE;
	jpeg_stdio_src(&cinfo, (void*)str);

	// read file header
	jpeg_read_header(&cinfo, TRUE);

	// calculate output image dimension
	jpeg_calc_output_dimensions(&cinfo);
	JPEGWidth = cinfo.output_width;
	JPEGBytes = cinfo.output_components;
	JPEGHeight = cinfo.output_height;
	JPEGWidthBytes = JPEGWidth*JPEGBytes;
	if (	((JPEGBytes != 1) && (JPEGBytes != 3)) ||
		(JPEGWidth < 1) || (JPEGWidth > 32000) ||
		(JPEGHeight < 1) || (JPEGHeight > 32000))
	{
			jpeg_destroy_decompress(&cinfo); 
			return NULL;
	}

	// create output buffer
	int s = JPEGWidthBytes*JPEGHeight;
	pic = (sPic*)MemAlloc(s + SPIC_HEADER_SIZE + 10);
	if (pic == NULL) return NULL;
	pic->w = JPEGWidth;
	pic->h = JPEGHeight;
	pic->wb = JPEGWidthBytes;
	pic->colfmt = (JPEGBytes == 3) ? CF_B8G8R8 : CF_A8;
	pic->bits = JPEGBytes*8;
	JPEGOutBuf = &pic->data[0];
	JPEGOutDst = JPEGOutBuf;
	JPEGOutEnd = JPEGOutBuf + s;

	// error exit
	if (setjmp(JPG_JmpBuf))
	{
		jpeg_destroy_decompress(&cinfo);
		MemFree(pic);
		JPEGOutBuf = NULL;
		return NULL;
	}

	// start decompressor
	jpeg_start_decompress(&cinfo);

	// error exit
	if (setjmp(JPG_JmpBuf))
	{
		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
		MemFree(pic);
		JPEGOutBuf = NULL;
		return NULL;
	}

	// read image data
	while (cinfo.output_scanline < cinfo.output_height)
	{
		jpeg_read_scanlines(&cinfo, &JPEGOutDst, 1);
		JPEGOutDst += JPEGWidthBytes;
	}

	// finish decompression
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	JPEGOutBuf = NULL;
	return pic;
}

// decode JPG image from file to image in CF_B8G8R8 or CF_A8 format
//  Returns sPic* image (allocated with MemAlloc()) or NULL on error.
sPic* JPGLoadFile(sFile* file)
{
	// create file stream
	sStream str;
	StreamReadFileInit(&str, file);

	// decore JPG image
	return JPGLoadStream(&str);
}

// decode JPG image from buffer to image in CF_B8G8R8 or CF_A8 format
//  Returns sPic* image (allocated with MemAlloc()) or NULL on error.
sPic* JPGLoadBuf(const void* buf, int size)
{
	// create memory stream
	sStream str;
	StreamReadBufInit(&str, buf, size);

	// decore JPG image
	return JPGLoadStream(&str);
}

#endif // USE_JPG
