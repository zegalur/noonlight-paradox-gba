/*!****************************************************************************
 * @file mem_allocator.cppm
 * @brief Fast and VERY simple memory allocator.
 * WARNING: Make sure you have enough memory to work with!
 *******************************************************************************/

module;

#include <utility>

export module engine:mem_allocator;

import :types;

#include <new>

namespace gba {
namespace mem {

export template<int S> struct Bank {
    u32 cur_pos;
    u8 memory_bank[S];
};

export template<int S> void init(Bank<S>& data) {
    data.cur_pos = 0;
}

export template<typename T> consteval u32 sizeof_u32() {
    return clamp(sizeof(T) >> u32(2), u32(1), sizeof(T));
}

export template<int S, typename T, typename... Args> 
inline void alloc(Bank<S>& data, T* &ptr, Args&&... args) {
    ptr = reinterpret_cast<T*>(&data.memory_bank[data.cur_pos]);
    data.cur_pos += sizeof_u32<T>();
    new(ptr) T{std::forward(args)...}; // calling the constructor
}

export template<int S, typename T> 
inline void free(Bank<S>& data, T* ptr) {
    ptr->~T(); // calling the destructor
    data.cur_pos -= sizeof_u32<T>();
}

} // namespace mem
} // namespace gba
