/*!****************************************************************************
 * @file particles.cppm
 * @brief Temporary particle system.
 * TODO: Replace with a better one.
 *******************************************************************************/        

module;
export module game.particles;

import engine;
import game.options;
import data.sparkle;
import data.explosion_01;
import data.shield_burst;
import data.sand;

#include "../engine/components/visual.hpp"

using namespace gba;

namespace game {
namespace particles {

export constexpr u32 E_COUNT        = 4;
export constexpr u32 E_SPARKLE_T1   = 0;
export constexpr u32 E_EXPLOSION_T1 = 1;
export constexpr u32 E_SHIELD_BURST = 2;
export constexpr u32 E_SAND         = 3;

export constexpr u32 SPARKLE_T1_SPEED = fix32(70);
export constexpr u32 EXPLOSION_T1_SPEED = from_int(1) >> 2;
export constexpr u32 SHIELD_BURST_SPEED = from_int(1) >> 3;
export constexpr u32 SAND_SPEED = from_int(1) >> 2;

export struct Data {
    Shared<Pool_ObjPalette> explosion_pal;
    Shared<Pool_ObjTile_4_Placeholder> sparkle_tile_pl;
    Shared<Pool_ObjTile_4_Placeholder> explosion_tile_pl;
    Shared<Pool_ObjTile_4_Placeholder> shield_b_tile_pl;
    Shared<Pool_ObjTile_4_Placeholder> sand_tile_pl;
    comp::Spatial spatials[E_COUNT];
    comp::Visual visuals[E_COUNT];
    
    Data(Shared<Pool_ObjPalette>& player_pal, 
         Shared<Pool_ObjPalette>& items_pal) :
      explosion_pal{ {&data::pal_explosion_01}, 1 },
      sparkle_tile_pl { {data::sparkle_tbank}, data::sparkle_tcount },
      explosion_tile_pl{ {data::explosion_01_tbank}, data::explosion_01_tcount },
      shield_b_tile_pl{ {data::shield_burst_tbank}, data::shield_burst_tcount },
      sand_tile_pl{ {data::sand_tbank}, data::sand_tcount },
      spatials {
        {FIX32_ZERO,FIX32_ZERO, 8*data::sparkle_sx, 8*data::sparkle_sy, false},
        {FIX32_ZERO,FIX32_ZERO, 8*data::explosion_01_sx, 8*data::explosion_01_sy, false},
        {FIX32_ZERO,FIX32_ZERO, 8*data::shield_burst_sx, 8*data::shield_burst_sy, false},
        {FIX32_ZERO,FIX32_ZERO, 8*data::sand_sx, 8*data::sand_sy, false},
      },
      visuals {
        DVISUAL(player_pal, sparkle_tile_pl, 8x8, 0, 
                sparkle, FALLING, SPARKLE_T1_SPEED, false, 0),
        DVISUAL(explosion_pal, explosion_tile_pl, 32x32, 1, 
                explosion_01, IDLE, EXPLOSION_T1_SPEED, false, 0),
        DVISUAL(items_pal, shield_b_tile_pl, 16x32, 0, 
                shield_burst, IDLE, SHIELD_BURST_SPEED, false, 0),
        DVISUAL(items_pal, sand_tile_pl, 16x32, 0, 
                sand, IDLE, SAND_SPEED, true, 0),
      }
    { /* do nothing */ }
};

export void init(Data& /*data*/) {
}

export void free(Data& data) {
    sys::director::free_all(E_COUNT, data.visuals);
}

export void update(
        Data& data,
        const i32 cam_pos_x,
        const i32 cam_pos_y) {
    for(u32 i=0; i<E_COUNT; ++i)
        if(is_anim_done(data.visuals[i]))
            data.spatials[i].visible = false;
    sys::director::update_all(
            PROJ_DX, PROJ_DY,
            cam_pos_x, cam_pos_y, 
            E_COUNT, data.spatials, data.visuals);
}

export void draw(Data& data) {
    sys::director::redraw_all(E_COUNT, data.visuals);
}

inline void add_particle(const u32 eid, Data& data, fix32 px, fix32 py) {
    data.spatials[eid].visible = true;
    data.spatials[eid].px = px;
    data.spatials[eid].py = py;
    comp::replay_anim(data.visuals[eid], 0, 
            sys::director::VisualState::QUEUE_UPD_FRAME);
}

export void add_sparkle(Data& data, fix32 px, fix32 py) {
    add_particle(E_SPARKLE_T1, data, px - from_int(3), py);
}

export void add_explosion(Data& data, fix32 px, fix32 py) {
    add_particle(E_EXPLOSION_T1, data, px - from_int(16), py - from_int(16));
}

export void add_shield_burst(Data& data, fix32 px, fix32 py) {
    add_particle(E_SHIELD_BURST, data, px - from_int(8), py - from_int(16));
}

export void add_sand(Data& data, fix32 px, fix32 py) {
    add_particle(E_SAND, data, px, py);
}

} // namespace particles
} // namespace game
