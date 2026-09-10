import converter_utils as cu

from PIL import Image,ImageOps
from dataclasses import dataclass

import numpy as np
import numpy.typing as npt

import argparse
import math
import os

# create an argument parser

parser = argparse.ArgumentParser(
        description=
                "Converts the animations into a C++ file with"
                " ready-to-copy 8x8 tiles, palette etc."
        )

parser.add_argument('name'
        ,type=str
        ,help="The base name of the asset."
        )

parser.add_argument('in_anim_dir'
        ,type=str
        ,help="The input directory each sub-directory is treated as an animation."
        )

parser.add_argument('-ex', '--expand_to'
        , nargs=1
        , type=int
        , help=f"Expands the image to a square shaped image of set size.")

parser.add_argument('-np', '--no_palette'
        , action='store_true'
                    , help=f"When set, does not include the palette array.")

cu.add_std_args(parser)

args = parser.parse_args()
verbose_mode = args.verbose


# ---------------------- Analyzing the input directory ---------------------- #

if verbose_mode:
    print(f"Analyzing the `{args.in_anim_dir}`...")


# --------------------------- Getting the palette --------------------------- #

pal_img = Image.open(args.in_anim_dir + "/palette.png")
pal_data = np.asarray(pal_img.convert('RGBA'), dtype='int')
(px,py) = pal_img.size

pal_colors = np.unique(
        np.append([[0,0,0,0]], pal_data.reshape((px*py),-1), axis=0)
        , axis=0)

if len(pal_colors) > 16:
    raise ValueError(f"Too many colors in palette."
                     " Maximum 15 non-transparent colors supported.")

sorted_color_indexes = np.argsort(np.apply_along_axis(cu.L, 1, pal_colors))
palette = np.array([ pal_colors[i] for i in sorted_color_indexes ])
pimg = Image.new('P', (1, 1))
pimg.putpalette(palette, rawmode="RGBA")

pal_16 = palette.copy()
if len(pal_16) < 16:
    add_count = 16 - len(pal_16)
    pal_16 = np.vstack([pal_16, [ [0,0,0,0] for i in range(add_count)]])

# returns the index of the closest color
def get_index(c):
    min_d = cu.col_dist_2(c, palette[0]) + (0 if c[3] == palette[0][3] else 1000)
    min_i = 0
    for i in range(len(palette)):
        d = cu.col_dist_2(c, palette[i]) + (0 if c[3] == palette[i][3] else 1000) 
        if min_d > d:
            min_d = d
            min_i = i
    return min_i

if verbose_mode:
    print(f"Palette:")
    print(palette)
    print()


# ---------------------- Analyzing the sub-directories ---------------------- #

anim_list = [sub_dir for sub_dir in os.listdir(args.in_anim_dir) if not '.' in sub_dir]

if verbose_mode:
    print(f"Found {len(anim_list)} sub-directories:")
    i = 1
    for sd in anim_list:
        print(f"  {i}) `{sd}`")
        i += 1
    print()


# -------------------------- Collecting the frames -------------------------- #

if verbose_mode:
    print("Collecting the frames and tiles...")

(sx, sy) = (0, 0) # size in pixels
(tx, ty) = (0, 0) # size in tiles

@dataclass
class Tile:
    data : npt.NDArray
    hash_val : int
    replaced = False 
    replace_indx = -1
    unique_id = -1

@dataclass
class Frame:
    name : str
    tiles : [int]

@dataclass
class Anim:
    name : str
    frames : [Frame]

tbank : [Tile] = []
all_anim : [Anim] = []

