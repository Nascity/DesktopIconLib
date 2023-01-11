#include <stdio.h>

#include "..\DesktopIconLib_Src\Desktop.h"
#pragma comment(lib, "DesktopIcon.lib")

int main(void)
{
	DESKTOP dt;

	if (DesktopInit(&dt))
	{

	}
	DesktopFree(&dt);

	return 0;
}