#include "player_sprite.h"
#include "mario_atlas.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static void expect(const Player *p, unsigned frame, bool flip)
{
    Player before;
    memcpy(&before, p, sizeof(before));
    const PlayerSprite sprite = player_sprite_select(p);
    assert(sprite.frame == frame && sprite.flip_x == flip);
    assert(memcmp(&before, p, sizeof(before)) == 0);
}

static void test_native_metadata(void)
{
    for (unsigned i = 0; i <= 8; ++i) {
        assert(mario_frames[i].native_facing == MARIO_FACE_RIGHT);
        assert(!mario_frames[i].mirror_for_right);
    }
    assert(mario_frames[9].native_facing == MARIO_FACE_LEFT);
    assert(mario_frames[10].native_facing == MARIO_FACE_LEFT);
    assert(mario_frames[9].mirror_for_right && mario_frames[10].mirror_for_right);
    assert(mario_frames[11].native_facing == MARIO_FACE_FRONT);
    assert(mario_frames[12].native_facing == MARIO_FACE_LEFT);
    assert(mario_frames[13].native_facing == MARIO_FACE_LEFT);
    assert(mario_frames[14].native_facing == MARIO_FACE_BACK_LEFT);
    assert(mario_frames[15].native_facing == MARIO_FACE_BACK);
    for (unsigned i = 16; i <= 18; ++i) {
        assert(mario_frames[i].native_facing == MARIO_FACE_RIGHT);
        assert(mario_frames[i].mirror_for_right); /* Retain authored pound mirror. */
    }
    assert(mario_frames[19].native_facing == MARIO_FACE_RIGHT);
}

static void test_preserved_states(void)
{
    Player p = {0};
    for (unsigned left = 0; left < 2; ++left) {
        p.facing_left = left != 0;
        p.animation = PLAYER_IDLE;
        expect(&p, 0, left != 0);
        p.animation = PLAYER_RUN;
        for (unsigned phase = 0; phase < 8; ++phase) {
            p.animation_time = (phase + 0.5f) / 12;
            expect(&p, phase + 1, left != 0);
        }
        p.animation = PLAYER_POUND_WINDUP; p.pound_windup = 0.1f;
        expect(&p, 16, left == 0);
        p.pound_windup = 0.04f;
        expect(&p, 17, left == 0);
        p.animation = PLAYER_POUND;
        expect(&p, 18, left == 0);
        p.animation = PLAYER_LAND;
        expect(&p, 19, left != 0);
    }
}

static void test_airborne_facing(void)
{
    Player p = {0};
    for (unsigned left = 0; left < 2; ++left) {
        p.facing_left = left != 0;
        p.animation = PLAYER_RISE;
        expect(&p, 9, left == 0);
        p.animation = PLAYER_FALL;
        expect(&p, 10, left == 0);
    }
    /* Exercise direction changes through the existing physics, including the
       reversal interval where facing follows input before velocity crosses 0. */
    const World empty = {.solids=NULL, .count=0, .width=2400, .height=960, .spawn_x=100, .spawn_y=100};
    player_init(&p, 100, 100); p.vy = -300;
    player_step(&p, &empty, (GameInput){.horizontal = 1, .jump_held = true});
    assert(p.animation == PLAYER_RISE && !p.facing_left);
    expect(&p, 9, true);
    player_step(&p, &empty, (GameInput){.horizontal = -1, .jump_held = true});
    assert(p.animation == PLAYER_RISE && p.facing_left);
    expect(&p, 9, false);
    p.vy = 200;
    player_step(&p, &empty, (GameInput){.horizontal = 1});
    assert(p.animation == PLAYER_FALL && !p.facing_left);
    expect(&p, 10, true);
}

static void test_spin_keyframe_mirroring(void)
{
    /* Original Flash turn order, including PNG reuse with opposite mirrors. */
    static const unsigned frames[] = {13, 12, 11, 12, 13, 14, 15, 14};
    static const bool mirrors[] = {true, true, false, false, false, false, false, true};
    Player p = {.animation = PLAYER_SPIN};
    for (unsigned phase = 0; phase < 8; ++phase) {
        p.spin_time = 0.32f - (phase + 0.5f) / 25;
        for (unsigned left = 0; left < 2; ++left) {
            p.facing_left = left != 0;
            expect(&p, frames[phase], (left != 0) != mirrors[phase]);
        }
    }
    /* The same frame has different mirrors across the turn. Native-facing
       compensation must not be applied again to an already mirrored phase. */
    p.facing_left = false;
    p.spin_time = 0.30f; expect(&p, 13, true);
    p.spin_time = 0.14f; expect(&p, 13, false);
}

static void test_spin_exit_preserves_facing(void)
{
    const World empty = {.solids=NULL, .count=0, .width=2400, .height=960, .spawn_x=100, .spawn_y=100};
    for (unsigned left = 0; left < 2; ++left) {
        Player p;
        player_init(&p, 100, 100);
        p.vy = 0;
        const float axis = left ? -1 : 1;
        player_step(&p, &empty, (GameInput){.horizontal = axis, .spin_pressed = true, .jump_held = true});
        assert(p.animation == PLAYER_SPIN);
        for (int tick = 0; tick < 100; ++tick) {
            const bool facing = p.facing_left;
            (void)player_sprite_select(&p);
            assert(p.facing_left == facing);
            player_step(&p, &empty, (GameInput){.horizontal = axis, .jump_held = true});
        }
        assert(p.spin_time == 0 && p.facing_left == (left != 0));
        const unsigned frame = p.animation == PLAYER_RISE ? 9 : 10;
        expect(&p, frame, left == 0);
    }
}

static void test_v04_pose_orientation(void)
{
    Player p={0};
    for(int state=PLAYER_CROUCH;state<PLAYER_ANIMATION_COUNT;++state)
        for(unsigned left=0;left<2;++left) {
            p.animation=(PlayerAnimation)state;p.facing_left=left!=0;p.action_time=0.125f;
            const PlayerSprite pose=player_sprite_select(&p);
            assert(pose.frame<20 && pose.height_scale>0 && pose.height_scale<=1);
            assert(pose.flip_x==((left!=0)!=(mario_frames[pose.frame].mirror_for_right!=0)));
            if(state==PLAYER_BACKFLIP || state==PLAYER_ROLL || state==PLAYER_SWIM || state==PLAYER_DIVE) {
                const float angle=pose.rotation;
                p.facing_left=!p.facing_left;
                assert(player_sprite_select(&p).rotation==-angle);
            }
        }
}
int main(void)
{
    test_native_metadata(); test_preserved_states(); test_airborne_facing();
    test_spin_keyframe_mirroring(); test_spin_exit_preserves_facing(); test_v04_pose_orientation();
    puts("6 sprite orientation test groups passed");
    return 0;
}
