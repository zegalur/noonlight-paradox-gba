/*!****************************************************************************
 * @file fonts.cppm
 * @brief This module specifies the standard in-game fonts.
 *******************************************************************************/        

module;
export module game.player;

import engine;
import game.data;
import game.options;
import game.particles;
import game.player_stats;
import game.player_gui;

import data.mainc;
import data.i_sight;
import data.sparkle;

#include "../engine/components/visual.hpp"

using namespace gba;
using namespace gba::sound;

namespace game {
namespace player {

export enum Msg {
    NOTHING = 0,
    ON_DEATH,
    ON_SHOOT_PISTOL,
    CARGO_PICKED_UP,
};

/// @brief The number of player components.
export constexpr u32 PLAYER_ECOUNT = 3;
export constexpr u32 E_BODY = 0;
export constexpr u32 E_SIGHT = 1;
export constexpr u32 E_SPARKLE = 2;

/// @brief Player height in pixels.
export constexpr i32 PLAYER_HEIGHT_PX = 5*8;
export constexpr i32 JUMP_CEILING_OFFSET = 4*8;
export constexpr i32 PLATFORM_HANG_MAX_DX = 8;
export constexpr i32 MAX_PLATFORM_HEIGHT = 8*2 + 1;

/// @brief Player look offset in pixels.
export constexpr i32 PLAYER_LOOK_OFFSET = 25;

export constexpr i32 RUN_SPEED = 2;
export constexpr i32 WALK_SPEED = 1;
export constexpr i32 MAX_UP_SLOPE = 2;
export constexpr i32 MAX_DOWN_SLOPE = MAX_UP_SLOPE;
export constexpr fix32 JUMP_INITIAL_FORCE = from_int(-5); // -6
export constexpr fix32 JUMP_FORCE_ATTENUATION = fix32(14); // 20
export constexpr fix32 JUMP_UP_INITIAL_FORCE = from_int(-6) + (from_int(13)>>5);

export constexpr fix32 SIGHT_SPEED = from_int(2);
export constexpr fix32 SIGHT_INITIAL_X = from_int(64);
export constexpr fix32 SIGHT_INITIAL_Y = from_int(-32);
export constexpr fix32 SIGHT_LEVEL_0_MAX_PX = from_int(80); // 80
export constexpr fix32 SIGHT_LEVEL_0_MAX_PY = from_int(60); // 60

export enum LookDir {
    CENTERED = 0,
    LOOK_LEFT   = 0b000001,
    LOOK_RIGHT  = 0b000010,
    /*PEEK_LEFT   = 0b000100,
    PEEK_RIGHT  = 0b001000,
    PEEK_UP     = 0b010000,
    PEEK_DOWN   = 0b100000,*/
};

constexpr u32 SHOOTING_ACOUNT = 24;

/// @brief Sector table. First/second line - down, then CCW.
constexpr fix32 ALINES[SHOOTING_ACOUNT + 1][2] = {
    {-519803,68433},   {-519803,-68433},  {-484379,-200636}, {-415946,-319166},
    {-319166,-415946}, {-200636,-484379}, {-68433,-519803},  {68433,-519803},
    {200636,-484379},  {319166,-415946},  {415946,-319166},  {484379,-200636},
    {519803,-68433},   {519803,68433},    {484379,200636},   {415946,319166},
    {319166,415946},   {200636,484379},   {68433,519803},    {-68433,519803},
    {-200636,484379},  {-319166,415946},  {-415946,319166},  {-484379,200636},
    {-519803,68433} /* last is the copy of first */ };

constexpr i32 PISTOL_MUZZLE_POS[SHOOTING_ACOUNT][2] = {
    {17,31}, {20,29}, {25,27}, {27,24}, {30,20}, {32,16}, {33,13}, {32,12},
    {31,10}, {29,7}, {25,4}, {19,2}, {17,1}, {16,2}, {14,3}, {11,5}, {9,8}, 
    {6,10}, {5,13}, {4,16}, {6,22}, {8,25}, {9,28}, {14,31} };

/// @brief ...
export struct Data {
    PlayerStats stats;
    player_gui::Data player_gui;
    bool init_gui;
    particles::Data* particles_ptr;

    u32 pistol_reload_timer;

    fix32 pos_x;
    fix32 pos_y;
    LookDir look_dir;

    // relative
    fix32 sight_pos_x;
    fix32 sight_pos_y;
    fix32 sight_max_px;
    fix32 sight_max_py;
    
    // the player position on the current sprite (in pixels)
    i32 origin_px;
    i32 origin_py;

    // collision map
    const CollisionMap* cmap_ptr;

    bool on_ground;
    bool long_jump;
    bool jump_up;
    fix32 jump_force;

    comp::Spatial* cargo_ptr;

    // entities

    Shared<Pool_ObjPalette> player_pal{{&data::pal_mainc}, 1};

    Shared<Pool_ObjTile_4_Placeholder> body_tile_pl{
            {data::mainc_tbank}, data::mainc_tcount};
    Shared<Pool_ObjTile_4_Placeholder> i_sight_tile_pl{
            {data::i_sight_tbank}, data::i_sight_tcount};
    Shared<Pool_ObjTile_4_Placeholder> sparkle_tile_pl{
            {data::sparkle_tbank}, data::sparkle_tcount};
    
