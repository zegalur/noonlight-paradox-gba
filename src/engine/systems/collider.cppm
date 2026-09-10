/*!****************************************************************************
 * @file collider.cppm
 * @brief Responsible for detecting collisions.
 *******************************************************************************/        

module;
export module engine:collider;

import :types;
import :spatial;
import :visual;
import :comp_collision;

namespace gba {
namespace sys {

/// @brief Checks for collision with a given body.
/// @param collision_mask Only check the bodies with all bits set from this mask.
/// @param body_spatial Spatial component of the colliding body.
/// @param body_padding The colliding body padding.
/// @param entity_count Entity count.
/// @param spatial Entities spatial component array.
/// @param collision Entities collision component array.
/// @param on_collide A callback function (func(id)).
export
template<typename OnCollideFunc>
inline void collide_with_body(
        const u32 collision_mask,
        const comp::Spatial& body_spatial,
        const i32 body_padding,
        const u32 entity_count,
        const comp::Spatial* const spatial,
        const comp::Collision* const collision,
        OnCollideFunc& on_collide
        ) {
    for(u32 i=0; i<entity_count; ++i) {
        if(spatial[i].visible == false)
            continue;
        if((collision[i].mask & collision_mask) != collision_mask)
            continue;
        switch(collision[i].body_type) {
            case comp::BodyType::AABB:
                if(comp::intersect_aabb(
                        body_spatial, spatial[i], 
                        body_padding, collision[i].padding))
                    on_collide(i);
                break;
        }
    }
}

} // namespace sys
} // namespace gba
