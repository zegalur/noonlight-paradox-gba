/*!****************************************************************************
 * @file tilemask.cppm
 * @brief Use this module to draw small foreground tile mask.
 *        This should be used together with a properly set-up main TileMap layer.
 *******************************************************************************/        

module;
export module engine:tilemask;

import :types;
import :video;
import :graphics;
import :resources;

namespace gba {


/// @brief This structure holds all the necessary data to draw a tilemask.
export struct TileMask {
    /// @brief Unique tiles, used in the tilemap.
    const Tile_4* tile_array;
    /// @brief A grid of tile references that constitute the tilemask.
    const u32* sb_array;
    /// @brief Tilemap width (in tiles).
    i32 tilemask_full_size_x;
    /// @brief Tilemap height (in tiles).
    i32 tilemask_full_size_y;
    /// @brief Tilemask visible part' width (in tiles).
    i32 mask_size_x;
    /// @brief Tilemask visible part' height (in tiles).
    i32 mask_size_y;

    /// @brief A charblock to use to hold the `Tiles`.
    u32 charblock;
    /// @brief The offset inside the charblock to the first tile.
    u32 charblock_offset;
    /// @brief A screenblock to use to hold the `Background` tile references.
    u32 screenblock;

    u32 empty_tile_id;

    /// @brief The corresponding `REG_BGXHOFS` register.
    volatile u16* bg_x_offset;
    /// @brief The corresponding `REG_BGXVOFS` register.
    volatile u16* bg_y_offset;

    i32 last_pos_tx;
    i32 last_pos_ty;
};


/// @brief ...
/// Expects the main layer to be initialized to use its palettes and camera.
/// @param charblock_offset Sets from where the 64 tilemask tiles will be saved.
export void init_tilemask(
        TileMask& tilemask,
        const u32 mask_size_x,
        const u32 mask_size_y,
        const u32 bg_layer,
        const u32 bg_priority,
        const u32 charblock,
        const u32 charblock_offset,
        const u32 empty_tile_id,
        const Tile_4* const tile_array,
        const u32 screenblock,
        const u32 tilemap_size_x,
        const u32 tilemap_size_y,
        const u32* const screenblock_array
        ) {
    volatile u16* bg_controls[] =
            {&REG_BG0CNT, &REG_BG1CNT, &REG_BG2CNT, &REG_BG3CNT};
    volatile u16* bg_x_offsets[] =
            {&REG_BG0HOFS, &REG_BG1HOFS, &REG_BG2HOFS, &REG_BG3HOFS};
    volatile u16* bg_y_offsets[] =
            {&REG_BG0VOFS, &REG_BG1VOFS, &REG_BG2VOFS, &REG_BG3VOFS};
    
    // set to be a 32 x 32 tiles (256 x 256 px) 4 bpp background
    const u32 bgcnt_value =
            ( (bg_priority << 0x0) // layer priority
            | (charblock << 0x2) // character base block
            | (screenblock << 0x8) // screen base block
            );
    *(bg_controls[bg_layer]) = bgcnt_value;

    // initialize the TileMask structure
    tilemask.tile_array = tile_array;
    tilemask.sb_array = screenblock_array;
    tilemask.tilemask_full_size_x = tilemap_size_x;
    tilemask.tilemask_full_size_y = tilemap_size_y;
    tilemask.mask_size_x = mask_size_x;
    tilemask.mask_size_y = mask_size_y;
    tilemask.charblock = charblock;
    tilemask.charblock_offset = charblock_offset;
    tilemask.screenblock = screenblock;
    tilemask.empty_tile_id = empty_tile_id;
    tilemask.bg_x_offset = bg_x_offsets[bg_layer];
    tilemask.bg_y_offset = bg_y_offsets[bg_layer];
    // set to some big unrealistic value
    tilemask.last_pos_tx = tilemask.last_pos_ty = i32(1)<<i32(30);

    // fully clearing up the screenblocks
    for(u32 i=0; i<SCREENBLOCK_SIZE; ++i)
        screenblocks[screenblock][i] = empty_tile_id;
}

export inline void reset_tilemask(TileMask& tilemask) {
    tilemask.last_pos_tx = tilemask.last_pos_ty = i32(1)<<i32(30);
}


// TODO: write an optimized version with shift support
export inline void update_tilemask(
        TileMask& tilemask,
        i32 new_pos_tx,
        i32 new_pos_ty
        ) {
    if(new_pos_tx == tilemask.last_pos_tx)
        if(new_pos_ty == tilemask.last_pos_ty)
            return; // skip, already there
    tilemask.last_pos_tx = new_pos_tx;
    tilemask.last_pos_ty = new_pos_ty;
    for(i32 y = 0; y < tilemask.mask_size_y; ++y) {
        const i32 ty = y + new_pos_ty;
        for(i32 x = 0; x < tilemask.mask_size_x; ++x) {
            const i32 tx = x + new_pos_tx;
            u32 sb_tid = 0;
            u32 sb_data = 0;
            if(tx >= 0 && tx < tilemask.tilemask_full_size_x) {
                if(ty >= 0 && ty < tilemask.tilemask_full_size_y) {
                    const u32 sb_element = tilemask.sb_array[
                            ty*tilemask.tilemask_full_size_x + tx];
                    sb_tid = sb_element & ((1 << 16) - 1);
                    sb_data = sb_element >> 16;
                }
            }
            const i32 bg_cell_id = x + y*32;
            if(sb_tid == 0) {
                sb_tid = tilemask.empty_tile_id;
            } else {
                // copy a non-empty tile data
                const i32 tcell = 
                        tilemask.charblock_offset + y*tilemask.mask_size_x + x;
                tile_memory_4[tilemask.charblock][tcell] = 
                        tilemask.tile_array[sb_tid];
                sb_tid = tcell;
            }
            screenblocks[tilemask.screenblock][bg_cell_id] = sb_tid | sb_data;
        }
    }
}

export inline void set_tilemask_bg_offset(
        TileMask& tilemask, const i32 x_offset, const i32 y_offset) {
    *(tilemask.bg_x_offset) = x_offset & 255;
    *(tilemask.bg_y_offset) = y_offset & 255;
}

} // namespace gba
