# Tutorial audio

`tools/export_audio.py` takes the original sounds by linkage name from the
pinned SWF (DefineSound MP3), decodes them with ffmpeg at their own rate (mono),
trims them to the DefineSound sample count and stores them IMA ADPCM encoded in
`assets/generated/sounds_8e1.bin` (about 1 MB). On the Wii,
`source/platform/audio.c` decodes the pack once into MEM2 (about 4 MB of 16-bit
PCM) and plays it with ASND.

The simulation only queues requests (`source/game/sfx.c`); the platform drains
them once per frame. Volumes and pans follow the scripts: Mario's sounds at
MarioVolume 100; enemy sounds use the scripts' `distancevolume`
(`100 - min(100, distance / 100 * 15)`, 20 for the Bob-omb's hurt explosion)
and `panX = (x - camX) / 2`.

| Sound | Trigger (original script) |
|---|---|
| PrincessPeachesCastle-Intro, then -Repeat looping | `StartChar` with `resetmusic` (new game in 8-E1-1); warps keep the music |
| CoinJingle | GoldCoin / BlueCoin frame 4 |
| Mario Jump-1 / Jump-1b (random), Jump-2, Tripple Jump | CharCode jump chain |
| Mario-Dive | dive start |
| Mario-Step, step - Water (puddle), step-wetwood (Tipping Log, 3 frames) | Walk.xml / Running.xml when `frame` enters 11-20 or 51-60 |
| sound - Splash | entering water faster than 7, jumping out of the surface |
| sound - Water | each swim stroke |
| Mario-Fall | long fall (`hurtfallcount` reaches 5) |
| Mario - Doh / Uh (random), Mario-Squish (Bob-omb blast), Mario - Ohhhhh (0 HP) | `_root.MarioHit` |
| Goomba-Attention, Goomba-Stomped | GoombaCode |
| fuse-lite, fuse-loop, Explosion | BombombCode (fuse-loop at 13, 50 and 87 frames, the lengths of fuse-lite and fuse-loop) |
| flippers | Koopa stomps |
| boxbreak | Breakable Orange Box frame 4 |
| Nozzle Box Break, Fludd Nozzle | FLUDD box, Hnozzle pickup, Water Bottle |
| fluddrepeat (looping, volume 1/1.5) | Char frame 1 while FLUDD sprays |

Not yet: FluddSwitch (no nozzle switching in the tutorial), the Goomba walk
sounds, Mario's idle voices, Toad/sign sounds.
