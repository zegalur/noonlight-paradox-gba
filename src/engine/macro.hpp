/*!****************************************************************************
 * @file macro.cppm
 * @brief Useful general purpose macros.
 *        Based on information from: https://gbadev.net/tonc/
 *******************************************************************************/

#ifndef MACRO_HPP
#define MACRO_HPP

#define EWRAM_DATA __attribute__((section(".ewram")))
#define IWRAM_DATA __attribute__((section(".iwram")))
#define EWRAM_BSS __attribute__((section(".sbss")))

#define EWRAM_CODE __attribute__((section(".ewram"), long_call))
#define IWRAM_CODE __attribute__((section(".iwram"), long_call))

#define ALIGN(n)    __attribute__((aligned(n)))
#define PACKED      __attribute__((packed))


// BIOS calls

#if defined(__thumb__)
#define swi_call(x)   asm volatile("swi\t"#x ::: "r0", "r1", "r2", "r3")
#elif defined(__arm__) || defined(__aarch64__)
#define swi_call(x)   asm volatile("swi\t"#x"<<16" ::: "r0", "r1", "r2", "r3")
#else
#define swi_call(x)   /* do nothing */
#endif


// other

#define DEFINE_ENUM_OR(T,B) inline T operator|(const T a, const T b) \
        { return static_cast<T>(static_cast<B>(a) | static_cast<B>(b)); }

#endif
