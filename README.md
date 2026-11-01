# Super Mario 63: Enhanced Wii Edition

A native Nintendo Wii homebrew project using C/C++, devkitPPC and libogc.

**Milestone 1 is a build pipeline and interactive graphics test, not a playable
port of Super Mario 63.** The repository was empty at initial inspection; no
original source, levels, story data or art assets were available to port.
The scene uses original placeholder rectangles and placeholder movement.

## Scope

The intended game is the original Story Mode, preserving its gameplay, physics,
levels, story and art style while improving presentation for Wii. Level Designer
and all level-editing functionality are excluded. Future work must establish
authorized access to the original source/assets and measure fidelity and
performance on hardware. No original game code or assets are bundled here.

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
is optional and is not supplied. Expect a blue sky, sun, cloud, hill, ground and
a movable red square. The square is an input/graphics test, not Mario.

Use Wii Remote channel 1 or GameCube port 1:

| Controller | Move | Jump | Return to loader |
| --- | --- | --- | --- |
| Wii Remote + Nunchuk | Nunchuk stick / upright Remote D-pad | A or Nunchuk Z | HOME |
| Classic Controller | Left stick / D-pad | A | HOME |
| GameCube | Stick / D-pad | A | START |

The Wii RESET button requests a clean exit. Console and Wii Remote power
callbacks request shutdown from the main loop. Controller hot-plugging is polled
each frame. Stick dead zones reduce idle drift.

## Layout

```text
source/main.c               Application lifecycle and placeholder test scene
source/platform/graphics.c  GX rendering, video mode and frame presentation
source/platform/input.c     Controller polling and common input state
include/                    Platform interfaces
packaging/meta.xml          Homebrew Channel metadata
.github/workflows/build.yml Official-toolchain build and artifact upload
```

## Validation

The initial scaffold compiled successfully without warnings using the official
image pinned above. `boot.elf` was identified as a 32-bit, big-endian PowerPC
executable, and `make package` generated `boot.dol` and Homebrew Channel metadata.
No hosted GitHub Actions run, Dolphin run or Wii hardware test has been verified.

Compile with the official container and check that `make package` succeeds before
testing. A second `make package` should reuse existing objects; `make clean` then
`make package` checks the complete build again.

Hardware validation remains required: check scene visibility, each input device,
disconnect/reconnect, HOME/START and RESET exits, power handling, 4:3 and 16:9
proportions, and PAL/NTSC video modes. Dolphin can provide an additional smoke
test but cannot establish original Wii performance or controller behavior.
Story Mode fidelity and 60 FPS performance are future milestones.
