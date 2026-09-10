module;
export module game.intro;

import engine;
import game.data;
import game.text;
import game.fonts;
import game.music.intro;

import data.intro_02_fg;
import data.intro_02_bg;
import data.intro_03_spaceship;
import data.explosion_01;
import data.a_button;
import data.bubble;

#include "../options.hpp"
#include "../../engine/components/visual.hpp"

using namespace gba;
using namespace gba::sound;

namespace game {
namespace intro {

export constexpr u32 WBG_SCREENBLOCK = 29;
export constexpr u32 WBG_PAL_INDX = 5;

export constexpr u32 E_COUNT        = 3;
export constexpr u32 E_A_BUTTON     = 0;
export constexpr u32 E_BUBBLE       = 1;
export constexpr u32 E_EXPLOSION    = 2;

export constexpr u32 TEXT_SCREENBLOCK = 30;
export constexpr u32 TEXT_TWIDTH = 19;
export constexpr u32 TEXT_THEIGHT = 12;

constexpr Palette INTRO_TEXT_PALETTE = 
    // WARNING: make sure #5 is black
    { 0xFFFF0000, 0x27A97529, 0x0000185E, 0x00000000
    , 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

export enum State {
    ZERO = 0,
    INTRO_01,
    INTRO_02,
    INTRO_03,
    INTRO_04,
    END
};

export struct Data {
    u32 fade_timer;
    bool transition;
    State state;

    TileMap tm_1;
    TileMap tm_2;
    bool use_tm_1;
    bool use_tm_2;
 
    TileGrid<TEXT_TWIDTH,TEXT_THEIGHT> text_grid;
    const TextBlock* cur_block;
    u32 cur_text_id;
    DrawText_State dt_state;
    bool reset_text;
    fix32 text_timer; 
    fix32 text_speed;
    bool text_waiting;
    i32 text_hoffset;
    i32 text_voffset;

    fix32 coef_1;
    
    // assets

    Shared<Pool_ObjPalette> a_button_pal{{&data::pal_a_button}, 1};
    Shared<Pool_ObjPalette> bubble_pal{{&data::pal_bubble}, 1};
    Shared<Pool_ObjPalette> explosion_pal{{&data::pal_explosion_01}, 1};

    Shared<Pool_ObjTile_4_Placeholder> a_button_tiles{
            {data::a_button_tbank}, data::a_button_tcount};
    Shared<Pool_ObjTile_4_Placeholder> bubble_tiles{
            {data::bubble_tbank}, data::bubble_tcount};
    Shared<Pool_ObjTile_4_Placeholder> explosion_tiles{
            {data::explosion_01_tbank}, data::explosion_01_tcount};

    comp::Spatial spatials[E_COUNT] = {
        { from_int(222), from_int(143), 
          8*data::a_button_sx, 8*data::a_button_sy, false },
        { from_int(122), from_int(60), 
          8*data::bubble_sx, 8*data::bubble_sy, false },
        { from_int(92), from_int(104), 
          8*data::explosion_01_sx, 8*data::explosion_01_sy, false }
    };

    comp::Visual visuals[E_COUNT] = {
        DVISUAL(a_button_pal, a_button_tiles, 16x16, 1, 
                a_button, IDLE, from_int(1) >> 5, true, 0),
        DVISUAL(bubble_pal, bubble_tiles, 32x32, 1, 
                bubble, IDLE, from_int(1) >> 5, true, 0),
        DVISUAL(explosion_pal, explosion_tiles, 32x32, 1, 
                explosion_01, IDLE, from_int(1) >> 5, true, 0),
    };
};

void set_text_block(Data& data, const TextBlock* text_block) {
    data.text_timer = from_int(0);
    data.text_speed = from_int(1);
    data.cur_text_id = 0;
    data.cur_block = text_block;
    data.text_speed = text_block->speed;
    data.reset_text = true;
    data.text_waiting = false;
}

void init_intro(GameData& game_data, Data& data, State state) {
    // setting up the text block
    switch(state) {
        case State::ZERO:
        case State::END:
            return; // skip
        case State::INTRO_01:
            set_text_block(data, &(PART_1_TEXTBLOCKS[game_data.language][0]));
            break;
        case State::INTRO_02:
            set_text_block(data, &(PART_2_TEXTBLOCKS[game_data.language][0]));
            break;
        case State::INTRO_03:
            set_text_block(data, &(PART_3_TEXTBLOCKS[game_data.language][0]));
            break;
        case State::INTRO_04:
            set_text_block(data, &(PART_4_TEXTBLOCKS[game_data.language][0]));
            break;
    }
    // setup the new state
    switch(state) {
        case State::ZERO:
        case State::END:
            return; // skip
        case State::INTRO_01:
            set_dmg_volume(3, 3, music::intro_ch_mask);
            play_music(music::intro_plen, music::intro_melody, 
                       len(music::intro_melody), true);
            [[fallthrough]];
        case State::INTRO_04:
            data.use_tm_2 = true;
            data.text_hoffset = -50;
            data.text_voffset = -17;
            init_tilemap(data.tm_2, 1,2, WBG_PAL_INDX,
                         data::intro_02_bg_pcount,
                         data::intro_02_bg_palettes,
                         2,0,data::intro_02_bg_tcount, 
                         data::intro_02_bg_tiles, 
                         WBG_SCREENBLOCK, 
                         data::intro_02_bg_tsize_x,
                         data::intro_02_bg_tsize_y,
                         data::intro_02_bg_sb_array, 
                         0,0, true);
            REG_DISPCNT = DCNT_MODE0 //| DCNT_BG0 
                    | DCNT_BG1 | DCNT_BG3 | DCNT_OBJ | (1<<6);
            break;

        case State::INTRO_02:
            data.use_tm_1 = true;
            data.use_tm_2 = true;
            data.text_hoffset = -25;
            data.text_voffset = -20;
            if(game_data.language == Language::JAP)
                data.text_voffset -= 8;
            data.spatials[E_BUBBLE].visible = true;
            init_tilemap(data.tm_1, 0,1,0, 
                         data::intro_02_fg_pcount,
                         data::intro_02_fg_palettes,
                         0,0,0, data::intro_02_fg_tiles, 11, 
                         data::intro_02_fg_tsize_x, 
                         data::intro_02_fg_tsize_y,
                         data::intro_02_fg_sb_array, 0,0);
            init_tilemap(data.tm_2, 1,2, WBG_PAL_INDX,
                         data::intro_02_bg_pcount,
                         data::intro_02_bg_palettes,
                         2,0,data::intro_02_bg_tcount, 
                         data::intro_02_bg_tiles, 
                         WBG_SCREENBLOCK, 
                         data::intro_02_bg_tsize_x,
                         data::intro_02_bg_tsize_y,
                         data::intro_02_bg_sb_array, 
                         0,0, true);
            REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 
                    | DCNT_BG1 | DCNT_BG3 | DCNT_OBJ | (1<<6);
            break;

        case State::INTRO_03:
            data.use_tm_1 = true;
            data.use_tm_2 = true;
            data.text_hoffset = -20;
            data.text_voffset = -30;
            init_tilemap(data.tm_1, 0,1,0, 
                         data::intro_03_spaceship_pcount,
                         data::intro_03_spaceship_palettes,
                         0,0,0, data::intro_03_spaceship_tiles, 11, 
                         data::intro_03_spaceship_tsize_x, 
                         data::intro_03_spaceship_tsize_y,
                         data::intro_03_spaceship_sb_array, 0,0);
            init_tilemap(data.tm_2, 1,2, WBG_PAL_INDX,
                         data::intro_02_bg_pcount,
                         data::intro_02_bg_palettes,
                         2,0,data::intro_02_bg_tcount, 
                         data::intro_02_bg_tiles, 
                         WBG_SCREENBLOCK, 
                         data::intro_02_bg_tsize_x,
                         data::intro_02_bg_tsize_y,
                         data::intro_02_bg_sb_array, 
                         0,0, true);
            REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 
                    | DCNT_BG1 | DCNT_BG3 | DCNT_OBJ | (1<<6);
            break;
    }
}

export u32 init(GameData& game_data, Data& data, State new_state) {
    data.state = State::ZERO;

    // setting up the text grid, making sure it's not overlaps with the 
    // other tile-map backgrounds
    bg_palette[PALETTE_COUNT - 1] = INTRO_TEXT_PALETTE;
    REG_BG3CNT = (TEXT_SCREENBLOCK << 8) | (2 << 2);
    tile_memory_4[2][256] = EMPTY_TILE;
    for(u32 i=0; i<SCREENBLOCK_SIZE; ++i)
        screenblocks[TEXT_SCREENBLOCK][i] = 256;
    for(u32 x=0; x<TEXT_TWIDTH; ++x)
      for(u32 y=0; y<TEXT_THEIGHT; ++y) {
            const u32 tid = (1 + 256 + x + y*TEXT_TWIDTH);
            // manually setting up the tile grid cells
            data.text_grid.cells[y][x] = &tile_memory_4[2][tid];
            screenblocks[TEXT_SCREENBLOCK][x + y*32] = 
                    tid | ((PALETTE_COUNT - 1) << 0xC);
        }
    clear_tile_grid(data.text_grid);

    data.state = new_state;
    data.use_tm_1 = false;
    data.use_tm_2 = false;
    data.text_hoffset = data.text_voffset = 0;
    data.coef_1 = 0;

    // hide all the entities
    for(u32 i=0; i<E_COUNT; ++i)
        data.spatials[i].visible = false;
    
    init_intro(game_data, data, new_state);
    return new_state;
}


export void free(GameData&, Data& data) {
    sys::director::free_all(E_COUNT, data.visuals);
}

u32 next_part(GameData& /*game_data*/, Data& data) {
    switch(data.state) {
        case State::ZERO:
            return State::ZERO;
        case State::END:
            return State::END;
        case State::INTRO_01:
            return State::INTRO_02;
        case State::INTRO_02:
            return State::INTRO_03;
        case State::INTRO_03:
            stop_music();
            return State::INTRO_04;
        case State::INTRO_04:
            stop_music();
            return State::END;
    }
    return State::ZERO;
}

export u32 update(u32 /*last_status*/, GameData& game_data, Data& data) {
    // set text offsets
    REG_BG3HOFS = data.text_hoffset;
    REG_BG3VOFS = data.text_voffset;

    // update the text block
    if(data.text_waiting) {
        data.spatials[E_A_BUTTON].visible = true;
        if(is_key_hit(Key::A)) {
            data.text_waiting = false;
            if(data.cur_block) 
                if(data.cur_block[data.cur_text_id].text == 0)
                    return next_part(game_data, data);
        }   
    } else {
        data.spatials[E_A_BUTTON].visible = false;
        if(data.reset_text)
            clear_tile_grid(data.text_grid);
        // scroll text faster when hit A
        if(is_key_hit(Key::A))
            data.text_speed = from_int(5);
        data.text_timer += data.text_speed;
        while(data.text_timer >= 1) {
            data.text_timer -= FIX32_ONE;
            if(data.cur_block && data.cur_block[data.cur_text_id].text) {
                if(draw_text(
                        data.reset_text, data.dt_state, 
                        data.cur_block[data.cur_text_id].text,
                        *game_data.std_font, data.text_grid, 0,0)) {
                    data.cur_text_id += 1;
                    data.reset_text = data.cur_block[data.cur_text_id].clear;
                    data.text_speed = data.cur_block[data.cur_text_id].speed;
                    data.text_waiting = true;
                    reset_string(data.dt_state);
                    break;
                } else {
                    data.reset_text = false;
                }
            }
        }
    }

    if(!data.text_waiting && !data.spatials[E_EXPLOSION].visible) {
        // show the explosion if needed
        if(data.cur_block && data.state == State::INTRO_03)
            if(data.cur_block[data.cur_text_id].text)
                if(data.cur_block[data.cur_text_id + 1].text == 0) {
                    data.spatials[E_EXPLOSION].visible = true;
                    set_dmg_volume(3,3, music::intro_expl_ch_mask);
                    play_music(music::intro_expl_plen,
                               music::intro_expl_sound,
                               len(music::intro_expl_sound),
                               true);
                }
    }

    sys::director::update_all(0,0, 0,0, E_COUNT, data.spatials, data.visuals);
    switch(data.state) {
        case State::ZERO:
        case State::END:
            break;
        case State::INTRO_01:
        case State::INTRO_04:
            // scroll the stars
            data.coef_1 -= from_int(1) >> 1;
            change_tilemap_bg_offset(data.tm_2, 0, intp(data.coef_1));
            break;
        case State::INTRO_02:
            // scroll the stars
            data.coef_1 += from_int(1) >> 2;
            change_tilemap_bg_offset(data.tm_2, intp(data.coef_1), 0);
            break;
        case State::INTRO_03:
            // scroll the stars
            data.coef_1 -= from_int(1) >> 1;
            change_tilemap_bg_offset(data.tm_2, 
                    intp(data.coef_1), -intp(data.coef_1));
            break;
    }
    return State::ZERO;
}


export u32 draw(u32 last_status, GameData& /*game_data*/, Data& data) {
    if(data.use_tm_1) set_tilemap_bg_offset(data.tm_1);
    if(data.use_tm_2) set_tilemap_bg_offset(data.tm_2);
    sys::director::redraw_all(E_COUNT, data.visuals);
    return last_status;
}

}
} // namespace game
