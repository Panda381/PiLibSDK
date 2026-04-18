
// ****************************************************************************
//
//                                 Enemy bullets
//
// ****************************************************************************

#include "../include.h"

// enemy bullet templates
sBulletTemp BulletTemp[BULLET_TEMP_NUM] = {
//	imgpng			imgpng_size		img	w		h
	{ ImgBullet1PNG,	sizeof(ImgBullet1PNG),	NULL,	BULLET_W,	BULLET_H,	},
	{ ImgBullet2PNG,	sizeof(ImgBullet2PNG),	NULL,	BULLET_W,	BULLET_H,	},
	{ ImgBullet3PNG,	sizeof(ImgBullet3PNG),	NULL,	BULLET_W,	BULLET_H,	},
};

// enemy bullet list
sBullet Bullet[BULLET_MAX];

// initialize enemy bullets on new level
void InitBullet()
{
	// clear bullets
	int i;
	for (i = 0; i < BULLET_MAX; i++) Bullet[i].temp = NULL;
}

// add new bullet
void AddBullet(const sBulletTemp* temp, int x, int y)
{
	int i;
	sBullet* b = Bullet;
	for (i = 0; i < BULLET_MAX; i++)
	{
		if (b->temp == NULL)
		{
			b->temp = temp;
			b->x = x;
			b->y = y;
			break;
		}
		b++;
	}
}

// shift bullets
void BulletShift()
{
	int i, a;
	const sBulletTemp* temp;
	sBullet* b = Bullet;
	for (i = 0; i < BULLET_MAX; i++)
	{
		temp = b->temp;
		if (temp != NULL)
		{
			// shift bullet
			b->x -= BULLET_SPEED;

			// delete bullet
			if (b->x + temp->w/2 <= 0) b->temp = NULL;
		}
		b++;
	}
}

// display bullets
void BulletDisp()
{
	int i;
	const sBulletTemp* temp;
	sBullet* b = Bullet;
	for (i = 0; i < BULLET_MAX; i++)
	{
		temp = b->temp;
		if (temp != NULL)
		{
			DrawImg(temp->img, 		// image data and palettes
				b->x - temp->w/2, b->y - temp->h/2, // destination X, Y
				0, 0,				// source X, Y
				temp->w, temp->h);		// width, height
		}
		b++;
	}
}
