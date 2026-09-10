# some shared common functions

from datetime import datetime

import argparse
import math


# Returns the perceived lightness of the color.
# (From: https://alienryderflex.com/hsp.html)
def L(c):
    return c[3] + math.sqrt(
              0.299*(c[0]/255)**2 
            + 0.587*(c[1]/255)**2 
            + 0.114*(c[2]/255)**2)


# Converts r8g8b8 color array into a r5g5b5x integer:
def r8g8b8_arr_to_r5g5b5x_int(color):
    r5 = int(color[0]) >> 3
    g5 = int(color[1]) >> 3
    b5 = int(color[2]) >> 3
    R = r5
    G = g5
    B = b5
    min_d = col_dist_2(color, (R << 3, G << 3, B << 3))
    for r in [r5, r5 + 1]:
      for g in [g5, g5 + 1]:
        for b in [b5, b5 + 1]:
            if (r << 3) > 255: continue
            if (g << 3) > 255: continue
            if (b << 3) > 255: continue
            d = col_dist_2(color, (r << 3, g << 3, b << 3))
            if d < min_d:
                min_d = d
                R = r
                G = g
                B = b
    return R | (G << 5) | (B << 10)
    #return r5 | (g5 << 5) | (b5 << 10)


# Returns the SQUARE OF the estimated color distant for NON-linear colors
def col_dist_2(c1, c2) -> float:
    # using the `redmean` formula:
    # https://en.wikipedia.org/wiki/Color_difference
    (R1, G1, B1) = (float(c1[0]), float(c1[1]), float(c1[2]))
    (R2, G2, B2) = (float(c2[0]), float(c2[1]), float(c2[2]))
    (dR, dG, dB) = (R1 - R2, G1 - G2, B1 - B2)
    r = (R1 + R2) / 2
    return ((2 + r/256)*dR*dR + 4*dG*dG + (2 + (255 - r)/256)*dB*dB)/(256**2)


# Returns the SQUARE OF the estimated color distant for NON-linear colors
def col_dist_2_with_a(c1, c2) -> float:
    return col_dist_2(c1,c2) + abs(float(c1[3])-float(c2[3]))**2


# sRGB_255 -> linear RGB_1.0
def col_to_linear(c):
    def l(v):
        x = float(v) / 255
        return (x / 12.92) if x <= 0.04045 else ((x + 0.055)/1.055) ** 2.4
    return (l(c[0]), l(c[1]), l(c[2]))


# linear RGB_1.0 -> sRGB_255
def col_to_srgb(c):
    def s(x):
        return (x * 12.92) if x <= 0.0031308 else (x**(1.0/2.4)*1.055 - 0.055)
    return (int(s(c[0])*255), int(s(c[1])*255), int(s(c[2])*255))


def avg_col(c1, c2):
    (R1, G1, B1) = col_to_linear(c1)
    (R2, G2, B2) = col_to_linear(c2)
    r = ((R1 + R2)/2, (G1 + G2)/2, (B1 + B2)/2)
    r2 = col_to_srgb(r)
    m = 255 ^ 7
    return (r2[0]&m, r2[1]&m, r2[2]&m, int(255))


###############################################################################

# Adds common program argumens.
def add_std_args(
        parser,
        out_cpp=True,
        ):
    parser.add_argument('-v', '--verbose'
        , action='store_true'
        , help="Verbose mode.")

    parser.add_argument('-0', '--dummy'
        , action='store_true'
        , help="A dummy argument. Does nothing, skipped.")
    
    if out_cpp:
        parser.add_argument('out_cpp_file'
            , type=str
            , help="The name of C++ file that will be created for the image."
            )
        parser.add_argument('-n', '--namespace'
            , nargs=1
            , type=str
            , default='data'
            , help="Sets the namespace where data variables will be placed (default `data`)."
            )
        parser.add_argument('-mm', '--main-module'
            , nargs=1
            , type=str
            , default='data'
            , help="Sets the main module name (default `data`)."
            )


###############################################################################

def get_def_cpp_header(
        in_file_name : str, 
        namespace : str,
        module : str,
        import_list = [],
        includes = [],
        ) -> str:
    cpp = f"// Auto-generated from `{in_file_name}`\n"
    cpp += f"// ({datetime.now()})\n"
    cpp += f"module;\n"

    # includes
    if len(includes) > 0: 
        cpp += "\n"
    for i in includes:
        cpp += f'#include "{i}"\n'
    if len(includes) > 0: 
        cpp += "\n"

    cpp += f"export module {module};\n"
    cpp += "\n"
    cpp += f"import engine;\n"

    # imports
    for m in import_list:
        cpp += f"import {m};\n"
    if len(import_list) > 0:
        cpp += "\n"

    cpp += f"using namespace gba;\n\n"
    cpp += f"namespace {namespace} {{\n\n"
    return cpp
    

def get_def_cpp_ending(namespace : str) -> str:
    return f"}} // namespace {namespace}"


def get_palette_cpp_code(palette, pre='') -> str:
    cpp = pre + "    { "
    for i in range(8):
        c1 = r8g8b8_arr_to_r5g5b5x_int(palette[2*i + 0])
        c2 = r8g8b8_arr_to_r5g5b5x_int(palette[2*i + 1])
        c = c1 | (c2 << 16)
        cpp += f"0x{c:08x}"
        if i == 3:
            cpp += "\n" + pre + "    , "
        elif i != 7:
            cpp += ", "
    cpp += " }" #"\n" + pre + "    }"
    return cpp


def get_tile_cpp_code(t, pre='') -> str:
    cpp = pre + "    {{ "
    for i in range(8):
        n = int(0)
        for j in range(8):
            n |= t[8*i + j] << (j * 4)
        cpp += f"0x{n:08x}"
        if i != 7: cpp += ", "
        if i == 3: cpp += "\n" + pre + "       "
    cpp += " }}"
    return cpp
