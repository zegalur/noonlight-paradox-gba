/*!****************************************************************************
 * @file animator.cppm
 * @brief Orchestrates more complex animations.
 * WARNING: Under construction!
 *******************************************************************************/        

module;
export module engine:animator;

import :animated;
import :visual;

namespace gba {
namespace sys {

namespace animator {

export consteval u32 VIS_ANIM(
    const u32 /*anim_id*/,
    const u32 /*frame_id*/,
    const bool /*loop*/
    ) {
    return 0; // TODO: ...
}

export struct Key {
    const fix32 len;
    const fix32 move_x;
    const fix32 move_y;
    const u32 vis_anim_data;
};

export struct Animation {
    const u32 key_count;
    const Key* const keys;
};

export void update_all(
        const u32 /*e_count*/,
        gba::comp::Visual* const /*visual*/,
        gba::comp::Animated* const /*animated*/) {
    // ...
}

} // animator

} // namespace sys
} // namespace gba
