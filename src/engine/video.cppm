/*!****************************************************************************
 * @file video.cppm
 * @brief Part of low-level GBA graphics API.
 *******************************************************************************/ 

module;
export module engine:video;

import :types;
import :memory;

#include "macro.hpp"

namespace gba {

export constexpr auto SCREEN_WIDTH = 240;
export constexpr auto SCREEN_HEIGHT = 160;

export constexpr auto SCREEN_WIDTH_T = SCREEN_WIDTH >> 3; // in tiles
export constexpr auto SCREEN_HEIGHT_T = SCREEN_HEIGHT >> 3; // in tiles

export constexpr u8 SCREEN_WIDTH_U8 = (u8)(SCREEN_WIDTH);
export constexpr u16 SCREEN_WIDTH_U16 = (u16)(SCREEN_WIDTH);
export constexpr u32 SCREEN_WIDTH_U32 = (u32)(SCREEN_WIDTH);

export constexpr u8 SCREEN_HEIGHT_U8 = (u8)(SCREEN_HEIGHT);
export constexpr u16 SCREEN_HEIGHT_U16 = (u16)(SCREEN_HEIGHT);
export constexpr u32 SCREEN_HEIGHT_U32 = (u32)(SCREEN_HEIGHT);


export u16* const video_memory = reinterpret_cast<u16*>(MEM_VRAM);

// video modes
export constexpr u32 DCNT_MODE0 = 0x0000;
export constexpr u32 DCNT_MODE1 = 0x0001;
export constexpr u32 DCNT_MODE2 = 0x0002;
export constexpr u32 DCNT_MODE3 = 0x0003;
export constexpr u32 DCNT_MODE4 = 0x0004;
export constexpr u32 DCNT_MODE5 = 0x0005;

// layers
export constexpr u32 DCNT_BG0 = 0x0100;
export constexpr u32 DCNT_BG1 = 0x0200;
export constexpr u32 DCNT_BG2 = 0x0400;
export constexpr u32 DCNT_BG3 = 0x0800;
export constexpr u32 DCNT_OBJ = 0x1000;


// `REG_DISPSTAT` control bits

export enum DISPSTAT_Masks {
    VBLANK_FLAG = 1 << 0, // read only
    HBLANK_FLAG = 1 << 1, // read only
    VCOUNTER_FLAG = 1 << 2, // read only
	VBLANK_IRQ_ENABLE = 1 << 3, // 1=enable
	HBLANK_IRQ_ENABLE = 1 << 4, // 1=enable
	VCOUNT_IRQ_ENABLE = 1 << 5, // 1=enable
};


// other

export void vblank_intr_wait() {
    swi_call(0x05);
}


} // namespace gba
