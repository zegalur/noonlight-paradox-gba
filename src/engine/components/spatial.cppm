/*!****************************************************************************
 * @file spatial.cppm
 * @brief Spatial component, contains the position and size information.
 *******************************************************************************/        

module;
export module engine:spatial;

import :types;

namespace gba {
namespace comp {

/// @brief Spatial component, contains the position and size information.
export struct Spatial {
    /// @brief Global X-position in pixels (fixed point).
    fix32 px;
    /// @brief Global Y-position in pixels (fixed point).
    fix32 py;
    /// @brief X-size in pixels (integer).
    i32 sx;
    /// @brief Y-size in pixels (integer).
    i32 sy;
    /// @brief Is it visible. `true` = visible.
    bool visible;
};


/// @brief Returns `true` if (rounded-to-int) spatial contains a point.
/// @param sp Spatial component.
/// @param px Point global X coordinate (integer).
/// @param px Point global Y coordinate (integer).
/// @return Returns `true` when spatial contains this point, `false` otherwise.
export inline bool contains(
        const Spatial& sp, const i32 px, const i32 py) {
    if(px < intp(sp.px)) return false;
    if(py < intp(sp.py)) return false;
    if(px > intp(sp.px) + sp.sx) return false;
    if(py > intp(sp.py) + sp.sy) return false;
    return true;
}


/// @brief Checks if two AABBs devived from spatials and padding, intersect.
/// @param sp1 First spatial.
/// @param sp2 Second spatial.
/// @param padding_1 First padding.
/// @param padding_2 Second padding.
/// @return Returns `true` when two spatial AABBs intersects.
export inline bool intersect_aabb(
        const Spatial& sp1, const Spatial& sp2,
        const i32 padding_1, const i32 padding_2) {
    const i32 distance_x = gba::abs(
            (intp(sp1.px) + (sp1.sx >> i32(1)) + padding_1) - 
            (intp(sp2.px) + (sp2.sx >> i32(1)) + padding_2));
    if((distance_x << i32(1)) 
            > (sp1.sx + sp2.sx + ((padding_1 + padding_2) << i32(1))))
        return false;
    const i32 distance_y = gba::abs(
            (intp(sp1.py) + (sp1.sy >> i32(1)) + padding_1) - 
            (intp(sp2.py) + (sp2.sy >> i32(1))) + padding_2);
    if((distance_y << i32(1)) 
            > (sp1.sy + sp2.sy + ((padding_1 + padding_2) << i32(1))))
        return false;
    return true;
}

} // namespace comp
} // namespace gba

