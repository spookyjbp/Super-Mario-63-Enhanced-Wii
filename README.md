# Super Mario 63: Enhanced Wii Edition

A native Nintendo Wii homebrew project using C/C++, devkitPPC and libogc.

**v0.3 — Movement and Physics** adds a responsive platforming playground with
original animated Mario sprites from Runouw's source repository. This is not yet
the Story Mode port. A small, attributed subset of original player artwork is
included; levels, story and measured original physics are not ported. Movement
constants here are independently tuned starting points, not a fidelity claim.

## Movement playground

- Accelerating ground/air movement, stronger braking when reversing, and stick dead zones.
- Variable jump height: tap A for a short hop, hold A for the full arc.
- Jump again within 0.28 seconds of landing to chain a double and then triple
  jump. After the third jump or an expired window, the sequence restarts.
- A 0.10 second jump buffer and coyote window make edges and landings forgiving.
- Ground/midair spin lasts 0.32 seconds with a 0.50 second cooldown. Only one
  aerial spin is allowed per landing; it briefly lifts/stalls descent. The active
  attack rectangle is exposed for future targets; there are no enemies yet.
- Ground pound has a 0.12 second windup, fast downward acceleration, an impact
  effect and 0.18 second landing recovery. No jump chaining out of a pound.
- Swept, axis-separated solid-rectangle collisions handle floors, ceilings and
  walls, including thin platforms at pound speed. No slopes, moving platforms
  or one-way surfaces are implemented. Falling into the gap respawns the player.
- Smoothed, world-bounded camera with render interpolation. The HUD displays the
  current animation state and jump stage. Original Mario frames animate walking,
  jumping, falling, spinning and pounding; spin sparks and landing dust remain
  placeholder effects.
- Pause menu: Resume, Restart (at the spawn) and Exit to Loader. Simulation and
  animation timers freeze while paused.

Physics advances at fixed 60 Hz with a bounded accumulator and up to eight
catch-up ticks per rendered frame. Input presses survive frames with no tick
and are consumed once during catch-up. PAL 50 Hz rendering still runs 60 Hz
simulation. A prolonged stall drops excess catch-up time to avoid a spiral;
this is not a guarantee of real-time 60 FPS on hardware. There are no gameplay
heap allocations; GX framebuffers/FIFO are allocated only at startup.

## Scope

The intended game is the original Story Mode, preserving its gameplay, physics,
levels, story and art style while improving presentation for Wii. Level Designer
and all level-editing functionality are excluded. Future work must port the original level and story data and measure fidelity
and performance on hardware. A small subset of original Mario PNGs is bundled; see [asset provenance and credits](assets/mario/README.md).

The scaffold supports Wii Remote + Nunchuk, Classic Controller and GameCube
controller input. It follows the console's preferred video mode and 4:3/16:9
system setting; widescreen uses a wider logical view with anamorphic output.
Rendering is synchronized to video refresh. The goal is 60 FPS where practical
on Wii; PAL 50 Hz modes retain their native refresh. No hardware performance
claim is made.

## Build

