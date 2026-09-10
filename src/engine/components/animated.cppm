/*!****************************************************************************
 * @file animated.cppm
 * @brief Holds the animation data for the animator system.
 * WARNING: Under construction!
 *******************************************************************************/        

module;
export module engine:animated;

import :types;

namespace gba {
namespace comp {

export struct Animated {
    u32 anim_id;
    u32 key_id;
    bool loop;
    fix32 timer;
    fix32 delta_px;
    fix32 delta_py;
};

} // namespace comp
} // namespace gba

