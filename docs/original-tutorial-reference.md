# Original tutorial reference: "Mushroom Kingdom (Tutorial)" (Tutorial Star, course 8-E1)

Source: Runouw/Super-Mario-63 `83e81fb6b80d11c4119e420d25298000a6e13f66`,
`Super_Mario_63_2010/` (XFL) and `Publish/Super_Mario_63_2010.swf`
(SHA-256 `6f8dc22d...e1c`). Paths below are relative to `Super_Mario_63_2010/`.
Every value here was read from those files with `tools/research/xfl_inspect.py`
and `tools/research/swf_course.py`; nothing is estimated unless marked.

## 1. Identity and entry

| Item | Value | Source |
|---|---|---|
| Name shown | "Mushroom Kingdom" / "*(Tutorial)*" | `8-E1-1 Back GFX` frame 3: `AreaTextClipF(...)` |
| Star | #39 "A: Tutorial Star (8-E1)" | `DOMDocument.xml:10804`; `Starnamenum = 39` in each area |
| Course ids | `8-E1-1`, `8-E1-1-2`, `8-E1-2`, `8-E1-2-2` | linkage ids `<id>BackGFX`, `<id>Platforms`... |
| Library folders | `FiRsT Tutoria` (8-E1-1, 8-E1-1-2), `Huvr Tuotrlai` (8-E1-2, 8-E1-2-2) | `LIBRARY/Course Clips/8 - Storyline Stars/8 - Tutorials/` |
| How a new game reaches it | Intro Movie last frame: `_root.StartChar("8-E1-1", 0, 0, 0, 0, 100, CurrentPlayer, "", "StarIn", true)` | `LIBRARY/Menu/Movies/Intro Movie.xml:225` |
| Spawn | Course origin (0,0); `Fluddpow = ""` (**no FLUDD**); transition `StarIn`; music reset | `StartChar`, `DOMDocument.xml:4889` |

The first tutorial is the whole event course **8-E1**: four areas joined by
warps, ending at a warp pipe whose first use plays Intro Movie 2, which awards
star 39 (section 10). The library splits it into a movement tutorial
(8-E1-1, 8-E1-1-2) and a Hover FLUDD tutorial (8-E1-2, 8-E1-2-2); there is no
star or exit between them.

## 2. How a course is built (`StartChar`, `DOMDocument.xml:4889-5060`)

`StartChar(a, b, c, d, e, zoom, player, fluddpow, transition, resetmusic)` attaches,
by linkage id `a + layer`: `FarBackground` (static), `Background` (`-b/10, -c/10`),
`MidBackground` (`/5`), `CloseBackground` (`/2`), then in `Course`: `Puddle`, `Ice`,
`BackGFX`, `Platforms` (invisible, solid), `BPlatforms` (invisible), `Char` at
(`b-d`, `c-e`) negated, `Water`, `Lava` (invisible), `FrontGFX`, `Edge`, `CamEdge`
(alpha 0). Collision is Flash `hitTest(x, y, true)` against these vector shapes.
`changecourse(transition, id, b, c, d, e)` swaps areas through a transition clip.

Per-course tables (`DOMDocument.xml`):

| Course | Background | Far background | Close background | Music |
|---|---|---|---|---|
| 8-E1-1 | `CastleColoredBG` | `CastleColoredFarBG` | `8-E1-1CloseBG` | `PrincessPeachesCastle-Intro` then `-Repeat` (11362) |
| 8-E1-1-2 | `CastleColoredBG` | `CastleColoredFarBG` | `8-E1-1CloseBG` | none set: keeps playing |
| 8-E1-2 | `CastleColoredBG` | `CastleColoredFarBG` | `8-E1-2CloseBG` | none set |
| 8-E1-2-2 | (none) | `CastleColoredFarBG` | `8-E1-2-2CloseBG` | `PrincessPeachesCastle` (11364) |

Coin data `_root["coindata8-E1-…"] = [""]` (`DOMDocument.xml:11127`): coins are
placed instances, below. Colour: every area's `Color Back/Front GFX` tints by
`TotalStars` (desaturated grey at 0 stars, the state on a new game).

## 3. Layer sprites and world bounds (SWF, course pixels)

