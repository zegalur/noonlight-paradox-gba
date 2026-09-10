/*!****************************************************************************
 * @file input.cppm
 * @brief Low level GBA input API.
 *******************************************************************************/        

module;
export module engine:input;

import :types;
import :memory;

#include "macro.hpp"

namespace gba {

// keypad masks

export enum class Key : u32 {
    A = 0x0001,
    B = 0x0002,
    SELECT = 0x0004,
    START = 0x0008,
    RIGHT = 0x0010,
    LEFT = 0x0020,
    UP = 0x0040,
    DOWN = 0x0080,
    R = 0x0100,
    L = 0x0200,
    ALL = 0x03FF,
};

export DEFINE_ENUM_OR(Key, u32);


// real-time keypad handling

export constexpr auto is_key_down_now(const Key key_mask) {
    return (~(REG_KEYINPUT) & static_cast<u32>(key_mask));
}


// keypad interrupt

export enum class KeyOp : u32 {
    KCNT_OR = 0x0000,
    KCNT_AND = 0x8000,
};

export constexpr u32 KCNT_IRQ = 0x4000;

export inline void disable_keypad_interrupt() {
    REG_KEYCNT = 0;
}

export inline void enable_keypad_interrupt(const KeyOp op, const Key key_mask) {
    REG_KEYCNT = KCNT_IRQ | static_cast<u32>(op) | static_cast<u32>(key_mask);
}


// better keypad handling

namespace {
    u32 key_prev = 0;
    u32 key_cur = 0;
}

// call this once per frame
export void update_keypad() {
    key_prev = key_cur;
    key_cur = is_key_down_now(Key::ALL);
}

export u32 is_key_down(const Key key_mask) { 
    return key_cur & static_cast<u32>(key_mask);
}
export u32 is_key_up(const Key key_mask) {
    return ~key_cur & static_cast<u32>(key_mask);
}

export u32 was_key_down(const Key key_mask) {
    return key_prev & static_cast<u32>(key_mask);
}
export u32 was_key_up(const Key key_mask) {
    return ~key_prev & static_cast<u32>(key_mask);
}

export u32 is_key_transit(const Key key_mask) {
    return (key_cur ^ key_prev) & static_cast<u32>(key_mask);
}
export u32 is_key_held(const Key key_mask) {
    return (key_cur & key_prev) & static_cast<u32>(key_mask);
}
export u32 is_key_hit(const Key key_mask) {
    return (key_cur & ~key_prev) & static_cast<u32>(key_mask);
}
export u32 is_key_released(const Key key_mask) {
    return (~key_cur & key_prev) & static_cast<u32>(key_mask);
}

} // namespace gba
