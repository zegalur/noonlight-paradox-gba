/*!****************************************************************************
 * @file map_template.cppm
 * @brief A template for a typical map.
 *******************************************************************************/        

module;
export module game.map_template;

import engine;

import game.data;
import game.player;
import game.options;
import game.particles;
import game.dialogue_box;
import game.music.common;

import game.entities.key;
import game.entities.mine;
import game.entities.cargo;
import game.entities.izone;
import game.entities.spider;
import game.entities.battery;
import game.entities.e_switch;

import data.map_data;
import data.b_button;
import data.i_respawn;
import data.items_palette;

#include "../../engine/components/visual.hpp"
#include "../entities/entity_types.hpp"

using namespace gba;
using namespace gba::sound;

namespace game {
namespace map_template {

export constexpr u32 NO_ID = ~u32(0);

export constexpr u32 B0_BG_LAYER = 1;
export constexpr u32 B0_BG_PRIORITY = 3;
export constexpr u32 B0_PCOUNT = 1;
export constexpr u32 B0_POFFSET = 12;
export constexpr u32 B0_CHARBLOCK = 2;
export constexpr u32 B0_CHARBLOCK_OFFSET = 0;
export constexpr u32 B0_SCREENBLOCK = 27;

export constexpr u32 B1_BG_LAYER = 2;
export constexpr u32 B1_BG_PRIORITY = 2;
export constexpr u32 B1_PCOUNT = 3;
export constexpr u32 B1_POFFSET = 13;
export constexpr u32 B1_CHARBLOCK = 2;
export constexpr u32 B1_CHARBLOCK_OFFSET = 256;
export constexpr u32 B1_SCREENBLOCK = 29;

export constexpr u32 M_BG_LAYER = 0;
export constexpr u32 M_BG_PRIORITY = 1;
export constexpr u32 M_POFFSET = 0;
export constexpr u32 M_CHARBLOCK = 0;
export constexpr u32 M_CHARBLOCK_OFFSET = 0;
export constexpr u32 M_SCREENBLOCK = 11;

export constexpr u32 F_BG_LAYER = 3;
export constexpr u32 F_BG_PRIORITY = 0;
export constexpr u32 F_BG_SX = 6;
export constexpr u32 F_BG_SY = 8;
export constexpr u32 F_CHARBLOCK = M_CHARBLOCK;
export constexpr u32 F_CHARBLOCK_OFFSET = 0x300;
export constexpr u32 F_SCREENBLOCK = 13;
export constexpr u32 F_EMPTY_TILE_ID = 0x2A0;

export constexpr u32 DB_PALETTE = 15;
export constexpr u32 DB_CHARBLOCK = F_CHARBLOCK;
export constexpr u32 DB_CHARBLOCK_OFFSET = 0x380;
export constexpr u32 DB_SCREENBLOCK = F_SCREENBLOCK;
export constexpr u32 DB_EMPTY_TILE_ID = F_EMPTY_TILE_ID;
export constexpr u32 DB_BG_LAYER = F_BG_LAYER;

export constexpr u32 TR_NONE = ~u32(0);

/// @brief Sets how many in-game frames we are waiting before the level intro.
export constexpr u32 INTRO_TIMER_VALUE = 60;

export constexpr u32 SND_VOLUME = 3;
export constexpr u32 MAP_SND_CHANNELS = sound::RNOISE | sound::LNOISE;

export constexpr u32 E_COUNT = 2;
export constexpr u32 E_CHECKPOINT_ICON = 0;
export constexpr u32 E_B_BUTTON = 1;

export constexpr u32 NO_CHECKPOINT = ~u32(0);
export constexpr u32 CHECKPOINT_TIME = 60*4; // 4 sec
export constexpr fix32 I_RESPAWN_SPEED = from_int(1) >> 3;
export constexpr fix32 B_BUTTON_SPEED = from_int(1) >> 5;
export constexpr fix32 B_BUTTON_PX = from_int(223);
export constexpr fix32 B_BUTTON_PY = from_int(143);
export constexpr i32 CHECKPOINT_RX = 16; // width / 2
export constexpr i32 CHECKPOINT_HY = 8*8; // height from the position
export constexpr i32 CHECKPOINT_DY = 7; // height (down)
export constexpr i32 CHECKPOINT_ICON_DY = 8*3;

export enum Status {
    RUNNING,
    RESPAWN_REQUEST,
    LOAD_REQUEST,
    QUEST_DONE,
    NEXT_MAP,
};

export enum State {
    IN_GAME,
    //PAUSED,
    DIALOGUE,
};

export struct Data {
    State state;
    State prev_state;

