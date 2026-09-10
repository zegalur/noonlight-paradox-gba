#ifndef MINE_HPP
#define MINE_HPP

#include "../../engine/components/visual.hpp"
#include "entity_types.hpp"
#include "common.hpp"

// MODULES: 
//  import data.mine_01

#define ADD_MINE_PAL Shared<Pool_ObjPalette> mine_pal{{&data::pal_mine_01}, 1}

#define ADD_MINE_01_PLACEHOLDER(i) Shared<Pool_ObjTile_4_Placeholder> \
        mine_tile_pl_##i{{data::mine_01_tbank}, data::mine_01_tcount}

#define ADD_MINE_01_SPATIAL(x,y, visible) \
        { from_int(x), from_int(y), 8*data::mine_01_sx, 8*data::mine_01_sy, visible }

#define ADD_MINE_01_SPATIAL_HIDDEN(x,y, visible) ADD_MINE_01_SPATIAL(x,y, false)

#define ADD_MINE_01_VISUAL(i) \
        DVISUAL(mine_pal, mine_tile_pl_##i, 8x8, 1, mine_01, IDLE, FIX32_ONE, false, 0)

#define ADD_MINE_01_HEALTH(i) { 9, 9, false }
#define ADD_MINE_01_HEALTH_ONESHOT(i) { 1, 1, false }

#define ADD_MINE_01_DAMAGE(i) { comp::ANY_DAMAGE }

#define ADD_MINE_01_ACTION(i) { comp::ON_DAMAGE | comp::ON_DEAD, {ET_MINE_01,0,0,false} }

#define ADD_MINE_01_COLLISION(i) \
        { comp::BodyType::AABB, ET_INTERACTIVE, i32(0) }

#endif
