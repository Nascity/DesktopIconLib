
/* ------------------------------------------------ */

#include <stdio.h>

#include "Desktop.h"

/* ------------------------------------------------ */



/* ------------------------------------------------ */

// INTERNAL
// Allocates memory for items array and their names
// Returns NULL on failure
LPVOID AllocateMemoryToItemArray(LPDESKTOP lpDesktop)
{
	return
		(lpDesktop->resource.lpItems
			= VirtualAlloc(
				NULL,
				sizeof(LVITEMW) * lpDesktop->dwItemCount,
				MEM_RESERVE | MEM_COMMIT,
				PAGE_READWRITE))
		&&
		(lpDesktop->resource.lpItemNames
			= VirtualAlloc(
				NULL,
				sizeof(WCHAR) * MAX_PATH * lpDesktop->dwItemCount,
				MEM_RELEASE | MEM_COMMIT,
				PAGE_READWRITE));
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
	return lpDesktop->resource.hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, dwPID);
}

/* ------------------------------------------------ */

// INTERNAL
// Retrieves the array of LVITEM structures at the point
// at which the function was called
BOOL RequestItems(LPDESKTOP lpDesktop)
{
	DWORD	i;
	LVITEMW	tempItem = { 0, };

	LPLVITEMW	lpTempItems = VirtualAllocEx(
		lpDesktop->resource.hProcess,
		NULL,
		sizeof(LVITEMW) * lpDesktop->dwItemCount,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_READWRITE
	);
	LPWSTR		lpTempNames = VirtualAllocEx(
		lpDesktop->resource.hProcess,
		NULL,
		sizeof(WCHAR) * MAX_PATH * lpDesktop->dwItemCount,
		MEM_RELEASE | MEM_COMMIT,
		PAGE_READWRITE
	);

	if (!lpTempItems || !lpTempNames)
		return FALSE;

	for (i = 0; i < lpDesktop->dwItemCount; i++)
	{
		tempItem.mask = LVIF_COLFMT | LVIF_COLUMNS | LVIF_GROUPID | LVIF_IMAGE | LVIF_INDENT | LVIF_PARAM | LVIF_STATE | LVIF_TEXT;
		tempItem.iItem = i;
		tempItem.iSubItem = 0;
		tempItem.stateMask = -1;

		tempItem.cchTextMax = MAX_PATH;
		tempItem.pszText = lpTempNames + MAX_PATH * i;

		if (!WriteProcessMemory(
			lpDesktop->resource.hProcess,
			lpTempItems + i,
			&tempItem,
			sizeof(LVITEMW),
			NULL
		))
			return FALSE;

		ListView_GetItem(lpDesktop->hwndListview, lpTempItems + i);
	}

	/*LVITEMW	temp = { 0, };

	LPLVITEMW	lpProcTempItem = VirtualAllocEx(
		lpDesktop->dkResource.hProcess,
		NULL,
		sizeof(LVITEMW),
		MEM_RESERVE | MEM_COMMIT,
		PAGE_READWRITE
	);
	LPWSTR		lpProcTempName = VirtualAllocEx(
		lpDesktop->dkResource.hProcess,
		NULL,
		sizeof(WCHAR) * MAX_PATH,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_READWRITE
	);
	LPWSTR		lpItemName = VirtualAlloc(
		NULL,
		sizeof(WCHAR) * MAX_PATH,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_READWRITE
	);

	if (!lpProcTempItem || !lpProcTempName || !lpItemName)
		return FALSE;

	temp.mask = LVIF_COLFMT | LVIF_COLUMNS | LVIF_GROUPID |
		LVIF_IMAGE | LVIF_INDENT | LVIF_PARAM |
		LVIF_STATE | LVIF_TEXT;
	temp.iItem = iItemIndex;
	temp.iSubItem = 0;
	temp.stateMask = -1;

	temp.cchTextMax = MAX_PATH;
	temp.pszText = lpProcTempName;

	if (WriteProcessMemory(
		lpDesktop->dkResource.hProcess,
		lpProcTempItem,
		&temp,
		sizeof(LVITEMW),
		NULL))
		return FALSE;

	return ListView_GetItem(lpDesktop->hwndListview, lpProcTempItem) &&
		ReadProcessMemory(
			lpDesktop->dkResource.hProcess,
			lpProcTempItem,
			lpDesktop->lpItems,
			sizeof(LVITEMW),
			NULL
		) &&
		ReadProcessMemory(
			lpDesktop->dkResource.hProcess,
			lpTempAtProcess,
			((LPLVITEMW)lpDesktop->dkResource.lpMemory)
		)*/
}

/* ------------------------------------------------ */

BOOL DesktopInit(LPDESKTOP lpDesktop)
{
	// Finds handle to the ListView
	if (!(lpDesktop->hwndListview = FindDesktopListViewHwnd()))
		return FALSE;

	// Retrieves item count
	lpDesktop->dwItemCount = ListView_GetItemCount(lpDesktop->hwndListview);

	// Allocates memory to lpItems
	if (!AllocateMemoryToItemArray(lpDesktop))
		return FALSE;

	// Retreives handle to the Desktop process
	// The handle has access rights of PROCESS_VM_READ and PROCESS_VM_WRITE
	if (!GetDesktopProcessHandle(lpDesktop))
		return FALSE;

	if (!RequestItems(lpDesktop))
		return FALSE;

	return TRUE;
}

BOOL DesktopFree(LPDESKTOP lpDesktop)
{
	return
		VirtualFree(lpDesktop->resource.lpItems, 0, MEM_RELEASE) &&
		VirtualFree(lpDesktop->resource.lpItemNames, 0, MEM_RELEASE) &&
		CloseHandle(lpDesktop->resource.hProcess);
}