/*!****************************************************************************
 * @file tilemap.cppm
 * @brief Use this module to draw big full screen tilemaps.
 *
 * Definitions:
 *
 * 1) `Screen` - what we eventually see (literally on the screen)
 *      +---------------+   
 *      |...............|   Width:  SCREEN_WIDTH px (SCREEN_WIDTH_T tiles)
 *      |...............|   Height: SCREEN_HEIGHT px (SCREEN_HEIGHT_T tiles)
 *      +---------------+
 *
 * 2) `Tilemap` - a huge grid of tile references, a part of which we later 
 *    draw on the `Screen`.
 *      +===========================+ 
 *      ||                         ||   W: tilemap_size_x (tiles)
 *      ||                         ||   H: tilemap_size_y (tiles)
 *      ||                         ||   
 *      +===========================+ 
 *
 * 3) `Camera` - sets the part of the `Tilemap` we currently see on the `Screen`.
 *      +===========================+ 
 *      || ->[.......]             ||   (camera_pos_x, camera_pos_y):
 *      ||   [.......]             ||       Position (in pixels) inside the
 *      ||                         ||       `Tilemap` (from top-left corner).
 *      +===========================+ 
 *
 * 4) `Tiles` - a set of tiles inside the memory that we use to form the image.
 *    Note, that some tiles inside the "frame" are not on the `Screen`.
 *      [.][r][ ][ ][l][.][.][.][.][.]  (1,2,3,4) - screen corners
 *      [b][4][ ][ ][3][b][b][b][b][b]  t - first top row outside of `Screen`
 *      [ ][ ][ ][ ][ ][ ][ ][ ][ ][ ]  b - first bottom row outside of `Screen`
 *      [t][2][ ][ ][1][t][t][t][t][t]  l - first left column outside of `Screen`
 *      [.][r][ ][ ][l][ ][.][.][.][.]  r - first right column outside of `Screen`
 *      [.][r][ ][ ][l][.][.][.][.][.]  
 *      [.][r][ ][ ][l][.][ ][.][.][.]  (W,H): (TILE_COL_COUNT, TILE_ROW_COUNT)
 *
 * 5) `Background` - a grid of the references to the tiles from the `Tiles`.
 *    Background is a circular grid that GBA render out on the `Screen`.
 *    (Also has the top, bottom, left, and right outside rows/columns too.)
 *      +~~~~~~~~~~~~~~~~~~~~~~~~~~~+
 *      I                           I   (W,H): (BG_WIDTH, BG_HEIGHT) (in tiles)
 *      I::::)              ->(:::::I   
 *      I::::)                (:::::I   (bg_x_offset, bg_y_offset):
 *      I                           I       Camera position inside the 
 *      +~~~~~~~~~~~~~~~~~~~~~~~~~~~+       `Background` (from left-top corner)
 *
 *******************************************************************************/        

module;
export module engine:tilemap;

import :types;
import :video;
import :graphics;
import :resources;

namespace gba {

namespace {
    /// @brief The width of the `Tiles` grid in tiles. (see above)
    constexpr i32 TILE_COL_COUNT = 32;
    /// @brief The height of the `Tiles` grid in tiles. (see above)
    constexpr i32 TILE_ROW_COUNT = SCREEN_HEIGHT_T + 1;
    constexpr i32 EMPTY_TILE_ID = TILE_COL_COUNT * TILE_ROW_COUNT;
    /// @brief `Background` width in tiles. (see above)
    constexpr u32 BG_WIDTH = 32;
    /// @brief `Background` height in tiles. (see above)
    constexpr u32 BG_HEIGHT = 32;
}

/// @brief This structure holds all the necessary data to draw a tilemap.
export struct TileMap {
    /// @brief Unique tiles, used in the tilemap.
    const Tile_4* tile_array;
    /// @brief When set `true`, disables the `Tiles` streaming and forces to 
    ///        use fixed tile indexes instead.
    bool fixed_tiles;
    /// @brief A grid of tile references that constitute the tilemap.
    const u32* sb_array;
    /// @brief Tilemap width (in tiles).
    i32 tilemap_size_x;
    /// @brief Tilemap height (in tiles).
    i32 tilemap_size_y;

