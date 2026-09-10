import converter_utils as cu
import argparse

# create an argument parser

parser = argparse.ArgumentParser(
        description=
                "A small tool for converting 24bpp colors into GBA color"
                " constants you can use in your CPP code."
        )

parser.add_argument('color'
        ,type=str
        ,help="The input color in HTML hex format (e.g. ffaf55)."
        )

args = parser.parse_args()
color = args.color

if (len(color)!=6) or (not all([c in "0123456789abcdefABCDEF" for c in color])):
    raise ValueError(f"Incorrect color `{color}`. Only HTML hex format"
                      " (e.g. ffaf55, FFAF55) is allowed.")

rgb = (int(color[0:2],16), int(color[2:4],16), int(color[4:6],16))
rgb5 = cu.r8g8b8_arr_to_r5g5b5x_int(rgb)

print(f"0x{rgb5:04X}")
