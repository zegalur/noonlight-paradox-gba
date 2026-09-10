module;
export module game.music.factory;

import engine;

using namespace gba;
using namespace gba::sound;

namespace game {
namespace music {

// --------------------------- Level Intro Music --------------------------- //

export constexpr u32 factory_1_ch_mask = 
          ChannelMask::LSQR2 | ChannelMask::RSQR2
        | ChannelMask::LSQR1 | ChannelMask::RSQR1;

export constexpr auto factory_1_plen = PLEN(100, 2);

#define BLOCK(v, w, h) \
  {Cmd::SQR_1_CNT,  CNT(0,Duty::D1_2,0,EnvDir::INC,clamp(10-h,0,15))}\
, {Cmd::SQR_1_FREQ, FREQ(Note::E2, HOLD,Reset::ON)}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(15-v,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::Ab2,TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(13-w,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::E2, TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(15-v,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::B2, TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(13-w,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::Ab2,TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(15-v,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::E3, TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(13-w,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::B2, TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(15-v,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::Ab3,TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(13-w,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::E3, TIMED,Reset::ON)}, {Cmd::WAIT,1}\
\
, {Cmd::SQR_1_CNT,  CNT(0,Duty::D1_4,0,EnvDir::INC,clamp(10-h,0,15))}\
, {Cmd::SQR_1_FREQ, FREQ(Note::G2, HOLD,Reset::ON)}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(15-v,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::B3, TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(13-w,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::G3, TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(15-v,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::G3, TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(13-w,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::D3, TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(15-v,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::D3, TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(13-w,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::B2, TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(15-v,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::B2, TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(13-w,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::G2, TIMED,Reset::ON)}, {Cmd::WAIT,1}\
\
, {Cmd::SQR_1_CNT,  CNT(0,Duty::D1_8,0,EnvDir::INC,clamp(10-h,0,15))}\
, {Cmd::SQR_1_FREQ, FREQ(Note::B2, HOLD,Reset::ON)}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(15-v,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::Gb2,TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(13-w,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::D2, TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(15-v,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::B2, TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(13-w,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::Gb2,TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(15-v,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::D3, TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(13-w,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::B2, TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(15-v,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::Gb3,TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(13-w,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::D3, TIMED,Reset::ON)}, {Cmd::WAIT,1}\
\
, {Cmd::SQR_1_CNT,  CNT(0,Duty::D1_8,0,EnvDir::INC,clamp(10-h,0,15))}\
, {Cmd::SQR_1_FREQ, FREQ(Note::C3, HOLD,Reset::ON)}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(15-v,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::C4, TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(13-w,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::G3, TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(15-v,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::G3, TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(13-w,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::E3, TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_1_CNT,  CNT(0,Duty::D1_8,0,EnvDir::INC,clamp(8-h,0,15))}\
, {Cmd::SQR_1_FREQ, FREQ(Note::D3, HOLD,Reset::ON)}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(15-v,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::Gb3,TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(13-w,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::D3, TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(15-v,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::D3, TIMED,Reset::ON)}, {Cmd::WAIT,1}\
, {Cmd::SQR_2_CNT,  CNT(0,Duty::D3_4,0,EnvDir::DEC,clamp(13-w,0,15))}\
, {Cmd::SQR_2_FREQ, FREQ(Note::A2, TIMED,Reset::ON)}, {Cmd::WAIT,1}

export const Track factory_1_melody[] = {
    {Cmd::SQR_1_CNT,  CNT(0,Duty::D1_2,0,EnvDir::INC,2)},
    {Cmd::SQR_1_FREQ, FREQ(Note::E2, HOLD,Reset::ON)}, 
    {Cmd::WAIT,1},
    {Cmd::SQR_1_CNT,  CNT(0,Duty::D1_2,0,EnvDir::INC,4)},
    {Cmd::SQR_1_FREQ, FREQ(Note::G2, HOLD,Reset::ON)}, 
    {Cmd::WAIT,1},
    {Cmd::SQR_1_CNT,  CNT(0,Duty::D1_2,0,EnvDir::INC,6)},
    {Cmd::SQR_1_FREQ, FREQ(Note::B2, HOLD,Reset::ON)}, 
    {Cmd::WAIT,1},
    {Cmd::SQR_1_CNT,  CNT(0,Duty::D1_2,0,EnvDir::INC,8)},
    {Cmd::SQR_1_FREQ, FREQ(Note::C3, HOLD,Reset::ON)}, 
    {Cmd::WAIT,1},
    //
    {Cmd::SQR_1_CNT,  CNT(0,Duty::D1_2,0,EnvDir::INC,8)},
    {Cmd::SQR_1_FREQ, FREQ(Note::E2, HOLD,Reset::ON)}, 
    {Cmd::WAIT,1},
    {Cmd::SQR_1_CNT,  CNT(0,Duty::D1_2,0,EnvDir::INC,6)},
    {Cmd::SQR_1_FREQ, FREQ(Note::G2, HOLD,Reset::ON)}, 
    {Cmd::WAIT,1},
    {Cmd::SQR_1_CNT,  CNT(0,Duty::D1_2,0,EnvDir::INC,4)},
    {Cmd::SQR_1_FREQ, FREQ(Note::B2, HOLD,Reset::ON)}, 
    {Cmd::WAIT,1},
    {Cmd::SQR_1_CNT,  CNT(0,Duty::D1_2,0,EnvDir::INC,8)},
    {Cmd::SQR_1_FREQ, FREQ(Note::C3, HOLD,Reset::ON)}, 
    {Cmd::WAIT,1},
    //
    BLOCK(0,0,0),
    BLOCK(0,3,0),
    BLOCK(5,8,3),
    BLOCK(10,13,6),
    BLOCK(13,15,7)
};


// ------------------------- Factory 00 Password --------------------------- //

export constexpr u32 factory_password_ch_mask = 
          ChannelMask::LSQR2 | ChannelMask::RSQR2;

export constexpr auto factory_password_plen = PLEN(50, 2);

export const Track factory_password[] =
    { {Cmd::SQR_2_CNT,  CNT(0, Duty::D3_4, 0, EnvDir::DEC, 15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::Eb5, HOLD, Reset::ON)}
    , {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT,  CNT(0, Duty::D3_4, 0, EnvDir::DEC, 15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::F5, HOLD, Reset::ON)}
    , {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT,  CNT(0, Duty::D3_4, 0, EnvDir::DEC, 15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::D5, HOLD, Reset::ON)}
    , {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT,  CNT(0, Duty::D3_4, 0, EnvDir::DEC, 15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::Eb5, HOLD, Reset::ON)}
    , {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT,  CNT(0, Duty::D3_4, 0, EnvDir::DEC, 15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::D5, HOLD, Reset::ON)}
    , {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT,  CNT(0, Duty::D3_4, 0, EnvDir::DEC, 15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::F5, HOLD, Reset::ON)}
    , {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT,  CNT(0, Duty::D3_4, 0, EnvDir::DEC, 15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::D5, HOLD, Reset::ON)}
    , {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT,  CNT(0, Duty::D3_4, 0, EnvDir::DEC, 15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::Eb5, HOLD, Reset::ON)}
    , {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT,  CNT(0, Duty::D3_4, 0, EnvDir::DEC, 0)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::Eb5, TIMED, Reset::ON)}
    , {Cmd::WAIT,1}
};

export const Track factory_notes[4][6] = {
    { {Cmd::SQR_2_CNT,  CNT(0, Duty::D3_4, 0, EnvDir::DEC, 15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::D5, HOLD, Reset::ON)}
    , {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT,  CNT(0, Duty::D3_4, 0, EnvDir::DEC, 0)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::D5, TIMED, Reset::ON)}
    , {Cmd::WAIT,1} 
    },
    { {Cmd::SQR_2_CNT,  CNT(0, Duty::D3_4, 0, EnvDir::DEC, 15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::Eb5, HOLD, Reset::ON)}
    , {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT,  CNT(0, Duty::D3_4, 0, EnvDir::DEC, 0)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::Eb5, TIMED, Reset::ON)}
    , {Cmd::WAIT,1} 
    },
    { {Cmd::SQR_2_CNT,  CNT(0, Duty::D3_4, 0, EnvDir::DEC, 15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::E5, HOLD, Reset::ON)}
    , {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT,  CNT(0, Duty::D3_4, 0, EnvDir::DEC, 0)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::E5, TIMED, Reset::ON)}
    , {Cmd::WAIT,1} 
    },
    { {Cmd::SQR_2_CNT,  CNT(0, Duty::D3_4, 0, EnvDir::DEC, 15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::F5, HOLD, Reset::ON)}
    , {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT,  CNT(0, Duty::D3_4, 0, EnvDir::DEC, 0)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::F5, TIMED, Reset::ON)}
    , {Cmd::WAIT,1} 
    }
};

} // namespace music
} // namespace game
