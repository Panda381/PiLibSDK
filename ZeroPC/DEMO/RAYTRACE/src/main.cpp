
// ****************************************************************************
//                                 
//                              Main code
//
// ****************************************************************************

#include "../include.h"

#if CORES>1
#define WIDTH2 (WIDTH/2)
#define HEIGHT2 (HEIGHT/2)
#define TOOFAR 1e3
#define DEPTHMAX 3			// max. depth of trace
#else
#define WIDTH2 WIDTH
#define HEIGHT2 HEIGHT
#define TOOFAR 1e2
#define DEPTHMAX 1			// max. depth of trace
#endif

typedef double real;

INLINE real div(real a, real b) { return a/b; }

INLINE real abs(real num) { return (num < 0) ? -num : num; }

#define FLOORY -2			// floor Y position

// 3D vector
class V3
{
public:
	
	// vector coordinates
	real x, y, z;

	// constructor
	V3() {};
	V3(real xx, real yy, real zz) { x = xx; y = yy; z = zz; }

	// compare vectors
	inline Bool Equ(const V3 &v) { return (x == v.x) && (y == v.y) && (z == v.z); }

	// set value
	void Zero() { x = y = z = 0; }
	void SetX() { x = 1; y = 0; z = 0; }
	void SetY() { x = 0; y = 1; z = 0; }
	void SetZ() { x = 0; y = 0; z = 1; }
	void Set(real xx, real yy, real zz) { x = xx; y = yy; z = zz; }
	void Set(const V3 &v) { x = v.x; y = v.y; z = v.z; }
	V3& operator = (const V3 &v) { x = v.x; y = v.y; z = v.z; return *this; }

	// set normalized vector (set length to 1)
	void SetNorm(const V3 &v)
	{
		real r = (real)sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
		if (r == 0)
		{
			x = y = 0;
			z = 1;
		}
		else
		{
			r = 1/r;
			x = v.x * r;
			y = v.y * r;
			z = v.z * r;
		}
	}

	// add two vectors
	void Add(real xx, real yy, real zz) { x += xx; y += yy; z += zz; }
	void Add(const V3 &v) { x += v.x; y += v.y; z += v.z; }
	V3 operator + (const V3 &v) const { return V3(x + v.x, y + v.y, z + v.z); }
	V3& operator += (const V3 &v) { x += v.x; y += v.y; z += v.z; return *this; }

	// subtract two vectors
	void Sub(real xx, real yy, real zz) { x -= xx; y -= yy; z -= zz; }
	void Sub(const V3 &v) { x -= v.x; y -= v.y; z -= v.z; }
	V3 operator - (const V3 &v) const { return V3(x - v.x, y - v.y, z - v.z); }
	V3& operator -= (const V3 &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }

	// unary negation, turn vector into inverse direction
	void Inv() { x = -x; y = -y; z = -z; }
	V3 operator - () const { return V3(-x, -y, -z); }

	// multiplication by scalar value
	void Mul(real k) { x *= k; y *= k; z *= k; }
	V3 operator * (const real &k) const { return V3(x*k, y*k, z*k); }
	V3& operator *= (const real &k) { x *= k; y *= k; z *= k; return *this; }

	// division
	void Div(real k) { k = div(1.0f, k); x *= k; y *= k; z *= k; }
	V3 operator / (real k) const { k = div(1.0f, k); return V3(x*k, y*k, z*k); }
	V3& operator /= (real k) { k = div(1.0f, k); x *= k; y *= k; z *= k; return *this; }

	// length of vector
	real Len2() const { return x*x + y*y + z*z; }
	real Len() const { return (real)sqrt(x*x + y*y + z*z); }

	// distance of two vectors
	real Dist2(const V3 &v) const
	{
		real dx = x - v.x;
		real dy = y - v.y;
		real dz = z - v.z;
		return dx*dx + dy*dy + dz*dz;
	}
	real Dist(const V3 &v) const { return (real)sqrt(Dist2(v)); }

	// normalize vector (set length to 1)
	void Norm()
	{
		real r = (real)sqrt(x*x + y*y + z*z);
		if (r == 0)
			{ x = 0; y = 0; z = 1; }
		else
			{ r = div(1.0f, r); x *= r; y *= r; z *= r; }
	}

