# Controller picture signs: approved design (with the user, 2026-11-01)

Approved mockups (shown over the tutorial scene; the original pages are
see-through white panels): `controls_mock.png`, `sign_Start.png`,
`sign_bomb.png`, `sign_hover.png`, `sign_stomp.png`, `sign_zoom.png`;
button sets: `sets.png`. Mockup code: `tools/sign_mockup/` (prototype, paths
point at a scratch work area). Built into the game by `tools/controller_signs.py`
(run by `export_course_art.py`); the pages match these mockups.

Decisions
- Option 1: keep the original page art, replace only the keyboard keys.
- Buttons in the original keycap style (light rim, darker face, hand-drawn
  thick marker letters), each in its real shape. GameCube buttons in their real
  colours (green A, red B, purple Z, yellow C); Classic and Wii Remote buttons
  grey like the original keycaps.
- Combos: "+" between buttons (Wii Remote A + B); Classic stomp "L or R".
- Controls page: the four arrow keycaps become one big D-pad with "Move" kept
  on top; Z/X/C become a 2 x 2 grid (Jump, Spin, Stomp / Read, FLUDD) with
  small labels. The diving Mario (bitmap in shape 363) sits on the middle of
  the D-pad's Down arm.
- Keys are removed from the original art itself where they are separate
  shapes (arrow keys 327/329/358/359/363 in 371, Z keycap 416, box keycaps 417
  in 424); where a key is baked into a larger shape (X in 413, C in 563,
  Minus/Plus in 322) only the keycap's own pixels are cleared.
- Bob-omb: the explosion (bitmap 412) is drawn whole again (the X keycap used
  to cover part of it); the spin button sits right of the explosion.
- Welcome page: text redrawn in bold white with the D-pad and the read button
  inline, original curved arrow redrawn.
- Zoom page: Out / In in the original blue lettering with the controller's zoom
  input (C-stick / right stick with an arrow, Wii Remote - / +).
- Stomp page: the boxes keep Mario's ground-pound demo (clip 424).

Later (user request): every picture sign animates like the original (the
stomp boxes' 101-frame demo, the controls page Marios, the Goomba demo, ...).

## "Press … to Continue" prompt, redone (approved 2026-11-02, not in the game yet)

Preview: `prompt_preview.png` (at in-game size on a Dolphin screenshot; the top row is the original).

- **Split:** the original prompt (sprite 250, shape 244 at frame 50) is split into its two layers, the teal words and the white glow. `tools/sign_mockup/prompt_split.py` makes the other layer's fills transparent and renders each one.
- **Words:** "Press" and "to Continue" keep the original letters.
- **Glow:** the original glow is stretched smoothly to the new width and stays centred.
- **Button:** the controller's read button replaces the "Z", sized 1.6 × the Z's cap height and centred on it, with the original word spacing. Code: `tools/sign_mockup/prompt_build.py`, `build(kind, 1.6)`.
- **Classic:** "L or R", with "or" cut from the prompt's own letters (the r of Press, the o of to).
- **Wii Remote:** "A + B", with a "+" drawn in the same teal.

To do when it is added to the game:

- Replace `ui_art.prompt_versions` with this method.
- The fade and pulse stay as they are (`prompt_alpha`).
