# Game data auto-generation

add_library(data)

target_link_libraries(data PUBLIC engine)
target_link_libraries(data PUBLIC gba_compiler_flags)
target_link_libraries(game PUBLIC data)

if(CLANG)
    set(PYTHON python)
    set(GIMP gimp-console)
else()
    set(PYTHON $ENV{PYTHON})
    set(GIMP $ENV{GIMPCONSOLE})
endif()

target_sources(data PUBLIC FILE_SET CXX_MODULES 
               FILES map_data.cppm)

set(C_UTILS ../src/tools/converter_utils.py)
configure_file(${C_UTILS} ${C_UTILS} COPYONLY)


# ------------------------------- Image Data -------------------------------- #

set(IMG_DIR img)
set(IMG_TO_CPP ../src/tools/img_to_cpp.py)
configure_file(${IMG_TO_CPP} ${IMG_TO_CPP} COPYONLY)

# NOTE: Append new entries in pairs "name" "additional arguments"

list(APPEND PNG_FILES items_palette)
list(APPEND PNG_ARGS "-po")

list(APPEND PNG_FILES player_gui)
list(APPEND PNG_ARGS "-0")

list(APPEND PNG_FILES dialogue_box_tiles)
list(APPEND PNG_ARGS "-ko")

list(APPEND PNG_FILES menu_arrow)
list(APPEND PNG_ARGS "-0")

list(APPEND PNG_FILES mmenu_wave)
list(APPEND PNG_ARGS "-s")

list(APPEND PNG_FILES flag_uk)
list(APPEND PNG_ARGS "-0")

list(APPEND PNG_FILES flag_uk_gray)
list(APPEND PNG_ARGS "-po")

list(APPEND PNG_FILES flag_ua)
list(APPEND PNG_ARGS "-0")

list(APPEND PNG_FILES flag_ua_gray)
list(APPEND PNG_ARGS "-po")

list(APPEND PNG_FILES flag_ja)
list(APPEND PNG_ARGS "-0")

list(APPEND PNG_FILES flag_ja_gray)
list(APPEND PNG_ARGS "-po")

list(APPEND PNG_FILES shadow)
list(APPEND PNG_ARGS "-0")

foreach(F A IN ZIP_LISTS PNG_FILES PNG_ARGS)
    configure_file(${IMG_DIR}/${F}.png ${IMG_DIR}/${F}.png COPYONLY)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${IMG_DIR}/${F}.cppm
        COMMAND ${PYTHON}
        ARGS    ${IMG_TO_CPP} ${A} ${F} ${IMG_DIR}/${F}.png ${IMG_DIR}/${F}.cppm
        DEPENDS ${IMG_TO_CPP} ${C_UTILS} ${IMG_DIR}/${F}.png
        VERBATIM)
    set_source_files_properties(
        ${CMAKE_CURRENT_BINARY_DIR}/${IMG_DIR}/${F}.cppm
        PROPERTIES GENERATED TRUE)
    target_sources(data PUBLIC FILE_SET CXX_MODULES 
        BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${IMG_DIR}/${F}.cppm)
endforeach()


# --------------------------------- Fonts ----------------------------------- #

set(FONT_DIR fonts)
set(TTF_TO_CPP ../src/tools/ttf_to_cpp.py)
configure_file(${TTF_TO_CPP} ${TTF_TO_CPP} COPYONLY)

# NOTE: Append new entries in tuples

list(APPEND FONT_NAMES cairopixel)
list(APPEND FONT_SIZES 16)
list(APPEND FONT_FILES Cairopixel)
list(APPEND FONT_CHARSETS european)
list(APPEND FONT_ALIGN_WITH "        ABC.T()_10ax.")

list(APPEND FONT_NAMES nosutaru)
list(APPEND FONT_SIZES 12)
list(APPEND FONT_FILES Nosutaru)
list(APPEND FONT_CHARSETS japanese)
list(APPEND FONT_ALIGN_WITH "     ABCDabcd")

