# Immutable level schema v1

`levels/movement.json` is the development fixture. It is **not** a recreation of
an original Story Mode level or a runtime level-editing feature.

Required fields:

| Field | Meaning |
| --- | --- |
| `schema_version` | Exactly 1 |
| `name` | Human-readable source label |
| `tile_size` | Integer 16..128 logical pixels; the lab uses 64 |
| `spawn` | `[x,y]` player top-left, clear of solid terrain |
| `tiles` | Equal-length strings, top-to-bottom rows, left-to-right columns |
| `backgrounds` | 1..4 parallax rectangles with `parallax`, `spacing`, `y`, `width`, `height`, RGB `color` |

Tile legend:

| Character | Type / interaction |
| --- | --- |
| `.` | Empty |
| `#` | Solid |
| `=` | One-way landing surface at the tile top; no side/ceiling collision |
| `/` | Solid triangle rising to the right |
| `\` | Solid triangle descending to the right |
| `~` | Non-solid water volume with swim/FLUDD refill |
| `I` | Solid ice, reduced acceleration/braking |
| `P` | Solid shallow-puddle material, intermediate friction and water refill |
| `!` | Solid lab hazard; contact respawns the player |

Generate and verify with standard Python, no extra packages:

```sh
python tools/build_level.py
python tools/build_level.py --check
python tests/test_level.py
```

The tool validates schema version, rectangular dimensions, tile IDs, spawn bounds
and clearance, and background dimensions. It emits checked-in
`assets/generated/movement_level.h`; normal Wii builds have no JSON parser or
external file-loading dependency. CI detects stale generated data. Future Story
Mode importers can translate source geometry into this schema or extend it under
a new version; original Flash `Platforms`/`BPlatforms`/water clips are not directly
interchangeable with this static grid.

Runtime data are immutable `Level`/`World` structures. Region queries walk only
visible/nearby cells and legacy rectangle fixtures. No collision lists, heap
allocations or tile mutations are made per frame. Slopes support the highest
point under the player's AABB footprint and snap only over nearby slope seams.
The renderer uses the same tile types and geometry as collision. Terrain uses
rectangle/triangle placeholders; background rectangles use configurable camera
parallax and viewport-limited repetition. Camera limits still use the logical
4:3/16:9 viewport width.
