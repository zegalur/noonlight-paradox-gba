import converter_utils as cu

from PIL import Image
from PIL import ImageFont, ImageDraw

from dataclasses import dataclass
from collections import OrderedDict

import numpy as np
import argparse
import array
import math


########################## create an argument parser ##########################

parser = argparse.ArgumentParser(
        description= "Generates a C++ file with ready-to-use TTF font data.")

parser.add_argument('name'
        ,type=str
        ,help="The base name of the asset."
        )

parser.add_argument('px_size'
        ,type=int
        ,help="Font size in pixels."
        )

parser.add_argument('in_ttf_file'
        ,type=str
        ,help="The input TTF file path."
        )

parser.add_argument('in_charlist_file'
        ,type=str
        ,help="The input UTF-8 txt file that contains the glyps that will be"
              " included into the final CPP file."
        )

parser.add_argument('-pi', '--preview-image'
        , nargs=1
        , metavar='PREVIEW_IMAGE'
        , type=str
        , help="When used, generates a preview PNG image containing all the glyphs.")

parser.add_argument('-aw', '--align-with'
        , nargs=1
        , metavar='ALIGN_WITH'
        , type=str
        , help="When used, the generated glyph bitmaps will be rendered "
               "together with these text. Use it to get a better glyph alignment.")

cu.add_std_args(parser)

args = parser.parse_args()
verbose_mode = args.verbose


############################## read the TTF file ##############################

if verbose_mode:
    print(f"Opening `{args.in_ttf_file}` file...")

font = ImageFont.truetype(args.in_ttf_file, size=args.px_size)


########################### read the charlist file ############################

if verbose_mode:
    print(f"Opening `{args.in_charlist_file}` file...")

glyphs = []
with open(args.in_charlist_file, 'r', encoding='utf-8') as file:
    while True:
        glyph = file.read(1)
        if not glyph:
            break
        if glyph == '\n':
            continue
        glyphs.append(glyph)

# removing the doubles (while maintaining the original order)
glyphs = sorted(list(OrderedDict.fromkeys(glyphs)))

if verbose_mode:
    print(glyphs)
    print(f"...found {len(glyphs)} unique glyphs.")


######################### creating the preview image ##########################

all_glyphs = "".join(glyphs)
if args.preview_image:
    if verbose_mode:
        print("Generating a preview PNG image...")
    bbox = font.getbbox(text=all_glyphs, mode='1')
    (left, top, right, bottom) = bbox
    (width, height) = (right - left, bottom - top)
    img = Image.new(mode='1', size=[right - left, bottom - top])
    draw = ImageDraw.Draw(img, mode='1')
    draw.text((-left, -top), all_glyphs, font=font, fill="white")
    img.save(args.preview_image[0])


############################ creating the cpp file ############################

if verbose_mode:
    print("Generating CPP file...")

base_name = args.name
glyph_count_var = base_name + "_glyph_count"
raw_data_var = base_name + "_raw_data"
glyphs_var = base_name + "_glyphs"
size_var = base_name + "_size_px"

cpp = cu.get_def_cpp_header(
        f'{args.in_ttf_file}({args.px_size})',
        args.namespace,
        args.main_module + "." + base_name)

cpp += f"export constexpr u32 {size_var} = {args.px_size};\n"
cpp += f"export constexpr u32 {glyph_count_var} = {len(glyphs)};\n"
cpp += f"\n"

raw_data = array.array('I')

@dataclass
class GlyphInfo:
    glyph : str
    data_indx : int
    left : int
    top : int
    right : int
    bottom : int

glyph_info : [GlyphInfo] = []

indx = 0
for glyph in glyphs:
    ++indx
    bbox = font.getbbox(text=glyph, mode='1')
    (left, top, right, bottom) = bbox
    (width, height) = (right - left, bottom - top)
    glyph_info.append(GlyphInfo(glyph, len(raw_data), left, top, right, bottom))
    if width == 0 or height == 0:
        continue
    if width > 32:
        raise ValueError(f"Glyph is too wide (width = {width}). "
                         "Only the values up to 32 are supported. Halt!")
    # generating the glyph bitmap
    img = Image.new(mode='1', size=[32, height])
    draw = ImageDraw.Draw(img, mode='1')
    # drawing the glyph
    align_with = args.align_with[0] if args.align_with else ""
    draw.text((-left, -top), glyph + align_with, font=font, fill=(1,))
    # converting into u32 array
    img = img.convert("L")
    arr_8 = img.tobytes()
    new_arr = []
    indx = 0
    for y in range(height):
        v = 0
        for x in range(32):
            if arr_8[indx]:
                v = v | (1 << x)
            indx = indx + 1
        new_arr.append(v)
    # appending
    raw_data.extend(new_arr)

# export the raw data array
cpp += "// one byte per pixel, zero = transparent, non-zero = non-transparent\n"
cpp += f"export constexpr u32 {raw_data_var}[] = {{\n    "
line = "    "
for j in range(len(raw_data)):
    entry = f"0x{raw_data[j]:x}"
    if j != len(raw_data) - 1:
        entry += ", "
    if len(line + entry) < 80:
        line += entry
        cpp += entry
    else:
        line = "    " + entry
        cpp += "\n    " + entry
cpp += f" }};\n"
cpp += f"\n"

# export glyphs
cpp += "// Glyphs, sorted (ascending) by the utf8 code\n"
cpp += f"export constexpr gba::Glyph {glyphs_var}[] = {{\n    "
for g in glyph_info:
    w = g.right - g.left
    h = g.bottom - g.top
    l, t = g.left, g.top
    cpp_glyph = (g.glyph if g.glyph != '\\' else '\\\\') if g.glyph != "'" else "\\'"
    cpp += f"{{ U'{cpp_glyph}', "
    cpp += f"u8{{{w}}}, u8{{{h}}}, "
    cpp += f"i8{{{l}}}, i8{{{t}}}, "
    cpp += f"({raw_data_var} + {g.data_indx}) }}"
    if g.glyph != glyph_info[-1].glyph:
        cpp += f", // `{g.glyph}`\n    "
    else:
        cpp += f"  // `{g.glyph}`\n    "
cpp += f"}};\n"
cpp += f"\n"

cpp += cu.get_def_cpp_ending(args.namespace)

if verbose_mode:
    print(f"...OK\nWriting into `{args.out_cpp_file}`...")

with open(args.out_cpp_file, "w", encoding='utf-8') as f:
    f.write(cpp)

if verbose_mode:
    print(f"...DONE\n")
