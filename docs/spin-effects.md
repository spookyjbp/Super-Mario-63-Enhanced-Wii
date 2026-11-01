# v0.3.2 spin effects audit

## Yellow-square cause

In the previous `source/platform/render.c`, `render_game` checked
`p->spin_time > 0` and drew six untextured 7x7 rectangles with RGBA
`(255, 250, 120, 255)`. Their positions came from sine/cosine offsets around the
interpolated player. Those rectangles were the Wii playground's placeholder
spin particles, not an atlas transparency or orientation problem.

v0.3.2 removes that loop. The original Mario spin body animation still renders
with the corrected per-keyframe mirrors, at the interpolated world position
minus the camera position. There are no extra spin particle draws or allocations.
Landing dust and the test scene's sun are separate and remain unchanged.

## Original-source investigation and result

Inspected Runouw's https://github.com/runouw/Super-Mario-63 at commit
`83e81fb6b80d11c4119e420d25298000a6e13f66`, including:

- `Super_Mario_63_2010/LIBRARY/Char/Mario CLIPS/Mario (none) Spin Attack.xml`:
  the existing five Mario poses, reused with authored keyframe mirrors;
  conditional FLUDD/wing layers; and animation/gameplay ActionScript.
- `Super_Mario_63_2010/DOMDocument.xml`: spin input/attack selection and the
  character's dispatch to the spin clip.
- The library's `Effects` directories and the referenced `Xtra/Lineless Movieclip.xml`.
- Repository root and README for license/asset terms. No LICENSE/COPYING file
  was found. The README invites changes and pull requests and contains a
  third-party graphics copyright disclaimer, but does not establish a separate
  asset redistribution license for an additional spin effect.

No separately redistributable authentic spin effect was established from that
inspection. **No new effect artwork or Flash scripts were imported. This release
uses the requested fallback: the original Mario spin animation without extra
particles. Authentic additional spin effects are deferred.** This does not change
or relicense the previously imported Mario sprite subset.

## Verification boundaries

`make -C tests test` runs the existing physics/orientation groups and three
render-command regression groups. The rendering tests compile the real
`render_game` and sprite-selection code against a recording graphics interface;
they check that spins add no rectangles, remain attached to moving/jumping Mario
and a moving camera, and pass the corrected UV direction through to the sprite
draw call. These are command-level checks, not an emulator or GPU visual test.

The user confirmed that the preceding build displays Mario and faces correctly
while jumping/spinning in Dolphin. v0.3.2 still requires Dolphin confirmation:
spin on the ground, while running, during jumps and air reversals, and near
camera boundaries; verify no yellow squares and retained facing. Pause mid-spin
and resume. No new visual success is claimed from compilation or host tests.
