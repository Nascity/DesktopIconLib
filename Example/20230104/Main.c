#include <stdio.h>
#include <locale.h>

#include "..\DesktopIconLib_Src\Desktop.h"
#pragma comment(lib, "DesktopIcon.lib")



int main(void)
{
	DESKTOP dt;
	POINT	pt;
	DWORD	i;

	_wsetlocale(LC_ALL, L"Korean");

	if (!DesktopInit(&dt))
		return -1;

	for (i = 0; i < dt.dwItemCount; i++)
	{
		if (GetItemPositionFromIndex(&dt, &pt, i))
		{
			wprintf(L"------------------------------------------\n");
			wprintf(L"%d. %s (%d, %d)\n", i, GetItemNameFromIndex(&dt, i), pt.x, pt.y);
		}
	}
	wprintf(L"------------------------------------------\n");

	DesktopFree(&dt);

	return 0;
}