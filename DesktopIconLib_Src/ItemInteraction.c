
/* ------------------------------------------------ */

#include "Desktop.h"

/* ------------------------------------------------ */

// Allocates memory for item(s) and their resources
// If allocating for DESKTOP, itemcount = lpDesktop->dwItemCount
// If allocating for explorer.exe, itemcount = 1
INTERNAL
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

// Frees memory for item(s) and their resources
INTERNAL
BOOL FreeIRS(LPIRS lpIRS, HANDLE hProcess)
{
	return
		VirtualFreeEx(hProcess, lpIRS->lpItems, 0, MEM_RELEASE) &&
		VirtualFreeEx(hProcess, lpIRS->lpItemNames, 0, MEM_RELEASE) &&
		VirtualFreeEx(hProcess, lpIRS->puColumnses, 0, MEM_RELEASE) &&
		VirtualFreeEx(hProcess, lpIRS->piColFmts, 0, MEM_RELEASE);
}

INTERNAL
BOOL SendMessageWithResource(LPDESKTOP lpDesktop, UINT msg, WPARAM wParam, LPVOID res, SIZE_T size)
{
	LPVOID	mem;
	BOOL	ret = FALSE;

	if (!(mem = VirtualAllocEx(
		lpDesktop->hProcessExplorer,
		NULL,
		size,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_READWRITE
	)))
		return FALSE;

	ret =
		WriteProcessMemory(
			lpDesktop->hProcessExplorer,
			mem,
			res,
			size,
			NULL
		) &&
		SendMessageW(
			lpDesktop->hwndListview,
			msg,
			wParam,
			(LPARAM)mem
		) &&
		ReadProcessMemory(
			lpDesktop->hProcessExplorer,
			mem,
			res,
			size,
			NULL
		);

	return VirtualFreeEx(
		lpDesktop->hProcessExplorer,
		mem,
		0,
		MEM_RELEASE
	) && ret;
}

/* ------------------------------------------------ */

INTERNAL
LPWSTR ItemNameFromIndex(LPDESKTOP lpDesktop, INT index)
{
	return lpDesktop->resource.lpItemNames + MAX_PATH * index;
}

/* ------------------------------------------------ */

INTERNAL
BOOL GetItem(LPDESKTOP lpDesktop, DWORD index)
{
	LVITEMW		itemTemp = { 0, };
	IRS			irsTemp = { 0, };

	BOOL	ret = FALSE;

	// 1. Alloc mem
	if (!AllocateIRS(&irsTemp, lpDesktop->hProcessExplorer, 1))
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
			lpDesktop->hProcessExplorer,
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
				lpDesktop->hProcessExplorer,
				irsTemp.lpItems,
				lpDesktop->resource.lpItems + index,
				sizeof(LVITEMW),
				NULL) &&
			ReadProcessMemory(
				lpDesktop->hProcessExplorer,
				irsTemp.lpItemNames,
				ItemNameFromIndex(lpDesktop, index),
				sizeof(WCHAR) * MAX_PATH,
				NULL) &&
			ReadProcessMemory(
				lpDesktop->hProcessExplorer,
				irsTemp.puColumnses,
				lpDesktop->resource.puColumnses + index,
				sizeof(UINT),
				NULL) &&
			ReadProcessMemory(
				lpDesktop->hProcessExplorer,
				irsTemp.piColFmts,
				lpDesktop->resource.piColFmts + index,
				sizeof(INT),
				NULL);
	}

	// 6. Free mem
	return FreeIRS(&irsTemp, lpDesktop->hProcessExplorer) && ret;
}

INTERNAL
NOT_IMPLEMENTED
BOOL SetItem(LPDESKTOP lpDesktop, LVITEMW item)
{
	// TODO: Implement

	return FALSE;
}

INTERNAL
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

INTERNAL
INT FindItemIndexFromText(LPDESKTOP lpDesktop, LPCWSTR lpText, SIZE_T size)
{
	INT i;

	for (i = 0; i < (INT)lpDesktop->dwItemCount; i++)
	{
		if (RtlCompareMemory(ItemNameFromIndex(lpDesktop, i), lpText, size) == size)
			return i;
	}

	return -1;
}

/* ------------------------------------------------ */

BOOL GetItemPositionFromIndex(LPDESKTOP lpDesktop, LPPOINT lpPoint, INT index)
{
	return SendMessageWithResource(
		lpDesktop,
		LVM_GETITEMPOSITION,
		(WPARAM)index,
		lpPoint,
		sizeof(POINT)
	);
}

BOOL GetItemPositionFromText(LPDESKTOP lpDesktop, LPPOINT lpPoint, LPCWSTR lpText, SIZE_T size)
{
	INT index;

	if ((index = FindItemIndexFromText(lpDesktop, lpText, size)) == -1)
		return FALSE;

	return GetItemPositionFromIndex(lpDesktop, lpPoint, index);
}