module;
export module game.entities.e_switch;

import engine;
import game.options;
import game.particles;
import data.switch_01;

using namespace gba;

namespace game {
namespace ent {
namespace e_switch {

export constexpr fix32 ANIM_SPEED = fix32(29);
export constexpr u32 DATA_IS_ON = 1;

// TODO: make this a template and move somewhere else
inline void change_animation(
        comp::Visual& v,
        const data::Anim_switch_01 anim_id,
        const bool loop,
        const fix32 speed) {
    if(v.anim_id == anim_id)
        return;
    comp::play_anim(v, anim_id, data::switch_01_frame_count[anim_id],
            0, loop, speed, sys::director::VisualState::QUEUE_UPD_FRAME);
}

export void on_damage(
        comp::Spatial& /*sp*/,
        comp::Visual& v,
        comp::Action& a,
        comp::Health& h,
        const i32 damage_px,
        const i32 damage_py,
        const fix32 /*from_px*/,
        const fix32 /*from_py*/,
        particles::Data& p_data) {
    h.health = h.max_health;
    particles::add_sparkle(p_data, from_int(damage_px), from_int(damage_py));
    a.data[DATA_IS_ON].b = !a.data[DATA_IS_ON].b;
    if(a.data[DATA_IS_ON].b)
        change_animation(v, data::Anim_switch_01::ON, false, ANIM_SPEED);
    else
        change_animation(v, data::Anim_switch_01::OFF, false, ANIM_SPEED);
}

} // namespace e_switch
} // namespace ent
} // namespace game

