
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************
// Mandlebrot set

/*
Save slots: 10
Max. zoom: 10^225 (SizeN 748)
Max. digits: 230
Max. steps: 100000
Palettes: 4294967296
Animation sub-frames: 8

Mode SizeN levels:
0..11: float
12..40: double
41..135: float perturbation
136..748: double perturbation

Calculations are NEON optimised.
*/

#include "../include.h"

#define STEPS_MAX	100000		// max. number of steps
#define STEPS_BASE	1000		// base steps
#define STEPS_MIN	10		// minimal steps

#define PALETTE_NUM	0x1000		// number of palettes (must be power of 2)
#define PALETTE_MASK	(PALETTE_NUM-1)	// mask of palette index

#define SLOT_NUM	10		// count of save slots (index 0..9)

#define CARDIO_TEST	1		// 1=do cardioid tests (applied only on main and left bubble)

// mode auto-levels
#define MODE_LEVEL_FLT		11	// max. level for float (SizeN > 11 can be shifted or blurred, SizeN > 18 does not work)
#define MODE_LEVEL_DBL		40	// max. level for double (SizeN > 40 can be shifted, SizeN > 45 does not work)
#define MODE_LEVEL_FLT_PER	135	// max. level for float perturbation (SizeN > 142 does not work, de-normalized exponent -149 underflow)

// save file
int Slot = 0;			// current memory slot
#define MANDELPATH_MAX	256
char MandelPath[MANDELPATH_MAX+1]; // path with saves (without tailing '/')
char MandelPathFile[14];	// save file
#define MANDELSAVE_MAX	(2*FIX_DIGNUM+100) // max. length of save file (PalSet, SizeN, Steps, OffX, OffY)
char MandelSaveBuf[MANDELSAVE_MAX+5]; // save file buffer (reserve: 5x NL)

// 2nd back buffer
u32 BackBuf2[WIDTH*HEIGHT];
Bool DrawBackBuf2 = False;	// draw to back buffer 2

// base state (to save)
u32 PalSet = 54;		// selected color palette (0=custom)
int SizeN = 0;			// size step ... Size = 1 >> SizeN, Zoom = 1 << SizeN
int Steps = STEPS_BASE;		// number of steps
fix OffX;			// middle X coordinate
fix OffY;			// middle Y coordinate

// current mode
enum {
	MODE_FLT = 0,	// float
	MODE_DBL,	// double
	MODE_FLT_PER,	// float perturbation
	MODE_DBL_PER,	// double perturbation
};

volatile int Mode = MODE_FLT;

// reference orbit of the perturbation method
float RefZrFlt[STEPS_MAX];	// reference point Zr float
float RefZiFlt[STEPS_MAX];	// reference point Zi float
double RefZrDbl[STEPS_MAX];	// reference point Zr double
double RefZiDbl[STEPS_MAX];	// reference point Zi double
int RefMaxLen;			// current valid length of the reference orbit (RefMaxLen <= Steps)
Bool Info = False;		// display info

// Mandelbrot state
fix Size;		// Size = 1 >> SizeN, Zoom = 1/Size
fix Size15;		// Size15 = 1.5*Size
fix Size2;		// Size2 = 2*Size
fix Height3;		// Height3 = -3/HEIGHT
fix Width4;		// Width4 = 4/WIDTH
fix X0i;		// start X coordinate
fix Y0i;		// start Y coordinate
fix SXi;		// X increment
fix SYi;		// Y increment
fix Cii;		// current Y coordinate

// graphics state
int CurY = HEIGHT;	// current graphics Y (HEIGHT = not active)
int IncY = 1;		// current increment of graphics Y
Bool Ok[HEIGHT];	// processed graphics lines
u32* Dst;		// current buffer
int DstInc;		// increment Dst
u32 StartTime;		// start time of render
u32 StopTime;		// stop time of render

// multi-core
#if CORES>1
u32* volatile CoreDst[CORES] = { NULL, NULL, NULL, NULL }; // cores "dst" (NULL = core is ready to accept new task)
fix CoreCii[CORES];	// cores "Cii"
#endif

u32 Palette[PALETTE_NUM];

// gradient for custom palette 0
const u32 Grad[] = {
	COLOR(0,0,84),
	COLOR(0,0,110),
	COLOR(0,0,140),
	COLOR(0,0,160),
	COLOR(0,0,195),
	COLOR(0,10,195),
	COLOR(0,20,195),
	COLOR(0,30,195),
	COLOR(0,41,195),
	COLOR(0,50,195),
	COLOR(0,62,195),
	COLOR(0,72,195),
	COLOR(0,84,195),
	COLOR(0,94,195),
	COLOR(0,100,195),
	COLOR(0,109,195),
	COLOR(0,117,195),
	COLOR(0,130,205),
	COLOR(0,140,220),
	COLOR(0,155,235),
	COLOR(0,166,255),
	COLOR(0,175,255),
	COLOR(0,182,255),
	COLOR(0,190,255),
	COLOR(0,195,255),
	COLOR(0,205,255),
	COLOR(0,210,255),
	COLOR(0,220,255),
	COLOR(0,230,255),
	COLOR(0,230,240),
	COLOR(0,230,220),
	COLOR(0,230,210),
	COLOR(0,230,195),
	COLOR(0,220,195),
	COLOR(0,210,195),
	COLOR(0,202,195),
	COLOR(0,195,195),
	COLOR(0,195,160),
	COLOR(0,195,130),
	COLOR(0,195,110),
	COLOR(0,195,84),
	COLOR(10,195,64),
	COLOR(20,195,40),
	COLOR(30,195,20),
	COLOR(41,195,0),
	COLOR(51,195,0),
	COLOR(60,195,0),
	COLOR(70,195,0),
	COLOR(84,195,0),
	COLOR(94,205,0),
	COLOR(100,210,0),
	COLOR(108,220,0),
	COLOR(117,230,0),
	COLOR(130,235,0),
	COLOR(142,245,0),
	COLOR(150,250,0),
	COLOR(166,255,0),
	COLOR(172,255,0),
	COLOR(180,255,0),
	COLOR(186,255,0),
	COLOR(195,255,0),
	COLOR(205,255,0),
	COLOR(210,255,0),
	COLOR(220,255,0),
	COLOR(230,255,0),
	COLOR(236,255,0),
	COLOR(242,255,0),
	COLOR(250,255,0),
	COLOR(255,255,0),
	COLOR(255,248,0),
	COLOR(255,242,0),
	COLOR(255,235,0),
	COLOR(255,230,0),
	COLOR(255,220,0),
	COLOR(255,210,0),
	COLOR(255,204,0),
	COLOR(255,195,0),
	COLOR(255,188,0),
	COLOR(255,180,0),
	COLOR(255,172,0),
	COLOR(255,166,0),
	COLOR(255,150,0),
	COLOR(255,140,0),
	COLOR(255,130,0),
	COLOR(255,117,0),
	COLOR(255,108,0),
	COLOR(255,100,0),
	COLOR(255,95,0),
	COLOR(255,84,0),
	COLOR(255,64,0),
	COLOR(255,40,0),
	COLOR(255,20,0),
	COLOR(255,0,0),
	COLOR(255,0,20),
	COLOR(255,0,40),
	COLOR(255,0,60),
	COLOR(255,0,84),
	COLOR(255,0,100),
	COLOR(255,0,120),
	COLOR(255,0,150),
	COLOR(255,0,195),
	COLOR(255,10,205),
	COLOR(255,20,220),
	COLOR(255,30,240),
	COLOR(255,41,255),
	COLOR(255,51,255),
	COLOR(255,60,255),
	COLOR(255,70,255),
	COLOR(255,84,255),
	COLOR(255,90,255),
	COLOR(255,100,255),
	COLOR(255,108,255),
	COLOR(255,117,255),
	COLOR(255,130,255),
	COLOR(255,140,255),
	COLOR(255,150,255),
	COLOR(255,166,255),
	COLOR(255,172,255),
	COLOR(255,180,255),
	COLOR(255,188,255),
	COLOR(255,195,255),
	COLOR(255,210,255),
	COLOR(255,220,255),
	COLOR(255,240,255),
	COLOR(255,255,255),
};

static double HueToRGB(double p, double q, double t)
{
	if (t < 0.0) t += 1.0;
	if (t > 1.0) t -= 1.0;
	if (t < 1.0/6.0) return p + (q - p)*6.0*t;
	if (t < 1.0/2.0) return q;
	if (t < 2.0/3.0) return p + (q - p)*(2.0/3.0 - t)*6.0;
	return p;
}

// h, s, l in range 0..1
u32 HSLtoBGR(double h, double s, double l)
{
	double r, g, b;
	if (s <= 0.0)
	{
		r = g = b = l;
	}
	else
	{
		double q = (l < 0.5) ? (l*(1.0+s)) : (l + s - l*s);
		double p = 2.0*l - q;
		r = HueToRGB(p, q, h + 1.0/3.0);
		g = HueToRGB(p, q, h);
		b = HueToRGB(p, q, h - 1.0/3.0);
	}

	u32 ir = (u32)(r*255.0 + 0.5);
	u32 ig = (u32)(g*255.0 + 0.5);
	u32 ib = (u32)(b*255.0 + 0.5);
	if (ir > 255) ir = 255;
	if (ig > 255) ig = 255;
	if (ib > 255) ib = 255;

	return 0xff000000 | (ib << 16) | (ig << 8) | ir;
}

