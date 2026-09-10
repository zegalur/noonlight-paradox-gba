module;
export module game.player_gui;

import engine;
import game.data;
import game.options;
import game.player_stats;

import data.player_gui;
import data.key;

using namespace gba;

namespace game {
namespace player_gui {

enum UpdFlags {
    UPD_HBAR    = 0b1,
    UPD_GBAR    = 0b10,
    UPD_OTHER   = 0b100,
    /*UPD_GREEN_K = 0b1000,
    UPD_BLUE_K  = 0b10000,
    UPD_GOLD_K  = 0b100000,*/
    UPD_ALL     = 0b111111,
};

export struct Data {
    u32 cur_max_hbar_count;
    u32 cur_max_gbar_count;
    u32 cur_hbar_count;
    u32 cur_gbar_count;
    u32 upd_flags;
    bool hidden;

    Shared<Pool_ObjPalette> gui_pal{{&data::pal_player_gui}, 1};

    Shared<Pool_ObjTile_4> t_corner     {{&data::player_gui[0]}, 1};
    Shared<Pool_ObjTile_4> t_hbar_empty {{&data::player_gui[1]}, 1};
    Shared<Pool_ObjTile_4> t_hbar_full  {{&data::player_gui[2]}, 1};
    Shared<Pool_ObjTile_4> t_hbar_icon  {{&data::player_gui[3]}, 1};
    Shared<Pool_ObjTile_4> t_gbar_empty {{&data::player_gui[5]}, 1};
    Shared<Pool_ObjTile_4> t_gbar_full  {{&data::player_gui[6]}, 1};
    Shared<Pool_ObjTile_4> t_gbar_icon  {{&data::player_gui[7]}, 1};
    Shared<Pool_ObjTile_4> t_pistol     {{&data::player_gui[8]}, 8};

    Shared<Pool_ObjTile_4> t_green_key
            {{&data::key_tbank[data::key_green_data[0][0]]}, 8};
    Shared<Pool_ObjTile_4> t_blue_key
            {{&data::key_tbank[data::key_blue_data[0][0]]}, 8};
    Shared<Pool_ObjTile_4> t_gold_key
            {{&data::key_tbank[data::key_gold_data[0][0]]}, 8};

    PoolSprite corner_1{gui_pal, t_corner, {{},1}, 
            new_obj_attr_8x8(0,0, 0,SpriteMode::Normal)};

    PoolSprite corner_2{gui_pal, t_corner, {{},1}, 
            new_obj_attr_8x8(0,8, 0,SpriteMode::Normal)};

    PoolSprite hbar_icon{gui_pal, t_hbar_icon, {{},1}, 
            new_obj_attr_8x8(232,0, 0,SpriteMode::Normal)};

    PoolSprite gbar_icon{gui_pal, t_gbar_icon, {{},1}, 
            new_obj_attr_8x8(232,8, 0,SpriteMode::Normal)};

    #define HBAR_EMPTY(i) {gui_pal, t_hbar_empty, {{},1}, \
            new_obj_attr_8x8(229-i*3,0, 0,SpriteMode::Disable)}

    PoolSprite hbar_empty[MAX_HBAR_COUNT] = {
            HBAR_EMPTY(0), HBAR_EMPTY(1), HBAR_EMPTY(2), HBAR_EMPTY(3) };

    #define HBAR_FULL(i) {gui_pal, t_hbar_full, {{},1}, \
            new_obj_attr_8x8(229-i*3,0, 0,SpriteMode::Disable)}

    PoolSprite hbar_full[MAX_HBAR_COUNT] = {
            HBAR_FULL(0), HBAR_FULL(1), HBAR_FULL(2), HBAR_FULL(3) };

    #define GBAR_EMPTY(i) {gui_pal, t_gbar_empty, {{},1}, \
            new_obj_attr_8x8(229-i*3,8, 0,SpriteMode::Disable)}

    PoolSprite gbar_empty[MAX_GBAR_COUNT] = {
            GBAR_EMPTY(0), GBAR_EMPTY(1), GBAR_EMPTY(2), GBAR_EMPTY(3) };

    #define GBAR_FULL(i) {gui_pal, t_gbar_full, {{},1}, \
            new_obj_attr_8x8(229-i*3,8, 0,SpriteMode::Disable)}

    PoolSprite gbar_full[MAX_GBAR_COUNT] = {
            GBAR_FULL(0), GBAR_FULL(1), GBAR_FULL(2), GBAR_FULL(3) };

    PoolSprite pistol{gui_pal, t_pistol, {{},1}, 
            new_obj_attr_32x16(0,0, 0,SpriteMode::Normal)};

    PoolSprite green_key{gui_pal, t_green_key, {{},1}, 
            new_obj_attr_8x8(0,11, 0,SpriteMode::Normal)};

    PoolSprite blue_key{gui_pal, t_blue_key, {{},1}, 
            new_obj_attr_8x8(5,11, 0,SpriteMode::Normal)};

    PoolSprite gold_key{gui_pal, t_gold_key, {{},1}, 
            new_obj_attr_8x8(10,11, 0,SpriteMode::Normal)};
};


