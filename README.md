# Desktop Icon Library
I am writing this library to make it easier to interact with icons on the Desktop.



## Usage
```c
#include "Desktop.h"
#pragma comment(lib, "insert lib name here")
```
To use the library, include `Desktop.h` and link the given `.lib` file.

WIP



## Example

<img src="https://github.com/NatsciT/DesktopIconLib/blob/libtest/Example/20230102/example.png" width="400"/>

[Example](https://github.com/NatsciT/DesktopIconLib/blob/05d1001287d92fcfe9e4bbd46c140b3240246f19/DesktopIconLib_ExecBuild/Main.c)



## Difficulties that I have encountered

- [ListView_GetItem macro](https://learn.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_getitem)

ListView_GetItem macro is used to obtain the LVITEM structure of a Listview specified by its handle. It takes a pointer that specifies the information to retrieve and receives information about the Listview item as one of its parameters. The problem was that explorer.exe cannot access the memory of the process that calls the macro.

I had to allocate memory on explorer.exe, write LVITEM structure containing the query to that memory, call the macro, read LVITEM structure from the memory, and free the memory. Not sure whether there is a cleverer way to get the job done, but this is the best I can think of right now.



## Licence
[GNU General Public License v3.0](/LICENCE)
