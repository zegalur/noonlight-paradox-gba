/*!****************************************************************************
 * @file visual.cppm
 * @brief Visual representation (as animated sprite.)
 *******************************************************************************/ 

module;
export module engine:visual;

import :types;
import :sprites;

namespace gba {
namespace comp {

/// @brief Visual component (animated sprite.)
export struct Visual {
    /// @brief Animated sprite resource.
    PoolAnimSprite sp;
    /// @brief Animation data, containing animation tile ids.
    const u16** data;
    /// @brief Visual component state (used by `sys::director` to store flags).
    u32 state;
    /// @brief Current animation id (index).
    u32 anim_id;
    /// @brief Current animation length.
    u32 anim_len;
    /// @brief Animation speed (fps). `FIX32_ONE` is equal to 1 FPS.
    fix32 anim_speed;
    /// @brief Is this a cyclic animation.
    bool loop;
    /// @brief Current frame index.
    u32 frame_id;
    /// @brief Frame timer.
    fix32 timer;
    /// @brief Previous animation ID.
    u32 prev_anim_id;
    /// @brief Previous frame ID.
    u32 prev_frame_id;
};


/// @return Returns `true` is animation is completed.
/// NOTE: When `loop` is `false`, and animation is completed, it also stopped.
export inline bool is_anim_done(const Visual& v) {
    return v.frame_id >= v.anim_len;
}


/// @brief Updated the animation component - switches frames etc.
export inline bool upd_anim(Visual& v) {
    if(!v.loop && is_anim_done(v))
        return false;
    const auto pre_frame_id = v.frame_id;
    v.timer += v.anim_speed;
    while(v.timer >= FIX32_ONE) {
        v.timer -= FIX32_ONE;
        ++v.frame_id;
    }
    if(v.loop) {
        while(v.frame_id >= v.anim_len)
            v.frame_id -= v.anim_len;
    }
    const bool frame_id_changed = pre_frame_id != v.frame_id;
    if(frame_id_changed)
        v.prev_frame_id = pre_frame_id;
    //const bool anim_id_changed = v.prev_anim_id != v.anim_id;
    //v.prev_anim_id = v.anim_id;
    return frame_id_changed/* || anim_id_changed*/;
}


/// @brief Forces to replay the current animation from a given frame.
export inline void replay_anim(
        Visual& v,
        const u32 from_frame, 
        const u32 add_state) {
    v.prev_frame_id = v.frame_id;
    v.state |= add_state;
    v.frame_id = from_frame;
}


/// @brief Forces to play an animation.
export inline void play_anim(
        Visual& v, 
        const u32 anim_id, 
        const u32 anim_len,
        const u32 from_frame, 
        const bool loop, 
        const fix32 speed,
        const u32 add_state) {
    v.prev_anim_id = v.anim_id;
    v.prev_frame_id = v.frame_id;
    v.state |= add_state;
    v.loop = loop;
    v.anim_id = anim_id;
    v.anim_len = anim_len;
    v.frame_id = from_frame;
    v.anim_speed = speed;
}

} // namespace comp
} // namespace gba
