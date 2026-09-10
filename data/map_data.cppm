/*!****************************************************************************
 * @file map_data.cppm
 * @brief Various map data structures.
 *******************************************************************************/        

module;
export module data.map_data;

import engine;

using namespace gba;

namespace data {

/// @brief Map checkpoint data.
export struct CheckPoint {
    /// @brief Checkpoint X position (in pixels).
    i32 pos_x;
    /// @brief Checkpoint Y position (in pixels).
    i32 pos_y;
};


/// @brief Map trigger data.
export struct Trigger {
    /// @brief Trigger X position (in pixels).
    i32 pos_x;
    /// @brief Trigger Y position (in pixels).
    i32 pos_y;
    /// @brief Trigger size in pixels 
    /// NOTE: 2*size_px = square size with a center in (pos_x, pos_y)
    i32 size_px;
    /// @brief Is this trigger a oneshot (activates only once per session).
    bool oneshot;
};


/// @brief General map data, like a list of checkpoints etc.
export struct MapData {
    /// @brief Checkpoint count.
    const u32 cp_count;
    /// @brief Pointer to checkpoint array (with `cp_count` elements).
    const CheckPoint* const checkpoints;
    /// @brief Trigger count.
    const u32 tr_count;
    /// @brief Pointer to trigger array (with `tr_count` elements).
    const Trigger* const triggers;
};

} // namespace data
