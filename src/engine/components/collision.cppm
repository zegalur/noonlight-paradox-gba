/*!****************************************************************************
 * @file collision.cppm
 * @brief Collision component - contains collision flags and shape info.
 *******************************************************************************/        

module;
export module engine:comp_collision;

import :types;

namespace gba {
namespace comp {

/// @brief Collision body type.
export enum BodyType {
    /// @brief Use AABB, derived from spatial component and `padding` field.
    AABB,
};


/// @brief Collision component, contains collision flags and shape info.
export struct Collision {
    /// @brief Collision body type.
    BodyType body_type;
    /// @brief Collision mask.
    u32 mask;
    /// @brief Shape padding. 
    /// NOTE: Negative values make shape smaller, positive - bigger.
    i32 padding;
};

} // namespace comp
} // namespace gba