//#define LOG20	0.69314718055994531	// log(2.0)

// fast log2() function - split number to integer and fractional part (fraction = 0..65536)
INLINE int FastLog2Bits(double x, u32* frac16)
{
	union { double d; u64 n; } vx;
	vx.d = x;
	int e = (int)((vx.n >> 52) & 0x7FF) - 1023;	// get exponent
	*frac16 = (u32)((vx.n >> (52 - 16)) & 0xFFFF);	// get 16 top bits of the mantissa
	return e;
}

// get color from mod2 and iteration
u32 GetColor(double mod2, int i)
{
	// underflow protection
	if (mod2 < 1.0000001) mod2 = 1.0000001;

	// get log of amplitude of escape vector
	//  double log_zn = log(mod2)/2;
	//	-> log2(mod2) = e1 + f1/65536
	u32 f1;
	int e1 = FastLog2Bits(mod2, &f1);
	double L1 = (double)e1 + (double)f1 * (1.0 / 65536.0);

	// get smooth escape speed
	//  double nu = (double)(i + 1) - log(log_zn / LOG20) / LOG20;
	//	-> log2(L1) = e2 + f2/65536
	u32 f2;
	int e2 = FastLog2Bits(L1, &f2);

	// split smooth iteration
	// i = (int)nu;		// integer part
	// double t = nu - i;	// fraction part
	//	nu = i + 2 - (e2 + f2/65536)
	int ii = i + 2 - e2;
	u32 tt; // 0..255, fractional part for interpolation
	if (f2 == 0)
		tt = 0;
	else
	{
		ii--;
		tt = (65536 - f2) >> 8;
	}
	if (ii < 0) { ii = 0; tt = 0; }

	// get colors
	u32 col1 = Palette[ii & PALETTE_MASK];
	u32 col2 = Palette[(ii + 1) & PALETTE_MASK];

	// interpolate
	int r1 = col1 & 0xff;
	int g1 = (col1 >> 8) & 0xff;
	int b1 = (col1 >> 16) & 0xff;

	int r2 = col2 & 0xff;
	int g2 = (col2 >> 8) & 0xff;
	int b2 = (col2 >> 16) & 0xff;
	
	int r = r1 + (int)((tt * (r2 - r1)) >> 8);
	int g = g1 + (int)((tt * (g2 - g1)) >> 8);
	int b = b1 + (int)((tt * (b2 - b1)) >> 8);

	return 0xff000000 | r | ((u32)g << 8) | ((u32)b << 16);
}

// generate palettes
void GenPalette()
{
	int i, j, lenmin, lenmax, len, hnum, hbase;
	double smin, smax, hmin, hmax, h, s, l, dl;
	Bool up;

	// custom palette 0
	if (PalSet == 0)
	{
		up = True;
		j = 0;
		for (i = 0; i < PALETTE_NUM; i++)
		{
			Palette[i] = Grad[j];
			if (up)
			{
				j++;
				if (j >= count_of(Grad))
				{
					j--;
					up = False;
				}
			}
			else
			{
				j--;
				if (j < 0)
				{
					j++;
					up = True;
				}
			}
		}
		return;
	}

	// random setup
	RandSet(PalSet);
	RandShift();
	RandShift();
	RandShift();

	// prepare number of base colors and base hue
	hnum = RandU32Max(8);
	hbase = RandDouble();

	// prepare saturation range
	smin = RandDouble();
	smax = RandDoubleMinMax(smin, 1);

	// prepare hue range
	if (hnum == 0)
		hmin = RandDouble();
	else
		hmin = RandDoubleMax(0.25/hnum);
	hmax = RandDoubleMinMax(hmin, 1);

	// prepare length range
	lenmin = RandU32MinMax(5, 50);
	lenmax = RandU32MinMax(lenmin, 200);

	// generate colors
	dl = h = s = l = 0;
	len = 0;
	up = False;	// last direction was "to the black"
	for (i = 0; i < PALETTE_NUM; i++)
	{
		// generate new color
		len--;
		if (len <= 0)
		{
			// generate new color
			switch (hnum)
			{
			// hue smooth range
			case 0:
				h = RandDoubleMinMax(hmin, hmax);
				break;

			// 1 color
			case 1:
				h = RandDoubleMinMax(hbase - hmin, hbase + hmin);
				break;

			// 2..8 colors
			default:
				h = hbase + 1.0/hnum*RandU32Max(hnum-1);
				h = RandDoubleMinMax(h - hmin, h + hmin);
				break;
			}
			if (h < 0) h += 1;
			if (h > 1) h -= 1;

			s = RandDoubleMinMax(smin, smax);	// saturation
			len = RandU32MinMax(lenmin, lenmax);	// color length
			dl = 1.0/len;				// light increment
			if (up) dl = -dl;

			// change direction
			up = !up;
		}

		// generate this color
		Palette[i] = HSLtoBGR(h, s, l);

		// shift luminance
		l += dl;
		if (l >= 1) l = 1;
		if (l <= 0) l = 0;
	}
}

// convert SizeN to Size
void SetupSize()
{
	// prepare Size
	Size.SetOne();
	int i;
	for (i = SizeN; i > 0; i--) Size.Div2(&Size);

	// prepare Size15 = 1.5*Size
	Size15.Div2(&Size);
	Size15.Add(&Size15, &Size);

	// prepare Size2 = 2*Size
	Size2.Mul2(&Size);
}

// prepare reference orbit for perturbation mode (call once per frame)
void MandelPrepPer()
{
	fix zr, zi, zr2, zi2, tmp;
	double zrd, zid;

	zr.SetZero();
	zi.SetZero();

	int i;
	int steps = Steps;
	for (i = 0; i < steps; i++)
	{
		zrd = zr.ToDouble();
		zid = zi.ToDouble();
		RefZrDbl[i] = zrd;
		RefZiDbl[i] = zid;
		RefZrFlt[i] = (float)zrd;
		RefZiFlt[i] = (float)zid;

		zr2.Sqr(&zr);
		zi2.Sqr(&zi);
		tmp.Add(&zr2, &zi2);
		if (tmp.m_data[0] > (FIX_BASE)(4ull << (FIX_BITS - FIX_INTBITS))) break;

		zi.Mul(&zr, &zi);
		zi.Mul2(&zi);
		zi.Add(&zi, &OffY);

		zr2.Sub(&zr2, &zi2);
		zr.Add(&zr2, &OffX);
	}
	RefMaxLen = i;
}

// update Y increment
void MandelUpdate()
{
	// current Y
	CurY = 0;

	// prepare Y increment
	//  ci = ((double)(HEIGHT-y)/HEIGHT - 0.5f)*3*size + offy;
	//  ci = y*sy + y0;
	//  SYd = (double)-3*Size/HEIGHT * IncY;
	SYi.Mul(&Size, &Height3);
	SYi.MulInt(IncY);

	// start Ci coordinate Y
	Cii.Copy(&Y0i);

	// pointer to buffer
	Dst = FrameBuffer.drawbuf;
	DstInc = IncY * WIDTH;	// address increment
}

// start new image
void MandelStart(Bool clear = True)
{
	// get start time
	StartTime = Time();

	// current Y increment
	IncY = 1;
	while (IncY*2 < HEIGHT) IncY *= 2;

	// prepare X increment
	//  cr = ((double)x/WIDTH - 0.5f)*4*size + offx;
	//  cr = x*sx + x0;
	//  SXi.FromDbl((double)4*Size/WIDTH);
	SXi.Mul(&Size, &Width4);

	// prepare X start coordinate
	X0i.Sub(&OffX, &Size2);

	// prepare Y start coordinate
	// ci = ((double)(HEIGHT-y)/HEIGHT - 0.5f)*3*size + offy;
	// ci = y*sy + y0;
	// Y0d = OffY + 1.5f*Size;
	Y0i.Add(&OffY, &Size15);

	// prepare current mode
	if (SizeN <= MODE_LEVEL_FLT)
		Mode = MODE_FLT;
	else if (SizeN <= MODE_LEVEL_DBL)
		Mode = MODE_DBL;
	else if (SizeN <= MODE_LEVEL_FLT_PER)
	{
		Mode = MODE_FLT_PER;
		MandelPrepPer();	// prepare reference orbit (call once per frame)
	}
	else
	{
		Mode = MODE_DBL_PER;
		MandelPrepPer();	// prepare reference orbit (call once per frame)
	}

	// clear output buffer, set to black color
	if (clear) DrawClear();

	// all lines are invalid
	memset(Ok, False, HEIGHT*sizeof(Bool));

	// update Y increment
	MandelUpdate();
}

#if RASPPI == 1

