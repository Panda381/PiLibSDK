
// ****************************************************************************
//
//                                Goals
//
// ****************************************************************************

#ifndef _GOALS_H
#define _GOALS_H

// goals init
extern const u8 GoalsInit[4];
extern int	Goals;		// remaining goals
extern u32	GoalTime;	// last goal time, to wait for end of the sector

// image width: 12 pixels
// image height: 8 lines
extern u8* GoalImg;
#define GOAL_IMG_W	12
#define GOAL_IMG_H	8

// display goals
void DispGoals();

// decrease goals
void DecGoals();

#endif // _GOALS_H
