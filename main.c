//=============================================================================
// main.c / Programmed by Salt
//
// $Id: main.c,v 1.12 2005/08/21 14:23:04 Administrator Exp $
//
// Yokero Ver.1.01
//
// tab4 �Ō��ĂˁB
// 
// �R�����g����Ă݂����ǁA���\�ʓ|�Ȃ̂œK�����B
// �܂��A���������Y��ɂ܂Ƃ܂��Ă�ł���H

#include "std.h"
#include "SaltLibPSP.h"
#include "syscall.h"
#include "Level.h"
#include "Bitmaps.h"
#include "Star.h"
#include "PadRecorder.h"


#define PROGRAM_VERSION "1.01"
#define COLOR_BG 0x8800


//-----------------------------------------------------------------------------
// enum
typedef enum {
	EMODE_PAD,
	EMODE_FILE,
} EMODE;


//-----------------------------------------------------------------------------
// local variables
LOCAL int l_nShipX;
LOCAL int l_nShipY;
LOCAL int l_nHighScore = 0;
LOCAL int l_nScore;					// nFrame * 10 �����ǁA�|�����葫�����ق����������Ȃ̂ŗp�ӂ��Ƃ�
LOCAL BOOL l_bInvincibility;		// ���G��Ԃ�
LOCAL BOOL l_bUsedInvincibility;	// ���G���g������


//-----------------------------------------------------------------------------
// HOME �L�[�Ή�
// Web ���炻�̂܂܃R�s�[

int exit_callback(void) 
{ 
// Cleanup the games resources etc (if required) 

// Exit game 
   sceKernelExitGame(); 

   return 0; 
} 

#define POWER_CB_POWER		0x80000000
#define POWER_CB_HOLDON		0x40000000
#define POWER_CB_STANDBY	0x00080000
#define POWER_CB_RESCOMP	0x00040000
#define POWER_CB_RESUME		0x00020000
#define POWER_CB_SUSPEND	0x00010000
#define POWER_CB_EXT		0x00001000
#define POWER_CB_BATLOW		0x00000100
#define POWER_CB_BATTERY	0x00000080
#define POWER_CB_BATTPOWER	0x0000007F
void power_callback(int unknown, int pwrflags) 
{ 
   // Combine pwrflags and the above defined masks 
} 

// Thread to create the callbacks and then begin polling 
int CallbackThread(int args, void *arg) 
{ 
   int cbid; 

   cbid = sceKernelCreateCallback("Exit Callback", exit_callback); 
   sceKernelRegisterExitCallback(cbid); 
   cbid = sceKernelCreateCallback("Power Callback", power_callback); 
   scePowerRegisterCallback(0, cbid); 

   KernelPollCallbacks(); 
} 

/* Sets up the callback thread and returns its thread id */ 
int SetupCallbacks(void) 
{ 
   int thid = 0; 

   thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0); 
   if(thid >= 0) 
   { 
      sceKernelStartThread(thid, 0, 0); 
   } 

   return thid; 
} 


//-----------------------------------------------------------------------------
// file
//#define FILE_NAME_SCORE "fatms0:/PSP/GAME/YOKERO/HIGHSCORE.DAT"
#define FILE_NAME_SCORE "ms0:/PSP/GAME/YOKERO/HIGHSCORE.DAT"
#define FILE_NAME_PADDATA "ms0:/PSP/GAME/YOKERO/HIGHSCORE.PAD"


//-----------------------------------------------------------------------------
int LoadHighScore(void)
{
	int nResult = 0;
	int f = sceIoOpen(FILE_NAME_SCORE, O_RDONLY, 0);
	if (f >= 0) {
		sceIoRead(f, (void *) &nResult, sizeof(nResult));
		sceIoClose(f);
	}
	
	return nResult;
}


//-----------------------------------------------------------------------------
void SaveHighScore(int nSocre)
{
	int f = sceIoOpen(FILE_NAME_SCORE, O_CREAT | O_WRONLY | O_TRUNC, 0777);
	if (f >= 0) {
		sceIoWrite(f, (void *) &nSocre, sizeof(nSocre));
		sceIoClose(f);
	}
}


