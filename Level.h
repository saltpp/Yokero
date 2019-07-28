//=============================================================================
// Level.h / Programmed by Salt
//
// $Id: Level.h,v 1.4 2005/08/17 14:50:05 Administrator Exp $

#if !defined(___LEVEL_H___)
#define ___LEVEL_H___


typedef struct ___TAG_INTERVAL {
	int nIntervalBulletA;
	int nIntervalBulletB;
	int nIntervalBulletC;
	int nRemainToNextLevel;
} INTERVAL, *PINTERVAL;

// exports
void LEVEL_Initialize(void);
void LEVEL_Transision();
int LEVEL_GetLevel(void);
int LEVEL_GetRemainToNextLevel(void);
PINTERVAL LEVEL_GetInterval(void);
void LEVEL_GenerateBullets(int nFrames);


#endif // !defined(___LEVEL_H___)
