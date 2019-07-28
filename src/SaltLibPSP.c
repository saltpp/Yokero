//=============================================================================
// SaltLibPSP.c / Programmed by Salt
//
// $Id: SaltLibPSP.c,v 1.8 2005/08/21 14:23:04 Administrator Exp $

#include "std.h"
#include "SaltLibPSP.h"
#include "Fonts5x7.h"


//=============================================================================
// string

//-----------------------------------------------------------------------------
/**
 * Copy a string.
 * @param	pDst		Location of destination string buffer.
 * @param	sizeInBytes	Size of destinationstring buffer.
 * @param	pSrc		Null-terminated string to copy.
 * @return	char *		pDst + strlen(pSrc) is returned.
 */
char *strcpy_s(char *pDst, size_t sizeInBytes, const char *pSrc)
{
	while (*pSrc && --sizeInBytes)
		*pDst++ = *pSrc++;

	*pDst = '\0';

	return pDst;
}


//-----------------------------------------------------------------------------
/**
 * Get the length of a string.
 * @param	pszBuf	Null-terminated string.
 * @param	sizeMax	The maximum size of the string, excluding the terminating null character.
 * @return	size_t	The number of characters in string, not including the terminating NULL character.
 */
size_t strlen_s(char *pszBuf, size_t sizeMax)
{
	char *p = pszBuf;
	while (*p && (p - pszBuf) < sizeMax)
		p++;

	return p - pszBuf;
}


//=============================================================================
// LCD

LOCAL int l_nDrawFrame;
LOCAL int l_nShowFrame;

//-----------------------------------------------------------------------------
unsigned short *LCD_GetVRAMAddress(int x, int y)
{
	return GetVRAMAddressDraw(x, y);
}


//-----------------------------------------------------------------------------
void LCD_SetDrawFrame(int n)
{
	l_nDrawFrame = n;
}


//-----------------------------------------------------------------------------
int LCD_GetDrawFrame(void)
{
	return l_nDrawFrame;
}


//-----------------------------------------------------------------------------
void LCD_SetShowFrame(int n)
{
	l_nShowFrame = n;
	//sceDisplaySetFrameBuf((unsigned short *) VRAM_BASE + (l_nShowFrame ? VRAM_FRAME_SIZE : 0), VRAM_WIDTH, 1, 0);
	sceDisplaySetFrameBuf((unsigned short *) VRAM_BASE + (l_nShowFrame ? VRAM_FRAME_SIZE : 0) + 16 * 512, VRAM_WIDTH, 1, 0);
}


//-----------------------------------------------------------------------------
void LCD_InitShowFrame(int n)
{
	l_nShowFrame = n;
	//sceDisplaySetFrameBuf((unsigned short *) VRAM_BASE + (l_nShowFrame ? VRAM_FRAME_SIZE : 0), VRAM_WIDTH, 1, 1);	// init の時は 1 にする
	sceDisplaySetFrameBuf((unsigned short *) VRAM_BASE + (l_nShowFrame ? VRAM_FRAME_SIZE : 0) + 16 * 512, VRAM_WIDTH, 1, 1);
}


//-----------------------------------------------------------------------------
void LCD_FlipFrame(void)
{
	l_nDrawFrame = l_nDrawFrame ? 0 : 1;
	LCD_SetShowFrame(l_nDrawFrame ? 0 : 1);		// 常に draw frame と逆にする
}


//-----------------------------------------------------------------------------
BOOL LCD_PutPixcel(int nX, int nY, COLORREF color)
{
	unsigned short *pusAddress = GetVRAMAddressDraw(nX, nY);

	*pusAddress = color;

	return TRUE;
}


