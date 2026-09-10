
# TODO: Split this script and generalize

from dataclasses import dataclass
import converter_utils as cu
import argparse

# create an argument parser

parser = argparse.ArgumentParser(
        description="Converts map info files into C++ file.")

parser.add_argument('name'
        ,type=str
        ,help="The base name of the map."
        )

parser.add_argument('in_txt_file'
        ,type=str
        ,help="The input map info file path."
        )

cu.add_std_args(parser)

args = parser.parse_args()
verbose_mode = args.verbose


if verbose_mode:
    print(f"Opening `{args.in_txt_file}` image file...")

# read the data from the input txt file
data = {}
with open(args.in_txt_file, 'r', encoding="utf-8") as f:
    txt_data = f.read()
    data = eval(txt_data)

cp_data = []
tr_data = []
en_data = []


# -------------------------------- Entities --------------------------------- #

modules = set()
includes = set()
palettes = set()

cpp_includes = []
cpp_modules = []
cpp_palettes = ""
cpp_tiles = ""
cpp_spatials = ""
cpp_visuals = ""
cpp_health = ""
cpp_damage = ""
cpp_actions = ""
cpp_collision = ""

entity_count = 0
e_names = []

@dataclass
class Entity:
    includes : [str]
    modules : [str]
    palettes : [str]
    tile_pl_cmds : [str]
    spatial_cmds : [str]
    visual_cmds : [str]
    health_cmds : [str]
    damage_cmds : [str]
    action_cmds : [str]
    collision_cmds : [str]
    indx_prefix : str # use this prefix for the sub-entities
    size : int
    count = 0

entities = { 
    'GREEN_KEY' : Entity(
        ["game/entities/key.hpp"],
        ["data.key"],
        ["ADD_KEY_PAL"],
        ["ADD_GREEN_KEY_PLACEHOLDER"],
        ["ADD_KEY_SPATIAL"],
        ["ADD_GREEN_KEY_VISUAL"],
        ["ADD_KEY_HEALTH"],
        ["ADD_KEY_DAMAGE"],
        ["ADD_GREEN_KEY_ACTION"],
        ["ADD_KEY_COLLISION"],
        "", 1
        ),
    'BLUE_KEY' : Entity(
        ["game/entities/key.hpp"],
        ["data.key"],
        ["ADD_KEY_PAL"],
        ["ADD_BLUE_KEY_PLACEHOLDER"],
        ["ADD_KEY_SPATIAL"],
        ["ADD_BLUE_KEY_VISUAL"],
        ["ADD_KEY_HEALTH"],
        ["ADD_KEY_DAMAGE"],
        ["ADD_BLUE_KEY_ACTION"],
        ["ADD_KEY_COLLISION"],
        "", 1
        ),
    'GOLD_KEY' : Entity(
        ["game/entities/key.hpp"],
        ["data.key"],
        ["ADD_KEY_PAL"],
        ["ADD_GOLD_KEY_PLACEHOLDER"],
        ["ADD_KEY_SPATIAL"],
        ["ADD_GOLD_KEY_VISUAL"],
        ["ADD_KEY_HEALTH"],
        ["ADD_KEY_DAMAGE"],
        ["ADD_GOLD_KEY_ACTION"],
        ["ADD_KEY_COLLISION"],
        "", 1
        ),
    'IZONE' : Entity(
        ["game/entities/izone.hpp"],
        ["data.mine_01", "data.izone"],
        ["ADD_MINE_PAL"],
        ["ADD_IZONE_PLACEHOLDER"],
        ["ADD_IZONE_SPATIAL"],
        ["ADD_IZONE_VISUAL"],
        ["ADD_IZONE_HEALTH"],
        ["ADD_IZONE_DAMAGE"],
        ["ADD_IZONE_ACTION"],
        ["ADD_IZONE_COLLISION"],
        "", 1
        ),
    'CARGO' : Entity(
        ["game/entities/cargo.hpp"],
        ["data.mine_01", "data.cargo"],
        ["ADD_MINE_PAL"],
        ["ADD_CARGO_PLACEHOLDER"],
        ["ADD_CARGO_SPATIAL"],
        ["ADD_CARGO_VISUAL"],
        ["ADD_CARGO_HEALTH"],
        ["ADD_CARGO_DAMAGE"],
        ["ADD_CARGO_ACTION"],
        ["ADD_CARGO_COLLISION"],
        "", 1
        ),
    'SWITCH_01_ON' : Entity(
        ["game/entities/switch.hpp"],
        ["data.mine_01", "data.switch_01"],
        ["ADD_MINE_PAL"],
        ["ADD_SWITCH_01_ON_PLACEHOLDER"],
        ["ADD_SWITCH_01_SPATIAL"],
        ["ADD_SWITCH_01_ON_VISUAL"],
        ["ADD_SWITCH_01_HEALTH"],
        ["ADD_SWITCH_01_DAMAGE"],
        ["ADD_SWITCH_01_ON_ACTION"],
        ["ADD_SWITCH_01_COLLISION"],
        "", 1
        ),
    'SWITCH_01_OFF' : Entity(
        ["game/entities/switch.hpp"],
        ["data.mine_01", "data.switch_01"],
        ["ADD_MINE_PAL"],
        ["ADD_SWITCH_01_OFF_PLACEHOLDER"],
        ["ADD_SWITCH_01_SPATIAL"],
        ["ADD_SWITCH_01_OFF_VISUAL"],
        ["ADD_SWITCH_01_HEALTH"],
        ["ADD_SWITCH_01_DAMAGE"],
        ["ADD_SWITCH_01_OFF_ACTION"],
        ["ADD_SWITCH_01_COLLISION"],
        "", 1
        ),
    'BATTERY' : Entity(
        ["game/entities/battery.hpp"],
        ["data.items_palette", "data.green_battery"],
        ["ADD_ITEMS_PALETTE"],
        ["ADD_BATTERY_PLACEHOLDER"],
        ["ADD_BATTERY_SPATIAL"],
        ["ADD_BATTERY_VISUAL"],
        ["ADD_BATTERY_HEALTH"],
        ["ADD_BATTERY_DAMAGE"],
        ["ADD_BATTERY_ACTION"],
        ["ADD_BATTERY_COLLISION"],
        "", 1
        ),
    'MINE_01' : Entity(
        ["game/entities/mine.hpp"],
        ["data.mine_01"],
        ["ADD_MINE_PAL"],
        ["ADD_MINE_01_PLACEHOLDER"],
        ["ADD_MINE_01_SPATIAL"],
        ["ADD_MINE_01_VISUAL"],
        ["ADD_MINE_01_HEALTH"],
        ["ADD_MINE_01_DAMAGE"],
        ["ADD_MINE_01_ACTION"],
        ["ADD_MINE_01_COLLISION"],
        "", 1
        ),
    'SPIDER' : Entity(
        ["game/entities/mine.hpp", "game/entities/spider.hpp"],
        ["data.mine_01", "data.spider"],
        ["ADD_MINE_PAL"],
        ["ADD_SPIDER_PLACEHOLDER",  "ADD_MINE_01_PLACEHOLDER"],
        ["ADD_SPIDER_SPATIAL",      "ADD_MINE_01_SPATIAL_HIDDEN"],
        ["ADD_SPIDER_VISUAL",       "ADD_MINE_01_VISUAL"],
        ["ADD_SPIDER_HEALTH",       "ADD_MINE_01_HEALTH_ONESHOT"],
        ["ADD_SPIDER_DAMAGE",       "ADD_MINE_01_DAMAGE"],
        ["ADD_SPIDER_ACTION",       "ADD_MINE_01_ACTION"],
        ["ADD_SPIDER_COLLISION",    "ADD_MINE_01_COLLISION"],
        "spider_", 2
        ),
    }


