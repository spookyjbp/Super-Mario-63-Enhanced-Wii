# Course pack format (v2)

`assets/generated/course_8e1.bin` holds the original tutorial areas `8-E1-1`,
`8-E1-1-2`, `8-E1-2`, `8-E1-2-2`, built by `tools/export_course.py` from a pinned
Runouw/Super-Mario-63 checkout and FFDec 26.4.0 (same requirements as the player
art; see `private-player-art.md`):

```sh
cd tools
python export_course.py /path/to/Super-Mario-63 /path/to/ffdec.jar ../assets/generated/course_8e1.bin \
    8-E1-1 8-E1-1-2 8-E1-2 8-E1-2-2
```

The report `private-art/rendered-course/course.json` lists every mask, water
volume, object (with parameters) and timeline script per area.

## Units

Original course pixels. `StartChar(area, 0, 0, 0, 0)` places Char (Mario's feet),
`BackGFX`, `Platforms` and every other layer at the same origin, so all data share
one space. The Wii world maps 1 course pixel to 1.375 port pixels (the scale the
port's movement constants already use).

## Layout (big-endian)

```
"SM63CRS1" u32 version=2 u32 area_count
per area: u32 byte_size, then
  string id
  u32 mask_count, per mask:
    u8 layer (0 Platforms,1 BPlatforms,2 CamEdge,3 Ice,4 Puddle,5 Lava) u8 frame u16 0
    i32 x0 i32 y0 u32 width u32 height
    u32 row_offset[height]; u32 run_bytes; per row: u16 n, n x (u16 start, u16 length)
  u32 water_count, per water: f32 x y w h        (Water Code: WaterLength x WaterDeepness)
  u32 object_count, per object:
    string type, name, layer; f32 x y a b c d (placement matrix); u16 frame_start frame_end
    u16 param_count, per param: string key, string value
string = u16 length + UTF-8 bytes
```

Masks are Flash `hitTest(x, y, true)` sampled as alpha >= 128 of the 1:1 FFDec
render. A layer with several frames (8-E1-1 `Platforms`: barrier before and after
the "controls" sign) stores one mask per frame; later frames hold the last
keyframe, as Flash does. Parameter values are the `onClipEvent(load)` assignments
with string expressions (`"a"+newline+"b"`, `_root.CurrentPlayer`) evaluated.

`source/game/course_pack.c` validates the whole blob once and then reads it in
place: no copies or allocation. A point test is one row lookup plus a binary search
over that row's runs.
