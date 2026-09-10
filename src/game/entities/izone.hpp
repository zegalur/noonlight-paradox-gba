#ifndef IZONE_HPP
#define IZONE_HPP

#include "../../engine/components/visual.hpp"
#include "entity_types.hpp"
#include "common.hpp"

// MODULES: 
//  import data.mine_01
//  import data.izone

// REQUIRES:
//  ADD_MINE_PAL

#define ADD_IZONE_PLACEHOLDER(i) Shared<Pool_ObjTile_4_Placeholder> \
        izone_tile_pl_##i{{data::izone_tbank}, data::izone_tcount};

#define ADD_IZONE_SPATIAL(x,y, visible) \
        { from_int(x - STD_PROJ_DX - 4), from_int(y - STD_PROJ_DY - 8), \
        8*data::izone_sx, 8*data::izone_sy, visible }

#define ADD_IZONE_VISUAL(i) \
DVISUAL(mine_pal, izone_tile_pl_##i, 8x8, 0, izone, HIDDEN, fix32(13), false, 0)

#define ADD_IZONE_HEALTH(i) { 1, 1, false }

#define ADD_IZONE_DAMAGE(i) { comp::NO_DAMAGE }

#define ADD_IZONE_ACTION(i) { 0, { ET_IZONE } }

#define ADD_IZONE_COLLISION(i) \
        { comp::BodyType::AABB, ET_INTERACTIVE, i32(5) }

#endif
