
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
				GetItemNameFromIndex(*lpDesktop, index),
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
POINT CellToPoint(DESKTOP desktop, CELL cell)
{
	POINT temp = { 0, 0 };

	temp.x = desktop.ptMysteryNumber.x + cell.column * desktop.wHorizSpacing;
	temp.y = desktop.ptMysteryNumber.y + cell.row * desktop.wVertiSpacing;

	return temp;
}

INTERNAL
CELL PointToCell(DESKTOP desktop, POINT point)
{
	CELL temp = { 0, 0 };

	temp.column = (WORD)((double)(point.x - desktop.ptMysteryNumber.x) / desktop.wHorizSpacing);
	temp.row = (WORD)((double)(point.y - desktop.ptMysteryNumber.y) / desktop.wVertiSpacing);

	return temp;
}

/* ------------------------------------------------ */

INT GetItemIndexFromText(DESKTOP desktop, LPCWSTR lpText, SIZE_T size)
{
	INT i;

	for (i = 0; i < (INT)desktop.dwItemCount; i++)
	{
		if (RtlCompareMemory(GetItemNameFromIndex(desktop, i), lpText, size) == size)
			return i;
	}

	return -1;
}

LPWSTR GetItemNameFromIndex(DESKTOP desktop, INT index)
{
	return desktop.resource.lpItemNames + MAX_PATH * index;
}

/* ------------------------------------------------ */

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