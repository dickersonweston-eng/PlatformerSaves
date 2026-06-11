# Platformer Saves

Save and load your progress in platformer levels!

Based on the original mod by [Sabe](https://github.com/0x5abe), updated for GD 2.2081 with multi-slot save support.

## Features

Pick up right where you left off and enjoy taking breaks without having to leave Geometry Dash open!

- Checkpoint saving and loading for platformer levels
- Auto save at every checkpoint
- Up to 4 independent save slots per level

## Known bugs

- Incompatibility with xdBot. The option "Always Practice Fixes" should be disabled when using this mod. Recording or playing back macros with the mod installed may cause issues.
- Music and sound effects can occasionally get out of sync
- Player colors applied to objects may not update if the player changes their colors mid-session
- Practice Fix is not fully implemented (some player data is not saved, same as vanilla)

## Report a bug

Found something broken? [Open an issue](https://github.com/dickersonweston-eng/PlatformerSaves/issues) on GitHub.

## Dependencies

This mod requires the following to be installed:

- **[PersistenceAPI](https://github.com/0x5abe/PersistenceAPI)** by Sabe — handles the underlying save/load file system
- **[Node IDs](https://github.com/geode-sdk/NodeIDs)** by the Geode team — provides stable identifiers for GD's UI elements

Both are installed automatically by Geode.

## Credits

- [Sabe](https://github.com/0x5abe) — original PlatformerSaves mod
- The Geode team — for building the mod loader that made this possible

## Licensing

This mod is licensed under **GPL-3.0-only**, in keeping with the original mod's license. Source code is available on [GitHub](https://github.com/dickersonweston-eng/PlatformerSaves).
