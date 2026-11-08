# Castle Grounds

User request (2026-11-08): Peach's Castle, the grounds in full, with the
front door and pipes shown but closed for now.

The area is the original's `Castle` (CastleBackGFX 2406 on its resting frame
3, CastleFrontGFX 2422, CastlePlatforms, CastleBPlatforms, CastleCamEdge,
CastleWater, the backgrounds `_root.CastleBackground` = CastleColoredBG and
`_root.CastleFarBackground` = CastleColoredFarBG). It is in the course pack
(`assets/generated/course_8e1.bin`) and the course art data file
(`course_8e1_art.bin`) with the tutorial's areas.

## Ways in

| From | Original | Port |
| --- | --- | --- |
| Intro Movie 2's end | `StartChar("Castle", 80, 60, ...)` (10108 frame 436) | `main.c` after `cinema_intro2.bin` |
| Play on a file with Shine Sprites | frame 10: `CharXscale = -100; StartChar("Castle", -1100, 100, ...)` | `main.c` (facing left) |
| the tutorial's last pipe with `Star[39]` | doorplace "C-Opening": `changecourse("righttransition2", "Castle", -1300, 100)` | warp rule in `course_session.c` |

The Castle has no `CastleMusic` and its BackGFX frame 1 calls `StopBGsong()`:
no music, as in the original. The waterfall's Sound FX Clip loops
"waterfall" by the camera's distance (`distancevolume`, `panX`).

## What is there (BackGFX frame 3 and FrontGFX frame 1)

| Original | Port |
| --- | --- |
| the castle (Princess Peaches Castle Code), pipes, doors, cannon, Sling Stars, Rainbow Star | in the layer art as placed (`export_course_art.py` KEEP) |
| CastleWater: three Water Code strips over three Giant Water Blocks | the level water (`water_art.py`); the small pool by the waterfall is tilted 5.6 degrees in the original and drawn level |
| Sling Star (5, -577; xblast 0, yblast -27) | `course_session.c` `sling_*`: held in the star, jump or spin to spin it, flung up to the roof; dropped after 32 frames without a press |
| 1 Up Code x3 on the roof | `LIVE_ONEUP`: the 1-Up Animation, "collect", "1up", taken for good |
| Cheep Cheeps, signs ("Left Leads to Tutorial..."; the controls picture), Camera Control Boxes, the area name | as in the tutorial |
| left pipe (warp1, with `Star[39]`) | lefttransition to 8-E1-2-2 (964, -783) |
| the door hint "(Hint: Press Up to enter a door)" | only while `TotalStars <= 0` and `Star[39]` is false, as in the original (so not after the tutorial) |

Closed (user decision): the front door ("inCastle"), the locked door
"C-8-3", the warp pipes (8-E3-1, 8-2-1, 8-10, 8-11u, 8-13, BC-0, BC-1) and the
right pipe (warp2, K-1). Up at a door or Down on a pipe shows
"(Closed for now: not in this port yet)".

On a file below 63 Shine Sprites the cannon is not set up (`cannonsetup =
false`), as in the original.

## Memory

The Castle's BackGFX and FrontGFX span about 4000 x 1100 course px: at the
art's 1.6 image px per course px they would take 31 MB, more than the 21 MB
an area's textures get, so they are drawn at 1.15 (`AREA_ZOOM`).

## Not done yet

- The Rainbow Star's Wing Cap (`WingCap`, `PowerTimer` 960, the Wing Cap
  music and flying): the star is drawn but cannot be taken.
- Mario's own Sling Star animation (1476): the port shows his spin pose
  while he is in the star.
- The Sling Star's own turning (`star.base._rotation`): drawn still.
- The castle's colour easing with the stars collected (the art has the
  colour of a new file).
- Lives (`CharLives`) are not counted, so a 1-Up only plays its sound.
- Not tested in Dolphin or on a Wii yet.
