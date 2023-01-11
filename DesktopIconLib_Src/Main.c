#include <stdio.h>

#include "Desktop.h"

int main(void)
{
	DESKTOP dt;

	if (DesktopInit(&dt))
	{
		
	}
	DesktopFree(&dt);

	return 0;
}