// calculate Mandelbrot line using float - base, on Raspberry Zero 1 without NEON instructions
void MandelFlt(u32* dst, fix* ci)
{
	int x, i;
	float zr, zi, zr2, zi2, cr, sx, cii, mod2;
	int steps = Steps;
	int w = WIDTH;

	// prepare cr
	sx = (float)SXi.ToDouble();	// X increment
	cr = (float)X0i.ToDouble();	// start X coordinate
	cii = (float)ci->ToDouble();	// Y coordinate

#if CARDIO_TEST		// 1=do cardioid tests (applied only on main and left bubble)
	float xq, q, x1, cii2;
	cii2 = cii*cii;			// for cardioid test
#endif

	// X loop	
	for (x = 0; x < w; x++)
	{
#if CARDIO_TEST		// 1=do cardioid tests (applied only on main and left bubble)
		// We perform the cardioid test only in float mode, because that
		// is the only case in which the main cardioid is rendered.

		// main cardioid test (applied when the main bubble is drawn)
		xq = cr - 0.25f;
		q = xq*xq + cii2;
		if (q * (q + xq) < 0.25f * cii2)
		{
			*dst = COL_BLACK;
			dst++;
			cr = cr + sx;
			continue;
		}

		// period-2 bubble test (applies when the bubble on the left is drawn)
		x1 = cr + 1.0f;
		if (x1*x1 + cii2 < 0.0625f)
		{
			*dst = COL_BLACK;
			dst++;
			cr = cr + sx;
			continue;
		}
#endif // CARDIO_TEST

		// start condition
		zr = cr;
		zi = cii;
		mod2 = 0;

		// main iteration
		for (i = 0; i < steps; i++)
		{
			zr2 = zr*zr;
			zi2 = zi*zi;
			mod2 = zr2 + zi2;
			if (mod2 > 4.0f) break;

			zr = zr * zi;
			zr = zr*2;
			zi = zr + cii;

			zr2 = zr2 - zi2;
			zr = zr2 + cr;
		}

		// set color
		if (i >= steps)
			*dst = COL_BLACK;
		else
			*dst = GetColor(mod2, i);

		dst++;
		cr = cr + sx;
	}
}

#else // RASPPI == 1

#if AARCH==32
INLINE u32 vmaxvq_u32(uint32x4_t v)
{
	uint32x2_t lo = vget_low_u32(v);   // lane 0,1
	uint32x2_t hi = vget_high_u32(v);  // lane 2,3
	uint32x2_t m  = vmax_u32(lo, hi);  // max(0,2), max(1,3)
	m = vpmax_u32(m, m);               // max both -> both half are identical
	return vget_lane_u32(m, 0);
}

INLINE u32 vminvq_u32(uint32x4_t v)
{
	uint32x2_t lo = vget_low_u32(v);   // lane 0,1
	uint32x2_t hi = vget_high_u32(v);  // lane 2,3
	uint32x2_t m  = vmin_u32(lo, hi);  // min(0,2), min(1,3)
	m = vpmin_u32(m, m);               // min both -> both half are identical
	return vget_lane_u32(m, 0);
}
#endif // AARCH==32

// calculate Mandelbrot line using float - using NEON vectorization, on Raspberry Zero 2
void MandelFlt(u32* dst, fix* ci)
{
	int i, x;
	float sx, sx4, cr, cii, cii2;
	int steps = Steps;
	int w = WIDTH;

	sx = (float)SXi.ToDouble();	// X increment
	sx4 = 4*sx;			// 4 * X-increment
	cr = (float)X0i.ToDouble();	// start X
	cii = (float)ci->ToDouble();	// Y coordinate

	float32x4_t v_cii  = vdupq_n_f32(cii);		// current Y coordinate
	float32x4_t v_four = vdupq_n_f32(4.0f);		// constant 4.0

#if CARDIO_TEST		// 1=do cardioid tests (applied only on main and left bubble)
	cii2 = cii * cii;
	float32x4_t v_cii2 = vdupq_n_f32(cii2);		// for cardioid test
	float32x4_t v_quarter = vdupq_n_f32(0.25f);
#endif

	// X loop
	for (x = 0; x < w; x += 4)
	{
		// prepare current X coordinate
		float cr2 = cr+sx;
		float cr3 = cr2+sx;
		float cr4 = cr3+sx;
		float32x4_t v_cr = { cr, cr2, cr3, cr4 };

#if CARDIO_TEST		// 1=do cardioid tests (applied only on main and left bubble)
		// We perform the cardioid test only in float mode, because that
		// is the only case in which the main cardioid is rendered.

		// cardioid + bulb tests, on lanes
		float32x4_t v_xq = vsubq_f32(v_cr, vdupq_n_f32(0.25f));	// xq = cr - 0.25
		float32x4_t v_q  = vaddq_f32(vmulq_f32(v_xq, v_xq), v_cii2); // q = xq*xq + cii2
		uint32x4_t v_in_cardioid = vcltq_f32(vmulq_f32(v_q, vaddq_f32(v_q, v_xq)), vmulq_f32(v_quarter, v_cii2)); // compare q*(q+xq) < 0.25*cii2

		float32x4_t v_x1 = vaddq_f32(v_cr, vdupq_n_f32(1.0f)); // x1 = cr + 1.0
		uint32x4_t v_in_bulb = vcltq_f32(vaddq_f32(vmulq_f32(v_x1, v_x1), v_cii2), vdupq_n_f32(0.0625f)); // compare x1*x1 + cii2 < 0.0625

		uint32x4_t v_black = vorrq_u32(v_in_cardioid, v_in_bulb); // black pixels inside cardioids

		// if all 4 pixels in the block are black, skip the entire loop
		if (vminvq_u32(v_black) != 0)
		{
			dst[0] = dst[1] = dst[2] = dst[3] = COL_BLACK;
			dst += 4;
			cr += sx4;
			continue;
		}
		uint32x4_t v_active = vmvnq_u32(v_black); // active pixels, if not black
#else
		uint32x4_t v_active = vdupq_n_u32(~0u);	// flags, all lanes are active
#endif // CARDIO_TEST

		// start condition
		float32x4_t v_zr = v_cr;		// Zr <- current X coordinate
		float32x4_t v_zi = v_cii;		// Zi <- current Y coordinate
		float32x4_t v_mod2 = vdupq_n_f32(0.0f);	// clear mod2
		int32x4_t v_iter = vdupq_n_s32(0);	// prepare iteration counter - escaped in iteration

		// main iteration
		for (i = 0; i < steps; i++)
		{
			float32x4_t v_zr2 = vmulq_f32(v_zr, v_zr);	// zr2 = zr*zr
			float32x4_t v_zi2 = vmulq_f32(v_zi, v_zi);	// zi2 = zi*zi
			float32x4_t v_m2  = vaddq_f32(v_zr2, v_zi2);	// m2 = zr2 + zi2

			// pixels escaped in this iteration
			//   vcgtq_f32 ... vector compare greater than (returns -1 on True)
			//   vandq_u32 ... bitwise AND
			uint32x4_t v_escaped_now = vandq_u32(vcgtq_f32(v_m2, v_four), v_active);

			// save mod2 and iter only if escaped right now
			//  vbslq_f32 ... bitwise select (mod2 <- select old "mod2" or new "m2" by result)
			v_mod2 = vbslq_f32(v_escaped_now, v_m2, v_mod2); // v_mod2 <- use old mod2 or new mod2
			int32x4_t v_i_now = vdupq_n_s32(i);		// v_i_now = current "i"
			v_iter = vbslq_s32(v_escaped_now, v_i_now, v_iter); // v_iter <- use old 'i' or new 'i'

			// deactivate the pixels that just leaked
			//  vbicq_u32 ... bitwise clear (AND NOT)
			v_active = vbicq_u32(v_active, v_escaped_now); // clear if not active

			// if all lanes are inactive, stop the iteration
			//  vmaxvq_u32 ... get maximal flag
			if (vmaxvq_u32(v_active) == 0) break;

			// iteration
			float32x4_t v_zrzi = vmulq_f32(v_zr, v_zi);	// zrzi <- zr*zi
			float32x4_t v_zi_new = vaddq_f32(vaddq_f32(v_zrzi, v_zrzi), v_cii); // zi_new = zrzi*2 + cii
			float32x4_t v_zr_new = vaddq_f32(vsubq_f32(v_zr2, v_zi2), v_cr); // zr_new = zr2 + cr

			// update only active lanes (select old or new results)
			v_zr = vbslq_f32(v_active, v_zr_new, v_zr);
			v_zi = vbslq_f32(v_active, v_zi_new, v_zi);
		}

		// load result
		//   vst1q_u32 ... store vector register to the memory
		uint32_t still_active[4];
		float mod2_arr[4];
		int32_t iter_arr[4];
		vst1q_u32(still_active, v_active);
		vst1q_f32(mod2_arr, v_mod2);
		vst1q_s32(iter_arr, v_iter);

#if CARDIO_TEST		// 1=do cardioid tests (applied only on main and left bubble)
		uint32_t black_mask[4];
		vst1q_u32(black_mask, v_black); // mask of invalid black pixels
#endif

		// set color
		u32 color;
		if (
#if CARDIO_TEST		// 1=do cardioid tests (applied only on main and left bubble)
			black_mask[0] ||
#endif
			still_active[0])
			color = COL_BLACK;
		else
			color = GetColor((double)mod2_arr[0], iter_arr[0]);
		dst[0] = color;

		if (
#if CARDIO_TEST		// 1=do cardioid tests (applied only on main and left bubble)
			black_mask[1] ||
#endif
			still_active[1])
			color = COL_BLACK;
		else
			color = GetColor((double)mod2_arr[1], iter_arr[1]);
		dst[1] = color;

		if (
#if CARDIO_TEST		// 1=do cardioid tests (applied only on main and left bubble)
			black_mask[2] ||
#endif
			still_active[2])
			color = COL_BLACK;
		else
			color = GetColor((double)mod2_arr[2], iter_arr[2]);
		dst[2] = color;

		if (
#if CARDIO_TEST		// 1=do cardioid tests (applied only on main and left bubble)
			black_mask[3] ||
#endif
			still_active[3])
			color = COL_BLACK;
		else
			color = GetColor((double)mod2_arr[3], iter_arr[3]);
		dst[3] = color;

		// next 4 pixels
		dst += 4;
		cr += sx4;
	}
}

