//=============================================================================
// SaltLibPSP.h / Programmed by Salt
//
// $Id: SaltLibPSP.h,v 1.4 2005/08/21 13:35:58 Administrator Exp $

#if !defined(___SALTLIBPSP_H___)
#define ___SALTLIBPSP_H___


#include "std.h"


//-----------------------------------------------------------------------------
// string
char *strcpy_s(char *pDst, size_t sizeInBytes, const char *pSrc);
size_t strlen_s(char *pszBuf, size_t sizeMax);


//-----------------------------------------------------------------------------
// VRAM
#define SCREEN_WIDTH  480		// in dot
#define SCREEN_HEIGHT 272		// in dot

#define VRAM_BASE 0x04000000
#define VRAM_WIDTH 512								// in short 16bits * 512dots
#define VRAM_HEIGHT (SCREEN_HEIGHT + 16)			// in short
//#define VRAM_FRAME_SIZE 0x22000					// in short 512 * 272
#define VRAM_FRAME_SIZE (VRAM_WIDTH * VRAM_HEIGHT)	// in short 512 * 272 + 512 * 16	// 16 dots ‚¸‚ç‚µ‚Ä‚Ý‚é

// #define GetVRAMAddressDraw(x, y) ((unsigned short *) VRAM_BASE + (l_nDrawFrame ? VRAM_FRAME_SIZE : 0) + (y * VRAM_WIDTH) + x)
#define GetVRAMAddressDraw(x, y) ((unsigned short *) VRAM_BASE + (l_nDrawFrame ? VRAM_FRAME_SIZE : 0) + (y * VRAM_WIDTH) + x + 16 * 512)	// 16 dots ‚¸‚ç‚µ‚Ä‚Ý‚é


//-----------------------------------------------------------------------------
// COLOR
typedef unsigned short COLORREF;
#define RGB(r, g, b) (0x8000 | ((b & 0x1f) << 10) + ((g & 0x1f) << 5) + (r & 0x1f) + 0x8000)


//-----------------------------------------------------------------------------
// LCD
unsigned short *LCD_GetVRAMAddress(int x, int y);
int LCD_puts(int nX, int nY, const char *str);
BOOL LCD_BitBltWithMask(int nX, int nY, int nWidth, int nHeight, unsigned short *pusSource, unsigned short *pusMask);
BOOL LCD_BitBlt(int nX, int nY, int nWidth, int nHeight, unsigned short *pusSource);


//-----------------------------------------------------------------------------
// others
unsigned int Salt_rand(void);
unsigned int ui_sqrt(unsigned int x);

#define NIBBLE2HEXCHAR(x) ((x) >= 0x0a ? ('A' + (x) - 10) : ('0' + (x)))
int BYTEtoHEXSTR(char *szBuffer, BYTE bValue);
int WORDtoHEXSTR(char *szBuffer, WORD wValue);
int DWORDtoHEXSTR(char *szBuffer, DWORD dwValue);
int DWORDtoDECIMALSTR(char *szBuffer, int nValue);


#endif // !defined(___SALTLIBPSP_H___)
