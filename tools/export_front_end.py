#!/usr/bin/env python3
"""The game's opening, title screen and menus, replayed from the original
Flash timelines (tools/timeline_art.py) into assets/data/front_end.bin.

Clips (the original's names and frames):
  opening    9280, frames 1-341: the copyright notice, then the Runouw logo
             (its MP3 sound stream is SFX_OPENING, tools/export_audio.py)
  title      9339, frames 1-340: the title screen up to where its credits
             fly through other levels (not in the port); frames 82-337 loop
             with the title theme's 2.58-10.58 s (the original repeats from
             frame 82 / 2.58 s too). START (9298, depth 1470) is group 1: its
             script (grow in, pulse, grow when pressed) runs in the port.
  title_out  2137 TitleScreenTransition (20 frames, File Select on its last)
  ...        the main menu's pieces (9535), see MENU below

    python export_front_end.py <checkout> <ffdec.jar> <out_dir> [work_dir]
"""
import argparse
from pathlib import Path
import timeline_art as T

TITLE_LOOP = (81, 337)          # 0-based frames: 'repeat' .. one bar-aligned 8 s later
TITLE_START_DEPTH = 1470


# The main menu (9535) is replayed in pieces the port moves itself, as the
# original's scripts do: the backdrop's scrolling layers (9373), the Story
# page's title (9535's frame 2 without its buttons: the port goes from START
# to the file select, without the Start, Extras and Options pages), each
# button as the shared box (2116, 92 frames) and its label, the file boxes'
# letters, the erase animation (9403) and MenuRedTrans.
BUTTONS = {   # name: (button sprite, label sprite)
    'back': (9417, 9416),
    'play': (9421, 9420), 'copy': (9426, 9425), 'erase': (9429, 9428), 'fileback': (9432, 9431),
}
MENU_PAGE_STORY = 1   # 9535's frame (0-based) the port shows
FILE_LETTERS = [9405, 9407, 9409, 9411]   # 9414 frames 2-5: A, B, C, D
PAGE_SKIP = {9373, 9376, 9379, 9382, 9433, 9417, 9478, 9481, 9486, 9469, 9493, 769, 773, 784, 9526, 9383, 9434,
             9435, 9516}


def clips(tl):
    out = [
        ('opening', 9280, range(341), None, None),
        ('title', 9339, range(340), None, {TITLE_START_DEPTH: 1}),
        ('title_out', 2137, range(len(tl[2137]['frames'])), None, None),
        ('menu_page', 9535, range(MENU_PAGE_STORY, MENU_PAGE_STORY + 1), None, None, PAGE_SKIP),
        ('mbg_sky', 9348, range(1), None, None), ('mbg_far', 9349, range(1), None, None),
        ('mbg_mid', 9350, range(1), None, None), ('mbg_tree', 9371, range(20), None, None),
        ('mbg_front', 9372, range(1), None, None),
        ('btn_box', 2116, range(92), None, None),
        ('erase', 9403, range(35), None, None),
        ('red_trans', 2127, range(51), None, None, {'yes', 'no'} | text_fields(tl, 2127)),
        ('red_yes', 2119, range(1), None, None), ('red_no', 2122, range(1), None, None),
    ]
    for name, (button, label) in BUTTONS.items():
        out.append((f'lbl_{name}', label, range(len(tl[label]['frames'])) if tl[label]['kind'] == 'sprite' else range(1),
                    None, None))
    for k, letter in enumerate(FILE_LETTERS):
        out.append((f'file_{"ABCD"[k]}', letter, range(1), None, None))
    out.append(('file_copy', 9413, range(1), None, None))
    return out


# load groups (the pages uploaded together): the opening, the title, the
# title's transition (shown over the title and the menu), the menu
GROUP_OPENING, GROUP_TITLE, GROUP_TITLE_OUT, GROUP_MENU = 0, 1, 2, 3


def load_groups(tl):
    g = {'opening': GROUP_OPENING, 'title': GROUP_TITLE, 'title_out': GROUP_TITLE_OUT}
    return {spec[0]: g.get(spec[0], GROUP_MENU) for spec in clips(tl)}


def text_fields(tl, sprite, depth=0):
    """Ids of the text fields a clip places at any depth (the port writes their text)."""
    out = set()
    for fr in tl[sprite]['frames']:
        for e in fr.values():
            k = tl.get(e['id'], {}).get('kind')
            if k == 'text':
                out.add(e['id'])
            elif k == 'sprite' and depth < 4:
                out |= text_fields(tl, e['id'], depth + 1)
    return out


