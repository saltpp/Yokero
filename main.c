//=============================================================================
// main.c / Programmed by Salt
//
// $Id: main.c,v 1.12 2005/08/21 14:23:04 Administrator Exp $
//
// Yokero Ver.1.01
//
// tab4 で見てね。
// 
// コメント入れてみたけど、結構面倒なので適当だ。
// まぁ、そこそこ綺麗にまとまってるでしょ？

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
LOCAL int l_nScore;					// nFrame * 10 だけど、掛けるより足したほうが速そうなので用意しとく
LOCAL BOOL l_bInvincibility;		// 無敵状態か
LOCAL BOOL l_bUsedInvincibility;	// 無敵を使ったか


//-----------------------------------------------------------------------------
// HOME キー対応
// Web からそのままコピー

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
// 中央に表示、80 文字以下
void DispStringCenter(int nY, char *szString)
{
	LCD_puts((SCREEN_WIDTH - strlen_s(szString, 80) * 6) >> 1, nY, szString);
}


//-----------------------------------------------------------------------------
// 中央に表示、点数付き、80 文字以下
void DispStringDecimalCenter(int nY, char *szString, int nValue)
{
	char szBuf[80];
	char *p = strcpy_s(szBuf, sizeof(szBuf) - 10, szString);	// 2^31 = 2147483648 = 10桁
	DWORDtoDECIMALSTR(p, nValue);
	DispStringCenter(nY, szBuf);
}


//-----------------------------------------------------------------------------
// タイトル文字列
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
// @return	TRUE	[START] ボタンが押された
// @return	FALSE	time out
BOOL TitleLoop(void)
{
	// Initialize
	STAR_SetDirection(STAR_CENTER);
	
	int nCount = 60 * 5;			// 5 sec
	while (--nCount) {
		LCD_FillVRAM(COLOR_BG);		// 全画面消去
		
		STAR_MoveAndDraw();			// 星の移動
		
		DispTitleStrings();			// text
		
		if (GetPadData(EMODE_PAD) & CTRL_START)
			return TRUE;
		
		LCD_WaitVSync();
		LCD_FlipFrame();
	}
	
	return FALSE;
}