#endif // RASPPI == 1

#if AARCH == 32

// calculate Mandelbrot line using double - base, on 32-bit modes without NEON instructions
void MandelDbl(u32* dst, fix* ci)
{
	int x, i;
	double zr, zi, zr2, zi2, cr, sx, cii, mod2;
	int steps = Steps;
	int w = WIDTH;

	// prepare cr
	sx = SXi.ToDouble();
	cr = X0i.ToDouble();
	cii = ci->ToDouble();

	// X loop	
	for (x = 0; x < w; x++)
	{
		// start condition
		zr = cr;
		zi = cii;
		mod2 = 0;

		// main iteration
		for (i = 0; i < steps; i++)
		{
			zr2 = zr*zr;
			zi2 = zi*zi;
			mod2 = zr2 + zi2;
			if (mod2 > 4.0) break;

			zr = zr * zi;
			zr = zr*2;
			zi = zr + cii;

			zr2 = zr2 - zi2;
			zr = zr2 + cr;
		}

		// set color
		if (i >= steps)
			*dst = COL_BLACK;
		else
			*dst = GetColor(mod2, i);
		dst++;
		cr = cr + sx;
	}
}

#else // AARCH == 32

// calculate Mandelbrot line using double - using NEON vectorization on 64-bit mode
void MandelDbl(u32* dst, fix* ci)
{
	int x, i;
	double cr, sx, sx2, cii;
	int steps = Steps;
	int w = WIDTH;

	sx = SXi.ToDouble();	// X-increment
	sx2 = sx+sx;		// 2 * X-increment
	cr = X0i.ToDouble();	// start X coordinate
	cii = ci->ToDouble();	// current Y coordinate

	float64x2_t v_cii  = vdupq_n_f64(cii);	// current Y double-coordinate
	float64x2_t v_four = vdupq_n_f64(4.0);	// constant 4.0

	// X loop
	for (x = 0; x < w; x += 2)
	{
		// prepare current X double-coordinate
		float64x2_t v_cr = { cr, cr+sx };

		// start condition
		float64x2_t v_zr = v_cr;		// Zr <- current X coordinate
		float64x2_t v_zi = v_cii;		// Zi <- current Y coordinate
		float64x2_t v_mod2 = vdupq_n_f64(0.0);	// clear mod2
		int64x2_t v_iter = vdupq_n_s64(0);      // prepare iteration counter - escaped in iteration
		uint64x2_t v_active = vdupq_n_u64(~0ull); // flags, both lanes are active

		// main iteration
		for (i = 0; i < steps; i++)
		{
			float64x2_t v_zr2 = vmulq_f64(v_zr, v_zr);	// zr2 = zr*zr
			float64x2_t v_zi2 = vmulq_f64(v_zi, v_zi);	// zi2 = zi*zi
			float64x2_t v_m2  = vaddq_f64(v_zr2, v_zi2);	// m2 = zr2 + zi2

			// pixels escaped in this iteration
			//   vcgtq_f64 ... vector compare greater than (returns -1 on True)
			//   vandq_u64 ... bitwise AND
			uint64x2_t v_escaped_now = vandq_u64(vcgtq_f64(v_m2, v_four), v_active);

			// save mod2 and iter only if escaped right now
			//  vbslq_f64 ... bitwise select (mod2 <- select old "mod2" or new "m2" by result)
			v_mod2 = vbslq_f64(v_escaped_now, v_m2, v_mod2); // v_mod2 <- use old mod2 or new mod2
			int64x2_t v_i_now = vdupq_n_s64(i);		// v_i_now = current "i"
			v_iter = vbslq_s64(v_escaped_now, v_i_now, v_iter); // v_iter <- use old 'i' or new 'i'

			// deactivate the pixels that just leaked
			//  vbicq_u64 ... bitwise clear (AND NOT)
			v_active = vbicq_u64(v_active, v_escaped_now); // clear if not active

			// if both lanes are inactive, stop the iteration
			//  vaddvq_u64 ... sum all active flags
			if (vaddvq_u64(v_active) == 0) break;

			// iteration
			float64x2_t v_zrzi = vmulq_f64(v_zr, v_zi);	// zrzi <- zr*zi
			float64x2_t v_zi_new = vaddq_f64(vaddq_f64(v_zrzi, v_zrzi), v_cii); // zi_new = zrzi*2 + cii
			float64x2_t v_zr_new = vaddq_f64(vsubq_f64(v_zr2, v_zi2), v_cr); // zr_new = zr2 + cr

			// update only active lanes (select old or new results)
			v_zr = vbslq_f64(v_active, v_zr_new, v_zr);
			v_zi = vbslq_f64(v_active, v_zi_new, v_zi);
		}

		// load result
		//   vst1q_u64 ... store vector register to the memory
		uint64_t still_active[2];
		double mod2_arr[2];
		int64_t iter_arr[2];
		vst1q_u64(still_active, v_active);
		vst1q_f64(mod2_arr, v_mod2);
		vst1q_s64(iter_arr, v_iter);

		// set color
		u32 color;
		if (still_active[0])
			color = COL_BLACK;
		else
			color = GetColor(mod2_arr[0], (int)iter_arr[0]);
		dst[0] = color;

		if (still_active[1])
			color = COL_BLACK;
		else
			color = GetColor(mod2_arr[1], (int)iter_arr[1]);
		dst[1] = color;

		// next 2 pixels
		dst += 2;
		cr += sx2;
	}
}

#endif // AARCH == 32

#if RASPPI == 1

// calculate Mandelbrot line using float perturbation - base, on Raspberry Zero 1 without NEON instructions
void MandelFltPer(u32* dst, fix* ci)
{
	int x, i;
	int w = WIDTH;		// screen width
	int steps = Steps;	// max number of iterations
	int reflen = RefMaxLen;	// max length of the reference

	fix cr, sx, dcrFix, dciFix;
	sx.Copy(&SXi);	// X-increment
	cr.Copy(&X0i);	// start X coordinate

	// delta-c real part
	dcrFix.Sub(&cr, &OffX);
	float dcr = (float)dcrFix.ToDouble();
	float dsx = (float)sx.ToDouble();	// delta X

	// delta-c imaginary part
	dciFix.Sub(ci, &OffY);
	float dci = (float)dciFix.ToDouble();

	// per-pixel loop
	for (x = 0; x < w; x++)
	{
		float dzr = 0;
		float dzi = 0;
		int refindex = 0;

		// main loop
		float mod2 = 0;
		for (i = 0; i < steps; i++)
		{
			float zr = RefZrFlt[refindex];
			float zi = RefZiFlt[refindex];

			// actual (full-precision) position at this iteration = reference + delta
			float ar = zr + dzr;
			float ai = zi + dzi;
			mod2 = ar*ar + ai*ai;
			if (mod2 > 4.0f) break;

			// glitch avoidance: if the reference orbit ran out, or the true value has
			// drifted so far from the reference that it is now closer to the origin
			// than the perturbation itself, rebase onto Z_0=0 using the actual value
			if ((refindex+1 >= reflen) || (mod2 < dzr*dzr+dzi*dzi))
			{
				dzr = ar;
				dzi = ai;
				zr = 0.0;
				zi = 0.0;
				refindex = 0;
			}

			// perturbation formula: dz_new = 2*Z*dz + dz^2 + dc
			float new_dzr = 2.0f*(zr*dzr - zi*dzi) + (dzr*dzr - dzi*dzi) + dcr;
			float new_dzi = 2.0f*(zr*dzi + zi*dzr + dzr*dzi) + dci;
			dzr = new_dzr;
			dzi = new_dzi;
			refindex++;
		}

		// output color
		if (i >= steps)
			*dst = COL_BLACK;	// reached end of reference orbit without escaping - treated as "inside"
		else
			*dst = GetColor(mod2, i);

		// next X
		dst++;
		dcr += dsx;
	}
}

#else // RASPPI == 1

