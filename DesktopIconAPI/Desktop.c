
/* ------------------------------------------------ */

#include "Desktop.h"

/* ------------------------------------------------ */

// INTERNAL
// Allocates memory for items array and their names
BOOL AllocateMemoryToItemArray(LPDESKTOP lpDesktop)
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
				MEM_RESERVE | MEM_COMMIT,
				PAGE_READWRITE));
}

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

// INTERNAL
// Retrieves the array of LVITEM structures at the point
// at which the function was called
BOOL RequestItems(LPDESKTOP lpDesktop)
{
	DWORD	i;
	BOOL	ret = TRUE;
	LVITEMW	tempItem = { 0, };

	LPLVITEMW	lpTempItems;
	LPWSTR		lpTempName;

	if (!(lpTempItems = VirtualAllocEx(
		lpDesktop->resource.hProcess,
		NULL,
		sizeof(LVITEMW) * lpDesktop->dwItemCount,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_READWRITE
	)))
	{
		return FALSE;
	}
	else if (!(lpTempName = VirtualAllocEx(
		lpDesktop->resource.hProcess,
		NULL,
		sizeof(WCHAR) * MAX_PATH,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_READWRITE
	)))
	{
		VirtualFreeEx(
			lpDesktop->resource.hProcess,
			lpTempItems,
			0,
			MEM_RELEASE
		);

		return FALSE;
	}

	tempItem.mask = LVIF_COLFMT | LVIF_COLUMNS | LVIF_GROUPID | LVIF_IMAGE | LVIF_INDENT | LVIF_PARAM | LVIF_STATE | LVIF_TEXT;
	tempItem.iSubItem = 0;
	tempItem.stateMask = -1;
	tempItem.cchTextMax = MAX_PATH;
	tempItem.pszText = lpTempName;

	for (i = 0; i < lpDesktop->dwItemCount; i++)
	{
		tempItem.iItem = i;

		// Write a temporary LVITEM structure to the Desktop process
		if (!WriteProcessMemory(
			lpDesktop->resource.hProcess,
			lpTempItems + i,
			&tempItem,
			sizeof(LVITEMW),
			NULL
		))
		{
			ret = FALSE;
			break;
		}

		// Calls the macro
		ListView_GetItem(lpDesktop->hwndListview, lpTempItems + i);

		// Read item text and store on this process
		if (!ReadProcessMemory(
			lpDesktop->resource.hProcess,
			lpTempName,
			lpDesktop->resource.lpItemNames + MAX_PATH * i,
			sizeof(WCHAR) * MAX_PATH,
			NULL
		))
		{
			ret = FALSE;
			break;
		}
	}

	if (!ReadProcessMemory(
		lpDesktop->resource.hProcess,
		lpTempItems,
		lpDesktop->resource.lpItems,
		sizeof(LVITEMW) * lpDesktop->dwItemCount,
		NULL
	))
	{
		ret = FALSE;
	}

	return
		VirtualFreeEx(lpDesktop->resource.hProcess, lpTempItems, 0, MEM_RELEASE) &&
		VirtualFreeEx(lpDesktop->resource.hProcess, lpTempName, 0, MEM_RELEASE) &&
		ret;
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