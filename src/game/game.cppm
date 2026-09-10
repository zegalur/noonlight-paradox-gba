module;
export module game;

import engine;
import game.data;
import game.fonts;
import game.options;
import game.language_selection;
import game.main_menu;
import game.intro;
import game.map_template;
import game.map.factory_00;
import game.map.factory_01;

using namespace gba;

namespace game {

// ------------------------------ Game States ------------------------------ //

// WARNING: for a new state, add the static_assert!

static_assert(MEMORY_BANK_SIZE > sizeof(language_selection::Data));
static_assert(MEMORY_BANK_SIZE > sizeof(main_menu::Data));
static_assert(MEMORY_BANK_SIZE > sizeof(intro::Data));
static_assert(MEMORY_BANK_SIZE > sizeof(map::factory_00::Data));
static_assert(MEMORY_BANK_SIZE > sizeof(map::factory_01::Data));

union StateData {
    language_selection::Data* lang_sel;
    main_menu::Data* mmenu;
    intro::Data* intro;
    map::factory_00::Data* factory_00;
    map::factory_01::Data* factory_01;
};

export enum GameState {
    TRANSIT_STATE,
    LANGUAGE_SELECTION,
    MAIN_MENU,
    INTRO,
    MAP_FACTORY_00,
    MAP_FACTORY_01,
};


// --------------------------------- Data ---------------------------------- //

export constexpr u32 FADE_TIME = 32;

export struct Data {
    mem::Bank<MEMORY_BANK_SIZE> membank;
    GameState prev_state, cur_state, next_state;
    GameData game_data;
    intro::State intro_state;
    bool is_respawn;
    StateData s;
};


// ------------------------------ Transitions ------------------------------ //

u32 fade_to(Data& data, const GameState next_state) {
    data.prev_state = data.cur_state;
    data.cur_state = GameState::TRANSIT_STATE;
    data.next_state = next_state;
    return 0;
}

u32 new_game(Data& data, const bool skip_intro) {
    if(!skip_intro)
        return fade_to(data, GameState::INTRO);
    return fade_to(data, GameState::MAP_FACTORY_00);
}

u32 next_level(Data& data) {
    // zero-out the session data
    data.is_respawn = false;
    data.game_data.session_data = SessionData();
    switch(data.cur_state) {
        case GameState::MAP_FACTORY_00:
            return fade_to(data, GameState::MAP_FACTORY_01);
        default:
            break;
    }
    return fade_to(data, GameState::MAIN_MENU);
}


// --------------------------------- Init ---------------------------------- //

export u32 init_game(Data& data) {
    data.cur_state = GameState::LANGUAGE_SELECTION;
    data.game_data.menu_template_wave_pos = 8 << 5;
    data.intro_state = intro::State::INTRO_01;
    data.is_respawn = false;
    mem::init(data.membank);
    mem::alloc(data.membank, data.s.lang_sel);
    return language_selection::init(data.game_data, *data.s.lang_sel);
}


// -------------------------------- Update --------------------------------- //

u32 update_transit(u32 last_status, Data& data) {
    ++last_status;
    if(last_status >= FADE_TIME) {
        REG_DISPCNT = 0;
        vblank_intr_wait();

        // free the resources
        switch(data.prev_state) {
            case GameState::TRANSIT_STATE:
                // should never happen
                break;
            case GameState::LANGUAGE_SELECTION:
                language_selection::free(data.game_data, *data.s.lang_sel);
                mem::free(data.membank, data.s.lang_sel);
                break;
            case GameState::MAIN_MENU:
                main_menu::free(data.game_data, *data.s.mmenu);
                mem::free(data.membank, data.s.mmenu);
                break;
            case GameState::INTRO:
                intro::free(data.game_data, *data.s.intro);
                mem::free(data.membank, data.s.intro);
                break;
            case GameState::MAP_FACTORY_00:
                map::factory_00::free(data.game_data, *data.s.factory_00);
                mem::free(data.membank, data.s.factory_00);
                break;
            case GameState::MAP_FACTORY_01:
                map::factory_01::free(data.game_data, *data.s.factory_01);
                mem::free(data.membank, data.s.factory_01);
                break;
        }
    
        // clear up all bg tiles
        for(u32 ch=0; ch<=3; ++ch)
            for(u32 i=0; i<TILE_4_COUNT; ++i)
                tile_memory_4[ch][i] = EMPTY_TILE;

        // clean up the video-memory
        vblank_intr_wait();
        disable_all_sprites();

        // initiate the next state
        data.cur_state = data.next_state;
        const auto from_checkpoint =
                ( data.is_respawn
                ? data.game_data.session_data.checkpoint_id
                : map_template::NO_CHECKPOINT );
        switch(data.next_state) {
            case GameState::TRANSIT_STATE:
                break; // should never happen
            case GameState::LANGUAGE_SELECTION:
                mem::alloc(data.membank, data.s.lang_sel);
                return language_selection::init(data.game_data, *data.s.lang_sel);
            case GameState::MAIN_MENU:
                mem::alloc(data.membank, data.s.mmenu);
                return main_menu::init(data.game_data, *data.s.mmenu);
            case GameState::INTRO:
                mem::alloc(data.membank, data.s.intro);
                return intro::init(data.game_data, *data.s.intro, data.intro_state);
            case GameState::MAP_FACTORY_00:
                mem::alloc(data.membank, data.s.factory_00);
                return map::factory_00::init(
                        data.game_data, *data.s.factory_00, 
                        from_checkpoint, data.is_respawn);
            case GameState::MAP_FACTORY_01:
                mem::alloc(data.membank, data.s.factory_01);
                return map::factory_01::init(
                        data.game_data, *data.s.factory_01,
                        from_checkpoint, data.is_respawn);
        }
        data.is_respawn = false;
    }
    return last_status;
}

u32 update_language_selection(u32 last_status, Data& data) {
    u32 selected_language = language_selection::update(
            last_status, data.game_data, *data.s.lang_sel);
    if(selected_language >= LANG_COUNT)
        return selected_language;
    data.game_data.language = Language(selected_language);
    data.game_data.std_font = 
            ( selected_language == Language::JAP 
            ? &fonts::nosutaru 
            : &fonts::cairopixel );
    return fade_to(data, GameState::MAIN_MENU);
}

u32 update_main_menu(u32 last_status, Data& data) {
    u32 s = main_menu::update(last_status, data.game_data, *data.s.mmenu);
    switch(s) {
        case main_menu::Status::BACK:
            return fade_to(data, GameState::LANGUAGE_SELECTION);
        case main_menu::Status::NEW_GAME:
            return new_game(data, false);
        case main_menu::Status::SKIP_INTRO:
            return new_game(data, true);
    }
    return s;
}

u32 update_intro(u32 last_status, Data& data) {
    u32 s = intro::update(last_status, data.game_data, *data.s.intro);
    if(s == intro::State::END)
        return new_game(data, true);
    if(s != intro::State::ZERO) {
        data.intro_state = intro::State(s);
        return fade_to(data, GameState::INTRO);
    } 
    return s;
}

u32 update_map(u32 last_status, Data& data) {
    u32 cmd = 0;
    switch(data.cur_state) {
        case GameState::MAP_FACTORY_00:
            cmd = map::factory_00::update(
                    last_status, data.game_data, *data.s.factory_00);
            break;
        case GameState::MAP_FACTORY_01:
            cmd = map::factory_01::update(
                    last_status, data.game_data, *data.s.factory_01);
            break;
        case GameState::TRANSIT_STATE:
        case GameState::LANGUAGE_SELECTION:
        case GameState::MAIN_MENU:
        case GameState::INTRO:
            break;
    }
    switch(cmd) {
        case map_template::Status::RESPAWN_REQUEST:
            data.is_respawn = true;
            return fade_to(data, data.cur_state);
        case map_template::Status::LOAD_REQUEST:
            data.game_data.session_data = SessionData();
            return fade_to(data, data.cur_state);
        case map_template::Status::NEXT_MAP:
            return next_level(data);
    }
    return cmd;
}

export u32 update_game(u32 last_status, Data& data) {
    switch(data.cur_state) {
        case GameState::TRANSIT_STATE:
            last_status = update_transit(last_status, data);
            // call the update of a new state before draw
            if(data.cur_state != GameState::TRANSIT_STATE)
                last_status = update_game(last_status, data);
            return last_status;
        case GameState::LANGUAGE_SELECTION:
            return update_language_selection(last_status, data);
        case GameState::MAIN_MENU:
            return update_main_menu(last_status, data);
        case GameState::INTRO:
            return update_intro(last_status, data);
        case GameState::MAP_FACTORY_00:
        case GameState::MAP_FACTORY_01:
            return update_map(last_status, data);
    }
    return ERROR_STATUS;
}


// --------------------------------- Draw ---------------------------------- //

inline u32 fade_color(u32 c) {
    u32 r = (c >> u32(0)) & u32(31);
    u32 g = (c >> u32(5)) & u32(31);
    u32 b = (c >> u32(10)) & u32(31);
    if(r > 0) --r;
    if(g > 0) --g;
    if(b > 0) --b;
    return r | (g << u32(5)) | (b << u32(10));
}

inline u32 fade_pal_color(u32 x) {
    u32 x1 = x & ((u32(1) << u32(16)) - u32(1));
    u32 x2 = x >> u32(16);
    return fade_color(x1) | (fade_color(x2) << u32(16));
}

u32 draw_transit(u32 last_status, Data&) {
    if(last_status < FADE_TIME) {
        for(i32 i=0; i<PALETTE_COUNT; ++i) {
            for(i32 j=0; j<8; ++j) {
                const u32 x = bg_palette[i].data[j];
                const u32 y = obj_palette[i].data[j];
                bg_palette[i].data[j] = fade_pal_color(x);
                obj_palette[i].data[j] = fade_pal_color(y);
            }
        }
    }
    return last_status;
}

export u32 draw_game(u32 last_status, Data& data) {
    switch(data.cur_state) {
        case GameState::TRANSIT_STATE:
            return draw_transit(last_status, data);
        case GameState::LANGUAGE_SELECTION:
            return language_selection::draw(
                    last_status, data.game_data, *data.s.lang_sel);
        case GameState::MAIN_MENU:
            return main_menu::draw(
                    last_status, data.game_data, *data.s.mmenu);
        case GameState::INTRO:
            return intro::draw(
                    last_status, data.game_data, *data.s.intro);
        case GameState::MAP_FACTORY_00:
            return map::factory_00::draw(
                    last_status, data.game_data, *data.s.factory_00);
        case GameState::MAP_FACTORY_01:
            return map::factory_01::draw(
                    last_status, data.game_data, *data.s.factory_01);
    }
    return ERROR_STATUS;
}

} // namespace gba