// calculate Mandelbrot line using float perturbation - using NEON vectorization, on Raspberry Zero 2
void MandelFltPer(u32* dst, fix* ci)
{
	int i, x;
	int w = WIDTH;		// screen width
	int steps = Steps;	// max number of iterations
	int reflen = RefMaxLen;	// max length of the reference

	fix cr, sx, dcrFix, dciFix;
	sx.Copy(&SXi);		// X-increment
	cr.Copy(&X0i);		// start X coordinate

	// delta-c real part
	dcrFix.Sub(&cr, &OffX);
	float dcr_start = (float)dcrFix.ToDouble();
	float dsx = (float)sx.ToDouble(); // delta X

	// delta-c imaginary part
	dciFix.Sub(ci, &OffY);		// Y offset
	float dci = (float)dciFix.ToDouble();

	float32x4_t v_dci = vdupq_n_f32(dci);	// Y offset
	float32x4_t v_four = vdupq_n_f32(4.0f);	// constant 4.0
	float32x4_t v_step4 = vdupq_n_f32(dsx * 4.0f);	// 4*sx increment

	// start X values for lanes 0..3 { dcr, dcr + sx, dcr + 2*sx, dcr + 3*sc }
	float32x4_t v_idx = { 0.0f, 1.0f, 2.0f, 3.0f };
	float32x4_t v_dcr = vmlaq_f32(vdupq_n_f32(dcr_start), v_idx, vdupq_n_f32(dsx));

	// per-pixel loop
	for (x = 0; x < w; x += 4)
	{
		// start condition
		float32x4_t v_dzr = vdupq_n_f32(0.0f); // dzr = 0
		float32x4_t v_dzi = vdupq_n_f32(0.0f); // dzi = 0
		int32x4_t v_refidx = vdupq_n_s32(0);	// clear reference indices
		uint32x4_t v_active = vdupq_n_u32(~0u); // flags, all lanes are active
		float32x4_t v_mod2 = vdupq_n_f32(0.0f); // mod2 = 0
		int32x4_t v_iter = vdupq_n_s32(0); // prepare iteration counter - escaped in iteration

		// main iteration
		for (i = 0; i < steps; i++)
		{
			// get reference points: gather RefZr/RefZi; indices can differ
			int32_t idx0 = vgetq_lane_s32(v_refidx, 0);
			int32_t idx1 = vgetq_lane_s32(v_refidx, 1);
			int32_t idx2 = vgetq_lane_s32(v_refidx, 2);
			int32_t idx3 = vgetq_lane_s32(v_refidx, 3);

			float32x4_t v_zr = vld1q_lane_f32(&RefZrFlt[idx0], vdupq_n_f32(0.0f), 0);
			v_zr = vld1q_lane_f32(&RefZrFlt[idx1], v_zr, 1);
			v_zr = vld1q_lane_f32(&RefZrFlt[idx2], v_zr, 2);
			v_zr = vld1q_lane_f32(&RefZrFlt[idx3], v_zr, 3);

			float32x4_t v_zi = vld1q_lane_f32(&RefZiFlt[idx0], vdupq_n_f32(0.0f), 0);
			v_zi = vld1q_lane_f32(&RefZiFlt[idx1], v_zi, 1);
			v_zi = vld1q_lane_f32(&RefZiFlt[idx2], v_zi, 2);
			v_zi = vld1q_lane_f32(&RefZiFlt[idx3], v_zi, 3);

			// actual (full-precision) position at this iteration = reference + delta
			float32x4_t v_ar = vaddq_f32(v_zr, v_dzr); // ar = zr + dzr
			float32x4_t v_ai = vaddq_f32(v_zi, v_dzi); // ai = zi + dzi
			float32x4_t v_m2 = vmlaq_f32(vmulq_f32(v_ar, v_ar), v_ai, v_ai); // m2 = ar*ar + ai*ai

			// escape test
			uint32x4_t v_escaped_now = vandq_u32(vcgtq_f32(v_m2, v_four), v_active); // pixels escaped in this iteration (compare with 4.0)
			v_mod2 = vbslq_f32(v_escaped_now, v_m2, v_mod2); // v_mod2 <- use old mod2 or new mod2
			v_iter = vbslq_s32(v_escaped_now, vdupq_n_s32(i), v_iter); // v_iter <- use old 'i' or new 'i'

			// deactivate the pixels that just leaked
			v_active = vbicq_u32(v_active, v_escaped_now); // clear escaped lanes (set non-active)

			// if all lanes are inactive, stop the iteration
			if (vmaxvq_u32(v_active) == 0) break; // sum all active flags

			// glitch avoidance: if the reference orbit ran out, or the true value has
			// drifted so far from the reference that it is now closer to the origin
			// than the perturbation itself, rebase onto Z_0=0 using the actual value

			// glitch/rebase test: (refidx+1 >= reflen) || (mod2 < dzr^2+dzi^2)
			uint32x4_t v_ref_end = vcgeq_s32(vaddq_s32(v_refidx, vdupq_n_s32(1)), vdupq_n_s32(reflen)); // ref_end = compare(refidx + 1 >= reflen)
			float32x4_t v_dz2 = vmlaq_f32(vmulq_f32(v_dzr, v_dzr), v_dzi, v_dzi); // dz2 = dzr*dzr + dzi*dzi
			uint32x4_t v_glitch = vorrq_u32(v_ref_end, vcltq_f32(v_m2, v_dz2)); // glitch = ref_end OR compare(m2 < dz2)
			uint32x4_t v_rebase = vandq_u32(v_glitch, v_active); // setup condition flags - rebase only active lanes

			// reset iteration
			v_dzr = vbslq_f32(v_rebase, v_ar, v_dzr); // dzr = ar (on rebase condition)
			v_dzi = vbslq_f32(v_rebase, v_ai, v_dzi); // dzi = ai (on rebase condition)
			v_refidx = vbslq_s32(v_rebase, vdupq_n_s32(0), v_refidx); // refidx = 0 (on rebase condition)
			v_zr = vbslq_f32(v_rebase, vdupq_n_f32(0.0f), v_zr); // zr = 0 (on rebase condition)
			v_zi = vbslq_f32(v_rebase, vdupq_n_f32(0.0f), v_zi); // zi = 0 (on rebase condition)

			// perturbation formula Real: new_dzr = 2*(zr*dzr - zi*dzi) + (dzr^2 - dzi^2) + dcr
			float32x4_t v_t1 = vsubq_f32(vmulq_f32(v_zr, v_dzr), vmulq_f32(v_zi, v_dzi)); // t1 = zr*dzr - zi*dzi
			float32x4_t v_dzr2 = vmulq_f32(v_dzr, v_dzr); // dzr2 = dzr*dzr
			float32x4_t v_dzi2 = vmulq_f32(v_dzi, v_dzi); // dzi2 = dzi*dzi
			float32x4_t v_new_dzr = vaddq_f32(vaddq_f32(v_t1, v_t1), vaddq_f32(vsubq_f32(v_dzr2, v_dzi2), v_dcr)); // dzr = 2*t1 + ((dzr2 - dzi2) + dcr)

			// perturbation formula Imag: new_dzi = 2*(zr*dzi + zi*dzr + dzr*dzi) + dci
			float32x4_t v_t2 = vaddq_f32(vaddq_f32(vmulq_f32(v_zr, v_dzi), vmulq_f32(v_zi, v_dzr)), vmulq_f32(v_dzr, v_dzi)); // t2 = (zr*dzi + zi*dzr) + dzr*dzi
			float32x4_t v_new_dzi = vaddq_f32(vaddq_f32(v_t2, v_t2), v_dci); // dzi = 2*t2 + dci

			// update only active lanes (select old or new results)
			v_dzr = vbslq_f32(v_active, v_new_dzr, v_dzr);
			v_dzi = vbslq_f32(v_active, v_new_dzi, v_dzi);

			// refindex++ only for active lanes
			v_refidx = vaddq_s32(v_refidx, vandq_s32(vdupq_n_s32(1), vreinterpretq_s32_u32(v_active)));
		}

		// load result
		uint32_t active0 = vgetq_lane_u32(v_active, 0);
		uint32_t active1 = vgetq_lane_u32(v_active, 1);
		uint32_t active2 = vgetq_lane_u32(v_active, 2);
		uint32_t active3 = vgetq_lane_u32(v_active, 3);

		// set color
		if (active0)
			dst[0] = COL_BLACK;
		else
			dst[0] = GetColor((double)vgetq_lane_f32(v_mod2, 0), vgetq_lane_s32(v_iter, 0));

		if (active1)
			dst[1] = COL_BLACK;
		else
			dst[1] = GetColor((double)vgetq_lane_f32(v_mod2, 1), vgetq_lane_s32(v_iter, 1));

		if (active2)
			dst[2] = COL_BLACK;
		else
			dst[2] = GetColor((double)vgetq_lane_f32(v_mod2, 2), vgetq_lane_s32(v_iter, 2));

		if (active3)
			dst[3] = COL_BLACK;
		else
			dst[3] = GetColor((double)vgetq_lane_f32(v_mod2, 3), vgetq_lane_s32(v_iter, 3));

		// next 4 pixels
		dst += 4;
		v_dcr = vaddq_f32(v_dcr, v_step4); // dcr += 4*sx
	}
}

#endif // RASPPI == 1

#if AARCH == 32

// calculate Mandelbrot line using double perturbation - base, on 32-bit modes without NEON instructions
void MandelDblPer(u32* dst, fix* ci)
{
	int x, i;
	int w = WIDTH;		// screen width
	int steps = Steps;	// max number of iterations
	int reflen = RefMaxLen;	// max length of the reference

	fix cr, sx, dcrFix, dciFix;
	sx.Copy(&SXi);		// X-increment
	cr.Copy(&X0i);		// start X coordinate

	// delta-c real part
	dcrFix.Sub(&cr, &OffX);
	double dcr = dcrFix.ToDouble();
	double dsx = sx.ToDouble();	// delta X

	// delta-c imaginary part
	dciFix.Sub(ci, &OffY);
	double dci = dciFix.ToDouble();

	// per-pixel loop
	for (x = 0; x < w; x++)
	{
		double dzr = 0;
		double dzi = 0;
		int refindex = 0;

		// main loop
		double mod2 = 0;
		for (i = 0; i < steps; i++)
		{
			double zr = RefZrDbl[refindex];
			double zi = RefZiDbl[refindex];

			// actual (full-precision) position at this iteration = reference + delta
			double ar = zr + dzr;
			double ai = zi + dzi;
			mod2 = ar*ar + ai*ai;
			if (mod2 > 4.0) break;

			// glitch avoidance: if the reference orbit ran out, or the true value has
			// drifted so far from the reference that it is now closer to the origin
			// than the perturbation itself, rebase onto Z_0=0 using the actual value
			if ((refindex+1 >= reflen) || (mod2 < dzr*dzr+dzi*dzi))
			{
				dzr = ar;
				dzi = ai;
				zr = 0.0;
				zi = 0.0;
				refindex = 0;
			}

			// perturbation formula: dz_new = 2*Z*dz + dz^2 + dc
			double new_dzr = 2.0*(zr*dzr - zi*dzi) + (dzr*dzr - dzi*dzi) + dcr;
			double new_dzi = 2.0*(zr*dzi + zi*dzr + dzr*dzi) + dci;
			dzr = new_dzr;
			dzi = new_dzi;
			refindex++;
		}

		// output color
		if (i >= steps)
			*dst = COL_BLACK;	// reached end of reference orbit without escaping - treated as "inside"
		else
			*dst = GetColor(mod2, i);

		// next X
		dst++;
		dcr += dsx;
	}
}