    TileMap background_0;
    TileMap background_1;
    TileMap main_layer;
    TileMask foreground;
    CollisionMap cmap;
    dlg_box::Data dbox_data;

    const data::MapData* map_data_ptr;
    player::Data player_data;
 
    u32 entity_count;
    comp::Spatial* spatials;
    comp::Visual* visuals;
    comp::Health* health;
    comp::Damage* damage;
    comp::Action* actions;
    comp::Collision* collision;
    
    fix32 b0_dx, b0_dy;
    fix32 b1_dx, b1_dy;
    fix32 b0_x_coef, b0_y_coef;
    fix32 b1_x_coef, b1_y_coef;
    
    /// @brief Foreground tilemask global position in pixels (x-coordinate).
    i32 foreground_global_px;
    /// @brief Foreground tilemask global position in pixels (y-coordinate).
    i32 foreground_global_py;

    /// @brief Activated trigger ID, or `TR_NONE` if none.
    u32 activated_trigger;

    /// @brief Timer for activating the level intro.
    u32 intro_timer;
    
    /// @brief Timer for the checkpoint animation.
    u32 checkpoint_timer;

    /// @brief The id of an activated izone, or `NO_ID`.
    u32 activated_izone_id;
    
    /// @brief The id of an activated switch, or `NO_ID`.
    u32 activated_switch_id;

    bool is_cargo_picked_up;
    bool is_cargo_music_played;
    u32 cargo_id;

    // intro music
    u32 intro_music_channels;
    const sound::Track* intro_track;
    u32 intro_track_plen;
    u32 intro_track_len;

    // common entities
    
    Shared<Pool_ObjPalette> items_pal{{&data::pal_items_palette}, 1};
    Shared<Pool_ObjPalette> b_button_pal{{&data::pal_b_button}, 1};

    Shared<Pool_ObjTile_4_Placeholder> i_respawn_tiles{
            {data::i_respawn_tbank}, data::i_respawn_tcount};
    Shared<Pool_ObjTile_4_Placeholder> b_button_tiles{
            {data::b_button_tbank}, data::b_button_tcount};

    comp::Spatial common_spatials[E_COUNT] = {
        { FIX32_ZERO,FIX32_ZERO, 
          8*data::i_respawn_sx, 8*data::i_respawn_sy, false }, // respawn icon
        { FIX32_ZERO, FIX32_ZERO, 
          8*data::b_button_sx, 8*data::b_button_sy, false } // B button
    };

    comp::Visual common_visuals[E_COUNT] = {
        DVISUAL(items_pal, i_respawn_tiles, 16x16, 0, 
                i_respawn, IDLE, I_RESPAWN_SPEED, true, 0),
        DVISUAL(b_button_pal, b_button_tiles, 16x16, 0, 
                b_button, IDLE, B_BUTTON_SPEED, true, 0),
    };

    // other
    
