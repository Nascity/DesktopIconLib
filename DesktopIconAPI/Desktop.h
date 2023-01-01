#pragma once

#ifdef UNICODE

/* ------------------------------------------------ */

#include <Windows.h>
#include <CommCtrl.h>

/* ------------------------------------------------ */

typedef struct tagDESKTOP_MEMORY
{
	// Handle to the Desktop process
	HANDLE	hProcess;

	// Pointer to the memory allocated inside the Desktop process
	LPVOID	lpMemory;
} DESKTOP_MEMORY, * LPDESKTOP_MEMORY;

typedef struct tagDESKTOP
{
	// Handle to the Desktop ListView
	HWND		hwndListview;

	// Points to the memory for items allocated in the Desktop process
	LPLVITEMW	lpItems;

	// Item count
	DWORD		dwItemCount;

	// Internal use
	DESKTOP_MEMORY	internal_dm;
} DESKTOP, * LPDESKTOP;

/* ------------------------------------------------ */

// Initialize DESKTOP structrue
BOOL DesktopInit(LPDESKTOP pDesktop);

// Frees memory allocated for DESKTOP structure
BOOL DesktopFree(LPDESKTOP lpDesktop);

/* ------------------------------------------------ */

#endif
