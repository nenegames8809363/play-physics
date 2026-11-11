# Play Physics

Fork of [Editor Physics](https://github.com/pelmeshkagrom/editor-physics) (MIT, pelmeshkagrom). Runs Box2D during level playtest instead of the editor.

## Current behavior
- Every solid block becomes a dynamic box body when the level starts.
- Blocks at or below floor height are skipped, so the floor stays put.
- Player is not simulated.
- Positions and rotations are copied back into the GameObjects each frame.
- The world is rebuilt on reset and destroyed on quit.

## Known gaps
- `deactivateObject` is blocked for simulated solids, but the offscreen activation logic is unverified in play.
- PlayLayer binding names/signatures are unverified against your Geode version.
- Not compiled yet. Build via the GitHub Actions workflow or a local Geode SDK.

## License
MIT. See LICENSE. Original copyright (c) 2026 pelmeshkagrom retained.
