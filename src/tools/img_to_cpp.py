import converter_utils as cu

from PIL import Image

import numpy as np
import argparse
import math

# create an argument parser

parser = argparse.ArgumentParser(
        description=
                "Converts the input image into a C++ file with"
                " ready-to-copy 8x8 tiles, palette etc."
        )

parser.add_argument('name'
        ,type=str
        ,help="The base name of the asset."
        )

parser.add_argument('in_image_file'
        ,type=str
        ,help="The input image file path."
        )

cu.add_std_args(parser)

parser.add_argument('-up', '--use-palette-file'
        , nargs=1
        , metavar='IN_PALETTE_FILE'
        , type=str
        , help="When used, uses a palette file of a given name,"
               " and doesn't write the palette into the resulting CPP file.")

parser.add_argument('-gp', '--gen-palette-file'
        , nargs=1
        , metavar='OUT_PALETTE_FILE'
        , type=str
        , help="When used, generates a palette file of a given name.")

parser.add_argument('-po', '--palette-only'
        , action='store_true'
        , help="When used, the C++ file will only contain the palette data.")

parser.add_argument('-s', '--add-size-info'
        , action='store_true'
        , help="Adds additional size info to the CPP (X and Y size).")

parser.add_argument('-ko', '--keep_color_order'
        , action='store_true'
        , help="When true - keeps the color order in the palette without sorting.")


args = parser.parse_args()
verbose_mode = args.verbose


# read the image

if verbose_mode:
    print(f"Opening `{args.in_image_file}` image file...")

in_image = Image.open(args.in_image_file)


# check image size (must be multiplies of 8)

(sx, sy) = in_image.size

if (sx % 8 > 0) or (sy % 8 > 0):
    raise ValueError(f"Incorrect input image size {in_image.size}."
                     f" The size must be multiplies of 8.")

(tx, ty) = (sx // 8, sy // 8)

if verbose_mode:
    print(f"...OK.\n\nImage size: {sx} x {sy}\n"
          f"Tiles: {tx*ty}\n"
          f"Size: {32*tx*ty} bytes\n"
          f"      {32*tx*ty/1024} KiB\n")


rgb_img = in_image.convert('RGBA')
rgb_data = np.asarray(rgb_img, dtype='int')


# get unique colors

unique_colors = np.unique(
        np.append([[0,0,0,0]], rgb_data.reshape((sx*sy),-1), axis=0)
        , axis=0)

if verbose_mode:
    print(f"Unique colors (+transparent): {len(unique_colors)}\n")

if len(unique_colors) > 16:
    raise ValueError(f"Input image has too many different colors"
                     f" ({len(unique_colors)} > 16).")

for c in unique_colors:
    if (c[3] != 255) and (sum(c) > 0):
        raise ValueError(f"Input image has an incorrect transparent color {c}."
                         f" Only [0 0 0 0] can be used with A != 255.")


# sort palette colors using the lightness formula

sorted_color_indexes = np.argsort(np.apply_along_axis(cu.L, 1, unique_colors))
palette = np.array([ unique_colors[i] for i in sorted_color_indexes ])

# but keep the original order when -ko is present
if args.keep_color_order:
    sorted_color_indexes = range(len(palette))
    first_use_array = [] # make sure the transparent is still #1
    #print(palette)
    for i in range(len(palette)):
        color_i = palette[i]
        # getting the index of the first use of a color
        first_use = np.argwhere(np.all(rgb_data == color_i, axis=-1))[0]
        indx = first_use[0]*sx + first_use[1]
        first_use_array.append(indx)
    #print(first_use_array)
    #print(np.argsort(np.array(first_use_array)))
    sorted_color_indexes = np.argsort(np.array(first_use_array))
    pal_copy = np.copy(palette)
    palette = np.array([ pal_copy[i] for i in sorted_color_indexes ])


# Save the palette file

if args.gen_palette_file:
    if verbose_mode:
        print(f"Saving palette into `{args.gen_palette_file[0]}`...")
    np.savetxt(args.gen_palette_file[0], palette, fmt='%4d',
            header=f"Generated from `{args.in_image_file}`\n"
                   f"Colors: {len(palette)}\n\n"
                    " R    G    B    A\n"
                    "------------------")
    if verbose_mode:
        print("...OK\n")


# Read the palette file

write_pal_to_cpp = True

if args.use_palette_file:
    if verbose_mode:
        print(f"Reading palette file `{args.use_palette_file[0]}`...")
    palette = np.loadtxt(args.use_palette_file[0], dtype='int')
    write_pal_to_cpp = False
    if verbose_mode:
        print("...OK\n")


if verbose_mode:
    print(f"Palette:\n       R    G    B    A")
    print(f"-----------------------")
    for i in range(len(palette)):
        print(f"{(i+1):2d}) {palette[i][0]:4d}"
              f" {palette[i][1]:4d}"
              f" {palette[i][2]:4d}"
              f" {palette[i][3]:4d}")
    print("")


# make indexed image

def get_index(c):
    for i in range(len(palette)):
        if np.all(c == palette[i]):
            return i
    raise ValueError("Unexpected color out of palette.")

indexed_img = np.apply_along_axis(get_index, 2, rgb_data)


# generating CPP file

if verbose_mode:
    print("Generating CPP file...")

img_var_name = args.name
pal_var_name = "pal_" + img_var_name
size_var_name = img_var_name + "_size"
sx_var_name = img_var_name + "_sx"
sy_var_name = img_var_name + "_sy"

cpp = cu.get_def_cpp_header(
        args.in_image_file, 
        args.namespace, 
        args.main_module + "." + img_var_name);

if len(palette) < 16:
    add_count = 16 - len(palette)
    palette = np.vstack([palette, [ [0,0,0,0] for i in range(add_count)]])

if write_pal_to_cpp:
    cpp += f"export const Palette {pal_var_name} = {{\n"
    cpp += cu.get_palette_cpp_code(palette)
    cpp += " };\n\n"


# split into tiles

tile_rows = np.split(indexed_img, sy//8, axis=0)
tiles = []

cpp += f"export constexpr u32 {size_var_name} = {tx * ty}; // total number of 8x8 tiles\n"
if args.add_size_info:
    cpp += f"export constexpr u32 {sx_var_name} = {tx}; // width (in tiles)\n"
    cpp += f"export constexpr u32 {sy_var_name} = {ty}; // height (in tiles)\n"
cpp += "\n"

cpp += f"export const Tile_4 {img_var_name}[{size_var_name}] = {{\n"

for j in range(len(tile_rows)):
    tile_row = tile_rows[j]
    tiles = np.split(tile_row, sx//8, axis=1)
    for i in range(len(tiles)):
        cpp += cu.get_tile_cpp_code(tiles[i].flatten())
        if i != (len(tiles) - 1):
            cpp += ",\n"
    if j != len(tile_rows) - 1:
        cpp += ',\n'
    else:
        cpp += ' '

cpp += f"}};\n\n"

cpp += cu.get_def_cpp_ending(args.namespace)


if verbose_mode:
    print(f"...OK\nWriting into `{args.out_cpp_file}`...")

with open(args.out_cpp_file, "w") as f:
    f.write(cpp)

if verbose_mode:
    print(f"...DONE\n")
