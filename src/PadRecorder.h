//=============================================================================
// PadRecorder.h / Programmed by Salt
//
// $Id: PadRecorder.h,v 1.4 2005/08/12 17:58:51 Administrator Exp $

#if !defined(___PADRECORDER_H___)
#define ___PADRECORDER_H___


// exports
void PADREC_InitializeForRecord(unsigned int uiRandSeed);
void PADREC_InitializeForPlay(void);
void PADREC_Record(WORD wPad);
WORD PADREC_Play(void);


#endif // !defined(___PADRECORDER_H___)