	// scalar (dot) product V1.V2 (result is cosinus of angle of two vectors * lengths of vectors)
	real Dot(const V3 &v) const { return x*v.x + y*v.y + z*v.z; }
	real Angle(const V3 &v)
	{
		real r = (real)sqrt(Len2()*v.Len2());
		if (r == 0) return 0;
		return (real)acos((x*v.x + y*v.y + z*v.z)/r);
	}

	// vector (cross) product V1xV2, multiply two vectors and store result into this vector 
	// (result is perpendicular normal vector to this two vectors)
	void Mul(const V3 &v1, const V3 &v2)
	{
		real xx = v1.y * v2.z - v1.z * v2.y;
		real yy = v1.z * v2.x - v1.x * v2.z;
		real zz = v1.x * v2.y - v1.y * v2.x;
		x = xx;
		y = yy;
		z = zz;
	}
	V3 operator * (const V3 &v) const { return V3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x); }
};

// sphere object
class Sphere
{
public:

	// sphere position (center)
	V3 pos;

	// sphere radius and radius^2
	real rad, rad2;

	// diffuse color (components 0..1)
	V3 col;

	// reflection (0..1)
	real ref;

	// transparency (0..1)
	real trans;

	// constructor
	Sphere() {}
	Sphere(const V3 &position, real radius, const V3 &color, real reflection = 0, real transparency = 0)
	{
		pos = position;
		rad = radius;
		rad2 = radius*radius;
		col = color;
		ref = reflection;
		trans = transparency;
	}

	// compute ray-sphere intersection (returns TRUE on success)
	//  orig = ray origin
	//  dir = ray direction
	//  OUTPUT t1 = distance of first intersection
	//  OUTPUT t2 = distance of second intersection
	bool Intersect(const V3 &orig, const V3 &dir, real* t1, real* t2) const
	{
		// direction vector to sphere center
		V3 l = pos - orig;

		// distance to center of intersection
		real tca = l.Dot(dir);

		// ray goes to oposide side, no intersection
		if (tca <= 0) return false;

		// distance from sphere center to the ray
		real d2 = l.Len2() - tca*tca;

		// ray is not intersecting the sphere
		if (d2 >= rad2) return false;

		// half length of intersection
		real thc = (real)sqrt(rad2 - d2);

		// distance of intersections
		*t1 = tca - thc;
		*t2 = tca + thc;

		return true;
	}
};

// list of spheres
Sphere Spheres0[] = {
	// position, radius, color, reflection, transparency (transparency not realized)
	Sphere(V3(-3.2,-0.4,1), 1, V3(1,1,0), 0.5, 0), // yellow sphere
	Sphere(V3(-1.4,0.4,0), 0.8f, V3(1,0,0), 0.5, 0),  // red sphere
	Sphere(V3(1.0,-0.1,-4), 2, V3(0,0,(real)0.4), (real)0.7, 0), // blue sphere
	Sphere(V3(2.8,0.2,1), 1.5, V3(0,(real)0.5,0), (real)0.6, 0), // green sphere
};

// list of spheres
Sphere Spheres1[] = {
	// position, radius, color, reflection, transparency (transparency not realized)
	Sphere(V3(-3.2,-0.4,1), 1, V3(1,0,0), 0.5, 0), // yellow sphere
	Sphere(V3(-1.4,0.4,0), 0.8f, V3(0,0,0.4), 0.5, 0),  // red sphere
	Sphere(V3(1.0,-0.1,-4), 2, V3(0,0.5,0), (real)0.7, 0), // blue sphere
	Sphere(V3(2.8,0.2,1), 1.5, V3(1,1,0), (real)0.6, 0), // green sphere
};

// list of spheres
Sphere Spheres2[] = {
	// position, radius, color, reflection, transparency (transparency not realized)
	Sphere(V3(-3.2,-0.4,1), 1, V3(0,0,0.4), 0.5, 0), // yellow sphere
	Sphere(V3(-1.4,0.4,0), 0.8f, V3(0,0.5,0), 0.5, 0),  // red sphere
	Sphere(V3(1.0,-0.1,-4), 2, V3(1,1,0), (real)0.7, 0), // blue sphere
	Sphere(V3(2.8,0.2,1), 1.5, V3(1,0,0), (real)0.6, 0), // green sphere
};

