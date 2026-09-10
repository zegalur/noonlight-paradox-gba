import converter_utils as cu

from dataclasses import dataclass
from datetime import datetime
from PIL import Image

import numpy as np
import numpy.typing as npt

import argparse
import math


MAX_PAL_DEF_COUNT = 16
MERGE_SIMILAR_REC_THRESHOLD = 0.11
MERGE_TILES_REC_THRESHOLD = 0.05
TILES_MEAN_REC_THRESHOLD = 0.01


########################## create an argument parser ##########################

parser = argparse.ArgumentParser(
        description=
                "Converts the map into a C++ file with"
                " ready-to-copy 8x8 tiles, palettes etc.")

parser.add_argument('name'
        ,type=str
        ,help="The base name of the map."
        )

parser.add_argument('in_map_img'
        ,type=str
        ,help="Map image file."
        )

parser.add_argument('-e', '--throw_error'
        , action='store_true'
        , help="Throw error when limits can't be achieved.")

parser.add_argument('-s', '--save_as_image'
        , nargs=1
        , type=str
        , help="When used, saves the result preview into"
               " the specified image file.")

parser.add_argument('-pc', '--palette_count'
        , nargs=1
        , type=int
        , default=[MAX_PAL_DEF_COUNT]
        , help=f"Sets how many different palettes there be"
               f" (default {MAX_PAL_DEF_COUNT}).")

parser.add_argument('-po', '--palette_offset'
        , nargs=1
        , type=int
        , default=[0]
        , help=f"Sets the screenblock's palbank offset value."
                " The resulting palbank value will be"
                " (palette_index + palette_offset)"
                " (default 0).")

parser.add_argument('-tc', '--tile_count'
        , nargs=1
        , type=int
        , help=f"When used, sets the maximum number of unique tiles.")

parser.add_argument('-z', '--add_zero_empty_tile'
        , action='store_true'
        , help=f"When used, adds empty tile at zero.")

parser.add_argument('-to', '--tile_offset'
        , nargs=1
        , type=int
        , default=[0]
        , help=f"Sets the tiles offset within the charblock.")

parser.add_argument('-ms', '--merge_similar'
        , nargs=1
        , type=float
        , help=f"When used, will merge similar palettes using the threshold"
               f" (recommended value {MERGE_SIMILAR_REC_THRESHOLD}).")

parser.add_argument('-p', '--pre_quantization'
        , nargs=1
        , type=int
        , help=f"When used, converts to limited palette with this number of "
                "colors before applying anything else.")

parser.add_argument('-sp', '--simplify_palette'
        , nargs=1
        , type=float
        , help=f"When used, adds additional step of merging similar colors "
                "WITHIN palettes itself using this threshold.")

parser.add_argument('-m', '--merge_smaller'
        , action='store_true'
        , help="Forces to merge the smaller palettes "
               "together (without artifacts) before applying any other "
               "merge techniques (that can create artifacts). We recommend "
               "trying `-ms` without `-m`.")

parser.add_argument('-d', '--with_dithering'
        , action='store_true'
        , help="Turn on the dithering for the tiles when converting to 15 colors.")

parser.add_argument('-mt', '--merge_tiles'
        , nargs=5
        , type=float
        , default=[0.0, 0.01, 0.01, 0.01, 0.01]
        , help=f"When used, merges similar tiles using the 4 thresholds: "
               f" [overall distance] [R_mean] [G_mean] [B_mean] [A_mean]"
               f" (recommended values are"
               f" {MERGE_TILES_REC_THRESHOLD}"
               f" {TILES_MEAN_REC_THRESHOLD}"
               f").")

parser.add_argument('-mt2', '--merge_tiles_2'
        , nargs=3
        , type=float
        , help=f"Same as -mt but uses less parameters (overall, channels, alpha).")

parser.add_argument('-dm', '--depth_mask'
        , nargs=1
        , type=str
        , help="When used, uses the specified depth mask to generate"
               " the foreground layer.")

parser.add_argument('-cm', '--collision_map'
        , action='store_true'
        , help="When used, applies `-z -m -pc 1 -e`. "
               "Also, does not use H and V flips on tiles.")