foreach(N S F C A IN ZIP_LISTS FONT_NAMES FONT_SIZES FONT_FILES FONT_CHARSETS FONT_ALIGN_WITH)
    configure_file(${FONT_DIR}/${F}.ttf ${FONT_DIR}/${F}.ttf COPYONLY)
    configure_file(${FONT_DIR}/${C}.txt ${FONT_DIR}/${C}.txt COPYONLY)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${FONT_DIR}/${N}.cppm
        COMMAND ${PYTHON}
        ARGS    ${TTF_TO_CPP} ${N} ${S} ${FONT_DIR}/${F}.ttf 
                ${FONT_DIR}/${C}.txt ${FONT_DIR}/${N}.cppm -aw ${A}
        DEPENDS ${TTF_TO_CPP} ${C_UTILS} ${FONT_DIR}/${N}.ttf ${FONT_DIR}/${C}.txt
        VERBATIM)
    set_source_files_properties(
        ${CMAKE_CURRENT_BINARY_DIR}/${FONT_DIR}/${N}.cppm
        PROPERTIES GENERATED TRUE)
    target_sources(data PUBLIC FILE_SET CXX_MODULES 
        BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${FONT_DIR}/${N}.cppm)
endforeach()


# ------------------------------ Backgrounds -------------------------------- #

set(BG_DIR backgrounds)
set(MAP_TO_CPP ../src/tools/map_to_cpp.py)
configure_file(${MAP_TO_CPP} ${MAP_TO_CPP} COPYONLY)

set(B0_PCOUNT 1)
set(B0_POFFSET 12)
set(B0_TCOUNT 256)
set(B0_TOFFSET 0)

set(B1_PCOUNT 2)
set(B1_POFFSET 13)
set(B1_TCOUNT 256)
set(B1_TOFFSET 256)

# NOTE: Append new entries in tuples

list(APPEND BG_DIRS "blue_mountains")
list(APPEND BG_NAMES "blue_mountains_b0")
list(APPEND BG_GIMP_COLOR_COUNT 14)
list(APPEND BG_GIMP_DITHER_TYPE 0)
list(APPEND BG_PAL_COUNT ${B0_PCOUNT})
list(APPEND BG_PAL_OFFSET ${B0_POFFSET})
list(APPEND BG_TILE_COUNT ${B0_TCOUNT})
list(APPEND BG_TILE_OFFSET ${B0_TOFFSET})
list(APPEND BG_ARGS_1 "-ms")
list(APPEND BG_ARGS_2 "0.03")
list(APPEND BG_ARGS_3 "-mt2")
list(APPEND BG_ARGS_4 "0.09")
list(APPEND BG_ARGS_5 "0.09")
list(APPEND BG_ARGS_6 "0.01")
list(APPEND BG_ARGS_7 "-0")
#list(APPEND BG_ARGS_1 "-ms")
#list(APPEND BG_ARGS_2 "0.09")
#list(APPEND BG_ARGS_3 "-mt2")
#list(APPEND BG_ARGS_4 "1.29")
#list(APPEND BG_ARGS_5 "1.29")
#list(APPEND BG_ARGS_6 "0.01")
#list(APPEND BG_ARGS_7 "-0")

list(APPEND BG_DIRS "blue_mountains")
list(APPEND BG_NAMES "blue_mountains_b1")
list(APPEND BG_GIMP_COLOR_COUNT 54)
list(APPEND BG_GIMP_DITHER_TYPE 0)
list(APPEND BG_PAL_COUNT ${B1_PCOUNT})
list(APPEND BG_PAL_OFFSET ${B1_POFFSET})
list(APPEND BG_TILE_COUNT ${B1_TCOUNT})
list(APPEND BG_TILE_OFFSET ${B1_TOFFSET})
list(APPEND BG_ARGS_1 "-ms")
list(APPEND BG_ARGS_2 "0.07")
list(APPEND BG_ARGS_3 "-mt2")
list(APPEND BG_ARGS_4 "0.65")
list(APPEND BG_ARGS_5 "0.65")
list(APPEND BG_ARGS_6 "0.02")
list(APPEND BG_ARGS_7 "-0")

list(APPEND BG_DIRS "intro")
list(APPEND BG_NAMES "intro_02_fg")
list(APPEND BG_GIMP_COLOR_COUNT 90)
list(APPEND BG_GIMP_DITHER_TYPE 0)
list(APPEND BG_PAL_COUNT 5)
list(APPEND BG_PAL_OFFSET 0)
list(APPEND BG_TILE_COUNT 500)
list(APPEND BG_TILE_OFFSET 0)
list(APPEND BG_ARGS_1 "-m")
list(APPEND BG_ARGS_2 "-ms")
list(APPEND BG_ARGS_3 "0.16")
list(APPEND BG_ARGS_4 "-z")
list(APPEND BG_ARGS_5 "-0")
list(APPEND BG_ARGS_6 "-0")
list(APPEND BG_ARGS_7 "-0")

