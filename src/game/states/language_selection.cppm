module;
export module game.language_selection;

import engine;
import game.data;
import game.options;
import game.menu_template;

import data.flag_uk;
import data.flag_ua;
import data.flag_ja;
import data.flag_uk_gray;
import data.flag_ua_gray;
import data.flag_ja_gray;

using namespace gba;


namespace game {
namespace language_selection {

namespace {
    const c32* const LANGUAGES[LANG_COUNT][LANG_COUNT] = {
      { U"^1English",
        U"^2Українська",
        U"^3日本語" },
      { U"^1English",
        U"^2Українська",
        U"^3日本語" },
      { U"^1English",
        U"^2Українська",
        U"^3日本語" } };
    constexpr i32 TGRID_POS_TX = 14;
    constexpr i32 TGRID_POS_TY = 5;
    const u32 FLAG_TILE_COUNT = 4; // 16x16px
}

enum Status {
    LANG_ENG = Language::ENG,
    LANG_UKR = Language::UKR,
    LANG_JAP = Language::JAP,
    INITIAL = menu_template::Status::INITIAL,
    WAIT = menu_template::Status::WAIT 
};

export struct Data {
    menu_template::Data<LANG_COUNT> menu_template_data;
    Data() : menu_template_data(&LANGUAGES, TGRID_POS_TX, TGRID_POS_TY, true)
    { /* do nothing */ }
};

export u32 init(GameData& game_data, Data& data) {
    return menu_template::init(game_data, data.menu_template_data);
}


export void free(GameData& game_data, Data& data) {
    menu_template::free(game_data, data.menu_template_data);
}

export u32 update(u32 last_status, GameData& game_data, Data& data) {
    u32 s = menu_template::update(last_status, game_data, data.menu_template_data);
    if(s == menu_template::Status::SELECTED)
        return data.menu_template_data.cursor_pos;
    return s;
}


export u32 draw(u32 last_status, GameData& game_data, Data& data) {
    // getting all the sprites ready
    if(last_status == Status::INITIAL) {
        // loading the flag tiles
        for(u32 i=0; i<FLAG_TILE_COUNT; ++i) {
            tile_memory_4[4][i + 0*FLAG_TILE_COUNT] = data::flag_uk[i];
            tile_memory_4[4][i + 1*FLAG_TILE_COUNT] = data::flag_ua[i];
            tile_memory_4[4][i + 2*FLAG_TILE_COUNT] = data::flag_ja[i];
        }
        // setting up the flag sprites
        obj_palette[0] = data::pal_flag_uk;
        obj_palette[1] = data::pal_flag_ua;
        obj_palette[2] = data::pal_flag_ja;
        obj_palette[3] = data::pal_flag_uk_gray;
        obj_palette[4] = data::pal_flag_ua_gray;
        obj_palette[5] = data::pal_flag_ja_gray;
        for(u32 i=0; i<LANG_COUNT; ++i) {
            obj_attrs[i].attr_0 = (
                    TGRID_POS_TY*8 + i*menu_template::LINE_HEIGHT + 1); // py
            obj_attrs[i].attr_1 = (
                    TGRID_POS_TX*8 - 30) | (1<<0xE); // px
        }
    } 

    if(data.menu_template_data.cursor_changed) {
        // changing the flag palettes
        for(i32 i=0; i<LANG_COUNT; ++i) {
            const u32 palette = (
                    data.menu_template_data.cursor_pos == i ? i : 3 + i);
            obj_attrs[i].attr_2 = (i*FLAG_TILE_COUNT) | (palette << 0xC);
        }
    }

    return menu_template::draw(last_status, game_data, data.menu_template_data);
}


} // namespace language_selection
} // namespace game
