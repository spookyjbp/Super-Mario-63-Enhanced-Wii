# Dolphin checklist: original tutorial (8-E1)

Nothing in this list has been run in Dolphin by the developer; the host tests
cover the simulation only. Run the steps in order and note anything that
differs from the original Flash game.

1. Boot `boot.dol`. The castle music intro plays, then loops into the repeat.
   Mario drops in at y 60, walks to the Start sign and the controls page opens.
2. HUD: shine and star coin counters top left, coin counter top right, POWER
   meter in the centre (it slides away after about 5 s at full health).
3. Walk: footsteps sound in time with the walk. Jump three times in a row:
   "Jump-1/1b", "Jump-2", "Tripple Jump".
4. Collect a coin (jingle, counter +1). Read a sign with Z.
5. Talk to Toad: cloud box, text typed in, Z prompt once complete.
6. Goomba: it notices Mario ("Goomba-Attention"), stomp it (bounce, coin).
7. 8-E1-1-2: Bob-ombs light their fuse on sight, flash red and explode after
   about 3 s; the blast knocks Mario (2 HP, pie and POWER meter update).
   Spin a Bob-omb: it tumbles and explodes on landing.
8. Ground-pound the stacked orange boxes: both break, 10 + 5 coins fly out.
9. Camera boxes (8-E1-1-2, 8-E1-2) shift the view as described in the signs.
10. 8-E1-2: jump onto the FLUDD box, the Hover nozzle drops; touch it. The
    tank appears bottom right; hold the FLUDD button: water loop sound, the
    level falls, the spray pushes enemies.
11. Stomp the Paratroopa: big bounce, it walks; stomp again for a shell; kick it.
12. Cheep Cheeps chase in the water; swim strokes and splashes sound.
13. 8-E1-2-2: green circle platforms carry Mario, tipping logs tip
    (wet-wood steps), rotating blocks shake and turn every 1-1.5 s, water
    bottles refill half a tank.
14. Enter the warp pipe at the end: the completion boundary screen appears.
15. 16:9 and 4:3 (Dolphin Graphics > Aspect Ratio): HUD stays in its corners,
    no texture corruption, steady 60 fps (check Dolphin's frame counter).