export inline void init(Data& data) {
    data.cur_max_hbar_count = 0;
    data.cur_max_gbar_count = 0;
    data.cur_gbar_count = 0;
    data.cur_hbar_count = 0;
    data.upd_flags = 0;
    data.hidden = false;
}


export inline void init_vblank(Data& data) {
    enable_sprite(data.corner_1);
    enable_sprite(data.corner_2);
    /*for(u32 i=0; i<MAX_HBAR_COUNT; ++i) {
        enable_sprite(data.hbar_full[MAX_HBAR_COUNT - i - 1]);
        enable_sprite(data.hbar_empty[MAX_HBAR_COUNT - i - 1]);
    }
    for(u32 i=0; i<MAX_GBAR_COUNT; ++i) {
        enable_sprite(data.gbar_full[MAX_GBAR_COUNT - i - 1]);
        enable_sprite(data.gbar_empty[MAX_GBAR_COUNT - i - 1]);
    }*/
    enable_sprite(data.hbar_icon);
    enable_sprite(data.gbar_icon);
    enable_sprite(data.pistol);
    update_sprite_attrs(data.hbar_icon);
    update_sprite_attrs(data.gbar_icon);
    update_sprite_attrs(data.pistol);
}


export inline void free_vblank(Data& data) {
    disable_sprite_if(data.corner_1);
    disable_sprite_if(data.corner_2);
    for(u32 i=0; i<MAX_HBAR_COUNT; ++i) {
        disable_sprite_if(data.hbar_full[i]);
        disable_sprite_if(data.hbar_empty[i]);
    }
    for(u32 i=0; i<MAX_GBAR_COUNT; ++i) {
        disable_sprite_if(data.gbar_full[i]);
        disable_sprite_if(data.gbar_empty[i]);
    }
    disable_sprite_if(data.hbar_icon);
    disable_sprite_if(data.gbar_icon);
    disable_sprite_if(data.pistol);
    disable_sprite_if(data.green_key);
    disable_sprite_if(data.blue_key);
    disable_sprite_if(data.gold_key);
}


export inline void update_bars(
        u32& cur_max,
        u32& cur_val,
        u32& update_flags,
        const UpdFlags flag,
        const u32 new_max,
        const u32 new_val,
        const u32 max_bar_count,
        PoolSprite& corner,
        PoolSprite* bars_full,
        PoolSprite* bars_empty) {
    if(cur_max != new_max || cur_val != new_val) {
        update_flags |= flag;
        cur_val = new_val;
        cur_max = new_max;
        for(u32 i=0; i<new_val; ++i) {
            enable_sprite_if(bars_full[i]);
            disable_sprite_if(bars_empty[i]);
            set_obj_attr_mode(bars_full[i].obj_attr, SpriteMode::Normal);
        }
        for(u32 i=new_val; i<new_max; ++i) {
            disable_sprite_if(bars_full[i]);
            enable_sprite_if(bars_empty[i]);
            set_obj_attr_mode(bars_empty[i].obj_attr, SpriteMode::Normal);
        }
        for(u32 i=new_max; i<max_bar_count; ++i) {
            disable_sprite_if(bars_full[i]);
            disable_sprite_if(bars_empty[i]);
        }
        set_obj_attr_px(corner.obj_attr, 232 - 3*new_max - 1);
    }
}
        


export inline void update_vblank(
        GameData& /*game_data*/, Data& data, const PlayerStats& stats) {
    if(data.hidden)
        return;
    // health bars
    update_bars(data.cur_max_hbar_count,
                data.cur_hbar_count,
                data.upd_flags,
                UPD_HBAR,
                stats.max_health,
                stats.cur_health,
                MAX_HBAR_COUNT,
                data.corner_1,
                data.hbar_full,
                data.hbar_empty);
    // ammo bars
    update_bars(data.cur_max_gbar_count,
                data.cur_gbar_count,
                data.upd_flags,
                UPD_GBAR,
                stats.max_pistol_ammo,
                stats.cur_pistol_ammo,
                MAX_GBAR_COUNT,
                data.corner_2,
                data.gbar_full,
                data.gbar_empty); 
}


