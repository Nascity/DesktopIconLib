#include <stdio.h>

#include "Desktop.h"



int main(void)
{
	DESKTOP dt;

	printf("%d\n", DesktopInit(&dt));
	printf("%d\n", DesktopFree(&dt));
}