    /// @brief A charblock to use to hold the `Tiles`.
    u32 charblock;
    /// @brief The offset inside the charblock to the first tile.
    u32 charblock_offset;
    /// @brief A screenblock to use to hold the `Background` tile references.
    u32 screenblock;

    /// @brief Current camera X-position inside the tilemap (in pixels).
    i32 camera_pos_x;
    /// @brief Current camera Y-position inside the tilemap (in pixels).
    i32 camera_pos_y;

    /// @brief The last value of the corresponding `REG_BGXHOFS`.
    i32 prev_bg_x_offset;
    /// @brief The last value of the corresponding `REG_BGXVOFS`.
    i32 prev_bg_y_offset;
    /// @brief The corresponding `REG_BGXHOFS` register.
    volatile u16* bg_x_offset;
    /// @brief The corresponding `REG_BGXVOFS` register.
    volatile u16* bg_y_offset;

    /// @brief The first `Tiles` top row outside of `Screen`.
    i32 top_tile_row;
    /// @brief The first `Tiles` bottom row outside of `Screen`.
    i32 bottom_tile_row;
    /// @brief The first `Tiles` left col outside of `Screen`.
    i32 left_tile_col;
    /// @brief The first `Tiles` right col outside of `Screen`.
    i32 right_tile_col;

