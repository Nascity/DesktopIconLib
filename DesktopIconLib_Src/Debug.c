
/* ------------------------------------------------ */

#include "Desktop.h"
#include <stdio.h>

/* ------------------------------------------------ */

DWORD Debug_LastError;

/* ------------------------------------------------ */

VOID Debug_PrintMembersByIndex(DESKTOP desktop, INT index)
{
	LVITEMW	item = desktop.iss.lpItems[index];

	wprintf(L"%-8s%-12s%u\n", L"UINT", L"mask", item.mask);
	wprintf(L"%-8s%-12s%d\n", L"int", L"iItem", item.iItem);
	wprintf(L"%-8s%-12s%d\n", L"int", L"iSubItem", item.iSubItem);
	wprintf(L"%-8s%-12s%u\n", L"UINT", L"state", item.state);
	wprintf(L"%-8s%-12s%u\n", L"UINT", L"stateMask", item.stateMask);
	wprintf(L"%-8s%-12s%s\n", L"LPWSTR", L"pszText", GetItemTextFromIndex(desktop, index));
	wprintf(L"%-8s%-12s%d\n", L"int", L"cchTextMax", item.cchTextMax);
	wprintf(L"%-8s%-12s%d\n", L"int", L"iImage", item.iImage);
	wprintf(L"%-8s%-12s%lld\n", L"LPARAM", L"lParam", item.lParam);
	wprintf(L"%-8s%-12s%d\n", L"int", L"iIndent", item.iIndent);
	wprintf(L"%-8s%-12s%d\n", L"int", L"iGroupId", item.iGroupId);
	wprintf(L"%-8s%-12s%u\n", L"UINT", L"cColumns", item.cColumns);
	wprintf(L"%-8s%-12s%u\n", L"PUINT", L"puColumns", desktop.iss.puColumnses[index]);
	wprintf(L"%-8s%-12s%d\n", L"int*", L"piColFmt", desktop.iss.piColFmts[index]);
	wprintf(L"%-8s%-12s%d\n", L"int", L"iGroup", item.iGroup);
}

VOID Debug_PrintInteger(INT integer)
{
	wprintf(L"%d\n", integer);
}

DWORD Debug_GetLastError(VOID)
{
	return Debug_LastError;
}

/* ------------------------------------------------ */