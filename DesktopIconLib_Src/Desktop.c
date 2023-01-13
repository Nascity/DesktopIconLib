
/* ------------------------------------------------ */

#include "Desktop.h"

/* ------------------------------------------------ */

// Function to find hwnd to the Desktop ListView
// Returns NULL when fails
INTERNAL
HWND FindDesktopListViewHwnd(VOID)
{
	HWND	hDesktop = GetDesktopWindow();

	HWND	hShellDLL = 0;
	HWND	hWorkerW = 0;
	HWND	hListView = 0;

	// Finding handle to SHELLDLL_DefView
	if ((hShellDLL = FindWindowExW(FindWindowW(L"Progman", NULL), 0, L"SHELLDLL_DefView", NULL)) == 0)
	{
		do
		{
			hWorkerW = FindWindowExW(hDesktop, hWorkerW, L"WorkerW", NULL);
			hShellDLL = FindWindowExW(hWorkerW, 0, L"SHELLDLL_DefView", NULL);
		} while (!hShellDLL);
	}

	// Finding handle to the ListView and returns
	return FindWindowExW(hShellDLL, 0, L"SysListView32", NULL);
}

INTERNAL
HANDLE GetDesktopProcessHandle(LPDESKTOP lpDesktop)
{
	DWORD	dwPID;

	GetWindowThreadProcessId(lpDesktop->hwndListview, &dwPID);
	return lpDesktop->hProcessExplorer = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, dwPID);
}

INTERNAL
VOID RetrieveTrivialInformation(LPDESKTOP lpDesktop)
{
	CONST RECT	defaultRect = { 0, 0, 0, 0 };
	DWORD	dwItemSpacingTemp;

	// Retrieves item count
	lpDesktop->dwItemCount = ListView_GetItemCount(lpDesktop->hwndListview);

	// Retrieves horizontal and vertical spacing
	dwItemSpacingTemp = ListView_GetItemSpacing(lpDesktop->hwndListview, FALSE);
	lpDesktop->wHorizSpacing = LOWORD(dwItemSpacingTemp);
	lpDesktop->wVertiSpacing = HIWORD(dwItemSpacingTemp);
}

INTERNAL
BOOL CalculateMysteryNumber(LPDESKTOP lpDesktop)
{
	POINT temp;

	if (!GetItemPixelFromIndex(*lpDesktop, 0, &temp))
		return FALSE;

	lpDesktop->ptMysteryNumber.x = temp.x % lpDesktop->wHorizSpacing;
	lpDesktop->ptMysteryNumber.y = temp.y % lpDesktop->wVertiSpacing;
	return TRUE;
}

/* ------------------------------------------------ */

BOOL DesktopInit(LPDESKTOP lpDesktop)
{
	// Finds handle to the ListView
	if (!(lpDesktop->hwndListview = FindDesktopListViewHwnd()))
		return FALSE;

	// Fetches information
	// DO NOT MOVE THIS LINE
	RetrieveTrivialInformation(lpDesktop);

	// If the item count is 0, initialization fails
	if (!(lpDesktop->dwItemCount))
		return FALSE;

	// Retreives handle to the Desktop process
	// The handle has access rights of PROCESS_VM_READ and PROCESS_VM_WRITE
	if (!GetDesktopProcessHandle(lpDesktop))
		return FALSE;

	// The item snapshot is not valid until the user initializes it
	lpDesktop->bItemSnapshotValid = FALSE;

	// Calculates the mystery number
	CalculateMysteryNumber(lpDesktop);

	return TRUE;
}

BOOL DesktopRefresh(LPDESKTOP lpDesktop)
{
	return DesktopFree(lpDesktop)
		&& DesktopInit(lpDesktop);
}

BOOL DesktopFree(LPDESKTOP lpDesktop)
{
	return
		FreeItemSnapshot(lpDesktop) &&
		CloseHandle(lpDesktop->hProcessExplorer);
}