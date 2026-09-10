module;
export module game.entities.izone;

import engine;
import game.options;
import game.data;
import data.izone;

using namespace gba;

namespace game {
namespace ent {
namespace izone {

constexpr fix32 IDLE_SPEED = fix32(11*2);
constexpr u32 DATA_ACTIVE = 1;

inline void change_animation(
        comp::Visual& v,
        const data::Anim_izone anim_id,
        const bool loop,
        const fix32 speed) {
    if(v.anim_id == anim_id)
        return;
    comp::play_anim(v, anim_id, data::izone_frame_count[anim_id],
            0, loop, speed, sys::director::VisualState::QUEUE_UPD_FRAME);
}

export void on_player_collide(
        comp::Action& a,
        comp::Visual& v) {
    change_animation(v, data::Anim_izone::IDLE, true, IDLE_SPEED);
    a.data[DATA_ACTIVE].b = true;
}

export void on_update(
        comp::Action& a,
        comp::Visual& v) {
    if(a.data[DATA_ACTIVE].b == false)
        change_animation(v, data::Anim_izone::HIDDEN, false, IDLE_SPEED);
    a.data[DATA_ACTIVE].b = false;
}

} // namespace izone
} // namespace ent
} // namespace game

