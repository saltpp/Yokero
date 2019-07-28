//=============================================================================
// PadRecorder.c / Programmed by Salt
//
// $Id: PadRecorder.c,v 1.7 2005/08/21 15:26:19 Administrator Exp $

#include "std.h"
#include "syscall.h"


//-----------------------------------------------------------------------------
// local variables

typedef struct {
	DWORD dwVersion;
	DWORD dwReserved;
} PAD_DATA_HEADER, *PPAD_DATA_HEADER;

#define MK_VER(major, minor) (((major) << 16) | (minor))

typedef struct {		// ‚Æ‚è‚ ‚¦‚¸Aˆê”ÔŠÈ’P‚È run length ‚Å•Û‘¶‚·‚é‚±‚Æ‚É‚·‚é
	WORD wPad;
	WORD wLength;
} PAD_DATA, *PPAD_DATA;

#define TERMINATOR 0xffff
#define MAX_COUNT 0xffff
#define MAX_PADDATA_SIZE 0x800

PAD_DATA l_pdPlay[MAX_PADDATA_SIZE] = {{TERMINATOR, MAX_COUNT}, };
PPAD_DATA l_ppdPlay;
LOCAL int l_nRandSeedPlay;	// rand ‚ÌŽí‚Ì•Û‘¶—p
BOOL l_bFirstPlay;

PAD_DATA l_pdRec[MAX_PADDATA_SIZE] = {{TERMINATOR, MAX_COUNT}, };
PPAD_DATA l_ppdRec;
LOCAL int l_nRandSeedRec;	// rand ‚ÌŽí‚Ì•Û‘¶—p
BOOL l_bFirstRec;


//-----------------------------------------------------------------------------
void PADREC_InitializeForRecord(unsigned int uiRandSeed)
{
	l_nRandSeedRec = uiRandSeed;
	l_ppdRec = l_pdRec;
	l_bFirstRec = TRUE;
}


//-----------------------------------------------------------------------------
void PADREC_Record(WORD wPad)
{
	if (l_bFirstRec) {	// ˆê”ÔÅ‰‚ÍŠi”[‚·‚é‚¾‚¯
		l_ppdRec->wPad = wPad;
		l_ppdRec->wLength = 1;
		(l_ppdRec + 1)->wPad = TERMINATOR;
		l_bFirstRec = FALSE;
	}
	else {				// ‚Q‰ñ–ÚˆÈ~‚ÍA‘O‰ñ‚Æ“¯‚¶‚È‚ç{‚PAˆá‚¦‚ÎŽŸ‚ÌêŠ‚ÉŠi”[
		if (l_ppdRec->wLength != MAX_COUNT && l_ppdRec->wPad == wPad)
			++l_ppdRec->wLength;
		else {
			if (l_ppdRec <= &l_pdRec[MAX_PADDATA_SIZE - 1 - 1]) {	// ƒCƒ“ƒfƒbƒNƒX‚ÌÅŒã‚Í|1ATERMINATOR ‚ª•K—v‚È‚Ì‚Å‚³‚ç‚É|1
				++l_ppdRec;
				l_ppdRec->wPad = wPad;
				l_ppdRec->wLength = 1;
				(l_ppdRec + 1)->wPad = TERMINATOR;
			}
		}
	}
}


//-----------------------------------------------------------------------------
void PADREC_InitializeForPlay(void)
{
	Salt_srand(l_nRandSeedPlay);
	l_ppdPlay = l_pdPlay;
	l_bFirstPlay = TRUE;
}


//-----------------------------------------------------------------------------
WORD PADREC_Play(void)
{
	static PAD_DATA sPadData = {0, 0};

	if (l_bFirstPlay) {					// ˆê”ÔÅ‰‚Íƒ[ƒh‚µ‚Ä‚¨‚­
		sPadData = *l_ppdPlay++;
		l_bFirstPlay = FALSE;
	}

	if (sPadData.wLength == 0) {		// ‰ñ”‚ª‚O‚É‚È‚Á‚Ä‚¢‚½‚çŽŸ‚Ì‚ðƒ[ƒh
		sPadData = *l_ppdPlay++;
	}

	if (sPadData.wPad != TERMINATOR) {	// TERMINATOR ‚Å‚È‚¯‚ê‚Î
		--sPadData.wLength;				// ƒJƒEƒ“ƒ^‚ðŒ¸‚ç‚µ‚Ä
		return sPadData.wPad;			// ‚»‚ê‚ð•Ô‚·
	}
	else {								// TERMINATOR ‚È‚ç’âŽ~‚ð•Ô‚·
		return 0x00;
	}
}


//-----------------------------------------------------------------------------
int PADREC_GetRecordedLength(void)
{
	return l_ppdRec - l_pdRec + 1 + 1;	// ƒf[ƒ^‚Í‚»‚ÌƒAƒhƒŒƒX‚É‚à‚ ‚é‚Ì‚Å{‚PATERMINATOR‚Å{‚P
}


//-----------------------------------------------------------------------------
// @return	ƒRƒs[‚µ‚½ƒoƒCƒg”
int PADREC_CopyFromRecToPlay(void)
{
	l_nRandSeedPlay = l_nRandSeedRec;

	PPAD_DATA pSrc = l_pdRec;
	PPAD_DATA pDst = l_pdPlay;
	while (pSrc->wPad != TERMINATOR) {
		*pDst++ = *pSrc++;
	}
	*pDst++ = *pSrc++;	// copy TERMINATOR

	return (pSrc - l_pdPlay) * sizeof(PAD_DATA);
}


//-----------------------------------------------------------------------------
BOOL PADREC_LoadPlayData(const char *szFileName)
{
	ASSERT(szFileName);

	int nResult = 0;
	int f = sceIoOpen(szFileName, O_RDONLY, 0);
	if (f >= 0) {
		PAD_DATA_HEADER header;
		sceIoRead(f, (void *) &header, sizeof(header));
		sceIoRead(f, (void *) &l_nRandSeedPlay, sizeof(l_nRandSeedPlay));
		sceIoRead(f, (void *) l_pdPlay, sizeof(PAD_DATA) * MAX_PADDATA_SIZE);
		sceIoClose(f);
		return TRUE;
	}

	return FALSE;
}


//-----------------------------------------------------------------------------
void PADREC_SaveRecData(const char *szFileName)
{
	ASSERT(szFileName);

	int f = sceIoOpen(szFileName, O_CREAT | O_WRONLY | O_TRUNC, 0777);
	if (f >= 0) {
		PAD_DATA_HEADER header;
		header.dwVersion = MK_VER(1, 0);
		header.dwReserved = 0x00;
		sceIoWrite(f, (void *) &header, sizeof(header));
		sceIoWrite(f, (void *) &l_nRandSeedRec, sizeof(l_nRandSeedRec));
		sceIoWrite(f, (void *) l_pdRec, sizeof(PAD_DATA) * PADREC_GetRecordedLength());
		sceIoClose(f);
	}
}
