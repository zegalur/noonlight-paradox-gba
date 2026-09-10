/*!****************************************************************************
 * @file damager.cppm
 * @brief This system is responsible for making/adding damage and 
 *        killing 0 health entities.
 *******************************************************************************/        

module;
export module engine:damager;

import :types;
import :spatial;
import :health;
import :damage;
import :action;

namespace gba {
namespace sys {

/// @brief Kills zero-health entities.
export template<typename OnDeadFunc> 
inline void kill_health_0(
        const u32 entity_count,
        const gba::comp::Spatial* const spatial,
        comp::Health* const health,
        const comp::Action* const actions,
        OnDeadFunc& on_dead
        ) {
    for(u32 i=0; i<entity_count; ++i) {
        if(health[i].is_dead)
            continue;
        if(!spatial[i].visible)
            continue;
        if(health[i].health == 0) {
            health[i].is_dead = true;
            if(actions[i].flags & gba::comp::ActionFlags::ON_DEAD)
                on_dead(i, spatial[i].px + (spatial[i].sx >> 1), 
                           spatial[i].py + (spatial[i].sy >> 1));
        }
    }
}


/// @brief Responsible for adding damage and calling damage related callbacks.
export template<typename OnDamageFunc, typename OnDeadFunc> 
inline void do_damage(
        const u32 damage_mask,
        const u32 amount,
        const i32 px,
        const i32 py,
        const u32 entity_count,
        const gba::comp::Spatial* const spatial,
        comp::Health* const health,
        const comp::Damage* const damage,
        const comp::Action* const actions,
        OnDamageFunc& on_damage,
        OnDeadFunc& on_dead
        ) {
    for(u32 i=0; i<entity_count; ++i) {
        if(!(damage[i].damage_mask & damage_mask))
            continue;
        if(health[i].is_dead)
            continue;
        if(!spatial[i].visible)
            continue;
        if(health[i].health == 0) {
            health[i].is_dead = true;
            if(actions[i].flags & gba::comp::ActionFlags::ON_DEAD)
                on_dead(i, px, py);
            continue;
        }
        if(!gba::comp::contains(spatial[i], px, py))
            continue;
        if(health[i].health > amount) {
            health[i].health -= amount;
            if(actions[i].flags & gba::comp::ActionFlags::ON_DAMAGE)
                on_damage(i, px, py);
        } else {
            health[i].health = 0;
            health[i].is_dead = true;
            if(actions[i].flags & gba::comp::ActionFlags::ON_DEAD)
                on_dead(i, px, py);
        }
    }
}

} // namespace sys
} // namespace gba