// list of spheres
Sphere Spheres3[] = {
	// position, radius, color, reflection, transparency (transparency not realized)
	Sphere(V3(-3.2,-0.4,1), 1, V3(0,0.5,0), 0.5, 0), // yellow sphere
	Sphere(V3(-1.4,0.4,0), 0.8f, V3(1,1,0), 0.5, 0),  // red sphere
	Sphere(V3(1.0,-0.1,-4), 2, V3(1,0,0), (real)0.7, 0), // blue sphere
	Sphere(V3(2.8,0.2,1), 1.5, V3(0,0,0.4), (real)0.6, 0), // green sphere
};
#define OBJNUM count_of(Spheres0)

Sphere* SphereList[4] = { Spheres0, Spheres1, Spheres2, Spheres3 };
INLINE Sphere* GetSpheres() { return SphereList[CpuID()]; }

V3 Camera(0, 0, 10);		// camera position
V3 Light(-6, 4, 10);		// light position
real Ambient = (real)0.3;	// intensity of ambient light
V3 BackCol[4] = {	V3(0.4, 0.6, 1),  // background color in horizon
			V3(0.2, 0.9, 0.5),
			V3(0.7, 0.3, 0.2),
			V3(1, 1, 0) };

real FloorPos[4] = { 0, 0, 0, 0 };	// floot position

// trace ray
void Trace(V3* rgb, const V3 &orig, const V3 &dir, int depth, const Sphere* disable)
{
	Sphere* spheres = GetSpheres();
	real* floorpos = &FloorPos[CpuID()];

	// find nearest intersection
	real t1best = TOOFAR, t2best = TOOFAR;
	const Sphere* sbest = NULL;
	real t1, t2;
	int i;
	for (i = 0; i < OBJNUM; i++)
	{
		if (&spheres[i] == disable) continue;
		t1 = TOOFAR;
		t2 = TOOFAR;
		if (spheres[i].Intersect(orig, dir, &t1, &t2))
		{
			if (t1 < 0) t1 = t2;
			if (t1 < t1best)
			{
				t1best = t1;
				t2best = t2;
				sbest = &spheres[i];
			}
		}
	}

	// if object not found, return sky color or continue with floor plane
	V3 col, pos, norm;
	real refl;
	if (sbest == NULL)
	{
		// sky (black in top direction)
		real k = dir.y;
		if (dir.y >= 0)
		{
			k *= 2.5;
			if (k > 1) k = 1;
			*rgb = BackCol[CpuID()]*(1-k);
			return;
		}

		// floor - substitute with plane parameters
		t1best = (orig.y - FLOORY) / dir.y;
		pos = orig - (dir*t1best);
		norm.Set(0,1,0);
		col = ((int)(ceilf(pos.x) + ceilf(pos.z + *floorpos)) & 1) ? V3(1,1,1) : V3(1,0.25,0.25);
		refl = (real)0.4;
	}
	else
	{
		// coordinate and normal in intersection point
		pos = orig + (dir*t1best);
		norm = pos - sbest->pos;
		norm.Norm();
		col = sbest->col;
		refl = sbest->ref;
	}

	// if normal and ray direction are not opposited, we are inside sphere, then reverse normal
	Bool inside = False;
	if (dir.Dot(norm) > 0)
	{
		inside = True;
		norm.Inv();
	}

	// vector to light
	V3 light = Light - pos;
	light.Norm();

	// check if point is in shadow
	real intens = 1;
	for (i = 0; i < OBJNUM; i++)
	{
		if (sbest == &spheres[i]) continue;
		if (spheres[i].Intersect(pos, light, &t1, &t2))
		{
			intens = 0;
			break;
		}
	}

	// get diffusion color
	intens = intens*norm.Dot(light)*(1-Ambient);
	if (intens <= 0) intens = 0;
	*rgb = col * (intens + Ambient);

	// add reflection
	if ((refl > 0) && (depth < DEPTHMAX))
	{
		// reflection vector
		V3 rdir = dir - norm*2*dir.Dot(norm);
		rdir.Norm();

		// add reflection
		V3 rgb2;
		Trace(&rgb2, pos, rdir, depth + 1, sbest);
		*rgb *= 1 - refl;
		*rgb += rgb2 * refl;
	}
}

