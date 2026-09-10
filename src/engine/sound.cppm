/*!****************************************************************************
 * @file sound.cppm
 * @brief Low-level GBA DMG API.
 *******************************************************************************/ 

module;
export module engine:sound;

import :types;
import :memory;
import :notes;

namespace gba {

namespace sound {

export enum Timed {
    HOLD = 0x0000,
    TIMED = 0x4000
};

export enum Reset {
    OFF = 0x0000,
    ON = 0x8000,
};

/// @brief For REG_SND1FREQ and REG_SND2FREQ.
export consteval u32 FREQ(
        const u32 rate,
        const Timed timed,
        const Reset reset) {
    /*static_assert(rate < (1 << 11));*/
    return rate | timed | reset;
}

export enum Duty {
    D1_8 = 0x0000, // 12.5%
    D1_4 = 0x0040, // 25%
    D1_2 = 0x0080, // 50%
    D3_4 = 0x00C0, // 75%
};

export enum EnvDir {
    INC = 0x0000,
    DEC = 0x0800,
};

/// @brief For REG_SND1CNT and REG_SND2CNT.
export consteval u32 CNT(
        const u32 len,
        const Duty duty,
        const u32 env_step_time,
        const EnvDir dir, 
        const u32 initial_vol) {
    /*static_assert(len <= (1 << 6));
    static_assert(env_step_time <= (1 << 3));
    static_assert(initial_vol <= (1 << 4));*/
    return len | duty | (env_step_time << 0x8) | dir | (initial_vol << 0xC);
}

constexpr u32 MAX_CMD_COUNT = 16;

export enum DMGVolumeRatio {
    VOL_25 = 0b00,
    VOL_50 = 0b01,
    VOL_100 = 0b10
};

export enum Cmd {
    WAIT, // wait some number of bpm
    SQR_1_SWEEP,
    SQR_1_FREQ,
    SQR_1_CNT,
    SQR_2_FREQ,
    SQR_2_CNT,
    WAVE_MODE,
    WAVE_CNT,
    WAVE_FREQ,
    NOISE_CNT,
    NOISE_FREQ,
};

export enum ChannelMask {
    LSQR1  = 0b0000000100000000,
    LSQR2  = 0b0000001000000000,
    LWAVE  = 0b0000010000000000,
    LNOISE = 0b0000100000000000,
    RSQR1  = 0b0001000000000000,
    RSQR2  = 0b0010000000000000,
    RWAVE  = 0b0100000000000000,
    RNOISE = 0b1000000000000000,
};

export struct Track {
    const Cmd cmd;
    const u32 data;
};

export struct DMG_Data {
    const Track* cur_track;
    u32 cur_track_len;
    u32 cur_track_pos;
    bool loop_track;
    u32 wait_timer;
};

export DMG_Data dmg_data;


/// @brief Enables the sound.
/// WARNING: Set this before doing ANYTHING sound related.
export inline void enable_sound() {
    REG_SNDSTAT = 1 << 0x7;
}

export inline void disable_sound() {
    REG_SNDSTAT = 0;
}

export inline void set_dmg_volume_ratio(const DMGVolumeRatio ratio) {
    REG_SNDDSCNT = ratio;
}

/// @brief Sets the main volume of the DMG channels.
/// @param left_volume_3 Left volume (3 bits, from [0,7]).
/// @param right_volume_3 Right volume (3 bits, from [0,7]).
/// @param channels_mask Use a combination of the `ChannelMask` values.
export inline void set_dmg_volume(
        const u32 left_volume_3,
        const u32 right_volume_3,
        const u32 channels_mask
        ) {
    REG_SNDDMGCNT = (left_volume_3 & 7) 
                  | ((right_volume_3 & 7) << 0x4)
                  | channels_mask;
}

export inline void stop_music(DMG_Data&/* data*/ = dmg_data) {
    // turn off the music timer
    REG_TM0CNT = 0;
    // set all volumes to 0
    REG_SND1CNT = 0;
    REG_SND2CNT = 0;
    REG_SND3CNT = 0;
    REG_SND4CNT = 0;
}


/// @param bpm Beats per minute. See the next parameter.
/// @param sub Subdivision (1,2,3,4 etc). 
///            (bpm*sub) - the number of 'pixels' per minute.
///            Pixel is the minimal resolution.
export consteval u32 PLEN(
        const u32 bpm,
        const u32 sub) {
    return u32(982962) / (bpm * sub);
}


export inline void upd_music(DMG_Data& data = dmg_data) {
    if(!data.cur_track)
        return;

    if(data.wait_timer > 1) {
        --data.wait_timer;
        return;
    }

    for(u32 i = 0; i < MAX_CMD_COUNT; ++i) {
        if(data.cur_track_pos == data.cur_track_len) {
            if(!data.loop_track) {
                stop_music(data);
                return;
            }
            data.cur_track_pos = 0;
        }
        const Cmd cmd = data.cur_track[data.cur_track_pos].cmd;
        const u32 cmd_data = data.cur_track[data.cur_track_pos].data;
        ++data.cur_track_pos;
        switch(cmd) {
            case WAIT:
                data.wait_timer = cmd_data;
                return;
            case SQR_1_SWEEP: REG_SND1SWEEP = cmd_data; break;
            case SQR_1_FREQ:  REG_SND1FREQ  = cmd_data; break;
            case SQR_1_CNT:   REG_SND1CNT   = cmd_data; break;
            case SQR_2_FREQ:  REG_SND2FREQ  = cmd_data; break;
            case SQR_2_CNT:   REG_SND2CNT   = cmd_data; break;
            case WAVE_MODE:   REG_SND3SEL   = cmd_data; break;
            case WAVE_CNT:    REG_SND3CNT   = cmd_data; break;
            case WAVE_FREQ:   REG_SND3FREQ  = cmd_data; break;
            case NOISE_CNT:   REG_SND4CNT   = cmd_data; break;
            case NOISE_FREQ:  REG_SND4FREQ  = cmd_data; break;
        }
    }

}


export inline bool is_music_done(DMG_Data& data = dmg_data) {
    return data.cur_track_pos == data.cur_track_len;
}


export inline void play_music(
        const u32 pixel_length,
        const Track* const track,
        const u32 track_len,
        const bool loop,
        const u32 start_from = 0,
        DMG_Data& data = dmg_data
        ) {
    stop_music(data);

    data.cur_track = track;
    data.cur_track_len = track_len;
    data.cur_track_pos = start_from;
    data.loop_track = loop;
    data.wait_timer = 0;

    // play the first potion of the melody right away
    upd_music(data);

    // set up the timer for longer melodies
    if(!is_music_done()) {
        // timer counter will "tick" `pixel_length` times before overflows
        REG_TM0D = ((u32(1) << u32(16)) - 1) - pixel_length;
        REG_TM0CNT = 3 | (1 << 0x6) | (1 << 0x7); //1024cycles|interrupt_ON|ON
    }
}




} // namespace sound

} // namespace gba
