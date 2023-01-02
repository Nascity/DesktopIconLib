#include <stdio.h>
#include <locale.h>

#include "Desktop.h"


int main(void)
{
	DESKTOP dt;
	INT i;

	_wsetlocale(LC_ALL, L"Korean");

	DesktopInit(&dt);

	for (i = 0; i < dt.dwItemCount; i++)
	{
		wprintf(L"---------------------------------------------------------------------\n");
		Debug_PrintMembersByIndex(&dt, i);
	}
	wprintf(L"---------------------------------------------------------------------\n");

	DesktopFree(&dt);
}