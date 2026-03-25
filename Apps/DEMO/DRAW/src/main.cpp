
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

#define RAND_XY		RandS16MinMax(-100, WIDTH+100)
#define RAND_WH		RandU16Max(100)
#define RAND_SWH	RandS16MinMax(-100,100)

int main()
{
	int x, y, i, key;
	Bool slow = False;
	u32 t, t2;

	u32* d = FrameBuffer.drawbuf;
	t = Time();
	while (True)
	{
		// keyboard
		key = KeyGet();
		if (key == KEY_ESC) Reboot();	// Program exit
		if (key == KEY_SCREENSHOT) ScreenShot(); //  Screenshot - This may take a few seconds to write.

		// (3) draw point
		DrawPoint(RAND_XY, RAND_XY, COL_RAND);
		DrawPoint(RAND_XY, RAND_XY, COL_RANDTRANS);
		//DrawPointInv(RAND_XY, RAND_XY);

		// (3) draw rectangle
		DrawRect(RAND_XY, RAND_XY, RAND_WH, RAND_WH, COL_RAND);
		DrawRect(RAND_XY, RAND_XY, RAND_WH, RAND_WH, COL_RANDTRANS);
		//DrawRectInv(RAND_XY, RAND_XY, RAND_WH, RAND_WH);

		// (3) draw horizontal line
		DrawHLine(RAND_XY, RAND_XY, RAND_WH, COL_RAND);
		DrawHLine(RAND_XY, RAND_XY, RAND_WH, COL_RANDTRANS);
		//DrawHLineInv(RAND_XY, RAND_XY, RAND_WH);

		// (3) draw vertical line
		DrawVLine(RAND_XY, RAND_XY, RAND_WH, COL_RAND);
		DrawVLine(RAND_XY, RAND_XY, RAND_WH, COL_RANDTRANS);
		//DrawVLineInv(RAND_XY, RAND_XY, RAND_WH);

		// (6) draw frame
		DrawFrame(RAND_XY, RAND_XY, RAND_WH, RAND_WH, COL_RAND, COL_RAND);
		DrawFrame(RAND_XY, RAND_XY, RAND_WH, RAND_WH, COL_RANDTRANS, COL_RANDTRANS);
		//DrawFrameInv(RAND_XY, RAND_XY, RAND_WH, RAND_WH);
		DrawFrameW(RAND_XY, RAND_XY, RAND_WH, RAND_WH, RandU16Max(20), COL_RAND);
		DrawFrameW(RAND_XY, RAND_XY, RAND_WH, RAND_WH, RandU16Max(20), COL_RANDTRANS);
		//DrawFrameWInv(RAND_XY, RAND_XY, RAND_WH, RAND_WH, RandU16Max(20));

		// (6) draw line
		x = RAND_XY; y = RAND_XY; DrawLine(x, y, x+RAND_SWH, y+RAND_SWH, COL_RAND);
		x = RAND_XY; y = RAND_XY; DrawLine(x, y, x+RAND_SWH, y+RAND_SWH, COL_RANDTRANS);
		//x = RAND_XY; y = RAND_XY; DrawLineInv(x, y, x+RAND_SWH, y+RAND_SWH);
		x = RAND_XY; y = RAND_XY; DrawLineW(x, y, x+RAND_SWH, y+RAND_SWH, RandU16Max(20), RandBool(), COL_RAND);
		x = RAND_XY; y = RAND_XY; DrawLineW(x, y, x+RAND_SWH, y+RAND_SWH, RandU16Max(20), RandBool(), COL_RANDTRANS);
		//x = RAND_XY; y = RAND_XY; DrawLineWInv(x, y, x+RAND_SWH, y+RAND_SWH, RandU16Max(20));

		// (3) Draw round
		DrawRound(RAND_XY, RAND_XY, RAND_WH, RandU8(), COL_RAND);
		DrawRound(RAND_XY, RAND_XY, RAND_WH, RandU8(), COL_RANDTRANS);
		//DrawRoundInv(RAND_XY, RAND_XY, RAND_WH, RandU8());

		// (3) Draw circle or arc
		DrawCircle(RAND_XY, RAND_XY, RAND_WH, RandU8(), COL_RAND);
		DrawCircle(RAND_XY, RAND_XY, RAND_WH, RandU8(), COL_RANDTRANS);
		//DrawCircleInv(RAND_XY, RAND_XY, RAND_WH, RandU8());

		// (3) Draw ring
		i = RAND_WH;
		DrawRing(RAND_XY, RAND_XY, i + RandU16Max(20), i, RandU8(), COL_RAND);
		DrawRing(RAND_XY, RAND_XY, i + RandU16Max(20), i, RandU8(), COL_RANDTRANS);
		//DrawRingInv(RAND_XY, RAND_XY, i + RandU16Max(20), i, RandU8());

		// (3) Draw triangle
		x = RAND_XY;
		y = RAND_XY;
		DrawTriangle(x+RAND_SWH, y+RAND_SWH, x+RAND_SWH, y+RAND_SWH, x+RAND_SWH, y+RAND_SWH, COL_RAND);
		DrawTriangle(x+RAND_SWH, y+RAND_SWH, x+RAND_SWH, y+RAND_SWH, x+RAND_SWH, y+RAND_SWH, COL_RANDTRANS);
		//DrawTriangleInv(x+RAND_SWH, y+RAND_SWH, x+RAND_SWH, y+RAND_SWH, x+RAND_SWH, y+RAND_SWH);

		// (1) Draw fill area (no blending)
		DrawFill(x+RAND_SWH, y+RAND_SWH, COL_RAND);

		// (6) draw fixed text
		FrameBuffer.inv = RandBool();
		DrawCharScale('A', RAND_XY, RAND_XY, RandU8MinMax(1,4), RandU8MinMax(1,4), COL_RAND);
		DrawCharScale('A', RAND_XY, RAND_XY, RandU8MinMax(1,4), RandU8MinMax(1,4), COL_RANDTRANS);
		//DrawCharScaleInv('A', RAND_XY, RAND_XY, RandU8MinMax(1,4), RandU8MinMax(1,4));
		DrawTextScale("Fixed font", RAND_XY, RAND_XY, RandU8MinMax(1,4), RandU8MinMax(1,4), COL_RAND);
		DrawTextScale("Fixed font", RAND_XY, RAND_XY, RandU8MinMax(1,4), RandU8MinMax(1,4), COL_RANDTRANS);
		//DrawTextScaleInv("Fixed font", RAND_XY, RAND_XY, RandU8MinMax(1,4), RandU8MinMax(1,4));

		// (6) draw proportional text
		DrawCharProp('B', RAND_XY, RAND_XY, False, COL_RAND);
		DrawCharProp('B', RAND_XY, RAND_XY, False, COL_RANDTRANS);
		//DrawCharPropInv('B', RAND_XY, RAND_XY, False);
		DrawTextProp("Proportional", RAND_XY, RAND_XY, COL_RAND);
		DrawTextProp("Proportional", RAND_XY, RAND_XY, COL_RANDTRANS);
		//DrawTextPropInv("Proportional", RAND_XY, RAND_XY);

		// (7) draw image
		DrawImg(Test1Img, RAND_XY, RAND_XY, 0, 0, IMG_MAXWH, IMG_MAXWH, RandU8());
		DrawImg(Test2Img, RAND_XY, RAND_XY, 0, 0, IMG_MAXWH, IMG_MAXWH, RandU8());
		//DrawImgInv(Test1Img, RAND_XY, RAND_XY, 0, 0, IMG_MAXWH, IMG_MAXWH);
		DrawImgMask(Test3Img, RAND_XY, RAND_XY, 0, 0, IMG_MAXWH, IMG_MAXWH, COL_BLACK);
		DrawImgMask(Test3Img, RAND_XY, RAND_XY, 0, 0, IMG_MAXWH, IMG_MAXWH, COL_RAND);
		DrawImgMask(Test3Img, RAND_XY, RAND_XY, 0, 0, IMG_MAXWH, IMG_MAXWH, COL_RANDTRANS);
		//DrawImgMaskInv(Test3Img, RAND_XY, RAND_XY, 0, 0, IMG_MAXWH, IMG_MAXWH);

		// (3) draw ellipse
		DrawEllipse(RAND_XY, RAND_XY, RAND_WH, RAND_WH, RandU8(), COL_RAND);
		DrawEllipse(RAND_XY, RAND_XY, RAND_WH, RAND_WH, RandU8(), COL_RANDTRANS);
		//DrawEllipseInv(RAND_XY, RAND_XY, RAND_WH, RAND_WH, RandU8());

		// (3) Draw filled ellipse
		DrawFillEllipse(RAND_XY, RAND_XY, RAND_WH, RAND_WH, RandU8(), COL_RAND);
		DrawFillEllipse(RAND_XY, RAND_XY, RAND_WH, RAND_WH, RandU8(), COL_RANDTRANS);
		//DrawFillEllipseInv(RAND_XY, RAND_XY, RAND_WH, RAND_WH, RandU8());

		DispUpdate();

		// change mode after some time
		t2 = Time();
		if (slow)
		{
			if ((t2 - t) > 7000000)
			{
				slow = False;
				t = t2;
			}
			WaitMs(50);
		}
		else
		{
			if ((t2 - t) > 3000000)
			{
				slow = True;
				t = t2;
			}
		}
	}

	return 0;
}
