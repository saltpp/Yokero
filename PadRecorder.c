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

typedef struct {		// �Ƃ肠�����A��ԊȒP�� run length �ŕۑ����邱�Ƃɂ���
	WORD wPad;
	WORD wLength;
} PAD_DATA, *PPAD_DATA;

#define TERMINATOR 0xffff
#define MAX_COUNT 0xffff
#define MAX_PADDATA_SIZE 0x800

PAD_DATA l_pdPlay[MAX_PADDATA_SIZE] = {{TERMINATOR, MAX_COUNT}, };
PPAD_DATA l_ppdPlay;
LOCAL int l_nRandSeedPlay;	// rand �̎�̕ۑ��p
BOOL l_bFirstPlay;

PAD_DATA l_pdRec[MAX_PADDATA_SIZE] = {{TERMINATOR, MAX_COUNT}, };
PPAD_DATA l_ppdRec;
LOCAL int l_nRandSeedRec;	// rand �̎�̕ۑ��p
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
	if (l_bFirstRec) {	// ��ԍŏ��͊i�[���邾��
		l_ppdRec->wPad = wPad;
		l_ppdRec->wLength = 1;
		(l_ppdRec + 1)->wPad = TERMINATOR;
		l_bFirstRec = FALSE;
	}
	else {				// �Q��ڈȍ~�́A�O��Ɠ����Ȃ�{�P�A�Ⴆ�Ύ��̏ꏊ�Ɋi�[
		if (l_ppdRec->wLength != MAX_COUNT && l_ppdRec->wPad == wPad)
			++l_ppdRec->wLength;
		else {
			if (l_ppdRec <= &l_pdRec[MAX_PADDATA_SIZE - 1 - 1]) {	// �C���f�b�N�X�̍Ō�́|1�ATERMINATOR ���K�v�Ȃ̂ł���Ɂ|1
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

	if (l_bFirstPlay) {					// ��ԍŏ��̓��[�h���Ă���
		sPadData = *l_ppdPlay++;
		l_bFirstPlay = FALSE;
	}

	if (sPadData.wLength == 0) {		// �񐔂��O�ɂȂ��Ă����玟�̂����[�h
		sPadData = *l_ppdPlay++;
	}

	if (sPadData.wPad != TERMINATOR) {	// TERMINATOR �łȂ����
		--sPadData.wLength;				// �J�E���^�����炵��
		return sPadData.wPad;			// �����Ԃ�
	}
	else {								// TERMINATOR �Ȃ��~��Ԃ�
		return 0x00;
	}
}


//-----------------------------------------------------------------------------
int PADREC_GetRecordedLength(void)
{
	return l_ppdRec - l_pdRec + 1 + 1;	// �f�[�^�͂��̃A�h���X�ɂ�����̂Ł{�P�ATERMINATOR�Ł{�P
}


//-----------------------------------------------------------------------------
// @return	�R�s�[�����o�C�g��
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
