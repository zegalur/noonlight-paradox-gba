/*!****************************************************************************
 * @file options.cppm
 * @brief This module describes the game global options.
 *******************************************************************************/        

module;
export module game.options;

import engine;

using namespace gba;

namespace game {

// -------------------------------- Mem Bank ------------------------------- //

export constexpr u32 MEMORY_BANK_SIZE_KB = u32(25); // 25
export constexpr u32 MEMORY_BANK_SIZE = MEMORY_BANK_SIZE_KB << u32(10);


// --------------------------------- Speed --------------------------------- //

// standard text speed
export constexpr fix32 MEDIUM = FIX32_ONE >> 2;
export constexpr fix32 SLOW = FIX32_ONE >> 3;
export constexpr fix32 SLOWEST = FIX32_ONE >> 5;


// ------------------------------- Projection ------------------------------ //

// @brief Oblique projection delta X correction.
export constexpr i32 PROJ_DX = 8;

// @brief Oblique projection delta Y correction.
export constexpr i32 PROJ_DY = -4;


// -------------------------------- Controls ------------------------------- //

/// @brief Select/activate an option in a menu.
export constexpr Key KEY_SELECT = Key::A;

/// @brief Jump up / Long jump.
export constexpr Key KEY_JUMP = Key::A;

/// @brief Shooting mode key.
export constexpr Key KEY_SHOOTING_MODE = Key::R;

/// @brief Shoot in the shooting mode.
export constexpr Key KEY_SHOOT = Key::A;

/// @brief Interactive zone activation.
export constexpr Key KEY_USE_IZONE = Key::B;


// --------------------------------- Stats --------------------------------- //

/// @brief The maximum amount of health bars player can have.
export constexpr u32 MAX_HBAR_COUNT = 4;

/// @brief The maximum amount of gun bars (for gui).
export constexpr u32 MAX_GBAR_COUNT = 4;

/// @brief Pistol reload speed
/// TODO: level-ups for this parameter
export constexpr u32 PISTOL_RELOAD_SPEED = 60;

/// @brief Green battery shield replenish amount.
export constexpr u32 GREEN_BATTERY_REPLENISH_AMOUNT = 3;

/// @brief Mine type 1 damage amount
export constexpr u32 MINE_01_DAMAGE_FULL = 2;
export constexpr u32 MINE_01_DAMAGE_MIN  = 1;


// --------------------------------- Masks --------------------------------- //

export enum DamageType {
    BULLET = 0b1,
};

export constexpr u32 PISTOL_DAMAGE_TYPE = DamageType::BULLET;
export constexpr u32 PISTOL_DAMAGE_AMOUNT = 3;

} // namespace game
