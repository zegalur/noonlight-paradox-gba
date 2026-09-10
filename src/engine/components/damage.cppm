/*!****************************************************************************
 * @file damage.cppm
 * @brief Damage component. Stores damage mask etc.
 *******************************************************************************/        

module;
export module engine:damage;

import :types;

using namespace gba;

namespace gba {
namespace comp {

/// @brief Entity immune to any damage.
export constexpr u32 NO_DAMAGE = u32(0);

/// @brief Entity takes any type of damage.
export constexpr u32 ANY_DAMAGE = ~NO_DAMAGE;


/// @brief Damage component. Stores damage mask etc.
export struct Damage {
    /// @brief Damage masks. Sets what kind of damage this entity takes.
    u32 damage_mask;
};

} // namespace comp
} // namespace gba
