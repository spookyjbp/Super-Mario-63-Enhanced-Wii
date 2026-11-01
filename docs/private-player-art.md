# Private player-art pipeline (v0.4.1)

New original PNGs are not redistributed in GitHub commits or Actions artifacts.
Use artwork you are entitled to use/export. No download, account or secret is
needed by the tools. The existing attributed sprite subset remains unchanged.

## Available source poses

From a local copy of the user-supplied Runouw source revision
`83e81fb6b80d11c4119e420d25298000a6e13f66`:

```sh
python -m pip install Pillow==11.3.0
python tools/import_source_poses.py /path/to/Super-Mario-63 private-art/player.art
```

Copy `private-art/player.art` beside `boot.dol`:
`SD:/apps/super-mario-63-enhanced-wii/player.art` (USB is also supported).
The app reads it once on startup. The private `.art` files are ignored by Git;
normal `make package` intentionally does not copy them into public packages.

This source-only conversion includes:

- Original grounded Down pose: `Mario (None) Dive.xml`, bitmap layer frame 2,
  `Mario Slide (None).png`, authored mirror and registration.
- `Mario Air (none) - 2` for ordinary ascent/takeoff and the shared double-jump pose;
  `Mario Air (none) - 1` for descent.
- Four original swimming poses, selected by the original Floating and Swimming Up
  clip velocity thresholds, with their authored bitmap registration.

This fast source-only conversion does not render Flash motion tweens. Use the
optional SWF exporter below to add the original bare triple jump and backflip.
Composite FLUDD equipment and animated water clips still require exports. Without a
private pose pack, the app uses a safe standing/air pose instead of the wrong
squashed landing sprite or arbitrary backflip/swim rotation. This fallback is
not a claim that the original animation has been restored.

## Optional original backflip/triple export

Install Java and obtain [JPEXS FFDec 26.4.0](https://github.com/jindrapetrik/jpexs-decompiler/releases/tag/version26.4.0).
The tested official `ffdec_26.4.0.zip` has SHA-256
`56ce960aeafc7caf034e2052ba49f878a883f7c7ca690da6eeffca9587b5e4b9`.
This is a local optional tool, not a Wii or CI dependency.

```sh
python tools/export_source_clips.py /path/to/Super-Mario-63 /path/to/ffdec.jar private-art/player.art
```

This includes the source poses above and renders all 22 bare backflip and 30
bare triple-jump frames from the pinned original SWF. It preserves registration
from the SVG export and source 32 Hz timing. The exporter rejects other SWF
revisions instead of guessing character IDs. Intermediate PNG/SVG files and a
manifest stay in ignored `private-art/rendered-source/`. The original checkout
is untouched. Copy the resulting `player.art` beside `boot.dol` as above.

FFDec's static export does not run ActionScript. The original WingCap=false
blank state is selected explicitly for the bare Mario exports. No conditional
FLUDD composite is presented as correctly rendered by this process. The 60-frame
pack has been generated and validated locally; Dolphin appearance still needs
testing. Public GitHub packages omit this private artwork.

## Rendered exports for remaining FLUDD clips

Export PNG frames from the original Flash/Animate project with transparent
backgrounds, **canonical facing LEFT** (original `_xscale=100`), 32 Hz timing,
and consistent original movie-clip registration. Include each rendered tween
frame rather than only keyframe PNG components. Preserve author timing and
composite FLUDD layers. Record each frame's top-left offset from the original
clip origin in **twips** (20 twips = 1 source pixel).

A manifest for `tools/build_player_art.py` looks like:

```json
{
  "canonical_facing": "left",
  "clips": [
    {
      "id": 5,
      "loop": false,
      "frames": [
        {"file": "backflip-00.png", "offset_twips": [-320, -320], "duration": 1}
      ]
    }
  ]
}
```

The example is structural, not a complete original animation. Frame files are
relative to the manifest. `duration` is an integer number of source 32 Hz frames.
Offsets must reflect the actual export, not the example numbers.

```sh
python tools/build_player_art.py private-art/export.json private-art/player.art
```

Clip IDs: 0 Down; 1 ascent; 2 descent; 3 double ascent; 4 triple; 5 backflip;
6 float/swim idle; 7 swim stroke; 8 land; 9 takeoff. Original single/double jumps
share the ordinary ascent clip; do not invent a distinct double somersault.
Fully equipped body variants use `16 + body_id*3 + nozzle` (H=0/R=1/T=2) and
`"equipped": true` to avoid drawing equipment twice. Equipment-only fallback
clips are 48/49/50. Water-effect clips are 51/52/53. These IDs are interfaces for
user exports, not evidence that correct original exports are already included.

The current loader/draw interface handles registration, animation durations,
canonical mirroring, player/camera attachment and suppression of Rocket water
before the boost. **Full source-accurate nozzle attachments across all actions,
nozzle-specific effect timing and equipped backflip/triple composite animation remain
incomplete until correct rendered clips and their state mappings are supplied
and compared to the original.** Generic export support does not fix issue 5 by
itself. No equipment artwork or distracting replacement squares are synthesized.

## Format and validation

`SM63ART1`, version 1, big endian: a 32-byte header, 8-byte clip records,
16-byte frame records, padding to a 32-byte boundary, then native tiled GX RGBA8.
Power-of-two texture dimensions are at most 1024; at most 64 clips / 256 frames.
The builder currently packs to 512x512. Frames include rectangle bounds, signed
twip offsets and source duration. The loader checks sizes, bounds, unique IDs,
frame ranges, durations, flags and offsets before creating the texture. Missing
or invalid files fall back safely. One aligned allocation happens at startup;
there are no per-frame texture allocations or file reads.

Tests use synthetic pixels and the actual parser, loader and draw command path;
no original artwork is needed on CI. These tests do not establish visual fidelity.
