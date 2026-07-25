
// ****************************************************************************
//
//                                Score
//
// ****************************************************************************

#ifndef _SCORE_H
#define _SCORE_H

#define SCORE_DIGNUM	6	// number of digits of the score
#define SCORE_LIVE	(20000 << FRAC) // increment of skore to increase the number of lives, with FRAC fraction

extern int	Score;		// current score, with FRAC fraction
extern int	MaxScore;	// max. score, with FRAC fraction
extern int	LastScore;	// score at last life addition, with FRAC fraction

// image width: 160 pixels
// image height: 20 lines
extern u8* DigitsImg;
#define DIGITS_IMG_W	16
#define DIGITS_IMG_H	20

// display score
void DispScore();

// increase score, with FRAC fraction
void AddScore(int add);

#endif // _SCORE_H
