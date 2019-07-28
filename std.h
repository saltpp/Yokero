//=============================================================================
// std.h / Programmed by Salt
//
// $Id: std.h,v 1.3 2005/08/12 17:58:51 Administrator Exp $

#if !defined(___STD_H___)
#define ___STD_H___


typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef int BOOL;
typedef unsigned long size_t;


#if !defined(NULL)
#define NULL ((void *) 0)
#endif // !defined(NULL)

#if !defined(FALSE)
#define FALSE (0)
#endif // !defined(FALSE)

#if !defined(TRUE)
#define TRUE (!FALSE)
#endif // !defined(TRUE)

#if !defined(LOCAL)
#define LOCAL static
#endif // !defined(LOCAL)


#if !defined(MIN)
#define MIN(x, y) ((x) <= (y) ? (x) : (y))
#endif // !defined(MIN)

#if !defined(MAX)
#define MAX(x, y) ((x) >= (y) ? (x) : (y))
#endif // !defined(MAX)


#if !defined(ASSERT)
#define ASSERT(x)
#endif // !defined(ASSERT)


#endif // !defined(___STD_H___)
