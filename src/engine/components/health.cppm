/*!****************************************************************************
 * @file health.cppm
 * @brief Health component, stores health related information.
 *******************************************************************************/        

module;
export module engine:health;

import :types;

namespace gba {
namespace comp {

/// @brief Health component, stores health related information.
export struct Health {
    /// @brief Current health. Set this to 0 if you want corresponding entity 
    /// receive on_dead reaction.
    u32 health;
    /// @brief Maximum health.
    u32 max_health;
    /// @brief Is it dead. `true` = dead, `false` = alive.
    bool is_dead;
};

} // namespace comp
} // namespace gba
