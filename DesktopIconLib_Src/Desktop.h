#pragma once

#ifdef UNICODE

/* ------------------------------------------------ */

#include <Windows.h>
#include <CommCtrl.h>

/* ------------------------------------------------ */

#define DESKTOP_USE_DEBUG_FUNCTION	TRUE

/* ------------------------------------------------ */

typedef struct tagCELL
{
	WORD	column;
	WORD	row;
} CELL, *LPCELL;

typedef struct tagITEMSNAPSHOT
{
	// Destination
	LPLVITEMW	lpItems;
	LPWSTR		lpItemNames;
	PUINT		puColumnses;
	PINT		piColFmts;
} ITEMSNAPSHOT, * LPITEMSNAPSHOT;

typedef struct tagDESKTOP
{
	HWND	hwndListview;
	HANDLE	hProcessExplorer;

	POINT	ptMysteryNumber;

	DWORD	dwItemCount;
	WORD	wHorizSpacing;
	WORD	wVertiSpacing;

	BOOL			bItemSnapshotValid;
	ITEMSNAPSHOT	iss;
} DESKTOP, * LPDESKTOP;

/* ------------------------------------------------ */

typedef enum { left = 0, up, right, down } DIRECTION;

/* ------------------------------------ Desktop.c - */

BOOL	DesktopInit(LPDESKTOP lpDesktop);
BOOL	DesktopRefresh(LPDESKTOP lpDesktop);
BOOL	DesktopFree(LPDESKTOP lpDesktop);

/* ------------------------- DesktopInteraction.c - */



/* ---------------------------- ItemInteraction.c - */

BOOL	TakeItemSnapshot(LPDESKTOP lpDesktop);
BOOL	FreeItemSnapshot(LPDESKTOP lpDesktop);

INT		GetItemIndexFromText(DESKTOP desktop, LPCWSTR lpText, SIZE_T size);
LPWSTR	GetItemTextFromIndex(DESKTOP desktop, INT index);

BOOL	GetItemPixelFromIndex(DESKTOP desktop, INT index, LPPOINT lpPoint);
BOOL	GetItemPixelFromText(DESKTOP desktop, LPCWSTR lpText, SIZE_T size, LPPOINT lpPoint);
#define	GetItemPixelFromTextM(desktop, lpText, lpPoint)	GetItemPixelFromText(desktop, lpText, sizeof(lpText), lpPoint)

BOOL	GetItemCellFromIndex(DESKTOP desktop, INT index, LPCELL lpCell);
BOOL	GetItemCellFromText(DESKTOP desktop, LPCWSTR lpText, SIZE_T size, LPCELL lpCell);
#define	GetItemCellFromTextM(desktop, lpText, lpCell)	GetItemCellFromText(desktop, lpText, sizeof(lpText), lpCell)

BOOL	SetItemPixelFromIndex(DESKTOP desktop, INT index, POINT point);
BOOL	SetItemPixelFromText(DESKTOP desktop, LPCWSTR lpText, SIZE_T size, POINT point);
#define	SetItemPixelFromTextM(desktop, lpText, point)	SetItemPixelFromText(desktop, lpText, sizeof(lpText), point)

BOOL	SetItemCellFromIndex(DESKTOP desktop, INT index, CELL cell);
BOOL	SetItemCellFromText(DESKTOP desktop, LPCWSTR lpText, SIZE_T size, CELL cell);
#define SetItemCellFromTextM(desktop, lpText, cell)		SetItemCellFromText(desktop, lpText, sizeof(lpText), cell)

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

#define INTERNAL		static
#define NOT_IMPLEMENTED	static

/* ------------------------------------------------ */

#endif
