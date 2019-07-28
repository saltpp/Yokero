//=============================================================================
// Level.c / Programmed by Salt
//
// $Id: Level.c,v 1.6 2005/08/17 14:50:05 Administrator Exp $

#include "std.h"
#include "Level.h"


//-----------------------------------------------------------------------------
// level table
#define MAX_LEVEL 20
LOCAL int l_nLevel;

LOCAL INTERVAL l_interval[MAX_LEVEL] = {
	{10000,	10000,	10000,	30 * 5}, 	// 0
	{12,	10000,	10000,	30 * 8}, 	// 1
	{10,	10000,	10000,	30 * 8}, 	// 2
	{8,		10000,	10000,	30 * 8}, 	// 3
	{6,		10000,	10000,	30 * 8}, 	// 4
	{5,		10000,	10000,	30 * 9}, 	// 5
	{9,		130,	10000,	30 * 9}, 	// 6
	{8,		100,	10000,	30 * 9}, 	// 7
	{7,		70,		10000,	30 * 9}, 	// 8
	{7,		40,		10000,	30 * 9}, 	// 9
	{9,		130,	130,	30 * 10}, 	// 10
	{9,		130,	120,	30 * 10}, 	// 11
	{9,		130,	110,	30 * 10}, 	// 12
	{9,		130,	100,	30 * 10}, 	// 13
	{8,		130,	90,		30 * 12}, 	// 14
	{8,		100,	80,		30 * 12}, 	// 15
	{7,		70,		70,		30 * 12}, 	// 16
	{6,		40,		60,		30 * 12}, 	// 17
	{4,		30,		40,		30 * 12}, 	// 18
	{2,		20,		20,		30 * 12}, 	// 19
};
LOCAL int l_nRemainToNextLevel;
LOCAL PINTERVAL l_pInterval;


//-----------------------------------------------------------------------------
void LEVEL_Initialize(void)
{
	l_nLevel = 0;
	l_pInterval = l_interval;
	l_nRemainToNextLevel = l_pInterval->nRemainToNextLevel;
}


//-----------------------------------------------------------------------------
void LEVEL_Transision()
{
	--l_nRemainToNextLevel;
	if (l_nRemainToNextLevel == 0) {
		++l_nLevel;
		if (l_nLevel >= MAX_LEVEL)
			l_nLevel = MAX_LEVEL - 1;
		l_pInterval = &l_interval[l_nLevel];
		l_nRemainToNextLevel = l_pInterval->nRemainToNextLevel;
	}
}


//-----------------------------------------------------------------------------
// for debugging
int LEVEL_GetLevel(void)
{
	return l_nLevel;
}


//-----------------------------------------------------------------------------
// for debugging
int LEVEL_GetRemainToNextLevel(void)
{
	return l_nRemainToNextLevel;
}


//-----------------------------------------------------------------------------
// for debugging
PINTERVAL LEVEL_GetInterval(void)
{
	return l_pInterval;
}


//-----------------------------------------------------------------------------
void LEVEL_GenerateBullets(int nFrames)
{
	if (nFrames % l_pInterval->nIntervalBulletA == l_pInterval->nIntervalBulletA - 1) {
		BULLET_AddBulletA();
	}
	
	if (nFrames % l_pInterval->nIntervalBulletB == l_pInterval->nIntervalBulletB - 1) {
		BULLET_AddBulletB();
	}
	
	if (nFrames % l_pInterval->nIntervalBulletC == l_pInterval->nIntervalBulletC - 1) {
		BULLET_AddBulletC();
	}
}
