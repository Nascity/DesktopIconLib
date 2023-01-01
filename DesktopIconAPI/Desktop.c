
/* ------------------------------------------------ */

#include <stdio.h>

#include <Windows.h>
#include <commctrl.h>

#include "Desktop.h"

/* ------------------------------------------------ */

// INTERNAL
// Returns NULL on failure
LPVOID AllocateMemoryToItemsArray(LPDESKTOP lpDesktop)
{
	return (lpDesktop->lpItems = VirtualAlloc(
		NULL,
		sizeof(LVITEMW) * lpDesktop->dwItemCount,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_EXECUTE_READWRITE
	));
}

/* ------------------------------------------------ */

// INTERNAL
// Function to find hwnd to the Desktop ListView
// Returns NULL when fails
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

// INTERNAL
HANDLE GetDesktopProcessHandle(LPDESKTOP lpDesktop)
{
	DWORD	dwPID;

	GetWindowThreadProcessId(lpDesktop->hwndListview, &dwPID);
	return lpDesktop->dkResource.hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, dwPID);
}

// INTERNAL
LPVOID AllocateMemoryInDesktopProcess(LPDESKTOP lpDesktop)
{
	return lpDesktop->dkResource.lpMemory = VirtualAllocEx(
		lpDesktop->dkResource.hProcess,
		NULL,
		sizeof(LVITEMW) * lpDesktop->dwItemCount,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_EXECUTE_READWRITE
	);
}

/* ------------------------------------------------ */

// INTERNAL
// Initializes LVITEM structure, copies it to the memory on the Desktop process,
// and uses ListView_GetItem macro to retrieve the item information
BOOL RequestItem(LPDESKTOP lpDesktop, INT iItemIndex)
{
	LVITEMW	temp = { 0, };

	temp.mask = LVIF_COLFMT | LVIF_COLUMNS | LVIF_GROUPID |
		LVIF_IMAGE | LVIF_INDENT | LVIF_PARAM |
		LVIF_STATE | LVIF_TEXT | LVIF_NORECOMPUTE;

	temp.iItem = iItemIndex;
	temp.iSubItem = 0;
	temp.stateMask = -1;

	WriteProcessMemory(
		lpDesktop->dkResource.hProcess,
		(LPLVITEMW)lpDesktop->dkResource.lpMemory + iItemIndex,
		&temp,
		sizeof(LVITEMW),
		NULL
	);

	return ListView_GetItem(lpDesktop->hwndListview, (LPLVITEMW)lpDesktop->dkResource.lpMemory + iItemIndex) &&
		ReadProcessMemory(
			lpDesktop->dkResource.hProcess,
			(LPLVITEMW)lpDesktop->dkResource.lpMemory,
			lpDesktop->lpItems,
			sizeof(LVITEMW),
			NULL
		);
}

/* ------------------------------------------------ */

// https://learn.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_getitem
// https://stackoverflow.com/questions/65910101/get-items-from-taskmanager-details-listview
BOOL DesktopInit(LPDESKTOP lpDesktop)
{
	DWORD				i;

	// Finds handle to the ListView
	if (!(lpDesktop->hwndListview = FindDesktopListViewHwnd()))
		return FALSE;

	// Retrieves item count
	lpDesktop->dwItemCount = ListView_GetItemCount(lpDesktop->hwndListview);

	// Allocates memory to lpItems
	if (!AllocateMemoryToItemsArray(lpDesktop))
		return FALSE;

	// Retreives handle to the Desktop process
	// The handle has access rights of PROCESS_VM_READ and PROCESS_VM_WRITE
	if (!GetDesktopProcessHandle(lpDesktop))
		return FALSE;

	// Allocates memory for items based on the item count
	if (!AllocateMemoryInDesktopProcess(lpDesktop))
		return FALSE;

	if (!RequestItem(lpDesktop, 0))
		return FALSE;

	return TRUE;
}

BOOL DesktopFree(LPDESKTOP lpDesktop)
{
	BOOL	ret = TRUE;
	
	ret &= VirtualFree(lpDesktop->lpItems, 0, MEM_RELEASE);
	ret &= VirtualFreeEx(lpDesktop->dkResource.hProcess, lpDesktop->dkResource.lpMemory, 0, MEM_RELEASE);
	ret &= CloseHandle(lpDesktop->dkResource.hProcess);

	return ret;
}