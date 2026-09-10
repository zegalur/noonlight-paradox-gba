/*!****************************************************************************
 * @file sprites.cppm
 * @brief Sprite resources management.
 *******************************************************************************/

module;
export module engine:sprites;

import :types;
import :graphics;
import :resources;

namespace gba {

/// @brief Disables all the sprites (all 128 are enabled by default).
export void disable_all_sprites() {
    for(u32 i=0; i<OBJ_ATTR_COUNT; ++i)
        obj_attrs[i].attr_0 = 2 << 8;
}


/// @brief Enables a sprite (common code).
template<typename T> inline void enable_sprite_func(T& s) {
    use(s.palette);
    use(s.tiles);
    use(s.sprite);
    // reset the TID and PB values, but keep the priority
    s.obj_attr.attr_2 &= (0b11 << 0xA);
    // set the proper TID and PB values
    s.obj_attr.attr_2 |= s.tiles.id | (s.palette.id << 0xC);
}


/// @brief Updates sprite attrs value (common code).
template<typename T> inline void update_sprite_attrs_func(T& s) {
    obj_attrs[s.sprite.id] = s.obj_attr;
}

/// @brief Disables a sprite (common code).
template<typename T> inline void disable_sprite_func(T& s) {
    obj_attrs[s.sprite.id].attr_0 = 2 << 8;
    free(s.sprite);
    free(s.tiles);
    free(s.palette);
}


// ---------------------------- Static Sprites ----------------------------- //

export template<typename PalRes, typename TileRes, typename SpriteRes>
struct Sprite {
    Shared<PalRes>& palette;
    Shared<TileRes>& tiles;
    Shared<SpriteRes> sprite;
    ObjAttr obj_attr;
};

export using StackSprite = 
        Sprite<Stack_ObjPalette, Stack_ObjTile_4, Stack_SpriteRes>;
export using PoolSprite = 
        Sprite<Pool_ObjPalette, Pool_ObjTile_4, Pool_SpriteRes>;

export template<typename P, typename T, typename S>
void enable_sprite(Sprite<P,T,S>& s) {
    enable_sprite_func(s);
}

export template<typename P, typename T, typename S>
void enable_sprite_if(Sprite<P,T,S>& s) {
    if(s.sprite.in_use == 0)
        enable_sprite(s);
}

export template<typename P, typename T, typename S>
void update_sprite_attrs(Sprite<P,T,S>& s) {
    update_sprite_attrs_func(s);
}

export template<typename P, typename T, typename S>
void update_sprite_attrs_if(Sprite<P,T,S>& s) {
    if(s.sprite.in_use)
        update_sprite_attrs_func(s);
}

export template<typename P, typename T, typename S>
void disable_sprite(Sprite<P,T,S>& s) {
    disable_sprite_func(s);
}

export template<typename P, typename T, typename S>
void disable_sprite_if(Sprite<P,T,S>& s) {
    if(s.sprite.in_use)
        disable_sprite(s);
}


// --------------------------- Animated Sprites ---------------------------- //

export template<
        typename PalRes,
        typename TilePlaceholderRes,
        typename SpriteRes>
struct AnimSprite {
    Shared<PalRes>& palette;
    Shared<TilePlaceholderRes>& tiles;
    Shared<SpriteRes> sprite;
    ObjAttr obj_attr;
};

export using StackAnimSprite =
    AnimSprite<Stack_ObjPalette, Stack_ObjTile_4_Placeholder, Stack_SpriteRes>;
export using PoolAnimSprite =
    AnimSprite<Pool_ObjPalette, Pool_ObjTile_4_Placeholder, Pool_SpriteRes>;

export template<typename P, typename T, typename S>
void enable_sprite(AnimSprite<P,T,S>& s) {
    enable_sprite_func(s);
}

export template<typename P, typename T, typename S>
void enable_sprite_if(AnimSprite<P,T,S>& s) {
    if(s.sprite.in_use == 0)
        enable_sprite(s);
}

u32 UPD_FULL_FRAME = ~u32(0);

export template<typename P, typename T, typename S, typename A>
inline void update_sprite_frame(
        AnimSprite<P,T,S>& s,
        const u16** data,
        const u32 frame_tcount,
        const A animation,
        const u32 frame,
        const A prev_anim = A(0),
        const u32 prev_frame = UPD_FULL_FRAME) {
    if(prev_frame == UPD_FULL_FRAME) {
        for(u32 j=0; j<frame_tcount; ++j)
            tile_memory_4[4][s.tiles.id + j] =
                    s.tiles.resource.data[data[animation][frame*frame_tcount + j]];
    } else {
        for(u32 j=0; j<frame_tcount; ++j) {
            const auto prev_tid = data[prev_anim][prev_frame*frame_tcount + j];
            const auto new_tid = data[animation][frame*frame_tcount + j];
            if(prev_tid != new_tid) {
                tile_memory_4[4][s.tiles.id + j] =
                    s.tiles.resource.data[data[animation][frame*frame_tcount + j]];
            }
        }
    }
}

export template<typename P, typename T, typename S>
void update_sprite_attrs(AnimSprite<P,T,S>& s) {
    update_sprite_attrs_func(s);
}

export template<typename P, typename T, typename S>
void update_sprite_attrs_if(AnimSprite<P,T,S>& s) {
    if(s.sprite.in_use)
        update_sprite_attrs_func(s);
}

export template<typename P, typename T, typename S>
void disable_sprite(AnimSprite<P,T,S>& s) {
    disable_sprite_func(s);
}

export template<typename P, typename T, typename S>
void disable_sprite_if(AnimSprite<P,T,S>& s) {
    if(s.sprite.in_use)
        disable_sprite(s);
}

} // namespace gba