    /// @brief The first `Background` top row outside of `Screen`.
    i32 top_bg_row;
    /// @brief The first `Background` bottom row outside of `Screen`.
    i32 bottom_bg_row;
    /// @brief The first `Background` left row outside of `Screen`.
    i32 left_bg_col;
    /// @brief The first `Background` right row outside of `Screen`.
    i32 right_bg_col;
};


/// @briefs Copies the tile and screenblock entry data for a given tilemap tile.
/// @param screenblock A screenblock that holds the `Background` tile references.
/// @param screenblock_array A grid of tile references that constitute the tilemap.
/// @param charblock A charblock that holds the `Tiles`.
/// @param charblock_offset The offset inside the charblock to the first tile.
/// @param tile_array Unique tiles, used in the tilemap.
/// @param tilemap_size_x Tilemap width (in tiles).
/// @param tmap_x The X-coordinate of the tile we want to copy (from `Tilemap`).
/// @param tmap_y The Y-coordinate of the tile we want to copy (from `Tilemap`).
/// @param tiles_x The X-coordinate of the tile we will copy into (in `Tiles`).
/// @param tiles_y The Y-coordinate of the tile we will copy into (in `Tiles`).
/// @param bg_x The X-coordinate of the tile we will copy into (in `Background`).
/// @param bg_y The Y-coordinate of the tile we will copy into (in `Background`).
inline void copy_tile_data(
        const bool fixed_tiles,
        const u32 screenblock,
        const u32* screenblock_array,
        const u32 charblock,
        const u32 charblock_offset,
        const Tile_4* tile_array,
        const u32 tilemap_size_x,
        const u32 tmap_x,
        const u32 tmap_y,
        const u32 tiles_x,
        const u32 tiles_y,
        const u32 bg_x,
        const u32 bg_y
        ) {
    const u32 sb_element = screenblock_array[tmap_x + tilemap_size_x*tmap_y];
    const u32 sb_tile_indx = sb_element & ((1 << 16) - 1);
    const u32 sb_data = sb_element >> 16;
    const u32 active_tile_indx = 
            (fixed_tiles ? sb_tile_indx :
            (sb_tile_indx == 0 ? EMPTY_TILE_ID : 
                charblock_offset + tiles_x + tiles_y*TILE_COL_COUNT));
    if(fixed_tiles == false && active_tile_indx != EMPTY_TILE_ID)
        tile_memory_4[charblock][active_tile_indx] = tile_array[sb_tile_indx];
    screenblocks[screenblock][bg_x + bg_y*BG_WIDTH] = sb_data | active_tile_indx;
}


/// @brief Initializes a `Tilemap` and copies all the data of the initial frame.
/// @param[out] tilemap The structure that will hold the `Tilemap` data.
/// @param bg_layer Background layer that will be used for the `Tilemap`.
/// @param bg_priority Background priority value.
/// @param palette_offset The offset to the tilemap palettes.
/// @param palette_count The number of palettes used in the tilemap.
/// @param palette_array The array of palettes.
/// @param charblock A charblock to use to hold the `Tiles`.
/// @param charblock_offset The offset inside the charblock to the first tile.
/// @param tile_count When this argument greater then 0, two things happen:
///        1) It uploads all the tiles at once.
///        2) The tilemap will not update the tiles, only the screenblocks.
///        Otherwise, it takes 31x21=651 tiles for the streamed dynamic tiles.
///        Use non-zero values for the middle- and far- background layers.
///        Use zero value for the big foreground layer that will be streamed.
///        For zero value make sure the zero tile is the empty tile.
/// @param tile_array The array of unique tiles, used in the tilemap.
/// @param screenblock A screenblock to use to hold the `Background` tile references.
/// @param tilemap_size_x Tilemap width (in tiles).
/// @param tilemap_size_y Tilemap height (in tiles).
/// @param screenblock_array A grid of tile references that constitute the tilemap.
/// @param camera_pos_x The initial camera X-position inside the tilemap (in pixels).
/// @param camera_pos_y The initial camera Y-position inside the tilemap (in pixels).
/// @param wrap_around_mode For the fixed `tile_count`, this will force to copy
///        all the screenblocks to fill the full 256x256 BG.
export void init_tilemap(
        TileMap& tilemap,
        
        const u32 bg_layer,
        const u32 bg_priority,

        const u32 palette_offset,
        const u32 palette_count,
        const Palette* palette_array,

        const u32 charblock,
        const u32 charblock_offset,
        const u32 tile_count,
        const Tile_4* tile_array,
    
        const u32 screenblock,
        const u32 tilemap_size_x,
        const u32 tilemap_size_y,
        const u32* screenblock_array,

        i32 camera_pos_x,
        i32 camera_pos_y,
        bool wrap_around_mode = false
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

    // fix the incorrect camera position
    if(camera_pos_x < i32(0))
        camera_pos_x = i32(0);
    if(camera_pos_y < i32(0))
        camera_pos_y = i32(0);
    if(camera_pos_x > i32(tilemap_size_x*8 - SCREEN_WIDTH))
        camera_pos_x = i32(tilemap_size_x*8 - SCREEN_WIDTH);
    if(camera_pos_y > i32(tilemap_size_y*8 - SCREEN_HEIGHT))
        camera_pos_y = i32(tilemap_size_y*8 - SCREEN_HEIGHT);

    const u32 camera_pos_tx = camera_pos_x >> 0x3;
    const u32 camera_pos_ty = camera_pos_y >> 0x3;
    const u32 camera_pos_ix = camera_pos_x & 0x7;
    const u32 camera_pos_iy = camera_pos_y & 0x7;

    // initialize the TileMap structure
    tilemap.tile_array = tile_array;
    tilemap.fixed_tiles = tile_count > 0;
    tilemap.tilemap_size_x = tilemap_size_x;
    tilemap.tilemap_size_y = tilemap_size_y;
    tilemap.sb_array = screenblock_array;
    tilemap.charblock = charblock;
    tilemap.charblock_offset = charblock_offset;
    tilemap.screenblock = screenblock;
    tilemap.camera_pos_x = camera_pos_x;
    tilemap.camera_pos_y = camera_pos_y;
    tilemap.prev_bg_x_offset = camera_pos_ix;
    tilemap.prev_bg_y_offset = camera_pos_iy;
    tilemap.bg_x_offset = bg_x_offsets[bg_layer];
    tilemap.bg_y_offset = bg_y_offsets[bg_layer];
    tilemap.top_tile_row = TILE_ROW_COUNT - 1;
    tilemap.left_tile_col = TILE_COL_COUNT - 1;
    tilemap.right_tile_col = SCREEN_WIDTH_T + (camera_pos_ix ? 1 : 0);
    tilemap.bottom_tile_row = SCREEN_HEIGHT_T + (camera_pos_iy ? 1 : 0);
    tilemap.top_bg_row = BG_HEIGHT - 1;
    tilemap.left_bg_col = BG_WIDTH - 1;
    tilemap.right_bg_col = SCREEN_WIDTH_T + (camera_pos_ix ? 1 : 0);
    tilemap.bottom_bg_row = SCREEN_HEIGHT_T + (camera_pos_iy ? 1 : 0);

    // copy the initially visible tiles

    if(tile_count > 0) {
        // copy all the tiles once
        for(u32 i=0; i<tile_count; ++i)
            tile_memory_4[charblock][charblock_offset + i] = tile_array[i];
    } else {
        // copy the empty tile
        tile_memory_4[charblock][charblock_offset + EMPTY_TILE_ID] = EMPTY_TILE;
    }

    if(wrap_around_mode) {
        const u32 x_to = (tilemap_size_x <= 32 ? tilemap_size_x : 32);
        const u32 y_to = (tilemap_size_y <= 32 ? tilemap_size_y : 32);
        for(u32 tmap_y = 0, tiles_y = 0, bg_y = 0;
                tmap_y < y_to; ++tmap_y, ++tiles_y, ++bg_y) {
          for(u32 tmap_x = 0, tiles_x = 0, bg_x = 0;
                  tmap_x < x_to; ++tmap_x, ++tiles_x, ++bg_x) {
            copy_tile_data(
                    tilemap.fixed_tiles,
                    screenblock, screenblock_array, charblock, charblock_offset,
                    tile_array, tilemap_size_x, tmap_x, tmap_y, 
                    tiles_x, tiles_y, bg_x, bg_y);
          }
        }
    } else {
        const u32 visible_tmap_col_from = camera_pos_tx;
        const u32 visible_tmap_row_from = camera_pos_ty;
        const u32 visible_tmap_col_to =
                visible_tmap_col_from + SCREEN_WIDTH_T - (camera_pos_ix ? 0 : 1);
        const u32 visible_tmap_row_to =
                visible_tmap_row_from + SCREEN_HEIGHT_T - (camera_pos_iy ? 0 : 1);

        for(u32 tmap_y = visible_tmap_row_from, tiles_y = 0, bg_y = 0;
                tmap_y <= visible_tmap_row_to;
                ++tmap_y, ++tiles_y, ++bg_y) {
          for(u32 tmap_x = visible_tmap_col_from, tiles_x = 0, bg_x = 0;
                  tmap_x <= visible_tmap_col_to;
                  ++tmap_x, ++tiles_x, ++bg_x) {
            copy_tile_data(
                    tilemap.fixed_tiles,
                    screenblock, screenblock_array, charblock, charblock_offset,
                    tile_array, tilemap_size_x, tmap_x, tmap_y, 
                    tiles_x, tiles_y, bg_x, bg_y);
          }
        }
    }

    // turning on the corresponding background
    *(tilemap.bg_x_offset) = tilemap.prev_bg_x_offset;
    *(tilemap.bg_y_offset) = tilemap.prev_bg_y_offset;
    *(bg_controls[bg_layer]) = bgcnt_value;

    // finally, copy the palettes
    for(u32 i=0; i<palette_count; ++i)
        bg_palette[i + palette_offset] = palette_array[i];
}


// WARNING: Make sure |dx|<=8 and |dy|<=8.
/// @brief: Shifts the tilemap camera by (dx,dy). Call this in update func.
/// @param tilemap Tilemap data structure.
/// @param dx X-shift (Make sure |dx|<=8).
/// @param dy Y-shift (Make sure |dy|<=8).
/// @note Movement is auto-confined within the tilemap.
export inline void shift_tilemap(
        TileMap& tilemap,
        i32 &dx,
        i32 &dy
        ) {
    if(dx == 0 && dy == 0)
        return; // skip

    const u32 prev_cam_pos_lx = tilemap.camera_pos_x >> 0x3;
    const u32 prev_cam_pos_ty = tilemap.camera_pos_y >> 0x3;
    const u32 prev_cam_pos_rx = (tilemap.camera_pos_x + SCREEN_WIDTH - 1) >> 0x3;
    const u32 prev_cam_pos_by = (tilemap.camera_pos_y + SCREEN_HEIGHT - 1) >> 0x3;

    // restrict the shift to the tilemap size
    if(dx < 0 && tilemap.camera_pos_x + dx < 0)
        dx = -tilemap.camera_pos_x;
    if(dy < 0 && tilemap.camera_pos_y + dy < 0)
        dy = -tilemap.camera_pos_y;
    if(dx > 0 && tilemap.camera_pos_x + dx > tilemap.tilemap_size_x*8 - SCREEN_WIDTH)
        dx = tilemap.tilemap_size_x*8 - SCREEN_WIDTH - tilemap.camera_pos_x;
    if(dy > 0 && tilemap.camera_pos_y + dy > tilemap.tilemap_size_y*8 - SCREEN_HEIGHT)
        dy = tilemap.tilemap_size_y*8 - SCREEN_HEIGHT - tilemap.camera_pos_y;

    if(dx == 0 && dy == 0)
        return; // skip
    
    tilemap.camera_pos_x += dx;
    tilemap.camera_pos_y += dy;

    const u32 cam_pos_lx = tilemap.camera_pos_x >> 0x3;
    const u32 cam_pos_ty = tilemap.camera_pos_y >> 0x3;
    const u32 cam_pos_rx = (tilemap.camera_pos_x + SCREEN_WIDTH - 1) >> 0x3;
    const u32 cam_pos_by = (tilemap.camera_pos_y + SCREEN_HEIGHT - 1) >> 0x3;

    // X-offset ---------------------------------------------------------------
    
    u32 tmap_col = ~u32(0), bg_col, tile_col;

    // first left outside
    if(cam_pos_lx > prev_cam_pos_lx) {
        tilemap.left_bg_col += 1;
        tilemap.left_tile_col += 1;
        if(tilemap.left_bg_col >= i32(BG_WIDTH))
            tilemap.left_bg_col = 0;
        if(tilemap.left_tile_col >= TILE_COL_COUNT)
            tilemap.left_tile_col = 0;
    } else { if(cam_pos_lx < prev_cam_pos_lx) {
        // force to redraw the left column
        bg_col = tilemap.left_bg_col;
        tile_col = tilemap.left_tile_col;
        tmap_col = cam_pos_lx;
        tilemap.left_bg_col -= 1;
        tilemap.left_tile_col -= 1;
        if(tilemap.left_bg_col < 0)
            tilemap.left_bg_col = i32(BG_WIDTH) - 1;
        if(tilemap.left_tile_col < 0)
            tilemap.left_tile_col = TILE_COL_COUNT - 1;
    } }

    // first right outside
    if(cam_pos_rx > prev_cam_pos_rx) {
        // force to redraw the right column
        bg_col = tilemap.right_bg_col;
        tile_col = tilemap.right_tile_col;
        tmap_col = cam_pos_rx;
        tilemap.right_bg_col += 1;
        tilemap.right_tile_col += 1;
        if(tilemap.right_bg_col >= i32(BG_WIDTH))
            tilemap.right_bg_col = 0;
        if(tilemap.right_tile_col >= TILE_COL_COUNT)
            tilemap.right_tile_col = 0;
    } else { if(cam_pos_rx < prev_cam_pos_rx) {
        tilemap.right_bg_col -= 1;
        tilemap.right_tile_col -= 1;
        if(tilemap.right_bg_col < 0)
            tilemap.right_bg_col = i32(BG_WIDTH) - 1;
        if(tilemap.right_tile_col < 0)
            tilemap.right_tile_col = TILE_COL_COUNT - 1;
    } }

    
    // Y-offset ---------------------------------------------------------------

    u32 tmap_row = ~u32(0), bg_row, tile_row;

    // first top outside row
    if(cam_pos_ty > prev_cam_pos_ty) {
        tilemap.top_bg_row += 1;
        tilemap.top_tile_row += 1;
        if(tilemap.top_bg_row >= i32(BG_HEIGHT))
            tilemap.top_bg_row = 0;
        if(tilemap.top_tile_row >= TILE_ROW_COUNT)
            tilemap.top_tile_row = 0;
    } else { if(cam_pos_ty < prev_cam_pos_ty) {
        // force to redraw the top row
        bg_row = tilemap.top_bg_row;
        tile_row = tilemap.top_tile_row;
        tmap_row = cam_pos_ty;
        tilemap.top_bg_row -= 1;
        tilemap.top_tile_row -= 1;
        if(tilemap.top_bg_row < 0)
            tilemap.top_bg_row = i32(BG_HEIGHT) - 1;
        if(tilemap.top_tile_row < 0)
            tilemap.top_tile_row = TILE_ROW_COUNT - 1;
    } }

    // first bottom outside row
    if(cam_pos_by > prev_cam_pos_by) {
        // force to redraw the bottom row
        bg_row = tilemap.bottom_bg_row;
        tile_row = tilemap.bottom_tile_row;
        tmap_row = cam_pos_by;
        tilemap.bottom_bg_row += 1;
        tilemap.bottom_tile_row += 1;
        if(tilemap.bottom_bg_row >= i32(BG_HEIGHT))
            tilemap.bottom_bg_row = 0;
        if(tilemap.bottom_tile_row >= TILE_ROW_COUNT)
            tilemap.bottom_tile_row = 0;
    } else { if(cam_pos_by < prev_cam_pos_by) {
        tilemap.bottom_bg_row -= 1;
        tilemap.bottom_tile_row -= 1;
        if(tilemap.bottom_bg_row < 0)
            tilemap.bottom_bg_row = i32(BG_HEIGHT) - 1;
        if(tilemap.bottom_tile_row < 0)
            tilemap.bottom_tile_row = TILE_ROW_COUNT - 1;
    } }

    // Redrawing --------------------------------------------------------------
    
    // redraw a column if needed
    if(tmap_col != ~u32(0)) {
        u32 tmap_y = cam_pos_ty; 
        u32 tiles_y = tilemap.top_tile_row;
        u32 bg_y = tilemap.top_bg_row;
        //const u32 cam_pos_iy = tilemap.camera_pos_y & 0x7;
        const u32 tmap_y_to = cam_pos_by;
        while(tmap_y <= tmap_y_to) {
            ++tiles_y;
            if(tiles_y >= TILE_ROW_COUNT)
                tiles_y = 0;
            ++bg_y;
            if(bg_y >= BG_HEIGHT)
                bg_y = 0;
            copy_tile_data(
                    tilemap.fixed_tiles,
                    tilemap.screenblock, tilemap.sb_array, 
                    tilemap.charblock, tilemap.charblock_offset,
                    tilemap.tile_array, tilemap.tilemap_size_x, 
                    tmap_col, tmap_y, 
                    tile_col, tiles_y, 
                    bg_col, bg_y);
            ++tmap_y;
        }
    }

    // redraw a row if needed
    if(tmap_row != ~u32(0)) {
        u32 tmap_x = cam_pos_lx; 
        u32 tiles_x = tilemap.left_tile_col;
        u32 bg_x = tilemap.left_bg_col;
        //const u32 cam_pos_ix = tilemap.camera_pos_x & 0x7;
        const u32 tmap_x_to = cam_pos_rx;
        while(tmap_x <= tmap_x_to) {
            ++tiles_x;
            if(tiles_x >= TILE_COL_COUNT)
                tiles_x = 0;
            ++bg_x;
            if(bg_x >= BG_WIDTH)
                bg_x = 0;
            copy_tile_data(
                    tilemap.fixed_tiles,
                    tilemap.screenblock, tilemap.sb_array, 
                    tilemap.charblock, tilemap.charblock_offset,
                    tilemap.tile_array, tilemap.tilemap_size_x, 
                    tmap_x, tmap_row, 
                    tiles_x, tile_row, 
                    bg_x, bg_row);
            ++tmap_x;
        }
    }

    tilemap.prev_bg_x_offset += dx;
    tilemap.prev_bg_y_offset += dy;
}

/// @brief Use this to set the BG offset for the wrap-around backgrounds.
export inline void change_tilemap_bg_offset(
        TileMap& tilemap,
        const i32 px, 
        const i32 py) {
    tilemap.prev_bg_x_offset = px;
    tilemap.prev_bg_y_offset = py;
}

export inline void set_tilemap_bg_offset(TileMap& tilemap) {
    *(tilemap.bg_x_offset) = tilemap.prev_bg_x_offset;
    *(tilemap.bg_y_offset) = tilemap.prev_bg_y_offset;
}


} // namespace gba