for v in range(len(anim_list)):
    anim_dir = anim_list[v]
    frame_files = sorted([
            fname for fname in os.listdir(args.in_anim_dir + "/" + anim_dir)])

    anim_data_var = args.name + "_" + anim_dir + "_data"
    anim = Anim(anim_data_var, [])
    all_anim.append(anim)

    for w in range(len(frame_files)):
        frame = Frame(frame_files[w], [])
        anim.frames.append(frame)

        frame_file = frame_files[w]
        frame_img = Image.open(
                args.in_anim_dir + "/" + anim_dir + "/" + frame_file)
        if args.expand_to:
            side = args.expand_to[0]
            frame_img = ImageOps.expand(
                    frame_img, 
                    # (left, top, right, bottom)
                    border=(0,side-frame_img.size[1],side-frame_img.size[0],0), 
                    fill=(0, 0, 0, 0))
        if sx == 0:
            (sx, sy) = frame_img.size
            if (sx % 8 != 0) or (sy % 8 != 0):
                raise ValueError(f"Incorrect frame size {(sx,sy)}."
                                 f" Must be in form (8*n, 8*m).")
            (tx, ty) = (sx // 8, sy // 8)

        if (sx, sy) != frame_img.size:
            raise ValueError(f"Inconsistent frame sizes are unsupported.")

        rgb_img = frame_img.convert('RGBA')
        rgb_data = np.asarray(rgb_img, dtype='int')
        indexed_img = np.apply_along_axis(get_index, 2, rgb_data)
        tile_rows = np.split(indexed_img, sy//8, axis=0)
        for j in range(len(tile_rows)):
            tile_row = tile_rows[j]
            tiles = np.split(tile_row, sx//8, axis=1)
            for i in range(len(tiles)):
                tindx = len(tbank)
                frame.tiles.append(tindx)
                tile = Tile(tiles[i], hash(tiles[i].data.tobytes()))
                tbank.append(tile)

if verbose_mode:
    print(f"Tile count: {len(tbank)}")


# ------------------------ Replacing the equal tiles ------------------------ #

rcount = 0

if verbose_mode:
    print(f"Merging equal tiles...")

for i in range(len(tbank)):
    if verbose_mode:
        if i % 1000 == 1000 - 1:
            print(f"({i} / {len(tbank)}, {rcount} replaced)") 
    if tbank[i].replaced:
        continue
    for j in range(i + 1, len(tbank)):
        if tbank[j].replaced:
            continue
        if tbank[i].hash_val != tbank[j].hash_val:
            continue
        if (tbank[i].data == tbank[j].data).all():
            tbank[j].replaced = True
            tbank[j].replace_indx = i
            rcount += 1

# Assigning the unique ids
tuid = 0
for i in range(len(tbank)):
    if tbank[i].replaced:
        continue
    tbank[i].unique_id = tuid
    tuid += 1

def get_utid(tile):
    if tile.replaced == False:
        return tile.unique_id
    return get_utid(tbank[tile.replace_indx])


# --------------------------- Generating the CPP ---------------------------- #

if verbose_mode:
    print("Generating CPP file...")

cpp = cu.get_def_cpp_header(
        args.in_anim_dir, 
        args.namespace, 
        args.main_module + "." + args.name);

tcount_vname = args.name + "_tcount"
acount_vname = args.name + "_anim_count"
aenum_name = "Anim_" + args.name
pal_var_name = "pal_" + args.name
adata_var = args.name + "_data"
fcount_var = args.name + "_frame_count"
tbank_var = args.name + "_tbank"
sx_var_name = args.name + "_sx"
sy_var_name = args.name + "_sy"

# adding some size information
cpp += f"export constexpr u32 {sx_var_name} = {tx}; // width (in tiles)\n"
cpp += f"export constexpr u32 {sy_var_name} = {ty}; // height (in tiles)\n"
cpp += "\n"
cpp += "// number of tiles per frame\n"
cpp += f"export constexpr u32 {tcount_vname} = {tx * ty};\n\n"

# adding the list of animations
cpp += f"export constexpr u32 {acount_vname} = {len(anim_list)};\n\n"
cpp += f"export enum {aenum_name} {{\n"
for i in range(len(anim_list)):
    cpp += f"    {anim_list[i].upper()} = {i}"
    if i != len(anim_list) - 1:
        cpp += ","
    cpp += "\n"
cpp += "};\n\n"

# adding the palette
if not args.no_palette:
    cpp += f"export const Palette {pal_var_name} = {{\n"
    cpp += cu.get_palette_cpp_code(pal_16)
    cpp += " };\n\n"

# adding unique tile data
cpp += f"export constexpr u32 {tbank_var}_size = {tuid};\n\n"
cpp += "// tile bank, contains only unique tiles\n"
cpp += f"export const Tile_4 {tbank_var}[{tbank_var}_size] = {{\n"
tmp_i = 0
for tile in tbank:
    if tile.replaced:
        continue
    cpp += cu.get_tile_cpp_code(tile.data.flatten())
    tmp_i += 1
    if tmp_i != tuid:
        cpp += ","
    cpp += "\n"
cpp += "};\n\n"

# adding the animation frames
for anim in all_anim:
    cpp += f"// each value is a tile index of a tile from the `{tbank_var}`\n"
    cpp += f"export const u16 {anim.name}[{len(anim.frames)}][{tx*ty}] = {{\n"
    for f in anim.frames:
        cpp += f"  {{ " #// {f.name}\n"
        l = 4
        #cpp += "    "
        for tid in range(len(f.tiles)):
            t = tbank[f.tiles[tid]]
            v = get_utid(t)
            s_x = f"0x{v:X}"
            s_0 = f"{v}"
            s = s_x if len(s_x) < len(s_0) else s_0
            if l + len(s) >= 80:
                cpp += "\n    "
                l = 4
            l += len(s)
            cpp += s
            if tid != len(f.tiles) - 1:
                cpp += ','
                l += 1
        cpp += " }"
        if f.name != anim.frames[-1].name:
            cpp += ",\n"
    cpp += " };\n\n"

# adding the main animation data array
cpp += f"export const u16 (*{adata_var}[{len(anim_list)}])[{tx*ty}] = {{\n"
for i in range(len(all_anim)):
    cpp += f"    {all_anim[i].name}"
    if i != len(all_anim) - 1:
        cpp += ",\n"
cpp += "\n};\n\n"

# same but as an array of pointers
cpp += f"export const u16* {adata_var}_ptr[{len(anim_list)}] = {{\n"
for i in range(len(all_anim)):
    cpp += f"    &{all_anim[i].name}[0][0]"
    if i != len(all_anim) - 1:
        cpp += ",\n"
cpp += "\n};\n\n"

# adding the frame count array
cpp += f"// {fcount_var}[ANIM_ID] = animation frame count\n"
cpp += f"export constexpr u32 {fcount_var}[{len(anim_list)}] = {{\n"
cpp += "    "
l = 4
for i in range(len(all_anim)):
    s = f"{len(all_anim[i].frames)}"
    if i != len(all_anim) - 1:
        s += ", "
    if len(s) + l >= 80:
        cpp += "\n    "
        l = 4
    cpp += s
    l += len(s)
cpp += "\n};\n\n"

cpp += cu.get_def_cpp_ending(args.namespace)


if verbose_mode:
    print(f"...OK\nWriting into `{args.out_cpp_file}`...")

with open(args.out_cpp_file, "w") as f:
    f.write(cpp)

if verbose_mode:
    print(f"...DONE\n")
