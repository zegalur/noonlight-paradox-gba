/*!****************************************************************************
 * @file types.cppm
 * @brief The very basic data types.
 *******************************************************************************/        

module;
export module engine:types;

namespace gba {

// -------------------------- The most basic types ------------------------- //

export using u8 = unsigned char;
export using u16 = unsigned short;
export using u32 = unsigned int;

export using i8 = char8_t;
export using i16 = short;
export using i32 = int;

export using c32 = char32_t;

export template<typename T> constexpr T abs(const T x)
{ return x >= T(0) ? x : -x; }

export template<typename T> consteval T clamp(const T x, const T a, const T b)
{ return x >= b ? b : (x <= a ? a : x); }

export template<typename T> inline const T clampi(const T x, const T a, const T b)
{ return x >= b ? b : (x <= a ? a : x); }

export template<typename T> inline constexpr T min(const T a, const T b) {
    return (a <= b ? a : b);
}


// ------------------------------ Fixed-point ------------------------------ //

/// @brief Fixed-point precision rational number.
export using fix32 = i32;

/// @brief Fixed-point precision = the number of bits of the fractional part.
export constexpr i32 FPP = 8;

/// @brief Returns the signed integer part of the number.
export constexpr i32 intp(const fix32 x) { 
    return (x >> FPP); 
}

/// @brief Returns the unsigned fractional part of the number.
export constexpr i32 frac(const fix32 x) { 
    return (x & ((1 << FPP) - 1));
}

/// ...
export constexpr fix32 from_int(const i32 x) {
    return fix32(x << FPP);
}

export constexpr fix32 fix_mul(const fix32 a, const fix32 b) {
    return (a >> (FPP >> 1)) * (b >> (FPP >> 1));
}

export constexpr fix32 FIX32_ZERO = from_int(0);
export constexpr fix32 FIX32_ONE = from_int(1);

export consteval fix32 from_float(float x) {
    return fix32(i32(x * float(128.0)));
}

export inline fix32 mdistance(
        const fix32 x1, const fix32 y1,
        const fix32 x2, const fix32 y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}


// --------------------------------- Other --------------------------------- //

// std::same_as equivalent for -nostd builds
export template <typename A, typename B>
concept SameAs = __is_same(A, B) && __is_same(B, A);

export template <typename T, int N>
consteval u32 len(const T (&)[N]) 
{ return N; }

/// @brief multi-type variable
export union Var {
    u32 u;
    i32 i;
    fix32 f;
    bool b;
};

} // namespace gba
