/*!****************************************************************************
 * @file collision.cppm
 * @brief A tilemap based collision and resolution engine.
 *******************************************************************************/        

module;
export module engine:collision;

import :types;
import :graphics;

namespace gba {


/// @brief ...
export enum CollisionColor {
    FREE = 0,
    PLATFORM = 0x7fe0
};


/// @brief ...
export struct CollisionMap {
    /// @brief Unique tiles, used in the tilemap.
    const Tile_4* tile_array;
    /// @brief A grid of tile references that constitute the tilemask.
    const u32* sb_array;
    /// @brief Tilemap width (in tiles).
    i32 size_tx;
    /// @brief Tilemap height (in tiles).
    i32 size_ty;

    /// @brief The index of the platform color.
    u32 platform_cid;
};


/// @brief ...
export void init_collision_map(
        CollisionMap& cmap,
        const Tile_4* const tile_array,
        const u32* screenblock_array,
        const u32 tilemap_size_x,
        const u32 tilemap_size_y,
        const Palette* const palette
        ) {
    // initialize the collision map structure
    cmap.tile_array = tile_array;
    cmap.sb_array = screenblock_array;
    cmap.size_tx = i32(tilemap_size_x);
    cmap.size_ty = i32(tilemap_size_y);

    // analyze the palette
    for(u32 i=0; i<8; ++i) {
        const u32 colors[2] = { 
                palette[0].data[i] & u32(0xffff),
                palette[0].data[i] >> u32(16) };
        for(u32 j=0; j<2; ++j) {
            switch (colors[j]) {
                case CollisionColor::PLATFORM:
                    cmap.platform_cid = 2*i + j;
                    break;
            }
        }
    }
}


/// @brief ...
export inline CollisionColor get_collision_color(
        const CollisionMap& cmap,
        const i32 pos_px,
        const i32 pos_py) {
    const i32 tx = pos_px >> 3;
    const i32 ty = pos_py >> 3;
    if(pos_px < 0 || tx >= cmap.size_tx)
        return CollisionColor::FREE;
    if(pos_py < 0 || ty >= cmap.size_ty)
        return CollisionColor::FREE;
    const u32 tid = cmap.sb_array[tx + ty*cmap.size_tx];
    if(tid == 0)
        return CollisionColor::FREE;
    const Tile_4& tile = cmap.tile_array[tid];
    const i32 rx = pos_px & 0x7;
    const i32 ry = pos_py & 0x7;
    const u32 tile_row = tile.data[ry];
    const u32 pixel_color = (tile_row >> (u32(4)*rx)) & u32(0xF);
    if(cmap.platform_cid == pixel_color)
        return CollisionColor::PLATFORM;
    return CollisionColor::FREE;//CollisionColor(pixel_color);
}


// TODO: optimize (use the information about tile type etc)
/// @brief Returns the first pixel with the set color from the given starting
///        position (using specified direction as the search direction).
/// @return Returns `true` when found (inside the map), `false` when it doesn't. 
export inline bool find_first_from(
        const CollisionColor ccol,
        const CollisionMap& cmap,
        i32 &px,
        i32 &py,
        const i32 step_px,
        const i32 step_py,
        const i32 max_steps) {
    i32 step = 0;
    while(get_collision_color(cmap, px, py) != ccol) {
        px += step_px;
        py += step_py;
        if(px < 0 || px >= 8*cmap.size_tx)
            return false;
        if(py < 0 || py >= 8*cmap.size_ty)
            return false;
        if(++step >= max_steps)
            return false;
    }
    return true;
}


} // namespace gba
