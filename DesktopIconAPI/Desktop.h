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

	// Desktop resources which have to be released
	struct unnamed
	{
		// Handle to the Desktop process
		HANDLE		hProcess;

		// Array of LVITEMs
		// NEVER USE THE pszText MEMBER
		LPLVITEMW	lpItems;

		LPWSTR		lpItemNames;
	} resource;
} DESKTOP, * LPDESKTOP;

/* ------------------------------------------------ */

// Initialize DESKTOP structrue
BOOL DesktopInit(LPDESKTOP pDesktop);

// Frees memory allocated for DESKTOP structure
BOOL DesktopFree(LPDESKTOP lpDesktop);

/* ------------------------------------------------ */

#endif