cu.add_std_args(parser)
args = parser.parse_args()

if args.merge_tiles_2:
    args.merge_tiles = [ args.merge_tiles_2[0] 
                       , args.merge_tiles_2[1]
                       , args.merge_tiles_2[1]
                       , args.merge_tiles_2[1]
                       , args.merge_tiles_2[2] ]

# apply 
if args.collision_map:
    args.add_zero_empty_tile = True
    args.merge_smaller = True
    args.palette_count[0] = 1

if (args.palette_count[0] > 16) or (args.palette_count[0] < 1):
    raise Exception("Palette count `-pc` or `--palette_count` should be"
                    f" between 1 and 16 inc. (now its = {args.palette_count[0]})!")

verbose_mode = args.verbose
throw_error = args.throw_error
tile_offset = args.tile_offset[0]


############################### read the image ################################

if verbose_mode:
    print(f"Opening `{args.in_map_img}` image file...")

in_image = Image.open(args.in_map_img).convert('RGBA')
in_depth_mask = None

if args.depth_mask:
    in_depth_mask = Image.open(args.depth_mask[0]).convert('1')


################## check image size (must be in form of 8*2^n) ################

(sx, sy) = in_image.size

if (sx % 8 > 0) or (sy % 8 > 0):
    raise ValueError(f"Incorrect input image size {in_image.size}."
                     f" The size must be 8 * N")#2^n.")

