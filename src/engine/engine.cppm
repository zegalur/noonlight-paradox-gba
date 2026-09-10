/*!****************************************************************************
 * @file engine.cppm
 * @brief A lightweight GBA game engine. Made for a GBA game jam.
 * The engine is written without OOP, exceptions, STL etc. It's meant to be
 * used as a very thin "middle-layer" between the game code and the bare metal.
 *******************************************************************************/        

module;
export module engine;

export import :types;
export import :options;
export import :memory;
export import :input;
export import :video;
export import :graphics;
export import :font;
export import :interrupts;
export import :resources;
export import :tilemap;
export import :tilemask;
export import :mem_allocator;
export import :obj_allocator;
export import :sprites;
export import :subgame;
export import :collision;
export import :notes;
export import :sound;

// common components
export import :spatial;
export import :visual;
export import :health;
export import :damage;
export import :action;
export import :comp_collision;

// common systems
export import :director;
export import :damager;
export import :collider;
