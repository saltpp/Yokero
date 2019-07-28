//=============================================================================
// Star.c / Programmed by Salt
//
// $Id: Star.c,v 1.7 2005/08/19 16:29:21 Administrator Exp $

#include "std.h"
#include "SaltLibPSP.h"
#include "Star.h"


//-----------------------------------------------------------------------------
typedef struct {
	int nX, nY;
	int nDY;
	int nInterval;
} STAR, *PSTAR;


//-----------------------------------------------------------------------------
#define MAX_STAR_COUNT 0x20


//-----------------------------------------------------------------------------
// local variables
LOCAL STAR l_star[MAX_STAR_COUNT];
LOCAL STAR_DIRECTION l_StarDirection;


//-----------------------------------------------------------------------------
void STAR_Initialize(void)
{
	PSTAR p = l_star;
	int i;

	for (i = 0; i < MAX_STAR_COUNT; ++i) {
		p->nX = Salt_rand() % SCREEN_WIDTH;
		p->nY = Salt_rand() % SCREEN_HEIGHT;
		p->nDY = (Salt_rand() % 2) + 2;
		p->nInterval = 0;

		++p;
	}
}


//-----------------------------------------------------------------------------
int STAR_MoveAndDraw(void)
{
	PSTAR p = l_star;
	int i;

	for (i = 0; i < MAX_STAR_COUNT; ++i) {
		unsigned short *pusAddress = LCD_GetVRAMAddress(p->nX, p->nY);
		*pusAddress = 0x8000 | Salt_rand();

		p->nInterval++;
		if (p->nInterval > 3) {
			p->nInterval = 0;
			p->nX += l_StarDirection;
		}
		p->nY += p->nDY;

		if (p->nY >= SCREEN_HEIGHT) {
			p->nX = Salt_rand() % SCREEN_WIDTH;
			p->nY = Salt_rand() % 10;		// ^‰¡‚É•À‚Ô‚Æ–Ú—§‚Â‚Ì‚Å 0 ‚Å‚È‚­ƒ‰ƒ“ƒ_ƒ€‚É‚µ‚Ä‚Ý‚é p->nY = 0;
			p->nDY = (Salt_rand() % 2) + 2;
		}

		++p;
	}
}


//-----------------------------------------------------------------------------
void STAR_SetDirection(STAR_DIRECTION direction)
{
	l_StarDirection = direction;
}