(tx, ty) = (int(sx // 8), int(sy // 8))

if args.depth_mask:
    if in_image.size != in_depth_mask.size:
        raise ValueError("Depth mask image size != input image size.")

if verbose_mode:
    print(f"...OK.\n\n"
          f"Image Size  : {sx} x {sy} px\n"
          f"      Tiles : {tx} x {ty}\n"
          f"Tiles Count : {tx*ty}\n"
          f"Size: ~{32*tx*ty} bytes\n"
          f"      ~{32*tx*ty/1024} KiB\n")


############################# pre-quantization ################################

if args.pre_quantization:
    if verbose_mode:
        print(f"Pre-quantization into a {args.pre_quantization[0]}-palette...")
    in_image = in_image.convert('P', palette=Image.ADAPTIVE, 
                                colors=args.pre_quantization[0]
                                ).convert('RGBA')
    if verbose_mode:
        print("...OK\n")

rgba_img = in_image


########## check if image has any alpha values other then 0 and 255 ###########

a_img = rgba_img.getchannel('A')
ualpha = a_img.getcolors(maxcolors=2)
incorrect_alpha = True

if ualpha:
    incorrect_alpha = False
    has_0_a = (ualpha[0][1] == 0)
    has_255_a = (ualpha[0][1] == 255)
    if len(ualpha) == 2:
        has_0_a   = has_0_a or (ualpha[1][1] == 0)
        has_255_a = has_255_a or (ualpha[1][1] == 255)
    if (len(ualpha) == 2) and (not (has_0_a and has_255_a)):
        incorrect_alpha = True
    elif (len(ualpha) == 1) and (not has_255_a):
        incorrect_alpha = True

if incorrect_alpha:
    raise ValueError(f"Input image contains a pixel with incorrect alpha."
                     " Only 0 and 255 alpha allowed.")


############################ pre-process the image ############################

data = np.asarray(in_image, dtype='int')


# replace every transparent pixel with RGBA = 0000
# also, convert all colors to R5G5B5

alphas = data[:, :, 3:4]
transp = np.any(alphas == 0, axis=-1)
data[transp, 0:3] = [0, 0, 0]
data[:, :, 0:3] &= (255 ^ 7)


# convert back into the Image object

in_image = Image.fromarray(data.astype('uint8'), 'RGBA')



########################### define main datatypes #############################

@dataclass
class Palette:
    data : npt.NDArray
    first_non_tr : int
    hash_val : int
    tiles : [int] # tile (indexes) that are using this palette
    eliminated = False

def remap_pal_tiles(pal, mapping):
    # remaps the connected tiles
    def mapi(indx): 
        return mapping[indx]
    mapi_v = np.vectorize(mapi)
    for t in pal.tiles:
        tiles[t].data = mapi_v(tiles[t].data).astype(np.uint8)
        if args.depth_mask:
            masked_tiles[t].data = mapi_v(masked_tiles[t].data).astype(np.uint8)

def eliminate_pal_into(pal, into, into_pal_indx, mapping):
    remap_pal_tiles(pal, mapping)
    into.tiles.extend(pal.tiles)
    for i in pal.tiles:
        tiles[i].pal_indx = into_pal_indx
        if args.depth_mask:
            masked_tiles[i].pal_indx = into_pal_indx
    pal.eliminated = True
    pal.tiles = []


@dataclass
class Tile:
    data : npt.NDArray
    data_original : npt.NDArray
    pal_indx : int
    x : int
    y : int
    hash_val : int
    masked : bool
    replaced = False
    replace_indx = -1
    unique_id = -1
    flip_h = False
    flip_v = False
    mean = -1.0

palettes : [Palette] = []
tiles : [Tile] = [] 
masked_tiles : [Tile] = [] # tiles masked by the depth mask

def clean_up_palettes():
    global palettes
    global tiles
    global masked_tiles
    pal_mapping = []
    cur_free = 0
    for pal in palettes:
        if pal.eliminated:
            pal_mapping.append(None)
        else:
            pal_mapping.append(cur_free)
            cur_free += 1
    # remove eliminated palettes
    palettes = [p for p in palettes if p.eliminated == False]
    # remap the tiles
    for tbank in [tiles, masked_tiles]:
        for t in tbank:
            t.pal_indx = pal_mapping[t.pal_indx]


############################## split into tiles ###############################

if verbose_mode:
    dith = ' (with dithering)' if args.with_dithering else ''
    print("Splitting into the tiles"+dith+"...")

if args.add_zero_empty_tile or args.collision_map:
    zero_pal_data = np.zeros(4*16, dtype=np.uint8).reshape(-1,4)
    zero_pal = Palette(zero_pal_data,16,hash(zero_pal_data.data.tobytes()),[0])
    palettes.append(zero_pal)
    zero_tile_data = np.zeros((8,8), dtype=np.uint8)
    tiles.append(Tile(zero_tile_data, zero_tile_data.copy(), 0, -1,-1,
                      hash(zero_tile_data.data.tobytes()), 
                      False))
    if args.depth_mask:
        zero_tile_data_2 = np.zeros((8,8), dtype=np.uint8)
        masked_tiles.append(Tile(zero_tile_data_2, zero_tile_data_2.copy(), 0, -1,-1,
                            hash(zero_tile_data_2.data.tobytes()), 
                            True))

for y in range(ty):
  for x in range(tx):
    # crop the tile out from the image
    tile_img = in_image.crop([x*8, y*8, (x+1)*8, (y+1)*8])

    # count the transparent pixels
    tile_alpha = tile_img.getchannel('A')
    is_transparent = False
    ualpha = tile_alpha.getcolors(maxcolors=2)
    if len(ualpha) == 1 and ualpha[0][1] == 0:
        is_transparent = True
    elif len(ualpha) == 2 and ((ualpha[0][1] == 0) or (ualpha[1][1] == 0)):
        is_transparent = True

    # convert into limited palette
    ccount = 16 if is_transparent else 15
    tile4_img = (tile_img.convert('P', colors=ccount) if args.with_dithering 
                 else
                 tile_img.convert('P', colors=ccount, palette=Image.ADAPTIVE))

    if ccount == 15:
        # for non-transparent images add the transparent color
        tile4_img.putpalette(tile4_img.getpalette('RGBA') + [0,0,0,0], 
                             rawmode='RGBA')
    
    # rearrange the palette so the transparent color is the first color
    '''tile_pal = tile4_img.getpalette('RGBA')
    tr_indx = -1
    for i in range(16):
        if sum(tile_pal[(i*4):((i+1)*4)]) == 0:
            tr_indx = i
            break
    if tr_indx != 0:
        order = list(range(16))
        order[0] = tr_indx
        order[tr_indx] = 0
        tile4_img = tile4_img.remap_palette(order)'''

    # rearrange the palette using by the perceived lightness
    pal_1 = np.reshape(np.asarray(tile4_img.getpalette('RGBA')), (-1, 4))
    sorted_indxs = list(np.apply_along_axis(cu.L, axis=1, arr=pal_1).argsort())
    tile4_img = tile4_img.remap_palette(sorted_indxs)

    tile_pal = tile4_img.getpalette('RGBA')
        
    # convert palette colors into r5g5b5
    for i in range(1,16):
        tile_pal[4*i + 0] &= (255 ^ 7)
        tile_pal[4*i + 1] &= (255 ^ 7)
        tile_pal[4*i + 2] &= (255 ^ 7)

    tile4_img.putpalette(tile_pal, rawmode='RGBA')

    # make sure transparent pixels remains transparent
    empty_image = Image.new(size=[8,8], mode='P')
    empty_image.putpalette(tile_pal, rawmode='RGBA')
    #tile4_img = Image.composite(
    #        tile4_img, empty_image, tile_img.getchannel('A'))

    # contains the final palette
    tile_pal = tile4_img.getpalette('RGBA')
    first_non_tr = 16
    for i in range(16):
        if tile_pal[i*4 + 3] != 0:
            first_non_tr = i
            break

    # save the palette
    pal_indx = len(palettes)
    pal_data = np.asarray(tile_pal, dtype=np.uint8).reshape(-1,4)
    pal = Palette(np.asarray(tile_pal, dtype=np.uint8).reshape(-1,4), 
                  first_non_tr, hash(pal_data.data.tobytes()),
                  [len(tiles)])
    palettes.append(pal)

    # save the tile
    def map_f(x): 
        return x if x>=first_non_tr else 0
    map_f_v = np.vectorize(map_f)
    tile_data = map_f_v(np.array(tile4_img, dtype=np.uint8).astype(np.uint8))
    tiles.append(Tile(tile_data, tile_data.copy(), pal_indx, x, y, 
                      hash(tile_data.data.tobytes()), 
                      False))

    if args.depth_mask:
        # save the masked tile
        depth_mask_tile = in_depth_mask.crop([x*8, y*8, (x+1)*8, (y+1)*8])
        masked_tile4 = Image.composite(tile4_img, empty_image, depth_mask_tile)
        masked_tile_data = np.array(masked_tile4, dtype=np.uint8)
        masked_tiles.append(Tile(masked_tile_data, masked_tile_data.copy(), 
                            pal_indx, x, y, 
                            hash(masked_tile_data.data.tobytes()), 
                            True))

if verbose_mode:
    print(f"...DONE ({len(palettes)} palette(s), {len(tiles)} tile(s))\n")


############################ merge equal palettes #############################

el_pal_count = 0

def merge_equal_p(tab = ''):
    global palettes
    global el_pal_count
    global tiles
    global masked_tiles

    eq_pal_count = 0

    if verbose_mode:
        print(tab + "Eliminating the duplicate palettes...")

    for i in range(len(palettes)):
        if palettes[i].eliminated:
            continue
        for j in range(i + 1, len(palettes)):
            if palettes[j].eliminated:
                continue
            if palettes[j].hash_val == palettes[i].hash_val:
                if np.all(palettes[j].data == palettes[i].data):
                    palettes[j].eliminated = True
                    palettes[i].tiles.extend(palettes[j].tiles)
                    for k in palettes[j].tiles:
                        tiles[k].pal_indx = i
                        if args.depth_mask:
                            masked_tiles[k].pal_indx = i
                    palettes[j].tiles = []
                    el_pal_count += 1
                    eq_pal_count += 1

    clean_up_palettes()

    if verbose_mode:
        print(tab + f"...DONE ({eq_pal_count} palette(s) removed, "
              f"{len(palettes)} left, "
              f"max = {args.palette_count[0]})\n")

# merge equal for the first time
merge_equal_p()


####################### merge colors within palettes ##########################

if args.simplify_palette:
    th = args.simplify_palette[0]
    th_2 = th ** 2.0
    mcount = 0
    if verbose_mode:
        print(f"Merging colors within palettes (threshold = {th})... =>\n")

    for p in palettes:
      while True:
        min_d = float('inf')
        min_i = -1
        min_j = -1
        for i in range(p.first_non_tr, 16):
          for j in range(i + 1, 16):
            d = cu.col_dist_2(p.data[i], p.data[j])
            if min_d > d:
                min_d = d
                min_i = i
                min_j = j
        if (min_d >= th_2) or (min_i < 0):
            break # nothing to eliminate
        mcount += 1
        # remove i-th color from the palette
        ci = p.data[i].copy()
        cj = p.data[j].copy()
        for i in reversed(range(p.first_non_tr, min_i+1)):
            p.data[i] = p.data[i-1].copy()
        p.first_non_tr += 1
        # replace j-th with the linear average
        p.data[j] = cu.avg_col(ci, cj)
        # re-index the tile pixels
        def ri(i):
            if i == min_i: return min_j
            if  i < min_i: return (i + 1)
            return i
        ri_v = np.vectorize(ri)
        for t in p.tiles:
            tiles[t].data = ri_v(tiles[t].data).astype(np.uint8) 
            if args.depth_mask:
                masked_tiles[t].data = ri_v(masked_tiles[t].data).astype(np.uint8) 

    # merge equal palettes again
    merge_equal_p('    ')

    if verbose_mode:
        print(f"=> ...DONE ({mcount} colors removed in total).")


############################ merge palettes together ##########################

in_pal_count = 0

if verbose_mode and args.merge_smaller:
    print("Eliminating smaller palettes that can be fully included into bigger...")

any_eliminated = False
while args.merge_smaller:
  for i in range(len(palettes)):
    if palettes[i].eliminated:
        continue
    pi = palettes[i]
    for j in range(i + 1, len(palettes)):
        if palettes[j].eliminated:
            continue
        pj = palettes[j]

        same_colors = (pj.data[:,None] == pi.data).all(axis=2).any(axis=1)
        all_colors = 16 - pj.first_non_tr
        fully_included = np.count_nonzero(same_colors) - pj.first_non_tr
        not_included = all_colors - fully_included
        empty_seats = pi.first_non_tr - 1

        if empty_seats >= not_included:
            # palette can be fully merged into
            for k in pj.tiles:
                tiles[k].pal_indx = i
                if args.depth_mask:
                    masked_tiles[k].pal_indx = i
            # build up the mapping
            mapping = []
            for c in pj.data:
                already_included = False
                for u in range(16):
                    if (c == pi.data[u]).all():
                        mapping.append(u)
                        already_included = True
                        break
                if already_included == False:
                    # include this color into `pi`
                    pi.data[pi.first_non_tr - 1] = c
                    pi.first_non_tr -= 1
                    mapping.append(pi.first_non_tr)
            eliminate_pal_into(pj, pi, i, mapping)
            any_eliminated = True
            in_pal_count += 1
            el_pal_count += 1
  if any_eliminated == False:
      break
  any_eliminated = False

clean_up_palettes()

if verbose_mode and args.merge_smaller:
    print(f"...DONE ({in_pal_count} palette(s) removed, "
          f"{len(palettes)} left, "
          f"max = {args.palette_count[0]})\n")


######################## merge close palettes together ########################

ms_pal_count = 0

if verbose_mode:
    print("Merging close palettes...")

if args.merge_similar:
  dist_th_2 = args.merge_similar[0] ** 2.0
  while True:
   if len(palettes) - ms_pal_count <= args.palette_count[0]:
     break
   any_eliminated = False
   for i in range(len(palettes)):
    if palettes[i].eliminated:
        continue
    pi = palettes[i]
    for j in range(len(palettes)):
        if len(palettes) - ms_pal_count <= args.palette_count[0]:
            break
        if i == j:
            continue
        if palettes[j].eliminated:
            continue
        pj = palettes[j]
        # test if `pj` is close enough to merge
        mapping = list([0] * 16)
        cant_include = 0
        free_seats = pi.first_non_tr - 1
        for u in range(pj.first_non_tr, 16):
            min_d2 = float('inf')
            min_indx = -1
            for v in range(pi.first_non_tr, 16):
                d2 = cu.col_dist_2(pj.data[u], pi.data[v])
                if d2 < min_d2:
                    min_d2 = d2
                    min_indx = v
            if min_indx == -1 or min_d2 > dist_th_2:
                cant_include += 1
                mapping[u] = pi.first_non_tr - cant_include
            else:
                mapping[u] = min_indx
            if cant_include > free_seats:
                break

        if cant_include <= free_seats:
            # copy each non-included color into an empty seat
            for u in range(pj.first_non_tr, 16):
                if mapping[u] < pi.first_non_tr:
                    pi.data[mapping[u]] = pj.data[u]
            pi.first_non_tr -= cant_include
            # eliminate the merged palette
            eliminate_pal_into(pj, pi, i, mapping)
            ms_pal_count += 1
            el_pal_count += 1
            any_eliminated = True
   #break
   if any_eliminated == False:
       break
 
clean_up_palettes()

if verbose_mode:
    print(f"...DONE ({ms_pal_count} palette(s) removed, "
          f"{len(palettes)} left, "
          f"max = {args.palette_count[0]})\n")

if args.throw_error:
    if len(palettes) > args.palette_count[0]:
        raise Exception("Converter couldn't reach the set palette count."
                        f" Current count = {len(palettes)},"
                        f" Limit = {args.palette_count[0]}")


############################# merge equal tiles ###############################

mt_count = 0
dfunc_v = np.vectorize(cu.col_dist_2_with_a, signature='(n),(n)->()')
mt_th_2 = args.merge_tiles[0] ** 2.0
mean_th = args.merge_tiles[1:]

if verbose_mode:
    print(f"Merging equal tiles...")

for tbank in [tiles, masked_tiles]:
    for tile in tbank:
        # calculate means
        tile_colors = palettes[tile.pal_indx].data[tile.data]
        tile.mean = np.mean(tile_colors, axis=(0,1))

tbank_indx = 0
for tbank in [tiles, masked_tiles]:
  is_masked = (tbank_indx == 1)
  tbank_indx += 1
  if verbose_mode:
      print(f"Working on tile bank #{tbank_indx}...")
  for i in range(len(tbank)):
    if verbose_mode:
        if i % tx == tx - 1:
            print(f"({i} / {len(tbank)}, {mt_count} replaced)")
    if tbank[i].replaced:
        continue
    ti = palettes[tbank[i].pal_indx].data[tbank[i].data]
    for j in range(i + 1, len(tbank)):
        if tbank[j].replaced:
            continue
        if np.any(abs(tbank[i].mean - tbank[j].mean) > mean_th):
            continue
        flip_h = False
        flip_v = False
        # no flips
        tj = palettes[tbank[j].pal_indx].data[tbank[j].data]
        d2_sum = np.sum(dfunc_v(ti, tj)) / (8.0 * 8.0)
        min_sum = d2_sum
        if not args.collision_map:
            # V-flip
            tj = palettes[tbank[j].pal_indx].data[np.flip(tbank[j].data, 0)]
            d2_sum = (np.sum(dfunc_v(ti, tj)) / (8.0 * 8.0)) if d2_sum>0.0 else 0.0
            if min_sum > d2_sum:
                min_sum = d2_sum
                flip_v = True
            # H-flip
            tj = palettes[tbank[j].pal_indx].data[np.flip(tbank[j].data, 1)]
            d2_sum = (np.sum(dfunc_v(ti, tj)) / (8.0 * 8.0)) if d2_sum>0.0 else 0.0
            if min_sum > d2_sum:
                min_sum = d2_sum
                flip_h = True
                flip_v = False
            # VH-flip
            tj = palettes[tbank[j].pal_indx].data[np.flip(tbank[j].data)]
            d2_sum = (np.sum(dfunc_v(ti, tj)) / (8.0 * 8.0)) if d2_sum>0.0 else 0.0
            if min_sum > d2_sum:
                flip_h = True
                flip_v = True
        # merge when it's within the threshold
        if True: #not is_masked:
            if min_sum <= mt_th_2:
                tbank[j].replaced = True
                tbank[j].replace_indx = i
                tbank[j].flip_h = flip_h
                tbank[j].flip_v = flip_v
                mt_count += 1
                continue
    #print(f"({i} / {len(tbank)}, {mt_count} replaced)")

if verbose_mode:
    size_red = mt_count * 32
    red_p = 100.0 * mt_count / (tx * ty * (2 if args.depth_mask else 1))
    print(f"...DONE ({mt_count} tiles replaced, "
          f"reduced {size_red} bytes ({red_p}:.3f %))\n")


############################ save result as image #############################

def get_final_tile(tile, tbank):
    if tile.replaced == False:
        return tile
    return get_final_tile(tbank[tile.replace_indx], tbank)

if args.save_as_image:
  if args.save_as_image[0] != '':
    img = Image.new('RGBA', [sx,sy])
    for tile in tiles:
        if tile.x < 0: continue # skip zero tile
        t = get_final_tile(tile, tiles)
        # fixing the incorrect alpha
        # TODO: fix this bug
        t.data[t.data_original==0] = 0
        timg = Image.fromarray(t.data.astype(np.uint8), mode='P')
        if tile.flip_v:
            timg = timg.transpose(method=Image.Transpose.FLIP_TOP_BOTTOM)
        if tile.flip_h:
            timg = timg.transpose(method=Image.Transpose.FLIP_LEFT_RIGHT)
        timg.putpalette(palettes[t.pal_indx].data.flatten(), rawmode='RGBA')
        img.paste(timg, box=[8*tile.x, 8*tile.y])
    if verbose_mode:
        print(f"Saving the debug image into `{args.save_as_image[0]}`.")
    img.save(args.save_as_image[0])


##################### count the number of unique tiles ########################

unique_tile_count = 0
unique_mtile_count = 0
for tile in tiles:
    if tile.replaced: continue
    tile.unique_id = unique_tile_count
    unique_tile_count = unique_tile_count + 1
for tile in masked_tiles:
    if tile.replaced: continue
    tile.unique_id = unique_mtile_count
    unique_mtile_count = unique_mtile_count + 1

if verbose_mode:
    print(f"Unique tile count: {unique_tile_count}\n")
    if args.depth_mask:
        print(f"Unique masked tile count: {unique_mtile_count}\n")

if throw_error:
    if args.tile_count:
        if unique_tile_count > args.tile_count[0]:
            raise ValueError(
                    f"Unique tile count ({unique_tile_count}) is greater then"
                    f" the set maximum tile count ({args.tile_count[0]}).")
        if unique_mtile_count > args.tile_count[0]:
            raise ValueError(
                    f"Unique masked tile count ({unique_mtile_count}) is greater then"
                    f" the set maximum tile count ({args.tile_count[0]}).")


############################ save result cpp file #############################

if verbose_mode:
    print("Generating CPP file...")

map_name = args.name
pal_arr_name = map_name + "_palettes"
pal_count_name = map_name + "_pcount"
pal_offset_name = map_name + "_poffset"
tile_arr_name = map_name + "_tiles"
mtile_arr_name = map_name + "_mtiles"
tile_count_name = map_name + "_tcount"
mtile_count_name = map_name + "_mtcount"
tile_offset_name = map_name + "_toffset"
map_tsize_x_name = map_name + "_tsize_x"
map_tsize_y_name = map_name + "_tsize_y"
map_t2pow_x_name = map_name + "_t2pow_x"
map_t2pow_y_name = map_name + "_t2pow_y"
sb_count_name = map_name + "_sb_count"
sb_array_name = map_name + "_sb_array"
sbm_array_name = map_name + "_sbm_array"

cpp = cu.get_def_cpp_header(
        args.in_map_img, args.namespace, args.main_module + "." + args.name)


# save map size info

cpp += f"// map width in tiles\n"
cpp += f"export constexpr u32 {map_tsize_x_name} = {tx};\n\n"

cpp += f"// map height in tiles\n"
cpp += f"export constexpr u32 {map_tsize_y_name} = {ty};\n\n"

cpp += f"// palette count\n"
cpp += f"export constexpr u32 {pal_count_name} = {len(palettes)};\n\n"

cpp += f"// palette offset\n"
cpp += f"export constexpr u32 {pal_offset_name} = {args.palette_offset[0]};\n\n"

cpp += f"// unique tile count\n"
cpp += f"export constexpr u32 {tile_count_name} = {unique_tile_count};\n\n"

if args.depth_mask:
    cpp += f"// unique masked tile count\n"
    cpp += f"export constexpr u32 {mtile_count_name} = {unique_mtile_count};\n\n"

cpp += f"// tile offset inside the charblock\n"
cpp += f"export constexpr u32 {tile_offset_name} = {tile_offset};\n\n"

cpp += f"// screenblock count (=tsize_x*tsize_y)\n"
cpp += f"export constexpr u32 {sb_count_name} = {tx * ty};\n\n"


# save palettes

cpp += f"export constexpr Palette {pal_arr_name}[{pal_count_name}] = {{\n"
for i in range(len(palettes)):
    cpp += cu.get_palette_cpp_code(palettes[i].data, pre='    ')
    if i != len(palettes) - 1:
        cpp += ",\n"
cpp += " };\n\n"


# save unique tiles

cpp += f"export constexpr Tile_4 {tile_arr_name}[{tile_count_name}] = {{\n"
for i in range(len(tiles)):
    if tiles[i].replaced:
        continue
    # fixing the incorrect alpha
    # TODO: fix this bug
    tiles[i].data[tiles[i].data_original==0] = 0
    cpp += cu.get_tile_cpp_code(tiles[i].data.flatten().astype(np.uint32))
    if i != (len(tiles) - 1):
        cpp += ",\n"
cpp += f" }};\n\n"

if args.depth_mask:
    cpp += f"export constexpr Tile_4 {mtile_arr_name}[{mtile_count_name}] = {{\n"
    for i in range(len(masked_tiles)):
        if masked_tiles[i].replaced:
            continue
        # fixing the incorrect alpha
        # TODO: fix this bug
        masked_tiles[i].data.flags.writeable = True
        masked_tiles[i].data[masked_tiles[i].data_original==0] = 0
        cpp += cu.get_tile_cpp_code(masked_tiles[i].data.flatten().astype(np.uint32))
        if i != (len(masked_tiles) - 1):
            cpp += ",\n"
    cpp += f" }};\n\n"


# save screenblock data


sbdata = [(tiles, sb_array_name)]
if args.depth_mask:
    sbdata.append((masked_tiles, sbm_array_name))

for (tbank, arr_name) in sbdata:
  sb_array_size = len(tbank) - (1 if args.add_zero_empty_tile else 0)
  cpp += f"// each `u32` value contains two `u16` values ([1] << 16 | [2]):\n"
  cpp += f"// [1] (pallette_index in `{pal_arr_name}` + palette_offset)(vflip)(hflip)(0)\n"
  cpp += f"// [2] (tile index in `{tile_arr_name}`)\n"
  cpp += f"// *([1] is in the regular GBA screenblock format)\n"
  cpp += f"export constexpr u32 {arr_name}[{sb_array_size}] = {{\n    "
  for i in range(len(tbank)):
    t = tbank[i]
    if t.x < 0: continue # skip zero tiles
    r = int(t.pal_indx + args.palette_offset[0]) << 12;
    if t.flip_h:
        r = r | int(1 << 10)
    if t.flip_v:
        r = r | int(1 << 11)
    r = (r << 16) | int(get_final_tile(t, tbank).unique_id + tile_offset)
    cpp += f"0x{r:08x}"
    if i != len(tbank) - 1: 
        cpp += ", "
    if i % 5 == 4: 
        cpp += "\n    "
  cpp += " };\n\n"


# closing up the cpp file

cpp += cu.get_def_cpp_ending(args.namespace)

if verbose_mode:
    print(f"...OK\nWriting into `{args.out_cpp_file}`...")

with open(args.out_cpp_file, "w") as f:
    f.write(cpp)

if verbose_mode:
    print(f"...DONE\n")


exit(0)

