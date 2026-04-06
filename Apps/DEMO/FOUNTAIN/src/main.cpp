
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

INLINE s32 float2int(float num) { return (s32)floorf(num); }

/*
Orginal source code (BBC BASIC):

 10 MODE 0:XS=2:YS=2 
 20 VDU 23,1,0;0;0;0;
 30 VDU 29,640;512;
 40 GCOL 0,13
 50 A=640:B=A*A:C=512
 60 FOR X=0 TO A STEP XS:S=X*X:P=SQR(B-S)
 70   FOR I=-P TO P STEP 6*YS
 80     R=SQR(S+I*I)/A
 90     Q=(R-1)*SIN(24*R)
100     Y=INT(I/3+Q*C)
110     IF I=-P M=Y:N=Y
120     IF Y>M M=Y
130     IF Y<N N=Y
140     IF M=Y OR N=Y PLOT 69,NOTX,Y:PLOT 69,X,Y
150   NEXT:NEXT
160 REPEAT:UNTIL FALSE
*/

int main()
{
	int x, y, m, n, s;
	float i, p, q, r;
	float a = 0;
	m = 0;
	n = 0;

#define XS 1	// step in X direction
#define YS 1	// step in Y direction
#define WIDTH2	(WIDTH/2)
#define HEIGHT2	(HEIGHT/2)
#define COL	COL_MAGENTA

	// main loop
	while (True)
	{
		// keyboard
		int key = KeyGet();
		if (key == KEY_ESC) Reboot();	// Program exit
		if (key == KEY_SCREENSHOT) ScreenShot(); //  Screenshot - This may take a few seconds to write.

		// clear screen
		DrawClear();

		// loop in X direction
		for (x = 0; x < WIDTH2; x += XS)
		{
			s = x*x;
			p = sqrtf(WIDTH2*WIDTH2 - s);
			for (i = -p; i < p; i += 6*YS)
			{
				r = sqrtf(s + i*i)/WIDTH2;
				q = (r - 1)*sinf(24*r + a);
				y = float2int(i/3 + q*HEIGHT2);
				if (i == -p) { m = y; n = y; }
				if (y > m) m = y;
				if (y < n) n = y;
				if ((m == y) || (n == y))
				{
					DrawPoint(WIDTH2+x, HEIGHT2-y, COL);
					DrawPoint(WIDTH2-x, HEIGHT2-y, COL);
				}
			}
		}

		// display update
		DispUpdate();

		// animation
		a += 0.02f;
		if (a >= (float)PI2) a -= (float)PI2;
	}

	return 0;
}
