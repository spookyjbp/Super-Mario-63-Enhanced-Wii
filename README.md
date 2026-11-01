# Super Mario 63: Enhanced Wii Edition

**v0.4.1 — Controls, Physics and Animation Corrections** starts from v0.4
(`978eda7`). This is a staged, tested correction set, **not all six reported
issues fully completed**. See the [per-issue source references and status](docs/v0.4.1-corrections.md).

- Corrected: repeated spin rules, source spin decay/lift/drag, swimming strokes,
  resistance and entry/surface rules, backflip control/facing duration, and
  ordinary jump ascent/descent/landing pose mapping.
- Correct original grounded Down, four swim poses, and complete bare backflip /
  triple-jump frames are available through the optional
  [private artwork pipeline](docs/private-player-art.md). Without that pack,
  safe upright poses remain rather than the wrong squashed/rotated replacements.
- Incomplete: authentic composite FLUDD equipment/water visuals, including
  nozzle-specific body variants. Correct conditional exports and further
  state/timing work are still required. The generic loader interface does not
  complete FLUDD rendering.

The ordinary single/double jumps share the source ascent pose; a new double flip
has not been invented. The collision, terrain, camera, level data, 60 Hz update
loop and original spin orientation fixes are preserved. No distracting particle
squares, Level Designer or unrelated Story Mode systems are added.

The original 32 Hz swim/spin recurrences run inside 60 Hz gameplay; collision and
position integration remain adaptations. Source equations and automated tests
are **not proof of accuracy against the original game**. No v0.4.1 Dolphin or
Wii playtest or side-by-side Flash comparison has been performed here.

## v0.4 foundation retained

- Low dive/slide, a source-confirmed backflip trigger, dive recovery roll,
  double-tap run, material friction and slope momentum.
- Swimming, vertical strokes, underwater spin, surface exit and water refill.
- Hover thrust, charge/cancel/launch Rocket and ground/air/underwater Turbo,
  including water/power limits and nozzle switching.
- Original-style high-speed wall bonk/rebound. Conventional manual wall jumping
  and a separate side somersault were not confirmed and have not been invented.
- An immutable tile level with solid/one-way platforms, 45-degree slopes, water,
  ice, shallow-puddle material, lab hazards and parallax backgrounds.
- Explicit animation states. The v0.4.1 patch removes misleading generic
  backflip/swim rotations; source-pose packs and full rendered exports are
  described above. Equipped variants and FLUDD artwork remain unfinished.
  Terrain/backgrounds use placeholders because redistribution
  permission for new original environment art has not been established.

Existing acceleration, variable-height jumping, double/triple jumps, spin, pound,
pause/restart/exit and camera following remain. Triple jumps now require momentum
as in the original source. Normal jump speeds and comfort windows remain port
tuning, not an exact original-physics claim. All three nozzles are enabled in the
lab; Story Mode unlocks, health/oxygen, enemies, moving platforms and scripts are
outside this milestone. There is no Level Designer.

## Controls

Use Wii Remote channel 1 or GameCube port 1. Analog sticks and D-pads work;
vertical analog movement uses a dead zone.

| Action | Wii Remote + Nunchuk | Classic Controller | GameCube |
| --- | --- | --- | --- |
| Move / swim vertically | Nunchuk stick or upright Remote D-pad | Left stick / D-pad | Stick / D-pad |
| Jump / rise in water | A (hold for higher jump) | A | A |
| Spin / hold to repeat when eligible | B or Nunchuk C | X | B |
| Ground pound in air | Nunchuk Z | ZR | Z |
| Low dive / slide / descend in water | Stick down or D-pad Down | Stick down or D-pad Down | Stick down or D-pad Down |
| Activate selected FLUDD | Hold Remote 1 | Hold Y | Hold R |
| Cycle Hover → Rocket → Turbo | Remote 2 | ZL | L |
| Pause / resume | PLUS or HOME | PLUS or HOME | START |
| Menu select / confirm / cancel | D-pad / A / B | D-pad / A / B | D-pad / A / B |

Swimming: A or Up starts an original-style stroke, holding sustains it, and
release after the minimum five source frames permits another stroke. Down
descends. Spin cannot start during a fast ascent (source vertical speed <= -3),
and pressing repeatedly does not reset an active spin. Holding it can start
another when the attack ends and the source restrictions allow it.

