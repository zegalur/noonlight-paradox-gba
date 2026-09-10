/*!****************************************************************************
 * @file graphics.cppm
 * @brief Low-level graphics API.
 *******************************************************************************/        

module;
export module engine:graphics;

import :types;
import :memory;

#include "macro.hpp"

namespace gba {

// -------------------------------- Palette -------------------------------- //

export constexpr auto PALETTE_COLORS = 16;
export constexpr auto PALETTE_COUNT = 16;

export using Color = u16;

export struct Palette {
    u32 data[8];
};

export inline auto& bg_palette = 
        *reinterpret_cast<Palette(*)[PALETTE_COUNT]>(MEM_BG_PAL);

export inline auto& obj_palette = 
        *reinterpret_cast<Palette(*)[PALETTE_COUNT]>(MEM_OBJ_PAL);


// --------------------------------- Tiles --------------------------------- //

// 8x8 4bpp : 32 bytes
export struct Tile_4 {
    u32 data[8];
};

// 8x8 8bpp : 64 bytes
export struct Tile_8 {
    u32 data[16];
};

export constexpr Tile_4 EMPTY_TILE = 
    {{ 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
       0x00000000, 0x00000000, 0x00000000, 0x00000000 }};

export constexpr auto TILE_4_COUNT = 512;
export constexpr auto TILE_8_COUNT = 256;
export constexpr auto CHARBLOCK_COUNT = 5;

export using Charblock_4 = Tile_4[TILE_4_COUNT];
export using Charblock_8 = Tile_8[TILE_8_COUNT];

export inline auto& tile_memory_4 = 
        *reinterpret_cast<Charblock_4(*)[CHARBLOCK_COUNT]>(MEM_VRAM);

export inline auto& tile_memory_8 = 
        *reinterpret_cast<Charblock_8(*)[CHARBLOCK_COUNT]>(MEM_VRAM);


// ----------------------------- Screen Blocks ----------------------------- //

export constexpr auto SCREENBLOCK_SIZE = 1024;
export constexpr auto SCREENBLOCK_COUNT = 32;

export using ScreenEntry = u16;
export using Screenblock = ScreenEntry[SCREENBLOCK_SIZE];

export inline auto& screenblocks = 
        *reinterpret_cast<Screenblock(*)[SCREENBLOCK_COUNT]>(MEM_VRAM);


// ------------------------------ Backgrounds ------------------------------ //

export constexpr auto BG_COUNT = 4;


// -------------------------------- Sprites -------------------------------- //

export struct ALIGN(4) ObjAttr {
    u16 attr_0;
    u16 attr_1;
    u16 attr_2;
    i16 _fill;
};

export struct ALIGN(4) ObjAffine {
    u16 _fill0[3];
    i16 pa;
    u16 _fill1[3];
    i16 pb;
    u16 _fill2[3];
    i16 pc;
    u16 _fill3[3];
    i16 pd;
};

export constexpr auto OBJ_ATTR_COUNT = 128;
export constexpr auto OBJ_AFFINE_COUNT = 32;

export inline auto& obj_attrs = 
        *reinterpret_cast<ObjAttr(*)[OBJ_ATTR_COUNT]>(MEM_OAM);

export inline auto& obj_affines = 
        *reinterpret_cast<ObjAffine(*)[OBJ_AFFINE_COUNT]>(MEM_OAM);


/// @brief Hardware sprite shape.
export enum SpriteShape {
    Square = 0b00,
    Wide = 0b01,
    Tall = 0b10
};

/// @brief Hardware sprite size.
export enum SpriteSize {
    Size_00 = 0b00, // 8x8, 16x8, 8x16
    Size_01 = 0b01, // 16x16, 32x8, 8x32
    Size_10 = 0b10, // 32x32, 32x16, 16x32
    Size_11 = 0b11, // 64x64, 64x32, 32x64
};

export enum SpriteMode {
    Normal = 0b00 << 8,
    Affine = 0b01 << 8,
    Disable = 0b10 << 8,
    AffineDR = 0b11 << 8,
    Mask = 0b11 << 8
};

export inline ObjAttr new_obj_attr(
        const SpriteShape shape, const SpriteSize size, const SpriteMode mode,
        const u32 px, const u32 py, const u32 priority) {
    return ObjAttr{
            u16((py & 0xFF) | (shape << 0xE) | mode), 
            u16((px & 0x1FF) | (size << 0xE)), 
            u16(priority << 0xA),
            u16(0)};
}

export inline ObjAttr new_obj_attr_8x8(
        const u32 px, const u32 py, const u32 priority, const SpriteMode mode) {
    return new_obj_attr(
        SpriteShape::Square, SpriteSize::Size_00, mode, px, py, priority);
}
export inline ObjAttr new_obj_attr_16x8(
        const u32 px, const u32 py, const u32 priority, const SpriteMode mode) {
    return new_obj_attr(
        SpriteShape::Wide, SpriteSize::Size_00, mode, px, py, priority);
}
export inline ObjAttr new_obj_attr_16x16(
        const u32 px, const u32 py, const u32 priority, const SpriteMode mode) {
    return new_obj_attr(
        SpriteShape::Square, SpriteSize::Size_01, mode, px, py, priority);
}
export inline ObjAttr new_obj_attr_32x32(
        const u32 px, const u32 py, const u32 priority, const SpriteMode mode) {
    return new_obj_attr(
        SpriteShape::Square, SpriteSize::Size_10, mode, px, py, priority);
}
export inline ObjAttr new_obj_attr_32x16(
        const u32 px, const u32 py, const u32 priority, const SpriteMode mode) {
    return new_obj_attr(
        SpriteShape::Wide, SpriteSize::Size_10, mode, px, py, priority);
}
export inline ObjAttr new_obj_attr_16x32(
        const u32 px, const u32 py, const u32 priority, const SpriteMode mode) {
    return new_obj_attr(
        SpriteShape::Tall, SpriteSize::Size_10, mode, px, py, priority);
}
export inline ObjAttr new_obj_attr_64x64(
        const u32 px, const u32 py, const u32 priority, const SpriteMode mode) {
    return new_obj_attr(
        SpriteShape::Square, SpriteSize::Size_11, mode, px, py, priority);
}

export inline void set_obj_attr_px(ObjAttr& obj_attr, const u16 px) {
    obj_attr.attr_1 &= ~u16(0x1FF);
    obj_attr.attr_1 |= u16(px & 0x1FF);
}

export inline void set_obj_attr_py(ObjAttr& obj_attr, const u16 py) {
    obj_attr.attr_0 &= ~u16(0xFF);
    obj_attr.attr_0 |= u16(py & 0xFF);
}

export inline void set_obj_attr_mode(ObjAttr& obj_attr, const SpriteMode mode) {
    obj_attr.attr_0 &= ~u16(SpriteMode::Mask);
    obj_attr.attr_0 |= u16(mode);
}

} // namespace gba
