# Movement animation (original clips)

Reference: Runouw/Super-Mario-63 `83e81fb`, pinned SWF
`6f8dc22d74963cb75928fcac6ad67769384c3088bb419f9995e24b81d8411e1c`.

## Where the clips come from

The CharCode in `DOMDocument.xml:6529-6531` picks the `Mario Main` frame label
`Fluddpow + frame` or `Fluddpow + attackFrame` (`"H1"`, `"Rbackflip"`...).
Those labels survive in the compiled SWF. `tools/swf_states.py` finds `Mario Main`
(DefineSprite 1707; Luigi's 2060 has the same labels) and the clip each label
places, with its matrix. `tools/export_movement_clips.py` renders every clip with
FFDec and records each frame's offset from Mario's feet (`Char` places `Mario Main`
at (0,-60) twips; ground checks use `Char._y+5`).

| Art state | Main label (none/H/R/T) | Frames | Wii frame rule |
|---|---|---|---|
| WALK | `1`/`2`, `H1`... | 82 | `Walk.xml`: `frame += xspeed`, wrap 1..80; ice rule |
| RISE / FALL | `3`/`4`, `H3`/`H4`... | 1 | CharCode: frame 4 when yspeed > 2 |
| DIVE | `Dive`, `HDive`... | 10 (Hover 8) | `Dive.xml` timeline; airborne `_rotation` easing |
| DIVEFLIP | `diveflip` | 9 | time, from the roll start |
| BACKFLIP | `backflip` | 22 | time |
| FRONTFLIP | `frontflip` (H/R/T 27) | 30 | time, third jump |
| SPIN | `Spin` (all use 1114) | 79 | `Spin Attack.xml`: fast 1-40, +40 when speed <= 10 |
| SWIM_FLOAT / STROKE | `swim1` / `swim2` | 2 | `Floating.xml`: yspeed > 0 |
| LAND | `land` | 8 | nested `Mario Land Animation`, time |
| RUN | `Run` | 87 | `Running.xml`: 0-5 start, then `round(frame)+7` |
| GROUND | `Ground` | 43 | `Ground Pound.xml` timeline |

FLUDD water layers are separate clips drawn behind or in front of the body, as
layered in `Mario Main`: `Mario Wait WaterBlast` (Hover, Turbo; level steps
up while spraying and down otherwise, mirrored vertically every frame) and
`Rocket WaterBlast` (charge 0-14, launch 15-22).

## Scripted sprites

Static export does not run ActionScript. The five scripted FLUDD sprites reachable
from these clips only choose a frame from `_root.Fluddpow`; the exporter freezes
each on that frame per FLUDD state and refuses unknown script shapes. Nested
water clips inside slides and flips are held at frame 1 (not spraying). One-frame
clips that nest an animation (land, FLUDD swim poses) are exported once per
nested frame.

## Physics corrected alongside

- Ground pound: 12-frame hang, fall from yspeed 8 with CharCode gravity/drag
  plus the clip's extra gravity, 16-frame landing lock (`xspeed *= 0.8` for 8).
- Hover dive recovery ends at frame 7 (`Slide (Hover).xml`), others at 9.

## Idle with FLUDD (a port addition)

The original plays Idle (1050) and Tired (1067), ending in Sleep (1450), only
from Mario (None) Walk (`count >= 92`); its FLUDD walk clips have no idle.
On request (2026-11-08) the port plays them with FLUDD on too.
`tools/idle_fludd.py` places the original's FLUDD pieces on each pose the way
the original's own clips do on those same poses (Mario Talk, Hurt 3, Boss
Cinema, Start Course Animation, Overworld Mario - 1): Fludd Side behind and
Fludd Side Front (the nozzle) in front on side poses, Fludd Front Code behind
a pose facing the camera, Fludd Backwards in front of one facing away. Poses
the original never draws with FLUDD (Fixing Hat, Thinking, Planning, Tired,
lying flat) take the nearest paired pose aligned onto their body. The FLUDD
sprites go into the SWF sprites and FFDec renders each FLUDD state
(`export_object_art.py` MARIO_IDLE_k / TIRED_k / SLEEP_k). Holding the FLUDD
button counts as moving, so the idle never covers a spray.

## Not reproduced yet

- Rocket blasts from a dive or frontflip, using `DiveRot` (nested rocket water
  clips), and Hover's 5-degree body tilt while moving.
- Ground-angle rotation of a grounded slide.
- Story-dependent states: stars, doors, pipes, paintings, hurt, drown, lava,
  Wing Cap flying, shells, sling stars, enemy bounce.
- Whether the original suppresses the triple-jump flip with FLUDD equipped
  (`DOMDocument.xml:5530` depends on AS2 string coercion); the port shows it.
