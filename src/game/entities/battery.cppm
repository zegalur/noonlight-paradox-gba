module;
export module game.entities.battery;

import engine;
import game.particles;
import game.options;
import game.player;

using namespace gba;

namespace game {
namespace ent {
namespace battery {

export void on_player_collide(
        player::Data& player_data,
        comp::Spatial& sp,
        particles::Data& particles) {
    if(player::is_max_health(player_data))
        return;
    sp.visible = false;
    player::replenish_health(player_data, GREEN_BATTERY_REPLENISH_AMOUNT);
    particles::add_shield_burst(
            particles, sp.px + (sp.sx >> 1), sp.py + (sp.sy >> 1));
}

} // namespace battery
} // namespace ent
} // namespace game