def placements(tl):
    """C tables of where the original places the menu's pieces (9535 / 9433 /
    9373 / 9525 / 2127 coordinates, 2 x 3 matrices)."""
    rows = {}
    def put(name, m):
        rows[name] = m
    page = tl[9535]['frames']
    for d, e in page[MENU_PAGE_STORY].items():
        if e['id'] == 9433:
            put('filem', e['matrix'])
    for name, (button, label) in BUTTONS.items():
        fr = tl[button]['frames'][0]
        for d, e in fr.items():
            if e.get('name') == 'bg':
                put(f'{name}_box', e['matrix'])
            if e['id'] == label:
                put(f'{name}_lbl', e['matrix'])
    for d, e in tl[9433]['frames'][0].items():
        if e['id'] == 9414:
            put(f'file_{e["name"]}', e['matrix'])
        if e['id'] == 9417:
            put('fileback_top', e['matrix'])
    fb = tl[9414]['frames']
    for d, e in fb[1].items():
        if e.get('name') == 'bg':
            put('filebox_box', e['matrix'])
        if e.get('name') == 'txt':
            put('filebox_lbl', e['matrix'])
        if e.get('name') == 'em':
            put('filebox_em', e['matrix'])
    for d, e in fb[5].items():
        if e['id'] == 9413:
            put('filebox_copy', e['matrix'])
    for d, e in tl[9373]['frames'][0].items():
        put({9348: 'mbg_sky', 9349: 'mbg_far', 9350: 'mbg_mid', 9372: 'mbg_front'}.get(e['id'], f'mbg_tree{d}'), e['matrix'])
    for d, e in tl[2127]['frames'][9].items():
        if e.get('name') in ('yes', 'no'):
            put(f'red_{e["name"]}', e['matrix'])
    for d, e in tl[2127]['frames'][9].items():   # the question's field (2123 in 2124), its box
        if e['id'] == 2124:
            for dd, ee in tl[2124]['frames'][0].items():
                if ee['id'] == 2123:
                    put('red_field', (308, 0, 0, 203, e['matrix'][4] + ee['matrix'][4] - 2, e['matrix'][5] + ee['matrix'][5] - 2))
    # the side buttons sliding in over 9433's frames 1-23
    side = {9421: 'play', 9426: 'copy', 9429: 'erase', 9432: 'fileback', 9423: 'fileinfo'}
    slide = {n: [] for n in side.values()}
    for fr in tl[9433]['frames']:
        got = {side[e['id']]: e['matrix'] for e in fr.values() if e['id'] in side}
        for n in slide:
            slide[n].append(got.get(n))
    return rows, slide


def main():
    p = argparse.ArgumentParser()
    p.add_argument('source', type=Path); p.add_argument('ffdec_jar', type=Path)
    p.add_argument('out', type=Path, help='assets/generated: the layout header')
    p.add_argument('work', type=Path, nargs='?')
    p.add_argument('--data', type=Path, help='where front_end.bin goes (default: assets/data, a data file on the SD card)')
    a = p.parse_args()
    swf = (a.source / 'Publish/Super_Mario_63_2010.swf').read_bytes()
    tl = T.parse(swf)
    work = a.work or a.out.parent.parent / 'private-art' / 'front-end'
    work.mkdir(parents=True, exist_ok=True)
    data, info = T.build_pack(tl, a.ffdec_jar, work, swf, clips(tl), load_groups(tl))
    data_dir = a.data or a.out.parent / 'data'
    data_dir.mkdir(parents=True, exist_ok=True)
    (data_dir / 'front_end.bin').write_bytes(data)
    rows, slide = placements(tl)
    lines = ['/* Generated by tools/export_front_end.py; do not edit. */', '#ifndef SM63_FRONT_END_LAYOUT_H',
             '#define SM63_FRONT_END_LAYOUT_H',
             '/* clip indices in front_end.bin */']
    for k, spec in enumerate(clips(tl)):
        lines.append(f'#define FE_CLIP_{spec[0].upper()} {k}')
    lines += [f'#define FE_GROUP_OPENING {GROUP_OPENING}', f'#define FE_GROUP_TITLE {GROUP_TITLE}',
              f'#define FE_GROUP_TITLE_OUT {GROUP_TITLE_OUT}', f'#define FE_GROUP_MENU {GROUP_MENU}']
    lines += [f'#define FE_CLIPS {len(clips(tl))}', '/* the menu_page clip shows 9535 frame 2, the Story page */', f'#define FE_TITLE_LOOP_FIRST {TITLE_LOOP[0]}',
              f'#define FE_TITLE_LOOP_LAST {TITLE_LOOP[1]}',
              '/* where the original places the menu pieces: a b c d tx ty */']
    for name, m in sorted(rows.items()):
        lines.append(f'static const float FE_AT_{name.upper()}[6] = {{' + ', '.join(f'{v:.4f}f' for v in m) + '};')
    lines.append('/* the file menu (9433) frames 1-23: the side buttons sliding in (x, y; 1e9 = not there) */')
    lines.append(f'#define FE_FILEM_FRAMES {len(next(iter(slide.values())))}')
    for name, ms in slide.items():
        vals = ', '.join('{%.2ff, %.2ff}' % (m[4], m[5]) if m else '{1e9f, 1e9f}' for m in ms)
        lines.append(f'static const float FE_SLIDE_{name.upper()}[FE_FILEM_FRAMES][2] = {{{vals}}};')
    lines += ['#endif', '']
    (a.out / 'front_end_layout.h').write_text('\n'.join(lines))
    print(info, len(data))


if __name__ == '__main__':
    main()