// render image
void Render3D(u32* dst)
{
	Sphere* spheres = GetSpheres();
	real* floorpos = &FloorPos[CpuID()];

	// local variables
	int tmp;
	int x, y;			// current X and Y coordinates in bitmap
	real xx, yy;		// current X and Y coordinates in viewing plane
	V3 rgbV;			// result pixel color as vactor 0..1
	V3 orig(0, 0, 10);	// camera position
	V3 dir;				// ray direction
	real fov = 45*PI/180; // field of view in degrees
	real tfov = (real)tan(fov/2); // height/2 of viewing plane
	real ar = WIDTH2/(real)HEIGHT2; // aspect ratio
	int red, green, blue;

	// render picture
	for (y = HEIGHT2-1; y >= 0; y--)
	{
		for (x = 0; x < WIDTH2; x++)
		{
			// ray direction vector
			xx = (real)(2*(x + 0.5)/WIDTH2 - 1) * tfov * ar;
			yy = (real)(2*(y + 0.5)/HEIGHT2 - 1) * tfov;
			dir.Set(xx, yy, -1);
			dir.Norm();

			// trace this ray
			Trace(&rgbV, Camera, dir, 0, NULL);

			// convert vector to RGB pixel
			tmp = (int)(rgbV.x*256 + 0.5);
			if (tmp < 0) tmp = 0;
			if (tmp > 255) tmp = 255;
			red =  tmp; // red

			tmp = (int)(rgbV.y*256 + 0.5);
			if (tmp < 0) tmp = 0;
			if (tmp > 255) tmp = 255;
			green =  tmp; // green

			tmp = (int)(rgbV.z*256 + 0.5);
			if (tmp < 0) tmp = 0;
			if (tmp > 255) tmp = 255;
			blue =  tmp; // blue

			*dst++ = COLOR(red, green, blue, 255);
		}

		dst += WIDTH-WIDTH2;

		// clean and invalidate data cache in range
		CleanAndInvalidateDataCacheRange((uintptr_t)(dst - WIDTH), WIDTH2*4);
	}
}

// core function
void CoreFnc(int core, void* arg)
{
	u32* dst = (u32*)arg;
	Sphere* spheres = GetSpheres();
	real* floorpos = &FloorPos[CpuID()];

	real a = 0.2 + RandDoubleMinMax(-0.2,+0.2);
	real b = 0.8 + RandDoubleMinMax(-0.2,+0.2);
	real c = 1.2 + RandDoubleMinMax(-0.2,+0.2);
	real d = 1.9 + RandDoubleMinMax(-0.2,+0.2);

	real da = 0.2 + RandDoubleMinMax(-0.05,+0.05);
	real db = 0.24 + RandDoubleMinMax(-0.05,+0.05);
	real dc = 0.12 + RandDoubleMinMax(-0.05,+0.05);
	real dd = 0.17 + RandDoubleMinMax(-0.05,+0.05);

#if CORES>1
	while (!CoreStopReq(core))
#else
	while (True)
#endif
	{
		// exit application
		if (core == 0)
		{
			if (KeyGet() == KEY_ESC) Reboot();
		}

		// animation
		*floorpos -= 0.2; if (*floorpos <= -2.0) *floorpos += 2.0;
		spheres[0].pos.y = abs(sin(a))*spheres[0].rad + spheres[0].rad + FLOORY;
		spheres[1].pos.y = abs(sin(b))*spheres[1].rad*1.5f + spheres[1].rad + FLOORY;
		spheres[2].pos.y = abs(sin(c))*spheres[2].rad + spheres[2].rad + FLOORY;
		spheres[3].pos.y = abs(sin(d))*spheres[3].rad + spheres[3].rad + FLOORY;
		a += da; if (a >= (real)PI2) a -= (real)PI2;
		b += db; if (b >= (real)PI2) b -= (real)PI2;
		c += dc; if (c >= (real)PI2) c -= (real)PI2;
		d += dd; if (d >= (real)PI2) d -= (real)PI2;

		// rendering
		Render3D(dst);
	}
}

int main()
{
#if CORES>1
	RunCore(1, CoreFnc, FrameBuffer.drawbuf + WIDTH2);
	RunCore(2, CoreFnc, FrameBuffer.drawbuf + HEIGHT2*WIDTH);
	RunCore(3, CoreFnc, FrameBuffer.drawbuf + HEIGHT2*WIDTH + WIDTH2);
#endif
	CoreFnc(0, FrameBuffer.screenbuf);
	return 0;
}