def add_entity(name:str, e:Entity, px:int, py:int, visible:bool):
    global modules
    global includes
    global palettes
    global cpp_includes
    global cpp_modules
    global cpp_palettes
    global cpp_tiles
    global cpp_spatials
    global cpp_visuals
    global cpp_health
    global cpp_damage
    global cpp_actions
    global cpp_collision
    global entity_count
    indx_original = e.count + 1
    e.count += 1
    e_names.append((name, entity_count))
    entity_count += e.size
    # check if the number of sub-entities are consistent across the components
    if len(e.spatial_cmds) != len(e.visual_cmds):
        raise ValueError("Inconsistent number of entities.")
    # includes
    for i in e.includes:
        if i in includes: continue
        cpp_includes.append(f'../../../../src/{i}')
        includes.add(i)
    # modules
    for m in e.modules:
        if m in modules: continue
        cpp_modules.append(m)
        modules.add(m)
    # palettes
    for p in e.palettes:
        if p in palettes: continue
        cpp_palettes += f'{p};\n'
        palettes.add(p)
    # tile placeholders
    indx = f"{indx_original}"
    for t in e.tile_pl_cmds:
        cpp_tiles += f'{t}({indx});\n'
        indx = e.indx_prefix + indx
    # spatials
    visible_cpp = "true" if visible else "false"
    for s in e.spatial_cmds:
        cpp_spatials += f'        {s}({px}, {py}, {visible_cpp}),\n'
    # visuals
    indx = f"{indx_original}"
    for v in e.visual_cmds:
        cpp_visuals += f'        {v}({indx}),\n'
        indx = e.indx_prefix + indx
    # health
    indx = f"{indx_original}"
    for h in e.health_cmds:
        cpp_health += f'        {h}({indx}),\n'
        indx = e.indx_prefix + indx
    # damage
    indx = f"{indx_original}"
    for d in e.damage_cmds:
        cpp_damage += f'        {d}({indx}),\n'
        indx = e.indx_prefix + indx
    # actions
    indx = f"{indx_original}"
    for a in e.action_cmds:
        cpp_actions += f'        {a}({indx}),\n'
        indx = e.indx_prefix + indx
    # collision
    indx = f"{indx_original}"
    for c in e.collision_cmds:
        cpp_collision += f'        {c}({indx}),\n'
        indx = e.indx_prefix + indx


