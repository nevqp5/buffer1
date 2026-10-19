# Buffer Click (Geode mod)

Press the jump button up to N steps early (default 6 at 240 TPS) while the cube is in the air.
The press is held back and applied on the first step the cube is on the ground
(or at the deadline, so a click is never lost).

## Build
1. Install the Geode SDK + CLI (docs.geode-sdk.org/getting-started).
2. `geode build` in this folder (or `cmake` + build).
3. Copy the resulting `.geode` to `%LOCALAPPDATA%\GeometryDash\geode\mods\`
   or install it via Geode menu -> Mods -> "Install from file".
4. Remove: Geode menu -> Mods -> Buffer Click -> Uninstall.

## Notes
- Untested prototype. If a member name differs in your SDK version
  (m_isOnGround, m_isShip, processCommands ...), check the bindings in the SDK.
- Only the cube is buffered; other gamemodes pass clicks through unchanged.
- Orbs/pads are not detected, only landing on the ground.