//-----------------------------------------------------------------------------
BOOL LCD_BitBlt(int nX, int nY, int nWidth, int nHeight, unsigned short *pusSource)
{
	int i, j;

	unsigned short *pusAddress = GetVRAMAddressDraw(nX, nY);

	for (i = 0; i < nHeight; i++) {
		for (j = 0; j < nWidth; j++) {
			*pusAddress++ = *pusSource++;
		}
		pusAddress += VRAM_WIDTH - nWidth;
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
BOOL LCD_BitBltWithMask(int nX, int nY, int nWidth, int nHeight, unsigned short *pusSource, unsigned short *pusMask)
{
	int i, j;

	unsigned short *pusAddress = GetVRAMAddressDraw(nX, nY);

	for (i = 0; i < nHeight; i++) {
		for (j = 0; j < nWidth; j++) {
			if (*pusMask++)
				*pusAddress = *pusSource;
			pusAddress++;
			pusSource++;
		}
		pusAddress += VRAM_WIDTH - nWidth;
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
BOOL LCD_PutFont5x7(int nX, int nY, char c)
{
	int i, j;
	unsigned short *pusFont;
	unsigned short *pusAddress = GetVRAMAddressDraw(nX, nY);

	pusFont = &usFonts5x7[(c - ' ') * 7][0];
	for (i = 0; i < 7; i++) {
		for (j = 0; j < 5; j++)
			*pusAddress++ = *pusFont++;
		pusAddress += VRAM_WIDTH - 5;
	}
	
	return TRUE;
}


//-----------------------------------------------------------------------------
BOOL LCD_PutFont5x7Transparent(int nX, int nY, char c)
{
	unsigned short *pusFont = &usFonts5x7[(c - ' ') * 7][0];

	return LCD_BitBltWithMask(nX, nY, 5, 7, pusFont, pusFont);
}


//-----------------------------------------------------------------------------
int LCD_puts5x7(int nX, int nY, const char *szStr)
{
	const char *p = szStr;
	while (*p && p < szStr + 0x100 /* MAX_LENGTH */) {
		// LCD_PutFont5x7(nX, nY, *p++);
		LCD_PutFont5x7Transparent(nX, nY, *p++);
		nX += (5 + 1);
	}

	return p - szStr;
}


//-----------------------------------------------------------------------------
int LCD_puts(int nX, int nY, const char *str)
{
	return LCD_puts5x7(nX, nY, str);
}


//-----------------------------------------------------------------------------
void LCD_WaitVSync(void)
{
	sceDisplayWaitVblankStart();
}


//-----------------------------------------------------------------------------
void LCD_FillVRAM(COLORREF color)
{
	unsigned long *pulAddress = (unsigned long *) GetVRAMAddressDraw(0, 0);
	int x, y;
	unsigned long ulColor = (((unsigned long) color << 16) | color);

	for (y = 0; y < SCREEN_HEIGHT; ++y) {
		for (x = 0; x < (SCREEN_WIDTH >> 1); ++x) {
			*pulAddress++ = ulColor;
		}
		pulAddress += ((VRAM_WIDTH - SCREEN_WIDTH) >> 1);
	}
}


//=============================================================================
// Others

//-----------------------------------------------------------------------------
LOCAL unsigned int l_nRND = 123;
unsigned int Salt_rand(void)
{
	l_nRND *= 1566083941;	// 69069, 1664525, 39894229, 48828125, 1566083941. 1812433253, 2100005341
	l_nRND += 13;	// 5;

	return l_nRND;
}


//-----------------------------------------------------------------------------
void Salt_srand(unsigned int uiSeed)
{
	l_nRND = uiSeed;
}


//-----------------------------------------------------------------------------
// アルゴリズム辞典の isqrt() から
unsigned int ui_sqrt(unsigned int x)
{
	unsigned int s, t;

	if (x == 0) return 0;
	s = 1;  t = x;
	
	while (s < t) {
		s <<= 1;
		t >>= 1;
	}
	
	do {
		t = s;
		s = (x / s + s) >> 1;
	} while (s < t);

	return t;
}


//-----------------------------------------------------------------------------
/**
 * 8bits を１６進２桁キャラクタに置き換え
 * @param	szBuffer	格納先
 * @param	bValue		変換する値
 * @return	置き換えた桁数（'\\0'を含まない）
 */
int BYTEtoHEXSTR(char *szBuffer, BYTE bValue)
{
	char *p = szBuffer;

	*p++ = NIBBLE2HEXCHAR((bValue & 0xf0) >> 4);
	*p++ = NIBBLE2HEXCHAR(bValue & 0x0f);
	*p = '\0';

	return p - szBuffer;
}


//-----------------------------------------------------------------------------
/**
 * 16bits を１６進４桁キャラクタに置き換え
 * @param	szBuffer	格納先
 * @param	wValue		変換する値
 * @return	置き換えた桁数（'\\0'を含まない）
 */
int WORDtoHEXSTR(char *szBuffer, WORD wValue)
{
	char *p = szBuffer;

	p += BYTEtoHEXSTR(p, (wValue & 0xff00) >> 8);
	p += BYTEtoHEXSTR(p, (wValue & 0x00ff));
	*p = '\0';

	return p - szBuffer;
}


//-----------------------------------------------------------------------------
/**
 * 32bits を１６進８桁キャラクタに置き換え
 * @param	szBuffer	格納先
 * @param	wValue		変換する値
 * @return	置き換えた桁数（'\\0'を含まない）
 */
int DWORDtoHEXSTR(char *szBuffer, DWORD dwValue)
{
	char *p = szBuffer;

	p += WORDtoHEXSTR(p, (dwValue & 0xffff0000) >> 16);
	p += WORDtoHEXSTR(p, (dwValue & 0x0000ffff));
	*p = '\0';

	return p - szBuffer;
}


//-----------------------------------------------------------------------------
/**
 * 32bits を十進キャラクタに置き換え
 * @param	szBuffer	格納先
 * @param	nValue		変換する値
 * @return	置き換えた桁数（'\\0'を含まない）
 */
int DWORDtoDECIMALSTR(char *szBuffer, int nValue)
{
	int nDivisor = 1000000000;		// 2^31 = 2147483648 = 10桁
	char *p = szBuffer;

	int n;
	while (nDivisor) {
		n = nValue / nDivisor;
		if (n)
			break;
		nDivisor /= 10;
	}
	if (nDivisor == 0)
		*p++ = '0';
	else {
		while (nDivisor) {
			*p++ = '0' + (nValue / nDivisor);
			nValue = nValue % nDivisor;
			nDivisor /= 10;
		}
	}

	*p = '\0';

	return p - szBuffer;
}
