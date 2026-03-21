
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                                JPEG file                                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
// >>> This code uses JPEG library from IJG group http://www.ijg.org/ <<<
// (c) Miroslav Nemecek

#include <stdio.h>						// I/O functions
#include "jpg.h"
#include "jpeg\cdjpeg.h"

BYTE*	JPEGOutBuf = NULL;		// output buffer
BYTE*	JPEGOutDst = NULL;		// destination pointer into output buffer
BYTE*	JPEGOutEnd = NULL;		// end of output buffer
int		JPEGWidth;				// image width
int		JPEGHeight;				// image height
int		JPEGBytes;				// bytes per pixel
BOOL	JPEGError = FALSE;		// error flag
int		JPEGWidthBytes;			// bytes per image row

// error service
static void JPEGErrorExit(j_common_ptr cinfo)
{
	JPEGError = TRUE;
}

// null error message table
static const char * const cdjpeg_message_table[] = {
  NULL
};

///////////////////////////////////////////////////////////////////////////////
// load JPEG file (returns TRUE if OK)

BOOL JPEGLoad(FILE* file, BYTE** dst, int* w, int* h, int* wb, TEXIFMT* fmt)
{
	// check JPEG header
	BYTE hd[3];
	size_t n = fread(hd, 1, 3, file);
	if ((n!=3)||(hd[0]!=0xff)||(hd[1]!=0xd8)||(hd[2]!=0xff)) return FALSE;
	fseek(file, -3, SEEK_CUR);

	// local variables
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	// create decompression object
	JPEGError = FALSE;
	cinfo.err = jpeg_std_error(&jerr);
	cinfo.err->error_exit = JPEGErrorExit;
	jpeg_create_decompress(&cinfo);
	if (JPEGError) return FALSE;

	// default error messages
	jerr.addon_message_table = cdjpeg_message_table;
	jerr.first_addon_message = 0;
	jerr.last_addon_message = 0;
	cinfo.err->trace_level = 0;

	// data source for decompression
	cinfo.dct_method = JDCT_FLOAT;
	cinfo.dither_mode = JDITHER_NONE;
	jpeg_stdio_src(&cinfo, file);
	if (JPEGError) { jpeg_destroy_decompress(&cinfo); return FALSE; }

	// read file header
	jpeg_read_header(&cinfo, TRUE);
	if (JPEGError) { jpeg_destroy_decompress(&cinfo); return FALSE; }

	// calculate output image dimension
	jpeg_calc_output_dimensions(&cinfo);
	JPEGWidth = cinfo.output_width;
	JPEGBytes = cinfo.output_components;
	JPEGHeight = cinfo.output_height;
	JPEGWidthBytes = JPEGWidth*JPEGBytes;
	if (JPEGError ||
		((JPEGBytes != 1) && (JPEGBytes != 3)) ||
		(JPEGWidth < 1) || (JPEGWidth > TEXMAXWH) ||
		(JPEGHeight < 1) || (JPEGHeight > TEXMAXWH))
	{
			jpeg_destroy_decompress(&cinfo); 
			return FALSE;
	}

	// create output buffer
	int s = JPEGWidthBytes*JPEGHeight;
	free(JPEGOutBuf);
	JPEGOutBuf = (BYTE*)malloc(s);
	if (JPEGError) { jpeg_destroy_decompress(&cinfo); return FALSE; }
	JPEGOutDst = JPEGOutBuf;
	JPEGOutEnd = JPEGOutBuf + s;

	// start decompressor
	jpeg_start_decompress(&cinfo);
	if (JPEGError)
	{
		jpeg_destroy_decompress(&cinfo);
		free(JPEGOutBuf);
		JPEGOutBuf = NULL;
		return FALSE;
	}

	// read image data
	while (cinfo.output_scanline < cinfo.output_height)
	{
		jpeg_read_scanlines(&cinfo, &JPEGOutDst, 1);
		if (JPEGError)
		{
			jpeg_finish_decompress(&cinfo);
			jpeg_destroy_decompress(&cinfo);
			free(JPEGOutBuf);
			JPEGOutBuf = NULL;
			return FALSE;
		}
		JPEGOutDst += JPEGWidthBytes;
	}

	// finish decompression
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	// set output image properties
	*dst = JPEGOutBuf;
	*w = JPEGWidth;
	*h = JPEGHeight;
	*wb = JPEGWidthBytes;
	*fmt = (JPEGBytes == 3) ? TEXIFMT_B8G8R8 : TEXIFMT_L8;
	JPEGOutBuf = NULL;
	return TRUE;
}
