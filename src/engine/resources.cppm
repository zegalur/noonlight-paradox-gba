/*!****************************************************************************
 * @file resources.cppm
 * @brief Managing shared resources, like palettes etc. Also, `TileGrid`.
 *******************************************************************************/

module;
export module engine:resources;

import :types;
import :graphics;
import :obj_allocator;

namespace gba {

// ------------------------------- TileGrid -------------------------------- //

export template<int W, int H> 
struct TileGrid {
    static constexpr i32 width = W; // in tiles
    static constexpr i32 height = H; // in tiles
    Tile_4* cells[H][W];
};


export template<int W, int H> inline void set_tile_grid(
        TileGrid<W,H> &tgrid, const u32 charblock, const u32 tx, const u32 ty) {
    for(i32 y=0; y<H; ++y)
        for(i32 x=0; x<W; ++x)
            tgrid.cells[y][x] = &tile_memory_4[charblock][32*(y+ty) + (x+tx)];
}


export template<int W, int H> inline void clear_tile_grid(TileGrid<W,H> &tgrid) {
    for(i32 y=0; y<H; ++y)
        for(i32 x=0; x<W; ++x)
            *(tgrid.cells[y][x]) = EMPTY_TILE;
}


// --------------------------- Shared Resources ---------------------------- //

export template<typename T> struct Shared {
    u32 in_use;
    u32 id;
    const u32 count;
    const T resource;
    Shared(const T&& resource_, const u32 count_)
            : in_use(0)
            , count(count_)
            , resource(resource_)
    { /* do nothing */ }
};

export template<typename T> inline void use(Shared<T>& shared) {
    if(shared.in_use == 0)
        shared.id = alloc_func(shared.resource, shared.count);
    ++shared.in_use;
}

export template<typename T> inline void free(Shared<T>& shared) {
    --shared.in_use;
    if(shared.in_use == 0)
        free_func(shared.resource, shared.id, shared.count);
}


// ----------------------- Object Palette Resources ------------------------ //

export struct Pool_ObjPalette { const Palette* const data; };
export struct Stack_ObjPalette { const Palette* const data; };

export u32 alloc_func(const Pool_ObjPalette& p, const u32 count) {
    const u32 id = alloc_obj_palettes(count);
    for(u32 i = 0; i < count; ++i)
        obj_palette[id + i] = p.data[i];
    return id;
}
export u32 alloc_func(const Stack_ObjPalette& p, const u32 count) {
    const u32 id = stack_alloc_obj_palettes(count);
    for(u32 i = 0; i < count; ++i)
        obj_palette[id + i] = p.data[i];
    return id;
}

export void free_func(const Pool_ObjPalette&, const u32 id, const u32 count) {
    free_obj_palettes(id, count);
}
export void free_func(const Stack_ObjPalette&, const u32, const u32 count) {
    stack_free_obj_palettes(count);
}


// ------------------------- Object Tile Resources ------------------------- //

export struct Pool_ObjTile_4 { const Tile_4* const data; };
export struct Stack_ObjTile_4 { const Tile_4* const data; };
export struct Pool_ObjTile_4_Placeholder { const Tile_4* const data; };
export struct Stack_ObjTile_4_Placeholder { const Tile_4* const data; };

export u32 alloc_func(const Pool_ObjTile_4& p, const u32 count) {
    const u32 id = alloc_obj_tiles(count);
    for(u32 i = 0; i < count; ++i)
        tile_memory_4[4][id + i] = p.data[i];
    return id;
}
export u32 alloc_func(const Stack_ObjTile_4& p, const u32 count) {
    const u32 id = stack_alloc_obj_tiles(count);
    for(u32 i = 0; i < count; ++i)
        tile_memory_4[4][id + i] = p.data[i];
    return id;
}
export u32 alloc_func(const Pool_ObjTile_4_Placeholder&, const u32 count) {
    return alloc_obj_tiles(count);
}
export u32 alloc_func(const Stack_ObjTile_4_Placeholder&, const u32 count) {
    return stack_alloc_obj_tiles(count);
}

export void free_func(const Pool_ObjTile_4&, const u32 id, const u32 count) {
    free_obj_tiles(id, count);
}
export void free_func(const Stack_ObjTile_4&, const u32, const u32 count) {
    stack_free_obj_tiles(count);
}
export void free_func(
        const Pool_ObjTile_4_Placeholder&, const u32 id, const u32 count) {
    free_obj_tiles(id, count);
}
export void free_func(
        const Stack_ObjTile_4_Placeholder&, const u32, const u32 count) {
    stack_free_obj_tiles(count);
}


// ------------------------ Object Sprite Resources ------------------------ //

export struct Pool_SpriteRes { };
export struct Stack_SpriteRes { };

export u32 alloc_func(const Pool_SpriteRes&, const u32 count) {
    const u32 id = alloc_sprites(count);
    return id;
}
export u32 alloc_func(const Stack_SpriteRes&, const u32 count) {
    const u32 id = stack_alloc_sprites(count);
    return id;
}

export void free_func(const Pool_SpriteRes&, const u32 id, const u32 count) {
    free_sprites(id, count);
}
export void free_func(const Stack_SpriteRes&, const u32, const u32 count) {
    stack_free_sprites(count);
}

} // namespace gba
