# Desktop Icon Library (WIP)
I am writing this library to make it easier to interact with icons on the Desktop.

## Usage

### Including the library in a project
```c
#include "Desktop.h"
#pragma comment(lib, "DesktopIcon.lib")
```
Include `Desktop.h` and link `DesktopIcon.lib`.

### Initializing `DESKTOP` struct
```c
DESKTOP	dt;

DesktopInit(&dt);
{ ... }
DesktopFree(&dt);
```
`DesktopInit` initializes the struct and `DesktopFree` frees the resources allocated for `DESKTOP`.

### Using functions
As of writing this, most of the functions defined in `ItemInteraction.c` has the following structure:
```ansi
BOOL [Get/Set/Move]Item[Pixel/Cell/Cpixel/Ccell]From[Index/Text](DESKTOP desktop, ...);
```
- Get/Set/Move

Get: The function returns the position(in pixels or cells) of the item<br>
Set: The function sets the position(in pixels or cells) of the item<br>
Move: The function moves the item given amount of distance(in pixels or cells) in a given direction.

- Pixel/Cell/Cpixel/Ccell

WIP

- Index/Text

WIP

## Roadmap

- [ ] Add more interactions with the desktop itself. (such as change the background image)
- [ ] Find a way to toggle snap-to-grid

## Contributing

To be honest, I haven't tested the command lines below lol

1. Fork this repo
2. Create new branch from [dev](https://github.com/NatsciT/DesktopIconLib/tree/dev) branch. Be sure to name the branch `dev/something`.
3. Commit your changes.
4. Push to the branch.
5. Open PR.

## Difficulties that I have encountered

- [ListView_GetItem macro](https://learn.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_getitem)

ListView_GetItem macro is used to obtain the LVITEM structure of a Listview specified by its handle. It takes a pointer that specifies the information to retrieve and receives information about the Listview item as one of its parameters. The problem was that explorer.exe cannot access the memory of the process that calls the macro.

I had to allocate memory on explorer.exe, write LVITEM structure containing the query to that memory, call the macro, read LVITEM structure from the memory, and free the memory. Not sure whether there is a cleverer way to get the job done, but this is the best I can think of right now.

- 'Mystery number'

WIP

## Licence
[GNU General Public License v3.0](/LICENCE)
