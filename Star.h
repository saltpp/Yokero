//=============================================================================
// Star.h / Programmed by Salt
//
// $Id: Star.h,v 1.4 2005/08/12 17:58:51 Administrator Exp $

#if !defined(___STAR_H___)
#define ___STAR_H___


typedef enum {
	STAR_LEFT = -1,
	STAR_CENTER,	// = 0,
	STAR_RIGHT,		// = 1,
} STAR_DIRECTION;


// exports
void STAR_Initialize(void);
int STAR_MoveAndDraw(void);
void STAR_SetDirection(STAR_DIRECTION dir);


#endif // !defined(___STAR_H___)
