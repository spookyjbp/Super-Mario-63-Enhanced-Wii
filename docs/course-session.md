# Tutorial session (course 8-E1)

`source/game/course_session.c` runs the original tutorial on top of the course
collision and camera. `source/platform/course_render.c` draws it. Rules and their
sources (paths under `Super_Mario_63_2010/LIBRARY/Course Clips/8 - Storyline
Stars/8 - Tutorials/` unless noted):

| Behaviour | Source | Port |
|---|---|---|
| Boot into 8-E1-1 at (0,0), no FLUDD | `Menu/Movies/Intro Movie.xml:225` | `course_session_start("8-E1-1", 0, 0)` |
| Area title "Mushroom Kingdom / (Tutorial)" | `8-E1-1 Back GFX` frame 3 | timed title |
| Barrier until the controls sign | `8-E1-1 Ground` frame 1 script; sign2 `completefunction` | `platform_frame` 0 then 1 |
| Opening walk-in and Start page | Start `Coded Sign` enterFrame | `SESSION_OPENING`: `y = 60`, 60 frames, 12-frame walk, read |
| Read signs with Z, close with Z | `Coded Sign.xml` frames 3/10/16 | box (-7.5,-7.7)-(7.5,7.3) at the sign's base |
| Camera on another clip while reading | `othercamcenter` | centre eases 1/10 per frame |
| Hint zones (`TextHint`) | `Blank Movieclip` `mess` instances | Blank shape (SWF 839) under its matrix; "No Mario!" only with the barrier |
| Area warps | each `BackGFX` frame-3 `changecourse(...)` | table in `course_session.c`; arrive at (d, e); respawn there |
| Completion | `Invisible Door` `doorplace = "C-Opening"`, `DOMDocument.xml:13207` | Down on the pipe: boundary screen (Intro Movie 2 and the Castle are not ported) |

Artwork: `tools/export_course_art.py` renders each area's FarBackground,
Background, CloseBackground, BackGFX (resting frame 3) and FrontGFX at 1.6
screen px per course pixel with placed objects removed and the 0-star colour
scripts applied, plus the picture-sign pages. Tiles are GX RGB5A3, LZ4-compressed
(`assets/generated/course_8e1_art.bin`, 4.4 MB); one area is decompressed into a
26 MB MEM2 reservation when entered.

## Not yet ported (see the reference for full lists)

Coins and health HUD, blue coins, Goombas, Cheep Cheeps, Bob-ombs, Koopas, Toad
dialogue, the falling log, breakable boxes, the FLUDD box and Hover tutorial
objects (water bottles, moving/tipping/rotating platforms, camera control boxes,
Giant Water Block), birds and butterflies, animated water surfaces, the original
message box and font (text uses the placeholder 5x7 font), controller-specific
buttons on the picture pages, transitions' original art, and all audio.
