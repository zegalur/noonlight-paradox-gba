module;
export module game.player_stats;

import engine;

using namespace gba;

namespace game {

export struct PlayerStats {
    u32 max_health;
    u32 cur_health;

    u32 max_pistol_ammo;
    u32 cur_pistol_ammo;
};

} // game