export inline void draw(
        GameData& game_data, Data& data, const PlayerStats& stats) {
    update_vblank(game_data, data, stats);

    if(data.upd_flags & UPD_HBAR) {
        update_sprite_attrs(data.corner_1);
        for(u32 i=0; i<MAX_HBAR_COUNT; ++i) {
            update_sprite_attrs_if(data.hbar_full[i]);
            update_sprite_attrs_if(data.hbar_empty[i]);
        }
    }
    if(data.upd_flags & UPD_GBAR) {
        update_sprite_attrs(data.corner_2);
        for(u32 i=0; i<MAX_HBAR_COUNT; ++i) {
            update_sprite_attrs_if(data.gbar_full[i]);
            update_sprite_attrs_if(data.gbar_empty[i]);
        }
    }
    if(data.upd_flags & UPD_OTHER) {
        update_sprite_attrs(data.hbar_icon);
        update_sprite_attrs(data.gbar_icon);
        update_sprite_attrs(data.pistol);
    }
    
    // keys
    if(!data.hidden) {
        if(game_data.session_data.key_flags & GREEN_KEY_FLAG) {
            enable_sprite_if(data.green_key);
            update_sprite_attrs(data.green_key);
        } else disable_sprite_if(data.green_key);
        if(game_data.session_data.key_flags & BLUE_KEY_FLAG) {
            enable_sprite_if(data.blue_key);
            update_sprite_attrs(data.blue_key);
        } else disable_sprite_if(data.blue_key);
        if(game_data.session_data.key_flags & GOLD_KEY_FLAG) {
            enable_sprite_if(data.gold_key);
            update_sprite_attrs(data.gold_key);
        } else disable_sprite_if(data.gold_key);
    }

    data.upd_flags = 0;
}

export inline void hide(Data& data) {
    data.upd_flags = UPD_ALL;
    data.hidden = true;
    set_obj_attr_mode(data.corner_1.obj_attr, SpriteMode::Disable);
    set_obj_attr_mode(data.corner_2.obj_attr, SpriteMode::Disable);
    for(u32 i=0; i<MAX_HBAR_COUNT; ++i) {
        /*set_obj_attr_mode(data.hbar_full[i].obj_attr, SpriteMode::Disable);
        set_obj_attr_mode(data.hbar_empty[i].obj_attr, SpriteMode::Disable);*/
        disable_sprite_if(data.hbar_full[i]);
        disable_sprite_if(data.hbar_empty[i]);
    }
    for(u32 i=0; i<MAX_GBAR_COUNT; ++i) {
        /*set_obj_attr_mode(data.gbar_full[i].obj_attr, SpriteMode::Disable);
        set_obj_attr_mode(data.gbar_empty[i].obj_attr, SpriteMode::Disable);*/
        disable_sprite_if(data.gbar_full[i]);
        disable_sprite_if(data.gbar_empty[i]);
    }
    set_obj_attr_mode(data.hbar_icon.obj_attr, SpriteMode::Disable);
    set_obj_attr_mode(data.gbar_icon.obj_attr, SpriteMode::Disable);
    set_obj_attr_mode(data.pistol.obj_attr, SpriteMode::Disable);
    disable_sprite_if(data.green_key);
    disable_sprite_if(data.blue_key);
    disable_sprite_if(data.gold_key);
}

export inline void show(Data& data) {
    data.upd_flags = UPD_ALL;
    data.hidden = false;
    set_obj_attr_mode(data.corner_1.obj_attr, SpriteMode::Normal);
    set_obj_attr_mode(data.corner_2.obj_attr, SpriteMode::Normal);
    set_obj_attr_mode(data.hbar_icon.obj_attr, SpriteMode::Normal);
    set_obj_attr_mode(data.gbar_icon.obj_attr, SpriteMode::Normal);
    set_obj_attr_mode(data.pistol.obj_attr, SpriteMode::Normal);
    data.cur_max_hbar_count = 0;
    data.cur_max_gbar_count = 0;
    data.cur_gbar_count = 0;
    data.cur_hbar_count = 0;
}

export inline bool is_hidden(const Data& data) {
    return data.hidden;
}

} // player
} // game
