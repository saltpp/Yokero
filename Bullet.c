//=============================================================================
// Bullet.c / Programmed by Salt
//
// $Id: Bullet.c,v 1.10 2005/08/21 14:23:04 Administrator Exp $

#include "std.h"
#include "Bullet.h"
#include "SaltLibPSP.h"
#include "SinCos.h"
#include "Bitmaps.h"

#define SDUP (8)	// Significant Digit Under Point �����_�ȉ���bits�g�����B2 ���ƃK�^�K�^�A4 �ł��������Y��A6 �ŏ\�������H

#define MK_FPN(x) ((x) << SDUP)
#define MK_INT(x) ((x) >> SDUP)
#define MUL_FPN(x, y) ((x) * (y) >> SDUP)
#define DIV_FPN(x, y) (((x) << SDUP) / (y))


//-----------------------------------------------------------------------------
// struct
typedef struct ___TAG_BULLET_FPN {	// FPN(Fixed Point Number), Fractional part is SDUP bits.
	BOOL bEnable;
	int fpnX, fpnY;
	int fpnDX, fpnDY;
	int nMode;
	int nCount;
	BOOL bCheckCrashing;
	unsigned short *pusBulletBmp;
} BULLET_FPN, *PBULLET_FPN;


//-----------------------------------------------------------------------------
// local variables
#define MAX_BULLET_COUNT 0x400	// const int c_nMaxBulletCount = 0x200;
LOCAL BULLET_FPN l_bullet[MAX_BULLET_COUNT];
LOCAL int l_nMaxBullet;			// �ő�̐��B�r���Ŏg���ĂȂ��Ƃ��������B
LOCAL int l_nShipX;
LOCAL int l_nShipY;
LOCAL int l_bInvincibility;


//-----------------------------------------------------------------------------
void BULLET_Initialize(void)
{
	l_nMaxBullet = 0;
}


//-----------------------------------------------------------------------------
// for debugging
int BULLET_GetCount(void)
{
	return l_nMaxBullet;
}


//-----------------------------------------------------------------------------
// (x1, y1) --> (x2, y2)
LOCAL void InitBulletFPN(int fpnX1, int fpnY1, int fpnX2, int fpnY2, int fpnSpeed, BULLET_FPN *bullet)
{
	bullet->fpnX = fpnX1;
	bullet->fpnY = fpnY1;
	
	int fpnDX = (fpnX2 > fpnX1) ? (fpnX2 - fpnX1) : (fpnX1 - fpnX2);
	int fpnDY = (fpnY2 > fpnY1) ? (fpnY2 - fpnY1) : (fpnY1 - fpnY2);
	
	// fail safe
	if (fpnDX == 0 && fpnDY == 0) {
		fpnDX = (Salt_rand() % 0x20) + 1;
		fpnDY = (Salt_rand() % 0x20) + 1;
	}
	
	// int fpnDistance = ui_sqrt(MUL_FPN(fpnDX, fpnDX) + MUL_FPN(fpnDY, fpnDY));									// �����ӂꂷ��
	// int fpnDistance = MK_FPN(ui_sqrt(MK_INT(fpnDX) * MK_INT(fpnDX) + MK_INT(fpnDY) * MK_INT(fpnDY)));			// int �ɂ��Ă����悷��Ό����ӂꂵ�Ȃ�
	int fpnDistance = ui_sqrt((MK_INT(fpnDX) * MK_INT(fpnDX) + MK_INT(fpnDY) * MK_INT(fpnDY)) << 4) << (SDUP - 2);	// ��ɂS�悵�Ă����Ɛ��x���������낤�B�iSDUP �� 2 �ȏ�K�v�j

	// bullet->fpnDX = DIV_FPN(MUL_FPN((fpnX2 - fpnX1), fpnSpeed), fpnDistance);	// ���ꂾ�ƁAMUL_FPN() �� >> SDUP ��������� << SDUP ���Ă��܂��̂Ő��x����
	// bullet->fpnDY = DIV_FPN(MUL_FPN((fpnY2 - fpnY1), fpnSpeed), fpnDistance);
	bullet->fpnDX = (fpnX2 - fpnX1) * fpnSpeed / fpnDistance;						// ����ŁA>> SDUP �� << SDUP �𑊎E�ł���
	bullet->fpnDY = (fpnY2 - fpnY1) * fpnSpeed / fpnDistance;
}


//-----------------------------------------------------------------------------
// (x1, y1) --> theta
LOCAL void InitDirectedBulletFPN(int fpnX1, int fpnY1, int nTheta, int fpnSpeed, BULLET_FPN *bullet)
{
	bullet->fpnX = fpnX1;
	bullet->fpnY = fpnY1;
	
	int nDir = (nTheta % 360 + 360) % 360;
	
	bullet->fpnDX = MUL_FPN(fpn8Sin[nDir], fpnSpeed) >> (8 - SDUP);		// SDUP �� 8 �ȉ��̕K�v����
	bullet->fpnDY = MUL_FPN(fpn8Cos[nDir], fpnSpeed) >> (8 - SDUP);		// sin/cos �̃e�[�u���� 8 �ȏ㍶�V�t�g���Ă����΁ASDUP �� 8 �ȏ�ɂł���
}