Backflip (source: `Mario (None) Dive.xml`): crouch with Down while standing or
moving slowly, then hold the direction **opposite** the way Mario faces and press
Jump. Down + Jump alone only stands Mario back up, exactly as in the original.
Jump may be held before or after pushing away, and the flip is still available
for the ~0.2 s recovery after releasing Down. Jump from a fast forward slide to
recover with a roll. Without the private art pack the crouch shows the standing
fallback pose; see `docs/v0.4.1-down-controls.md`. For running,
double-tap left or right within 9/32 s. Chain jumps within 0.28 s of landing;
the third requires horizontal speed above 132 px/s. Hold FLUDD to Hover or Turbo;
hold Rocket through its charge to launch, release early to cancel. The HUD shows
H/R/T nozzle, water W and power P. Water refills in the pool and on puddle tiles;
power recovers when grounded, in water, or while releasing the nozzle. Nozzle
switching is allowed while neutral. RESET exits safely; power callbacks retain
shutdown behavior. Remote-only fallback is also available through the D-pad,
A/B, 1/2 and Down-to-pound fallback, but Nunchuk is the intended Wii control setup.

## Build and install

Install official [devkitPro](https://devkitpro.org/wiki/Getting_Started) `wii-dev`:

```sh
export DEVKITPRO=/opt/devkitpro
export DEVKITPPC="$DEVKITPRO/devkitPPC"
export PATH="$DEVKITPRO/tools/bin:$DEVKITPPC/bin:$PATH"
make -j2 package
```

Or use the same pinned official container as CI:

```sh
docker run --rm -v "$PWD:/project" -w /project \
  devkitpro/devkitppc@sha256:d50f8a886f2ce1d3c22685a774afb0845379876b47d9b41a9b729f4f63a54813 \
  make -j2 package
```

Outputs are `boot.dol`, debug `boot.elf` / `boot.elf.map`, and
`dist/apps/super-mario-63-enhanced-wii/`. Copy the `apps` folder to an SD card or
supported USB device and launch through the installed Homebrew Channel.
Original Mario movement art (every movement state in all four FLUDD states,
plus the FLUDD water blasts) is embedded in `boot.dol` from
`assets/generated/player_art.bin`, built by `tools/export_movement_clips.py`.
An SD/USB `player.art` next to `boot.dol` overrides it. See
[movement animation](docs/movement-animation.md) and the
[export instructions](docs/private-player-art.md).
`make clean` removes generated build/package outputs. C11/C++17 modules are
supported; source basenames must be unique across listed source directories.

The environment lab has a platform near spawn, a hill with both slope directions,
a pool farther right, a tall wall, ice/puddle strips and a red respawn tile.
Use the pause menu to restart. Original Story Mode levels are not included.

## Architecture and data

- `source/game/player.c`: existing movement, action integration, animation choice.
- `source/game/player_special.c`: dive/backflip/FLUDD adapters.
- `source/game/source_movement.c`: original-frame swim/spin recurrence logic.
- `source/assets/art_pack.c` / `source/platform/user_art.c`: validated private
  texture/clip loader and user-asset rendering.
- `source/game/collision.c` / `world_query.c`: solid, one-way, slope and water queries.
- `source/game/game.c` / `camera.c`: pause/restart/respawn, input latching, camera.
- `source/platform/input.c`: device mappings; `graphics.c`: GX primitives;
  `render.c`: viewport-culling, parallax, terrain and player rendering.
- `source/render/player_sprite.c`: original pose selection/mirroring and temporary
  new-action transformations. Original jump/spin orientation regressions retained.
- `levels/movement.json`: [versioned level schema](docs/level-format.md), compiled by
  `tools/build_level.py` into checked-in `assets/generated/movement_level.h`.
- `assets/mario`: [original sprite subset and provenance](assets/mario/README.md).
  `tools/build_mario_atlas.py` regenerates GX texture data using Pillow 11.3.0.

Simulation remains fixed at 60 Hz with bounded catch-up and render interpolation.
PAL 50 Hz rendering still runs 60 Hz simulation. Camera and presentation preserve
4:3/16:9 system settings. No gameplay heap allocations are added. Immutable data
and textures survive packaging; runtime services and external asset files are not
needed. Actual Wii frame time and visual/controller behavior remain unmeasured.

## Tests and GitHub Actions

```sh
make -C tests test
python tools/build_level.py --check
python tests/test_level.py
# For optional atlas verification, install Pillow==11.3.0:
python tools/build_mario_atlas.py --check
python tests/test_atlas.py
python tests/test_player_art.py
```

Tests compile with warnings as errors. They run the actual game/presentation C
sources on the host and record renderer commands without emulating GX. CI runs
physics, orientation, movement/environment, renderer and asset/level checks before
the Wii build, then uploads the Homebrew Channel application artifact.
The pinned toolchain compiled v0.4.1 and produced `boot.dol`; host checks passed.

No v0.4.1 Dolphin or Wii test has been performed. Earlier user-confirmed Mario
sprite/jump/spin direction results do not establish that the new mechanics or
rendering work visually. The [v0.4 report](docs/v0.4-movement.md) includes the
required playtest checklist and precise fidelity/animation limitations. Prior
[spin-effect audit](docs/spin-effects.md) explains why extra spin particles remain
absent. Publication/build success is not hardware or gameplay verification.
