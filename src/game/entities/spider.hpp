#ifndef SPIDER_HPP
#define SPIDER_HPP

#include "../../engine/components/visual.hpp"
#include "entity_types.hpp"
#include "common.hpp"

// MODULES: 
//  import data.mine_01
//  import data.spider
//
// REQUIRES:
//  ADD_MINE_PAL
//  + an additional mine_01

#define ADD_SPIDER_PLACEHOLDER(i) Shared<Pool_ObjTile_4_Placeholder> \
        spider_tile_pl_##i{{data::spider_tbank}, data::spider_tcount};

#define ADD_SPIDER_SPATIAL(x,y, visible) \
        { from_int(x), from_int(y), 8*data::spider_sx, 8*data::spider_sy, visible }

#define ADD_SPIDER_VISUAL(i) \
        DVISUAL(mine_pal, spider_tile_pl_##i, 32x32, 1, spider, IDLE, fix32(11), true, 0)

#define ADD_SPIDER_HEALTH(i) { 10, 10, false }

#define ADD_SPIDER_DAMAGE(i) { comp::ANY_DAMAGE }

#define ADD_SPIDER_ACTION(i) { comp::ON_DAMAGE | comp::ON_DEAD, \
        { ET_SPIDER, u32(0), u32(0), u32(0) } }

#define ADD_SPIDER_COLLISION(i) \
        { comp::BodyType::AABB, ET_NONINTERACTIVE, i32(-9) }

#endif
