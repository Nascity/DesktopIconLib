
/* ------------------------------------------------ */

#include <stdio.h>

#include "Desktop.h"

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
	return lpDesktop->resource.hProcessExplorer = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, dwPID);
}

// INTERNAL
VOID RetrieveTrivialInformation(LPDESKTOP lpDesktop)
{
	// Retrieves item count
	lpDesktop->dwItemCount = ListView_GetItemCount(lpDesktop->hwndListview);

	// Query snap-to-grid
	lpDesktop->bSnapToGrid = ListView_GetExtendedListViewStyle(lpDesktop->hwndListview) && LVS_EX_SNAPTOGRID;
}

/* ------------------------------------------------ */

// INTERNAL
// Allocates memory for item(s) and their resources
// If allocating for DESKTOP, itemcount = lpDesktop->dwItemCount
// If allocating for explorer.exe, itemcount = 1
BOOL AllocateIRS(LPIRS lpIRS, HANDLE hProcess, DWORD itemcount)
{
	// For item(s)
	if (lpIRS->lpItems
		= VirtualAllocEx(
			hProcess,
			NULL,
			sizeof(LVITEMW) * itemcount,
			MEM_RESERVE | MEM_COMMIT,
			PAGE_READWRITE
		))
	{
		// For name(s)
		if (lpIRS->lpItemNames
			= VirtualAllocEx(
				hProcess,
				NULL,
				sizeof(WCHAR) * MAX_PATH * itemcount,
				MEM_RESERVE | MEM_COMMIT,
				PAGE_READWRITE
			))
		{
			// For puColumns(es)
			if (lpIRS->puColumnses
				= VirtualAllocEx(
					hProcess,
					NULL,
					sizeof(UINT) * itemcount,
					MEM_RESERVE | MEM_COMMIT,
					PAGE_READWRITE
				))
			{
				// For piColFmt(s)
				if (lpIRS->piColFmts
					= VirtualAllocEx(
						hProcess,
						NULL,
						sizeof(INT) * itemcount,
						MEM_RESERVE | MEM_COMMIT,
						PAGE_READWRITE
					))
				{
					return TRUE;
				}
				VirtualFreeEx(hProcess, lpIRS->puColumnses, 0, MEM_RELEASE);
			}
			VirtualFreeEx(hProcess, lpIRS->lpItemNames, 0, MEM_RELEASE);
		}
		VirtualFreeEx(hProcess, lpIRS->lpItems, 0, MEM_RELEASE);
	}

	return FALSE;
}

// INTERNAL
// Frees memory for item(s) and their resources
BOOL FreeIRS(LPIRS lpIRS, HANDLE hProcess)
{
	return
		VirtualFreeEx(hProcess, lpIRS->lpItems, 0, MEM_RELEASE) &&
		VirtualFreeEx(hProcess, lpIRS->lpItemNames, 0, MEM_RELEASE) &&
		VirtualFreeEx(hProcess, lpIRS->puColumnses, 0, MEM_RELEASE) &&
		VirtualFreeEx(hProcess, lpIRS->piColFmts, 0, MEM_RELEASE);
}

/* ------------------------------------------------ */

// INTERNAL
BOOL GetItem(LPDESKTOP lpDesktop, DWORD index)
{
	LVITEMW		itemTemp = { 0, };
	IRS			irsTemp = { 0, };

	BOOL	ret = FALSE;

	// 1. Alloc mem
	if (!AllocateIRS(&irsTemp, lpDesktop->resource.hProcessExplorer, 1))
		return FALSE;

	// 2. Write LVITEM
	itemTemp.mask
		= LVIF_COLFMT | LVIF_COLUMNS | LVIF_GROUPID | LVIF_IMAGE
		| LVIF_INDENT | LVIF_PARAM | LVIF_STATE | LVIF_TEXT;
	itemTemp.iItem = index;
	itemTemp.iSubItem = 0;
	itemTemp.stateMask = -1;

	itemTemp.cchTextMax = MAX_PATH;
	itemTemp.pszText = irsTemp.lpItemNames;

	itemTemp.puColumns = irsTemp.puColumnses;
	itemTemp.piColFmt = irsTemp.piColFmts;

	// 3. Insert LVITEM
	if (irsTemp.lpItemNames &&
		WriteProcessMemory(
			lpDesktop->resource.hProcessExplorer,
			irsTemp.lpItems,
			&itemTemp,
			sizeof(LVITEMW),
			NULL
		))
	{
		// 4. Call macro
		ListView_GetItem(lpDesktop->hwndListview, irsTemp.lpItems);

		// 5. Retrieve LVITEM
		ret =
			ReadProcessMemory(
				lpDesktop->resource.hProcessExplorer,
				irsTemp.lpItems,
				lpDesktop->resource.lpItems + index,
				sizeof(LVITEMW),
				NULL) &&
			ReadProcessMemory(
				lpDesktop->resource.hProcessExplorer,
				irsTemp.lpItemNames,
				lpDesktop->resource.lpItemNames + index * MAX_PATH,
				sizeof(WCHAR) * MAX_PATH,
				NULL) &&
			ReadProcessMemory(
				lpDesktop->resource.hProcessExplorer,
				irsTemp.puColumnses,
				lpDesktop->resource.puColumnses + index,
				sizeof(UINT),
				NULL) &&
			ReadProcessMemory(
				lpDesktop->resource.hProcessExplorer,
				irsTemp.piColFmts,
				lpDesktop->resource.piColFmts + index,
				sizeof(INT),
				NULL);
	}

	// 6. Free mem
	return FreeIRS(&irsTemp, lpDesktop->resource.hProcessExplorer) && ret;
}

// INTERNAL
BOOL FillItem(LPDESKTOP lpDesktop)
{
	DWORD	i;

	for (i = 0; i < lpDesktop->dwItemCount; i++)
	{
		if (!GetItem(lpDesktop, i))
			return FALSE;
	}

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
		CloseHandle(lpDesktop->resource.hProcessExplorer);
}