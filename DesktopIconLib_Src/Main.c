#include <stdio.h>
#include <locale.h>

#include "Desktop.h"



int main(void)
{
	DESKTOP dt;
	DWORD	i;

	_wsetlocale(LC_ALL, L"Korean");

	if (DesktopInit(&dt))
	{
		wprintf(L"-------------------------------------------------------\n");
		Debug_PrintDesktopAttributes(dt);
		wprintf(L"-------------------------------------------------------\n");
		for (i = 0; i < dt.dwItemCount; i++)
		{
			Debug_PrintMembersByIndex(dt, i);
			wprintf(L"-------------------------------------------------------\n");
		}
	}
	DesktopFree(&dt);

	return 0;
}