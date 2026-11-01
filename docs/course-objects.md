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
| Butterfly | ButterflyCode 2324 | Wander and wave as scripted; one of the six colour frames (the clip colour transforms) |
| Goonie (bird) | GoonieCode 2305 | Fly / dive / ride states, rideable GooniePL platform with carry and wrap |
| Falling log | Falling Log Code 3317 | Invisible BPlatforms log (SWF 57): fade-in, 10-frame quake, fall, carry, reset after 24; graphic stays, as in the original |
| YI Arrow | 3410 | Animates; fades out when the barrier opens |
| Goomba | GoombaCode 2386 | Full port: walk/turn at ledges and water, sees Mario (eye clip cycle) and hops, chases, hurts on touch (`MarioHit(1,90,-5,+/-7,7)`), stomp with the Squish clip hold and bounce (8, 10 with Jump held on contact, 12 when pressed during the hold), dive, spin, FLUDD spray and explosions (Enemyhurt) knock it tumbling, one bouncing coin per Goomba, respawn when 600 px off camera |
| Cheep Cheep | CheepCheepCode 2234 | Full port: idle swim with turns, chase within 80 px (rotating toward Mario), leap out of the water, bite (MarioHit, then 32 frames without chasing), bounce on ground, dive / spin / spray / explosion hits spin it away, "gone" clip drops one bouncing coin, respawn off camera |
| Bob-omb | BombombCode 3311 | Full port: patrols (ledge probe 100 px deep), lights its fuse on sight and chases faster, red fuse colour transform, explodes after 96 frames or when hurt (water, power-hit landing or wall), EnemyHurtBox blasts (35 x 50 when hurt, 30 x 30 from the fuse; 6 frames) that knock other enemies, explode clip hurts Mario `MarioHit(2,90,-7,+/-5,10,..,true)` (the mid-air explode2 clip has no hit box), touching shoves Mario `MarioHit(0,0,0,+/-3,0)`, cannot be stomped, one bouncing coin, respawn when its home is 600 px off camera |
| Koopa (Paratroopa) | KoopaCode 3403 | Full port: Flying hovers in place (wings clip), stomp knocks the wings off (Mario `yspeed/2 - 15`), Walk patrols with the Turn clip (frame 15 flips direction), stomp again for the Shell (`yspeed/2 - 12`), shells are kicked by touch or a stomp, slide (friction 0.99), float in water, bounce off walls and blast (EnemyHurtBox 15 x 15) while faster than 1; dive / spin / spray / explosions turn it into a shell; touching a Flying or Walking Koopa hurts Mario |
| Breakable Orange Box | 2808 | Solid OrangeBlockPL; ground pound (Mario keeps falling at `yspeed*0.75-3`), a hard run (HitWall bonk), Spin or a blast breaks it; frame 5 drops `stuffinside` coins (5 / 10 with the original launch speeds, once per box); reforms when the camera and Mario are 600 px away. Not modelled: breaking while knocked back (`box2` / hurt2) |
| FLUDD box (Squirt Nozzle Box Clip) | HoverFluddBox 972 | Landing on it bounces Mario (`yspeed = -6`), breaks it and drops the Hnozzle (42); touching the nozzle gives FLUDD Hover and a full tank; the box reforms after 300 frames, the nozzle fades at 900 and goes at 1000 |
| Water Bottle | 3995 | +5000 / 10000 water (half a tank), reappears when off camera or after 960 frames |
| Green Circle Platform | 3315 + GreenPLcode 2103 | `pl` circles the centre (`speed` degrees a frame, `distance`), path dots every `dotdistance` degrees, one-way platform that carries Mario by its step |
| Tipping Log | 3476 + LogPL 55 | Mario's weight and position tip it (rotspeed rules from the script), up to 80 degrees, one-way rotated plank |
| Rotating Block Plat | 3063 + code clip 7527 | Solid square (scale %) that quakes for 20 frames then turns 90 degrees every `rotwait`; pushes Mario standing inside the turn. Travel (`speedX/Y`) is not used by the tutorial and not modelled |
| Camera Control Box | Camera Control Box | Invisible; eases `CamXchangeVar`/`CamYchangeVar` by `amount` over `speed` while Mario's body is inside; with boxes present the camera uses `CamYchange = var + 50`, `CamXchange = var` (8-E1-1-2, 8-E1-2; the 8-E1-2-2 boxes are on its post-completion frame 5) |
| Shine Sprite | 977 | Not shown on a new save (frames reached only with Star[39]) |

Random numbers (`random()`, `Math.random()`) use a seeded generator per area.

Not yet drawn: the 8-E1-2 `Giant Water Block` texture scroll (a 132-frame
motion tween of the water pattern); the water volume itself works.

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