#else // AARCH == 32

// calculate Mandelbrot line using double perturbation - using NEON vectorization on 64-bit mode
void MandelDblPer(u32* dst, fix* ci)
{
	int x, i;
	int w = WIDTH;		// screen width
	int steps = Steps;	// max number of iterations
	int reflen = RefMaxLen;	// max length of the reference

	fix cr, sx, dcrFix, dciFix;
	sx.Copy(&SXi);		// X-increment
	cr.Copy(&X0i);		// start X coordinate

	// delta-c real part
	dcrFix.Sub(&cr, &OffX);
	double dcr_start = dcrFix.ToDouble();
	double dsx = sx.ToDouble(); // delta X

	// delta-c imaginary part
	dciFix.Sub(ci, &OffY);	// Y offset
	double dci = dciFix.ToDouble();

	float64x2_t v_dci = vdupq_n_f64(dci);	// Y offset
	float64x2_t v_four = vdupq_n_f64(4.0);	// constant 4.0
	float64x2_t v_step2 = vdupq_n_f64(dsx * 2.0); // 2*dsx increment

	// start X values for lanes 0..1 { dcr, dcr + sx }
	float64x2_t v_idx = { 0.0, 1.0 };	
	float64x2_t v_dcr = vaddq_f64(vdupq_n_f64(dcr_start), vmulq_f64(v_idx, vdupq_n_f64(dsx)));

	// per-pixel loop
	for (x = 0; x < w; x += 2)
	{
		// start condition
		float64x2_t v_dzr = vdupq_n_f64(0.0);	// dzr = 0
		float64x2_t v_dzi = vdupq_n_f64(0.0);	// dzi = 0
		int64x2_t v_refidx = vdupq_n_s64(0);	// refindex = 0
		uint64x2_t v_active = vdupq_n_u64(~0ULL); // both lanes are active
		float64x2_t v_mod2 = vdupq_n_f64(0.0);	// mod2 = 0
		int64x2_t v_iter = vdupq_n_s64(0);	// exit iteration = 0

		// iteration loop
		for (i = 0; i < steps; i++)
		{
			// get reference points: gather RefZrDbl/RefZiDbl of lanes, indices can differ
			int64_t idx0 = vgetq_lane_s64(v_refidx, 0);	// idx0 = reference index 0
			int64_t idx1 = vgetq_lane_s64(v_refidx, 1);	// idx1 = reference index 1
			float64x2_t v_zr = vcombine_f64(vld1_f64(&RefZrDbl[idx0]), vld1_f64(&RefZrDbl[idx1])); // zr = RefZrDbl[]
			float64x2_t v_zi = vcombine_f64(vld1_f64(&RefZiDbl[idx0]), vld1_f64(&RefZiDbl[idx1])); // zi = RefZiDbl[]

			// actual (full-precision) position at this iteration = reference + delta
			float64x2_t v_ar = vaddq_f64(v_zr, v_dzr);	// ar = zr + dzr
			float64x2_t v_ai = vaddq_f64(v_zi, v_dzi);	// ai = zi + dzi
			float64x2_t v_m2 = vmlaq_f64(vmulq_f64(v_ar, v_ar), v_ai, v_ai); // m2 = ar*ar + ai*ai

			// escape test
			uint64x2_t v_escaped_now = vandq_u64(vcgtq_f64(v_m2, v_four), v_active); // pixels escaped in this iteration (compare with 4.0)
			v_mod2 = vbslq_f64(v_escaped_now, v_m2, v_mod2); // mod2 = select new m2 or old mod2
			v_iter = vbslq_s64(v_escaped_now, vdupq_n_s64(i), v_iter); // iter = select old 'i' or new 'iter'

			// deactivate the pixels that just leaked
			v_active = vbicq_u64(v_active, v_escaped_now);	// clear escaped lanes (set non-active)

			// if all lanes are inactive, stop the iteration
			if (vaddvq_u64(v_active) == 0) break;		// all lanes are not active, exit

			// glitch avoidance: if the reference orbit ran out, or the true value has
			// drifted so far from the reference that it is now closer to the origin
			// than the perturbation itself, rebase onto Z_0=0 using the actual value

			// glitch/rebase test: (refidx+1 >= reflen) || (mod2 < dzr^2+dzi^2)
			uint64x2_t v_ref_end = vcgeq_s64(vaddq_s64(v_refidx, vdupq_n_s64(1)), vdupq_n_s64(reflen)); // ref_end = compare(refidx + 1 >= reflen)
			float64x2_t v_dz2 = vmlaq_f64(vmulq_f64(v_dzr, v_dzr), v_dzi, v_dzi); // dz2 = dzr*dzr + dzi*dzi
			uint64x2_t v_glitch = vorrq_u64(v_ref_end, vcltq_f64(v_m2, v_dz2)); // glitch = ref_end OR compare(m2 < dz2)
			uint64x2_t v_rebase = vandq_u64(v_glitch, v_active); // setup condition flags - rebase only active lanes

			// reset iteration
			v_dzr = vbslq_f64(v_rebase, v_ar, v_dzr);  // dzr = ar (on rebase condition)
			v_dzi = vbslq_f64(v_rebase, v_ai, v_dzi); // dzi = ai (on rebase condition)
			v_refidx = vbslq_s64(v_rebase, vdupq_n_s64(0), v_refidx); // refidx = 0 (on rebase condition)
			v_zr = vbslq_f64(v_rebase, vdupq_n_f64(0.0), v_zr); // zr = 0 (on rebase condition)
			v_zi = vbslq_f64(v_rebase, vdupq_n_f64(0.0), v_zi); // zi = 0 (on rebase condition)

			// perturbation formula Real: new_dzr = 2*(zr*dzr - zi*dzi) + (dzr^2 - dzi^2) + dcr
			float64x2_t v_t1 = vsubq_f64(vmulq_f64(v_zr, v_dzr), vmulq_f64(v_zi, v_dzi)); // t1 = zr*dzr - zi*dzi
			float64x2_t v_dzr2 = vmulq_f64(v_dzr, v_dzr); // dzr2 = dzr*dzr
			float64x2_t v_dzi2 = vmulq_f64(v_dzi, v_dzi); // dzi2 = dzi*dzi
			float64x2_t v_new_dzr = vaddq_f64(vaddq_f64(v_t1, v_t1), vaddq_f64(vsubq_f64(v_dzr2, v_dzi2), v_dcr)); // dzr = 2*t1 + ((dzr2 - dzi2) + dcr)

			// perturbation formula Imag: new_dzi = 2*(zr*dzi + zi*dzr + dzr*dzi) + dci
			float64x2_t v_t2 = vaddq_f64(vaddq_f64(vmulq_f64(v_zr, v_dzi), vmulq_f64(v_zi, v_dzr)), vmulq_f64(v_dzr, v_dzi)); // t2 = (zr*dzi + zi*dzr) + dzr*dzi
			float64x2_t v_new_dzi = vaddq_f64(vaddq_f64(v_t2, v_t2), v_dci); // dzi = 2*t2 + dci

			// update only active lanes (select old or new results)
			v_dzr = vbslq_f64(v_active, v_new_dzr, v_dzr);
			v_dzi = vbslq_f64(v_active, v_new_dzi, v_dzi);

			// refindex++ only for active lanes
			v_refidx = vaddq_s64(v_refidx, vandq_s64(vdupq_n_s64(1), vreinterpretq_s64_u64(v_active)));
		}

		// load result
		uint64_t active0 = vgetq_lane_u64(v_active, 0);
		uint64_t active1 = vgetq_lane_u64(v_active, 1);

		// set color
		if (active0)
			dst[0] = COL_BLACK;
		else
			dst[0] = GetColor(vgetq_lane_f64(v_mod2, 0), (int)vgetq_lane_s64(v_iter, 0));

		if (active1)
			dst[1] = COL_BLACK;
		else
			dst[1] = GetColor(vgetq_lane_f64(v_mod2, 1), (int)vgetq_lane_s64(v_iter, 1));

		// next 2 pixels
		v_dcr = vaddq_f64(v_dcr, v_step2);
		dst += 2;
	}
}

#endif // AARCH == 32

