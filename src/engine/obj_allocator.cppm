/*!****************************************************************************
 * @file obj_allocator.cppm
 * @brief Fast sprite, object tile and object palette allocators. 
 * Supports object pool and stack allocations. Overview:
 *      +---+---+---+   
 *      | B | ->|   |   <- pool blocks (starts from the first tile/palette)
 *      +---+---+---+   
 *      |   |   |   |
 *      +---+---+---+
 *      |<- | S | S |   <- stack (starts from the last tile/palette)
 *      +---+---+---+
 * WARNING: Does not automatically checks if stack overlaps with the pool nor 
 *          if pool or stack overflows.
 *******************************************************************************/

module;
export module engine:obj_allocator;

import :types;
import :graphics;

namespace gba {

namespace {
    /// @brief The number of tiles in a tile block.
    constexpr u32 TILE_BLOCK_SIZE = 32;
    /// @brief The log2 from the number of tiles in a tile block.
    constexpr u32 TILE_BLOCK_SIZE_LOG2 = 5;
    /// @brief The number of tile blocks.
    constexpr u32 TILE_BLOCK_COUNT = 32;
    /// @brief The number of object tiles overall.
    constexpr u32 TILE_COUNT = TILE_BLOCK_SIZE * TILE_BLOCK_COUNT;

