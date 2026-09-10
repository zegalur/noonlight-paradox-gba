module;
export module game.entities.key;

import engine;
import game.options;
import game.data;

using namespace gba;

namespace game {
namespace ent {
namespace key {

export constexpr u32 DATA_COLOR = 1;
export constexpr u32 GREEN = 0;
export constexpr u32 BLUE = 1;
export constexpr u32 GOLD = 2;

export void on_player_collide(
        comp::Spatial& sp,
        comp::Action& a,
        GameData& game_data) {
    sp.visible = false;
    switch(a.data[DATA_COLOR].u) {
        case GREEN:
            game_data.session_data.key_flags |= GREEN_KEY_FLAG;
            break;
        case BLUE:
            game_data.session_data.key_flags |= BLUE_KEY_FLAG;
            break;
        case GOLD:
            game_data.session_data.key_flags |= GOLD_KEY_FLAG;
            break;
    }
}

} // namespace key
} // namespace ent
} // namespace game

