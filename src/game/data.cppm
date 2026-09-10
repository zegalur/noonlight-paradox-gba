/*!****************************************************************************
 * @file data.cppm
 * @brief This module describes the global game data, common to all states.
 *******************************************************************************/        

module;
export module game.data;

import engine;

using namespace gba;

namespace game {

/// @brief How many user data can be stored in session data variable.
export constexpr i32 SESSION_DATA_COUNT = 8;

/// @brief How many languages this game supports.
export constexpr i32 LANG_COUNT = 3;

/// @brief Supported languages.
export enum Language {
    ENG = 0, 
    UKR = 1,
    JAP = 2
};

export constexpr u32 GREEN_KEY_FLAG = 0b1;
export constexpr u32 BLUE_KEY_FLAG  = 0b10;
export constexpr u32 GOLD_KEY_FLAG  = 0b100;

/// @brief Session persistent data. Save and restore this to restore the session.
///        (Eg, loading the same map on respawn etc.)
export struct SessionData {
    /// @brief bit set to 0 for already activated oneshot triggers.
    u32 trigger_flags;

    /// @brief last checkpoint ID
    u32 checkpoint_id;
    
    /// @brief Keys player has (on this map)
    u32 key_flags;

    /// @brief Custom data.
    Var data[SESSION_DATA_COUNT];

    SessionData()
    : trigger_flags(~u32(0))
    , checkpoint_id(0)
    , key_flags(0) { 
        // zero out the data
        for(u32 i = 0; i < SESSION_DATA_COUNT; ++i)
            data[i].i = 0;
    }
};

/// @brief Non-persistent flags that zero-up after new-game or continue.
export enum TmpFlags {
    TUT_RESPAWN = 0b1
};

/// @brief Global game data, common to all game states.
export struct GameData {
    Language language;
    u32 menu_template_wave_pos;
    const Font* std_font;
    SessionData session_data;
    /// @brief Non-persistent flags that zero-up after new-game or continue.
    u32 tmp_flags;
};


} // namespace game
