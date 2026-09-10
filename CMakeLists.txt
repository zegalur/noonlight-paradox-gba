cmake_minimum_required(VERSION 3.29)

project(GbaGame 
    VERSION 0.0.0 
    DESCRIPTION "A small GBA game for GBA Jam 2026 (by zegalur)." 
    LANGUAGES CXX ASM)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)


# --------------------------------- Options --------------------------------- #

add_library(gba_compiler_flags INTERFACE)

target_compile_features(gba_compiler_flags INTERFACE cxx_std_23)
target_compile_features(gba_compiler_flags INTERFACE cxx_constexpr)
target_compile_features(gba_compiler_flags INTERFACE cxx_noexcept)

target_compile_options(gba_compiler_flags INTERFACE -fno-rtti)
target_compile_options(gba_compiler_flags INTERFACE -nostdlib)
target_compile_options(gba_compiler_flags INTERFACE -nodefaultlibs)
target_compile_options(gba_compiler_flags INTERFACE -ffreestanding)
target_compile_options(gba_compiler_flags INTERFACE -fno-exceptions)
target_compile_options(gba_compiler_flags INTERFACE
    -Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused)

#target_compile_options(gba_compiler_flags INTERFACE -Werror)

#target_link_options(gba_compiler_flags INTERFACE -nostdlib)
#target_link_options(gba_compiler_flags INTERFACE -nodefaultlibs)
target_link_options(gba_compiler_flags INTERFACE -ffreestanding)

if(NOT CLANG)
    set(PYTHON $ENV{PYTHON})
    target_link_options(gba_compiler_flags INTERFACE --entry=main)
    target_compile_options(gba_compiler_flags INTERFACE -g;-O2;-s)
    target_compile_options(gba_compiler_flags INTERFACE -fmodules-ts)
    target_compile_options(gba_compiler_flags INTERFACE
        -mthumb-interwork;-mthumb;-mcpu=arm7tdmi;-mtune=arm7tdmi)
else()
    set(PYTHON python)
endif()


# ------------------------------ Game Engine -------------------------------- #

add_library(engine)
target_link_libraries(engine PRIVATE gba_compiler_flags)
target_sources(engine PUBLIC FILE_SET CXX_MODULES FILES 
    src/engine/engine.cppm
    src/engine/types.cppm
    src/engine/options.cppm
    src/engine/memory.cppm
    src/engine/video.cppm
    src/engine/graphics.cppm
    src/engine/resources.cppm
    src/engine/interrupts.cppm
    src/engine/sound.cppm
    src/engine/input.cppm
    src/engine/font.cppm
    src/engine/tilemap.cppm
    src/engine/tilemask.cppm
    src/engine/mem_allocator.cppm
    src/engine/obj_allocator.cppm
    src/engine/sprites.cppm
    src/engine/subgame.cppm
    src/engine/collision.cppm
    src/engine/components/spatial.cppm
    src/engine/components/visual.cppm
    src/engine/components/health.cppm
    src/engine/components/damage.cppm
    src/engine/components/action.cppm
    src/engine/components/animated.cppm
    src/engine/components/collision.cppm
    src/engine/systems/damager.cppm
    src/engine/systems/director.cppm
    src/engine/systems/animator.cppm
    src/engine/systems/collider.cppm
)

# add auto-generated notes table
set(GEN_NOTES src/tools/gen_notes.py)
set(NOTES_CPP notes.cppm)
configure_file(${GEN_NOTES} ${GEN_NOTES} COPYONLY)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${NOTES_CPP}
    COMMAND ${PYTHON} ARGS ${GEN_NOTES} DEPENDS ${GEN_NOTES} VERBATIM)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/${NOTES_CPP} 
    PROPERTIES GENERATED TRUE)
target_sources(engine PUBLIC FILE_SET CXX_MODULES 
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${NOTES_CPP})


# ------------------------------ Game Sources ------------------------------- #

add_executable(game)
set_target_properties(game PROPERTIES OUTPUT_NAME "game")
target_link_libraries(game PRIVATE gba_compiler_flags)
target_link_libraries(game PRIVATE engine)

target_sources(game PUBLIC FILE_SET CXX_MODULES FILES 
    src/game/options.cppm
    src/game/fonts.cppm
    src/game/text.cppm
    src/game/data.cppm
    src/game/player_stats.cppm
    src/game/player_gui.cppm
    src/game/player.cppm
    src/game/particles.cppm
    src/game/music/intro.cppm
    src/game/music/common.cppm
    src/game/music/factory.cppm
    src/game/entities/key.cppm
    src/game/entities/mine.cppm
    src/game/entities/izone.cppm
    src/game/entities/cargo.cppm
    src/game/entities/spider.cppm
    src/game/entities/switch.cppm
    src/game/entities/battery.cppm
    src/game/states/intro.cppm
    src/game/states/menu_template.cppm
    src/game/states/language_selection.cppm
    src/game/states/main_menu.cppm
    src/game/maps/map_template.cppm
    src/game/maps/dialogue_box.cppm
    src/game/maps/factory_00.cppm
    src/game/maps/factory_01.cppm
    src/game/game.cppm
)
target_sources(game PRIVATE src/main.cpp)

add_subdirectory(data)


# ---------------------------------- Other ---------------------------------- #

#add_library(system)
#target_link_libraries(system PRIVATE gba_compiler_flags)
#target_link_libraries(engine PUBLIC system)
#target_sources(system PUBLIC src/engine/system.cpp)

if(NOT CLANG)
    #target_link_options(system PRIVATE -specs=gba.specs)
    target_link_options(engine PRIVATE -specs=gba.specs)
    target_link_options(data PRIVATE -specs=gba.specs)
    target_link_options(game PRIVATE -specs=gba.specs)
endif()

