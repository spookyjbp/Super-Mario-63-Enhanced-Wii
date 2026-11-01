# Original Mario sprite assets

Source: https://github.com/runouw/Super-Mario-63
Commit: `83e81fb6b80d11c4119e420d25298000a6e13f66`.

`source/*.png` are unchanged PNGs from the original game's
`Super_Mario_63_2010/LIBRARY/Char/BITS Mario` directory. `manifest.json` records
each original filename, path and SHA-256. It includes standing, eight walking
frames, rising/falling, spin and ground-pound/landing poses. No Level Designer
files or game scripts were imported.

Credits: original Super Mario 63 project by Runouw; Mario artwork/characters
belong to their original rights holders. The upstream README states:

> This game uses graphics, characters, and audio that are subject to copyright by Nintendo Company Ltd, HAL Lab, and Sora Ltd. And this game is not published nor endorsed by them. Super Mario 63 is a not-for-profit fangame based off of the various games published by Nintendo in the Mario franchise.

## Wii conversion

`tools/build_mario_atlas.py` packs the PNGs without changing their pixels into a
transparent 256x256 atlas, then emits native GX RGBA8 4x4 tiles. The atlas, binary
and frame header are checked in so Wii builds need no PNG decoder, Pillow or
asset downloads. The 256 KiB texture is embedded in `boot.dol` and initialized
once; it needs no SD-card asset folder or per-frame allocations.

To regenerate, install Pillow (`python -m pip install Pillow==11.3.0`), then run:

```sh
python tools/build_mario_atlas.py
python tools/build_mario_atlas.py --check
python tests/test_atlas.py
```

The renderer anchors every pose at the player's feet, draws at 1.5x size with
nearest-neighbor filtering, and flips UVs when facing left. Pound poses are
mirrored by default as in the original Flash clip. Walking/spin pose order
comes from the original clips; timings and foot anchors are adapted to the Wii
physics rather than a full recreation of the Flash animation timeline.
Terrain, sky and attack effects still use the test playground's placeholder
shapes. FLUDD, Luigi, enemies and Story Mode scenery have not been imported.

## Facing and spin keyframes

The atlas keeps the original pixels and does not pre-flip them. Native pose
facing and the default right-facing presentation mirror are explicit in
`manifest.json` and the generated header:

| Atlas frames | Animation | Native facing | Presentation |
| --- | --- | --- | --- |
| 0–8 | Standing / walking | Right | Mirror only for left-facing player |
| 9–10 | Rising / falling | Left | Mirror for right-facing player |
| 11 | Spin front | Front | Use the spin keyframe's mirror |
| 12–13 | Spin side poses | Left | Use the spin keyframe's mirror |
| 14 | Spin rear three-quarter | Back-left | Use the spin keyframe's mirror |
| 15 | Spin back | Back | Use the spin keyframe's mirror |
| 16–18 | Pound windup / descent | Right | Preserve the existing authored pound mirror |
| 19 | Landing | Right | Mirror only for left-facing player |

`source/render/player_sprite.c` selects the frame and one final UV mirror bit.
The original `Mario (none) Spin Attack.xml` uses atlas frames
`13,12,11,12,13,14,15,14` with mirror flags
`true,true,false,false,false,false,false,true`. These are per-keyframe values:
the same PNG is mirrored on one half of the turn and unmirrored on the other.
Front/back poses remain part of the turn rather than being forced into a
side-facing pose. The player's facing XOR the authored pose mirror is applied
once by GX; native-facing compensation is not applied a second time to spin.
Selection is read-only, so changing direction in the air is reflected on the
next render and a spin cannot change the gameplay facing.

`make -C tests test` includes five orientation regression groups alongside the
physics tests. These cover both directions, every spin phase, existing ground
poses, actual airborne input reversals and facing after a spin ends. They test
selection and mirroring calculations, not Dolphin's displayed result.

Dolphin verification is still required after this correction: jump left/right,
reverse in the air, spin from both directions and reverse during a spin, then
check the direction after landing. Also check standing, walking and pounds to
confirm their existing presentation is retained.
