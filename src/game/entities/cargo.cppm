module;
export module game.entities.cargo;

import engine;
import game.particles;
import game.options;
import game.player;

#include "entity_types.hpp"

using namespace gba;

namespace game {
namespace ent {
namespace cargo {

export constexpr u32 DATA_FLAGS = 1;
export constexpr u32 CAN_PICK_UP_FLAG = 0b1;

export void set_can_pick_up(comp::Action& action) {
    action.data[DATA_FLAGS].u |= CAN_PICK_UP_FLAG;
}

export bool on_player_collide(
        player::Data& /*player_data*/,
        comp::Collision& c,
        const comp::Action& action,
        particles::Data& /*particles*/) {
    if(!(action.data[DATA_FLAGS].u & CAN_PICK_UP_FLAG))
        return false;
    c.mask = ET_NONINTERACTIVE;
    return true;
}

} // namespace cargo
} // namespace ent
} // namespace game

