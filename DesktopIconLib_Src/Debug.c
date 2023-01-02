
/* ------------------------------------------------ */

#include <stdio.h>
#include "Desktop.h"

/* ------------------------------------------------ */

VOID Debug_PrintDesktopAttributes(DESKTOP desktop)
{
	wprintf(L"%-25s:%7d\n", L"Item count", desktop.dwItemCount);
	wprintf(L"%-25s:%7s\n", L"Snap to grid is on", desktop.bSnapToGrid ? L"TRUE" : L"FALSE");
}

VOID Debug_PrintMembersByIndex(DESKTOP desktop, INT index)
{
	LVITEMW	item = desktop.resource.lpItems[index];

	wprintf(L"%-8s%-12s%u\n", L"UINT", L"mask", item.mask);
	wprintf(L"%-8s%-12s%d\n", L"int", L"iItem", item.iItem);
	wprintf(L"%-8s%-12s%d\n", L"int", L"iSubItem", item.iSubItem);
	wprintf(L"%-8s%-12s%u\n", L"UINT", L"state", item.state);
	wprintf(L"%-8s%-12s%u\n", L"UINT", L"stateMask", item.stateMask);
	wprintf(L"%-8s%-12s%s\n", L"LPWSTR", L"pszText", desktop.resource.lpItemNames + MAX_PATH * index);
	wprintf(L"%-8s%-12s%d\n", L"int", L"cchTextMax", item.cchTextMax);
	wprintf(L"%-8s%-12s%d\n", L"int", L"iImage", item.iImage);
	wprintf(L"%-8s%-12s%lld\n", L"LPARAM", L"lParam", item.lParam);
	wprintf(L"%-8s%-12s%d\n", L"int", L"iIndent", item.iIndent);
	wprintf(L"%-8s%-12s%d\n", L"int", L"iGroupId", item.iGroupId);
	wprintf(L"%-8s%-12s%u\n", L"UINT", L"cColumns", item.cColumns);
	wprintf(L"%-8s%-12s%s\n", L"PUINT", L"puColumns", L"Not Implemented");
	wprintf(L"%-8s%-12s%s\n", L"int*", L"piColFmt", L"Not Implemented");
	wprintf(L"%-8s%-12s%d\n", L"int", L"iGroup", item.iGroup);
}

/* ------------------------------------------------ */