//-----------------------------------------------------------------------------
LOCAL int AddBullet(PBULLET_FPN pbullet)
{
	int i;
	PBULLET_FPN p = l_bullet;
	
	// �󂫂�T���āA����΁A�����Ɋi�[
	for (i = 0; i < l_nMaxBullet; i++) {
		if (!p->bEnable) {
			*p = *pbullet;
			p->bEnable = TRUE;
			break;
		}
		++p;
	}
	// ������΁A
	if (i == l_nMaxBullet) {
		if (l_nMaxBullet < MAX_BULLET_COUNT) {
			// MAX �����Ȃ�Ō�Ɋi�[
			*p = *pbullet;
			p->bEnable = TRUE;
			++l_nMaxBullet;
		}
	}
	
	return l_nMaxBullet;
}


//-----------------------------------------------------------------------------
LOCAL int DelBullet(PBULLET_FPN pbullet)
{
	// disable �ɂ���
	pbullet->bEnable = FALSE;
	
	// �Ō�̕s�v����������Ă����B�@�`��A�����蔻��̃��[�v���팸���邾���B���Ȃ��Ă��������ǁB
	PBULLET_FPN p = &l_bullet[l_nMaxBullet - 1];
	while (p >= l_bullet) {
		if (p->bEnable)
			break;
		--l_nMaxBullet;
		--p;
	}
	
	return l_nMaxBullet;
}


//-----------------------------------------------------------------------------
// �}�W�b�N�i���o�[���炯���ȁA�e�[�u����������Ċ�������
LOCAL void MoveBullet_Mode0(PBULLET_FPN pbullet)
{
	if (pbullet->nCount < 10) {
		pbullet->pusBulletBmp = usBulletA4;
	} else if (pbullet->nCount < 20) {
		pbullet->pusBulletBmp = usBulletA3;
	} else if (pbullet->nCount < 30) {
		pbullet->pusBulletBmp = usBulletA2;
	} else if (pbullet->nCount == 30) {
		pbullet->pusBulletBmp = usBulletA1;

		// �����ŏ��߂Ēe�����v�Z����
		unsigned int nSpeed = (Salt_rand() % 2) + 1;
		InitBulletFPN(pbullet->fpnX, pbullet->fpnY,
						MK_FPN(l_nShipX), MK_FPN(l_nShipY),
						MK_FPN(nSpeed),
						pbullet);

		pbullet->bCheckCrashing = TRUE;
	}
	
	if (pbullet->nCount > 30) {
		pbullet->fpnX += pbullet->fpnDX;
		pbullet->fpnY += pbullet->fpnDY;
	}
	
	pbullet->nCount++;
}


//-----------------------------------------------------------------------------
LOCAL void MoveBullet_Mode1(PBULLET_FPN pbullet)
{
	if (pbullet->nCount < 30) {
		pbullet->pusBulletBmp = usBulletB4;
	} else if (pbullet->nCount < 60) {
		pbullet->pusBulletBmp = usBulletB3;
	} else if (pbullet->nCount < 90) {
		pbullet->pusBulletBmp = usBulletB2;
	} else if (pbullet->nCount == 90) {
		pbullet->pusBulletBmp = usBulletB1;
	
		pbullet->bCheckCrashing = TRUE;
	}
	
	if (pbullet->nCount > 90) {
		pbullet->fpnX += pbullet->fpnDX;
		pbullet->fpnY += pbullet->fpnDY;
	}
	
	pbullet->nCount++;
}


//-----------------------------------------------------------------------------
LOCAL void MoveBullet_Mode2(PBULLET_FPN pbullet)
{
	if (pbullet->nCount < 40) {
		pbullet->pusBulletBmp = usBulletC4;
	} else if (pbullet->nCount < 80) {
		pbullet->pusBulletBmp = usBulletC3;
	} else if (pbullet->nCount < 120) {
		pbullet->pusBulletBmp = usBulletC2;
	} else if (pbullet->nCount == 120) {
		pbullet->pusBulletBmp = usBulletC1;
	
		pbullet->bCheckCrashing = TRUE;
	}
	
	if (pbullet->nCount > 120) {
		pbullet->fpnX += pbullet->fpnDX;
		pbullet->fpnY += pbullet->fpnDY;
	}
	
	pbullet->nCount++;
}


