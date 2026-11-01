# HUD (Stats Clip, SWF 800)

The HUD is drawn from the original Stats Clip art, exported by
`tools/export_object_art.py` at 1.6 screen px per stage px (the stage is
450 x 300; 300 px fill the 480-line screen):

| Part | Source | Port |
|---|---|---|
| Shine and star coin icons with "=" | Stats Clip frame 1 (dynamic parts stripped) | `OBJ_HUD_STATIC`, left edge |
| Counters (`_root.TotalStars`, `TotalStarCoins`, `Coins`) | DefineEditText, font 187 (Arial 16 px), black 50% copy 2 px down-right | `OBJ_HUD_DIGITS` (0-9 and "="), rendered from the exported font |
| Coin icon | GoldCoin spin (SWF 17) at (342.6, 10.3) | `OBJ_COIN_SPIN`, right edge |
| POWER meter | 114 (slide) + 111 (pie, frame = CharHP + 1, eased 0.3 a frame) | `source/game/hud.c` follows 114's script: shown while CharHP changes, slides away after 160 frames at 8 HP, back in (fast when hurt, "slower" when healed) |
| FLUDD tank | 160 (only with FLUDD): `cl` level (149, frame = Waterpercent x 5), power gauge (153), percentage | `OBJ_HUD_TANK`, `_LEVEL` (every 10th frame), `_POWER_BAR` (every 5th), digits at 15/16 size |
| Toad message | `tm` (186): cloud, text typed 2 characters a frame, Z prompt (`bz`) when complete | `OBJ_HUD_TALK_BOX`, `OBJ_HUD_TALK_Z`; the text uses the port's bitmap font |

On 16:9 the parts left of stage x 150 stay at the left edge, those right of
x 300 at the right edge and the POWER meter at the centre.

Hidden in the tutorial, as in the original: lives (237 shows frame 2 while
`TotalStars == 0`), red coins / silver stars (232 frame 4 when both are 0).
Not modelled: the WaterHP (drowning) labels of the POWER meter.
