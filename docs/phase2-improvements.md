# Phase 2: animation, graphics, FLUDD, controls and audio

| # | Improvement | What the port does now | Where | Regression tests |
|---|---|---|---|---|
| 1 | Two missing hurt animations | `_root.MarioHit` (frame 3) as in the original: with damage the speed halves and the attack becomes a hurt clip. `g == false`: **hurt1** (Mario Main 1411) - setYspeed/setXspeed are added on its frame 4, attack ends on frame 9. `g == true`: the speed is added now; **hurt3** (1430) when the new xspeed points the way Mario faces (`xspeed * _xscale < 0`), else hurt2 from "afterwall". hurt3 waits on frame 4 for the ground or water and on frame 16 for a key, attack ends on frame 23. Frames up to the attack's end are exported (hurt1 1-9, hurt3 1-23; later frames are the 0 HP faint). Blink/invulnerability, HP and stun are unchanged; normal poses cannot replace a running hurt clip; jump-cut, ground pound and FLUDD wait for it. | `course_objects.c` `course_mario_hit`, `player_special.c` ACTION_HURT, `player_art_select.c`, `export_movement_clips.py` | `course_objects_test` `test_hurt_animations`, Goomba hurt timing; `art_pack_test` clip lengths and selection |
| 2 | Wii controller picture signs | The keyboard pages (Start, controls, bomb, hover, stomp, zoom) get one version per controller: GameCube, Wii Remote + Nunchuk, Classic Controller and Wii Remote alone. Each keeps the original page art; only the keyboard parts are cleared back to the page's own background shape and redrawn with that controller's buttons (taken from `input.c` / `input_map.c`). The page shown follows the controller used last (`input_active_controller`), also while the sign is open. Text signs naming keys ("pressing Up and back", "pressing **P**") get the jump and pause buttons. Pages are drawn one texel per screen pixel, centred (readable at 4:3 and 16:9). | `tools/controller_signs.py`, `export_course_art.py`, `course_art.c` `course_art_controller_picture`, `course_session.c` `course_sign_message`, `course_render.c` | `course_art_test` (24 pages present, fallbacks, texture size), `course_session_test` `test_sign_messages_per_controller`, `controls_test` controller detection |
| 3 | Choppy / doubled Mario | Mario moves every 60 Hz tick but the original camera moves once per 32 Hz source frame, so on 28 of every 60 ticks the view stood still while Mario moved and on the others it jumped: on screen Mario stepped back and forth (judder, seen as doubling). The camera's 32 Hz move is now spread over the following ticks; the view and Mario are placed on whole screen pixels (Mario is sampled nearest-texel). Physics and collisions are untouched. | `course_world.c` `publish`, `game.h` Camera, `course_render.c` | `course_session_test` `test_camera_moves_every_tick` |
| 4 | Rocket spray in special moves | The Rocket water nested in RDive (1349 in 1355) and Rfrontflip (1370 in 1372 'waterclip') is exported (39 frames: charge 1-30, blast 31-39) with its per-frame placement, and drawn while charging and during the boost, following the dive rotation and facing; never when the Rocket is idle or with no nozzle. | `export_movement_clips.py`, `player_spray.h`, `player_art_select.c` `player_spray_pick`, `user_art.c` | `art_pack_test` `test_spray_during_specials` |
| 5 | Custom water | Port-made water (the tutorial's lake had a flat fill): a translucent body with soft caustic bands drifting slowly and an 8-frame wave crest with a white highlight along the surface. Two small RGB5A3 textures (64x64 and 64x128, 24 KiB), seamless horizontally (the body also vertically), one repeating quad each per volume: no seams, no full-screen pass. Drawn over Mario as before, so he is tinted underwater. Water collision and swimming are unchanged. | `tools/make_water_texture.py`, `water_tex.h`, `course_art.c` `course_water_quads`, `course_render.c` | `tests/test_water_texture.py`, `course_art_test` water quads |
| 6 | Mario half-pixel offset | Confirmed in the exported art: Mario's frames are pixel-art bitmap fills, and FFDec draws non-smoothed fills with texel floor(u - 1/2) instead of the Flash Player's floor(u). Where a fill sits off the pixel grid (most poses: rotated flips and dives, scaled or half-pixel placed parts) rows or columns of pixels were doubled or dropped and some frames were shifted by a pixel against their neighbours. The movement exporter now applies the same matrix correction as the course art (`swf_bitmap_fix.py`); 1,509 of the 1,874 previously exported frames changed (all frames compared), the rest were already exact. Proportions and timing are unchanged. On screen, the view and Mario are placed on whole screen pixels (Mario is sampled nearest-texel), and frame UVs stay on texel edges. | `export_movement_clips.py`, `player_art.bin`, `course_render.c` | `art_pack_test` (committed pack), `test_player_art.py` |
| 7 | Toad reactions | Toad Main Clip 2517 and its head 2498: idle body (120 frames with its random skips) or the walk body (8-E1-2's Toad), the head's eight moods (Smile, Talk, Sad, Mad, Shocked, Happy, Confused, Crazy) with their random Skip jumps, each Toad's start Mood / frame / `_xscale`, each page's M<n>Moo mood, Mario and Toad facing each other on the first page, Mario pushed out of Toad's body, Toad's CameraZoom to 150 while talking, and the start mood back after the talk. | `course_objects.c/h`, `course_session.c`, `object_select.c`, `export_object_art.py` | `course_objects_test` `test_toad_reactions` |
| 8 | None nozzle | `_root.ChangeFludd`'s cycle: Hover -> Rocket -> Turbo -> None -> Hover over the collected nozzles. None: no spray, water, lift, sounds or FLUDD on Mario's back; the tank is hidden as in the original (Fluddpow ""). A banner names the selected nozzle for 1.5 s. Picking up FLUDD still makes every nozzle available (the port's existing rule). | `player_special.c` `player_change_fludd`, `course_session.c`, `course_render.c` | `movement_v04_test` nozzles, `course_session_test` `test_none_nozzle` |
| 9 | Zoom (as the original) | `_root.CameraZoom`: the zoom input changes it by 2 a source frame (KeyPlus / KeyMinus; a stick scales the rate), clamped to Maxzoomout 50 .. Maxzoomin 200 (0.5x-2x; no tutorial area overrides them); the view (CameraZoom2) follows a twentieth of the rounded difference a frame, smoothed between source frames. Toad's talk is the same variable: +3 to 150, back to 100 after (unless it was already past 150). The camera keeps the zoomed view inside CamEdge (tested at the zoomed screen edges, as the original does); backgrounds follow the zoom less: Background 1/5, MidBackground 1/2, CloseBackground 1/1, the far background not at all. Controls: GameCube C-stick, Classic right stick, Wii Remote + / -. | `course_session.c` `course_session_zoom_step`, `course_world.c` `course_camera_set_zoom`, `course_render.c` | `course_session_test` `test_zoom_controls` |
| 10 | Nozzle and Rocket audio | Switching (to None too) plays the original "FluddSwitch"; it only plays when the selection changes. The 2010 Rocket is silent, so two short port-made sounds (synthesised filtered noise, -9 dBFS): a pressure charge when the charge starts and a burst when it fires. Both go through the voice manager from the coin fix (no stacking, no clipping). | `export_audio.py`, `sfx.h`, `player_special.c` | `sound_test` (pack has every sound, voice rules) |

## Control scheme change (after Phase 2 testing)

Requested by the user: the Nunchuk is no longer supported (a Wii Remote with one
attached plays as a sideways Wii Remote), every controller moves with its D-pad
(no analog movement), and the jump button does nothing in water (Up swims).

| Action | GameCube | Classic Controller | Wii Remote (sideways) |
|---|---|---|---|
| Move | D-pad | D-pad | Control Pad (Up on the pad = left) |
| Jump | A | b | 2 |
| Spin | B | y | 1 |
| Stomp / read / talk | Z | L or R | A + B together |
| FLUDD | R | ZR | B |
| Nozzle | L | ZL | A |
| Zoom | C-stick | Right stick | + in, - out |
| Pause | START | + (or HOME) | + and - together (or HOME) |

Combos (`input_chord_step`): the second button counts if pressed within 6 polls
(~0.1 s). A waits that long before switching the nozzle (or switches on a quick
release); B sprays at once and stops when A joins it; + and - zoom only after
the wait. Mapping code: `source/game/input_map.c` (host tested in
`controls_test`), hardware reading: `source/platform/input.c`. The controller
picture pages now exist for GameCube, Classic and Wii Remote (their look is
still to be redesigned).

## Text signs (original message box and font)

Text signs use the original message box, Stats.Message (sprite 253), rendered from the SWF without its text field and prompt (`tools/ui_art.py`):

- **Animation.** Frames 2-6 open the box and frames 7-11 close it; the port scales the frame-6 box into each frame's rectangle (`ui_layout.h` `message_box_rects`).
- **Text.** The message is drawn in the original font 155 (bold Arial, the 2010 SWF's embedded font) at the field's size 17, centred and white within the field's bounds. Font 155 has no 'q', so that glyph comes from Liberation Sans Bold, which has Arial's metrics (`tools/export_fonts.py`, `font_atlas.bin`, `font_metrics.h`). Tags and entities are stripped and lines wrap at spaces (`source/render/text_layout.c`).
- **Prompt.** The original "Press Z to Continue" (sprite 250) comes in one version per controller, with the "Z" replaced by that controller's read button from `tools/sign_buttons.py`. It fades in and pulses with the original's per-frame brightness (frames 80-135 loop), in the message box and at each picture page's own prompt position. Picture pages are now placed at their original stage position.
- **Other text.** Toad's talk cloud uses font 155 at size 15 in the original light yellow (255,255,179), left aligned like field 185. The hint bar, area title, completion screen and nozzle banner use the same font instead of the old block letters.

Tests:

- `text_layout_test`: HTML stripping, wrapping and the original fonts.
- `course_session_test` `test_message_box_frames`.
- `course_art_test`: the box and the three prompts.

## Toad dialogue (original talk cloud behaviour)

This follows Stats.tm's script (186) and Toad Main Clip's page logic (2517).

- **Fade-in.** The cloud fades in, easing a quarter of the way to full each frame. A page only starts typing once the cloud is above 98% opacity, then types two characters a frame.
- **Holding the read button.** Holding it shows the rest of the page at once (`KeyZ()` held, `GameInput.pound_held`). It never skips a page.
- **Turning the page.** The next page needs a read press (`BP()`), but only once the page is fully shown and its prompt has faded in to 98% (`MnEnd` with `tm.txt == ToadMessage && tm.bz._alpha >= 98`).
- **Between pages.** `ToadMessage` is empty for one frame, so the cloud dips briefly.
- **After the last page.** The cloud fades out with its text.
- **Prompt.** The prompt (bz, 184) was the keyboard "Z" key wobbling between two tilted poses. It is now the controller's read button in the same two poses (`TalkZ@<n>`), right-aligned with the key, and it fades in at half the remaining distance each frame.

Tests: `course_objects_test` `test_toad_dialogue_pages`, `controls_test` (held read button), `course_art_test`, `tests/test_picture_signs.py`.

## Controller picture signs (approved design)

`tools/controller_signs.py` builds the design approved in `docs/sign-design` from the original page renders.

- **Original art.** Keys that are separate shapes are removed from the render itself. These are the arrow and Z/X/C keys in 371, the stomp page's Z key 416, and the boxes' keys 417 in 424.
- **Reused pieces.** The diving Mario (bitmap 362) and the explosion (bitmap 412) are taken from the SWF.
- **Stomp demo.** The boxes show a mid-pound frame of Mario's ground-pound demo (424). Each frame is placed by snapping it to the box outline the page already shows.
- **Buttons.** All buttons come from `tools/sign_buttons.py`.

The pages are still single frames: animating every picture sign is a later item. Tests: `tests/test_picture_signs.py`.