list(APPEND BG_DIRS "intro")
list(APPEND BG_NAMES "intro_02_bg")
list(APPEND BG_GIMP_COLOR_COUNT 90)
list(APPEND BG_GIMP_DITHER_TYPE 0)
list(APPEND BG_PAL_COUNT 1)
list(APPEND BG_PAL_OFFSET 5)
list(APPEND BG_TILE_COUNT 256)
list(APPEND BG_TILE_OFFSET 0)
list(APPEND BG_ARGS_1 "-m")
list(APPEND BG_ARGS_2 "-mt")
list(APPEND BG_ARGS_3 "0.08")
list(APPEND BG_ARGS_4 "0.08")
list(APPEND BG_ARGS_5 "0.08")
list(APPEND BG_ARGS_6 "0.08")
list(APPEND BG_ARGS_7 "0.001")

list(APPEND BG_DIRS "intro")
list(APPEND BG_NAMES "intro_03_spaceship")
list(APPEND BG_GIMP_COLOR_COUNT 90)
list(APPEND BG_GIMP_DITHER_TYPE 0)
list(APPEND BG_PAL_COUNT 5)
list(APPEND BG_PAL_OFFSET 0)
list(APPEND BG_TILE_COUNT 500)
list(APPEND BG_TILE_OFFSET 0)
list(APPEND BG_ARGS_1 "-m")
list(APPEND BG_ARGS_2 "-ms")
list(APPEND BG_ARGS_3 "0.19")
list(APPEND BG_ARGS_4 "-z")
list(APPEND BG_ARGS_5 "-0")
list(APPEND BG_ARGS_6 "-0")
list(APPEND BG_ARGS_7 "-0")

foreach(DR N C D PC PO TC TO A1 A2 A3 A4 A5 A6 A7 IN ZIP_LISTS BG_DIRS BG_NAMES BG_GIMP_COLOR_COUNT BG_GIMP_DITHER_TYPE BG_PAL_COUNT BG_PAL_OFFSET BG_TILE_COUNT BG_TILE_OFFSET BG_ARGS_1 BG_ARGS_2 BG_ARGS_3 BG_ARGS_4 BG_ARGS_5 BG_ARGS_6 BG_ARGS_7)

    configure_file(${BG_DIR}/${DR}/${N}.png ${BG_DIR}/${DR}/${N}.png COPYONLY)

    # GIMP pre-processing
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${BG_DIR}/${DR}/${N}_gimp.png
        COMMAND ${GIMP}
        ARGS    --batch-interpreter=plug-in-script-fu-eval -b "(let* ((in-img (car (file-png-load 1 \"${BG_DIR}/${DR}/${N}.png\")))) (gimp-image-convert-indexed in-img ${D} 0 ${C} FALSE FALSE \"\") (file-png-export 1 in-img \"${BG_DIR}/${DR}/${N}_gimp.png\"))" -b "(gimp-quit 0)"
        DEPENDS ${BG_DIR}/${DR}/${N}.png
        VERBATIM)

    # convert to cpp
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${BG_DIR}/${DR}/${N}.cppm
        COMMAND ${PYTHON}
        ARGS    ${MAP_TO_CPP} ${N}
                ${BG_DIR}/${DR}/${N}_gimp.png
                ${BG_DIR}/${DR}/${N}.cppm
                -e -pc ${PC} -po ${PO} -tc ${TC} -to ${TO}
                -s ${BG_DIR}/${DR}/${N}_prev.png 
                ${A1} ${A2} ${A3} ${A4} ${A5} ${A6} ${A7} 
        DEPENDS ${MAP_TO_CPP} ${C_UTILS} ${BG_DIR}/${DR}/${N}_gimp.png
        VERBATIM)

    set_source_files_properties(
        ${CMAKE_CURRENT_BINARY_DIR}/${BG_DIR}/${DR}/${N}.cppm
        PROPERTIES GENERATED TRUE)

    target_sources(data PUBLIC FILE_SET CXX_MODULES 
        BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${BG_DIR}/${DR}/${N}.cppm)

