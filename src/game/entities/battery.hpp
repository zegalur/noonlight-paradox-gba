#ifndef BATTERY_HPP
#define BATTERY_HPP

#include "../../engine/components/visual.hpp"
#include "entity_types.hpp"
#include "common.hpp"

// MODULES: 
//  import data.items_palette
//  import data.green_battery

// PALETTES:
//  ADD_ITEMS_PALETTE -> items_pal

#define BATTERY_SPEED (from_int(1)>>1)

#define ADD_BATTERY_PLACEHOLDER(i) Shared<Pool_ObjTile_4_Placeholder> \
        battery_tile_pl_##i{{data::green_battery_tbank}, data::green_battery_tcount}

#define ADD_BATTERY_SPATIAL(x,y, visible) \
        { from_int(x), from_int(y), \
          8*data::green_battery_sx, 8*data::green_battery_sy, \
          visible }

#define ADD_BATTERY_VISUAL(i) \
        DVISUAL(items_pal, battery_tile_pl_##i, 16x16, 1, \
                green_battery, IDLE, BATTERY_SPEED, true, 0)

#define ADD_BATTERY_HEALTH(i) { 1, 1, false }

#define ADD_BATTERY_DAMAGE(i) { comp::NO_DAMAGE }

#define ADD_BATTERY_ACTION(i) { 0, {ET_BATTERY} }

#define ADD_BATTERY_COLLISION(i) \
        { comp::BodyType::AABB, ET_INTERACTIVE, i32(-4) }

#endif
