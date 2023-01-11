# Desktop Icon Library (Pre-release)
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
```
BOOL [Get/Set/Move]Item[Pixel/Cell/Cpixel/Ccell]From[Index/Text](DESKTOP desktop, ...);
```
- Get/Set/Move

Get: The function returns the position(in pixels or cells) of the item.<br>
Set: The function sets the position(in pixels or cells) of the item.<br>
Move: The function moves the item given amount of distance(in pixels or cells) in a given direction.

- Pixel/Cell/Cpixel/Ccell

WIP

- Index/Text

WIP

## Roadmap

- [ ] Add more interactions with the desktop itself
    - [ ] Set/Get background image
    - [ ] Find a way to toggle snap-to-grid
- [ ] Add more interactions with the icon itself
    - [ ] Add a feature to import the icon into .bmp file
    - [ ] Add a feature to change the icon from .bmp file

## Contributing

1. Fork this repo. `gh repo fork --clone NatsciT/DesktopIconLib`
2. Create new branch from [dev](https://github.com/NatsciT/DesktopIconLib/tree/dev) branch. `git checkout -b feature/something dev`
3. Commit your changes. `git commit -m "commit message"`
4. Push to the branch. `git push origin`
5. Open PR. `gh pr create -B origin/dev -t "pr title" -b "pr body"`

## Difficulties that I have encountered

- [ListView_GetItem macro](https://learn.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_getitem)

ListView_GetItem macro is used to obtain the LVITEM structure of a Listview specified by its handle. It takes a pointer that specifies the information to retrieve and receives information about the Listview item as one of its parameters. The problem was that explorer.exe cannot access the memory of the process that calls the macro.

I had to allocate memory on explorer.exe, write LVITEM structure containing the query to that memory, call the macro, read LVITEM structure from the memory, and free the memory. Not sure whether there is a cleverer way to get the job done, but this is the best I can think of right now.

- 'Mystery number'

![image](https://user-images.githubusercontent.com/97524957/211827755-f1291ad8-c4b2-459d-b58c-8794e87350fa.png)

When I tried to retrieve the position of an item on mth column and nth row, it always returned the coordinate with some number added to it ─ on my computer, (+21, +2). I still have no idea why this is happening, so I decided to name it 'Mystery number' and threw them into calculations whenever they were needed.

## Licence
[GNU General Public License v3.0](/LICENCE)