endforeach()


# ---------------------------------- Maps ----------------------------------- #

set(MAPS_DIR maps)
set(MAP_PAL_COUNT 12)

# Already set, skip
#set(MAP_TO_CPP ../src/tools/map_to_cpp.py)
#configure_file(${MAP_TO_CPP} ${MAP_TO_CPP} COPYONLY)

set(MAP_INFO_TO_CPP ../src/tools/map_info_to_cpp.py)
configure_file(${MAP_INFO_TO_CPP} ${MAP_INFO_TO_CPP} COPYONLY)

# NOTE: Append new entries in tuples

list(APPEND MAP_NAMES "factory_00") # NOTE: add color cor in gimp
list(APPEND MAP_GIMP_COLOR_COUNT 35) # 30 31 32 (35)
list(APPEND MAP_GIMP_DITHER_TYPE 0) # (0)
list(APPEND MAP_SIMP_PAL 0.08) # 0.1 0.08 0.06 0.01 (0.08)
list(APPEND MAP_MERGE_SIM 0.127) # 0.127 (0.127)

list(APPEND MAP_NAMES "factory_01")
list(APPEND MAP_GIMP_COLOR_COUNT 90)
list(APPEND MAP_GIMP_DITHER_TYPE 3)
list(APPEND MAP_SIMP_PAL 0.1)
list(APPEND MAP_MERGE_SIM 0.127)

foreach(M C D P SP MS IN ZIP_LISTS MAP_NAMES MAP_GIMP_COLOR_COUNT MAP_GIMP_DITHER_TYPE MAP_EXPORT_PARAMS MAP_SIMP_PAL MAP_MERGE_SIM)

    configure_file(${MAPS_DIR}/${M}/color.png ${MAPS_DIR}/${M}/color.png COPYONLY)
    configure_file(${MAPS_DIR}/${M}/depth_mask.png ${MAPS_DIR}/${M}/depth_mask.png COPYONLY)

    # GIMP pre-processing
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${MAPS_DIR}/${M}/color_gimp.png
        COMMAND ${GIMP}
        ARGS    --batch-interpreter=plug-in-script-fu-eval -b "(let* ((in-img (car (file-png-load 1 \"${MAPS_DIR}/${M}/color.png\")))) (gimp-image-convert-indexed in-img ${D} 0 ${C} FALSE FALSE \"\") (file-png-export 1 in-img \"${MAPS_DIR}/${M}/color_gimp.png\"))" -b "(gimp-quit 0)"
        DEPENDS ${MAPS_DIR}/${M}/color.png
        VERBATIM)

    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${MAPS_DIR}/${M}/${M}.cppm
        COMMAND ${PYTHON}
        ARGS    ${MAP_TO_CPP} ${M} 
                ${MAPS_DIR}/${M}/color_gimp.png 
                ${MAPS_DIR}/${M}/${M}.cppm 
                -e -pc ${MAP_PAL_COUNT} -sp ${SP} -ms ${MS} -z
                -s ${MAPS_DIR}/${M}/color_prev.png 
                -dm ${MAPS_DIR}/${M}/depth_mask.png 
        DEPENDS ${MAP_TO_CPP} ${C_UTILS} 
                ${MAPS_DIR}/${M}/color_gimp.png
                ${MAPS_DIR}/${M}/depth_mask.png
        VERBATIM)

    set_source_files_properties(
        ${CMAKE_CURRENT_BINARY_DIR}/${MAPS_DIR}/${M}/${M}.cppm
        PROPERTIES GENERATED TRUE)

    target_sources(data PUBLIC FILE_SET CXX_MODULES 
        BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${MAPS_DIR}/${M}/${M}.cppm)


    # generate map info module

    configure_file(${MAPS_DIR}/${M}/info.txt ${MAPS_DIR}/${M}/info.txt COPYONLY)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${MAPS_DIR}/${M}/${M}_info.cppm
        COMMAND ${PYTHON}
        ARGS    ${MAP_INFO_TO_CPP} ${M} 
                ${MAPS_DIR}/${M}/info.txt 
                ${MAPS_DIR}/${M}/${M}_info.cppm 
        DEPENDS ${MAP_INFO_TO_CPP} ${C_UTILS} ${MAPS_DIR}/${M}/info.txt
        VERBATIM)

    set_source_files_properties(
        ${CMAKE_CURRENT_BINARY_DIR}/${MAPS_DIR}/${M}/${M}_info.cppm
        PROPERTIES GENERATED TRUE)

    target_sources(data PUBLIC FILE_SET CXX_MODULES 
        BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${MAPS_DIR}/${M}/${M}_info.cppm)


    # generate collision map

    configure_file(${MAPS_DIR}/${M}/shapes.png ${MAPS_DIR}/${M}/shapes.png COPYONLY)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${MAPS_DIR}/${M}/${M}_shapes.cppm
        COMMAND ${PYTHON}
        ARGS    ${MAP_TO_CPP} -cm ${M}_shapes
                ${MAPS_DIR}/${M}/shapes.png
                ${MAPS_DIR}/${M}/${M}_shapes.cppm
        DEPENDS ${MAP_TO_CPP} ${C_UTILS}
                ${MAPS_DIR}/${M}/shapes.png
        VERBATIM)

    set_source_files_properties(
        ${CMAKE_CURRENT_BINARY_DIR}/${MAPS_DIR}/${M}/${M}_shapes.cppm
        PROPERTIES GENERATED TRUE)

    target_sources(data PUBLIC FILE_SET CXX_MODULES 
        BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${MAPS_DIR}/${M}/${M}_shapes.cppm)