Install the official [devkitPro toolchains](https://devkitpro.org/wiki/Getting_Started)
and the `wii-dev` package group (devkitPPC, libogc and Wii tools). In a configured
devkitPro shell:

```sh
export DEVKITPRO=/opt/devkitpro
export DEVKITPPC="$DEVKITPRO/devkitPPC"
export PATH="$DEVKITPRO/tools/bin:$DEVKITPPC/bin:$PATH"
make -j2 package
```

Alternatively use the official Linux container, also used by CI:

```sh
docker run --rm -v "$PWD:/project" -w /project \
  devkitpro/devkitppc@sha256:d50f8a886f2ce1d3c22685a774afb0845379876b47d9b41a9b729f4f63a54813 \
  make -j2 package
```

The image supplies devkitPPC, libogc and its supporting libraries. Both C (`.c`)
and C++17 (`.cpp`) sources are discovered in the listed source directories; C++
sources switch the linker to G++. Keep source basenames unique across directories.
Add new module directories to `SOURCES` in the Makefile.

Outputs:

- `boot.dol`: executable for the Wii loader.
- `boot.elf` and `boot.elf.map`: debugging symbols and link map.
- `dist/apps/super-mario-63-enhanced-wii/`: packaged application.

`make` builds the executable, `make package` refreshes the install folder, and
`make clean` removes generated files. Build outputs are ignored by Git.

## GitHub Actions

Every push and pull request builds in the official `devkitpro/devkitppc` image on Linux;
the workflow can also be started manually from the Actions tab. A successful
build uploads the `super-mario-63-enhanced-wii` artifact containing the application
folder and its `boot.dol` and `meta.xml`. Download and extract the artifact after
the workflow succeeds. Failed builds do not upload an application artifact.

The workflow and local container command pin the same verified image digest.
Update both together and validate a clean build when upgrading the toolchain.
Local compilation alone does not establish that a hosted Actions run succeeded.

## Install and run

Use a Wii with the Homebrew Channel already installed. Copy the generated folder
to the SD card (or a supported USB device), preserving this layout:

```text
SD:/apps/super-mario-63-enhanced-wii/boot.dol
SD:/apps/super-mario-63-enhanced-wii/meta.xml
```

Launch **Super Mario 63: Enhanced Wii Edition** in the Homebrew Channel. An icon
is optional and is not supplied. Expect a scrolling platform playground with original Mario artwork, a
state HUD, platforms, a wall, a low ceiling and a gap. Mario now uses original
sprites with transparency and facing direction; terrain is still placeholder art.

Use Wii Remote channel 1 or GameCube port 1:

| Controller | Move | Jump (hold for height) | Spin | Ground pound | Pause |
| --- | --- | --- | --- | --- | --- |
| Wii Remote + Nunchuk | Nunchuk stick / upright Remote D-pad | A | B or Nunchuk C | Nunchuk Z | PLUS or HOME |
| Classic Controller | Left stick / D-pad | A | X | ZR | PLUS or HOME |
| GameCube | Stick / D-pad | A | B | Z | START |

Menu: D-pad up/down to choose, A to confirm, B to resume, or the pause button to
resume. To exit normally, choose **Exit to Loader** in the pause menu. HOME and
START now open the menu rather than immediately exiting. Nunchuk Z is now pound,
not jump. Remote-only fallback uses D-pad movement and Down to pound.

The Wii RESET button requests a clean exit. Console and Wii Remote power
callbacks request shutdown from the main loop. Controller hot-plugging is polled
each frame. Stick dead zones reduce idle drift.

## Layout

```text
source/main.c               Lifecycle, fixed-step accumulator and input latching
source/game/player.c        Platform-independent movement, attacks and collision
source/game/game.c          Pause/restart/respawn and input edge handling
source/game/camera.c        Camera smoothing and world limits
source/game/world.c         Static collision playground (not a Story Mode level)
source/platform/input.c     Wii/Classic/GameCube mappings
source/platform/graphics.c  GX video, rectangles and small built-in text font
source/platform/render.c    Original Mario animation, interpolated scene and menu
assets/mario/               Original PNG subset and provenance
assets/generated/           Embedded GX texture atlas and frame metadata
tools/build_mario_atlas.py  Reproducible PNG-to-GX conversion (optional Pillow tool)
include/                    Game and platform interfaces
tests/                      Host-side physics tests, no Wii SDK required
packaging/meta.xml          Homebrew Channel metadata
.github/workflows/build.yml Host tests, Wii compilation and artifact upload
```

## Validation

Run platform-independent tests with the host C compiler (no devkitPro needed):

```sh
make -C tests test
```

Nine test groups exercise acceleration/braking/reversal, variable jump height,
consecutive jump windows, input buffering and coyote time, floor/wall/ceiling
and edge collisions, high-speed pounds, spin limits and attack bounds, landing
recovery, camera clamps, pause/restart/exit/respawn and one-shot input latching.
Sprite conversion also verifies source checksums, atlas bounds and RGBA tile layout.
Tests compile with `-Wall -Wextra -Werror -pedantic`. CI runs these tests before
building the Wii application; Wii compilation also treats warnings as errors.
All gameplay calculations tested are the same C sources linked into `boot.dol`.

The v0.3 source compiled using the pinned official toolchain without errors or
warnings and generated `boot.dol`. Host tests passed. This establishes build and
calculation checks, not gameplay verification.

Dolphin and original Wii testing remain required: responsiveness, controller
mapping and hot-plugging, jump/spin/pound timing, collision corners, pause and
loader return, power/reset handling, camera smoothness, 4:3/16:9 proportions,
PAL/NTSC pacing and performance. No Dolphin or Wii gameplay test has been
performed in this environment. Story Mode fidelity remains a future milestone.
