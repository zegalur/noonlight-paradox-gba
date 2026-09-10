module;
export module game.dialogue_box;

import engine;
import game.data;
import game.fonts;
import game.options;

import data.dialogue_box_tiles;
import data.p_computer;
import data.d_arrow;
import data.p_akiba;
import data.p_kira;

using namespace gba;

#include "../options.hpp"
#include "../../engine/components/visual.hpp"

namespace game {
namespace dlg_box {

export constexpr u32 TILE_LEFT_TOP     = 0;
export constexpr u32 TILE_TOP          = 1;
export constexpr u32 TILE_RIGHT_TOP    = 2;
export constexpr u32 TILE_LEFT         = 3;
export constexpr u32 TILE_CENTER       = 4;
export constexpr u32 TILE_RIGHT        = 5;
export constexpr u32 TILE_LEFT_BOTTOM  = 6;
export constexpr u32 TILE_BOTTOM       = 7;
export constexpr u32 TILE_RIGHT_BOTTOM = 8;
export constexpr u32 TILE_TGRID        = 32;

export constexpr u32 DBOX_HEIGHT_T = 6;
export constexpr u32 TGRID_TWIDTH = 24;
export constexpr u32 TGRID_THEIGHT = 4;

export constexpr u32 P_TCOUNT = 16;
export constexpr u32 A_COUNT = 3;
export constexpr u32 E_COUNT = A_COUNT + 1;
export constexpr u32 E_ARROW = A_COUNT;

export constexpr fix32 P_ANIM_SPEED = from_int(1) >> 4;
export constexpr fix32 P_ANIM_SPEED_2 = from_int(1) >> 3;
export constexpr fix32 P_ARROW_SPEED = from_int(1) >> 5;

export enum Actor {
    MAINC = 0,
    PILOT,
    COMPUTER,
    NONE,
};

const c32* const ACTOR_NAMES[A_COUNT][LANG_COUNT] = {
    { U"^3" MAINC_NAME_ENG U"^1\n^|^|"
    , U"^3" MAINC_NAME_UKR U"^1\n^|^|"
    , U"^3" MAINC_NAME_JAP U"様^1\n^|^|"
    },
    { U"^3Captain " PILOT_NAME_ENG U"^1\n^|^|"
    , U"^3Капітан " PILOT_NAME_UKR U"^1\n^|^|"
    , U"^3" PILOT_NAME_JAP U"機長^1\n^|^|"
    },
    { U"^4Computer^1\n^|^|"
    , U"^4Комп'ютер^1\n^|^|"
    , U"^4コンピュータ^1\n^|^|"
    },
};

const i32 TEXT_DX[LANG_COUNT] = {  3,  3,  3 };
const i32 TEXT_DY[LANG_COUNT] = { -4, -4,  0 };

export struct TextBlock {
    Actor actor;
    const c32* const text;
    const fix32 speed;
};

export struct Data {
    TileGrid<TGRID_TWIDTH, TGRID_THEIGHT> text_grid;
    DrawText_State dt_state;
    const dlg_box::TextBlock* const* text_block_ptr;
    i32 text_block_indx;
    i32 prev_text_block_indx;
    bool waiting;
    fix32 text_timer;

    Shared<Pool_ObjPalette> p_kira_pal{{&data::pal_p_kira}, 1};
    Shared<Pool_ObjPalette> p_akiba_pal{{&data::pal_p_akiba}, 1};
    Shared<Pool_ObjPalette> p_computer_pal{{&data::pal_p_computer}, 1};
    Shared<Pool_ObjPalette> d_arrow_pal{{&data::pal_d_arrow}, 1};

    Shared<Pool_ObjTile_4_Placeholder> p_kira_tiles{{data::p_kira_tbank}, P_TCOUNT};
    Shared<Pool_ObjTile_4_Placeholder> p_akiba_tiles{{data::p_akiba_tbank}, P_TCOUNT};
    Shared<Pool_ObjTile_4_Placeholder> p_computer_tiles{{data::p_computer_tbank}, P_TCOUNT};
    Shared<Pool_ObjTile_4_Placeholder> d_arrow_tiles{{data::d_arrow_tbank}, 1};

    comp::Spatial spatials[E_COUNT] = {
        { from_int(8), from_int(8), 32, 32, false }, // main character (Kira)
        { from_int(8), from_int(8), 32, 32, false }, // spaceship pilot (Akiba)
        { from_int(8), from_int(8), 32, 32, false }, // computer
        { 0,0, 8,8, false }, // arrow sprite
    };