# Process the entities
for en_name, en_v in data['entities'].items():
    en_v['name'] = en_name
    en_data.append(en_v)
    add_entity(en_name, entities[en_v['type']], 
               en_v['position'][0], en_v['position'][1], True)


# -------------------------- Export as CPP module --------------------------- #

if verbose_mode:
    print(f"Generating CPP file...")

checkpoint_enum_name = "CP_" + args.name
triggers_enum_name = "TR_" + args.name
cp_count_var = args.name + "_cp_count"
tr_count_var = args.name + "_tr_count"
checkpoints_var = args.name + "_checkpoints"
triggers_var = args.name + "_triggers"
map_data_var = args.name + "_data"
entities_struct = "Entities_" + args.name
entity_names = "E_" + args.name

cpp = cu.get_def_cpp_header(
        args.in_txt_file, 
        args.namespace, 
        args.main_module + "." + args.name + ".info",
        ["data.map_data"] + cpp_modules, 
        cpp_includes );

if len(cpp_palettes) > 0:
    cpp += "// palettes\n" + cpp_palettes + "\n"
if len(cpp_tiles) > 0:
    cpp += "// tiles\n" + cpp_tiles + "\n"

cpp += "// entity name to entity first sub-entity index\n"
cpp += f"export enum {entity_names} {{\n"
for (n,i) in e_names:
    cpp += f"    {n} = {i},\n"
cpp += "};\n\n"

cpp += "// map entities data\n"
cpp += f"export struct {entities_struct} {{\n"
cpp += f"    // entities count\n"
cpp += f"    static const u32 E_COUNT = {entity_count};\n\n"
cpp += f"    comp::Spatial spatials[E_COUNT] = {{\n{cpp_spatials}    }};\n\n"
cpp += f"    comp::Visual visuals[E_COUNT] = {{\n{cpp_visuals}    }};\n\n"
cpp += f"    comp::Health health[E_COUNT] = {{\n{cpp_health}    }};\n\n"
cpp += f"    comp::Damage damage[E_COUNT] = {{\n{cpp_damage}    }};\n\n"
cpp += f"    comp::Action actions[E_COUNT] = {{\n{cpp_actions}    }};\n"
cpp += f"    comp::Collision collision[E_COUNT] = {{\n{cpp_collision}    }};\n"
cpp += "};\n\n"


# ------------------------------- Checkpoints ------------------------------- #

# gather checkpoints
cpp += f"export enum {checkpoint_enum_name} {{\n"
for cp_name, cp_v in data['checkpoints'].items():
    cpp += f"    {cp_name} = {len(cp_data)},\n"
    cp_v['name'] = cp_name
    cp_data.append(cp_v)
cpp += "};\n\n"

# write checkpoint data
cpp += "// checkpoint count\n"
cpp += f"export constexpr u32 {cp_count_var} = {len(cp_data)};\n\n"

cpp += f"export const CheckPoint"
cpp += f" {checkpoints_var}[{cp_count_var}] = {{\n"
for i in range(len(cp_data)):
    cpp += f"    {{ {cp_data[i]['position'][0]}, {cp_data[i]['position'][1]} }}"
    if i != len(cp_data) - 1:
        cpp += ","
    cpp += "\n"
cpp += "};\n\n"


# -------------------------------- Triggers --------------------------------- #

# gather triggers
cpp += f"export enum {triggers_enum_name} {{\n"
for tr_name, tr_v in data['triggers'].items():
    cpp += f"    {tr_name} = {len(tr_data)},\n"
    tr_v['name'] = tr_name
    tr_data.append(tr_v)
cpp += "};\n\n"

# write triggers data
cpp += "// trigger count\n"
cpp += f"export constexpr u32 {tr_count_var} = {len(tr_data)};\n\n"

cpp += f"export const Trigger"
cpp += f" {triggers_var}[{tr_count_var}] = {{\n"
for i in range(len(tr_data)):
    cpp += f"    {{ {tr_data[i]['position'][0]}"
    cpp += f", {tr_data[i]['position'][1]}"
    cpp += f", i32({tr_data[i]['size']})"
    cpp += f", {'true' if tr_data[i]['oneshot'] else 'false'} }}"
    if i != len(tr_data) - 1:
        cpp += ","
    cpp += f" // {tr_data[i]['name']}"
    cpp += "\n"
cpp += "};\n\n"


# -------------------------------- Map Info --------------------------------- #

# write map data struct
cpp += f"export const MapData {map_data_var} = {{\n";
cpp += f"    {cp_count_var},\n"
cpp += f"    {checkpoints_var},\n"
cpp += f"    {tr_count_var},\n"
cpp += f"    {triggers_var}\n"
cpp += f"}};\n\n"

cpp += cu.get_def_cpp_ending(args.namespace)

if verbose_mode:
    print(f"...OK\nWriting into `{args.out_cpp_file}`...")

with open(args.out_cpp_file, "w") as f:
    f.write(cpp)

if verbose_mode:
    print(f"...DONE\n")
