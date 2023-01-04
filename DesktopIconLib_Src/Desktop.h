#pragma once

#ifdef UNICODE

/* ------------------------------------------------ */

#include <Windows.h>
#include <CommCtrl.h>

/* ------------------------------------------------ */

#define DESKTOP_USE_DEBUG_FUNCTION	TRUE

/* ------------------------------------------------ */

#define INTERNAL
#define NOT_IMPLEMENTED	static

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
	// Handle to the Desktop ListView
	HWND	hwndListview;
	// Handle to the Desktop process
	HANDLE	hProcessExplorer;
	// Item count
	DWORD	dwItemCount;
	// Desktop resources which have to be released
	IRS		resource;
} DESKTOP, * LPDESKTOP;

/* ------------------------------------ Desktop.c - */

// Initializes DESKTOP structrue
BOOL DesktopInit(LPDESKTOP pDesktop);

// Frees memory allocated for DESKTOP structure
BOOL DesktopFree(LPDESKTOP lpDesktop);

/* ---------------------------- ItemInteraction.c - */

LPWSTR GetItemNameFromIndex(LPDESKTOP lpDesktop, INT index);

BOOL GetItemPositionFromIndex(LPDESKTOP lpDesktop, LPPOINT lpPoint, INT index);

BOOL GetItemPositionFromText(LPDESKTOP lpDesktop, LPPOINT lpPoint, LPCWSTR lpText, SIZE_T size);

/* -------------------------------------- Debug.c - */

#if !DESKTOP_USE_DEBUG_FUNCTION
static
#endif
VOID Debug_PrintDesktopAttributes(DESKTOP desktop);

#if !DESKTOP_USE_DEBUG_FUNCTION
static
#endif
VOID Debug_PrintMembersByIndex(DESKTOP desktop, INT index);

#if !DESKTOP_USE_DEBUG_FUNCTION
static
#endif
VOID Debug_PrintInteger(INT integer);

/* ------------------------------------------------ */

#endif
