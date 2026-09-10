#ifndef KEY_HPP
#define KEY_HPP

#include "../../engine/components/visual.hpp"
#include "entity_types.hpp"
#include "common.hpp"

// MODULES: 
//  import data.key

// PALETTES:
//  TODO: use player GUI palette

#define ADD_KEY_PAL Shared<Pool_ObjPalette> key_pal{{&data::pal_key}, 1}

#define ADD_GREEN_KEY_PLACEHOLDER(i) Shared<Pool_ObjTile_4_Placeholder> \
        green_key_tile_pl_##i{{data::key_tbank}, data::key_tcount}

#define ADD_BLUE_KEY_PLACEHOLDER(i) Shared<Pool_ObjTile_4_Placeholder> \
        blue_key_tile_pl_##i{{data::key_tbank}, data::key_tcount}

#define ADD_GOLD_KEY_PLACEHOLDER(i) Shared<Pool_ObjTile_4_Placeholder> \
        gold_key_tile_pl_##i{{data::key_tbank}, data::key_tcount}

#define ADD_KEY_SPATIAL(x,y, visible) \
        { from_int(x - STD_PROJ_DX), from_int(y - STD_PROJ_DY), \
          8*data::key_sx, 8*data::key_sy, \
          visible }

#define ADD_GREEN_KEY_VISUAL(i) \
        DVISUAL(key_pal, green_key_tile_pl_##i, 8x8, 1, \
                key, GREEN, fix32(17), false, 0)

#define ADD_BLUE_KEY_VISUAL(i) \
        DVISUAL(key_pal, blue_key_tile_pl_##i, 8x8, 1, \
                key, BLUE, fix32(17), false, 0)

#define ADD_GOLD_KEY_VISUAL(i) \
        DVISUAL(key_pal, gold_key_tile_pl_##i, 8x8, 1, \
                key, GOLD, fix32(17), false, 0)

#define ADD_KEY_HEALTH(i) { ~u32(0), ~u32(0), false }

#define ADD_KEY_DAMAGE(i) { comp::NO_DAMAGE }

#define ADD_GREEN_KEY_ACTION(i) { 0, {ET_KEY, u32(KEY_TYPE_GREEN)} }
#define ADD_BLUE_KEY_ACTION(i) { 0, {ET_KEY, u32(KEY_TYPE_BLUE)} }
#define ADD_GOLD_KEY_ACTION(i) { 0, {ET_KEY, u32(KEY_TYPE_GOLD)} }

#define ADD_KEY_COLLISION(i) \
        { comp::BodyType::AABB, ET_INTERACTIVE, i32(-1) }

#endif
