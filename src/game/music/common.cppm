module;
export module game.music.common;

import engine;

using namespace gba;
using namespace gba::sound;

namespace game {
namespace music {

// ------------------------------- Pick Up --------------------------------- //

export constexpr u32 pick_up_ch_mask = 
          ChannelMask::LSQR2 | ChannelMask::RSQR2;

export constexpr auto pick_up_plen = PLEN(130, 2);

export const Track pick_up_track[] =
    { {Cmd::SQR_2_CNT,  CNT(0, Duty::D3_4, 0, EnvDir::DEC, 15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::Bb4, HOLD, Reset::ON)}
    , {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT,  CNT(0, Duty::D3_4, 0, EnvDir::DEC, 15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::C5, HOLD, Reset::ON)}
    , {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT,  CNT(0, Duty::D3_4, 0, EnvDir::DEC, 15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::D5, HOLD, Reset::ON)}
    , {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT,  CNT(0, Duty::D3_4, 0, EnvDir::DEC, 15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::Eb5, HOLD, Reset::ON)}
    , {Cmd::WAIT,1}
    , {Cmd::SQR_2_CNT,  CNT(0, Duty::D3_4, 0, EnvDir::DEC, 15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::F5, HOLD, Reset::ON)}
    , {Cmd::WAIT,3}
    , {Cmd::SQR_2_CNT,  CNT(0, Duty::D3_4, 0, EnvDir::DEC, 15)}
    , {Cmd::SQR_2_FREQ, FREQ(Note::F5, TIMED, Reset::ON)}
    , {Cmd::WAIT,1}
};

} // namespace music
} // namespace game
