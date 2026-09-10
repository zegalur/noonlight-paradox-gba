/*!****************************************************************************
 * @file action.cppm
 * @brief Stores entities internal state and sets some reaction flags.
 *******************************************************************************/        

module;
export module engine:action;

import :types;

using namespace gba;

namespace gba {
namespace comp {

/// @brief Action component data cell count.
export constexpr u32 DATA_COUNT = 8;


/// @brief Action component flags.
export enum ActionFlags {
    /// @brief Turns off the reaction to some events, like damage or dead.
    INACTIVE   = 0,
    /// @brief Allows reacting to a damage.
    ON_DAMAGE  = 0b1,
    /// @brief Allows reaction to "on dead" event.
    ON_DEAD    = 0b10,
};


/// @brief Action component that stores entities internal state and sets some 
/// reaction flags.
export struct Action {
    /// @brief Reaction flags, use values from `ActionFlags`.
    u32 flags;
    /// @brief Stores internal state or other per entity data, like type etc.
    Var data[DATA_COUNT];
};

} // namespace comp
} // namespace gba
