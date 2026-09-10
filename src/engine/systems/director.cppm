/*!****************************************************************************
 * @file director.cppm
 * @brief Director is responsible for orchestrating sprite resources attached
 *        to visual components.
 *******************************************************************************/        

module;
export module engine:director;

import :video;
import :spatial;
import :visual;

namespace gba {
namespace sys {

namespace director {

/// @brief Visual component state flags (for the director.)
export enum VisualState {
    NONE = 0,
    /// @brief Disable sprite resource on the next `director::update_all()` call.
    QUEUE_DISABLE       = 0b00000001,
    /// @brief Enable sprite resource on the next `director::update_all()` call.
    QUEUE_ENABLE        = 0b00000010,
    /// @brief Update sprite position attr on the next `director::update_all()` call.
    QUEUE_UPD_POS       = 0b00000100,
    /// @brief Update sprite frame pixels on the next `director::update_all()` call.
    QUEUE_UPD_FRAME     = 0b00001000,
};


inline bool upd_vis_sp_mode(comp::Visual& v, const SpriteMode mode) {
    if((v.sp.obj_attr.attr_0 & u16(SpriteMode::Mask)) == mode)
        return false;
    set_obj_attr_mode(v.sp.obj_attr, mode);
    return true;
}

inline void hide_visual(comp::Visual& v) {
    if(upd_vis_sp_mode(v, SpriteMode::Disable))
        v.state |= VisualState::QUEUE_DISABLE;
}

inline void show_visual(comp::Visual& v) {
    if(upd_vis_sp_mode(v, SpriteMode::Normal))
        v.state |= VisualState::QUEUE_ENABLE;
}


/// @brief Update sprite resources attached to visual components. 
///        Doesn't require vblank.
/// @param proj_dx Projection X shift (int, in pixels).
/// @param proj_dy Projection Y shift (int, in pixels).
/// @param cam_pos_x Camera X position (int, in pixels).
/// @param cam_pos_y Camera Y position (int, in pixels).
/// @param entity_count Entity count
/// @param spatial Spatial components array.
/// @param visual Corresponding visual components array.
export void update_all(
        const i32 proj_dx,
        const i32 proj_dy,
        const i32 cam_pos_x,
        const i32 cam_pos_y,
        const u32 entity_count,
        const comp::Spatial* spatial,
        comp::Visual* visual) {
    for(u32 i = 0; i < entity_count; ++i) {
        const comp::Spatial &si = spatial[i];
        comp::Visual &vi = visual[i];
        if(si.visible == false) {
            hide_visual(vi);
            continue;
        }
        i32 loc_px = intp(si.px) - cam_pos_x + proj_dx;
        i32 loc_py = intp(si.py) - cam_pos_y + proj_dy;
        if((loc_px > -si.sx) && (loc_py > -si.sy)
                && (loc_px < i32(SCREEN_WIDTH))
                && (loc_py < i32(SCREEN_HEIGHT))) {
            // update the position
            if(loc_px < 0) loc_px += i32(512);
            if(loc_py < 0) loc_py += i32(256);
            set_obj_attr_px(vi.sp.obj_attr, u16(loc_px));
            set_obj_attr_py(vi.sp.obj_attr, u16(loc_py));
            vi.state |= VisualState::QUEUE_UPD_POS;
            // force to show the visuals
            show_visual(vi);
            // update the animation
            if(comp::upd_anim(vi))
                vi.state |= VisualState::QUEUE_UPD_FRAME;
        } else {
            // sprite is clipped
            hide_visual(vi);
        }
    }
}


/// @brief Updates VRAM data of the attached sprite resources.
/// WARNING: Call this during vblank.
/// @param entity_count Entity count
/// @param visual Corresponding visual components array.
export void redraw_all(
        const u32 entity_count,
        comp::Visual* visual) {
    for(u32 i = 0; i < entity_count; ++i) {
        comp::Visual &vi = visual[i];
        if(vi.state & VisualState::QUEUE_DISABLE) {
            disable_sprite(vi.sp);
            vi.state = VisualState::NONE;
            continue;
        }
        if(vi.state & VisualState::QUEUE_ENABLE)
            enable_sprite(vi.sp);
        if(vi.state & VisualState::QUEUE_UPD_POS)
            update_sprite_attrs(vi.sp);
        // TODO: [ fix this bug - theoretically we don't need to use this
        if(vi.sp.sprite.in_use == 0) 
            continue;
        // ]
        if(vi.state & VisualState::QUEUE_ENABLE || vi.anim_id != vi.prev_anim_id) {
            // a full redraw is needed to clean up the VRAM garbage
            update_sprite_frame(
                    vi.sp, vi.data, vi.sp.tiles.count, 
                    vi.anim_id, gba::min(vi.frame_id, vi.anim_len - 1));
            vi.prev_anim_id = vi.anim_id;
            vi.prev_frame_id = vi.frame_id;
        } else if(vi.state & VisualState::QUEUE_UPD_FRAME) {
            update_sprite_frame(
                    vi.sp, vi.data, vi.sp.tiles.count, 
                    vi.anim_id, gba::min(vi.frame_id, vi.anim_len - 1),
                    vi.prev_anim_id, vi.prev_frame_id);
            vi.prev_anim_id = vi.anim_id;
            vi.prev_frame_id = vi.frame_id;
        }
        vi.state = VisualState::NONE;
    }
}


/// @brief Freeing up the attached sprite resources.
/// WARNING: Call this during vblank.
/// @param entity_count Entity count
/// @param visual Corresponding visual components array.
export void free_all(
        const u32 entity_count,
        comp::Visual* visual) {
    for(u32 i = 0; i < entity_count; ++i) {
        comp::Visual &vi = visual[i];
        hide_visual(vi);
        if(vi.state & VisualState::QUEUE_DISABLE)
            disable_sprite(vi.sp);
        vi.state = VisualState::NONE;
    }
}

} // director

} // namespace sys
} // namespace gba
