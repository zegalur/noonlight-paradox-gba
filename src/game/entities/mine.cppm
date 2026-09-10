module;
export module game.entities.mine;

import engine;
import game.particles;
import game.options;
import game.player;

using namespace gba;

namespace game {
namespace ent {
namespace mine {

constexpr fix32 MOVE_EPS = from_int(7);

export constexpr u32 DATA_X_VELOCITY    = 1;
export constexpr u32 DATA_Y_VELOCITY    = 2;
export constexpr u32 DATA_IS_DYNAMIC    = 3;
export constexpr u32 DATA_TIMER         = 4;

export constexpr fix32 MINE_Y_GRAVITY = from_int(1) >> 6;
export constexpr fix32 MINE_Y_MAX_SPEED = from_int(1) >> 0;
export constexpr fix32 REFL_COEF = from_int(-7) >> 3;


export inline void shoot(
        comp::Spatial& sp, 
        comp::Action& action,
        comp::Health& health,
        const fix32 px,
        const fix32 py,
        const fix32 x_velocity,
        const fix32 y_velocity,
        const u32 timer = u32(0)) {
    sp.visible = true;
    sp.px = px;
    sp.py = py;
    health.is_dead = false;
    health.health = health.max_health;
    action.data[DATA_IS_DYNAMIC].b = true;
    action.data[DATA_X_VELOCITY].f = x_velocity;
    action.data[DATA_Y_VELOCITY].f = y_velocity;
    action.data[DATA_TIMER].u = timer;
}


export void on_update(
        comp::Spatial &sp,
        comp::Action &action,
        comp::Health &health,
        CollisionMap &cmap) {
    if(!sp.visible)
        return;
    if(action.data[DATA_TIMER].u > u32(0)) {
        if(--action.data[DATA_TIMER].u == u32(0))
            health.health = 0;
    }
    if(!action.data[DATA_IS_DYNAMIC].b)
        return;
    const fix32 new_x = sp.px + action.data[DATA_X_VELOCITY].f;
    const fix32 new_y = sp.py + action.data[DATA_Y_VELOCITY].f;
    if(get_collision_color(cmap, intp(new_x)+4, intp(sp.py)+4) == CollisionColor::FREE)
        sp.px = new_x;
    else
        action.data[DATA_X_VELOCITY].f = fix_mul(
                action.data[DATA_X_VELOCITY].f, REFL_COEF);
    if(get_collision_color(cmap, intp(sp.px)+4, intp(new_y)+4) == CollisionColor::FREE)
        sp.py = new_y;
    else
        action.data[DATA_Y_VELOCITY].f = fix_mul(
                action.data[DATA_Y_VELOCITY].f, REFL_COEF);
    action.data[DATA_Y_VELOCITY].f = clampi(
            action.data[DATA_Y_VELOCITY].f + MINE_Y_GRAVITY,
            -(MINE_Y_MAX_SPEED << 1), MINE_Y_MAX_SPEED);
}


export void on_damage(
        comp::Spatial& sp, 
        const i32 damage_px,
        const i32 damage_py,
        const fix32 from_px, 
        const fix32 /*from_py*/,
        particles::Data& p_data) {
    if(from_px < sp.px - MOVE_EPS)
        sp.px += from_int(1);
    else if(from_px > sp.px + MOVE_EPS)
        sp.px -= from_int(1);
    particles::add_sparkle(p_data, from_int(damage_px), from_int(damage_py));
}


export void on_dead(
        comp::Spatial& sp, 
        particles::Data& p_data) {
    sp.visible = false;
    particles::add_explosion(p_data, 
            sp.px + from_int(4), 
            sp.py + from_int(4));
}


export void on_player_collide(
        player::Data& player_data,
        comp::Spatial& sp,
        comp::Health& health,
        particles::Data& particles) {
    health.health = 0;
    health.is_dead = true;
    on_dead(sp, particles);
    player::drain_health(player_data, 
            (health.max_health == 1 ? MINE_01_DAMAGE_MIN : MINE_01_DAMAGE_FULL));
}

} // namespace mine
} // namespace ent
} // namespace game

