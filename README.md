# Desktop Icon Library (Pre-release)
I am writing this library to make it easier to interact with icons on the Desktop.

## Usage

[Docs (WIP)](/DOCS.md)

## Devlog

[Devlog](/DEVLOG.md)

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

## Licence
[GNU General Public License v3.0](/LICENCE)

## Changelog

### 0.1.0

- Change the way how `DESKTOP` struct is initialized.
	- Renamed `INTERNALRESOURCESTRUCT` to `ITEMSNAPSHOT`.
	- Removed the part where `DesktopInit` function allocates memory for (what used to be) `INTERNALRESOURCESTRUCT`.<br>
The user is obligated to call `TakeItemSnapshot`.
- Added `DesktopInteraction.c`.

### 0.0.0

- Initial release.
