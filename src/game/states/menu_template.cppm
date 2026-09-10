module;
export module game.menu_template;

import engine;
import game.data;
import game.options;
import game.fonts;

import data.menu_arrow;
import data.mmenu_wave;
import data.mmenu_bg;

using namespace gba;
using namespace gba::sound;

namespace game {
namespace menu_template {

export constexpr u32 BG_SCREENBLOCK = 29;
export constexpr u32 WAVE_SCREENBLOCK = 30;
export constexpr i32 LINE_HEIGHT = 20;
export constexpr u32 COLOR_SELECTED = 0x027F;
export constexpr u32 COLOR_NEUTRAL = 0x4210;

export enum Status {
    INITIAL = 0xFF,
    WAIT,
    SELECTED,
};

const Track up_melody[] =
    { {Cmd::SQR_2_CNT, CNT(32,Duty::D1_2,1,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::C2,TIMED,Reset::ON)}
    , {Cmd::WAIT,1} };

const Track down_melody[] =
    { {Cmd::SQR_2_CNT, CNT(32,Duty::D1_4,1,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::C2,TIMED,Reset::ON)}
    , {Cmd::WAIT,1} };

const Track select_melody[] =
    { {Cmd::SQR_2_CNT, CNT(32,Duty::D3_4,1,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::D2,TIMED,Reset::ON)}
    , {Cmd::WAIT,1} 
    , {Cmd::SQR_2_CNT, CNT(32,Duty::D3_4,1,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::E2,TIMED,Reset::ON)}
    , {Cmd::WAIT,1} };

export template<int C> struct Data {
    i32 cursor_pos;
    bool cursor_changed;
    u32 frame;

    // animated background
    u32 bg_cur_time;
    u32 bg_cur_frame;

    const bool lang_menu;
    const u32 tgrid_pos_tx;
    const u32 tgrid_pos_ty;

    const c32* const (*menu_text)[LANG_COUNT][C];
    DrawText_State dt_states[C];
    bool dt_done[C];
    TileGrid<14,C*2+2> text_grid;

    // graphics
    Shared<Stack_ObjPalette> arrow_pal;
    Shared<Stack_ObjTile_4> arrow_tiles;
    StackSprite arrow_sp;

    Data(const c32* const (*menu_text_ptr)[LANG_COUNT][C],
         const u32 tgrid_pos_tx_, const u32 tgrid_pos_ty_,
         const bool lang_menu_ = false) :
        lang_menu(lang_menu_),
        tgrid_pos_tx(tgrid_pos_tx_),
        tgrid_pos_ty(tgrid_pos_ty_),
        menu_text(menu_text_ptr),
        arrow_pal({&data::pal_menu_arrow}, 1),
        arrow_tiles({data::menu_arrow}, data::menu_arrow_size),
        arrow_sp(arrow_pal, arrow_tiles, {{},1}, 
                 new_obj_attr_16x16(0,0,1,SpriteMode::Normal))
        { /* do nothing */ }
};


export template<int C> u32 init(GameData& /*game_data*/, Data<C>& data) {
    data.cursor_pos = 0;
    data.frame = 0;
    data.cursor_changed = true;

    data.bg_cur_time = 0;
    data.bg_cur_frame = 0;

    for(u32 i=0; i<C; ++i)
        data.dt_done[i] = false;

    // setting up the sound channel
    set_dmg_volume(3, 3, ChannelMask::LSQR2 | ChannelMask::RSQR2);

    // setting up the text tile grid
    set_tile_grid(data.text_grid, 0, data.tgrid_pos_tx, data.tgrid_pos_ty);
    clear_tile_grid(data.text_grid);
    
    // check if tile count is low enough
    static_assert(data::mmenu_bg_tbank_size < 30*20); 

    // copying all bg animation tiles
    for(u32 i=0; i<data::mmenu_bg_tbank_size; ++i)
        tile_memory_4[2][i] = data::mmenu_bg_tbank[i];
    // and the bg palette
    bg_palette[2] = data::pal_mmenu_bg;

    // copying the wave tiles
    for(u32 i=0; i<data::mmenu_wave_size; ++i)
        tile_memory_4[2][i + data::mmenu_bg_tbank_size] = data::mmenu_wave[i];
    // and the wave palette
    bg_palette[3] = data::pal_mmenu_wave;

    // setting up the display mode
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | (1<<6);
    
    // setting up the screenblocks for the text
    REG_BG0CNT = (10 << 8);
    for(u32 i=0; i<SCREENBLOCK_SIZE; ++i)
        screenblocks[10][i] = i;

    // setting up the screenblocks for the background
    REG_BG1CNT = 2 | (2 << 0x2) | (BG_SCREENBLOCK << 8);
    for(u32 y=0; y<data::mmenu_bg_sy; ++y)
      for(u32 x=0; x<data::mmenu_bg_sx; ++x) {
        const u32 i = x + y*32;
        const u32 j = x + y*data::mmenu_bg_sx;
        screenblocks[BG_SCREENBLOCK][i] = 
                data::mmenu_bg_idle_data[0][j] | (2 << 0xC);
      }

    // setting up the screenblocks for the wave
    REG_BG2CNT = 1 | (2 << 0x2) | (WAVE_SCREENBLOCK << 8);
    for(u32 y=0; y<32; ++y)
      for(u32 x=0; x<32; ++x) {
        const u32 i = x + y*32;
        u32 j = data::mmenu_bg_tbank_size;
        if(x>0 && x<3)
            j += ((x + 1) & 1) + (y & 3) * 2;
        screenblocks[WAVE_SCREENBLOCK][i] = j | (3 << 0xC);
      }

    vblank_intr_wait();
    enable_sprite(data.arrow_sp);

    return Status::INITIAL;
}


export template<int C> void free(GameData&, Data<C>& data) {
    //vblank_intr_wait();
    disable_sprite(data.arrow_sp);
}


export template<int C> u32 update(u32 last_status, GameData& game_data, Data<C>& data) {
    ++data.bg_cur_time;
    if(data.bg_cur_time > 6) {
        data.bg_cur_time = 0;
        data.bg_cur_frame++;
        if(data.bg_cur_frame >= data::mmenu_bg_frame_count[0])
            data.bg_cur_frame = 0;
        for(u32 y=0; y<data::mmenu_bg_sy; ++y)
          for(u32 x=0; x<data::mmenu_bg_sx; ++x) {
            const u32 i = x + y*32;
            const u32 j = x + y*data::mmenu_bg_sx;
            screenblocks[BG_SCREENBLOCK][i] = 
                    data::mmenu_bg_idle_data[data.bg_cur_frame][j] | (2 << 0xC);
          }
    }

    // read the input
    if(last_status == Status::WAIT) {
        if(is_key_down(Key::DOWN))
            game_data.menu_template_wave_pos+=50;
        if(is_key_down(Key::UP))
            game_data.menu_template_wave_pos-=50;
        if(is_key_hit(Key::DOWN)) {
            play_music(PLEN(100,2), down_melody, len(down_melody), false);
            data.cursor_changed = true;
            ++data.cursor_pos;
            if(data.cursor_pos >= C)
                data.cursor_pos = 0;
        }
        if(is_key_hit(Key::UP)) {
            play_music(PLEN(100,2), up_melody, len(up_melody), false);
            data.cursor_changed = true;
            --data.cursor_pos;
            if(data.cursor_pos < 0)
                data.cursor_pos = C - 1;
        }
        if(is_key_hit(KEY_SELECT)) {
            play_music(PLEN(300,2), select_melody, len(select_melody), false);
            return Status::SELECTED;
        }
    }
    ++data.frame;
    return last_status;
}


export template<int C> u32 draw(
        u32 last_status, GameData& game_data, Data<C>& data) {
    if(last_status == Status::INITIAL || data.cursor_changed) {
        // TODO: generalize
        // setting up the bg palette to highlight the text
        const u32 c1 = (data.cursor_pos == 0 ? COLOR_SELECTED : COLOR_NEUTRAL);
        const u32 c2 = (data.cursor_pos == 1 ? COLOR_SELECTED : COLOR_NEUTRAL);
        const u32 c3 = (data.cursor_pos == 2 ? COLOR_SELECTED : COLOR_NEUTRAL);
        const u32 c4 = (data.cursor_pos == 3 ? COLOR_SELECTED : COLOR_NEUTRAL);
        const u32 c5 = (data.cursor_pos == 4 ? COLOR_SELECTED : COLOR_NEUTRAL);
        bg_palette[0].data[0] = c1 << 16;
        bg_palette[0].data[1] = c2 | (c3 << 16);
        bg_palette[0].data[2] = c4 | (c5 << 16);
    }

    data.cursor_changed = false;

    // drawing the text
    for(i32 i=0; i<C; ++i) {
        if(data.dt_done[i])
            continue;
        const auto& font = (data.lang_menu 
                ? (i != Language::JAP ? fonts::cairopixel : fonts::nosutaru)
                : (game_data.language != Language::JAP ? fonts::cairopixel : fonts::nosutaru));
        data.dt_done[i] = draw_text(
                (last_status == Status::INITIAL), 
                data.dt_states[i], (*data.menu_text)[game_data.language][i],
                font, data.text_grid, 0, i*LINE_HEIGHT);
    }

    // moving the arrow sprite
    const i32 a_dx = (i32(data.frame >> 4) & 1) * 4;
    set_obj_attr_px(data.arrow_sp.obj_attr, 
            data.tgrid_pos_tx*8 - (data.lang_menu ? 60 : 28) - a_dx);
    set_obj_attr_py(data.arrow_sp.obj_attr, 
            data.tgrid_pos_ty*8 + data.cursor_pos*LINE_HEIGHT + 1);
    update_sprite_attrs(data.arrow_sp);

    // moving the wave bg
    game_data.menu_template_wave_pos++;
    game_data.menu_template_wave_pos &= (1 << (16+5)) - 1;
    REG_BG2VOFS = game_data.menu_template_wave_pos >> 5;

    return Status::WAIT;
}

} // namespace main_menu
} // namespace game

