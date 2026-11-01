# Player art pack

The pack embedded in `boot.dol` is `assets/generated/player_art.bin` (owner
decision 2026-11-01: personal, non-distributed project; note the repository is
public). An SD/USB `player.art` beside `boot.dol` overrides it, so you can try a
re-export without rebuilding.

## Regenerating

Needs Java, Pillow 11.3.0, a checkout of Runouw/Super-Mario-63 at `83e81fb`
(the exporter verifies the SWF hash) and
[JPEXS FFDec 26.4.0](https://github.com/jindrapetrik/jpexs-decompiler/releases/tag/version26.4.0)
(`ffdec_26.4.0.zip` SHA-256 `56ce960aeafc7caf034e2052ba49f878a883f7c7ca690da6eeffca9587b5e4b9`).

```sh
python -m pip install Pillow==11.3.0
cd tools
python export_movement_clips.py /path/to/Super-Mario-63 /path/to/ffdec.jar ../private-art/player.art
cp ../private-art/player.art ../assets/generated/player_art.bin
```

About five minutes. Intermediate PNGs, patched SWFs, `manifest.json` and
`report.json` (label, sprite id and frame count of every clip) stay in ignored
`private-art/rendered-movement/`.

## Format (version 2)

`tools/build_player_art.py`: frames trimmed to their opaque bounds, identical
images stored once, one GX RGBA8 texture of at most 1024x1024. Clip ids:
`state*4 + variant` (variant 0 none, 1 Hover, 2 Rocket, 3 Turbo), FLUDD water
behind the body at 64+, in front at 128+ (`include/art_pack.h`). Offsets are
twips from Mario's feet, canonical facing left. Version 1 packs are rejected.