endforeach()


# ----------------------------- Animation Data ------------------------------ #

set(ANIM_DIR anim)
set(ANIM_TO_CPP ../src/tools/anim_to_cpp.py)
configure_file(${ANIM_TO_CPP} ${ANIM_TO_CPP} COPYONLY)

if(CLANG)
    set(CP_CMD copy)
else()
    set(CP_CMD cp)
endif()

# NOTE: Append new entries in pairs "name" "additional arguments"

list(APPEND ANIM_DIRS "mainc")
list(APPEND ANIM_C1 "-ex")
list(APPEND ANIM_A1 "64")
list(APPEND ANIM_PAL "U")

list(APPEND ANIM_DIRS "mmenu_bg")
list(APPEND ANIM_C1 "-0")
list(APPEND ANIM_A1 "-0")
list(APPEND ANIM_PAL "U")

list(APPEND ANIM_DIRS "green_battery")
list(APPEND ANIM_C1 "-np")
list(APPEND ANIM_A1 "-0")
list(APPEND ANIM_PAL "ITEM")

list(APPEND ANIM_DIRS "shield_burst")
list(APPEND ANIM_C1 "-np")
list(APPEND ANIM_A1 "-0")
list(APPEND ANIM_PAL "ITEM")

list(APPEND ANIM_DIRS "bubble")
list(APPEND ANIM_C1 "-0")
list(APPEND ANIM_A1 "-0")
list(APPEND ANIM_PAL "U")

list(APPEND ANIM_DIRS "a_button")
list(APPEND ANIM_C1 "-0")
list(APPEND ANIM_A1 "-0")
list(APPEND ANIM_PAL "U")

list(APPEND ANIM_DIRS "b_button")
list(APPEND ANIM_C1 "-0")
list(APPEND ANIM_A1 "-0")
list(APPEND ANIM_PAL "U")

list(APPEND ANIM_DIRS "d_arrow")
list(APPEND ANIM_C1 "-0")
list(APPEND ANIM_A1 "-0")
list(APPEND ANIM_PAL "U")

list(APPEND ANIM_DIRS "i_respawn")
list(APPEND ANIM_C1 "-np")
list(APPEND ANIM_A1 "-0")
list(APPEND ANIM_PAL "ITEM")

list(APPEND ANIM_DIRS "sand")
list(APPEND ANIM_C1 "-np")
list(APPEND ANIM_A1 "-0")
list(APPEND ANIM_PAL "ITEM")

list(APPEND ANIM_DIRS "izone")
list(APPEND ANIM_C1 "-np") # uses mine_01 palette
list(APPEND ANIM_A1 "-0")
list(APPEND ANIM_PAL "U")

