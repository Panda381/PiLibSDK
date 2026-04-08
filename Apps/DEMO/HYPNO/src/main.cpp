
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

// image
#define WIDTH2 800
#define HEIGHT2 800
#define FRAMESIZE2 (WIDTH2*HEIGHT2)

// Back buffer with source image, format sPic
ALIGNED u32 TempBuf[2+FRAMESIZE2];

// transformation matrix
sMat2D Mat;

// setup rotation transformation matrix
void SetMat(float a)
{
	// prepare transformation matrix
//  ws ... source image width
//  hs ... source image height
//  x0 ... reference point X on source image
//  y0 ... reference point Y on source image
//  wd ... destination image width (negative = flip image in X direction)
//  hd ... destination image height (negative = flip image in Y direction)
//  shearx ... shear image in X direction
//  sheary ... shear image in Y direction
//  rot ... rotate image (angle in radians)
//  tx ... shift in X direction (ws = whole image width)
//  ty ... shift in Y direction (hs = whole image height)
	Mat2D_PrepDrawImg(&Mat, WIDTH2, HEIGHT2, WIDTH2/2, HEIGHT2/2, WIDTH2*3/2, HEIGHT2*3/2, 0, 0, a, 0, 0);
}

int main()
{
	int x, y, k;
	float r, a, y2;
	u32* d;

	// inicialize source image
	sPic* pic = (sPic*)TempBuf;
	pic->w = WIDTH2;
	pic->h = HEIGHT2;
	pic->wb = WIDTH2*4;
	pic->colfmt = CF_A8B8G8R8;
	pic->bits = 32;

	// setup transformation matrix
	SetMat(0);

	// generate hypno image
	d = TempBuf+2;
	for (y = -HEIGHT2/2; y < HEIGHT2/2; y++)
	{
		for (x = -WIDTH2/2; x < WIDTH2/2; x++)
		{
			// radius (y is converted to ensure square image)
			y2 = (float)y*WIDTH2/HEIGHT2;
			r = sqrtf(x*x + y2*y2);

			// angle
			a = atan2f(y2, x);

			// intensity (+ small raster dithering)
			k = (int)(127*sinf(r*0.17f + a) + 127);

			// store pixel, on red color
			*d++ = (k & 0xff) | 0xff000000;
		}
	}

	// main loop
	a = 0;
	while (True)
	{
		SetMat(a);
		a -= 0.1f;
		if (a < 0) a += PI*2;

		// draw image with transformation
		DrawImgMat((const u8*)TempBuf, 0, 0, WIDTH, HEIGHT, &Mat, DRAWIMGMAT_CLAMP, COL_BLACK); 

		// update display
		DispUpdate();

		// keyboard
		int key = KeyGet();
		if (key == KEY_ESC) Reboot();	// Program exit
		if (key == KEY_SCREENSHOT) ScreenShot(); //  Screenshot - This may take a few seconds to write.
	}

	return 0;
}
