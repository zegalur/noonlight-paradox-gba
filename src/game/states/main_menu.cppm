module;
export module game.main_menu;

import engine;
import game.data;
import game.text;
import game.menu_template;

using namespace gba;

namespace game {
namespace main_menu {

namespace {
    constexpr i32 TGRID_POS_TX = 10;
    constexpr i32 TGRID_POS_TY = 4;
}

export enum Status {
    NEW_GAME = 0,
    SKIP_INTRO = 1,
    CONTINUE = 2,
    BACK = 3,
    INITIAL = menu_template::Status::INITIAL,
    WAIT = menu_template::Status::WAIT,
};

export struct Data {
    menu_template::Data<OP_COUNT> menu_template_data;
    Data() : menu_template_data(&MENU_TEXT, TGRID_POS_TX, TGRID_POS_TY) 
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
    return menu_template::draw(last_status, game_data, data.menu_template_data);
}

} // namespace main_menu
} // namespace game

