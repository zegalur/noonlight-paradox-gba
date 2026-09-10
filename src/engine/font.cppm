/*!****************************************************************************
 * @file font.cppm
 * @brief ...
 * This module ...
 *******************************************************************************/        

module;
export module engine:font;

import :types;
import :graphics;
import :resources;

namespace gba {

/// @brief This structure holds all the information need to draw a glyph.
/// It's intended to be used together with the `ttf_to_cpp.py` script.
export struct Glyph {
    /// @brief Glyph' UTF-8 code.
    const c32 utf8_code;
    /// @brief Glyph' bitmap width.
    const u8 width;
    /// @brief Glyph' bitmap height.
    const u8 height;
    /// @brief Glyph' bitmap horizontal offset.
    const i8 left;
    /// @brief Glyph' bitmap vertical offset.
    const i8 top;
    /// @brief Pointer to the data array were glyph bitmap is stored.
    /// It is stored as a collection of `u32` numbers, each number is a 
    /// bit-row, where 0 is transparent, and 1 is non-transparent.
    const u32* const data;
};


/// @brief Use this structure to specify the font's glyphs and metrics.
export struct Font {
    /// @brief The distance between two consecutive lines of text.
    const u32 line_height;
    /// @brief Tells how many glyphs are in the `glyphs` array.
    const u32 glyph_count;
    /// @brief Glyphs array.
    const gba::Glyph (&glyphs)[];
};


/// @brief Finds a glyph inside a sorted (asc) glyph array.
/// @param utf8_code An UTF-8 code of a glyph we want to find.
/// @param glyphs An array of glyphs were the search will be performed.
/// @param glyph_count The number of glyphs in the `glyphs` array.
/// @return The index of a glyph with this `utf8_code` or `glyph_count` otherwise.
export constexpr u32 find_glyph(
        const c32 utf8_code,
        const Glyph (&glyphs)[],
        const u32 glyph_count) {
    for(u32 i=0, s=1; s!=0; ) {
        if(glyphs[i].utf8_code == utf8_code)
            return i;
        s <<= 1;
        while(i + s >= glyph_count)
            s >>= 1;
        while(glyphs[i + s].utf8_code > utf8_code)
            s >>= 1;
        i += s;
    }
    return glyph_count;
}


/// @brief This structure holds the information from a previous `draw_text` call.
/// Use this it to continue the drawing process from where it was halted from 
/// the last call.
/// @see draw_text()
export struct DrawText_State {
    /// @brief Current font color.
    u8 cur_color;
    /// @brief Current cursor X-position (inside the tile grid).
    i32 cursor_x;
    /// @brief Current cursor Y-position (inside the tile grid).
    i32 cursor_y;
    /// @brief The next symbol that will be processed.
    i32 str_indx;
};


/// @brief Resets the string cursor, but NOT the screen cursor.
export void reset_string(DrawText_State& state) {
    state.str_indx = 0;
}


/// @brief Draws a text into a tile grid.
/// This function does not clean up the tiles.
/// @see DrawText_State
/// @param reset When `true` resets the state and start drawing from scratch.
/// @param state The state from a previous call.
/// @param str String that will be drawn.
/// @param font The font that will be used.
/// @param tile_grid The tile grid where the drawing will be performed.
/// @param pos_x The starting X-position (inside the tile grid).
/// @param pos_y The starting Y-position (inside the tile grid).
/// @return Return `true` when done. Otherwise, returns `false`. Use the
///         modified `state` variable to continue the drawing in the next call.
export
template<int W, int H>
bool draw_text(
        const bool reset,
        DrawText_State& state,
        const c32* str,
        const Font& font,
        TileGrid<W,H>& tile_grid,
        const i32 pos_x,
        const i32 pos_y) {
    if(reset) {
        state.cur_color = 1;
        state.cursor_x = pos_x;
        state.cursor_y = pos_y;
        state.str_indx = 0;
    }

    const c32 ch = str[state.str_indx];
    ++state.str_indx;
    
    switch(ch) {
        case U'\0':
            return true;
        case U'\n':
            state.cursor_x = pos_x;
            state.cursor_y += font.line_height;
            return draw_text(false, state, str, font, tile_grid, pos_x, pos_y);
        case U'^':
            if(str[state.str_indx] != U'\0') {
              if(str[state.str_indx] == U'#') {
                // half height enter
                state.cursor_x = pos_x;
                state.cursor_y += font.line_height >> 1;
                ++state.str_indx;
                return draw_text(false, state, str, font, tile_grid, pos_x, pos_y);
              }
              if(str[state.str_indx] == U'-') {
                // shift left by 1 pixel
                state.cursor_x -= 1;
                ++state.str_indx;
                return draw_text(false, state, str, font, tile_grid, pos_x, pos_y);
              }
              if(str[state.str_indx] == U'|') {
                // shift down by 1 pixel
                state.cursor_y += 1;
                ++state.str_indx;
                return draw_text(false, state, str, font, tile_grid, pos_x, pos_y);
              }
              if(str[state.str_indx] >= U'0')
                if(str[state.str_indx] <= U'9') {
                    state.cur_color = str[state.str_indx] - U'0';
                    ++state.str_indx;
                }
            }
            return draw_text(false, state, str, font, tile_grid, pos_x, pos_y);
    }
    
    // find the corresponding glyph
    const auto glyph_indx = find_glyph(ch, font.glyphs, font.glyph_count);
    if(glyph_indx == font.glyph_count) {
        ++state.str_indx;
        // can't find the glyph, skip and return
        return false;
    }
    const Glyph& glyph = font.glyphs[glyph_indx];
    const i32 draw_from_x = state.cursor_x + i32(glyph.left);
    const i32 draw_from_y = state.cursor_y + i32(glyph.top);
    state.cursor_x += glyph.width;

    // check if glyph is visible or fully clipped
    if(draw_from_x >= (tile_grid.width << 3))
        return false; // fully clipped, skip
    if(draw_from_y >= (tile_grid.height << 3))
        return false; // fully clipped, skip
    if((draw_from_x + i32(glyph.width) - 1) < 0)
        return false; // fully clipped, skip
    if((draw_from_y + i32(glyph.height) - 1) < 0)
        return false; // fully clipped, skip

    for(i32 y = 0; y < glyph.height; ++y) {
        const i32 Y = draw_from_y + y;
        if(Y < 0) // TODO: eliminate
            continue; // clipped
        if(Y >= (tile_grid.height << 3))
            break; // clipped
        const i32 Y8 = Y & 0x7;
        u32 mask = glyph.data[y];
        for(i32 x = 0; mask; mask >>= u32(1), ++x) {
            if(mask & u32(1)) {
                const i32 X = draw_from_x + x;
                if(X < 0) // TODO: eliminate
                    continue; // clipped
                if(X >= (tile_grid.width << 3))
                    break; // clipped
                const i32 X8 = X & 0x7;
                Tile_4& tile = *(tile_grid.cells[Y >> 3][X >> 3]);
                tile.data[Y8] &= ~(u32(0xF) << u32(4*X8));
                tile.data[Y8] |= state.cur_color << u32(4*X8);
            }
        }
    }

    return false;
}

} // namespace gba
