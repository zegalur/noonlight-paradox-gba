module;
export module game.music.intro;

import engine;

using namespace gba;
using namespace gba::sound;

namespace game {
namespace music {

// ------------------------------ Intro music ------------------------------ //

export constexpr u32 intro_ch_mask = ChannelMask::LSQR2 
                                   | ChannelMask::RSQR2
                                   | ChannelMask::LSQR1 
                                   | ChannelMask::RSQR1;

export constexpr auto intro_plen = PLEN(100, 2);

export const Track intro_melody[] =
    //
    { {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::C3,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::G3,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::C4,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::D4,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_1_CNT, CNT(0,Duty::D1_8,0,EnvDir::INC,10)}
    , {Cmd::SQR_1_FREQ, FREQ(Note::C3, HOLD,Reset::ON)}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::E4,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::D4,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::C4,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::G3,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_1_CNT, CNT(0,Duty::D1_8,0,EnvDir::DEC,0)}
    , {Cmd::SQR_1_FREQ, FREQ(Note::C3, HOLD,Reset::ON)}
    //
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::D3,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::A3,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::D4,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::E4,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_1_CNT, CNT(0,Duty::D1_8,0,EnvDir::INC,10)}
    , {Cmd::SQR_1_FREQ, FREQ(Note::D3, HOLD,Reset::ON)}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::F4,TIMED,Reset::ON)}, {Cmd::WAIT,3}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::A3,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_1_CNT, CNT(0,Duty::D1_8,0,EnvDir::DEC,0)}
    , {Cmd::SQR_1_FREQ, FREQ(Note::C3, HOLD,Reset::ON)}
    //
    , {Cmd::SQR_1_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,0)}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::A3,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::E4,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::A4,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::B4,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_1_CNT, CNT(0,Duty::D1_8,0,EnvDir::INC,10)}
    , {Cmd::SQR_1_FREQ, FREQ(Note::A2, HOLD,Reset::ON)}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::C5,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::B4,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::A4,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::E4,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_1_CNT, CNT(0,Duty::D1_8,0,EnvDir::DEC,0)}
    , {Cmd::SQR_1_FREQ, FREQ(Note::C3, HOLD,Reset::ON)}
    //
    , {Cmd::SQR_1_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,0)}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::E3,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::B3,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::E4,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::F4,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_1_CNT, CNT(0,Duty::D1_8,0,EnvDir::INC,10)}
    , {Cmd::SQR_1_FREQ, FREQ(Note::B2, HOLD,Reset::ON)}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::G4,TIMED,Reset::ON)}, {Cmd::WAIT,3}
    , {Cmd::SQR_2_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::B3,TIMED,Reset::ON)}, {Cmd::WAIT,1}
    , {Cmd::SQR_1_CNT, CNT(0,Duty::D1_8,0,EnvDir::DEC,0)}
    , {Cmd::SQR_1_FREQ, FREQ(Note::C3, HOLD,Reset::ON)}
    };

// ------------------------------ Explosions ------------------------------- //

export constexpr u32 intro_expl_ch_mask = ChannelMask::LNOISE 
                                        | ChannelMask::RNOISE
                                        | ChannelMask::LSQR1
                                        | ChannelMask::RSQR1;

export constexpr auto intro_expl_plen = PLEN(50, 1);

export const Track intro_expl_sound[] =
    { {Cmd::NOISE_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,15)}
    , {Cmd::NOISE_FREQ, FREQ(Note::C3,HOLD,Reset::ON)}
    , {Cmd::SQR_1_CNT, CNT(0,Duty::D1_8,0,EnvDir::INC,10)}
    , {Cmd::SQR_1_FREQ, FREQ(Note::D2,TIMED,Reset::ON)}
    , {Cmd::WAIT,1}
    , {Cmd::NOISE_CNT, CNT(0,Duty::D3_4,0,EnvDir::DEC,13)}
    , {Cmd::NOISE_FREQ, FREQ(Note::D3,HOLD,Reset::ON)}
    , {Cmd::WAIT,1}
    , {Cmd::NOISE_CNT, CNT(0,Duty::D1_8,0,EnvDir::DEC,11)}
    , {Cmd::NOISE_FREQ, FREQ(Note::D3,HOLD,Reset::ON)}
    , {Cmd::WAIT,1}
    , {Cmd::NOISE_CNT, CNT(0,Duty::D1_2,0,EnvDir::DEC, 9)}
    , {Cmd::NOISE_FREQ, FREQ(Note::D3,HOLD,Reset::ON)}
    , {Cmd::WAIT,1}
    };

} // namespace music
} // namespace game