    comp::Visual visuals[E_COUNT] = {
        DVISUAL(p_kira_pal, p_kira_tiles, 
                32x32, 0, p_kira, IDLE, P_ANIM_SPEED, true, 0),
        DVISUAL(p_akiba_pal, p_akiba_tiles, 
                32x32, 0, p_akiba, IDLE, P_ANIM_SPEED, true, 0),
        DVISUAL(p_computer_pal, p_computer_tiles, 
                32x32, 0, p_computer, IDLE, P_ANIM_SPEED, true, 0),
        DVISUAL(d_arrow_pal, d_arrow_tiles, 
                8x8, 0, d_arrow, IDLE, P_ARROW_SPEED, true, 0),
    };
};

// -------------------------------- Init ----------------------------------- //

export void init(
        Data& data,
        const u32 palette_id,
        const u32 charblock,
        const u32 offset,
        const u32 /*screenblock*/
        ) {
    // copy palette
    bg_palette[palette_id] = data::pal_dialogue_box_tiles;

    // copy the box tiles
    tile_memory_4[charblock][offset+TILE_LEFT_TOP    ] = data::dialogue_box_tiles[3];
    tile_memory_4[charblock][offset+TILE_TOP         ] = data::dialogue_box_tiles[4];
    tile_memory_4[charblock][offset+TILE_RIGHT_TOP   ] = data::dialogue_box_tiles[5];
    tile_memory_4[charblock][offset+TILE_LEFT        ] = data::dialogue_box_tiles[6];
    tile_memory_4[charblock][offset+TILE_CENTER      ] = data::dialogue_box_tiles[7];
    tile_memory_4[charblock][offset+TILE_RIGHT       ] = data::dialogue_box_tiles[8];
    tile_memory_4[charblock][offset+TILE_LEFT_BOTTOM ] = data::dialogue_box_tiles[9];
    tile_memory_4[charblock][offset+TILE_BOTTOM      ] = data::dialogue_box_tiles[10];
    tile_memory_4[charblock][offset+TILE_RIGHT_BOTTOM] = data::dialogue_box_tiles[11];

    // setup the textgrid
    for(u32 i=0, y=0; y<TGRID_THEIGHT; ++y)
        for(u32 x=0; x<TGRID_TWIDTH; ++x, ++i) {
            const u32 tid = offset + TILE_TGRID + i;
            tile_memory_4[charblock][tid] = data::dialogue_box_tiles[7];
            data.text_grid.cells[y][x] = &tile_memory_4[charblock][tid];
        }
}

export void set_text_block(
        Data& data,
        const dlg_box::TextBlock* const* const text_block
        ) {
    data.text_block_ptr = text_block;
    data.prev_text_block_indx = -1;
    data.text_timer = FIX32_ZERO;
    data.text_block_indx = 0;
    data.waiting = false;
}

export void free(
        Data& data,
        const u32 palette_id,
        const u32 screenblock,
        const u32 empty_tile_id
        ) {
    // clean up the bg
    for(u32 tx = 0; tx < SCREEN_WIDTH_T; ++tx)
        for(u32 ty = 0; ty < DBOX_HEIGHT_T; ++ty)
            screenblocks[screenblock][tx + ty*32] = 
                    (empty_tile_id) | (palette_id << 0xC);
    sys::director::free_all(E_COUNT, data.visuals);
}


// ------------------------------- Update ---------------------------------- //

/// @return Returns `true` when dialogue finished.
export bool update(
        GameData& game_data,
        Data& data,
        const u32 charblock,
        const u32 offset
        ) {
    const TextBlock* const text_block_p = data.text_block_ptr[game_data.language];
    const TextBlock& text_block = text_block_p[data.text_block_indx];
    if(text_block.text == 0)
        return true;
    if(data.text_block_indx != data.prev_text_block_indx) {
        // text block index has been changed
        data.prev_text_block_indx = data.text_block_indx;
        data.text_timer = FIX32_ZERO;
        data.waiting = false;
        // clean up the text grid tiles
        for(u32 i=0, y=0; y<TGRID_THEIGHT; ++y)
            for(u32 x=0; x<TGRID_TWIDTH; ++x, ++i) {
                const u32 tid = offset + TILE_TGRID + i;
                tile_memory_4[charblock][tid] = data::dialogue_box_tiles[7];
            }
        // output the actor name
        for(u32 i=0; ; ++i) {
            if(draw_text(i==0, data.dt_state, 
                    ACTOR_NAMES[text_block.actor][game_data.language],
                    *game_data.std_font, data.text_grid, 
                    TEXT_DX[game_data.language], 
                    TEXT_DY[game_data.language])) {
                reset_string(data.dt_state);
                break;
            }
        }
        // change the actor portrait
        for(u32 i=0; i<A_COUNT; ++i) {
            if(i == text_block.actor) {
                data.spatials[i].visible = true;
            } else {
                data.spatials[i].visible = false;
            }
        }
    } 
    bool a_hit = is_key_hit(Key::A);
    if(data.waiting == false) {
        data.text_timer += text_block.speed;
        while(a_hit || data.text_timer >= FIX32_ONE) {
            data.text_timer -= FIX32_ONE;
            if(draw_text(false, data.dt_state, text_block.text,
                    *game_data.std_font, data.text_grid, 
                    TEXT_DX[game_data.language], 0)) {
                data.waiting = true;
                break;
            }
        }
        if(a_hit)
            a_hit = false;
    } 
    if(data.waiting) {
        data.spatials[E_ARROW].visible = true;
        data.spatials[E_ARROW].px = from_int(data.dt_state.cursor_x + 40 + 2);
        data.spatials[E_ARROW].py = from_int(data.dt_state.cursor_y + 8 + 5);
    } else {
        data.spatials[E_ARROW].visible = false;
    }
    sys::director::update_all(0,0, 0,0, E_COUNT, data.spatials, data.visuals);

    if(a_hit)
        ++data.text_block_indx;

    return false;
}

export void draw_first_time(
        Data& /*data*/,
        const u32 palette_id,
        const u32 /*charblock*/,
        const u32 offset,
        const u32 screenblock,
        const u32 empty_tile_id,
        const u32 bg_layer
        ) {
    // draw the dialogue box
    for(u32 tx = 0; tx < SCREEN_WIDTH_T; ++tx)
        for(u32 ty = 0; ty < DBOX_HEIGHT_T; ++ty) {
            u32 tile_id = TILE_CENTER;
            if(tx == 0) {
                if(ty == 0) 
                    tile_id = TILE_LEFT_TOP;
                else if(ty == DBOX_HEIGHT_T - 1) 
                    tile_id = TILE_LEFT_BOTTOM;
                else 
                    tile_id = TILE_LEFT;
            } else if(tx == SCREEN_WIDTH_T - 1) {
                if(ty == 0) 
                    tile_id = TILE_RIGHT_TOP;
                else if(ty == DBOX_HEIGHT_T - 1) 
                    tile_id = TILE_RIGHT_BOTTOM;
                else 
                    tile_id = TILE_RIGHT;
            } else if(ty == 0) {
                tile_id = TILE_TOP;
            } else if(ty == DBOX_HEIGHT_T - 1) {
                tile_id = TILE_BOTTOM;
            } else if(tx >= 5) {
                tile_id = TILE_TGRID + (tx - 5) + (ty - 1)*TGRID_TWIDTH;
            }
            screenblocks[screenblock][tx + ty*32] = 
                    (tile_id + offset) | (palette_id << 0xC);
        }
    
    // clean up the rest of the bg layer
    for(u32 tx = 0; tx < SCREEN_WIDTH_T; ++tx)
        for(u32 ty = DBOX_HEIGHT_T; ty < SCREEN_HEIGHT_T; ++ty)
            screenblocks[screenblock][tx + ty*32] = 
                    (empty_tile_id) | (palette_id << 0xC);

    // setting the BG offset to zero
    switch(bg_layer) {
        case 0:
            REG_BG0HOFS = 0;
            REG_BG0VOFS = 0;
            break;
        case 1:
            REG_BG1HOFS = 0;
            REG_BG1VOFS = 0;
            break;
        case 2:
            REG_BG2HOFS = 0;
            REG_BG2VOFS = 0;
            break;
        case 3:
            REG_BG3HOFS = 0;
            REG_BG3VOFS = 0;
            break;
    }
}


// -------------------------------- Draw ----------------------------------- //

export void draw(
        GameData& /*game_data*/,
        Data& data) {
    sys::director::redraw_all(E_COUNT, data.visuals);
}

} // namespace dlg_box
} // namespace game
