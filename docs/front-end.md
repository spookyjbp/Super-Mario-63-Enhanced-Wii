# The opening, title screen and main menu

The game now starts as the original does (its main timeline frames 6-9):
the copyright notice and Runouw logo, the title screen, then the main menu,
whose Story page is the file select. Play on a file starts the tutorial
with that file's progress and achievements. Quit Game in the pause menu
goes back to the title (the original's `ReturnToTitle()`).

## How it is drawn

The original's clips are replayed, not filmed: `tools/timeline_art.py`
reads the Flash timelines and flattens every frame into a list of pieces
(each shape, text or filtered clip baked once by FFDec) with the matrix and
colour transform Flash would use. `tools/export_front_end.py` writes them to
`assets/generated/front_end.bin` and the placements the port needs to
`front_end_layout.h`. Whole frames would take about 19 MB for the opening
alone; the replay is under 5 MB for everything.

* Filters are baked into the pieces. A blurred piece is stored at the
  resolution its blur leaves (3 / blur texels a pixel); soft pieces (glows,
  gradients) at a fifth; detailed ones at the port's 1.6 x. Pieces whose blur
  animates (the copyright text: 55 different blurs) are baked at 7 key blurs
  and cross-faded.
* Morph shapes are baked at 1/40 steps of their tween.
* Masks (the title's TV screen) are rectangles: each piece under one is
  clipped to it at export and drawn as a polygon.
* Pieces are cropped to what the 16:9 screen ever shows and split into
  508 px tiles; opaque detailed pieces are RGB5A3, soft ones RGBA8.
* Pages are grouped by what is shown together (the opening; the title; its
  transition; the menu) and only the group on screen is uploaded, into the
  course's MEM2 texture area (`COURSE_MEM2_RESERVE`, 21 MB; the title needs
  19.6 MB, the largest course area 19.7 MB). `tests/test_front_end.py` and
  `course_art_test` check both still fit.

## What follows the original

* Opening (9280): 341 frames at 32 a second; frame 64 holds 65 more (its
  `stop()` and counter); its MP3 sound stream (frames 158-341) is
  `SFX_OPENING`.
* Title (9339): synced to the title theme as the original (frame =
  position / length x 1106). The original then flies through eight other
  levels with the credits (from frame 341); those levels are not in the port,
  so the port loops frames 82-337 with the theme's 2.58-10.58 s (the
  original's own repeat point, and an 8 s phrase that lines up with it).
  START (9298) runs its script: grows in, pulses, grows when pressed; jump
  or START presses it. TitleScreenTransition (2137) plays 40 frames, the menu
  appearing on its frame 20.
* Main menu (9535): the scrolling backdrop (9373's layers and their
  scripts), the buttons' float script (bob, sway, grow when chosen, spin and
  bounce when pressed), MenuTransition between pages, the file menu (9433)
  sliding its PLAY! / COPY / DELETE / CANCEL in, Copy into an empty file
  (MenuGreenTrans), Delete with the original question (MenuRedTrans) and the
  erase animation (9403), the red messages (MenuMessage), Play's fade to
  black (MenuBlackTrans), Options (Mute Music; the Flash quality and speed
  settings shown, unavailable), Extras and the credits (scrolled with the
  D-pad inside their box). Four files, A-D, as the original.

## What the port changes

* Controller: the D-pad moves between items (the nearest one that way),
  jump chooses, spin goes back.
* Extras: Level Designer is not in the port; its place holds the port's
  Achievements page (per file; Left / Right changes file). Share levels and
  the Sound Test are shown greyed out (the port has none of their songs or
  sharing).
* Options: the PC controls panel (9516) is left out.
* The Wii mode choice (frame 8) is skipped: the port is always on a Wii.
* Text the original sets from script (the delete question, the messages,
  the Achievements label) is drawn in the original font 155.

## Memory

`player_art.bin` and `font_atlas.bin` are stored LZ4-wrapped
(`tools/pack_lz4.py`, `include/lz4w.h`) and unpacked into MEM2 at boot; the
embedded data is about 18 MB of MEM1's 24 MB. MEM2 holds about 46.5 MB.
