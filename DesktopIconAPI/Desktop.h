#pragma once

#ifdef UNICODE

/* ------------------------------------------------ */

#include <Windows.h>
#include <CommCtrl.h>

/* ------------------------------------------------ */

typedef struct tagDESKTOP
{
	// Handle to the Desktop ListView
	HWND		hwndListview;

	// Item count
	DWORD		dwItemCount;

	// Is snap-to-grid on?
	BOOL		bSnapToGrid;

	// Desktop resources which have to be released
	struct unnamedstruct
	{
		// Handle to the Desktop process
		HANDLE		hProcess;

		// Array of LVITEMs
		// NEVER USE THE pszText MEMBER
		LPLVITEMW	lpItems;

		LPWSTR		lpItemNames;
	} resource;
} DESKTOP, * LPDESKTOP;

/* ------------------------------------ Desktop.c - */

// Initialize DESKTOP structrue
BOOL DesktopInit(LPDESKTOP pDesktop);

// Frees memory allocated for DESKTOP structure
BOOL DesktopFree(LPDESKTOP lpDesktop);

/* ---------------------------- ItemInteraction.c - */



/* -------------------------------------- Debug.c - */

VOID Debug_PrintDesktopAttributes(DESKTOP desktop);

VOID Debug_PrintMembersByIndex(DESKTOP desktop, INT index);

/* ------------------------------------------------ */

#endif
