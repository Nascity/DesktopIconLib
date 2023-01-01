
/* ------------------------------------------------ */

#include <Windows.h>
#include <commctrl.h>

#include "Desktop.h"

/* ------------------------------------------------ */

// INTERNAL
// Function to find hwnd to the Desktop ListView
// Returns NULL when fails
HWND FindDesktopListViewHwnd()
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

// INTERNAL
HANDLE GetDesktopProcessHandle(LPDESKTOP lpDesktop)
{
	DWORD	dwPID;

	GetWindowThreadProcessId(lpDesktop->hwndListview, &dwPID);
	return lpDesktop->internal_dm.hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, dwPID);
}

// INTERNAL
LPVOID AllocateMemoryInDesktopProcess(LPDESKTOP lpDesktop)
{
	return lpDesktop->internal_dm.lpMemory = VirtualAllocEx(
		lpDesktop->internal_dm.hProcess,
		NULL,
		sizeof(LVITEMW) * lpDesktop->dwItemCount,
		MEM_COMMIT,
		PAGE_READWRITE
	);
}

BOOL TestFunction(LPDESKTOP lpItem)
{
	
}

/* ------------------------------------------------ */

// https://learn.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_getitem
// https://stackoverflow.com/questions/65910101/get-items-from-taskmanager-details-listview
BOOL DesktopInit(LPDESKTOP lpDesktop)
{
	INT				i;
	DESKTOP_MEMORY	dm;

	// Finds handle to the ListView
	if (!(lpDesktop->hwndListview = FindDesktopListViewHwnd()))
		return FALSE;

	// Retrieves item count
	lpDesktop->dwItemCount = ListView_GetItemCount(lpDesktop->hwndListview);

	// Retreives handle to the Desktop process
	// The handle has access rights of PROCESS_VM_READ and PROCESS_VM_WRITE
	if (!GetDesktopProcessHandle(lpDesktop))
		return FALSE;

	// Allocates memory for items based on the item count
	if (!(lpDesktop->lpItems = AllocateMemoryInDesktopProcess(lpDesktop, &dm)))
		return FALSE;

	if (!TestFunction(lpDesktop))
		return FALSE;





	//// Retrieves LVITEM structure for every item on the desktop
	//for (i = 0; i < lpDesktop->dwItemCount; i++)
	//{
	//	
	//}

	return TRUE;
}

BOOL DesktopFree(LPDESKTOP lpDesktop)
{
	BOOL	ret = VirtualFreeEx(lpDesktop->internal_dm.hProcess, lpDesktop->internal_dm.lpMemory, 0, MEM_RELEASE);

	CloseHandle(lpDesktop->internal_dm.hProcess);

	return ret;
}