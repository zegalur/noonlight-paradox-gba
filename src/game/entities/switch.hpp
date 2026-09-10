#ifndef SWITCH_HPP
#define SWITCH_HPP

#include "../../engine/components/visual.hpp"
#include "entity_types.hpp"
#include "common.hpp"

// MODULES: 
//  import data.mine_01
//  import data.switch_01

// PALETTES:
//  ADD_MINE_PAL

// Switch 01 - activates via shooting

#define ADD_SWITCH_01_ON_PLACEHOLDER(i) Shared<Pool_ObjTile_4_Placeholder> \
        switch_01_on_tile_pl_##i{{data::switch_01_tbank}, data::switch_01_tcount}

#define ADD_SWITCH_01_OFF_PLACEHOLDER(i) Shared<Pool_ObjTile_4_Placeholder> \
        switch_01_off_tile_pl_##i{{data::switch_01_tbank}, data::switch_01_tcount}

#define ADD_SWITCH_01_SPATIAL(x,y, visible) \
        { from_int(x - STD_PROJ_DX), from_int(y - STD_PROJ_DY), \
          8*data::switch_01_sx, 8*data::switch_01_sy, \
          visible }

#define ADD_SWITCH_01_ON_VISUAL(i) \
        DVISUAL(mine_pal, switch_01_on_tile_pl_##i, 8x8, 1, \
                switch_01, ON, fix32(15), false, 0)

#define ADD_SWITCH_01_OFF_VISUAL(i) \
        DVISUAL(mine_pal, switch_01_off_tile_pl_##i, 8x8, 1, \
                switch_01, OFF, fix32(15), false, 0)

#define ADD_SWITCH_01_HEALTH(i) { ~u32(0), ~u32(0), false }

#define ADD_SWITCH_01_DAMAGE(i) { comp::ANY_DAMAGE }

#define ADD_SWITCH_01_ON_ACTION(i) { comp::ON_DAMAGE, {ET_SWITCH, true} }
#define ADD_SWITCH_01_OFF_ACTION(i) { comp::ON_DAMAGE, {ET_SWITCH, false} }

#define ADD_SWITCH_01_COLLISION(i) \
        { comp::BodyType::AABB, ET_NONINTERACTIVE, i32(0) }

#endif
