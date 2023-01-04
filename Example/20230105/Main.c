#include <stdio.h>
#include <locale.h>
#include <time.h>

#include "..\DesktopIconLib_Src\Desktop.h"
#pragma comment(lib, "DesktopIcon.lib")



int main(void)
{
	DESKTOP dt;
	LPPOINT	lppt;

	DWORD	i;

	_wsetlocale(LC_ALL, L"Korean");
	srand(clock());

	if (!DesktopInit(&dt))
		return -1;

	lppt = malloc(sizeof(POINT) * dt.dwItemCount);

	if (!lppt) return -1;

	for (i = 0; i < dt.dwItemCount; i++)
	{
		GetItemPositionFromIndex(dt, i, lppt + i);
	}

	for (i = 0; i < 50; i++)
	{
		MoveItemCcellFromIndex(dt, rand() % dt.dwItemCount, rand() % 4, rand() % 5);

		Sleep(250);
	}

	for (i = 0; i < dt.dwItemCount; i++)
	{
		SetItemPositionFromIndex(dt, i, lppt[i]);
	}

	free(lppt);

	DesktopFree(&dt);

	return 0;
}