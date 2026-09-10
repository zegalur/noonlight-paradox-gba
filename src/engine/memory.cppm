/*!****************************************************************************
 * @file memory.cppm
 * @brief GBA memory table.
 *        Based on information from: https://gbadev.net/tonc/
 *******************************************************************************/        

module;
export module engine:memory;

import :types;

namespace gba {

// memory sections

export constexpr auto MEM_IO = 0x04000000; // I/O registers
export constexpr auto MEM_BG_PAL = 0x05000000; // background palette RAM
export constexpr auto MEM_OBJ_PAL = 0x05000200; // object palette RAM
export constexpr auto MEM_VRAM = 0x06000000; // video RAM
export constexpr auto MEM_OAM = 0x07000000; // object attribute memory


// I/O register list

#define NEW_REG_16(name, addr) \
export inline volatile u16& name = *reinterpret_cast<volatile u16*>(addr)
#define NEW_REG_32(name, addr) \
export inline volatile u32& name = *reinterpret_cast<volatile u32*>(addr)

#define NEW_CREG_16(name, addr) \
export inline const volatile u16& name = *reinterpret_cast<const volatile u16*>(addr)
#define NEW_CREG_32(name, addr) \
export inline const volatile u32& name = *reinterpret_cast<const volatile u32*>(addr)

NEW_REG_16(REG_DISPCNT, MEM_IO + 0x0000); // display control
NEW_REG_16(REG_DISPSTAT, MEM_IO + 0x0004); // display status
NEW_CREG_16(REG_VCOUNT, MEM_IO + 0x0006); // scanline counter

NEW_REG_16(REG_BG0CNT, MEM_IO + 0x0008); // BG0 Control
NEW_REG_16(REG_BG1CNT, MEM_IO + 0x000a); // BG1 Control
NEW_REG_16(REG_BG2CNT, MEM_IO + 0x000c); // BG2 Control
NEW_REG_16(REG_BG3CNT, MEM_IO + 0x000e); // BG3 Control

NEW_REG_16(REG_BG0HOFS, MEM_IO + 0x0010); // BG0 X-Offset
NEW_REG_16(REG_BG0VOFS, MEM_IO + 0x0012); // BG0 Y-Offset

NEW_REG_16(REG_BG1HOFS, MEM_IO + 0x0014); // BG1 X-Offset
NEW_REG_16(REG_BG1VOFS, MEM_IO + 0x0016); // BG1 Y-Offset

NEW_REG_16(REG_BG2HOFS, MEM_IO + 0x0018); // BG2 X-Offset
NEW_REG_16(REG_BG2VOFS, MEM_IO + 0x001a); // BG2 Y-Offset

NEW_REG_16(REG_BG3HOFS, MEM_IO + 0x001c); // BG3 X-Offset
NEW_REG_16(REG_BG3VOFS, MEM_IO + 0x001e); // BG3 Y-Offset

NEW_CREG_16(REG_KEYINPUT, MEM_IO + 0x0130); // keypad register
NEW_REG_16(REG_KEYCNT, MEM_IO + 0x0132); // key control

NEW_REG_16(REG_IE, MEM_IO + 0x0200); // interrupt enable register
NEW_REG_16(REG_IF, MEM_IO + 0x0202); // interrupt flags register
NEW_REG_32(REG_IME, MEM_IO + 0x0208); // master interrupt control

NEW_REG_16(REG_IFBIOS, 0x03fffff8); // BIOS interrupt flags register

// sound (using the tonc naming)

NEW_REG_16(REG_SND1SWEEP,   MEM_IO + 0x0060); // #1 (SQR) sweep
NEW_REG_16(REG_SND1CNT,     MEM_IO + 0x0062); // #1 (SQR) control
NEW_REG_16(REG_SND1FREQ,    MEM_IO + 0x0064); // #1 (SQR) freq, on/off
//
NEW_REG_16(REG_SND2CNT,     MEM_IO + 0x0068); // #2 (SQR) control
NEW_REG_16(REG_SND2FREQ,    MEM_IO + 0x006C); // #2 (SQR) freq, on/off
//
NEW_REG_16(REG_SND3SEL,     MEM_IO + 0x0070); // #3 (WAVE) mode
NEW_REG_16(REG_SND3CNT,     MEM_IO + 0x0072); // #3 (WAVE) control
NEW_REG_16(REG_SND3FREQ,    MEM_IO + 0x0074); // #3 (WAVE) freq, on/off
//
NEW_REG_16(REG_SND4CNT,     MEM_IO + 0x0078); // #4 (NOISE) control
NEW_REG_16(REG_SND4FREQ,    MEM_IO + 0x007C); // #4 (NOISE) freq, on/off
//
NEW_REG_16(REG_SNDDMGCNT,   MEM_IO + 0x0080); // DMG master control
NEW_REG_16(REG_SNDDSCNT,    MEM_IO + 0x0082); // DSound master control
NEW_REG_16(REG_SNDSTAT,     MEM_IO + 0x0084); // sound status
NEW_REG_16(REG_SNDBIAS,     MEM_IO + 0x0088); // bias control

// timers

NEW_REG_16(REG_TM0D,    MEM_IO + 0x0100 + 0x4*0); // Timer 0 (data)
NEW_REG_16(REG_TM0CNT,  MEM_IO + 0x0102 + 0x4*0); // Timer 0 (control)

NEW_REG_16(REG_TM1D,    MEM_IO + 0x0100 + 0x4*1); // Timer 1 (data)
NEW_REG_16(REG_TM1CNT,  MEM_IO + 0x0102 + 0x4*1); // Timer 1 (control)

NEW_REG_16(REG_TM2D,    MEM_IO + 0x0100 + 0x4*2); // Timer 2 (data)
NEW_REG_16(REG_TM2CNT,  MEM_IO + 0x0102 + 0x4*2); // Timer 2 (control)

NEW_REG_16(REG_TM3D,    MEM_IO + 0x0100 + 0x4*3); // Timer 3 (data)
NEW_REG_16(REG_TM3CNT,  MEM_IO + 0x0102 + 0x4*3); // Timer 3 (control)


} // namespace gba
