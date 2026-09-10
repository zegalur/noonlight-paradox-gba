/*!****************************************************************************
 * @file fonts.cppm
 * @brief This module specifies the standard in-game fonts.
 *******************************************************************************/        

module;
export module game.fonts;

import engine;
import data.cairopixel;
import data.nosutaru;

using namespace gba;

namespace game {
namespace fonts {


/// @brief Standard Japanese font.
export const Font nosutaru = { 
        data::nosutaru_size_px + 1, 
        data::nosutaru_glyph_count, 
        data::nosutaru_glyphs };


/// @brief Standard "European" font.
export const Font cairopixel = { 
        10, // we'll use a smaller line height for tighter text
        data::cairopixel_glyph_count, 
        data::cairopixel_glyphs };


} // namespace fonts
} // namespace game
