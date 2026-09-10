module;
export module game.entities.spider;

import engine;
import game.particles;
import game.options;
import game.player;
import game.entities.mine;
import data.spider;

using namespace gba;

namespace game {
namespace ent {
namespace spider {

export constexpr u32 E_BODY = 0;
export constexpr u32 E_MINE = 1;

export constexpr u32 DATA_FLAGS = 1;
export constexpr u32 DATA_RELOAD_TIMER = 2;
export constexpr u32 DATA_ACTIVATION_TIMER = 3;

export constexpr u32 ON_GROUND_FLAG     = 0b1;
export constexpr u32 IS_AWAKE_FLAG      = 0b10;
export constexpr u32 MOVE_LEFT_FLAG     = 0b100;
export constexpr u32 IS_DESTROYED_FLAG  = 0b1000;

export constexpr fix32 BODY_ORIGIN_PX = from_int(15);
export constexpr fix32 BODY_ORIGIN_PY = from_int(27);
export constexpr fix32 FREEFALL_SPEED = FIX32_ONE >> 1;
export constexpr fix32 ACTIVATION_DISTANCE_X = from_int(210);
export constexpr fix32 ACTIVATION_DISTANCE_Y = from_int(110);
export constexpr fix32 RUN_CLOSER_DISTANCE = from_int(120);
export constexpr fix32 RUNAWAY_DISTANCE = from_int(110);
export constexpr fix32 SHOOT_DISTANCE = RUN_CLOSER_DISTANCE;
export constexpr fix32 MOVE_SPEED = from_int(1) >> 1;

export constexpr fix32 MINE_X_SPEED = from_int(1) >> 2;
export constexpr fix32 MINE_Y_INITIAL_SPEED = -from_int(1) >> 1;
export constexpr u32 MINE_DET_TIME = 8*60; // 8 sec
export constexpr u32 RELOAD_TIME = 1*60; // 1 sec
export constexpr u32 ACTIVATION_UPD_TIME = 60; // 1 sec

export constexpr i32 MAX_UP_SLOPE = 2;

export constexpr fix32 MOVE_ANIM_SPEED = fix32(70);
export constexpr fix32 IDLE_ANIM_SPEED = fix32(11);
export constexpr fix32 SHOOT_ANIM_SPEED = fix32(70);


// --------------------------------- Init ---------------------------------- //

export void on_init(comp::Spatial* const sp_ptr) {
    sp_ptr[E_BODY].px -= BODY_ORIGIN_PX;
    sp_ptr[E_BODY].py -= BODY_ORIGIN_PY;
}


// -------------------------------- Update --------------------------------- //

inline void change_animation(
        comp::Visual* const v_ptr,
        const data::Anim_spider anim_id,
        const bool loop,
        const fix32 speed) {
    if(v_ptr[E_BODY].anim_id == anim_id)
        return;
    comp::play_anim(v_ptr[E_BODY], anim_id, data::spider_frame_count[anim_id],
            0, loop, speed, sys::director::VisualState::QUEUE_UPD_FRAME);
}


inline void freefall(
        comp::Spatial* const sp_ptr,
        comp::Visual* const /*v_ptr*/,
        comp::Action* const a_ptr,
        CollisionMap& cmap
        ) {
    fix32 px = sp_ptr[E_BODY].px + BODY_ORIGIN_PX;
    fix32 py = sp_ptr[E_BODY].py + BODY_ORIGIN_PY + FREEFALL_SPEED;
    if(get_collision_color(cmap,intp(px),intp(py)) == CollisionColor::FREE) {
        sp_ptr[E_BODY].py = py - BODY_ORIGIN_PY;
    } else {
        i32 ipx = intp(px);
        i32 ipy = intp(py);
        if(find_first_from(CollisionColor::FREE, cmap,
                ipx, ipy, 0, -1, MAX_UP_SLOPE + 1)) {
            sp_ptr[E_BODY].py = from_int(ipy) - BODY_ORIGIN_PY;
            a_ptr[E_BODY].data[DATA_FLAGS].u |= ON_GROUND_FLAG;
        }
    }
}


inline fix32 x_distance_to_player(
        comp::Spatial* const sp_ptr, player::Data& pdata) {
    return mdistance(pdata.pos_x, FIX32_ZERO,
                     sp_ptr[E_BODY].px + BODY_ORIGIN_PX, FIX32_ZERO);
}


inline fix32 y_distance_to_player(
        comp::Spatial* const sp_ptr, player::Data& pdata) {
    return mdistance(FIX32_ZERO, pdata.pos_y, 
                     FIX32_ZERO, sp_ptr[E_BODY].py + BODY_ORIGIN_PY);
}


inline void move_around(
        comp::Spatial* const sp_ptr,
        comp::Visual* const v_ptr,
        comp::Action* const a_ptr,
        CollisionMap& cmap,
        player::Data& pdata
        ) {
    const fix32 dx = x_distance_to_player(sp_ptr, pdata);
    const fix32 dy = y_distance_to_player(sp_ptr, pdata);
    if(dx > ACTIVATION_DISTANCE_X && dy > ACTIVATION_DISTANCE_Y) {
        a_ptr[E_BODY].data[DATA_FLAGS].u &= ~IS_AWAKE_FLAG;
        return; // deactivate
    }
    const fix32 s1 = (dx < RUNAWAY_DISTANCE ? -fix32(1) : 
                     (dx > RUN_CLOSER_DISTANCE ? fix32(1) : FIX32_ZERO));
    const fix32 s2 = (pdata.pos_x < sp_ptr[E_BODY].px ? -fix32(1) : fix32(1));
    const fix32 new_px = sp_ptr[E_BODY].px + s1*s2*MOVE_SPEED;
    const fix32 px = new_px + BODY_ORIGIN_PX;
    const fix32 py = sp_ptr[E_BODY].py + BODY_ORIGIN_PY;
    const data::Anim_spider anim_id = 
            ( s1 == FIX32_ZERO ? data::Anim_spider::IDLE : 
            ( s1*s2 > FIX32_ZERO ? data::Anim_spider::WALK_LTR 
                                 : data::Anim_spider::WALK_RTL));
    const fix32 anim_speed = 
            (s1 == FIX32_ZERO ? IDLE_ANIM_SPEED : MOVE_ANIM_SPEED);
    i32 ipx = intp(px);
    i32 ipy = intp(py);
    const CollisionColor c1 = get_collision_color(cmap, intp(px), intp(py));
    if(c1 != CollisionColor::FREE) {
        // check if we have an uphill situation
        if(find_first_from(CollisionColor::FREE, cmap,
                ipx, ipy, 0, -1, MAX_UP_SLOPE + 1)) {
            change_animation(v_ptr, anim_id, true, anim_speed);
            sp_ptr[E_BODY].py = from_int(ipy) - BODY_ORIGIN_PY;
            sp_ptr[E_BODY].px = new_px;
            return;
        }
    } else {
        // move, but only if it's an allowed downhill
        if(find_first_from(CollisionColor::PLATFORM, cmap,
                ipx, ipy, 0, 1, MAX_UP_SLOPE + 1)) {
            change_animation(v_ptr, anim_id, true, anim_speed);
            sp_ptr[E_BODY].py = from_int(ipy - 1) - BODY_ORIGIN_PY;
            sp_ptr[E_BODY].px = new_px;
            return;
        }
    }
    change_animation(v_ptr, data::Anim_spider::IDLE, true, IDLE_ANIM_SPEED);
}


inline void shoot_mine(
        comp::Spatial* const sp_ptr,
        comp::Visual* const v_ptr,
        comp::Action* const a_ptr,
        comp::Health* const h_ptr,
        CollisionMap& /*cmap*/,
        player::Data& pdata) {
    if(sp_ptr[E_MINE].visible)
        return; // is there already
    if(a_ptr[E_BODY].data[DATA_RELOAD_TIMER].u > 0)
        return; // reload
    const fix32 dx = x_distance_to_player(sp_ptr, pdata);
    if(dx < SHOOT_DISTANCE) {
        const bool shoot_right = (pdata.pos_x < sp_ptr[E_MINE].px ? false : true);
        a_ptr[E_BODY].data[DATA_RELOAD_TIMER].u = RELOAD_TIME;
        change_animation(v_ptr, data::Anim_spider::SHOOT, false, SHOOT_ANIM_SPEED);
        mine::shoot(sp_ptr[E_MINE], a_ptr[E_MINE], h_ptr[E_MINE],
                    sp_ptr[E_BODY].px + BODY_ORIGIN_PX,
                    sp_ptr[E_BODY].py + 0*(BODY_ORIGIN_PY << 1),
                    (shoot_right ? MINE_X_SPEED : -MINE_X_SPEED),
                    MINE_Y_INITIAL_SPEED, MINE_DET_TIME);
    }
}


export void on_update(
        comp::Spatial* const sp_ptr,
        comp::Visual* const v_ptr,
        comp::Action* const a_ptr,
        comp::Health* const h_ptr,
        CollisionMap& cmap,
        player::Data& pdata) {
    if(a_ptr[E_BODY].data[DATA_FLAGS].u & IS_DESTROYED_FLAG)
        return; // spider robot has been destroyed
    if(a_ptr[E_BODY].data[DATA_RELOAD_TIMER].u > 0) {
        a_ptr[E_BODY].data[DATA_RELOAD_TIMER].u--;
    }
    const bool on_ground = a_ptr[E_BODY].data[DATA_FLAGS].u & ON_GROUND_FLAG;
    if(on_ground == false) {
        freefall(sp_ptr, v_ptr, a_ptr, cmap);
    } else {
        if(v_ptr[E_BODY].anim_id == data::Anim_spider::SHOOT) {
            if(!comp::is_anim_done(v_ptr[E_BODY]))
                return;
        }
        if(a_ptr[E_BODY].data[DATA_FLAGS].u & IS_AWAKE_FLAG) {
            move_around(sp_ptr, v_ptr, a_ptr, cmap, pdata);
            shoot_mine(sp_ptr, v_ptr, a_ptr, h_ptr, cmap, pdata);
        }
        else {
            if(++a_ptr[E_BODY].data[DATA_ACTIVATION_TIMER].u >= ACTIVATION_UPD_TIME) {
              a_ptr[E_BODY].data[DATA_ACTIVATION_TIMER].u = 0;
              if(x_distance_to_player(sp_ptr, pdata) < ACTIVATION_DISTANCE_X)
                if(y_distance_to_player(sp_ptr, pdata) < ACTIVATION_DISTANCE_Y)
                  a_ptr[E_BODY].data[DATA_FLAGS].u |= IS_AWAKE_FLAG;
            }
        }
    }
}


// --------------------------------- Other --------------------------------- //

export void on_damage(
        comp::Spatial& /*sp*/, 
        const i32 damage_px,
        const i32 damage_py,
        const fix32 /*from_px*/, 
        const fix32 /*from_py*/,
        particles::Data& p_data) {
    particles::add_sparkle(p_data, from_int(damage_px), from_int(damage_py));
}


export void on_dead(
        comp::Spatial& sp, 
        comp::Action& action,
        particles::Data& p_data) {
    sp.visible = false;
    action.data[DATA_FLAGS].u |= IS_DESTROYED_FLAG;
    particles::add_explosion(
            p_data, sp.px + BODY_ORIGIN_PX, sp.py + (BODY_ORIGIN_PY >> 1));
}


/*export void on_player_collide(
        player::Data& player_data,
        comp::Spatial& sp,
        comp::Health& health,
        particles::Data& particles) {
    // ...
}*/

} // namespace spider
} // namespace ent
} // namespace game


