#include <stdio.h>
#include <locale.h>

#include "Desktop.h"

#pragma comment(lib, "test.lib")



int wmain(void)
{
	DESKTOP	dt;
	DWORD	i;

	DesktopInit(&dt);

	for (i = 0; i < dt.dwItemCount; i++)
	{
		wprintf(L"--------------------------------------------------------\n");
		Debug_PrintMembersByIndex(dt, i);
	}
	wprintf(L"--------------------------------------------------------\n");

	DesktopFree(&dt);

	return 0;
}