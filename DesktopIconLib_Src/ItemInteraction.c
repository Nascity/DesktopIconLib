
/* ------------------------------------------------ */

#include "Desktop.h"

/* ------------------------------------------------ */

// Allocates memory for item(s) and their resources
// If allocating for DESKTOP, itemcount = lpDesktop->dwItemCount
// If allocating for explorer.exe, itemcount = 1
INTERNAL
BOOL AllocateISS(LPITEMSNAPSHOT lpISS, HANDLE hProcess, DWORD itemcount)
{
	// For item(s)
	if (lpISS->lpItems
		= VirtualAllocEx(
			hProcess,
			NULL,
			sizeof(LVITEMW) * itemcount,
			MEM_RESERVE | MEM_COMMIT,
			PAGE_READWRITE
		))
	{
		// For name(s)
		if (lpISS->lpItemNames
			= VirtualAllocEx(
				hProcess,
				NULL,
				sizeof(WCHAR) * MAX_PATH * itemcount,
				MEM_RESERVE | MEM_COMMIT,
				PAGE_READWRITE
			))
		{
			// For puColumns(es)
			if (lpISS->puColumnses
				= VirtualAllocEx(
					hProcess,
					NULL,
					sizeof(UINT) * itemcount,
					MEM_RESERVE | MEM_COMMIT,
					PAGE_READWRITE
				))
			{
				// For piColFmt(s)
				if (lpISS->piColFmts
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
				VirtualFreeEx(hProcess, lpISS->puColumnses, 0, MEM_RELEASE);
			}
			VirtualFreeEx(hProcess, lpISS->lpItemNames, 0, MEM_RELEASE);
		}
		VirtualFreeEx(hProcess, lpISS->lpItems, 0, MEM_RELEASE);
	}

	return FALSE;
}

// Frees memory for item(s) and their resources
INTERNAL
BOOL FreeISS(LPITEMSNAPSHOT lpISS, HANDLE hProcess)
{
	return
		VirtualFreeEx(hProcess, lpISS->lpItems, 0, MEM_RELEASE) &&
		VirtualFreeEx(hProcess, lpISS->lpItemNames, 0, MEM_RELEASE) &&
		VirtualFreeEx(hProcess, lpISS->puColumnses, 0, MEM_RELEASE) &&
		VirtualFreeEx(hProcess, lpISS->piColFmts, 0, MEM_RELEASE);
}

INTERNAL
BOOL SendMessageWithResource(DESKTOP desktop, UINT msg, WPARAM wParam, LPVOID res, SIZE_T size)
{
	LPVOID	mem;
	BOOL	ret = FALSE;

	if (!(mem = VirtualAllocEx(
		desktop.hProcessExplorer,
		NULL,
		size,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_READWRITE
	)))
		return FALSE;

	ret =
		WriteProcessMemory(
			desktop.hProcessExplorer,
			mem,
			res,
			size,
			NULL
		) &&
		SendMessage(
			desktop.hwndListview,
			msg,
			wParam,
			(LPARAM)mem
		) &&
		ReadProcessMemory(
			desktop.hProcessExplorer,
			mem,
			res,
			size,
			NULL
		);

	return VirtualFreeEx(
		desktop.hProcessExplorer,
		mem,
		0,
		MEM_RELEASE
	) && ret;
}

/* ------------------------------------------------ */

INTERNAL
BOOL GetItem(LPDESKTOP lpDesktop, DWORD index)
{
	LVITEMW			itemTemp = { 0, };
	ITEMSNAPSHOT	issTemp = { 0, };

	BOOL	ret = FALSE;

	// 1. Alloc mem
	if (!AllocateISS(&issTemp, lpDesktop->hProcessExplorer, 1))
		return FALSE;

	// 2. Write LVITEM
	itemTemp.mask
		= LVIF_COLFMT | LVIF_COLUMNS | LVIF_GROUPID | LVIF_IMAGE
		| LVIF_INDENT | LVIF_PARAM | LVIF_STATE | LVIF_TEXT;
	itemTemp.iItem = index;
	itemTemp.iSubItem = 0;
	itemTemp.stateMask = -1;

	itemTemp.cchTextMax = MAX_PATH;
	itemTemp.pszText = issTemp.lpItemNames;

	itemTemp.puColumns = issTemp.puColumnses;
	itemTemp.piColFmt = issTemp.piColFmts;

	// 3. Insert LVITEM
	if (issTemp.lpItemNames &&
		WriteProcessMemory(
			lpDesktop->hProcessExplorer,
			issTemp.lpItems,
			&itemTemp,
			sizeof(LVITEMW),
			NULL
		))
	{
		// 4. Call macro
		ListView_GetItem(lpDesktop->hwndListview, issTemp.lpItems);

		// 5. Retrieve LVITEM
		ret =
			ReadProcessMemory(
				lpDesktop->hProcessExplorer,
				issTemp.lpItems,
				lpDesktop->iss.lpItems + index,
				sizeof(LVITEMW),
				NULL) &&
			ReadProcessMemory(
				lpDesktop->hProcessExplorer,
				issTemp.lpItemNames,
				GetItemTextFromIndex(*lpDesktop, index),
				sizeof(WCHAR) * MAX_PATH,
				NULL) &&
			ReadProcessMemory(
				lpDesktop->hProcessExplorer,
				issTemp.puColumnses,
				lpDesktop->iss.puColumnses + index,
				sizeof(UINT),
				NULL) &&
			ReadProcessMemory(
				lpDesktop->hProcessExplorer,
				issTemp.piColFmts,
				lpDesktop->iss.piColFmts + index,
				sizeof(INT),
				NULL);
	}

	// 6. Free mem
	return FreeISS(&issTemp, lpDesktop->hProcessExplorer) && ret;
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
inline
POINT CellToPoint(DESKTOP desktop, CELL cell)
{
	POINT temp = { 0, 0 };

	temp.x = desktop.ptMysteryNumber.x + cell.column * desktop.wHorizSpacing;
	temp.y = desktop.ptMysteryNumber.y + cell.row * desktop.wVertiSpacing;

	return temp;
}

INTERNAL
inline
CELL PointToCell(DESKTOP desktop, POINT point)
{
	CELL temp = { 0, 0 };

	temp.column = (WORD)((double)(point.x - desktop.ptMysteryNumber.x) / desktop.wHorizSpacing);
	temp.row = (WORD)((double)(point.y - desktop.ptMysteryNumber.y) / desktop.wVertiSpacing);

	return temp;
}

/* ------------------------------------------------ */

BOOL TakeItemSnapshot(LPDESKTOP lpDesktop)
{
	// 1. Allocates memory for iss
	// 2. Uses FillItem function to fill in the struct
	// If there's already a snapshot, it only calls FillItem function
	return (
		lpDesktop->bItemSnapshotValid ||
		AllocateISS(&(lpDesktop->iss), GetCurrentProcess(), lpDesktop->dwItemCount)) &&
		FillItem(lpDesktop);
}

BOOL FreeItemSnapshot(LPDESKTOP lpDesktop)
{
	return lpDesktop->bItemSnapshotValid &&
		FreeISS(&(lpDesktop->iss), GetCurrentProcess());
}

/* ------------------------------------------------ */

INT GetItemIndexFromText(DESKTOP desktop, LPCWSTR lpText, SIZE_T size)
{
	INT i;

	if (desktop.bItemSnapshotValid)
	{
		for (i = 0; i < (INT)desktop.dwItemCount; i++)
		{
			if (RtlCompareMemory(GetItemTextFromIndex(desktop, i), lpText, size) == size)
				return i;
		}
	}

	return -1;
}

LPWSTR GetItemTextFromIndex(DESKTOP desktop, INT index)
{
	if (!desktop.bItemSnapshotValid) return L"No valid item snapshot";
	return desktop.iss.lpItemNames + MAX_PATH * index;
}

/* ------------------------------------------------ */

// Ultimately, this function is called by GetItem[...] functions
BOOL GetItemPixelFromIndex(DESKTOP desktop, INT index, LPPOINT lpPoint)
{
	return SendMessageWithResource(
		desktop,
		LVM_GETITEMPOSITION,
		(WPARAM)index,
		lpPoint,
		sizeof(POINT)
	);
}

BOOL GetItemPixelFromText(DESKTOP desktop, LPCWSTR lpText, SIZE_T size, LPPOINT lpPoint)
{
	INT index;

	if ((index = GetItemIndexFromText(desktop, lpText, size)) == -1)
		return FALSE;

	return GetItemPixelFromIndex(desktop, index, lpPoint);
}

BOOL GetItemCellFromIndex(DESKTOP desktop, INT index, LPCELL lpCell)
{
	POINT temp = { 0, 0 };

	if (!GetItemPixelFromIndex(desktop, index, &temp))
		return FALSE;

	*lpCell = PointToCell(desktop, temp);
	return TRUE;
}

BOOL GetItemCellFromText(DESKTOP desktop, LPCWSTR lpText, SIZE_T size, LPCELL lpCell)
{
	INT index;

	if ((index = GetItemIndexFromText(desktop, lpText, size)) == -1)
		return FALSE;

	return GetItemCellFromIndex(desktop, index, lpCell);
}

/* ------------------------------------------------ */

// Ultimately, this function is called by SetItem[...] and MoveItem[...] functions
BOOL SetItemPixelFromIndex(DESKTOP desktop, INT index, POINT point)
{
	return ListView_SetItemPosition(
		desktop.hwndListview,
		index,
		point.x,
		point.y
	);
}

BOOL SetItemPixelFromText(DESKTOP desktop, LPCWSTR lpText, SIZE_T size, POINT point)
{
	INT index;

	if ((index = GetItemIndexFromText(desktop, lpText, size)) == -1)
		return FALSE;

	return SetItemPixelFromIndex(desktop, index, point);
}

BOOL SetItemCellFromIndex(DESKTOP desktop, INT index, CELL cell)
{
	return SetItemPixelFromIndex(desktop, index, CellToPoint(desktop, cell));
}

BOOL SetItemCellFromText(DESKTOP desktop, LPCWSTR lpText, SIZE_T size, CELL cell)
{
	INT index;

	if ((index = GetItemIndexFromText(desktop, lpText, size)) == -1)
		return FALSE;

	return SetItemCellFromIndex(desktop, index, cell);
}

/* ------------------------------------------------ */

BOOL MoveItemCpixelFromIndex(DESKTOP desktop, INT index, DIRECTION direction, INT Cpixel)
{
	INT		horizontalMultiplier = 0;
	INT		verticalMultiplier = 0;
	POINT	ptCurrent;

	switch (direction)
	{
	case left:
		horizontalMultiplier = -1;
		verticalMultiplier = 0;
		break;
	case up:
		horizontalMultiplier = 0;
		verticalMultiplier = -1;
		break;
	case right:
		horizontalMultiplier = 1;
		verticalMultiplier = 0;
		break;
	case down:
		horizontalMultiplier = 0;
		verticalMultiplier = 1;
		break;
	}

	if (!GetItemPixelFromIndex(desktop, index, &ptCurrent))
		return FALSE;

	ptCurrent.x += Cpixel * horizontalMultiplier;
	ptCurrent.y += Cpixel * verticalMultiplier;

	return SetItemPixelFromIndex(desktop, index, ptCurrent);
}

BOOL MoveItemCpixelFromText(DESKTOP desktop, LPCWSTR lpText, SIZE_T size, DIRECTION direction, INT Cpixel)
{
	INT index;

	if ((index = GetItemIndexFromText(desktop, lpText, size)) == -1)
		return FALSE;

	return MoveItemCpixelFromIndex(desktop, index, direction, Cpixel);
}

BOOL MoveItemCcellFromIndex(DESKTOP desktop, INT index, DIRECTION direction, INT Ccell)
{
	if (direction == left || direction == right)
		return MoveItemCpixelFromIndex(desktop, index, direction, desktop.wHorizSpacing * Ccell);
	else
		return MoveItemCpixelFromIndex(desktop, index, direction, desktop.wVertiSpacing * Ccell);
}

BOOL MoveItemCcellFromText(DESKTOP desktop, LPCWSTR lpText, SIZE_T size, DIRECTION direction, INT Ccell)
{
	INT index;

	if ((index = GetItemIndexFromText(desktop, lpText, size)) == -1)
		return FALSE;

	return MoveItemCcellFromIndex(desktop, index, direction, Ccell);
}

/* ------------------------------------------------ */