    particles::Data particles { player_data.player_pal, items_pal };
};


// --------------------------------- Init ---------------------------------- //

inline void init_entities(GameData& /*game_data*/, Data& data) {
    for(u32 i=0; i<data.entity_count; ++i) {
        switch(data.actions[i].data[ENTITY_TYPE].i) {
            case ET_SPIDER:
                ent::spider::on_init(&data.spatials[i]);
                break;
        }
    }
}

export u32 init(
        GameData& game_data, 
        Data& data,
        const bool is_respawn,

        const data::MapData* const map_data_ptr,
        u32 from_checkpoint,

        // entities
        const u32 entity_count,
        comp::Spatial* const spatials,
        comp::Visual* const visuals,
        comp::Health* const health,
        comp::Damage* const damage,
        comp::Action* const actions,
        comp::Collision* const collision,

        // b0 background
        const Palette* const b0_palette_array,
        const u32 b0_tile_count,
        const Tile_4* const b0_tile_array,
        const u32 b0_tmap_sx,
        const u32 b0_tmap_sy,
        const u32* const b0_screenblock_array,

        // b1 background
        const Palette* const b1_palette_array,
        const u32 b1_tile_count,
        const Tile_4* const b1_tile_array,
        const u32 b1_tmap_sx,
        const u32 b1_tmap_sy,
        const u32* const b1_screenblock_array,
        
        // main background
        const u32 main_pcount,
        const Palette* const main_palette_array,
        const Tile_4* const main_tile_array,
        const u32 main_tmap_sx,
        const u32 main_tmap_sy,
        const u32* const main_screenblock_array,
        const Tile_4* const fg_tile_array,
        const u32* const fg_screenblock_array,

        // collision map
        const Tile_4* const cmap_tile_array,
        const u32* cmap_screenblock_array,
        const u32 cmap_size_tx,
        const u32 cmap_size_ty,
        const Palette* const cmap_palette,

        // size constants
        const fix32 b0_to_main_x,
        const fix32 b0_to_main_y,
        const fix32 b1_to_main_x,
        const fix32 b1_to_main_y,

        // map intro info
        const u32 intro_music_channels,
        const sound::Track* const intro_track,
        const u32 intro_track_plen,
        const u32 intro_track_len
        ) {
    if(from_checkpoint == NO_CHECKPOINT)
        from_checkpoint = 0;

    const u32 checkpoint_px = map_data_ptr->checkpoints[from_checkpoint].pos_x;
    const u32 checkpoint_py = map_data_ptr->checkpoints[from_checkpoint].pos_y;

    data.map_data_ptr = map_data_ptr;

    data.entity_count = entity_count;
    data.spatials = spatials;
    data.visuals = visuals;
    data.health = health;
    data.damage = damage;
    data.actions = actions;
    data.collision = collision;

    data.is_cargo_picked_up = false;
    data.is_cargo_music_played = false;
    data.activated_izone_id = NO_ID;
    data.activated_switch_id = NO_ID;
    data.activated_trigger = TR_NONE;
    data.intro_timer = (is_respawn ? 0 : INTRO_TIMER_VALUE);

    game_data.session_data.checkpoint_id = NO_CHECKPOINT;
    data.common_spatials[E_CHECKPOINT_ICON].visible = false;
    data.checkpoint_timer = 0;

    // setting up the camera
    
    const i32 m_camera_px = i32(checkpoint_px) 
            - i32(SCREEN_WIDTH >> 1)
            + PROJ_DX;
    const i32 m_camera_py = i32(checkpoint_py) 
            - i32(SCREEN_HEIGHT >> 1) 
            - (player::PLAYER_HEIGHT_PX >> 1)
            + PROJ_DY;

    data.b0_x_coef = b0_to_main_x;
    data.b0_y_coef = b0_to_main_y;
    const i32 b0_camera_px = fix_mul(data.b0_x_coef, from_int(m_camera_px));
    const i32 b0_camera_py = fix_mul(data.b0_y_coef, from_int(m_camera_py));

    data.b1_x_coef = b1_to_main_x;
    data.b1_y_coef = b1_to_main_y;
    const i32 b1_camera_px = fix_mul(data.b1_x_coef, from_int(m_camera_px));
    const i32 b1_camera_py = fix_mul(data.b1_y_coef, from_int(m_camera_py));

    data.b0_dx = data.b0_dy = data.b1_dx = data.b1_dy = FIX32_ZERO;

    // collision map
    init_collision_map(data.cmap, cmap_tile_array, cmap_screenblock_array,
                       cmap_size_tx, cmap_size_ty, cmap_palette);

    // background #0
    init_tilemap(data.background_0, B0_BG_LAYER, B0_BG_PRIORITY,
            B0_POFFSET, B0_PCOUNT, b0_palette_array,
            B0_CHARBLOCK, B0_CHARBLOCK_OFFSET, b0_tile_count, b0_tile_array,
            B0_SCREENBLOCK, b0_tmap_sx, b0_tmap_sy, b0_screenblock_array,
            b0_camera_px, b0_camera_py);

    // background #1
    init_tilemap(data.background_1, B1_BG_LAYER, B1_BG_PRIORITY,
            B1_POFFSET, B1_PCOUNT, b1_palette_array,
            B1_CHARBLOCK, B1_CHARBLOCK_OFFSET, b1_tile_count, b1_tile_array,
            B1_SCREENBLOCK, b1_tmap_sx, b1_tmap_sy, b1_screenblock_array,
            b1_camera_px, b1_camera_py);

    // main color layer
    init_tilemap(data.main_layer, M_BG_LAYER, M_BG_PRIORITY,
            M_POFFSET, main_pcount, main_palette_array,
            M_CHARBLOCK, M_CHARBLOCK_OFFSET, 0, main_tile_array,
            M_SCREENBLOCK, main_tmap_sx, main_tmap_sy, main_screenblock_array,
            m_camera_px, m_camera_py);

    // initialize the foreground tilemask
    init_tilemask(data.foreground, F_BG_SX, F_BG_SY, F_BG_LAYER, F_BG_PRIORITY,
            F_CHARBLOCK, F_CHARBLOCK_OFFSET, F_EMPTY_TILE_ID, fg_tile_array,
            F_SCREENBLOCK, main_tmap_sx, main_tmap_sy, fg_screenblock_array);

    // initialize the dialogue box data
    dlg_box::init(data.dbox_data, 
            DB_PALETTE, DB_CHARBLOCK, 
            DB_CHARBLOCK_OFFSET, DB_SCREENBLOCK);

    // initialize other map objects
    player::init(game_data, data.player_data, 
                 i32(checkpoint_px), i32(checkpoint_py), 
                 &data.cmap, &data.particles);

    // initialize particles
    particles::init(data.particles);

    // initialize entities
    init_entities(game_data, data);

    data.state = IN_GAME;
    data.prev_state = IN_GAME;

    REG_DISPCNT = DCNT_MODE0 
                | DCNT_BG0 
                | DCNT_BG1 
                | DCNT_BG2 
                | DCNT_BG3 
                | DCNT_OBJ 
                | (1<<6);

    // setting up the sound
    data.intro_music_channels = intro_music_channels;
    data.intro_track = intro_track;
    data.intro_track_plen = intro_track_plen;
    data.intro_track_len = intro_track_len;
    if(!is_respawn)
        sound::set_dmg_volume(3,3, MAP_SND_CHANNELS);
    
    // add an energy burst on respawn and stop the respawn sound
    if(is_respawn) {
        player::add_respawn_burst(data.player_data);
        // play respawn sound
        REG_SND4CNT  = CNT(0, Duty::D1_8, 0, EnvDir::DEC, 15);
        REG_SND4FREQ = FREQ(Note::D3, TIMED, Reset::ON);
    }

    return RUNNING;
}


export void free(GameData& game_data, Data& data) {
    particles::free(data.particles);
    player::free(game_data, data.player_data);
    sys::director::free_all(data.entity_count, data.visuals);
    sys::director::free_all(E_COUNT, data.common_visuals);
}


// -------------------------------- Update --------------------------------- //

inline i32 clamp(const i32 x, const i32 min_x, const i32 max_x) {
    if(x < min_x) return min_x;
    if(x > max_x) return max_x;
    return x;
}

inline void play_pick_up_music() {
    sound::set_dmg_volume(3,3, MAP_SND_CHANNELS | music::pick_up_ch_mask);
    sound::play_music(music::pick_up_plen, music::pick_up_track,
            len(music::pick_up_track), false);
}

inline void update_camera(Data& data) {
    //i32 speed_0 = 1;
    i32 speed_f = (data.player_data.on_ground ? 2 : 3);

    // get the target point
    i32 target_x = intp(data.player_data.pos_x) + PROJ_DX;
    i32 target_y = intp(data.player_data.pos_y) + PROJ_DY;
    target_x -= SCREEN_WIDTH >> 1;
    target_y -= SCREEN_HEIGHT >> 1;

    if(player::is_shooting(data.player_data)) {
        target_x += intp(data.player_data.sight_pos_x);
        target_y += intp(data.player_data.sight_pos_y);
    } else {
        target_y -= player::PLAYER_HEIGHT_PX >> 1;
        // apply the player look
        switch(data.player_data.look_dir) {
            case player::LookDir::CENTERED:
                break;
            case player::LookDir::LOOK_RIGHT:
                target_x += player::PLAYER_LOOK_OFFSET;
                break;
            case player::LookDir::LOOK_LEFT:
                target_x -= player::PLAYER_LOOK_OFFSET;
                break;
        }
    }
    const i32 H_WND = (player::is_shooting(data.player_data) ? 100 : 0 );
    const i32 V_WND = (player::is_shooting(data.player_data) ?  60 : 0 );
    i32 delta_x = target_x - data.main_layer.camera_pos_x;
    i32 delta_y = target_y - data.main_layer.camera_pos_y;
    delta_x = abs(delta_x)<=H_WND ? 0 : delta_x - (delta_x>=0 ? H_WND : -H_WND);
    delta_y = abs(delta_y)<=V_WND ? 0 : delta_y - (delta_y>=0 ? V_WND : -V_WND);

    i32 tmp_x, tmp_y;
    tmp_x = clamp(delta_x, -speed_f, speed_f);
    tmp_y = clamp(delta_y, -speed_f, speed_f);
    shift_tilemap(data.main_layer, tmp_x, tmp_y);

    // shift the backgrounds
    
    data.b0_dx += fix_mul(data.b0_x_coef, from_int(tmp_x));
    data.b0_dy += fix_mul(data.b0_y_coef, from_int(tmp_y));
    data.b1_dx += fix_mul(data.b1_x_coef, from_int(tmp_x));
    data.b1_dy += fix_mul(data.b1_y_coef, from_int(tmp_y));

    tmp_x = intp(data.b0_dx);
    tmp_y = intp(data.b0_dy);
    shift_tilemap(data.background_0, tmp_x, tmp_y);
    data.b0_dx -= from_int(tmp_x);
    data.b0_dy -= from_int(tmp_y);

    tmp_x = intp(data.b1_dx);
    tmp_y = intp(data.b1_dy);
    shift_tilemap(data.background_1, tmp_x, tmp_y);
    data.b1_dx -= from_int(tmp_x);
    data.b1_dy -= from_int(tmp_y);
}

inline void update_foreground(Data& data) {
    data.foreground_global_px = intp(data.player_data.pos_x) - 8*(i32(F_BG_SX)>>2);
    data.foreground_global_py = intp(data.player_data.pos_y) - 8*(i32(F_BG_SY)-1);
    update_tilemask(data.foreground, 
                    data.foreground_global_px >> i32(3),
                    data.foreground_global_py >> i32(3));
    set_tilemask_bg_offset(data.foreground, 
        data.main_layer.camera_pos_x - (data.foreground_global_px & ~7),
        data.main_layer.camera_pos_y - (data.foreground_global_py & ~7));
}

inline void update_triggers(GameData& game_data, Data& data) {
    data.activated_trigger = TR_NONE;
    for(u32 i=0; i<data.map_data_ptr->tr_count; ++i) {
        if(game_data.session_data.trigger_flags & (u32(1) << i)) {
            const data::Trigger& tr = data.map_data_ptr->triggers[i];
            if(gba::abs(tr.pos_x - intp(data.player_data.pos_x)) > tr.size_px)
                continue;
            if(gba::abs(tr.pos_y - intp(data.player_data.pos_y)) > tr.size_px)
                continue;
            if(tr.oneshot)
                game_data.session_data.trigger_flags &= ~(u32(1) << i);
            data.activated_trigger = i;
            return;
        }
    }
}

inline void update_checkpoints(GameData& game_data, Data& data) {
    for(u32 i=0; i<data.map_data_ptr->cp_count; ++i) {
        const data::CheckPoint& cp = data.map_data_ptr->checkpoints[i];
        if(gba::abs(cp.pos_x - intp(data.player_data.pos_x)) > CHECKPOINT_RX)
            continue;
        if(cp.pos_y + CHECKPOINT_DY < intp(data.player_data.pos_y))
            continue;
        if(cp.pos_y - intp(data.player_data.pos_y) > CHECKPOINT_HY)
            continue;
        if(game_data.session_data.checkpoint_id == i)
            break;
        game_data.session_data.checkpoint_id = i;
        data.checkpoint_timer = CHECKPOINT_TIME;
        data.common_spatials[E_CHECKPOINT_ICON].px = from_int(cp.pos_x);
        data.common_spatials[E_CHECKPOINT_ICON].py = 
                from_int(cp.pos_y - CHECKPOINT_ICON_DY);
        data.common_spatials[E_CHECKPOINT_ICON].visible = true;
    }
    // hide the checkpoint icon after some time
    if(data.checkpoint_timer > 0) {
        data.common_spatials[E_CHECKPOINT_ICON].py -= from_int(1) >> 2;
        if(--data.checkpoint_timer == 0)
            data.common_spatials[E_CHECKPOINT_ICON].visible = false;
    }
}


inline void update_interactions(GameData& game_data, Data& data) {
    data.activated_izone_id = NO_ID;
    data.common_spatials[E_B_BUTTON].visible = false;
    auto on_collide = [&data, &game_data](const u32 id) {
        switch(data.actions[id].data[ENTITY_TYPE].i) {
            case ET_MINE_01:
                ent::mine::on_player_collide(
                        data.player_data, 
                        data.spatials[id], 
                        data.health[id], 
                        data.particles);
                break;
            case ET_BATTERY:
                ent::battery::on_player_collide(
                        data.player_data, 
                        data.spatials[id], 
                        data.particles);
                break;
            case ET_KEY:
                ent::key::on_player_collide(
                        data.spatials[id], 
                        data.actions[id], 
                        game_data);
                break;
            case ET_IZONE:
                ent::izone::on_player_collide(
                        data.actions[id], 
                        data.visuals[id]);
                // show B button hint
                data.common_spatials[E_B_BUTTON].visible = true;
                data.common_spatials[E_B_BUTTON].px = B_BUTTON_PX + 
                        from_int(data.main_layer.camera_pos_x) - from_int(PROJ_DX);
                data.common_spatials[E_B_BUTTON].py = B_BUTTON_PY + 
                        from_int(data.main_layer.camera_pos_y) - from_int(PROJ_DY);
                // player activated this izone
                if(is_key_hit(KEY_USE_IZONE))
                    data.activated_izone_id = id;
                break;
            case ET_CARGO:
                data.is_cargo_picked_up = 
                    ent::cargo::on_player_collide(
                        data.player_data, 
                        data.collision[id], 
                        data.actions[id], 
                        data.particles);
                if(data.is_cargo_picked_up) {
                    data.cargo_id = id;
                    player::start_cargo_anim(
                            data.player_data, &data.spatials[id]);
                }
                break;
        }
    };
    const comp::Spatial player_body_spatial = 
            player::get_body_spatial(data.player_data);
    const i32 player_body_padding = 
            player::get_body_padding(data.player_data);
    sys::collide_with_body(
            ET_INTERACTIVE,
            player_body_spatial,
            player_body_padding,
            data.entity_count,
            data.spatials,
            data.collision,
            on_collide);
}


template <typename OnDeadFunc>
inline void do_damage(
        OnDeadFunc& on_dead,
        Data& data, 
        const u32 damage_type,
        const u32 damage_amount) {
    auto on_damage = [&data](const u32 id, i32 px, i32 py) {
        switch(data.actions[id].data[ENTITY_TYPE].i) {
            case ET_MINE_01:
                ent::mine::on_damage(
                        data.spatials[id], px, py,
                        data.player_data.pos_x,
                        data.player_data.pos_y,
                        data.particles);
                break;
            case ET_SPIDER:
                ent::spider::on_damage(
                        data.spatials[id], px, py,
                        data.player_data.pos_x,
                        data.player_data.pos_y,
                        data.particles);
                break;
            case ET_SWITCH:
                data.activated_switch_id = id;
                ent::e_switch::on_damage(
                        data.spatials[id], data.visuals[id], 
                        data.actions[id], data.health[id],
                        px, py, data.player_data.pos_x, 
                        data.player_data.pos_y,
                        data.particles);
                break;
        }
    };
    sys::do_damage(damage_type, damage_amount,
            intp(player::get_sight_gpos_x(data.player_data)),
            intp(player::get_sight_gpos_y(data.player_data)),
            data.entity_count,
            data.spatials, data.health,
            data.damage, data.actions,
            on_damage, on_dead);
}


inline void update_entities(GameData& /*game_data*/, Data& data) {
    for(u32 i=0; i<data.entity_count; ++i) {
        switch(data.actions[i].data[ENTITY_TYPE].i) {
            case ET_MINE_01:
                ent::mine::on_update(
                        data.spatials[i], data.actions[i], 
                        data.health[i], data.cmap);
                break;
            case ET_SPIDER:
                ent::spider::on_update(
                        &data.spatials[i], &data.visuals[i], 
                        &data.actions[i], &data.health[i],
                        data.cmap, data.player_data);
                break;
            case ET_IZONE:
                ent::izone::on_update(
                        data.actions[i],
                        data.visuals[i]);
                break;
        }
    }
}


export u32 update_in_game(u32 last_status, GameData& game_data, Data& data) {
    auto on_dead = [&data](const u32 id, i32 /*px*/, i32 /*py*/) {
        switch(data.actions[id].data[ENTITY_TYPE].i) {
            case ET_MINE_01:
                ent::mine::on_dead(data.spatials[id], data.particles);
                break;
            case ET_SPIDER:
                ent::spider::on_dead(
                        data.spatials[id], data.actions[id], data.particles);
                break;
        }
    };
    sys::kill_health_0(data.entity_count, 
            data.spatials, data.health,
            data.actions, on_dead);

    u32 player_status = player::update_input(0, game_data, data.player_data);
    switch(player_status) {
        case player::Msg::NOTHING:
            break;
        case player::Msg::ON_DEATH:
            return Status::RESPAWN_REQUEST;
        case player::Msg::CARGO_PICKED_UP:
            last_status = Status::QUEST_DONE;
            if(!data.is_cargo_music_played) {
                play_pick_up_music();
                data.is_cargo_music_played = true;
            }
            break;
        case player::Msg::ON_SHOOT_PISTOL:
            do_damage(on_dead, data, PISTOL_DAMAGE_TYPE, PISTOL_DAMAGE_AMOUNT);
            break;
    }

    update_camera(data);
    update_entities(game_data, data);
    // WARNING: it is important to call `update_interactions` 
    //          after the `update_entities()`
    update_interactions(game_data, data);

    sys::director::update_all(PROJ_DX, PROJ_DY,
            data.main_layer.camera_pos_x, data.main_layer.camera_pos_y,
            E_COUNT, data.common_spatials, data.common_visuals);
    sys::director::update_all(PROJ_DX, PROJ_DY,
            data.main_layer.camera_pos_x, data.main_layer.camera_pos_y,
            data.entity_count, data.spatials, data.visuals);
    /*u32 player_comp_status =*/ player::update(0,
            data.main_layer.camera_pos_x,
            data.main_layer.camera_pos_y,
            game_data, data.player_data);

    update_triggers(game_data, data);
    update_checkpoints(game_data, data);

    return last_status;
}

export u32 update_dialogue(u32 last_status, GameData& game_data, Data& data) {
    if(dlg_box::update(game_data, data.dbox_data, DB_CHARBLOCK, DB_CHARBLOCK_OFFSET)) {
        // dialogue was finished
        dlg_box::free(data.dbox_data, 0, DB_SCREENBLOCK, DB_EMPTY_TILE_ID);
        data.state = State::IN_GAME;
        reset_tilemask(data.foreground);
    }
    return last_status;
}

void start_intro(GameData& /*game_data*/, Data& data) {
    // play intro music
    sound::set_dmg_volume(3,3, MAP_SND_CHANNELS | data.intro_music_channels);
    sound::play_music(data.intro_track_plen, data.intro_track, 
            data.intro_track_len, false);
}

export u32 update(u32 last_status, GameData& game_data, Data& data) {
    data.activated_izone_id = NO_ID;
    data.activated_switch_id = NO_ID;
    switch(data.state) {
        case State::IN_GAME:
            // dealing with the map intro
            if(data.intro_timer > 0) {
                if(--data.intro_timer == 0)
                    start_intro(game_data, data);
            }
            last_status = update_in_game(last_status, game_data, data);
            break;
        case State::DIALOGUE:
            last_status = update_dialogue(last_status, game_data, data);
            break;
    }
    // update particle system
    particles::update(data.particles, 
                      data.main_layer.camera_pos_x,
                      data.main_layer.camera_pos_y);
    // disable the music channels when music isn't playing
    if(sound::is_music_done())
        sound::set_dmg_volume(3,3, MAP_SND_CHANNELS);
    return last_status;
}

// --------------------------------- Draw ---------------------------------- //

export u32 draw(u32 last_status, GameData& game_data, Data& data) {
    switch(data.state) {
        case State::IN_GAME:
            sys::director::redraw_all(E_COUNT, data.common_visuals);
            sys::director::redraw_all(data.entity_count, data.visuals);
            set_tilemap_bg_offset(data.background_0);
            set_tilemap_bg_offset(data.background_1);
            set_tilemap_bg_offset(data.main_layer);
            player::draw(0, game_data, data.player_data);
            update_foreground(data);
            break;
        case State::DIALOGUE:
            if(data.prev_state != State::DIALOGUE) {
                player::hide_gui(data.player_data);
                player::draw(0, game_data, data.player_data);
                dlg_box::draw_first_time(data.dbox_data, 
                        DB_PALETTE, DB_CHARBLOCK, 
                        DB_CHARBLOCK_OFFSET, DB_SCREENBLOCK,
                        DB_EMPTY_TILE_ID, DB_BG_LAYER);
            } else {
                dlg_box::draw(game_data, data.dbox_data);
            }
            break;
    }
    particles::draw(data.particles);
    data.prev_state = data.state;
    return last_status;
}


// -------------------------------- Other ---------------------------------- //

export void start_dialogue(
        GameData& /*game_data*/, 
        Data& data,
        const dlg_box::TextBlock* const* const text_block
        ) {
    data.prev_state = data.state;
    data.state = State::DIALOGUE;
    data.activated_trigger = TR_NONE;
    dlg_box::set_text_block(data.dbox_data, text_block);
}

} // namespace map_template
} // namespace game

