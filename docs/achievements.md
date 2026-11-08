# Achievements, pause menu and save file

The port's 64 achievements (Bronze 30, Silver 20, Gold 13, Platinum 1), the
original pause menu with an Achievements page, the "Achievement Unlocked!"
card and the save file that keeps them.

## What can unlock today

Only the tutorial (8-E1-1 .. 8-E1-2-2) is in the port, so **only #1 A New
Adventure can be earned in normal play** (the tutorial's end: Intro Movie 2
sets `_root.Star[39]`). Every other trigger is wired to the original's own
progress variables and fires as soon as the port has the course, boss or
item that sets them. Three achievements have no matching event in the
original game and stay locked ("pending"); with them locked the Platinum
cannot be earned. They are kept as defined, not given invented conditions.

## Trigger map

Shine Sprite numbers are the original's `_root.Star[n]` (StarNameArray in
frame 2 of the 2010 SWF). Star Coins are `_root.StarCoin[n]`, keys
`_root.BowserKey1..3`. Counts use sets of unique collectibles, so collecting
one twice never counts twice.

| # | Tier | Name | Unlocks on |
|---|---|---|---|
| 1 | Bronze | A New Adventure | Star 39 "Mushroom Kingdom" |
| 2 | Bronze | Let There Be Light | **Pending** — the original has no lights-restored event or shine for this |
| 3 | Bronze | King of the Mountain | Star 1 "Battle with King Bob-omb" |
| 4 | Bronze | Head in the Clouds | Star 2 "Island in the Sky" |
| 5 | Bronze | Five Little Stars | Star 3 "The Five Silver Stars" |
| 6 | Bronze | Winter Wonderland | any shine of Star 6-10 (Snowmans Land) |
| 7 | Bronze | Ice Cold Victory | Star 8 "Chief Chilly" |
| 8 | Bronze | Ancient Treasure | Star 55 "Inside the Pyramid" |
| 9 | Bronze | The First Showdown | Star 40 "Bowser's Trap" |
| 10 | Bronze | Key to the Kingdom | _root.BowserKey1 |
| 11 | Bronze | Full Speed Ahead | Star 41 "Mushroom Kingdom 2" |
| 12 | Bronze | Into the Depths | any shine of Star 11-15 (Hazy Maze Cave) |
| 13 | Bronze | Toxic Trouble | Star 12 "The Deadly Toxic Maze" |
| 14 | Bronze | Garden Guardian | Star 50 "Peach's Garden" |
| 15 | Bronze | Ghost Hunter | any shine of Star 16-20 (Boo's Mansion) |
| 16 | Bronze | Boo Who? | Star 16 "Big Boo's Balcony" |
| 17 | Bronze | Underwater Explorer | any shine of Star 58-60 (Wet Dry World) |
| 18 | Bronze | Fortress Assault | entering Bowser's Second Trap (story milestone) |
| 19 | Bronze | Bowser Strikes Back | Star 42 "Bowser's Second Trap" |
| 20 | Bronze | Second Key Secured | _root.BowserKey2 |
| 21 | Bronze | Hot Stuff | any shine of Star 21-25 (Lethal Lava Land) |
| 22 | Bronze | Volcano Explorer | Star 22 "Into the Volcano" |
| 23 | Bronze | Magical Showdown | Star 51 "Kamek's Castle" |
| 24 | Bronze | Mountain Climber | Star 26 "Scale the Mountain" |
| 25 | Bronze | High Altitude | Star 27 "Silver Stars in the Skies" |
| 26 | Bronze | Against the Clock | any shine of Star 61-63 (Tick Tock Clock) |
| 27 | Bronze | Rainbow Traveler | any shine of Star 31-35 (Rainbow Ride) |
| 28 | Bronze | Mushroom Hunter | Star 44 "The Secret of the Mushroom" |
| 29 | Bronze | Deep Sea Diver | Star 46 "Jolly Roger Bay" |
| 30 | Bronze | First Steps to Greatness | 10 unique Shine Sprites |
| 31 | Silver | Battlefield Champion | every shine of Star 1-5 (Bob-omb Battle Field) |
| 32 | Silver | Master of the Snow | every shine of Star 6-10 (Snowmans Land) |
| 33 | Silver | Cave Conqueror | every shine of Star 11-15 (Hazy Maze Cave) |
| 34 | Silver | Ghostbuster | every shine of Star 16-20 (Boo's Mansion) |
| 35 | Silver | Lava Survivor | every shine of Star 21-25 (Lethal Lava Land) |
| 36 | Silver | Summit Master | every shine of Star 26-30 (Tall Tall Mountain) |
| 37 | Silver | Over the Rainbow | every shine of Star 31-35 (Rainbow Ride) |
| 38 | Silver | Desert Explorer | every shine of Star 55-57 (Sinking/Shifting Sand Land) |
| 39 | Silver | Water World | every shine of Star 58-60 (Wet Dry World) |
| 40 | Silver | Clockwork Precision | every shine of Star 61-63 (Tick Tock Clock) |
| 41 | Silver | Red Coin Collector | 5 shines of red-coin missions: Star 4, 9, 14, 19, 24, 29, 34, 37, 56, 59, 62 |
| 42 | Silver | Silver Star Seeker | 3 shines of Silver Star missions: Star 3, 23, 27, 54, 60 |
| 43 | Silver | Secret Passage | Star 48 "Secret of the Sky" |
| 44 | Silver | Frozen Speed | Star 47 "Frosty Fludd" |
| 45 | Silver | Castle Crusher | Star 49 "Thwomp's Castle" |
| 46 | Silver | Puzzle Solver | Star 52 "The Cave of Empuzzlement" |
| 47 | Silver | Fire and Water | Star 53 "The Magma Maze" |
| 48 | Silver | Across the Galaxy | Star 54 "A Galaxy of Stars" |
| 49 | Silver | Treasure Hunter | 32 unique Star Coins |
| 50 | Silver | Halfway to the Sun | 32 unique Shine Sprites |
| 51 | Gold | The Final Showdown | Star 36 "The Final Battle" |
| 52 | Gold | The Third Key | _root.BowserKey3 |
| 53 | Gold | Kingdom Saved | the ending (story milestone) |
| 54 | Gold | The Secret Edge | **Pending** — no course of that name exists in the original's course or star lists |
| 55 | Gold | Red Coin Royalty | **Pending** — the original has one Bowser red-coin Shine Sprite (37, Bowser's Castle Red-Coins), not three |
| 56 | Gold | Castle Secrets Revealed | every shine of Star 44-54 (secret courses) |
| 57 | Gold | Master of F.L.U.D.D. | every shine of Star 39 + Star 41 + Star 47 |
| 58 | Gold | The 16-Star Coin Prize | Star 38 "The 16 Star Coin Prize" |
| 59 | Gold | The 48-Star Coin Prize | Star 43 "The 48 Star Coin Prize" |
| 60 | Gold | Star Coin Legend | 64 unique Star Coins |
| 61 | Gold | Shine Sprite Legend | 64 unique Shine Sprites |
| 62 | Gold | The Ultimate Adventure | all 64 shines, all 64 Star Coins, all 3 keys |
| 63 | Gold | The Power of 64 | all 64 unique shines and 64 unique Star Coins in one slot |
| 64 | Platinum | Super Mario 63 | achievements 1-63 |

Notes on the checks against the original:

* "Shifting Sand Land" is the name in the pyramid shine's description; the
  course is "Sinking Sand Land" elsewhere in the original. Stars 55-57.
* #41 counts the 11 shines whose missions are about 8 Red Coins; #42 the 5
  whose descriptions are about Silver Stars.
* #2: the original has no "lights restored" event or shine. #54: no course
  named "Edge of the Mushroom Kingdom" exists in the course or star lists.
  #55: the original has one Bowser red-coin shine (37), not three.
* #18 and #53 are story milestones (`ach_milestone`) to be raised by the
  Bowser's Second Trap entrance and the ending when those exist.

## Code

* `source/game/achievements.c` / `include/achievements.h`: definitions and
  the manager. Events (`ach_shine`, `ach_star_coin`, `ach_key`,
  `ach_milestone`) check only the kinds of achievement that event can change;
  the Platinum is checked once after any unlock. Unlocks go into a queue
  (16) in unlock order. `ach_evaluate_all` runs once after loading a save.
* `source/game/ach_notice.c`: the card's timing (0.35 s in, 5 s, 0.35 s out,
  then the next queued one) and the sound, once per card.
* `source/game/pause_menu.c`: the original pause menu 795 / 794 (open and
  close animation, button easing, page-change clip 661, pages default / Star
  Map / Fludd / Options / Exit) driven by the controller, plus the port's
  Achievements button and page. While it is open the game stands still.
* `source/platform/course_render.c`: draws the menu (`draw_pause`), the
  Achievements page and the card (`draw_achievement_card`).
* `source/game/save_data.c`, `source/platform/save_io.c`: the save file.
* Art: `tools/pause_art.py` (pages and parts from the original SWF, plus the
  Achievements button in the buttons' style), `tools/export_trophies.py`
  (the four cups from `assets/art/trophies.png`). Sound:
  `assets/audio/achievement_ping.wav`, through `tools/export_audio.py`
  (mono, 22 050 Hz, peak -9 dBFS, played at 70 % on an effect voice: the
  music keeps playing).

## Controls in the menu

| | GameCube | Classic | Wii Remote (sideways) |
|---|---|---|---|
| Open / close | START | + | + and − together |
| Move | D-pad | D-pad | Control Pad |
| Choose | A | b | 2 |
| Back | B | y | 1 |

## Save format

`sd:/sm63wii/save.dat` (developer builds: `devsave.dat`). Big-endian,
212 bytes:

| Offset | Size | Contents |
|---|---|---|
| 0 | 8 | `SM63WIIS` |
| 8 | 2 | version (1) |
| 10 | 2 | slot count (3, the original's three files) |
| 12 | 4 | flags: bit 0 music muted; bits 8-15 the slot being played |
| 16 | 64 x 3 | slots: +0 used, +1 Bowser keys (bit k-1), +4 milestones, +8 shines (bit n-1 = Star[n]), +16 Star Coins, +24 achievements unlocked (bit id-1), +32 32 reserved bytes |
| end | 4 | CRC-32 of everything before it |

A write goes to `save.tmp`, is read back and checked, then the old file
becomes `save.bak` and the new one `save.dat`; loading falls back to
`save.bak`. A file with a bad CRC or a version of 256 or more is ignored.
Newer minor versions load (unknown fields stay zero), and the reserved bytes
leave room for new fields without changing the size. IDs 1-64 are fixed:
never renumber them.

The game writes when the tutorial ends (the original's `SaveFile()`), when
Mute Music changes, and within two seconds of an unlock, never every frame.
On loading, achievements the stored progress already proves are awarded
(and announced); nothing is awarded for events the file does not record.

## Developer test build

`make DEV=1` builds the same game with tests on the Achievements page (pause
menu > Achievements). It saves to `devsave.dat`, so the real save is never
changed:

* jump (A / b / 2) on a row: unlock it (card, sound, queue; the Platinum
  follows the 63rd);
* nozzle button (L / ZL / A): collect all 64 Shine Sprites, 64 Star Coins and
  the three keys through the game's events (#30, #31-40, #49-52, #56-63 ...);
* stomp (Z / L or R / A + B): clear the dev save's progress.

## Tests

`tests/achievements_test.c` (definitions, queue order, duplicates, counts,
keys, The Power of 64, Platinum once, existing-save evaluation, save format
round trip and damage), `tests/course_session_test.c` (pause menu open /
navigation / pages / close, the game standing still, START refused during a
death, the card queue and one sound per card), `tests/test_pause_art.py`.
