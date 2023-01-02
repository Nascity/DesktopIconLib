
/* ------------------------------------------------ */

#include <stdio.h>

#include "Desktop.h"

/* ------------------------------------------------ */

// INTERNAL
// Debug method for printing all the members
VOID Debug_PrintMembersByIndex(LPDESKTOP lpDesktop, INT index)
{
	LVITEMW	item = lpDesktop->resource.lpItems[index];

	wprintf(L"%-8s%-12s%u\n", L"UINT", L"mask", item.mask);
	wprintf(L"%-8s%-12s%d\n", L"int", L"iItem", item.iItem);
	wprintf(L"%-8s%-12s%d\n", L"int", L"iSubItem", item.iSubItem);
	wprintf(L"%-8s%-12s%u\n", L"UINT", L"state", item.state);
	wprintf(L"%-8s%-12s%u\n", L"UINT", L"stateMask", item.stateMask);
	wprintf(L"%-8s%-12s%s\n", L"LPWSTR", L"pszText", lpDesktop->resource.lpItemNames + MAX_PATH * index);
	wprintf(L"%-8s%-12s%d\n", L"int", L"cchTextMax", item.cchTextMax);
	wprintf(L"%-8s%-12s%d\n", L"int", L"iImage", item.iImage);
	wprintf(L"%-8s%-12s%lld\n", L"LPARAM", L"lParam", item.lParam);
	wprintf(L"%-8s%-12s%d\n", L"int", L"iIndent", item.iIndent);
	wprintf(L"%-8s%-12s%d\n", L"int", L"iGroupId", item.iGroupId);
	wprintf(L"%-8s%-12s%u\n", L"UINT", L"cColumns", item.cColumns);
}

/* ------------------------------------------------ */



/* ------------------------------------------------ */