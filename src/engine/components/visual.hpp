/// @brief A helper macros for defining the visual component (general case).
#define VISUAL( palette, \
                tile_placeholder, \
                sp_size, \
                sp_priority, \
                data_ptr, \
                anim_id, \
                frame_count, \
                anim_speed_fix32, \
                anim_loop, \
                from_frame ) \
    { PoolAnimSprite{ \
            palette, tile_placeholder, {{},1},\
            new_obj_attr_##sp_size(0,0,sp_priority, SpriteMode::Disable) }, \
    data_ptr, sys::director::VisualState::NONE, anim_id, frame_count, \
    anim_speed_fix32, anim_loop, from_frame, from_int(0), anim_id,from_frame }


/// @brief A helper macros for defining the visual component (for data::...).
#define DVISUAL(palette, \
                tile_placeholder, \
                sp_size, \
                sp_priority, \
                res_name, \
                anim_id, \
                anim_speed_fix32, \
                anim_loop, \
                from_frame ) \
    VISUAL( palette, tile_placeholder, sp_size, sp_priority, \
            data::res_name##_data_ptr, data::Anim_##res_name::anim_id, \
            data::res_name##_frame_count[data::Anim_##res_name::anim_id],\
            anim_speed_fix32, anim_loop, from_frame )