    /// @brief The number of sprites in a sprite block.
    constexpr u32 SPRITE_BLOCK_SIZE = 16;
    /// @brief The log2 from number of sprites in a sprite block.
    constexpr u32 SPRITE_BLOCK_SIZE_LOG2 = 4;
    /// @brief The number of sprite blocks.
    constexpr u32 SPRITE_BLOCK_COUNT = 8;
    /// @brief The number of sprites.
    constexpr u32 SPRITE_COUNT = SPRITE_BLOCK_SIZE * SPRITE_BLOCK_COUNT;
}


// ---------------------------- Stack Allocator ---------------------------- //

namespace {

/// @brief The index of the first free palette on top of the stack.
u32 obj_palette_stack_head = PALETTE_COUNT;

/// @brief How many stack tiles have been allocated so far.
u32 obj_tile_stack_head = TILE_COUNT;

/// @brief How many sprites have been allocated so far.
u32 sprite_stack_head = SPRITE_COUNT;

}


// TODO: replace
/// @brief: Finds the first set bit of a 32bit int.
/// @return: Returns bit position + 1 when found, 0 otherwise.
inline u32 ffs(u32 x) { 
    return __builtin_ffs(x);
}

// TODO: replace
/// @brief: Counts the number of consecutive zero bit from the least significant.
/// @return: Returns the number of consecutive zero bits.
inline u32 ctz(u32 x) {
    return __builtin_ctz(x);
}

/// @brief Allocates object palette(s) on the stack.
/// @param palette_count The number of palettes to allocate.
/// @return Returns the index of the first allocated palette.
export u32 stack_alloc_obj_palettes(const u32 palette_count) {
    obj_palette_stack_head -= palette_count;
    return obj_palette_stack_head;
}

/// @brief Pops the object palette(s) from the stack.
/// @param palette_count The number of palettes to deallocate.
export void stack_free_obj_palettes(const u32 palette_count) {
    obj_palette_stack_head += palette_count;
}


/// @brief Allocates consecutive object tiles on the stack.
/// @param tile_count The number of tiles to allocate.
/// @return Returns the first allocated tile index.
export u32 stack_alloc_obj_tiles(const u32 tile_count) {
    obj_tile_stack_head -= tile_count;
    return obj_tile_stack_head;
}

/// @brief Pops the topmost object tile from the stack.
/// @param tile_count The number of tiles to deallocate.
export void stack_free_obj_tiles(const u32 tile_count) {
    obj_tile_stack_head += tile_count;
}


/// @brief Allocates consecutive sprites on the stack.
/// @param sprite_count The number of sprites to allocate.
/// @return Returns the first allocated sprite.
export u32 stack_alloc_sprites(const u32 sprite_count) {
    sprite_stack_head -= sprite_count;
    return sprite_stack_head;
}

/// @brief Pops the topmost sprites from the stack.
/// @param sprite_count The number of sprites to deallocate.
export void stack_free_sprites(const u32 sprite_count) {
    sprite_stack_head += sprite_count;
}


// ---------------------------- Pool Allocator ----------------------------- //

namespace {

/// @brief Bitmap that marks what palettes are free (1 - free).
u32 obj_pal_bitmap = (u32(1) << PALETTE_COUNT) - u32(1);


/// @brief Bitmap that marks what tile blocks are free (1 - free).
u32 obj_tblocks_bitmap = ~u32(0);

/// @brief Each number is a block bitmap that marks what tiles are free (1).

u32 obj_tblock_bitmap[TILE_BLOCK_COUNT] = {
    ~u32(0), ~u32(0), ~u32(0), ~u32(0), ~u32(0), ~u32(0), ~u32(0), ~u32(0),
    ~u32(0), ~u32(0), ~u32(0), ~u32(0), ~u32(0), ~u32(0), ~u32(0), ~u32(0),
    ~u32(0), ~u32(0), ~u32(0), ~u32(0), ~u32(0), ~u32(0), ~u32(0), ~u32(0),
    ~u32(0), ~u32(0), ~u32(0), ~u32(0), ~u32(0), ~u32(0), ~u32(0), ~u32(0) };


/// @brief Bitmap that marks what sprite blocks are free (1 - free).
u32 sprite_blocks_bitmap = (u32(1) << SPRITE_BLOCK_COUNT) - u32(1);

constexpr u32 SB_FREE = (u32(1) << SPRITE_BLOCK_SIZE) - u32(1);

/// @brief Each number is a block bitmap that marks what tiles are free (1).
u32 sblock_bitmap[SPRITE_BLOCK_COUNT] = {
    SB_FREE, SB_FREE, SB_FREE, SB_FREE, SB_FREE, SB_FREE, SB_FREE, SB_FREE };

}

/// @brief Allocates object palette(s) from the pool.
/// @param palette_count The number of palettes to allocate.
/// @return Returns the index of the first allocated palette.
export u32 alloc_obj_palettes(const u32 palette_count) {
    u32 bitmap = obj_pal_bitmap;
    u32 r = 0;
    while(bitmap) {
        const u32 r1 = ffs(bitmap) - u32(1); // find first set
        bitmap >>= r1;
        r += r1;
        const u32 z = ctz(~bitmap); // count ones
        if(z >= palette_count) {
            // we found a free interval with the proper length
            obj_pal_bitmap &= ~(((u32(1) << palette_count) - u32(1)) << r);
            return r;
        }
        bitmap >>= z;
        r += z;
    }
    // Oops! No free palettes left. 
    // Hopefully this will never happen.
    // TODO: error handling here
    return 0;
}

/// @brief Deallocates object palette(s).
/// @param palette_indx The index of the first palette to deallocate.
/// @param palette_count The number of palettes to deallocate.
export void free_obj_palettes(
        const u32 palette_indx,
        const u32 palette_count) {
    obj_pal_bitmap |= (((u32(1) << palette_count) - u32(1)) << palette_indx);
}


/// @brief Allocates from the pool (general case).
/// @param count The number of units to allocate.
/// @param unit_blocks_bitmap Blocks bitmap.
/// @param block_bitmap_array Block bitmaps array.
/// @param block_size The number of units in a block.
/// @return Returns the index of the first allocated unit.
export u32 alloc_units(
        const u32 count,
        u32& unit_blocks_bitmap,
        u32* const block_bitmap_array,
        const u32 block_size
        ) {
    u32 blocks_bitmap = unit_blocks_bitmap;
    u32 block_id = 0;
    while(blocks_bitmap) {
        // find the next block with some empty cells
        const u32 br = ffs(blocks_bitmap) - u32(1); // find first set
        block_id += br;
        blocks_bitmap >>= br;
        // check if this block has enough free consecutive tiles
        u32 bitmap = block_bitmap_array[block_id];
        u32 r = 0;
        while(bitmap) {
            u32 r1 = ffs(bitmap) - u32(1); // find first set
            bitmap >>= r1;
            r += r1;
            u32 z = ctz(~bitmap); // count ones
            if(z >= count) {
                // we found a free interval with a proper length
                block_bitmap_array[block_id] &=
                        ~(((u32(1) << count) - u32(1)) << r);
                if(block_bitmap_array[block_id] == u32(0)) {
                    // no free space left in this block
                    unit_blocks_bitmap &= ~(u32(1) << block_id);
                }
                return block_id * block_size + r;
            }
            bitmap >>= z;
            r += z;
        }
        // not enough space, skip this one
        block_id += 1;
        blocks_bitmap >>= 1;
    }
    // Oops! No free units left. 
    // Hopefully this will never happen.
    // TODO: error handling here
    return 0;
}

/// @brief Deallocates unit(s) back to pool (general case).
/// @param unit_indx The index of the first unit to deallocate.
/// @param unit_count The number of units to allocate.
/// @param unit_blocks_bitmap Blocks bitmap.
/// @param block_bitmap_array Block bitmaps array.
/// @param block_size_log2 =log2(The number of units in a block).
export void free_units(
        const u32 unit_indx,
        const u32 unit_count,
        u32& unit_blocks_bitmap,
        u32* const block_bitmap_array,
        const u32 block_size_log2
        ) {
    const u32 block_id = unit_indx >> u32(block_size_log2);
    const u32 uid = unit_indx & ((u32(1) << block_size_log2) - u32(1));
    unit_blocks_bitmap |= u32(1) << block_id;
    block_bitmap_array[block_id] |= ((u32(1) << unit_count) - u32(1)) << uid;
}


// Tiles

/// @brief Allocates object tiles(s) from the tile pool.
/// @param tile_count The number of tiles to allocate.
/// @return Returns the index of the first allocated object tile.
export u32 alloc_obj_tiles(const u32 tile_count) {
    if(tile_count == 64) {
        // TODO: optimize
        // exceptional case for large 64x64 sprites
        for(u32 i=0; i<TILE_BLOCK_COUNT-1; ++i) {
            if(obj_tblock_bitmap[i] == ~u32(0)) 
              if(obj_tblock_bitmap[i+1] == ~u32(0)) {
                  obj_tblock_bitmap[i] = obj_tblock_bitmap[i+1] = u32(0);
                  obj_tblocks_bitmap &= ~(u32(0b11) << i);
                  return i*TILE_BLOCK_SIZE;
              }
        }
        // TODO: Oops! Error, not enough space...
        return 0;
    }
    return alloc_units(
            tile_count,
            obj_tblocks_bitmap, obj_tblock_bitmap,
            TILE_BLOCK_SIZE);
}

/// @brief Deallocates object tile(s).
/// @param tile_indx The index of the first tile to deallocate.
/// @param tile_count The number of tiles to deallocate.
export void free_obj_tiles(
        const u32 tile_indx,
        const u32 tile_count) {
    if(tile_count == 64) {
        // exceptional case for large 64x64 sprites
        const u32 block_id = tile_indx >> u32(TILE_BLOCK_SIZE_LOG2);
        obj_tblock_bitmap[block_id] = obj_tblock_bitmap[block_id+1] = ~u32(0);
        obj_tblocks_bitmap |= (u32(0b11) << block_id);
        return;
    }
    free_units(
            tile_indx, tile_count,
            obj_tblocks_bitmap, obj_tblock_bitmap,
            TILE_BLOCK_SIZE_LOG2);
}


// Primitive Sprites

/// @brief Allocates primitive sprites(s) from the sprite pool.
/// @param sprite_count The number of sprites to allocate.
/// @return Returns the index of the first allocated sprite.
export u32 alloc_sprites(const u32 sprite_count) {
    return alloc_units(
            sprite_count,
            sprite_blocks_bitmap, sblock_bitmap,
            SPRITE_BLOCK_SIZE);
}

/// @brief Deallocates sprite(s).
/// @param sprite_indx The index of the first sprite to deallocate.
/// @param sprite_count The number of sprites to deallocate.
export void free_sprites(
        const u32 sprite_indx,
        const u32 sprite_count) {
    free_units(
            sprite_indx, sprite_count,
            sprite_blocks_bitmap, sblock_bitmap,
            SPRITE_BLOCK_SIZE_LOG2);
}

} // namespace gba