| Course | Layer | SWF id | Frames | Bounds (x0, y0, x1, y1) |
|---|---|---:|---:|---|
| 8-E1-1 | Platforms | 6262 | 2 | (-239.8, -445.9, 1921.8, 353.2) |
| 8-E1-1 | BPlatforms | 6264 | 1 | (160.9, -16.8, 1181.2, 107.5) |
| 8-E1-1 | BackGFX | 6254 | 4 | (-133.5, -175.1, 1798.6, 337.8) |
| 8-E1-1 | FrontGFX | 6260 | 1 | (-233.9, -160.2, 1987.3, 359.5) |
| 8-E1-1 | Water | 6255 | 1 | script-generated |
| 8-E1-1 | CamEdge | 6269 | 1 | (-435.9, -556.0, 2045.4, 586.1) |
| 8-E1-1 | Ice | 6266 | 1 | (1639.2, -20.4, 1686.0, 36.0) |
| 8-E1-1-2 | Platforms | 6287 | 1 | (-184.5, -411.9, 1806.3, 310.9) |
| 8-E1-1-2 | BPlatforms | 6289 | 1 | (945.8, -96.0, 1663.8, 108.5) |
| 8-E1-1-2 | BackGFX | 6278 | 3 | (72.5, -142.9, 1720.0, 292.5) |
| 8-E1-1-2 | FrontGFX | 6281 | 1 | (-104.3, -185.8, 1825.2, 392.5) |
| 8-E1-1-2 | Water | 6285 | 1 | (474.3, -101.0, 1328.3, 409.1) |
| 8-E1-1-2 | CamEdge | 6291 | 1 | (-303.4, -571.1, 2105.2, 571.1) |
| 8-E1-2 | Platforms | 6304 | 1 | (-68.1, -510.6, 1986.9, 44.0) |
| 8-E1-2 | BPlatforms | 6306 | 1 | (267.9, -381.6, 1460.5, -48.0) |
| 8-E1-2 | BackGFX | 6296 | 3 | (-11.8, -384.6, 1842.3, 9.6) |
| 8-E1-2 | FrontGFX | 6302 | 1 | (-51.0, -482.4, 1891.4, 165.9) |
| 8-E1-2 | Water | 6307 | 1 | (698.1, -498.4, 2421.7, 473.4) |
| 8-E1-2 | CamEdge | 6298 | 1 | (-243.4, -723.2, 2241.2, 279.9) |
| 8-E1-2-2 | Platforms | 6319 | 1 | (-1269.7, -1173.1, 1242.7, 201.1) |
| 8-E1-2-2 | BPlatforms | 6321 | 1 | (-928.9, -859.4, 657.1, -49.9) |
| 8-E1-2-2 | BackGFX | 6313 | 5 | (-1275.0, -927.9, 927.4, 25.2) |
| 8-E1-2-2 | FrontGFX | 6317 | 1 | (-1210.0, -865.7, 1239.5, 297.6) |
| 8-E1-2-2 | CamEdge | 6310 | 1 | (-1437.4, -1214.3, 1262.2, 494.1) |