//-----------------------------------------------------------------------------
// デバッグ情報表示
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
// スコア表示
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
	
	STAR_Initialize();		// 星の初期化、rand を使うので、PADREC の後で
	
	int nFrames = 0;
	l_nScore = 0;
	l_bInvincibility = FALSE;
	l_bUsedInvincibility = FALSE;
	
	while (TRUE) {
		LCD_FillVRAM(COLOR_BG);				// 全画面消去
		
		STAR_MoveAndDraw();					// 星の移動
		STAR_SetDirection(STAR_CENTER);
		
		WORD wButtons = GetPadData(mode);	// read pad
		if (mode == EMODE_PAD)
			PADREC_Record(wButtons);		// record
		
		// ship の速度
		int nSpeed = (wButtons & CTRL_SQUARE) ? 4 : ((wButtons & CTRL_CROSS) ? 3 : 2);
		
		// ship の移動
		if (wButtons & CTRL_UP)
			l_nShipY -= nSpeed;
		if (wButtons & CTRL_DOWN)
			l_nShipY += nSpeed;
		if (wButtons & CTRL_LEFT) {
			l_nShipX -= nSpeed;
			STAR_SetDirection(STAR_RIGHT);	// 星は逆方向へ
		}
		if (wButtons & CTRL_RIGHT) {
			l_nShipX += nSpeed;
			STAR_SetDirection(STAR_LEFT);	// 星は逆方向へ
		}
		l_nShipX = MIN(MAX(l_nShipX, 0), SCREEN_WIDTH - 8);	// 画面内に収める
		l_nShipY = MIN(MAX(l_nShipY, 0), SCREEN_HEIGHT - 8);	// 画面内に収める
		
		// 無敵判定
		if (wButtons & CTRL_TRIANGLE)		// △＝無敵
			l_bInvincibility = l_bUsedInvincibility = TRUE;
		else
			l_bInvincibility = FALSE;
		
		// my ship の描画
		LCD_BitBltWithMask(l_nShipX, l_nShipY, 8, 8, l_bInvincibility ? usShip2 : usShip, usShip);
		
		// bullet 生成
		LEVEL_GenerateBullets(nFrames);
		
		// bullet 描画
		BULLET_SetShipLocation(l_nShipX, l_nShipY, l_bInvincibility);
		BULLET_Move();
		BULLET_Draw();
		if (BULLET_CheckCrashing())		// 当たったら
			break;						// 抜ける
		
		if (mode == EMODE_FILE) {		// demo 中ならボタン押下で抜ける
			if (GetPadData(EMODE_PAD) & (CTRL_SQUARE | CTRL_TRIANGLE | CTRL_CIRCLE | CTRL_CROSS | CTRL_START))
				break;
			DispTitleStrings();			// demo 中なら title 表示
		}
		
		// disp score
		DispScore();
		
		// level 遷移
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
		// ハイスコア判定
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
		nCount = 60 * 20;		// 20 sec * 2（LCD_WaitVSync() が１回余分に入ってるので * 2）
	else
		nCount = 60 * 5;		// 5 sec * 2
	
	while (--nCount) {
		// 全画面消去
		LCD_FillVRAM(COLOR_BG);
		
		// 星
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
				bHighScoreDisp = !bHighScoreDisp;		// 点滅させる
			}
			DispStringDecimalCenter(((SCREEN_HEIGHT - 8) >> 1) + 12, "High score is ", l_nHighScore);
		}
		
		// disp score
		DispScore();
		
		// ボタンが押されたら抜ける
		if (GetPadData(EMODE_PAD) & (CTRL_SQUARE | CTRL_TRIANGLE | CTRL_CIRCLE | CTRL_CROSS | CTRL_START)) {
			break;
		}
		
		Salt_rand();		// rnd をまわしておく
		
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
	
	LCD_SetDrawFrame(0);					// 描画フレームを 0 にして
	LCD_FillVRAM(RGB(0x00, 0x1f, 0x1f));	// 描画フレーム 0 をクリア
	LCD_SetDrawFrame(1);					// 描画フレームを 1 にして
	LCD_FillVRAM(RGB(0x00, 0x1f, 0x1f));	// 描画フレーム 1 をクリア
	LCD_InitShowFrame(1);					// 表示フレーム 1 で初期化、これが必要らしい。
	LCD_InitShowFrame(0);					// 表示フレーム 0 で初期化、こっちは必要ないか？
	LCD_SetShowFrame(0);					// 表示フレームを 0 に
	
	// initialize data
	STAR_Initialize();
	l_nHighScore = LoadHighScore();
	PADREC_LoadPlayData(FILE_NAME_PADDATA);
	
	// main loop
	while (TRUE) {
		// タイトル表示
		while (TRUE) {
			if (TitleLoop())
				break;
			GameLoop(EMODE_FILE);		// demo
			EndingLoop(EMODE_FILE);		// demo
		}
		
		// ゲームのメイン
		GameLoop(EMODE_PAD);
		
		// 最後の１フレームに切り替える
		LCD_WaitVSync();
		LCD_FlipFrame();
		LCD_FillVRAM(COLOR_BG);	// これが無いと画面が崩れる。[HOME]ボタンを何回か押すとちゃんと描画されるみたいだが。
		
		// ボタンが離されるまで待つ。押されてると次の EndingLoop() を抜けてしまうので。
		while (TRUE) {
			if (!(GetPadData(EMODE_PAD) & (CTRL_SQUARE | CTRL_TRIANGLE | CTRL_CIRCLE | CTRL_CROSS)))
				break;
		}
		
		// スコア表示
		EndingLoop(EMODE_PAD);
	}
	
	
	// なんかで抜けたら VSync を待っておく。これで HOME ボタンで戻れる。
	// 抜ける訳ないので削除。
	// while (TRUE)
	//	LCD_WaitVSync();
	
	return 0;
}
