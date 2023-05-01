#include <stdio.h>

#include "Desktop.h"

int main(void)
{
	DESKTOP dt;

	if (DesktopInit(&dt))
	{
		if (TakeItemSnapshot(&dt))
		{
			
		}
		FreeItemSnapshot(&dt);
	}
	DesktopFree(&dt);

	return 0;
}