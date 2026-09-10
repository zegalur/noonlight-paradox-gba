
#define MAP_ENTITIES(var) \
        (var).E_COUNT, \
        (var).spatials, \
        (var).visuals, \
        (var).health, \
        (var).damage, \
        (var).actions, \
        (var).collision

#define MAP_BX_LAYER(name) \
        name ## _palettes, \
        name ## _tcount, \
        name ## _tiles, \
        name ## _tsize_x, \
        name ## _tsize_y, \
        name ## _sb_array

#define MAP_MAIN_LAYER(name) \
        name ## _pcount, \
        name ## _palettes, \
        name ## _tiles, \
        name ## _tsize_x, \
        name ## _tsize_y, \
        name ## _sb_array, \
        name ## _mtiles, \
        name ## _sbm_array

#define MAP_COLLISION_MAP(name) \
        name ## _shapes_tiles, \
        name ## _shapes_sb_array, \
        name ## _shapes_tsize_x, \
        name ## _shapes_tsize_y, \
        name ## _shapes_palettes

#define SIZE_CONSTANTS(bx, main) \
        from_float(float(bx ## _tsize_x) / float(main ## _tsize_x)), \
        from_float(float(bx ## _tsize_y) / float(main ## _tsize_y))

#define INTRO_MUSIC(track) \
        track##_ch_mask, track##_melody, track##_plen, len(track##_melody)