//-----------------------------------------------------------------------------
// �����ɕ\���A80 �����ȉ�
void DispStringCenter(int nY, char *szString)
{
	LCD_puts((SCREEN_WIDTH - strlen_s(szString, 80) * 6) >> 1, nY, szString);
}


//-----------------------------------------------------------------------------
// �����ɕ\���A�_���t���A80 �����ȉ�
void DispStringDecimalCenter(int nY, char *szString, int nValue)
{
	char szBuf[80];
	char *p = strcpy_s(szBuf, sizeof(szBuf) - 10, szString);	// 2^31 = 2147483648 = 10��
	DWORDtoDECIMALSTR(p, nValue);
	DispStringCenter(nY, szBuf);
}


//-----------------------------------------------------------------------------
// �^�C�g��������
void DispTitleStrings(void)
{
	DispStringCenter(((SCREEN_HEIGHT - 8) >> 1) - 18, "Yokero Ver." PROGRAM_VERSION " / Programmed by Salt");
	DispStringDecimalCenter(((SCREEN_HEIGHT - 8) >> 1) - 0, "High score is ", l_nHighScore);
	DispStringCenter(((SCREEN_HEIGHT - 8) >> 1) + 18, "Push [START] button to start");
}


//-----------------------------------------------------------------------------
// Get PAD data
// @param	mode EMODE_FILE	from file
// @param	mode EMODE_PAD	from PAD
// @return	DWORD buttons
WORD GetPadData(EMODE mode)
{
	if (mode == EMODE_FILE) {					// from file
		return PADREC_Play();
	}
	else if (mode == EMODE_PAD) {
		ctrl_data_t paddata;
		sceCtrlRead(&paddata, 1);				// read buttons
		return (WORD) paddata.buttons;
	}
	
	return 0x00;
}


//-----------------------------------------------------------------------------
// title loop
// @return	TRUE	[START] �{�^���������ꂽ
// @return	FALSE	time out
BOOL TitleLoop(void)
{
	// Initialize
	STAR_SetDirection(STAR_CENTER);
	
	int nCount = 60 * 5;			// 5 sec
	while (--nCount) {
		LCD_FillVRAM(COLOR_BG);		// �S��ʏ���
		
		STAR_MoveAndDraw();			// ���̈ړ�
		
		DispTitleStrings();			// text
		
		if (GetPadData(EMODE_PAD) & CTRL_START)
			return TRUE;
		
		LCD_WaitVSync();
		LCD_FlipFrame();
	}
	
	return FALSE;
}


//-----------------------------------------------------------------------------
// �f�o�b�O���\��
void DispDebugInfo(void)
{
	int nY = 0;
	char szBuf[80];

	DWORDtoDECIMALSTR(szBuf, BULLET_GetCount());
	LCD_puts(0, nY += 8, szBuf);
	
	DWORDtoDECIMALSTR(szBuf, LEVEL_GetLevel());
	LCD_puts(0, nY += 8, szBuf);
	
	DWORDtoDECIMALSTR(szBuf, LEVEL_GetRemainToNextLevel());
	LCD_puts(0, nY += 8, szBuf);
	
	DWORDtoDECIMALSTR(szBuf, LEVEL_GetInterval()->nIntervalBulletA);
	LCD_puts(0, nY += 8, szBuf);
	
	DWORDtoDECIMALSTR(szBuf, LEVEL_GetInterval()->nIntervalBulletB);
	LCD_puts(0, nY += 8, szBuf);
	
	DWORDtoDECIMALSTR(szBuf, LEVEL_GetInterval()->nIntervalBulletC);
	LCD_puts(0, nY += 8, szBuf);

	DWORDtoHEXSTR(szBuf, PADREC_GetRecordedLength());
	LCD_puts(0, nY += 8, szBuf);
}


//-----------------------------------------------------------------------------
// �X�R�A�\��
void DispScore(void)
{
	char szBuf[80];

	DWORDtoDECIMALSTR(szBuf, l_nScore);
	LCD_puts(0, 0, szBuf);
}


