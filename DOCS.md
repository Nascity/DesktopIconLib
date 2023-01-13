## Including the library in a project
```c
#include "Desktop.h"
#pragma comment(lib, "DesktopIcon.lib")
```
Include `Desktop.h` and link `DesktopIcon.lib`.

## Initializing `DESKTOP` struct
```c
DESKTOP	dt;

DesktopInit(&dt);
{ ... }
DesktopFree(&dt);
```
`DesktopInit` initializes the struct and `DesktopFree` frees the resources allocated for `DESKTOP`.

## Using functions
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
