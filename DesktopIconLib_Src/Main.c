#include <stdio.h>
#include <locale.h>

#include "Desktop.h"



int main(void)
{
	DESKTOP dt;

	_wsetlocale(LC_ALL, L"Korean");

	if (DesktopInit(&dt))
	{
		ToggleItemSnapToGridState(&dt);

		ListView_SetItemPosition(dt.hwndListview, 0, 1000, 0);

		ToggleItemSnapToGridState(&dt);
	}
	DesktopFree(&dt);

	return 0;
}