list(APPEND ANIM_DIRS "i_sight")
list(APPEND ANIM_C1 "-np") # uses player's palette
list(APPEND ANIM_A1 "-0")
list(APPEND ANIM_PAL "U")

list(APPEND ANIM_DIRS "sparkle")
list(APPEND ANIM_C1 "-np") # uses player's palette
list(APPEND ANIM_A1 "-0")
list(APPEND ANIM_PAL "U")

list(APPEND ANIM_DIRS "p_kira")
list(APPEND ANIM_C1 "-0")
list(APPEND ANIM_A1 "-0")
list(APPEND ANIM_PAL "U")

list(APPEND ANIM_DIRS "p_akiba")
list(APPEND ANIM_C1 "-0")
list(APPEND ANIM_A1 "-0")
list(APPEND ANIM_PAL "U")

list(APPEND ANIM_DIRS "p_computer")
list(APPEND ANIM_C1 "-0")
list(APPEND ANIM_A1 "-0")
list(APPEND ANIM_PAL "U")

list(APPEND ANIM_DIRS "explosion_01")
list(APPEND ANIM_C1 "-0")
list(APPEND ANIM_A1 "-0")
list(APPEND ANIM_PAL "U")

list(APPEND ANIM_DIRS "mine_01")
list(APPEND ANIM_C1 "-0")
list(APPEND ANIM_A1 "-0")
list(APPEND ANIM_PAL "U")

list(APPEND ANIM_DIRS "spider")
list(APPEND ANIM_C1 "-np") # use mine_01 palette
list(APPEND ANIM_A1 "-0")
list(APPEND ANIM_PAL "U")

list(APPEND ANIM_DIRS "cargo")
list(APPEND ANIM_C1 "-np") # use mine_01 palette
list(APPEND ANIM_A1 "-0")
list(APPEND ANIM_PAL "U")

list(APPEND ANIM_DIRS "switch_01")
list(APPEND ANIM_C1 "-np") # use mine_01 palette
list(APPEND ANIM_A1 "-0")
list(APPEND ANIM_PAL "U")

list(APPEND ANIM_DIRS "key")
list(APPEND ANIM_C1 "-0") # TODO: use player_gui palette
list(APPEND ANIM_A1 "-0")
list(APPEND ANIM_PAL "U")

set(ITEM_PAL "${CMAKE_CURRENT_SOURCE_DIR}/${IMG_DIR}/items_palette.png")
set(ITEM_PAL "${CMAKE_CURRENT_SOURCE_DIR}/${IMG_DIR}/items_palette.png")

foreach(D C1 A1 P IN ZIP_LISTS ANIM_DIRS ANIM_C1 ANIM_A1 ANIM_PAL)
    if(P STREQUAL "ITEM")
        add_custom_command(
            OUTPUT  ${ANIM_DIR}/${D}/palette.png
            COMMAND ${CP_CMD}
            ARGS    ${ITEM_PAL} ${ANIM_DIR}/${D}/palette.png
            DEPENDS ${ITEM_PAL}
            VERBATIM)
        set_source_files_properties(
            ${CMAKE_CURRENT_BINARY_DIR}/${ANIM_DIR}/${D}/palette.png
            PROPERTIES GENERATED TRUE)
    endif()
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/${ANIM_DIR}/${D}"
         DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/${ANIM_DIR}")
    file(GLOB_RECURSE DFILES "${CMAKE_CURRENT_SOURCE_DIR}/${ANIM_DIR}/${D}")
    list(APPEND DFILES ${ANIM_DIR}/${D}/palette.png)
    list(APPEND DFILES ${ANIM_TO_CPP})
    list(APPEND DFILES ${C_UTILS})
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${ANIM_DIR}/${D}.cppm
        COMMAND ${PYTHON}
        ARGS    ${ANIM_TO_CPP} ${D} ${ANIM_DIR}/${D} ${ANIM_DIR}/${D}.cppm ${C1} ${A1}
        DEPENDS ${DFILES}
        VERBATIM)
    set_source_files_properties(
        ${CMAKE_CURRENT_BINARY_DIR}/${ANIM_DIR}/${D}.cppm
        PROPERTIES GENERATED TRUE)
    target_sources(data PUBLIC FILE_SET CXX_MODULES 
        BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${ANIM_DIR}/${D}.cppm)
endforeach()