    comp::Spatial spatials[PLAYER_ECOUNT] = {
        {FIX32_ZERO,FIX32_ZERO, 8*data::mainc_sx,   8*data::mainc_sy,   true},
        {FIX32_ZERO,FIX32_ZERO, 8*data::i_sight_sx, 8*data::i_sight_sy, false},
        {FIX32_ZERO,FIX32_ZERO, 8*data::sparkle_sx, 8*data::sparkle_sy, false}
    };

    comp::Visual visuals[PLAYER_ECOUNT] = {
        DVISUAL(player_pal, body_tile_pl, 64x64, 1, mainc, IDLE_LTR, fix32(20), true, 0),
        DVISUAL(player_pal, i_sight_tile_pl, 8x8, 0, i_sight, IDLE, fix32(70), false, 0),
        DVISUAL(player_pal, sparkle_tile_pl, 8x8, 0, sparkle, STAR, fix32(70), false, 0)
    };
};


// --------------------------------- Misc ---------------------------------- //

export inline bool is_shooting(const Data& data) {
    return data.spatials[E_SIGHT].visible;
}

export inline void hide_gui(Data& data) {
    player_gui::hide(data.player_gui);
}

export inline void show_gui(Data& data) {
    player_gui::show(data.player_gui);
}

export inline bool is_gui_hidden(Data& data) {
    return player_gui::is_hidden(data.player_gui);
}

export inline fix32 get_sight_gpos_x(Data& data) {
    return data.sight_pos_x + data.pos_x;
}

export inline fix32 get_sight_gpos_y(Data& data) {
    return data.sight_pos_y + data.pos_y;
}

export inline void set_health(Data& data, const u32 new_hbar_count) {
    data.stats.cur_health = clampi(new_hbar_count, u32(0), data.stats.max_health);
}

export inline void replenish_health(Data& data, const u32 amount) {
    set_health(data, data.stats.cur_health + amount);
}

export inline void drain_health(Data& data, const u32 amount) {
    set_health(data, data.stats.cur_health >= amount
               ? data.stats.cur_health - amount : u32(0));
}

export inline bool is_max_health(const Data& data) {
    return data.stats.cur_health == data.stats.max_health;
}

export comp::Spatial get_body_spatial(const Data& data) {
    return { data.pos_x - from_int(PLAYER_HEIGHT_PX >> 2), 
             data.pos_y - from_int(PLAYER_HEIGHT_PX),
             PLAYER_HEIGHT_PX >> 1, PLAYER_HEIGHT_PX, 
             true };
}

export i32 get_body_padding(const Data& /*data*/) {
    return 0;
}

export void add_respawn_burst(const Data& data) {
    // play respawn sound
    REG_SND4CNT  = CNT(0, Duty::D1_2, 2, EnvDir::INC, 10);
    REG_SND4FREQ = FREQ(Note::D4, HOLD, Reset::ON);
    particles::add_shield_burst(
            *data.particles_ptr, data.pos_x, 
            data.pos_y - from_int(PLAYER_HEIGHT_PX >> 1));
}

export void start_cargo_anim(Data& data, comp::Spatial* const cargo_ptr) {
    data.cargo_ptr = cargo_ptr;
}


// -------------------------------- Update --------------------------------- //

inline fix32 get_anim_speed(const data::Anim_mainc anim) {
    switch(anim) {
        case data::Anim_mainc::SHOOTING:
            return fix32(45); // ??
        case data::Anim_mainc::PICK_UP:
            return fix32(75*2); // ??
        case data::Anim_mainc::IDLE_LTR:
        case data::Anim_mainc::IDLE_RTL:
            return fix32(20); // <= 7 FPS 
        case data::Anim_mainc::RUN_LTR:
        case data::Anim_mainc::RUN_RTL:
        case data::Anim_mainc::RUN_TO_JUMP_LTR:
        case data::Anim_mainc::RUN_TO_JUMP_RTL:
        case data::Anim_mainc::LONG_JUMP_LTR:
        case data::Anim_mainc::LONG_JUMP_RTL:
            return fix32(68*2); // ~ 16 FPS 
        case data::Anim_mainc::JUMP_ROLL_LTR:
        case data::Anim_mainc::JUMP_ROLL_RTL:
            return fix32(80*2); // ~ ? FPS 
        case data::Anim_mainc::IDLE_LTR_TO_RUN_LTR:
        case data::Anim_mainc::IDLE_LTR_TO_RUN_RTL:
        case data::Anim_mainc::IDLE_RTL_TO_RUN_LTR:
        case data::Anim_mainc::IDLE_RTL_TO_RUN_RTL:
        case data::Anim_mainc::IDLE_RTL_TO_SHOOTING:
        case data::Anim_mainc::IDLE_LTR_TO_SHOOTING:
            return fix32(51*3); // ~12 FPS
        case data::Anim_mainc::RUN_LTR_TO_IDLE_LTR:
        case data::Anim_mainc::RUN_RTL_TO_IDLE_RTL:
            return fix32(60*2); // ~14 FPS
        case data::Anim_mainc::RUN_RTL_TO_RUN_LTR:
        case data::Anim_mainc::RUN_LTR_TO_RUN_RTL:
            return fix32(98*2); // ~23 FPS
        case data::Anim_mainc::FREEFALL_LTR:
        case data::Anim_mainc::FREEFALL_RTL:
        case data::Anim_mainc::FREEFALL_LANDING_LTR:
        case data::Anim_mainc::FREEFALL_LANDING_RTL:
            return fix32(70*2); // ~ 16 FPS
        case data::Anim_mainc::IDLE_TO_JUMP_RTL:
        case data::Anim_mainc::IDLE_TO_JUMP_LTR:
            return fix32(51*3); // ~12 FPS
        case data::Anim_mainc::HANGING_LTR:
        case data::Anim_mainc::HANGING_RTL:
            return fix32(30*2); // ~ ?? FPS 
        case data::Anim_mainc::CLIMB_UP_LTR:
        case data::Anim_mainc::CLIMB_UP_RTL:
            return fix32(20*2); // ~ ?? FPS 
    }
    return 0;
}

inline void change_anim(
        const data::Anim_mainc anim, 
        const bool loop,
        Data& data,
        const u32 from_frame=0,
        const i32 speed_shift=0) {
    if(data.visuals[E_BODY].anim_id == anim)
        return;
    comp::play_anim(
            data.visuals[E_BODY], anim, 
            data::mainc_frame_count[anim],
            from_frame, loop, get_anim_speed(anim) >> speed_shift,
            sys::director::VisualState::QUEUE_UPD_FRAME);
}


inline Msg shoot_pistol(
        GameData& /*game_data*/, 
        Data& data,
        const u32 frame_id) {
    if(data.stats.cur_pistol_ammo == 0)
        return Msg::NOTHING; // no ammo left
    --data.stats.cur_pistol_ammo;
    data.pistol_reload_timer = 0;

    // play shooting sound
    REG_SND4CNT  = CNT(0, Duty::D1_8, 0, EnvDir::DEC, 15);
    REG_SND4FREQ = FREQ(Note::D3, TIMED, Reset::ON);

    // play shooting animations
    data.spatials[E_SPARKLE].visible = true;
    data.spatials[E_SPARKLE].px = data.pos_x 
            + from_int(PISTOL_MUZZLE_POS[frame_id][0] 
                       - data.origin_px - 3);
    data.spatials[E_SPARKLE].py = data.pos_y 
            + from_int(PISTOL_MUZZLE_POS[frame_id][1] 
                       - data.origin_py - 3 + (64-48));
    comp::replay_anim(data.visuals[E_SPARKLE], 0,
            sys::director::VisualState::QUEUE_UPD_FRAME);
    comp::replay_anim(data.visuals[E_SIGHT], 0,
            sys::director::VisualState::QUEUE_UPD_FRAME);
    comp::play_anim(data.visuals[E_BODY], data::Anim_mainc::SHOOTING,
            4*frame_id + 4, 4*frame_id, false,
            get_anim_speed(data::Anim_mainc::SHOOTING),
            sys::director::VisualState::QUEUE_UPD_FRAME);

    return Msg::ON_SHOOT_PISTOL;
}


inline Msg update_shooting(
        GameData& game_data, 
        Data& data) {
    Msg res = Msg::NOTHING;
    // making sure we have the same amount of possible angles
    static_assert(
        4*SHOOTING_ACOUNT == data::mainc_frame_count[data::Anim_mainc::SHOOTING],
        "Oops! The number of `shooting` frames != `SHOOTING_ACOUNT`.");
    i32 dx = 0, dy = 0;
    if(is_key_down(Key::LEFT))  { dx -= 1; }
    if(is_key_down(Key::RIGHT)) { dx += 1; }
    if(is_key_down(Key::UP))    { dy -= 1; }
    if(is_key_down(Key::DOWN))  { dy += 1; }
    data.sight_pos_x = gba::clampi(
            data.sight_pos_x + dx*SIGHT_SPEED,
            -data.sight_max_px, data.sight_max_px);
    data.sight_pos_y = gba::clampi(
            data.sight_pos_y + dy*SIGHT_SPEED,
            SIGHT_INITIAL_Y - data.sight_max_py, 
            SIGHT_INITIAL_Y + data.sight_max_py);

    switch(data.visuals[E_BODY].anim_id) {
        case data::Anim_mainc::IDLE_RTL_TO_SHOOTING:
        case data::Anim_mainc::IDLE_LTR_TO_SHOOTING:
            if(comp::is_anim_done(data.visuals[E_BODY]) == false)
                return res;
            break;
        case data::Anim_mainc::IDLE_LTR:
            change_anim(data::Anim_mainc::IDLE_LTR_TO_SHOOTING, false, data);
            return res;
        case data::Anim_mainc::IDLE_RTL:
            change_anim(data::Anim_mainc::IDLE_RTL_TO_SHOOTING, false, data);
            return res;
        default:
            break;
    }

    if(data.visuals[E_BODY].anim_id != data::Anim_mainc::SHOOTING 
            || comp::is_anim_done(data.visuals[E_BODY])) {
        // getting the corresponding angle
        u32 frame_id = 0;
        fix32 prev_val = fix_mul(ALINES[0][0], data.sight_pos_x) -
                fix_mul(ALINES[0][1], data.sight_pos_y - SIGHT_INITIAL_Y);
        for(u32 i = 0; i < SHOOTING_ACOUNT; ++i) {
            const fix32 val = fix_mul(ALINES[i + 1][0], data.sight_pos_x) -
                    fix_mul(ALINES[i + 1][1], data.sight_pos_y - SIGHT_INITIAL_Y);
            if(prev_val <= FIX32_ZERO && val > FIX32_ZERO) {
                frame_id = i;
                break;
            }
            prev_val = val;
        }
        if(is_key_hit(KEY_SHOOT)) {
            // TODO: add other weapons
            res = shoot_pistol(game_data, data, frame_id);
        } else {
            data.spatials[E_SPARKLE].visible = false;
            // manually update the sprite frame to fit new direction
            comp::play_anim(
                    data.visuals[E_BODY], data::Anim_mainc::SHOOTING,
                    4*frame_id + 3, 4*frame_id + 3, false, 0,
                    sys::director::VisualState::QUEUE_UPD_FRAME);
        }
    }
    return res;
}

inline void start_shooting(
        GameData& game_data, 
        Data& data,
        const bool looking_left,
        const bool /*falling*/) {
    data.sight_pos_y = SIGHT_INITIAL_Y;
    data.sight_pos_x = (looking_left ? -1 : 1)*SIGHT_INITIAL_X;
    data.spatials[E_SIGHT].visible = true;
    update_shooting(game_data, data);
}

inline void end_shooting(
        GameData& /*game_data*/, 
        Data& data) {
    if(data.visuals[E_BODY].anim_id == data::Anim_mainc::SHOOTING 
            && !comp::is_anim_done(data.visuals[E_BODY]))
        return;
    data.spatials[E_SPARKLE].visible = false;
    data.spatials[E_SIGHT].visible = false;
    if(data.sight_pos_x >= FIX32_ZERO)
        change_anim(data::Anim_mainc::IDLE_LTR, true, data);
    else
        change_anim(data::Anim_mainc::IDLE_RTL, true, data);
}

export u32 update_input(
        u32 last_status, 
        GameData& game_data, 
        Data& data) {
    last_status = Msg::NOTHING;
    i32 dx = 0, dy = 0;
    i32 horizontal_speed = RUN_SPEED;
    bool move = false;
    bool jump = false;
    bool shooting = false;
    bool ready_to_long_jump = false;
    bool ready_to_jump_up = false;
    bool skip_vertical = false;
    
    // read the inputs
    if(is_key_down(Key::LEFT)) { dx -= 1; move = true; }
    if(is_key_down(Key::RIGHT)) { dx += 1; move = true; }
    if(is_key_down(KEY_SHOOTING_MODE)) { shooting = true; }
    if(is_key_hit(KEY_JUMP)) { jump = true; }
    
    // change the dx when in a jump
    if(data.long_jump) {
        move = true;
        jump = false;
        if(data.look_dir == LookDir::LOOK_RIGHT)
            dx = 1;
        else
            dx = -1;
    } else if(data.jump_up) {
        move = false;
        jump = false;
        dx = 0;
    } else {
        data.jump_force = 0;
    }

    // change the look direction
    if(move && dx != 0) {
        if(dx > 0) {
            data.look_dir = LookDir::LOOK_RIGHT;
        } else if(dx < 0) {
            data.look_dir = LookDir::LOOK_LEFT;
        }
    }

    // change the animation
    switch(data.visuals[E_BODY].anim_id) {
        case data::Anim_mainc::SHOOTING:
        case data::Anim_mainc::IDLE_LTR_TO_SHOOTING:
        case data::Anim_mainc::IDLE_RTL_TO_SHOOTING: 
            if(shooting)
                last_status = update_shooting(game_data, data);
            else
                end_shooting(game_data, data);
            [[fallthrough]];
        case data::Anim_mainc::PICK_UP:
            move = false;
            jump = false;
            dx = 0;
            ready_to_jump_up = false;
            skip_vertical = true;
            break;

        case data::Anim_mainc::IDLE_LTR:
            if(move) {
              switch(data.look_dir) {
                case LookDir::LOOK_LEFT:
                    change_anim(data::Anim_mainc::IDLE_LTR_TO_RUN_RTL, false, data);
                    break;
                case LookDir::LOOK_RIGHT:
                    change_anim(data::Anim_mainc::IDLE_LTR_TO_RUN_LTR, false, data);
                    break;
                case LookDir::CENTERED:
                    break;
              }
            } else {
                ready_to_jump_up = true;
                if(shooting)
                    start_shooting(game_data, data, false, false);
            }
            break;
        case data::Anim_mainc::IDLE_RTL:
            if(move) {
              switch(data.look_dir) {
                case LookDir::LOOK_LEFT:
                    change_anim(data::Anim_mainc::IDLE_RTL_TO_RUN_RTL, false, data);
                    break;
                case LookDir::LOOK_RIGHT:
                    change_anim(data::Anim_mainc::IDLE_RTL_TO_RUN_LTR, false, data);
                    break;
                case LookDir::CENTERED:
                    break;
              }
            } else {
                ready_to_jump_up = true;
                if(shooting)
                    start_shooting(game_data, data, true, false);
            }
            break;

        case data::Anim_mainc::RUN_LTR:
            if(data.look_dir == LookDir::LOOK_LEFT) {
                change_anim(data::Anim_mainc::RUN_LTR_TO_RUN_RTL, false, data);
            } else if(!move) {
                change_anim(data::Anim_mainc::RUN_LTR_TO_IDLE_LTR, false, data);
            } else if(is_key_down(Key::DOWN)) {
                change_anim(data::Anim_mainc::JUMP_ROLL_LTR, false, data);
            } else {
                ready_to_long_jump = true;
            }
            break;
        case data::Anim_mainc::RUN_RTL:
            if(data.look_dir == LookDir::LOOK_RIGHT) {
                change_anim(data::Anim_mainc::RUN_RTL_TO_RUN_LTR, false, data);
            } else if(!move) {
                change_anim(data::Anim_mainc::RUN_RTL_TO_IDLE_RTL, false, data);
            } else if(is_key_down(Key::DOWN)) {
                change_anim(data::Anim_mainc::JUMP_ROLL_RTL, false, data);
            } else {
                ready_to_long_jump = true;
            }

            break;

        case data::Anim_mainc::IDLE_LTR_TO_RUN_LTR:
        case data::Anim_mainc::IDLE_RTL_TO_RUN_LTR:
        case data::Anim_mainc::RUN_RTL_TO_RUN_LTR:
            if(comp::is_anim_done(data.visuals[E_BODY]))
                change_anim(data::Anim_mainc::RUN_LTR, true, data);
            break;

        case data::Anim_mainc::IDLE_LTR_TO_RUN_RTL:
        case data::Anim_mainc::IDLE_RTL_TO_RUN_RTL:
        case data::Anim_mainc::RUN_LTR_TO_RUN_RTL:
            if(comp::is_anim_done(data.visuals[E_BODY]))
                change_anim(data::Anim_mainc::RUN_RTL, true, data);
            break;

        case data::Anim_mainc::RUN_LTR_TO_IDLE_LTR:
            if(comp::is_anim_done(data.visuals[E_BODY]))
                change_anim(data::Anim_mainc::IDLE_LTR, true, data);
            break;
        case data::Anim_mainc::RUN_RTL_TO_IDLE_RTL:
            if(comp::is_anim_done(data.visuals[E_BODY]))
                change_anim(data::Anim_mainc::IDLE_RTL, true, data);
            break;

        case data::Anim_mainc::FREEFALL_LANDING_LTR:
            if(comp::is_anim_done(data.visuals[E_BODY]))
                change_anim(data::Anim_mainc::IDLE_LTR, true, data);
            break;
        case data::Anim_mainc::FREEFALL_LANDING_RTL:
            if(comp::is_anim_done(data.visuals[E_BODY]))
                change_anim(data::Anim_mainc::IDLE_RTL, true, data);
            break;

        case data::Anim_mainc::RUN_TO_JUMP_LTR:
            if(comp::is_anim_done(data.visuals[E_BODY]))
                change_anim(data::Anim_mainc::LONG_JUMP_LTR, true, data);
            break;
        case data::Anim_mainc::RUN_TO_JUMP_RTL:
            if(comp::is_anim_done(data.visuals[E_BODY]))
                change_anim(data::Anim_mainc::LONG_JUMP_RTL, true, data);
            break;

        case data::Anim_mainc::IDLE_TO_JUMP_LTR:
        case data::Anim_mainc::IDLE_TO_JUMP_RTL: 
            if(comp::is_anim_done(data.visuals[E_BODY])) {
                if(!data.jump_up)
                    data.jump_force = JUMP_UP_INITIAL_FORCE;
                data.jump_up = true;
                data.on_ground = false;
            } else {
                move = false;
                jump = false;
                dx = 0;
                ready_to_jump_up = false;
                skip_vertical = true;
            }
            break;

        case data::Anim_mainc::JUMP_ROLL_LTR:
            dx = 1;
            if(comp::is_anim_done(data.visuals[E_BODY]))
                change_anim(data::Anim_mainc::RUN_LTR, true, data);
            break;
        case data::Anim_mainc::JUMP_ROLL_RTL:
            dx = -1;
            if(comp::is_anim_done(data.visuals[E_BODY]))
                change_anim(data::Anim_mainc::RUN_RTL, true, data);
            break;

        case data::Anim_mainc::HANGING_LTR:
            if(is_key_down(KEY_JUMP)) {
                skip_vertical = true;
                dx = 0;
                if(is_key_hit(Key::UP))
                    change_anim(data::Anim_mainc::CLIMB_UP_LTR, false, data);
            } else
                change_anim(data::Anim_mainc::FREEFALL_LTR, true, data);
            break;
        case data::Anim_mainc::HANGING_RTL:
            if(is_key_down(KEY_JUMP)) {
                skip_vertical = true;
                dx = 0;
                if(is_key_hit(Key::UP))
                    change_anim(data::Anim_mainc::CLIMB_UP_RTL, false, data);
            } else
                change_anim(data::Anim_mainc::FREEFALL_RTL, true, data);
            break;

        case data::Anim_mainc::CLIMB_UP_RTL:
            if(comp::is_anim_done(data.visuals[E_BODY]))
                change_anim(data::Anim_mainc::FREEFALL_RTL, true, data);
            else {
                dx = 0;
                data.jump_force = from_int(-8);
            }
            break;
        case data::Anim_mainc::CLIMB_UP_LTR:
            if(comp::is_anim_done(data.visuals[E_BODY]))
                change_anim(data::Anim_mainc::FREEFALL_LTR, true, data);
            else {
                dx = 0;
                data.jump_force = from_int(-8);
            }
            break;
    }

    // change the move speed accordint to the animation
    switch(data.visuals[E_BODY].anim_id) {
        case data::Anim_mainc::IDLE_LTR:
        case data::Anim_mainc::IDLE_RTL:
        case data::Anim_mainc::IDLE_TO_JUMP_LTR:
        case data::Anim_mainc::IDLE_TO_JUMP_RTL:
        case data::Anim_mainc::HANGING_LTR:
        case data::Anim_mainc::HANGING_RTL:
        case data::Anim_mainc::SHOOTING:
            horizontal_speed = 0;
            break;
        case data::Anim_mainc::RUN_LTR:
        case data::Anim_mainc::RUN_RTL:
        case data::Anim_mainc::LONG_JUMP_LTR:
        case data::Anim_mainc::LONG_JUMP_RTL:
        case data::Anim_mainc::JUMP_ROLL_LTR:
        case data::Anim_mainc::JUMP_ROLL_RTL:
            horizontal_speed = RUN_SPEED;
            break;
        case data::Anim_mainc::IDLE_LTR_TO_RUN_LTR:
        case data::Anim_mainc::IDLE_RTL_TO_RUN_LTR:
            horizontal_speed = WALK_SPEED;
            break;
        case data::Anim_mainc::IDLE_LTR_TO_RUN_RTL:
        case data::Anim_mainc::IDLE_RTL_TO_RUN_RTL:
            horizontal_speed = WALK_SPEED;
            break;
        case data::Anim_mainc::RUN_LTR_TO_IDLE_LTR:
        case data::Anim_mainc::RUN_RTL_TO_IDLE_RTL:
            horizontal_speed = 0;
            break;
        case data::Anim_mainc::RUN_RTL_TO_RUN_LTR:
        case data::Anim_mainc::RUN_LTR_TO_RUN_RTL:
            horizontal_speed = 0;
            break;
        case data::Anim_mainc::FREEFALL_LTR:
        case data::Anim_mainc::FREEFALL_RTL:
            horizontal_speed = WALK_SPEED;
            break;
        case data::Anim_mainc::FREEFALL_LANDING_LTR:
        case data::Anim_mainc::FREEFALL_LANDING_RTL:
            horizontal_speed = 0;
            break;
        case data::Anim_mainc::RUN_TO_JUMP_LTR:
        case data::Anim_mainc::RUN_TO_JUMP_RTL:
            horizontal_speed = RUN_SPEED;
            break;
    }


    // applying the horizontal movement
    const fix32 new_px = data.pos_x + from_int(horizontal_speed * dx);
    if(data.on_ground) {
        if(get_collision_color(*data.cmap_ptr, intp(new_px), intp(data.pos_y)) == 
                CollisionColor::FREE) {
            data.pos_x = new_px;
            // checking if we dealing with an downward slope
            i32 px = intp(new_px);
            i32 py = intp(data.pos_y) + MAX_DOWN_SLOPE + 1;
            const i32 py_copy = py;
            if(find_first_from(
                    CollisionColor::FREE, *data.cmap_ptr, 
                    px, py, 0, -1, MAX_DOWN_SLOPE + 1)) {
                if(py_copy != py) {
                    data.pos_y = from_int(py);
                    //skip_vertical = true;
                    //ready_to_long_jump = false;
                    data.long_jump = false;
                }
            }
        } else {
            // checking if we dealing with an upward slope
            i32 px = intp(new_px);
            i32 py = intp(data.pos_y);
            if(find_first_from(
                    CollisionColor::FREE, *data.cmap_ptr, 
                    px, py, 0, -1, MAX_UP_SLOPE + 1)) {
                data.pos_y = from_int(py);
                data.pos_x = new_px;
                skip_vertical = true;
            }
            //ready_to_long_jump = false;
            data.long_jump = false;
        }
    } else {
        if(get_collision_color(*data.cmap_ptr, intp(new_px), intp(data.pos_y)) == 
                CollisionColor::FREE) {
            data.pos_x = new_px;
        }
    }

    if(skip_vertical == false) {
        // gravity
        dy = 1;
        i32 vertical_speed = 3;

        if(jump && ready_to_long_jump) {
            if(data.on_ground && move && (!data.long_jump)) {
                // perform a long jump
                data.long_jump = true;
                data.on_ground = false;
                data.jump_force = JUMP_INITIAL_FORCE;
                // switching the animation to run-to-jump
                switch(data.look_dir) {
                case LookDir::LOOK_LEFT:
                    change_anim(data::Anim_mainc::RUN_TO_JUMP_RTL, false, data);
                    break;
                case LookDir::LOOK_RIGHT:
                    change_anim(data::Anim_mainc::RUN_TO_JUMP_LTR, false, data);
                    break;
                case LookDir::CENTERED:
                    break;
                }
            }
        } else if(jump && ready_to_jump_up && (!data.jump_up)) {
            if(data.on_ground && (!move)) {
                // perform a jump up
                // switching the animation to idle-to-jump
                switch(data.look_dir) {
                case LookDir::LOOK_LEFT:
                    change_anim(data::Anim_mainc::IDLE_TO_JUMP_RTL, false, data);
                    break;
                case LookDir::LOOK_RIGHT:
                    change_anim(data::Anim_mainc::IDLE_TO_JUMP_LTR, false, data);
                    break;
                case LookDir::CENTERED:
                    break;
                }
            }
        }

        // applying the vertical movement
        
        const fix32 new_py = data.pos_y + data.jump_force + 
                from_int(vertical_speed * dy);

        if(data.long_jump || data.jump_up) {
            if(data.jump_force < 0) {
                // check for the ceiling
                if(get_collision_color(
                        *data.cmap_ptr, 
                        intp(data.pos_x), 
                        intp(new_py) - JUMP_CEILING_OFFSET) != CollisionColor::FREE) {
                    // we hit a ceiling
                    data.jump_force = from_int(-vertical_speed*dy) >> 1;//fix32(0);
                } else {
                    data.jump_force += JUMP_FORCE_ATTENUATION;
                }
            } else
                data.jump_force = fix32(0);
        } 

        // check if we can hang on a platform (during the jump-up)
        if(data.jump_up) {
            i32 px = intp(data.pos_x);
            const i32 coef = (data.look_dir == LookDir::LOOK_LEFT ? -1 : 1);
            const i32 delta = coef*PLATFORM_HANG_MAX_DX;
            i32 py = intp(new_py) - JUMP_CEILING_OFFSET;
            if(get_collision_color(*data.cmap_ptr, px + delta, py) == 
                    CollisionColor::PLATFORM) {
                if(get_collision_color(*data.cmap_ptr, px - delta, py) ==
                        CollisionColor::FREE) {
                    // hang on the platform
                    px += delta;
                    const i32 py_copy = py;
                    find_first_from(CollisionColor::FREE, *data.cmap_ptr,
                                    px, py, 0, -1, MAX_PLATFORM_HEIGHT);
                    //data.pos_y = from_int(py + JUMP_CEILING_OFFSET);
                    py = py_copy;
                    find_first_from(CollisionColor::FREE, *data.cmap_ptr,
                                    px, py, -coef, 0, 2*PLATFORM_HANG_MAX_DX);
                    data.pos_x = from_int(px + coef);
                    switch(data.look_dir) {
                    case LookDir::LOOK_LEFT:
                        change_anim(data::Anim_mainc::HANGING_RTL, true, data);
                        break;
                    case LookDir::LOOK_RIGHT:
                        change_anim(data::Anim_mainc::HANGING_LTR, true, data);
                        break;
                    case LookDir::CENTERED:
                        break;
                    }
                    data.long_jump = data.jump_up = false;
                    skip_vertical = true;
                }
            }
        }

        if(!skip_vertical) {
        if(get_collision_color(*data.cmap_ptr, intp(data.pos_x), intp(new_py)) == 
                CollisionColor::FREE) {
            data.pos_y = new_py;
            if(data.on_ground) {
                // switching the animation to freefall
                switch(data.look_dir) {
                case LookDir::LOOK_LEFT:
                    change_anim(data::Anim_mainc::FREEFALL_RTL, true, data);
                    break;
                case LookDir::LOOK_RIGHT:
                    change_anim(data::Anim_mainc::FREEFALL_LTR, true, data);
                    break;
                case LookDir::CENTERED:
                    break;
                }
            }
            data.on_ground = false;
        } else {
            if(data.on_ground == false) {
                // switching the animation to freefall landing
                if(data.long_jump) {
                    switch(data.look_dir) {
                    case LookDir::CENTERED:
                        break;
                    case LookDir::LOOK_LEFT:
                        if(is_key_down(Key::LEFT))
                            change_anim(data::Anim_mainc::JUMP_ROLL_RTL, false, data);
                        else
                            change_anim(data::Anim_mainc::FREEFALL_LANDING_RTL, false, data);
                        break;
                    case LookDir::LOOK_RIGHT:
                        if(is_key_down(Key::RIGHT))
                            change_anim(data::Anim_mainc::JUMP_ROLL_LTR, false, data);
                        else
                            change_anim(data::Anim_mainc::FREEFALL_LANDING_LTR, false, data);
                        break;
                    }
                } else {
                    switch(data.look_dir) {
                    case LookDir::CENTERED:
                        break;
                    case LookDir::LOOK_LEFT:
                        change_anim(data::Anim_mainc::FREEFALL_LANDING_RTL, false, data);
                        break;
                    case LookDir::LOOK_RIGHT:
                        change_anim(data::Anim_mainc::FREEFALL_LANDING_LTR, false, data);
                        break;
                    }
                }
                // finding the landing py
                i32 px = intp(data.pos_x);
                i32 py = intp(new_py);
                if(find_first_from(
                        CollisionColor::FREE, *data.cmap_ptr, 
                        px, py, 0, -1, PLAYER_HEIGHT_PX)) {
                    data.pos_y = from_int(py);
                }
            }
            data.on_ground = true;
            data.long_jump = false;
            data.jump_up = false;
        }
        }
    }

    // handling 0 shield/health
    if(data.on_ground && data.stats.cur_health <= 0) {
        switch(data.visuals[E_BODY].anim_id) {
            case data::Anim_mainc::FREEFALL_LANDING_LTR:
            case data::Anim_mainc::FREEFALL_LANDING_RTL:
                if(data.visuals[E_BODY].frame_id == 4)
                    add_respawn_burst(data);
                if(data.visuals[E_BODY].frame_id >= 8)
                    return Msg::ON_DEATH;
                break;
            default:
                switch(data.look_dir) {
                    case LookDir::LOOK_LEFT:
                        change_anim(data::Anim_mainc::FREEFALL_LANDING_RTL, 
                                    false, data, 0, 2);
                        break;
                    case LookDir::CENTERED:
                    case LookDir::LOOK_RIGHT:
                        change_anim(data::Anim_mainc::FREEFALL_LANDING_LTR, 
                                    false, data, 0, 2);
                        break;
                    }
                break;
        }
    }

    // handling cargo pick-up
    if(data.on_ground && data.cargo_ptr) {
        switch(data.visuals[E_BODY].anim_id) {
            case data::Anim_mainc::PICK_UP:
                data.look_dir = LookDir::LOOK_RIGHT;
                if(comp::is_anim_done(data.visuals[E_BODY]))
                    return Msg::CARGO_PICKED_UP;
                break;
            default:
                change_anim(data::Anim_mainc::PICK_UP, false, data, 0, 2);
                break;
        }
    }
    if(data.cargo_ptr) {
        data.cargo_ptr->px = data.pos_x - from_int(data.cargo_ptr->sx>>1);
        data.cargo_ptr->py = data.pos_y - from_int(
                PLAYER_HEIGHT_PX - (data.cargo_ptr->sy) + 18);
    }

    // oops, the player is out the map
    if(intp(data.pos_y) > PLAYER_HEIGHT_PX*2 + 8*data.cmap_ptr->size_ty) {
        add_respawn_burst(data);
        return Msg::ON_DEATH;
    }

    return last_status;
}

export u32 update_components(
        u32 last_status, 
        const u32 m_camera_px,
        const u32 m_camera_py,
        GameData& /*game_data*/, 
        Data& data) {
    // update body spatial component
    data.spatials[E_BODY].px = data.pos_x - from_int(data.origin_px);
    data.spatials[E_BODY].py = data.pos_y - from_int(data.origin_py);
    data.spatials[E_SIGHT].px = data.pos_x + data.sight_pos_x - from_int(4);
    data.spatials[E_SIGHT].py = data.pos_y + data.sight_pos_y - from_int(4);
    sys::director::update_all(PROJ_DX, PROJ_DY, m_camera_px, m_camera_py,
            PLAYER_ECOUNT, data.spatials, data.visuals);
    return last_status;
}

export void update_gui(
        GameData& /*game_data*/, 
        Data& /*data*/) {
    //player_gui::update(data.player_gui, data.stats);
}

export void update_weapons(
        GameData& /*game_data*/, 
        Data& data) {
    ++data.pistol_reload_timer;
    if(data.pistol_reload_timer >= PISTOL_RELOAD_SPEED) {
        data.pistol_reload_timer = 0;
        if(data.stats.cur_pistol_ammo < data.stats.max_pistol_ammo)
            data.stats.cur_pistol_ammo += 1;
    }
}

export u32 update(
        u32 last_status, 
        const u32 m_camera_px,
        const u32 m_camera_py,
        GameData& game_data, 
        Data& data) {
    update_weapons(game_data, data);
    u32 status = update_components(
            last_status, m_camera_px, m_camera_py, game_data, data);
    if(is_gui_hidden(data))
        show_gui(data);
    update_gui(game_data, data);
    return status;
}


// --------------------------------- Draw ---------------------------------- //

/// @brief ...
export u32 draw(u32 last_status, GameData& game_data, Data& data) {
    sys::director::redraw_all(PLAYER_ECOUNT, data.visuals);
    if(data.init_gui) {
        player_gui::init_vblank(data.player_gui);
        data.init_gui = false;
    }
    player_gui::draw(game_data, data.player_gui, data.stats);
    return last_status;
}


// --------------------------------- Init ---------------------------------- //

/// @brief ...
export u32 init(
        GameData& /*game_data*/, 
        Data& data,
        i32 checkpoint_px, 
        i32 checkpoint_py,
        const CollisionMap* const cmap_ptr,
        particles::Data* const particles_ptr) {
    player_gui::init(data.player_gui);
    find_first_from(
            CollisionColor::FREE, *cmap_ptr, 
            checkpoint_px, checkpoint_py, 0,-1, 1000);
    data.pos_x = from_int(checkpoint_px);
    data.pos_y = from_int(checkpoint_py);
    data.origin_px = 19;
    data.origin_py = 57;
    data.look_dir = LookDir::CENTERED;
    data.cmap_ptr = cmap_ptr;
    data.on_ground = false;
    data.long_jump = false;
    data.jump_up = false;
    data.jump_force = 0;
    data.sight_max_px = SIGHT_LEVEL_0_MAX_PX;
    data.sight_max_py = SIGHT_LEVEL_0_MAX_PY;
    data.stats.max_health = data.stats.cur_health = 4;
    data.stats.max_pistol_ammo = data.stats.cur_pistol_ammo = 4;
    data.init_gui = true;
    data.pistol_reload_timer = 0;
    data.particles_ptr = particles_ptr;
    data.cargo_ptr = 0;
    change_anim(data::Anim_mainc::IDLE_LTR, true, data);
    return 0;
}


export void free(GameData& /*game_data*/, Data& data) {
    sys::director::free_all(PLAYER_ECOUNT, data.visuals);
    player_gui::free_vblank(data.player_gui);
}

} // namespace player
} // namespace game