//-----------------------------------------------------------------------------
// main loop
// @param	EMODE_FILE	demo mode
// @param	EMODE_PAD	play mode
void GameLoop(EMODE mode)
{
	// initialize
	BULLET_Initialize();
	l_nShipX = (SCREEN_WIDTH - 8) >> 1;
	l_nShipY = (SCREEN_HEIGHT - 8)  >> 1;
	
	LEVEL_Initialize();
	
	if (mode == EMODE_FILE)
		PADREC_InitializeForPlay();
	else
		PADREC_InitializeForRecord(Salt_rand());
	
	STAR_Initialize();		// ���̏������Arand ���g���̂ŁAPADREC �̌��
	
	int nFrames = 0;
	l_nScore = 0;
	l_bInvincibility = FALSE;
	l_bUsedInvincibility = FALSE;
	
	while (TRUE) {
		LCD_FillVRAM(COLOR_BG);				// �S��ʏ���
		
		STAR_MoveAndDraw();					// ���̈ړ�
		STAR_SetDirection(STAR_CENTER);
		
		WORD wButtons = GetPadData(mode);	// read pad
		if (mode == EMODE_PAD)
			PADREC_Record(wButtons);		// record
		
		// ship �̑��x
		int nSpeed = (wButtons & CTRL_SQUARE) ? 4 : ((wButtons & CTRL_CROSS) ? 3 : 2);
		
		// ship �̈ړ�
		if (wButtons & CTRL_UP)
			l_nShipY -= nSpeed;
		if (wButtons & CTRL_DOWN)
			l_nShipY += nSpeed;
		if (wButtons & CTRL_LEFT) {
			l_nShipX -= nSpeed;
			STAR_SetDirection(STAR_RIGHT);	// ���͋t������
		}
		if (wButtons & CTRL_RIGHT) {
			l_nShipX += nSpeed;
			STAR_SetDirection(STAR_LEFT);	// ���͋t������
		}
		l_nShipX = MIN(MAX(l_nShipX, 0), SCREEN_WIDTH - 8);	// ��ʓ��Ɏ��߂�
		l_nShipY = MIN(MAX(l_nShipY, 0), SCREEN_HEIGHT - 8);	// ��ʓ��Ɏ��߂�
		
		// ���G����
		if (wButtons & CTRL_TRIANGLE)		// �������G
			l_bInvincibility = l_bUsedInvincibility = TRUE;
		else
			l_bInvincibility = FALSE;
		
		// my ship �̕`��
		LCD_BitBltWithMask(l_nShipX, l_nShipY, 8, 8, l_bInvincibility ? usShip2 : usShip, usShip);
		
		// bullet ����
		LEVEL_GenerateBullets(nFrames);
		
		// bullet �`��
		BULLET_SetShipLocation(l_nShipX, l_nShipY, l_bInvincibility);
		BULLET_Move();
		BULLET_Draw();
		if (BULLET_CheckCrashing())		// ����������
			break;						// ������
		
		if (mode == EMODE_FILE) {		// demo ���Ȃ�{�^�������Ŕ�����
			if (GetPadData(EMODE_PAD) & (CTRL_SQUARE | CTRL_TRIANGLE | CTRL_CIRCLE | CTRL_CROSS | CTRL_START))
				break;
			DispTitleStrings();			// demo ���Ȃ� title �\��
		}
		
		// disp score
		DispScore();
		
		// level �J��
		LEVEL_Transision(nFrames);
		
		// disp information for debugging
		if (wButtons & CTRL_CIRCLE)
			DispDebugInfo();
		
		LCD_WaitVSync();
		LCD_FlipFrame();
		
		nFrames++;
		l_nScore += 10;
	}
}


