#include <stdio.h>
#include <locale.h>

#include "Desktop.h"



int main(void)
{
	DESKTOP dt;
	INT i;

	_wsetlocale(LC_ALL, L"Korean");

	printf("%d\n", DesktopInit(&dt));

	for (i = 0; i < dt.dwItemCount; i++)
	{
		wprintf(L"%s\n", dt.resource.lpItemNames + MAX_PATH * i);
	}

	printf("%d\n", DesktopFree(&dt));
}