/*
// calculate Mandelbrot line using fixed integer - very slow, do not use
void MandelFix(u32* dst, fix* ci)
{
	int x, i;
	fix zr, zi, zr2, zi2, cr, sx;
	int steps = Steps;
	int w = WIDTH;

	// prepare cr
	sx.Copy(&SXi);
	cr.Copy(&X0i);

	// X loop	
	for (x = 0; x < w; x++)
	{
		zr.Copy(&cr);
		zi.Copy(ci);

		for (i = 0; i < steps; i++)
		{
			zr2.Sqr(&zr);
			zi2.Sqr(&zi);
			if ((zr2.m_data[0] + zi2.m_data[0]) > (FIX_BASE)(4ull << (FIX_BITS - FIX_INTBITS))) break;

			zr.Mul(&zr, &zi);
			zr.Mul2(&zr);
			zi.Add(&zr, ci);

			zr2.Sub(&zr2, &zi2);
			zr.Add(&zr2, &cr);
		}

		if (i >= steps)
			*dst = COL_BLACK;
		else
			*dst = GetColor(zr2.ToDouble() + zi2.ToDouble(), i);

		dst++;
		cr.Add(&cr, &sx);
	}
}
*/

// load file
void Load()
{
	// try to mount disk
	if (DiskMount())
	{
		// get path with saves
		GetHomePath(MandelPath, MANDELPATH_MAX, MandelPathFile);

		// set current directory
		SetDir(MandelPath);

		// prepare filename
		int len = StrLen(MandelPathFile);
		MandelPathFile[len++] = '.';
		MandelPathFile[len++] = 'S';
		MandelPathFile[len++] = 'A';
		MandelPathFile[len++] = Slot + '0';
		MandelPathFile[len] = 0;
		
		// load file
		sFile file;
		if (FileOpen(&file, MandelPathFile))
		{
			// load file
			char* s = MandelSaveBuf;
			int maxlen = FileRead(&file, s, MANDELSAVE_MAX);

			// close file
			FileClose(&file);

			// add 5 NL characters
			s[maxlen] = 10;
			s[maxlen+1] = 10;
			s[maxlen+2] = 10;
			s[maxlen+3] = 10;
			s[maxlen+4] = 10;

			// parse PalSet
			PalSet = StrToUInt(s, (const char**)&s);
			if (*s == 13) s++;
			if (*s == 10) s++;

			// parse SizeN
			SizeN = StrToUInt(s, (const char**)&s);
			if (*s == 13) s++;
			if (*s == 10) s++;
			if (SizeN < 0) SizeN = 0;
			if (SizeN > SIZEN_MAX) SizeN = SIZEN_MAX;

			// parse Steps
			Steps = StrToUInt(s, (const char**)&s);
			if (*s == 13) s++;
			if (*s == 10) s++;
			if (Steps < STEPS_MIN) Steps = STEPS_MIN;
			if (Steps > STEPS_MAX) Steps = STEPS_MAX;
			
			// parse OffX
			len = OffX.FromText(s);
			s += len;
			if (*s == 13) s++;
			if (*s == 10) s++;

			// parse OffY
			len = OffY.FromText(s);
			s += len;
			if (*s == 13) s++;
			if (*s == 10) s++;

			// inicialize new state
			GenPalette();
			SetupSize(); // convert SizeN to Size
			MandelStart();

			// load OK
			FrameBuffer.printpos = 0;
			FrameBuffer.printrow = 0;
			printf("Load slot: %d OK   \n", Slot);
			DispUpdate();
			return;
		}
	}

	// load error
	FrameBuffer.printpos = 0;
	FrameBuffer.printrow = 0;
	printf("Load slot: %d ERROR\n", Slot);
	DispUpdate();
}

// save file
void Save()
{
	// prepare save buffer
	char* d = MandelSaveBuf;
	int maxlen = MANDELSAVE_MAX - 5;

	// print PalSet
	int len = DecUNum(d, PalSet, 0);
	d += len;
	*d++ = 10; // LF
	maxlen = maxlen - len + 1;

	// print SizeN
	len = DecUNum(d, SizeN, 0);
	d += len;
	*d++ = 10; // LF
	maxlen = maxlen - len + 1;

	// print Steps
	len = DecUNum(d, Steps, 0);
	d += len;
	*d++ = 10; // LF
	maxlen = maxlen - len + 1;

	// print OffX
	len = OffX.ToText(d, maxlen);
	d += len;
	*d++ = 10; // LF
	maxlen = maxlen - len + 1;

	// print OffY
	len = OffY.ToText(d, maxlen);
	d += len;
	*d++ = 10; // LF

	// total length of the text
	maxlen = d - MandelSaveBuf;

	// try to mount disk
	if (DiskMount())
	{
		// get path with saves
		GetHomePath(MandelPath, MANDELPATH_MAX, MandelPathFile);

		// set current directory
		SetDir(MandelPath);

		// prepare filename
		len = StrLen(MandelPathFile);
		MandelPathFile[len++] = '.';
		MandelPathFile[len++] = 'S';
		MandelPathFile[len++] = 'A';
		MandelPathFile[len++] = Slot + '0';
		MandelPathFile[len] = 0;
		
		// create file
		FileDelete(MandelPathFile);
		sFile file;
		if (FileCreate(&file, MandelPathFile))
		{
			// save file
			len = FileWrite(&file, MandelSaveBuf, maxlen);

			// close file
			FileClose(&file);

			// save OK
			if (len == maxlen)
			{
				FrameBuffer.printpos = 0;
				FrameBuffer.printrow = 0;
				printf("Save slot: %d OK   \n", Slot);
				DispUpdate();
				return;
			}
		}
	}

	// save error
	FrameBuffer.printpos = 0;
	FrameBuffer.printrow = 0;
	printf("Save slot: %d ERROR\n", Slot);
	DispUpdate();
}

// render line
void RenderLine(u32* dst, fix* ci)
{
	switch (Mode)
	{
	case MODE_FLT:	// float
		MandelFlt(dst, ci);
		break;

	case MODE_DBL:	// double
		MandelDbl(dst, ci);
		break;

	case MODE_FLT_PER: // float perturbation
		MandelFltPer(dst, ci);
		break;

	default:
	case MODE_DBL_PER:  // double perturbation
		MandelDblPer(dst, ci);
		break;
	};
}

// multi-core
#if CORES > 1

// core function
void CoreFnc(int core, void* arg)
{
	while (True)
	{
		// wait for event
		dsb();
		wfe();

		// process tasks
		u32* dst = CoreDst[core];
		if (dst != NULL)
		{
			// render line
			dmb();
			RenderLine(dst, &CoreCii[core]);

			// delete this task
			dmb();
			CoreDst[core] = NULL;
		}
	}
}

#endif // CORES > 1

// find next line to render (returns False if all lines are processed)
Bool NextLine()
{
	while (True)
	{
		// current line is not valid
		if (CurY >= HEIGHT)
		{
			// image is already processed
			if (IncY <= 1) return False; // next line is not valid

			// resize line increment
			IncY /= 2;

			// update Y increment
			MandelUpdate();
		}

		// has that line already been rendered?
		if (!Ok[CurY])
		{
			// processed ok
			Ok[CurY] = True;
			return True;	// next line is OK
		}

		// shift to next line
		CurY += IncY;		// shift index of current line
		Cii.Add(&Cii, &SYi);	// shift Y coordinate
		Dst += DstInc;		// shift destination address

		// that was last line, get stop time
		if ((CurY >= HEIGHT) && (IncY <= 1)) StopTime = Time();
	}
}

// render line set (returns False on end of frame)
Bool RenderLineSet()
{
	// get first line
	Bool ok = NextLine(); // find next line to render (returns False if all lines are processed)
	if (!ok) return False;

#if CORES > 1

// dmb() ... ensures that all read/write operations preceding dmb() are executed before the 
//		read/write operations following the dmb() instruction. It is used as a memory
//		access separator that enforces a specific order of operations.
//		Non-memory operations can be performed while waiting for operations to complete.

// dsb() ... in addition to the dmb() barrier function, it also ensures that the execution
//		of all instructions - even non-memory-access instructions - is halted.
//		It is used before triggering an interrupt and before sev()/wfe().

	// run tasks on cores
	int c;
	for (c = 1; c < CORES; c++)
	{
		// Cii coordinate
		CoreCii[c].Copy(&Cii);
		dmb();

		// dst address, enable this task
		CoreDst[c] = Dst;
		dmb();

		// shift to next line
		ok = NextLine();
		if (!ok) break;
	}

	// start cores
	dsb();
	sev();	// send event signal

	if (ok)
#endif // CORES > 1

	{
		// render line on core 0
		RenderLine(Dst, &Cii);
	}

#if CORES > 1
	// wait all cores to stop
	dmb();
	for (c = 1; c < CORES; c++)
	{
		while (CoreDst[c] != NULL) dmb();
	}
#endif // CORES > 1

	return True;
}

// Zoom image (upscale image in ratio num/den)
//  src ... source back buffer
//  dst ... destination back buffer
//  num ... upscale numerator
//  den ... upscale denominator
// Interpolation uses Bresenham's accumulator.
void ZoomCenter(const u32* src, u32* dst, u32 num, u32 den)
{
	// prepare source box
	u32 src_w = (WIDTH*den)/num;	// source width
	u32 src_h = (HEIGHT*den)/num;	// source height
	u32 src_x0 = (WIDTH - src_w)/2;	// source X
	u32 src_y0 = (HEIGHT - src_h)/2; // source Y
 
	// Y error accumulator
	u32 y_err = HEIGHT/2;	// Y center
	u32 src_y = src_y0;	// source Y
 
	// Y lines loop
	u32 y, x;
	for (y = 0; y < HEIGHT; y++)
	{
		// Y source and destination line address
		const u32* src_row = src + src_y*WIDTH;
		u32* dst_row = dst + y*WIDTH;
 
		// X error accumulator
		u32 x_err = WIDTH/2;	// X center
		u32 src_x = src_x0;	// source X

		// X pixels loop 
		for (x = 0; x < WIDTH; x++)
		{
			// copy one pixel
			dst_row[x] = src_row[src_x];

			// shift X 
			x_err += src_w;
			while (x_err >= WIDTH)
			{
				x_err -= WIDTH;
				src_x++;
			}
		}

		// shift Y 
		y_err += src_h;
		while (y_err >= HEIGHT)
		{
			y_err -= HEIGHT;
			src_y++;
		}
	}
}
 
