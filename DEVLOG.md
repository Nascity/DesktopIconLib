## Difficulties that I have encountered

- [ListView_GetItem macro](https://learn.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-listview_getitem)

ListView_GetItem macro is used to obtain the LVITEM structure of a Listview specified by its handle. It takes a pointer that specifies the information to retrieve and receives information about the Listview item as one of its parameters. The problem was that explorer.exe cannot access the memory of the process that calls the macro.

I had to allocate memory on explorer.exe, write LVITEM structure containing the query to that memory, call the macro, read LVITEM structure from the memory, and free the memory. Not sure whether there is a cleverer way to get the job done, but this is the best I can think of right now.

- 'Mystery number'

![image](https://user-images.githubusercontent.com/97524957/211827755-f1291ad8-c4b2-459d-b58c-8794e87350fa.png)

When I tried to retrieve the position of an item on mth column and nth row, it always returned the coordinate with some number added to it ─ on my computer, (+21, +2). I still have no idea why this is happening, so I decided to name it 'Mystery number' and threw them into calculations whenever they were needed.
