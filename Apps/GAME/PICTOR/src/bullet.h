
// ****************************************************************************
//
//                                 Enemy bullets
//
// ****************************************************************************

#define BULLET_SPEED	10		// bullet speed

// enemy bullet template
typedef struct {
	const u8*	imgpng;		// image PNG source
	int		imgpng_size;	// image PNG source size
	u8*		img;		// image
	int		w;		// image width
	int		h;		// image height
} sBulletTemp;

// enemy bullet templates
#define BULLET_TEMP_NUM	3
extern sBulletTemp BulletTemp[BULLET_TEMP_NUM];

// enemy bullet descriptor
typedef struct {
	const sBulletTemp*	temp;		// bullet template (NULL = not used)
	int			x, y;		// bullet coordinate
} sBullet;

// enemy bullet list
#define BULLET_MAX	30	// max. bullets
extern sBullet Bullet[BULLET_MAX];

#define BULLET_W	20		// bullet width
#define BULLET_H	20		// bullet height

// initialize enemy bullets on new level
void InitBullet();

// add new bullet
void AddBullet(const sBulletTemp* temp, int x, int y);

// shift bullets
void BulletShift();

// display bullets
void BulletDisp();
