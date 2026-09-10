module;
export module game.map.factory_01;

import engine;
import game.data;
import game.text;
import game.map_template;
import game.music.factory;

import data.factory_01;
import data.factory_01.info;
import data.factory_01_shapes;
import data.blue_mountains_b0;
import data.blue_mountains_b1;

import data.items_palette;
import data.green_battery;

#include "map_template_macro.hpp"

using namespace gba;

namespace game {
namespace map {
namespace factory_01 {

export struct Data {
    data::Entities_factory_01 entities;
    map_template::Data map_template_data;
    bool show_thank_you_dialogue;
};

export u32 init(
        GameData& game_data, 
        Data& data, 
        const u32 from_checkpoint,
        const bool is_respawn) {
    data.show_thank_you_dialogue = !is_respawn;
    return map_template::init(
            game_data, data.map_template_data, is_respawn,
            &data::factory_01_data, from_checkpoint,
            MAP_ENTITIES(data.entities),
            MAP_BX_LAYER(data::blue_mountains_b0),
            MAP_BX_LAYER(data::blue_mountains_b1),
            MAP_MAIN_LAYER(data::factory_01),
            MAP_COLLISION_MAP(data::factory_01),
            SIZE_CONSTANTS(data::blue_mountains_b0, data::factory_01),
            SIZE_CONSTANTS(data::blue_mountains_b1, data::factory_01),
            INTRO_MUSIC(music::factory_1)
            );
}

export void free(GameData& game_data, Data& data) {
    return map_template::free(game_data, data.map_template_data);
}


export u32 update(u32 last_status, GameData& game_data, Data& data) {
    u32 s = map_template::update(last_status, game_data, data.map_template_data);
    if(data.show_thank_you_dialogue) {
        data.show_thank_you_dialogue = false;
        map_template::start_dialogue(
                game_data, data.map_template_data, TB_THANK_YOU);
    }
    return s;
}


export u32 draw(u32 last_status, GameData& game_data, Data& data) {
    return map_template::draw(last_status, game_data, data.map_template_data);
}

} // namespace factory_01
} // namespace map
} // namespace game