//-----------------------------------------------------------------------------
// ending loop
void EndingLoop(EMODE mode)
{
	BOOL bHighScore = FALSE;
	BOOL bHighScoreDisp = TRUE;

	if (mode == EMODE_PAD) {
		// �n�C�X�R�A����
		if (!l_bUsedInvincibility && l_nScore > l_nHighScore) {
			l_nHighScore = l_nScore;
			bHighScore = TRUE;
			SaveHighScore(l_nHighScore);
			PADREC_CopyFromRecToPlay();
			PADREC_SaveRecData(FILE_NAME_PADDATA);
		}
	}
	
	int nCount;
	if (mode == EMODE_PAD)
		nCount = 60 * 20;		// 20 sec * 2�iLCD_WaitVSync() ���P��]���ɓ����Ă�̂� * 2�j
	else
		nCount = 60 * 5;		// 5 sec * 2
	
	while (--nCount) {
		// �S��ʏ���
		LCD_FillVRAM(COLOR_BG);
		
		// ��
		STAR_SetDirection(STAR_CENTER);
		STAR_MoveAndDraw();
		
		// ship
		LCD_BitBltWithMask(l_nShipX, l_nShipY, 8, 8, usShip, usShip);
		
		// bullet
		BULLET_Move();
		BULLET_Draw();
		
		// text
		if (mode == EMODE_FILE)
			DispTitleStrings();
		else {
			DispStringDecimalCenter(((SCREEN_HEIGHT - 8) >> 1) - 12, "Your score is ", l_nScore);
			
			if (bHighScore) {
				if (bHighScoreDisp)
					DispStringCenter(((SCREEN_HEIGHT - 8) >> 1) - 0, "It is high score!");
				bHighScoreDisp = !bHighScoreDisp;		// �_�ł�����
			}
			DispStringDecimalCenter(((SCREEN_HEIGHT - 8) >> 1) + 12, "High score is ", l_nHighScore);
		}
		
		// disp score
		DispScore();
		
		// �{�^���������ꂽ�甲����
		if (GetPadData(EMODE_PAD) & (CTRL_SQUARE | CTRL_TRIANGLE | CTRL_CIRCLE | CTRL_CROSS | CTRL_START)) {
			break;
		}
		
		Salt_rand();		// rnd ���܂킵�Ă���
		
		LCD_WaitVSync();
		LCD_WaitVSync();
		LCD_FlipFrame();
	}
}


//-----------------------------------------------------------------------------
// main
int xmain(void)
{
	// initialize H/W
	SetupCallbacks();
	
	sceCtrlInit(0);
	sceCtrlSetAnalogMode(1);
	
	LCD_SetDrawFrame(0);					// �`��t���[���� 0 �ɂ���
	LCD_FillVRAM(RGB(0x00, 0x1f, 0x1f));	// �`��t���[�� 0 ���N���A
	LCD_SetDrawFrame(1);					// �`��t���[���� 1 �ɂ���
	LCD_FillVRAM(RGB(0x00, 0x1f, 0x1f));	// �`��t���[�� 1 ���N���A
	LCD_InitShowFrame(1);					// �\���t���[�� 1 �ŏ������A���ꂪ�K�v�炵���B
	LCD_InitShowFrame(0);					// �\���t���[�� 0 �ŏ������A�������͕K�v�Ȃ����H
	LCD_SetShowFrame(0);					// �\���t���[���� 0 ��
	
	// initialize data
	STAR_Initialize();
	l_nHighScore = LoadHighScore();
	PADREC_LoadPlayData(FILE_NAME_PADDATA);
	
	// main loop
	while (TRUE) {
		// �^�C�g���\��
		while (TRUE) {
			if (TitleLoop())
				break;
			GameLoop(EMODE_FILE);		// demo
			EndingLoop(EMODE_FILE);		// demo
		}
		
		// �Q�[���̃��C��
		GameLoop(EMODE_PAD);
		
		// �Ō�̂P�t���[���ɐ؂�ւ���
		LCD_WaitVSync();
		LCD_FlipFrame();
		LCD_FillVRAM(COLOR_BG);	// ���ꂪ�����Ɖ�ʂ������B[HOME]�{�^�������񂩉����Ƃ����ƕ`�悳���݂��������B
		
		// �{�^�����������܂ő҂B������Ă�Ǝ��� EndingLoop() �𔲂��Ă��܂��̂ŁB
		while (TRUE) {
			if (!(GetPadData(EMODE_PAD) & (CTRL_SQUARE | CTRL_TRIANGLE | CTRL_CIRCLE | CTRL_CROSS)))
				break;
		}
		
		// �X�R�A�\��
		EndingLoop(EMODE_PAD);
	}
	
	
	// �Ȃ񂩂Ŕ������� VSync ��҂��Ă����B����� HOME �{�^���Ŗ߂��B
	// �������Ȃ��̂ō폜�B
	// while (TRUE)
	//	LCD_WaitVSync();
	
	return 0;
}
