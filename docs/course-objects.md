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
| Goomba | GoombaCode 2386 | Full port: walk/turn at ledges and water, sees Mario (eye clip cycle) and hops, chases, hurts on touch (`MarioHit(1,90,-5,+/-7,7)`), stomp with the Squish clip hold and bounce (8, 10 with Jump held on contact, 12 when pressed during the hold), dive, spin, FLUDD spray and explosions (Enemyhurt) knock it tumbling, one bouncing coin per Goomba, respawn when 600 px off camera |
| Cheep Cheep | CheepCheepCode 2234 | Full port: idle swim with turns, chase within 80 px (rotating toward Mario), leap out of the water, bite (MarioHit, then 32 frames without chasing), bounce on ground, dive / spin / spray / explosion hits spin it away, "gone" clip drops one bouncing coin, respawn off camera |
| Bob-omb | BombombCode 3311 | Full port: patrols (ledge probe 100 px deep), lights its fuse on sight and chases faster, red fuse colour transform, explodes after 96 frames or when hurt (water, power-hit landing or wall), EnemyHurtBox blasts (35 x 50 when hurt, 30 x 30 from the fuse; 6 frames) that knock other enemies, explode clip hurts Mario `MarioHit(2,90,-7,+/-5,10,..,true)` (the mid-air explode2 clip has no hit box), touching shoves Mario `MarioHit(0,0,0,+/-3,0)`, cannot be stomped, one bouncing coin, respawn when its home is 600 px off camera |
| Koopa | 3403 | **Drawn only** (walk animation); behaviour next |
| Breakable box, FLUDD box, Water bottle, platforms | 2808, 972, 3995, 3315, 3476, 3063 | **Drawn only**; behaviour next |
| Shine Sprite | 977 | Not shown on a new save (frames reached only with Star[39]) |

Random numbers (`random()`, `Math.random()`) use a seeded generator per area.

## Mario attacks seen by enemies

Enemy scripts test Mario's charClip boxes: `body`/`feet` (Mario Main), the
Spin Attack clip's `water` (Blank 896 at 1.31 x 0.637) and the FLUDD stream
`water` (Wait WaterBlast 1157, frame = spray level 1-4, 12.7-43.7 px long,
pointing down in the Hover frames, its width mirrored every frame). The eye
clip 2385 cycles shapes 2381 / 2382 / 2383 and holds the larger 2384 while it
sees Mario (its random +/-10 degree tilt on frames 13-19 is not modelled).

## Mario damage

`course_mario_hit` follows `_root.MarioHit(a, b, c, d, e, f, g)`: lose `a` HP,
blink `b` frames (invulnerable, drawn every other pair of frames), no control
for `e` frames, hpcoincount reset. With `a > 0` the speed halves and (`d`, `c`)
replaces it (hurt1) or, when `g` is true, is added (hurt2/3); with `a == 0`
(`d`, `c`) is added and Mario's attack ends (the Bob-omb shove). At 0 HP the
original plays Faint and restarts the course from a life; the port restarts
the current area's entry with full health (lives are not modelled yet).
