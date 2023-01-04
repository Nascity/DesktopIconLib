#include <stdio.h>
#include <locale.h>

#include "Desktop.h"

extern BOOL TestFunc(DESKTOP desktop, LPPOINT lppt);

int main(void)
{
	DESKTOP dt;

	_wsetlocale(LC_ALL, L"Korean");

	if (DesktopInit(&dt))
	{
		// I was too lazy
	}
	DesktopFree(&dt);

	return 0;
}