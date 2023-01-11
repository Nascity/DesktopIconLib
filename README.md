# Desktop Icon Library (WIP)
I am writing this library to make it easier to interact with icons on the Desktop.



## Usage
```c
#include "Desktop.h"
#pragma comment(lib, "DesktopIcon.lib")
```
To use the library, include `Desktop.h` and link `DesktopIcon.lib`.

WIP



## Example

<img src="https://github.com/NatsciT/DesktopIconLib/blob/libtest/Example/20230102/example.png" width="400"/>

[Example](https://github.com/NatsciT/DesktopIconLib/blob/libtest/Example/20230102/Main.c)



## Difficulties that I have encountered

- [ListView_GetItem macro](https://learn.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_getitem)

ListView_GetItem macro is used to obtain the LVITEM structure of a Listview specified by its handle. It takes a pointer that specifies the information to retrieve and receives information about the Listview item as one of its parameters. The problem was that explorer.exe cannot access the memory of the process that calls the macro.

I had to allocate memory on explorer.exe, write LVITEM structure containing the query to that memory, call the macro, read LVITEM structure from the memory, and free the memory. Not sure whether there is a cleverer way to get the job done, but this is the best I can think of right now.



- 'Mystery number'

```c
#include <stdio.h>
#include "Desktop.h"

int main(void)
{
	DESKTOP dt;
	POINT	pt;

	if (DesktopInit(&dt))
	{
		GetItemPositionFromTextM(dt, L"foo", &pt);
		wprintf(L"foo: (%d, %d)\n", pt.x, pt.y);
		GetItemPositionFromTextM(dt, L"bar", &pt);
		wprintf(L"bar: (%d, %d)\n", pt.x, pt.y);
	}
	DesktopFree(&dt);

	return 0;
}
```



## Licence
[GNU General Public License v3.0](/LICENCE)
