#include <stdio.h>
#include <locale.h>

#include "Desktop.h"



int main(void)
{
	DESKTOP dt;
	POINT pt;

	_wsetlocale(LC_ALL, L"Korean");

	if (DesktopInit(&dt))
	{
		if (GetItemPositionFromIndex(&dt, &pt, 0))
			wprintf(L"(%d, %d)", pt.x, pt.y);
	}
	DesktopFree(&dt);

	return 0;
}