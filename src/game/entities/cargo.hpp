#ifndef CARGO_HPP
#define CARGO_HPP

#include "../../engine/components/visual.hpp"
#include "entity_types.hpp"
#include "common.hpp"

// MODULES: 
//  import data.mine_01
//  import data.cargo

// PALETTES:
//  ADD_MINE_PAL

#define ADD_CARGO_PLACEHOLDER(i) Shared<Pool_ObjTile_4_Placeholder> \
        cargo_tile_pl_##i{{data::cargo_tbank}, data::cargo_tcount}

#define ADD_CARGO_SPATIAL(x,y, visible) \
        { from_int(x), from_int(y), \
          8*data::cargo_sx, 8*data::cargo_sy, \
          visible }

#define ADD_CARGO_VISUAL(i) \
        DVISUAL(mine_pal, cargo_tile_pl_##i, 16x8, 1, \
                cargo, TYPE_1, fix32(15), true, 0)

#define ADD_CARGO_HEALTH(i) { 1, 1, false }

#define ADD_CARGO_DAMAGE(i) { comp::NO_DAMAGE }

#define ADD_CARGO_ACTION(i) { 0, {ET_CARGO, u32(0)} } // 0

#define ADD_CARGO_COLLISION(i) \
        { comp::BodyType::AABB, ET_INTERACTIVE, i32(0) }

#endif
