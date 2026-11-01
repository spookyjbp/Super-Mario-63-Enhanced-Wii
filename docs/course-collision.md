# Course collision and camera (original rules)

Source: `DOMDocument.xml` CharCode collision (6262-6420) and camera (5200-5290),
Runouw/Super-Mario-63 `83e81fb`. Implemented in `source/game/course_world.c`
for `World.course`; tile levels (the movement lab) keep `collision.c`.

## Collision

Mario is a feet point `(x, y)` in course pixels, `wide = 10` (8 diving),
`tall = 34` without FLUDD, 40 with, 25 diving. Each tick is split into
`repeats` substeps (the original's `ceil((|xspeed|+|yspeed|)/5)` per 32 Hz frame,
1..50). Per substep, in this order, against the exported masks:

1. Feet inside `Platforms` or `BPlatforms`: grounded, `yspeed = min(0, yspeed)`.
2. While `(x, y-3)` is in `Platforms` (or `BPlatforms` while `yspeed >= -3`):
   `y -= 1`, `xspeed *= 0.98` (0.997 diving). This is how slopes are climbed, and
   it leaves the feet resting about 3 px inside the ground, as in the original.
3. Ground snap (`lockon`, off when jumping or swimming): while `(x, y+5)` is
   solid and `(x, y-1)` is not, `y += 1`.
4. Walls at half height: push out of `x +/- (wide-1)`; touching `x +/- wide`
   records the wall for the existing bonk rule, otherwise bounces
   (`xspeed = min(xspeed-1, -xspeed*0.5)`; holding toward it stops instead).
5. Ceiling: `(x, y-tall+1)` caps `yspeed >= -3`; push out of `(x, y-tall)`.
6. Move by `velocity / repeats`.

`BPlatforms` are one-way: floor and climb only. Ice and Puddle masks set the
surface material. The ground slope sign for slides comes from the surface 5 px
either side. Velocities, gravity, jumps and every movement rule are the port's
existing (Dolphin-verified) ones; only geometry handling follows the source.

## Camera

The view is the original 450x300 stage height: 300 course pixels fill the
480-line screen (zoom 480 / (300 x 1.375)); widescreen shows more width. Each
32 Hz frame the centre moves by `(Mario - centre)/10 + speed/4`, so it trails
Mario; vertically it targets `CamYchange`, updated exactly as written
(`CamYchange = (80 - CamYchange)/10` in the air, 50 on the ground). Moving toward
a touched edge is limited to 1 px, then each screen-edge midpoint is pushed out of
`CamEdge` by up to 50 px.

## Tests

`tests/course_play_test.c` plays area 8-E1-1: landing under the spawn, the
barrier before and after the "controls" sign, the route to the central rock (the
ledge needs the chained jump the area's hint describes), the far bank to the exit
warp over slopes and ice, and the camera never crossing `CamEdge`. The crossing
between them needs the Falling Log object, ported with the objects.
