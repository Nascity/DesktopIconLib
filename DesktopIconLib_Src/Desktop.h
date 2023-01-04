#pragma once

#ifdef UNICODE

/* ------------------------------------------------ */

#include <Windows.h>
#include <CommCtrl.h>

/* ------------------------------------------------ */

#define DESKTOP_USE_DEBUG_FUNCTION	TRUE

/* ------------------------------------------------ */

typedef struct tagINTERNALRESOURCESTRUCT
{
	// Destination
	LPLVITEMW	lpItems;
	LPWSTR		lpItemNames;
	PUINT		puColumnses;
	PINT		piColFmts;
} IRS, *LPIRS;

typedef struct tagDESKTOP
{
	HWND	hwndListview;
	HANDLE	hProcessExplorer;

	DWORD	dwItemCount;
	WORD	wHorizSpacing;
	WORD	wVertiSpacing;

	IRS		resource;
} DESKTOP, * LPDESKTOP;

typedef POINT CELL;

/* ------------------------------------------------ */

typedef enum { left = 0, up, right, down } DIRECTION;

/* ------------------------------------ Desktop.c - */

BOOL	DesktopInit(LPDESKTOP pDesktop);
BOOL	DesktopFree(LPDESKTOP lpDesktop);

/* ---------------------------- ItemInteraction.c - */

LPWSTR	GetItemNameFromIndex(DESKTOP desktop, INT index);

BOOL	GetItemPositionFromIndex(DESKTOP desktop, INT index, LPPOINT lpPoint);
BOOL	GetItemPositionFromText(DESKTOP desktop, LPCWSTR lpText, SIZE_T size, LPPOINT lpPoint);
#define	GetItemPositionFromTextM(desktop, lpText, lpPoint)	GetItemPositionFromText(desktop, lpText, sizeof(lpText), lpPoint)

BOOL	SetItemPositionFromIndex(DESKTOP desktop, INT index, POINT point);
BOOL	SetItemPositionFromText(DESKTOP desktop, LPCWSTR lpText, SIZE_T size, POINT point);
#define	SetItemPositionFromTextM(desktop, lpText, point)	SetItemPositionFromText(desktop, lpText, sizeof(lpText), point)

BOOL	SetItemCellFromIndex(DESKTOP desktop, INT index, CELL cell);
BOOL	SetItemCellFromText(DESKTOP desktop, LPCWSTR lpText, SIZE_T size, CELL cell);
#define SetItemCellFromTextM(desktop, lpText, cell)		SetItemCellFromText(desktop, lpText, sizeof(lpText), cell)

BOOL	SetItemCellFromIndex(DESKTOP desktop, INT index, CELL cell);
BOOL	SetItemCellFromText(DESKTOP desktop, LPCWSTR lpText, SIZE_T size, CELL cell);
#define	SetItemCellFromTextM(desktop, lpText, cell)		SetItemCellFromText(desktop, lpText, sizeof(lpText), cell)

BOOL	MoveItemCpixelFromIndex(DESKTOP desktop, INT index, DIRECTION direction, INT Cpixel);
BOOL	MoveItemCpixelFromText(DESKTOP desktop, LPCWSTR lpText, SIZE_T size, DIRECTION direction, INT Cpixel);
#define	MoveItemCpixelFromTextM(desktop, lpText, direction, Cpixel)	MoveItemCpixelFromText(desktop, lpText, sizeof(lpText), direction, Cpixel)

BOOL	MoveItemCcellFromIndex(DESKTOP desktop, INT index, DIRECTION direction, INT Ccell);
BOOL	MoveItemCcellFromText(DESKTOP desktop, LPCWSTR lpText, SIZE_T size, DIRECTION direction, INT Ccell);
#define	MoveItemCcellFromTextM(desktop, lpText, direction, Ccell)	MoveItemCcellFromText(desktop, lpText, sizeof(lpText), direction, Ccell)

/* -------------------------------------- Debug.c - */

#if !DESKTOP_USE_DEBUG_FUNCTION
static
#endif
VOID	Debug_PrintMembersByIndex(DESKTOP desktop, INT index);

#if !DESKTOP_USE_DEBUG_FUNCTION
static
#endif
VOID	Debug_PrintInteger(INT integer);

/* ------------------------------------------------ */

#define INTERNAL
#define NOT_IMPLEMENTED	static

/* ------------------------------------------------ */

#endif
