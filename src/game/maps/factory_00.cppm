module;
export module game.map.factory_00;

import engine;
import game.data;
import game.text;
import game.player;
import game.particles;
import game.map_template;
import game.dialogue_box;
import game.music.factory;
import game.entities.cargo;

import data.factory_00;
import data.factory_00.info;
import data.factory_00_shapes;
import data.blue_mountains_b0;
import data.blue_mountains_b1;

import data.items_palette;
import data.green_battery;

#include "map_template_macro.hpp"

using namespace gba;

namespace game {
namespace map {
namespace factory_00 {

constexpr u32 DATA_MAGNETS_OFF = 0;

// password is: 1,3,0,1,0,3,0,1
constexpr u32 password = (u32(1) << u32(7*4))
                       | (u32(3) << u32(6*4))
                       | (u32(0) << u32(5*4))
                       | (u32(1) << u32(4*4))
                       | (u32(0) << u32(3*4))
                       | (u32(3) << u32(2*4))
                       | (u32(0) << u32(1*4))
                       | (u32(1) << u32(0*4));

export struct Data {
    data::Entities_factory_00 entities;
    map_template::Data map_template_data;
    bool show_respawn_dialogue;
    bool final_dialogue_done;
    u32 cur_pass;
};


// --------------------------------- Init ---------------------------------- //

export u32 init(
        GameData& game_data, 
        Data& data, 
        /*const */u32 from_checkpoint,
        const bool is_respawn) {
    data.cur_pass = u32(0);
    // DELETE THIS ()
    //from_checkpoint = data::CP_factory_00::CP_TMP;
    // !!!
    data.final_dialogue_done = false;
    data.show_respawn_dialogue = 
        is_respawn && !(game_data.tmp_flags & TmpFlags::TUT_RESPAWN);
    const u32 status = map_template::init(
            game_data, data.map_template_data, is_respawn,
            &data::factory_00_data, from_checkpoint,
            MAP_ENTITIES(data.entities),
            MAP_BX_LAYER(data::blue_mountains_b0),
            MAP_BX_LAYER(data::blue_mountains_b1),
            MAP_MAIN_LAYER(data::factory_00),
            MAP_COLLISION_MAP(data::factory_00),
            SIZE_CONSTANTS(data::blue_mountains_b0, data::factory_00),
            SIZE_CONSTANTS(data::blue_mountains_b1, data::factory_00),
            INTRO_MUSIC(music::factory_1)
            );
    if(from_checkpoint == data::CP_factory_00::CP_00_START
            || from_checkpoint == map_template::NO_CHECKPOINT)
        // for the sake of the tutorial we set the health bar to 1
        player::set_health(data.map_template_data.player_data, 1);
    // hide the keys
    data.entities.spatials[data::E_factory_00::GREEN_KEY].visible = false;
    data.entities.spatials[data::E_factory_00::BLUE_KEY].visible = false;
    // turn off the magnets when they were turned off previously
    if(game_data.session_data.data[DATA_MAGNETS_OFF].b) {
        ent::cargo::set_can_pick_up(
                data.entities.actions[data::E_factory_00::CARGO]);
    }
    return status;
}


export void free(GameData& game_data, Data& data) {
    return map_template::free(game_data, data.map_template_data);
}


// -------------------------------- Update --------------------------------- //

inline void upd_green_key(GameData& game_data, Data& data) {
    // add green key when SPIDER_02 is dead
    if(!(game_data.session_data.key_flags & GREEN_KEY_FLAG)) {
        auto& green_key_sp = data.entities.spatials[data::E_factory_00::GREEN_KEY];
        auto& spider_02_sp = data.entities.spatials[data::E_factory_00::SPIDER_02];
        if(!green_key_sp.visible)
            if(data.entities.health[data::E_factory_00::SPIDER_02].is_dead) {
                green_key_sp.visible = true;
                green_key_sp.px = spider_02_sp.px + from_int(16);
                green_key_sp.py = spider_02_sp.py + from_int(16);
            }
    }
}

inline void upd_music_password_computer(GameData& game_data, Data& data) {
    // music password computer logic
    if(data.map_template_data.activated_izone_id == data::E_factory_00::MUSIC_IZONE) {
        if(game_data.session_data.key_flags & GREEN_KEY_FLAG) {
            // play password sound
            sound::set_dmg_volume(3,3,
                    map_template::MAP_SND_CHANNELS | music::factory_password_ch_mask);
            sound::play_music(music::factory_password_plen, music::factory_password, 
                    len(music::factory_password), false);
            map_template::start_dialogue(
                    game_data, data.map_template_data, TB_CPASSM);
        } else {
            // show green card dialogue
            map_template::start_dialogue(
                    game_data, data.map_template_data, TB_CKEY_TUT);
        }
    }
}

inline void upd_magnet_computer(GameData& game_data, Data& data) {
    if(data.map_template_data.activated_izone_id == data::E_factory_00::MAGNET_IZONE) {
        if(game_data.session_data.key_flags & BLUE_KEY_FLAG) {
            game_data.session_data.data[DATA_MAGNETS_OFF].b = true;
            ent::cargo::set_can_pick_up(
                    data.entities.actions[data::E_factory_00::CARGO]);
            // show magnet off dialogue
            map_template::start_dialogue(
                    game_data, data.map_template_data, TB_MAGNET_OFF);
        } else {
            // show green card dialogue
            map_template::start_dialogue(
                    game_data, data.map_template_data, TB_BLUE_KEY);
        }
    }
}

inline void upd_sandbox(GameData& game_data, Data& data) {
    u32 play = 0;
    switch(data.map_template_data.activated_switch_id) {
        case data::E_factory_00::SWITCH_1:
            play = 1;
            break;
        case data::E_factory_00::SWITCH_2:
            play = 2;
            break;
        case data::E_factory_00::SWITCH_3:
            play = 3;
            break;
        case data::E_factory_00::SWITCH_4:
            play = 4;
            break;
    }
    auto& blue_key_sp = data.entities.spatials[data::E_factory_00::BLUE_KEY];
    if(blue_key_sp.visible && blue_key_sp.py < from_int(424)) {
        // blue key free fall logic, when blue key is visible
        blue_key_sp.py += fix32(35);
    }
    if(play > 0) {
        // play switch sound
        sound::set_dmg_volume(3,3,
                map_template::MAP_SND_CHANNELS | music::factory_password_ch_mask);
        sound::play_music(music::factory_password_plen, music::factory_notes[play-1], 
                len(music::factory_notes[0]), false);
        // update the current password
        data.cur_pass = (data.cur_pass << u32(4)) | (play - u32(1));
        if(data.cur_pass == password) {
            // password accepted
            auto& switch_2 = data.entities.spatials[data::E_factory_00::SWITCH_2];
            if(!(game_data.session_data.key_flags & BLUE_KEY_FLAG)) {
                blue_key_sp.visible = true;
                blue_key_sp.px = switch_2.px;
                blue_key_sp.py = switch_2.py + from_int(8);
                particles::add_sand(
                        data.map_template_data.particles, 
                        switch_2.px, switch_2.py + from_int(16+4));
            }
        }
    }
}

inline u32 upd_dialogues(u32 last_status, GameData& game_data, Data& data) {
    // final dialogue
    if(last_status == map_template::Status::QUEST_DONE) {
        if(data.final_dialogue_done) {
            if(data.map_template_data.state != map_template::State::DIALOGUE)
                return map_template::Status::NEXT_MAP;
        } else {
            data.final_dialogue_done = true;
            map_template::start_dialogue(
                    game_data, data.map_template_data, TB_F00_END);
        }
    }
    // respawn dialogue
    if(data.show_respawn_dialogue) {
        data.show_respawn_dialogue = false;
        game_data.tmp_flags |= TmpFlags::TUT_RESPAWN;
        map_template::start_dialogue(
                game_data, data.map_template_data, TB_RESPAWN);
    } else {
      switch(data.map_template_data.activated_trigger) {
        case data::TR_factory_00::DIA_BEGINNING:
            map_template::start_dialogue(
                    game_data, data.map_template_data, TB_BEGINNING);
            break;
        case data::TR_factory_00::DIA_MINES:
            map_template::start_dialogue(
                    game_data, data.map_template_data, TB_MINES);
            break;
        case data::TR_factory_00::DIA_MAGNET:
            map_template::start_dialogue(
                    game_data, data.map_template_data, TB_MAGNET);
            break;
      }
    }
    return last_status;
}

export u32 update(u32 last_status, GameData& game_data, Data& data) {
    last_status = map_template::update(last_status, game_data, data.map_template_data);

    upd_green_key(game_data, data);
    upd_music_password_computer(game_data, data);
    upd_magnet_computer(game_data, data);
    upd_sandbox(game_data, data);
    return upd_dialogues(last_status, game_data, data);
}


// --------------------------------- Draw ---------------------------------- //

export u32 draw(u32 last_status, GameData& game_data, Data& data) {
    return map_template::draw(last_status, game_data, data.map_template_data);
}

} // namespace factory_00
} // namespace map
} // namespace game

