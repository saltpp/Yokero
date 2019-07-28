//=============================================================================
// Bullet.h / Programmed by Salt
//
// $Id: Bullet.h,v 1.4 2005/08/17 14:50:05 Administrator Exp $

#if !defined(___BULLET_H___)
#define ___BULLET_H___


// exports
void BULLET_Initialize(void);
int BULLET_GetCount(void);
void BULLET_Move(void);
void BULLET_Draw(void);
int BULLET_CheckCrashing(void);
int BULLET_SetShipLocation(int nX, int nY, BOOL bInvincibility);
void BULLET_AddBulletA(void);
void BULLET_AddBulletB(void);
void BULLET_AddBulletC(void);


#endif // !defined(___BULLET_H___)
