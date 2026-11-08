# Story cutscenes

User request (2026-11-08): the cutscene before the tutorial, the one after
it, and Peach's Castle. This page covers the two cutscenes.

| File (SD card `data/`) | Original | When | Ends with |
| --- | --- | --- | --- |
| `cinema_intro.bin` | main timeline frame "Intro", sprite 9814 (1051 frames, its own MP3 sound stream) | Play on a file with no Shine Sprites (the original's frame 10: `TotalStars == 0`) | `StartChar("8-E1-1", 0, 0, ...)`: the tutorial |
| `cinema_intro2.bin` | frame "Intro2", sprite 10108 (436 frames, holding 9979, 10055 and 10106 with 1907, 493 and more frames of their own) | the tutorial's last pipe (doorplace "C-Opening", `Star[39]` false) | `StartChar("Castle", 80, 60, ...)` |

## How they are made

The cutscenes are not plain timelines: their scripts move the camera (9862
`camControl` scrolls and scales `Course`, `BG` and `farBG`, shakes them),
throw debris (10047), fade colours, start and fade the music, show the cinema
messages and wait for the read button (Z). So `tools/cinema_sim.py` plays them
the way Flash Player does, with the game's own ActionScript:

- `tools/as2.py` is a small ActionScript 2 interpreter for the scripts FFDec
  decompiles (SWF 8 rules: `undefined` is NaN in arithmetic, so the water
  clips, which test `_root.Course.Char`, stay empty as in the original).
- `cinema_sim.py` keeps the display list (clips, their frames, script-set
  properties that the timeline no longer moves, `attachMovie`, `Color`,
  `Sound`), runs a frame as AVM1 does (enterFrame, then every playing clip's
  next frame, then the queued frame scripts) and records every frame's draws.
  It presses the read button 40 frames after a message is out (`HOLD`) and
  notes where (PRESS); random numbers come from a fixed seed.
- `tools/export_cinema.py` turns the recording into the data file: the
  events (messages with their portrait, PRESS, music starts with their
  `onSoundComplete` repeat, stops, volume changes, sound effects including the
  timelines' own event sounds, the Skip button's target, the end), the sounds
  (IMA ADPCM with seek points, so the music can start part way), and a
  streamed timeline pack ("SM63TLP3", `timeline_art.write_stream`).

The streamed pack exists because a cutscene's art is far bigger than memory
(the Intro alone draws about 42 MB of textures). Each frame draws only the
256 x 256 tiles in view, each piece at the resolution it is drawn at (levels
2^(L/2) apart), all in RGB5A3; `cinema_render.c` keeps the tiles of the next
8 frames decoded in the course renderer's MEM2 area (decoding a few ahead each
frame) and the message box's always. The exporter checks the most this needs
fits in that area.

## Playing them

`cinema.h` is the player (host tested in `tests/cinema_test.c`):

- the recorded frames at 32 a second on the 60 Hz tick;
- 627's message box, as its script: it fades in, types two characters a frame
  once it is in, the read button shows all of it, the prompt (`bz`) fades in
  when it is out. The port writes the text (the original's tags taken out)
  and shows the controller's jump button where `bz` is;
- at a PRESS the cutscene holds until the read button (the course signs'
  read button, or jump) is pressed with the text out; pressing earlier, once
  the text is out, goes on from there;
- START does what the original's Skip button does at that point (9814: to
  "Scene2", then to "end"; 10108: to "scene2", "scene3", "scene4", "end"),
  with the music it would have there;
- the music is decoded as it plays (`audio_cinema_update`).

The stage is drawn 1.6 times (its 300 rows on the 480 lines), its sides as
far as 16:9 shows; the cutscenes' own masks keep them to the stage width.

## Memory

The cutscene's file replaces the front end's and the course art's files in
MEM1 while it plays (they are read again afterwards); its textures, draws and
sounds use the course renderer's MEM2 area (no course is shown meanwhile).

Intro Movie 2 ends in the Castle Grounds (docs/castle.md).

## Not done

- Not tested in Dolphin or on a Wii yet.
