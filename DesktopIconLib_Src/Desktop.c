
/* ------------------------------------------------ */

#include "Desktop.h"

/* ------------------------------------------------ */

extern BOOL FillItem(LPDESKTOP lpDesktop);

extern BOOL AllocateIRS(LPIRS lpIRS, HANDLE hProcess, DWORD itemcount);
extern BOOL FreeIRS(LPIRS lpIRS, HANDLE hProcess);

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
	DWORD	dwItemSpacingTemp;

	// Retrieves item count
	lpDesktop->dwItemCount = ListView_GetItemCount(lpDesktop->hwndListview);

	// Retrieves horizontal and vertical spacing
	dwItemSpacingTemp = ListView_GetItemSpacing(lpDesktop->hwndListview, FALSE);
	lpDesktop->wHorizSpacing = LOWORD(dwItemSpacingTemp);
	lpDesktop->wVertiSpacing = HIWORD(dwItemSpacingTemp);
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
	
	// Allocates memory to lpItems
	if (!AllocateIRS(&(lpDesktop->resource), GetCurrentProcess(), lpDesktop->dwItemCount))
		return FALSE;

	// Retreives handle to the Desktop process
	// The handle has access rights of PROCESS_VM_READ and PROCESS_VM_WRITE
	if (!GetDesktopProcessHandle(lpDesktop))
		return FALSE;

	if (!FillItem(lpDesktop))
		return FALSE;

	return TRUE;
}

BOOL DesktopFree(LPDESKTOP lpDesktop)
{
	return
		FreeIRS(&(lpDesktop->resource), GetCurrentProcess()) &&
		CloseHandle(lpDesktop->hProcessExplorer);
}