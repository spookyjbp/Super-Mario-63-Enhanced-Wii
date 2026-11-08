# The opening, title screen and main menu

The game starts as the original does (its main timeline frames 6-9): the
copyright notice and Runouw logo, then the title screen. START goes straight
to the file select (the main menu's Story page; user request 2026-11-08:
no Start page, no Options or Extras). Play on a file starts the tutorial
with that file's progress and achievements. BACK (or spin) on the file
select, and Quit Game in the pause menu, go back to the title (the
original's `ReturnToTitle()`).

## How it is drawn

The original's clips are replayed, not filmed: `tools/timeline_art.py`
reads the Flash timelines and flattens every frame into a list of pieces
(each shape, text or filtered clip baked once by FFDec) with the matrix and
colour transform Flash would use. `tools/export_front_end.py` writes them to
`assets/data/front_end.bin` and the placements the port needs to
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
* Full screen (user request 2026-11-08): the opening, the title and the
  menu's backdrop are drawn with the 450 x 300 stage as wide as the screen
  (1.9 x at 16:9), the extra rows cut around a height that keeps the screen's
  content in view (the title: its logo's top to START's foot, stage y 19-270).
  The file select's own items (the files, the buttons, the delete question)
  need the stage's whole height, so they stay at 1.6 x in the middle, over
  the full-screen backdrop; the red delete screen covers the screen too.
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
  or START presses it. TitleScreenTransition (2137) plays 40 frames, the file
  select appearing on its frame 20.
* The file select (9535's Story page): the scrolling backdrop (9373's layers
  and their scripts, its sky with the original's white stripes), the
  buttons' float script (bob, sway, grow when chosen, spin and bounce when
  pressed), PLAY! / COPY / DELETE / CANCEL sliding in, Copy into an empty file
  (MenuGreenTrans), Delete with the original question (MenuRedTrans) and the
  erase animation (9403), the red messages (MenuMessage), Play's fade to
  black (MenuBlackTrans). Four files, A-D, as the original.

## What the port changes

* The file select's layout (user request 2026-11-08, `FE_GRID_*` /
  `FE_PANEL_*` in front_end.h): the original file boxes in a 2 x 2 grid on
  the left under STORY, BACK under them; on the right a detail panel in the
  pause menu's row style: the file's own label (9414's "File A"), its Shine
  Sprites, Star Coins and achievements (of 64) and its trophies by tier (or
  "Empty file"; while copying, "Copy File A to File B"), and under them the
  original PLAY! / COPY / DELETE / CANCEL buttons (2 x 2), sliding in from
  the right one after another when the file is chosen. The panel shows the
  file under the cursor (the chosen one once open).
* Controller (jump chooses, spin goes back): the D-pad follows fixed links
  (`front_menu_link`): the files' 2 x 2 grid (A B / C D), BACK under it (Down
  from C or D), the open file's buttons PLAY COPY / DELETE CANCEL; buttons
  that cannot be chosen yet are skipped, and the cursor waits on a button
  still sliding in. Holding a direction repeats after 0.4 s every
  0.1 s. Spin: stops copying, closes the open file, else back to the title.
  Delete's question: Left / Right Yes / No, spin is No.
* No Start, Options or Extras pages (user request): Level Designer, Share
  levels, the Sound Test, the credits page and Mute Music (in the pause
  menu's Options) are not in the menu.
* The Wii mode choice (frame 8) is skipped: the port is always on a Wii.
* Text the original sets from script (the delete question, the messages) is
  drawn in the original font 155.

## Memory

`player_art.bin` and `font_atlas.bin` are stored LZ4-wrapped
(`tools/pack_lz4.py`, `include/lz4w.h`) and unpacked into MEM2 at boot; the
embedded data is about 18 MB of MEM1's 24 MB. MEM2 holds about 46.5 MB.
