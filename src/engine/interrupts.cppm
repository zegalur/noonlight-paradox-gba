/*!****************************************************************************
 * @file interrupts.cppm
 * @brief GBA interrupts.
 *******************************************************************************/        

module;
export module engine:interrupts;

import :types;

namespace gba {

export using IFuncPtr = void(*)(void);


// pointer to a IRQ handler (interrupt service routine)
export inline volatile IFuncPtr& REG_ISR_MAIN = 
        *reinterpret_cast<IFuncPtr*>(0x03007FFC);


// interrupt masks for `REG_IE` and `REG_IF`
export enum IRQ_Masks {
    IRQ_VBLANK = 1 << 0,
    IRQ_HBLANK = 1 << 1,
    IRQ_VCOUNT = 1 << 2,
	IRQ_TIMER0 = 1 << 3,
	IRQ_TIMER1 = 1 << 4,
	IRQ_TIMER2 = 1 << 5,
	IRQ_TIMER3 = 1 << 6,
	IRQ_SERIAL = 1 << 7,
	IRQ_DMA0 = 1 << 8,
	IRQ_DMA1 = 1 << 9,
	IRQ_DMA2 = 1 << 10,
	IRQ_DMA3 = 1 << 11,
	IRQ_KEYPAD = 1 << 12,
	IRQ_GAMEPAK = 1 << 13,
};

} // namespace gba