//-----------------------------------------------------------------------------
// bullet �̈ړ�
void BULLET_Move(void)
{
	int i;
	PBULLET_FPN p = l_bullet;
	BOOL bCrashed = FALSE;
	void (*pFunc[])(PBULLET_FPN pbullet) = {MoveBullet_Mode0,
											MoveBullet_Mode1,
											MoveBullet_Mode2};
	
	for (i = 0; i < l_nMaxBullet; i++) {
		if (p->bEnable) {
			// ASSERT(p->nMode < 3);
			pFunc[p->nMode](p);
		}
		
		++p;
	}
}


//-----------------------------------------------------------------------------
// ��ʊO����A�`��
// ���� MoveBullets() �̃��[�v�ɓ������������I�����A�����������킩��₷��
void BULLET_Draw(void)
{
	int i;
	PBULLET_FPN p = l_bullet;

	for (i = 0; i < l_nMaxBullet; i++) {
		if (p->bEnable) {
			// ��ʔ���
			if (p->fpnX < MK_FPN(-8)
					|| MK_INT(p->fpnX) > SCREEN_WIDTH
					|| p->fpnY < MK_FPN(-8)
					|| MK_INT(p->fpnY) > SCREEN_HEIGHT) {
				DelBullet(p);
			}
			else {
				LCD_BitBltWithMask(MK_INT(p->fpnX), MK_INT(p->fpnY), 8, 8, p->pusBulletBmp, p->pusBulletBmp);
			}
		}
		
		++p;
	}
}


//-----------------------------------------------------------------------------
// �����蔻��
// ��� BULLET_SetShipLocation() ���Ă΂�Ă���K�v����
// ���� MoveBullets() �̃��[�v�ɓ������������I�����A�����������킩��₷��
int BULLET_CheckCrashing(void)
{
	int i;
	PBULLET_FPN p = l_bullet;
	BOOL bCrashed = FALSE;

	for (i = 0; i < l_nMaxBullet; i++) {
		if (p->bEnable) {
			// �����蔻��
			if (!l_bInvincibility && p->bCheckCrashing) {
				if (p->fpnX > MK_FPN(l_nShipX + 3)
					|| p->fpnX < MK_FPN(l_nShipX - 3)
					|| p->fpnY > MK_FPN(l_nShipY + 3)
					|| p->fpnY < MK_FPN(l_nShipY - 3)) {
				}
				else {
					bCrashed = TRUE;
					p->fpnDX = 0;
					p->fpnDY = 0;
				}
			}
		}
		++p;
	}
	
	return bCrashed;
}


//-----------------------------------------------------------------------------
// �����蔻��p�� my ship �̍��W�� BULLET ���W���[���ɑ΂��Đݒ肷��
int BULLET_SetShipLocation(int nX, int nY, BOOL bInvincibility)
{
	l_nShipX = nX;
	l_nShipY = nY;
	l_bInvincibility = bInvincibility;
}


//-----------------------------------------------------------------------------
void BULLET_AddBulletA(void)
{
	unsigned int nX = Salt_rand() % SCREEN_WIDTH;
	unsigned int nY = Salt_rand() % SCREEN_HEIGHT;
	BULLET_FPN bullet;
	bullet.fpnX = MK_FPN(nX);
	bullet.fpnY = MK_FPN(nY);
	bullet.nCount = 0;
	bullet.nMode = 0;
	bullet.bCheckCrashing = FALSE;
	AddBullet(&bullet);
}


//-----------------------------------------------------------------------------
void BULLET_AddBulletB(void)
{
	unsigned int nX = Salt_rand() % SCREEN_WIDTH;
	unsigned int nY = Salt_rand() % SCREEN_HEIGHT;
	BULLET_FPN bullet;
	bullet.fpnX = MK_FPN(nX);
	bullet.fpnY = MK_FPN(nY);
	bullet.nCount = 0;
	bullet.nMode = 1;
	bullet.bCheckCrashing = FALSE;
	int i;
	for (i = 0; i < 360; i += 30) {
		InitDirectedBulletFPN(bullet.fpnX, bullet.fpnY, i, MK_FPN(2), &bullet);
		AddBullet(&bullet);
	}
}


//-----------------------------------------------------------------------------
void BULLET_AddBulletC(void)
{
	unsigned int nX = Salt_rand() % SCREEN_WIDTH;
	unsigned int nY = Salt_rand() % SCREEN_HEIGHT;
	BULLET_FPN bullet;
	bullet.fpnX = MK_FPN(nX);
	bullet.fpnY = MK_FPN(nY);
	bullet.nCount = 0;
	bullet.nMode = 2;
	bullet.bCheckCrashing = FALSE;
	int i;
	for (i = 0; i < 360; i += 10) {
		bullet.nCount++;
		InitDirectedBulletFPN(bullet.fpnX, bullet.fpnY, i, MK_FPN(2), &bullet);
		AddBullet(&bullet);
	}
}
