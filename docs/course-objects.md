# Tutorial objects

`tools/export_object_art.py` renders each object's animation clips from the
pinned SWF (SPEC table: sprite, frames, placement within the object), removes the
invisible Blank Movieclip hit boxes, and packs them as art pack v3 (GX RGB5A3,
LZ4) in `assets/generated/objects_8e1_*.bin`, with clip ids in
`object_art_ids.h`. `source/game/course_objects.c` runs the objects at 32 Hz;
`source/render/object_select.c` picks the clip frame; `course_render.c` draws
BackGFX objects behind Mario and FrontGFX objects in front.

| Object | Source script | Status |
|---|---|---|
| Gold / Blue Coin | GoldCoin 963, BlueCoin 3495 | Spin, collect (+1 / +5 coins, hpcoincount +1 / +15), heal 1 HP per 5 (DOMDocument.xml:5151), stay collected per area (`coindata`) |
| Coded Sign | CodedSign 986 | Drawn; Z prompt ('aa') eases in when readable; reading in course_session |
| Toad | Toad Main Clip 2517 | Idle animation, Z prompt ('bz'), pages M1..M6 until an undefined `MnTxt`, faces Mario, camera on Toad. Not yet: mood faces, typewriter text, zoom to 150 |
| Butterfly | ButterflyCode 2324 | Wander and wave as scripted (colour variant frames not yet) |
| Goonie (bird) | GoonieCode 2305 | Fly / dive / ride states, rideable GooniePL platform with carry and wrap |
| Falling log | Falling Log Code 3317 | Invisible BPlatforms log (SWF 57): fade-in, 10-frame quake, fall, carry, reset after 24; graphic stays, as in the original |
| YI Arrow | 3410 | Animates; fades out when the barrier opens |
| Goomba, Cheep Cheep, Bob-omb, Koopa | 2386, 2234, 3311, 3403 | **Drawn only** (idle animation); behaviour next |
| Breakable box, FLUDD box, Water bottle, platforms | 2808, 972, 3995, 3315, 3476, 3063 | **Drawn only**; behaviour next |
| Shine Sprite | 977 | Not shown on a new save (frames reached only with Star[39]) |

Random numbers (`random()`, `Math.random()`) use a seeded generator per area.
