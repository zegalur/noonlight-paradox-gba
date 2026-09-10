/*!****************************************************************************
 * @file subgame.cppm
 * @brief Use this module to call the std game cycle.
 *******************************************************************************/ 

module;
export module engine:subgame;

import :types;
import :input;
import :video;
import :sound;
import :memory;
import :sprites;
import :graphics;
import :interrupts;

#include "macro.hpp"

namespace gba {

export constexpr u32 ERROR_STATUS = 0xFFFFFFFF;
export constexpr u32 EXIT_STATUS = 0xFFFFFFFE;


export template<typename InitFunc, typename Data>
concept SubgameInit = requires(InitFunc f, Data &d) {
    { f(d) } -> SameAs<u32>;
};


export template<typename UpdFunc, typename Data>
concept SubgameUpdate = requires(UpdFunc f, u32 s, Data &d) {
    { f(s, d) } -> SameAs<u32>;
};


export template<typename DrawFunc, typename Data>
concept SubgameDraw = requires(DrawFunc f, u32 s, Data &d) {
    { f(s, d) } -> SameAs<u32>;
};


// compiled in `arm` and placed into the `.ewram` section
__attribute__((target("arm"))) 
EWRAM_CODE void func0(void) {
    //u32 ie = REG_IE;
    u32 ieif = /*ie &*/ REG_IF;

    REG_IF = ieif;
    REG_IFBIOS |= ieif;

    if(ieif & IRQ_Masks::IRQ_TIMER0) {
        u32 ime = REG_IME;
        REG_IME = 0;
        //REG_IE &= ~ieif;
        sound::upd_music(sound::dmg_data);
        REG_IME = 0;
        //REG_IE = ie;
        REG_IME = ime;
    }
}


void init_interrupts() {
    REG_IME = 0;
    // turn-off all the timers
    REG_TM0CNT = 0;
    REG_TM1CNT = 0;
    REG_TM2CNT = 0;
    REG_TM3CNT = 0;
    // turn-on VBlank interrupts
    REG_ISR_MAIN = func0;
    REG_DISPSTAT |= DISPSTAT_Masks::VBLANK_IRQ_ENABLE;
    REG_IE |= IRQ_Masks::IRQ_VBLANK;
    // enable interrupts
    REG_IME = 1;
}


void init_std_mode() {
    init_interrupts();
    vblank_intr_wait();
    disable_all_sprites();
    sound::enable_sound();
    sound::set_dmg_volume_ratio(sound::DMGVolumeRatio::VOL_100);
}


export template<
    typename Data,
    SubgameInit<Data> InitFunc,
    SubgameUpdate<Data> UpdFunc,
    SubgameDraw<Data> DrawFunc>
void run_subgame(
        Data& data, 
        InitFunc init_func, 
        UpdFunc upd_func,
        DrawFunc draw_func
) {
    init_std_mode();
    u32 last_status = init_func(data);
    switch(last_status) {
        case ERROR_STATUS:
            // TODO: init_func() error handling here...
        case EXIT_STATUS:
            return;
    }
    while(1) {
        update_keypad();
        last_status = upd_func(last_status, data);
        switch(last_status) {
            case ERROR_STATUS:
                // TODO: upd_func() error handling here...
            case EXIT_STATUS:
                return;
        }

        vblank_intr_wait();
        last_status = draw_func(last_status, data);
        switch(last_status) {
            case ERROR_STATUS:
                // TODO: draw_func() error handling here...
            case EXIT_STATUS:
                return;
        }
    }
}


} // namespace gba