`8-E1-1Platforms` (library `8-E1-1 Ground`) has **two frames**: frame 1 includes a
barrier left of the path; its script keeps frame 1 only when `TotalStars == 0` and
Mario is left of the start sign, else frame 2. Reading the "controls" sign calls
`_root.Course.Platforms.gotoAndStop(2)`, removing the barrier. `BPlatforms` is the
second solid layer (`BackGFX` hitTests both). Water volumes come from `Water Code`
instances (`WaterLength` wide, `WaterDeepness` deep, default 400; spring-wave
surface after Jim Bumgardner's "Ocean"). Ice: one area in 8-E1-1.

Not yet extracted as geometry: the vector outlines themselves (needed for
collision). Plan: render each `Platforms`/`BPlatforms` frame from the SWF at 1:1
into a collision mask (section 11).

## 4. Background art and parallax

Far/Background/Close backgrounds above, parallax 0, 1/10, 1/5 (Mid, unused here)
and 1/2 of camera motion (`StartChar`). Course art: `BackGFX` (behind Mario) and
`FrontGFX` (in front), each with bitmap fills and nested decoration clips.

## 5. Decorations and environmental animation

`Goonie Code` (flying birds, `_xscale` per instance) and `Butterfly Code` in every
area; `Front GFX` frame scripts play random `Birdy1..6` chirps at
`MarioVolume/5 * TotalStars/64` (silent at 0 stars unless `CheatCastleRestore`).
`YI Arrow Animation` in 8-E1-1 fades out once the barrier opens.

## 6-9. Objects, enemies, signs, hints, triggers and collectibles

Generated from the XFL (instance positions are in that area's `BackGFX`/`FrontGFX`
space, which `StartChar` places at the course origin). Frames are 1-based.

Summary: gold coins 18/13/24/33 (88), blue coins 1/1/0/0, Goombas 3/3/2/2,
Bob-ombs 0/3/0/1, Koopas 0/0/1/2, Cheep Cheeps 4/2/3/0, Toads 1/0/1/1, signs
4/3/2/4, camera control boxes 0/2/4/6, plus the FLUDD box (8-E1-2), breakable
boxes, falling log, tipping logs, rotating and circling platforms, water
bottles and the Shine Sprite (8-E1-2-2).

### 8-E1-1

| Layer | Object | Instance | x | y | Parameters |
|---|---|---|---:|---:|---|
| Back GFX / Layer 5 (frame 3) | Blank Movieclip | warp1 | 1855.8 | -126.0 |  |
| Back GFX / Layer 5 (frame 3) | Blank Movieclip |  | 287.2 | 53.0 | mess="<b>No Mario!  Go Left and read that Sign!</b>" |
| Back GFX / Layer 5 (frame 3) | Blank Movieclip |  | 750.5 | 44.9 | mess="(Hint:  Jumping twice in a row makes you jump higher)" |
| Back GFX / Layer 5 (frame 3) | Blank Movieclip |  | 1408.0 | -13.0 | mess="(Hint:  If you get hurt, collect 5 coins to heal 1 health)" |
| Back GFX / Layer 5 (frame 4) | Blank Movieclip | warp1 | 1743.8 | -126.0 |  |
| Back GFX / Layer 5 (frame 4) | Blank Movieclip | warp2 | -207.0 | -10.0 |  |
| Back GFX / Coins | Gold Coin Code |  | 177.0 | 77.8 |  |
| Back GFX / Coins | Gold Coin Code |  | 207.0 | 76.8 |  |
| Back GFX / Coins | Gold Coin Code |  | 237.0 | 72.8 |  |
| Back GFX / Coins | Gold Coin Code |  | 267.0 | 67.8 |  |
| Back GFX / Coins | Gold Coin Code |  | 465.1 | 47.0 |  |
| Back GFX / Coins | Gold Coin Code |  | 495.1 | 54.0 |  |
| Back GFX / Coins | Gold Coin Code |  | 525.1 | 60.0 |  |
| Back GFX / Coins | Gold Coin Code |  | 555.1 | 65.0 |  |
| Back GFX / Coins | Gold Coin Code |  | 585.1 | 69.0 |  |
| Back GFX / Coins | Gold Coin Code |  | 615.1 | 71.0 |  |
| Back GFX / Coins | Gold Coin Code |  | 645.1 | 70.0 |  |
| Back GFX / Coins | Gold Coin Code |  | 919.5 | 2.0 |  |
| Back GFX / Coins | Gold Coin Code |  | 939.5 | -20.0 |  |
| Back GFX / Coins | Gold Coin Code |  | 959.5 | -37.0 |  |
| Back GFX / Coins | Gold Coin Code |  | 979.5 | -44.0 |  |
| Back GFX / Coins | Gold Coin Code |  | 999.5 | -37.0 |  |
| Back GFX / Coins | Gold Coin Code |  | 1019.5 | -20.0 |  |
| Back GFX / Coins | Gold Coin Code |  | 1039.5 | 2.0 |  |
| Back GFX / Coins | Blue Coin Code |  | 1586.9 | -15.0 |  |
| Back GFX / Coins | Blank Movieclip | target3 | 1410.9 | -74.1 |  |
| Back GFX / Coins | Toad Main Clip |  | 689.5 | 71.8 | Toad dialogue (6 pages) |
| Back GFX / Coins | Coded Sign |  | 416.0 | 67.8 | signmessage="You can run by double tapping Right or Left."+newline+"In the air ... |
| Back GFX / Enemies | Goomba |  | 1338.0 | 23.8 |  |
| Back GFX / Enemies | Goomba |  | 1501.9 | 37.8 |  |
| Back GFX / Enemies | Goomba |  | 1542.9 | 38.0 |  |
| Back GFX / Enemies | Butterfly Code |  | 385.4 | 29.4 |  |
| Back GFX / Enemies | Butterfly Code |  | 1185.5 | -40.5 |  |
| Back GFX / Enemies | Cheep Cheep Code |  | 1014.1 | 127.3 | scale=1; chasespeed=0.3; idlespeed=2 |
| Back GFX / Enemies | Cheep Cheep Code |  | 936.0 | 261.9 | scale=1; chasespeed=0.3; idlespeed=2 |
| Back GFX / Enemies | Cheep Cheep Code |  | 620.0 | 206.9 | scale=1; chasespeed=0.3; idlespeed=2 |
| Back GFX / Enemies | Cheep Cheep Code |  | 253.5 | 174.3 | scale=1; chasespeed=0.3; idlespeed=2; hungry=false |
| Back GFX / Layer 1 | Coded Sign |  | 1133.5 | -18.2 | signmessage="goomba"; picture=true; othercamcenter=_root.Course.BackGFX.target3 |
| Back GFX / Layer 1 | Coded Sign |  | 36.0 | 95.0 | signmessage="Start"; picture=true; othercamcenter=_root.Course.BackGFX["sign2center"+depth] |
| Back GFX / Layer 1 | Coded Sign | sign2 | 146.0 | 95.0 | signmessage="controls"; picture=true; on read: _root.Course.Platforms.gotoAndStop(2); |
| Back GFX / Layer 1 | Falling Log Code (graphics) |  | 939.5 | 35.5 |  |
| Front GFX / Goonies | Goonie Code |  | -102.3 | -28.4 |  |
| Front GFX / Goonies | Goonie Code |  | 1569.1 | -1.2 |  |
| Front GFX / Goonies | Goonie Code |  | 379.5 | 33.1 |  |
| Front GFX / Goonies | Goonie Code |  | 639.5 | 79.7 |  |
| Front GFX / Goonies | Goonie Code |  | 1050.5 | 29.7 |  |
| Front GFX / Goonies | Goonie Code |  | 580.5 | 62.5 |  |
| Front GFX / Goonies | Goonie Code |  | 1407.0 | 68.8 |  |
| Front GFX / Goonies | Goonie Code |  | 215.5 | -24.1 |  |
| Front GFX / Goonies | Butterfly Code |  | -21.5 | 53.9 |  |
| Front GFX / Goonies | Butterfly Code |  | 143.6 | 29.4 |  |
| Front GFX / Goonies | Butterfly Code |  | 759.7 | -17.6 |  |
| Front GFX / Goonies | Butterfly Code |  | 1308.8 | -1.7 |  |
| Front GFX / Goonies | Butterfly Code |  | 1543.8 | -34.5 |  |
| Front GFX / Goonies | Butterfly Code |  | 1884.0 | -76.5 |  |
| Front GFX / Back | YI Arrow Animation |  | 146.3 | -10.7 |  |
| Water / Layer 9 | Water Code |  | 941.0 | 158.8 | WaterLength=100; polygons=11 |
| Water / Layer 4 | Water Code |  | 448.0 | 158.8 | WaterLength=410; polygons=40 |
| Water / Layer 3 | Water Code |  | 169.7 | 158.8 | WaterLength=120; polygons=14 |

### 8-E1-1-2

| Layer | Object | Instance | x | y | Parameters |
|---|---|---|---:|---:|---|
| Back GFX / Code (frame 3) | Blank Movieclip | warp1 | -72.0 | -78.1 |  |
| Back GFX / Code (frame 3) | Blank Movieclip | warp2 | 1783.2 | -129.2 |  |
| Back GFX / Code (frame 3) | Camera Control Box |  | 716.5 | 18.4 | t="Y"; amount=-50; speed=20 |
| Back GFX / Code (frame 3) | Camera Control Box |  | 1080.4 | 210.4 | t="Y"; amount=50; speed=20 |
| Back GFX / Code (frame 3) | Blank Movieclip |  | 882.5 | 198.7 | mess="(Hint: Press <b>UP</b> to swim upwards)" |
| Back GFX / Layer 2 | Coded Sign |  | 89.0 | 18.7 | signmessage="bomb"; picture=true; othercamcenter=_root.Course.BackGFX.target2 |
| Back GFX / Layer 2 | Coded Sign |  | 1568.0 | 73.2 | signmessage="ttt"; picture=true |
| Back GFX / Layer 2 | Bomb omb Code |  | 282.5 | 16.6 |  |
| Back GFX / Layer 2 | Bomb omb Code |  | 1236.5 | 87.8 |  |
| Back GFX / Layer 2 | Bomb omb Code |  | 450.5 | 25.6 |  |
| Back GFX / Layer 2 | Goomba |  | 1549.0 | 70.2 |  |
| Back GFX / Layer 2 | Goomba |  | 1590.0 | 70.4 |  |
| Back GFX / Layer 2 | Goomba |  | 1303.0 | 89.4 |  |
| Back GFX / Layer 2 | Butterfly Code |  | 1304.0 | 36.1 |  |
| Back GFX / Layer 2 | Gold Coin Code |  | 1056.7 | 63.1 |  |
| Back GFX / Layer 2 | Gold Coin Code |  | 1096.7 | 70.2 |  |
| Back GFX / Layer 2 | Gold Coin Code |  | 1136.7 | 72.2 |  |
| Back GFX / Layer 2 | Blue Coin Code |  | 1660.5 | -59.6 |  |
| Back GFX / Layer 2 | Gold Coin Code |  | 229.6 | -6.0 |  |
| Back GFX / Layer 2 | Gold Coin Code |  | 268.6 | -6.0 |  |
| Back GFX / Layer 2 | Gold Coin Code |  | 307.6 | -6.0 |  |
| Back GFX / Layer 2 | Gold Coin Code |  | 346.6 | -6.0 |  |
| Back GFX / Layer 2 | Gold Coin Code |  | 385.6 | -6.0 |  |
| Back GFX / Layer 2 | Blank Movieclip | target2 | 349.1 | -74.5 |  |
| Back GFX / Layer 2 | Breakable Orange Box |  | 710.5 | 93.2 | stuffinside="10coins" |
| Back GFX / Layer 2 | Breakable Orange Box |  | 710.5 | 125.2 | stuffinside="5coins" |
| Back GFX / Layer 2 | Gold Coin Code |  | 816.5 | 202.4 |  |
| Back GFX / Layer 2 | Gold Coin Code |  | 866.5 | 202.4 |  |
| Back GFX / Layer 2 | Gold Coin Code |  | 916.5 | 202.4 |  |
| Back GFX / Layer 2 | Gold Coin Code |  | 966.5 | 202.4 |  |
| Back GFX / Layer 2 | Gold Coin Code |  | 1016.5 | 202.4 |  |
| Back GFX / Layer 2 | Coded Sign |  | 665.5 | 56.2 | signmessage="stomp"; picture=true; othercamcenter=_root.Course.BackGFX.target3 |
| Back GFX / Layer 2 | Blank Movieclip | target3 | 708.0 | 38.3 |  |
| Back GFX / Layer 2 | Cheep Cheep Code |  | 888.0 | 249.9 | scale=1; chasespeed=0.4; idlespeed=2 |
| Back GFX / Layer 2 | Cheep Cheep Code |  | 1094.2 | 194.9 | scale=1; chasespeed=0.3; idlespeed=2 |
| Front GFX / Goonies | Goonie Code |  | 1066.2 | 78.8 |  |
| Front GFX / Goonies | Goonie Code |  | 438.1 | 99.3 |  |
| Front GFX / Goonies | Goonie Code |  | 859.7 | 79.5 |  |
| Front GFX / Goonies | Goonie Code |  | 605.5 | 134.8 |  |
| Front GFX / Goonies | Butterfly Code |  | 23.9 | -32.4 |  |
| Front GFX / Goonies | Goonie Code |  | 765.5 | 174.8 |  |
| Front GFX / Goonies | Goonie Code |  | 925.5 | -65.2 |  |
| Front GFX / Goonies | Goonie Code |  | 125.5 | -145.2 |  |

### 8-E1-2

| Layer | Object | Instance | x | y | Parameters |
|---|---|---|---:|---:|---|
| Back GFX / Layer 2 (frame 3) | Blank Movieclip | warp1 | -60.0 | -75.0 |  |
| Back GFX / Layer 2 (frame 3) | Blank Movieclip | warp2 | 1905.9 | -315.4 |  |
| Back GFX / Layer 2 (frame 3) | Camera Control Box |  | 439.4 | -41.6 | t="X"; amount=50; speed=20 |
| Back GFX / Layer 2 (frame 3) | Camera Control Box |  | 602.9 | -41.6 | t="X"; amount=-50; speed=20 |
| Back GFX / Layer 2 (frame 3) | Camera Control Box |  | 778.4 | -105.7 | t="Y"; amount=50; speed=20 |
| Back GFX / Layer 2 (frame 3) | Camera Control Box |  | 1409.2 | -217.8 | t="X"; amount=100; speed=20 |
| Back GFX / Layer 2 (frame 3) | Blank Movieclip |  | 792.1 | -216.5 | mess="(Hint: Press <b>C</b> to use Fludd)" |
| Back GFX / Coins | Gold Coin Code |  | 266.9 | -42.0 |  |
| Back GFX / Coins | Gold Coin Code |  | 306.9 | -42.0 |  |
| Back GFX / Coins | Gold Coin Code |  | 346.9 | -42.0 |  |
| Back GFX / Coins | Gold Coin Code |  | 738.1 | -146.8 |  |
| Back GFX / Coins | Gold Coin Code |  | 806.0 | -169.8 |  |
| Back GFX / Coins | Gold Coin Code |  | 773.0 | -80.4 |  |
| Back GFX / Coins | Gold Coin Code |  | 657.1 | -111.5 |  |
| Back GFX / Coins | Gold Coin Code |  | 918.0 | -275.3 |  |
| Back GFX / Coins | Gold Coin Code |  | 968.0 | -275.3 |  |
| Back GFX / Coins | Gold Coin Code |  | 1018.0 | -249.3 |  |
| Back GFX / Coins | Gold Coin Code |  | 1058.0 | -249.3 |  |
| Back GFX / Coins | Gold Coin Code |  | 1098.0 | -249.3 |  |
| Back GFX / Coins | Gold Coin Code |  | 1146.9 | -231.3 |  |
| Back GFX / Coins | Gold Coin Code |  | 1440.9 | -231.3 |  |
| Back GFX / Coins | Gold Coin Code |  | 1454.9 | -267.3 |  |
| Back GFX / Coins | Gold Coin Code |  | 1485.9 | -288.3 |  |
| Back GFX / Coins | Gold Coin Code |  | 1523.9 | -284.3 |  |
| Back GFX / Coins | Gold Coin Code |  | 1563.9 | -289.3 |  |
| Back GFX / Coins | Gold Coin Code |  | 1605.9 | -300.8 |  |
| Back GFX / Coins | Gold Coin Code |  | 1651.9 | -306.3 |  |
| Back GFX / Coins | Gold Coin Code |  | 1697.9 | -297.8 |  |
| Back GFX / Coins | Gold Coin Code |  | 1735.9 | -274.8 |  |
| Back GFX / Coins | Gold Coin Code |  | 1764.9 | -240.8 |  |
| Back GFX / Coins | Gold Coin Code |  | 1786.9 | -192.8 |  |
| Back GFX / Enemy | Goomba |  | 1073.0 | -263.3 |  |
| Back GFX / Enemy | Koopa Code |  | 1323.0 | -216.8 | State="Flying" |
| Back GFX / Enemy | Goomba |  | 227.5 | -47.0 |  |
| Back GFX / Enemy | Cheep Cheep Code |  | 1555.9 | -42.5 | scale=1; chasespeed=0.4; idlespeed=2 |
| Back GFX / Enemy | Cheep Cheep Code |  | 1448.9 | -98.1 | scale=1; chasespeed=0.3; idlespeed=2 |
| Back GFX / Enemy | Cheep Cheep Code |  | 1673.9 | -81.1 | scale=1; chasespeed=0.3; idlespeed=2 |
| Back GFX / Enemy | Toad Main Clip |  | 811.0 | -34.0 | Toad dialogue (6 pages) |
| Back GFX / Layer 1 | Squirt Nozzle Box Clip |  | 522.0 | -24.1 |  |
| Back GFX / Layer 1 | Coded Sign |  | 1204.9 | -192.2 | signmessage="refill"; picture=true; othercamcenter=_root.Course.BackGFX.wawa |
| Back GFX / Layer 1 | Coded Sign |  | 473.0 | -1.9 | signmessage="hover"; picture=true; othercamcenter=_root.Course.BackGFX.hoover |
| Back GFX / Layer 1 | Blank Movieclip | hoover | 522.0 | -24.6 |  |
| Back GFX / Layer 1 | Blank Movieclip | wawa | 1492.2 | -35.0 |  |
| Front GFX / Layer 4 | Goonie Code |  | -23.0 | -193.2 |  |
| Front GFX / Layer 4 | Goonie Code |  | 172.0 | 18.5 |  |
| Front GFX / Layer 4 | Goonie Code |  | 419.9 | -100.7 |  |
| Front GFX / Layer 4 | Goonie Code |  | 631.0 | 4.0 |  |
| Front GFX / Layer 4 | Goonie Code |  | 1090.0 | -112.5 |  |
| Front GFX / Layer 4 | Goonie Code |  | 1675.9 | -161.6 |  |
| Front GFX / Layer 4 | Goonie Code |  | 842.4 | -54.0 |  |
| Front GFX / Layer 4 | Goonie Code |  | 1431.9 | -130.2 |  |
| Front GFX / Layer 4 | Goonie Code |  | 1222.8 | -378.9 |  |
| Front GFX / Layer 4 | Butterfly Code |  | 57.0 | -62.9 |  |
| Front GFX / Layer 4 | Butterfly Code |  | 248.9 | -58.4 |  |
| Front GFX / Layer 4 | Butterfly Code |  | 385.9 | -40.3 |  |
| Front GFX / Layer 4 | Butterfly Code |  | 667.9 | -148.2 |  |
| Front GFX / Layer 4 | Butterfly Code |  | 978.4 | -289.1 |  |
| Front GFX / Layer 4 | Butterfly Code |  | 1172.0 | -217.2 |  |
| Front GFX / Layer 4 | Butterfly Code |  | 1325.0 | -162.8 |  |
| Front GFX / Layer 4 | Butterfly Code |  | 1818.2 | -209.9 |  |
| Front GFX / Layer 4 | Butterfly Code |  | 788.0 | -94.5 |  |
| Water / Layer 2 | Water Code |  | 1380.8 | -63.0 | WaterLength=360; polygons=30 |
| Water / Layer 3 | Giant Water Block |  | 1532.9 | -7.5 |  |

### 8-E1-2-2

| Layer | Object | Instance | x | y | Parameters |
|---|---|---|---:|---:|---|
| BackGFX / Layer 7 | Blank Movieclip | warp1 | -52.8 | -113.0 |  |
| BackGFX / Layer 7 | Blank Movieclip | warp3 | -674.1 | -275.3 |  |
| BackGFX / Layer 5 (frame 5) | Blank Movieclip | warp2 | 1124.9 | -913.5 |  |
| BackGFX / Layer 5 (frame 5) | Camera Control Box |  | 52.9 | -470.6 | t="Y"; amount=25; speed=20 |
| BackGFX / Layer 5 (frame 5) | Camera Control Box |  | -760.2 | -599.6 | t="Y"; amount=40; speed=20 |
| BackGFX / Layer 5 (frame 5) | Camera Control Box |  | 384.9 | -150.1 | t="Y"; amount=35; speed=20 |
| BackGFX / Layer 5 (frame 5) | Camera Control Box |  | 372.2 | -438.4 | t="X"; amount=-50; speed=20 |
| BackGFX / Layer 5 (frame 5) | Camera Control Box |  | -716.0 | -841.0 | t="X"; amount=50; speed=20 |
| BackGFX / Layer 5 (frame 5) | Camera Control Box |  | -382.4 | -577.1 | t="Y"; amount=-50; speed=20 |
| BackGFX / Layer 6 (frame 2) | Invisible Door |  | 964.0 | -783.3 | doorplace="C-Opening"; walk="warppipe" |
| BackGFX / Layer 4 (frame 2) | Blank Movieclip |  | 960.4 | -870.2 |  |
| BackGFX / Layer 4 (frame 4) | Shine Sprite Clip |  | 730.2 | -813.5 | starnum=39 |
| BackGFX / Layer 2 | Coded Sign |  | 338.2 | -0.8 | signmessage="Hover Fludd also can be used to regain balance if you slip off a P... |
| BackGFX / Layer 2 | Coded Sign |  | -770.6 | -505.4 | signmessage="zoom"; picture=true |
| BackGFX / Layer 2 | Goomba |  | -825.6 | -521.5 |  |
| BackGFX / Layer 2 | Koopa Code |  | -87.7 | -519.5 | State="Flying" |
| BackGFX / Layer 2 | Koopa Code |  | -667.6 | -860.8 | State="Flying" |
| BackGFX / Layer 2 | Green Circle Platform |  | 353.8 | -239.9 | speed=2; distance=50; angle=0 |
| BackGFX / Layer 2 | Green Circle Platform |  | 353.8 | -239.9 | speed=2; distance=50; angle=90 |
| BackGFX / Layer 2 | Green Circle Platform |  | 353.8 | -239.9 | speed=2; distance=50; angle=180 |
| BackGFX / Layer 2 | Green Circle Platform |  | 353.8 | -239.9 | speed=2; distance=50; angle=270 |
| BackGFX / Layer 2 | Green Circle Platform |  | 403.8 | -392.4 | speed=2; distance=50; angle=0 |
| BackGFX / Layer 2 | Green Circle Platform |  | 403.8 | -392.4 | speed=2; distance=50; angle=90 |
| BackGFX / Layer 2 | Green Circle Platform |  | 403.8 | -392.4 | speed=2; distance=50; angle=180 |
| BackGFX / Layer 2 | Green Circle Platform |  | 403.8 | -392.4 | speed=2; distance=50; angle=270 |
| BackGFX / Layer 2 | Green Circle Platform |  | 259.8 | -463.4 | speed=2; distance=50; angle=0 |
| BackGFX / Layer 2 | Green Circle Platform |  | 259.8 | -463.4 | speed=2; distance=50; angle=90 |
| BackGFX / Layer 2 | Green Circle Platform |  | 259.8 | -463.4 | speed=2; distance=50; angle=180 |
| BackGFX / Layer 2 | Green Circle Platform |  | 259.8 | -463.4 | speed=2; distance=50; angle=270 |
| BackGFX / Layer 2 | Tipping Log |  | 48.2 | -444.4 | scale=1 |
| BackGFX / Layer 2 | Tipping Log |  | -167.7 | -471.5 | scale=2 |
| BackGFX / Layer 2 | Rotating Block Plat |  | -388.9 | -506.4 | rotwait=48; rotdirection="Right"; scale=90; blocktype=1 |
| BackGFX / Layer 2 | Rotating Block Plat |  | -567.6 | -528.5 | rotwait=32; rotdirection="Right"; scale=70; blocktype=2 |
| BackGFX / Layer 2 | Rotating Block Plat |  | -668.6 | -528.5 | rotwait=32; rotdirection="Left"; scale=70; blocktype=2 |
| BackGFX / Layer 2 | Gold Coin Code |  | 260.2 | -429.5 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 260.2 | -529.5 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 210.2 | -479.5 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 310.2 | -479.5 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 224.2 | -515.5 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 296.2 | -515.5 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 224.2 | -443.5 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 296.2 | -443.5 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 353.2 | -211.3 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 353.2 | -311.3 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 303.2 | -261.3 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 403.2 | -261.3 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 317.2 | -297.3 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 389.2 | -297.3 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 317.2 | -225.3 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 389.2 | -225.3 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 404.2 | -361.5 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 404.2 | -461.5 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 354.2 | -411.5 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 454.2 | -411.5 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 368.2 | -447.5 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 440.2 | -447.5 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 368.2 | -375.5 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 440.2 | -375.5 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | -170.7 | -508.5 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | -389.7 | -586.6 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | -568.6 | -595.6 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | -668.6 | -595.6 |  |
| BackGFX / Layer 2 | Green Circle Platform |  | -496.9 | -867.8 | speed=-2; distance=50; angle=0 |
| BackGFX / Layer 2 | Green Circle Platform |  | -496.9 | -867.8 | speed=-2; distance=50; angle=120 |
| BackGFX / Layer 2 | Green Circle Platform |  | -496.9 | -867.8 | speed=-2; distance=50; angle=240 |
| BackGFX / Layer 2 | Tipping Log |  | -317.9 | -843.8 | scale=2 |
| BackGFX / Layer 2 | Green Circle Platform |  | -52.7 | -855.8 | speed=2; distance=50; angle=0 |
| BackGFX / Layer 2 | Green Circle Platform |  | 47.3 | -855.8 | speed=2; distance=50; angle=0 |
| BackGFX / Layer 2 | Green Circle Platform |  | 147.3 | -855.8 | speed=2; distance=50; angle=0 |
| BackGFX / Layer 2 | Green Circle Platform |  | 247.3 | -855.8 | speed=2; distance=50; angle=0 |
| BackGFX / Layer 2 | Gold Coin Code |  | -345.6 | -878.8 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | -292.6 | -878.8 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 367.2 | -836.8 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 467.2 | -836.8 |  |
| BackGFX / Layer 2 | Gold Coin Code |  | 417.2 | -836.8 |  |
| BackGFX / Layer 2 | Water Bottle Clip |  | 420.1 | -81.8 |  |
| BackGFX / Layer 2 | Water Bottle Clip |  | -36.3 | -500.1 |  |
| BackGFX / Layer 2 | Water Bottle Clip |  | -847.0 | -529.6 |  |
| BackGFX / Layer 2 | Bomb omb Code |  | 493.1 | 4.0 |  |
| BackGFX / Layer 2 | Goomba |  | 275.2 | -6.3 |  |
| BackGFX / Layer 2 | Coded Sign |  | 910.9 | -731.9 | signmessage="You can take this Warp-Pipe to Princess Peaches Castle\n(Get on to... |
| BackGFX / Layer 2 | Toad Main Clip |  | -911.4 | -824.4 | Toad dialogue (6 pages) |
| BackGFX / Layer 2 | Coded Sign |  | -162.7 | -840.4 | signmessage="Oh!  Mario!  You can pause the game by pressing <b>P</b>.  There, ... |
| FrontGFX / Layer 2 | Goonie Code |  | 357.8 | 57.8 |  |
| FrontGFX / Layer 2 | Goonie Code |  | 455.9 | -20.7 |  |
| FrontGFX / Layer 2 | Goonie Code |  | -3.1 | 36.3 |  |
| FrontGFX / Layer 2 | Goonie Code |  | 266.4 | -209.2 |  |
| FrontGFX / Layer 2 | Goonie Code |  | 485.9 | -148.7 |  |
| FrontGFX / Layer 2 | Goonie Code |  | 16.9 | -220.9 |  |
| FrontGFX / Layer 2 | Goonie Code |  | 245.8 | -335.8 |  |
| FrontGFX / Layer 2 | Goonie Code |  | 54.5 | -514.5 |  |
| FrontGFX / Layer 2 | Goonie Code |  | 252.8 | -644.5 |  |
| FrontGFX / Layer 2 | Goonie Code |  | -443.1 | -418.5 |  |
| FrontGFX / Layer 2 | Goonie Code |  | -1194.0 | -335.8 |  |
| FrontGFX / Layer 2 | Butterfly Code |  | -22.8 | -57.5 |  |
| FrontGFX / Layer 2 | Butterfly Code |  | 101.8 | -13.4 |  |
| FrontGFX / Layer 2 | Butterfly Code |  | 265.2 | -41.6 |  |
| FrontGFX / Layer 2 | Butterfly Code |  | -197.8 | -508.8 |  |


### Picture signs (`Message Picture - Pictures`, SWF sprite 619)

`Coded Sign` with `picture = true` shows a page of sprite 619 by label: `Start`,
`controls`, `goomba`, `bomb`, `stomp`, `ttt`, `hover`, `refill`, `zoom`. The pages
are vector art with keyboard keys drawn in (Arrow keys, **Z** read/talk/stomp,
**X** spin, **C** FLUDD, **+/-** zoom). Text signs use `PlayMessage`. Reading:
stand on the sign box, press Z with no Left/Right; Z again closes; then
`completefunction()` runs (`Coded Sign.xml` frames 3/10/16).

### Scripted opening (`8-E1-1 Back GFX`, "Start" sign)

On a new game (`TotalStars == 0`, Mario left of the sign): Mario is set to
`y = 60`, controls lock, he is walked to the sign after 60+12 frames and the
"Start" page opens with the camera on `sign2` (the "controls" sign).

### Toad dialogue

`Toad Main Clip` pages `M1..M6` (`Mood`, `Txt`, start/end conditions), read with Z.
8-E1-1 Toad: Final Fantasy monologue; 8-E1-2-2 Toad: "...Hello Mario, How are you?"
sequence. Full text in the inventory parameters' source scripts.

## 10. Warps, completion and destination

| From | Trigger | To |
|---|---|---|
| 8-E1-1 `warp1` (1855.8, -126) | feet hitTest | `8-E1-1-2` (0,0), right transition |
| 8-E1-1 `warp2` (-207, -10), frame 4 only | feet | `8-E2-1` (later event, not tutorial) |
| 8-E1-1-2 `warp1` | feet | back to `8-E1-1` (1760, -20) |
| 8-E1-1-2 `warp2` | feet | `8-E1-2` (0,0) |
| 8-E1-2 `warp1` / `warp2` | feet | `8-E1-1-2` (1730, -30) / `8-E1-2-2` (0,0) |
| 8-E1-2-2 `warp1` / `warp2` / `warp3` | feet | `8-E1-2` (1840,-170) / `8-E3-1` / fall back to `8-E1-2` |
| 8-E1-2-2 `Invisible Door` (964, -783), `doorplace = "C-Opening"`, warp pipe | Down on pipe | **Completion** |

Completion (`DOMDocument.xml:13207`): if `Star[39]` is false, stop music,
`gotoAndStop("Intro2")`, remove the course. Intro Movie 2 (`LIBRARY/Menu/Intro
Movie 2.xml`) plays; its `Mario Talk` clip sets `Star[39] = true`,
`CalculateStars()`, `SaveFile()` (`LIBRARY/Char/Mario Talk.xml:79-86`); its last
frame starts `"Castle"` at (80, 60) facing left. Later uses go straight to
`"Castle"` (-1300, 100). The 8-E1-2-2 Shine Sprite (`starnum = 39`, (730, -814))
is only on the frames reached when `Star[39]` is already true.

**Destination not in this port:** Intro Movie 2 and the Castle. Implement as a
clearly marked transition boundary.

## Mechanics the tutorial uses

Walking, double-tap run, jumps and chained/triple jump, dive, crouch and backflip,
spin, ground pound ("stomp"), reading signs and talking (Z), swimming, coins and
health (5 coins heal 1), Goomba stomp/dive, Bob-omb spin, Koopas, Cheep Cheeps,
breakable boxes, FLUDD box (Hover), Hover, water refill (lake, Water Bottle),
moving platforms, camera control boxes, zoom (+/-), pause menu, warp pipe.

## Missing or uncertain

- Collision outlines not yet rasterised; FFDec render of the `Platforms` sprites
  is the planned source.
- Object behaviour scripts (Goomba, Koopa, Bob-omb, Cheep Cheep, platforms,
  Toad, FLUDD box, Water Bottle, camera boxes) still to be read before porting.
- Intro Movie 2 and the Castle are separate content outside this level.