// Run animation
void Anim()
{
	// save current state
	int sizen = SizeN;

	// setup base zoom
	SizeN = 0;
	SetupSize();

	// prepare back buffer
	FrameBuffer.drawbuf = BackBuf;
	u32* src = BackBuf2;
	DrawBackBuf2 = False;
	memset(BackBuf, 0, sizeof(BackBuf));
	memset(BackBuf2, 0, sizeof(BackBuf2));

	// start render current frame
	MandelStart(False);

	// prepare animation step counter
	int anim_steps = HEIGHT/CORES;	// total number of steps (480 or 120)
	anim_steps /= 8;		// number of steps per one mini-image (60 or 15)
	int nextimg = 1;		// next image step
	int nextcnt = anim_steps;	// counter to next image

	// render loop
	while (True)
	{
		// break by keyboard
		int c = KeyGet();

		if (c == KEY_SCREENSHOT)
		{
			ScreenShot();
			c = NOKEY;
		}

		if (c == KEY_ZOOM)
		{
			LCDRezoom();
			c = NOKEY;
		}

		if (c == KEY_PAD_X)
		{
			Info = !Info;
			c = NOKEY;
		}

		if ((c != NOKEY) && (c != KEY_ALT)) break;

		// render line set (returns False on end of frame)
		if (RenderLineSet())
		{
			// mini-animation
			nextcnt--;
			if (nextcnt <= 0)
			{
				// draw mini-image
				if (nextimg < 8)
				{
					ZoomCenter(src, FrameBuffer.screenbuf, nextimg + 8, 8);
					CleanDataCache();
				}
				nextimg++;

				// next step
				nextcnt = anim_steps;	// counter to next image
			}
		}
		else
		{
			// display info
			if (Info)
			{
				FrameBuffer.printpos = 0;
				FrameBuffer.printrow = 0;
				printf("SizeN: %d\n", SizeN);
			}

			// display update
			DispUpdate();

			// max. zoom
			if (SizeN >= SIZEN_MAX) break;

			// zoom in
			SizeN++;
			SetupSize(); // convert SizeN to Size

			// exchange back buffers
			DrawBackBuf2 = !DrawBackBuf2;
			if (DrawBackBuf2)
			{
				FrameBuffer.drawbuf = BackBuf2;
				src = BackBuf;
			}
			else
			{
				FrameBuffer.drawbuf = BackBuf;
				src = BackBuf2;
			}

			// start new frame
			MandelStart(False);

			// prepare animation
			nextimg = 1;		// next image step
			nextcnt = anim_steps;	// counter to next image
		}
	}

	// restore current state
	SizeN = sizen;
	SetupSize();

	// prepare back buffer
	FrameBuffer.drawbuf = BackBuf;
	DrawBackBuf2 = False;

	// update
	MandelStart();
}

// main function
int main()
{
	int c;
	Bool info_disp = False;

	// generate palettes
	GenPalette();

	// clear back buffer
	memset(BackBuf, 0, sizeof(BackBuf));
	memset(BackBuf2, 0, sizeof(BackBuf2));
	FrameBuffer.drawbuf = BackBuf;
	DrawBackBuf2 = False;

	// base setup
	Steps = STEPS_BASE;	// number of steps
	SizeN = 0;		// default zoom
	SetupSize();		// convert SizeN to Size
	OffX.SetZero();		// default X
	OffY.SetZero();		// default Y

	// -3/HEIGHT
	Height3.FromDbl(-3.0/HEIGHT);

	// 4/WIDTH
	Width4.FromDbl(4.0/WIDTH);

#if CORES > 1
	// run cores
	for (c = 1; c < CORES; c++) RunCore(c, CoreFnc, NULL);
#endif

	// start new image
	MandelStart();

	while (true)
	{
		// keys
		c = KeyGet();
		if ((c != NOKEY))
		{
			switch (c)
			{
			// Animation
			case KEY_PAD_B:
				Anim();
				break;

			// display info
			case KEY_PAD_X:
				Info = !Info;
				if (!Info) MandelStart();
				break;

			// quit
			case KEY_PAD_Y:
				Reboot();
				break;

			// move up, zoom in
			case KEY_UP:
				if (KeyPressed(KEY_PAD_A))
				{
					// zoom in
					if (SizeN < SIZEN_MAX)
					{
						SizeN++;
						SetupSize(); // convert SizeN to Size
						MandelStart();
					}
				}
				else
				{
					// move up
					OffY.Add(&OffY, &Size);
					MandelStart();
				}
				break;

			// move down, zoom out
			case KEY_DOWN:
				if (KeyPressed(KEY_PAD_A))
				{
					// zoom out
					if (SizeN > 0)
					{
						SizeN--;
						SetupSize(); // convert SizeN to Size
						MandelStart();
					}
				}
				else
				{
					// move down
					OffY.Sub(&OffY, &Size);
					MandelStart();
				}
				break;

			// move left, less steps
			case KEY_LEFT:
				if (KeyPressed(KEY_PAD_A))
				{
					if (Steps > STEPS_MIN)
					{
						if (Steps <= 500)
							Steps -= 10;
						else if (Steps <= 1000)
							Steps -= 20;
						else if (Steps <= 5000)
							Steps -= 50;
						else if (Steps <= 10000)
							Steps -= 100;
						else
							Steps -= 200;
						if (Steps < STEPS_MIN) Steps = STEPS_MIN;
						MandelStart();
					}
				}
				else
				{
					// move left
					OffX.Sub(&OffX, &Size);
					MandelStart();
				}
				break;

			// move right, more steps
			case KEY_RIGHT:
				if (KeyPressed(KEY_PAD_A))
				{
					if (Steps < STEPS_MAX)
					{
						if (Steps < 500)
							Steps += 10;
						else if (Steps < 1000)
							Steps += 20;
						else if (Steps < 5000)
							Steps += 50;
						else if (Steps < 10000)
							Steps += 100;
						else
							Steps += 200;
						if (Steps > STEPS_MAX) Steps = STEPS_MAX;
						MandelStart();
					}
				}
				else
				{
					// move right
					OffX.Add(&OffX, &Size);
				}
				MandelStart();
				break;

			// next slot
			case KEY_PGUP:
				if (Slot < SLOT_NUM-1)
				{
					Slot++;
					FrameBuffer.printpos = 0;
					FrameBuffer.printrow = 0;
					printf("Save slot: %d      \n", Slot);
					DispUpdate();
				}
				break;

			// previous slot
			case KEY_PGDN:
				if (Slot > 0)
				{
					Slot--;
					FrameBuffer.printpos = 0;
					FrameBuffer.printrow = 0;
					printf("Save slot: %d      \n", Slot);
					DispUpdate();
				}
				break;

			// palette prev
			case KEY_HOME:
				if (PalSet > 0)
				{
					PalSet--;
					GenPalette();
					MandelStart();
				}
				break;

			// palette next
			case KEY_END:
				PalSet++;
				GenPalette();
				MandelStart();
				break;				

			// load file
			case KEY_INSERT:
				Load();
				break;

			// save file
			case KEY_MENU:
				Save();
				break;

			// screenshot
			case KEY_SCREENSHOT:
				ScreenShot();
				break;

			// LCD display rezoom
			case KEY_ZOOM:
				LCDRezoom();
				break;

			default:
				break;
			}
		}

		// render line set
		if (RenderLineSet())
			info_disp = False;
		else		
		{
			// display info
			if (Info && !info_disp)
			{
				info_disp = True;

				// display info
				FrameBuffer.printpos = 0;
				FrameBuffer.printrow = 0;
				printf("Save slot: %d      \n", Slot);
				printf("SizeN: %d, Zoom: %e\n", SizeN, 1/Size.ToDouble());
				printf("Steps: %d\n", Steps);
				printf("PalSet: %d\n", PalSet);
				printf("Time: %d ms\n", (StopTime - StartTime)/1000);
				OffX.ToText(MandelSaveBuf, MANDELSAVE_MAX);
				printf("X: %s\n", MandelSaveBuf);
				OffY.ToText(MandelSaveBuf, MANDELSAVE_MAX);
				printf("Y: %s\n", MandelSaveBuf);

				// display help
				printf("\nHelp:\n");
				printf("Arrows ........... move\n");
				printf("[B] .............. animation\n");
				printf("[X] .............. info\n");
				printf("[Y] .............. quit\n");
				printf("[A]+Up/Down ...... zoom\n");
				printf("[A]+Left/Right ... steps\n");
				printf("Alt+Up/Down ...... save slot\n");
				printf("Alt+Left/Right ... palette\n");
				printf("Alt+[A] .......... LCD zoom\n");
				printf("Alt+[B] .......... load\n");
				printf("Alt+[Y] .......... save\n");
				printf("Alt+[X] .......... screenshot\n");
			}
		}

		// display auto-update
		DispUpdateAuto();
	}

	return 0;
}
