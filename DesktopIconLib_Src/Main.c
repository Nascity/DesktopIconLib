#include <stdio.h>
#include <locale.h>

#include "Desktop.h"

int main(void)
{
	DESKTOP dt;

	_wsetlocale(LC_ALL, L"Korean");

	if (DesktopInit(&dt))
	{
		MoveItemCcellFromIndex(dt, 0, right, 5);
	}
	DesktopFree(&dt);

	return 0;
}