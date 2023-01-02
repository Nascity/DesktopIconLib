#include <stdio.h>
#include <locale.h>

#include "Desktop.h"



int main(void)
{
	DESKTOP dt;

	_wsetlocale(LC_ALL, L"Korean");

	DesktopInit(&dt);

	Debug_PrintDesktopAttributes(dt);

	DesktopFree(&dt);
}