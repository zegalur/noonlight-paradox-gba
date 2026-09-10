import bpy
import os

# USAGE:
#   1. Select the collection with the meta data.
#   2. Make sure the collection has "export_to" and "export_to_2" Custom Property where the export path specified.
#   3. Press "RUN".

# TODO:
# [ ] ...

TILE_SIZE = 0.25
MAP_SIZE_OBJ_NAME = "MapSize"
CHECKPOINTS_COL_NAME = "CheckPoints"
TRIGGERS_COL_NAME = "Triggers"
ENTITIES_COL_NAME = "Entities"
ENTITY_TYPE_PROP = "TYPE"

selected_collection = bpy.context.collection
active_camera = bpy.context.scene.camera
bbox_obj = selected_collection.objects[MAP_SIZE_OBJ_NAME]
export_to = selected_collection.get("export_to")
export_to_2 = selected_collection.get("export_to_2")

# get map bounding box
map_bbox = bbox_obj.bound_box
map_left, map_right = 0, 0
map_top, map_bottom = 0, 0
for p in map_bbox:
    x,z = p[0], p[2]
    if map_left > x: map_left = x
    if map_right < x: map_right = x
    if map_top < z: map_top = z
    if map_bottom > z: map_bottom = z

def to_map_pos(global_pos):
    x = int(((global_pos[0] - map_left)/TILE_SIZE)*8)
    y = int(((map_top - global_pos[2])/TILE_SIZE)*8)
    return [x,y]


# collect the check points
check_points = {}
if CHECKPOINTS_COL_NAME in selected_collection.children:
  print("Checkpoints:")
  for cp in selected_collection.children[CHECKPOINTS_COL_NAME].objects:
    cp_map_pos = to_map_pos(cp.matrix_world.translation)
    check_points[cp.name] = {
            "position" : cp_map_pos }
    print(f"    {cp.name}: {cp_map_pos}")


# collect trigger points
triggers = {}
if TRIGGERS_COL_NAME in selected_collection.children:
  print("Triggers:")
  for trigger in selected_collection.children[TRIGGERS_COL_NAME].objects:
    trigger_map_pos = to_map_pos(trigger.matrix_world.translation)
    oneshot = "ONESHOT" in trigger
    triggers[trigger.name] = {
            "position" : trigger_map_pos,
            "oneshot" : oneshot,
            "size" : int(trigger.empty_display_size * 32),
            }
    print(f"    {trigger.name}: {trigger_map_pos}")


# collect entities
entities = {}
if ENTITIES_COL_NAME in selected_collection.children:
  print("Entities:")
  for e in selected_collection.children[ENTITIES_COL_NAME].objects:
    e_map_pos = to_map_pos(e.matrix_world.translation)
    e_type = e[ENTITY_TYPE_PROP]
    entities[e.name] = {
            "position" : e_map_pos,
            "type" : e_type,
            }
    print(f"    {e.name}: {e_type}, {e_map_pos}")



# making the info object
map_info = {
    "map_size_tx" : int((map_right - map_left)/TILE_SIZE),
    "map_size_ty" : int((map_top - map_bottom)/TILE_SIZE),
    "checkpoints" : check_points,
    "triggers" : triggers,
    "entities" : entities,
}

# write the results
for path in [export_to, export_to_2]:
    with open(bpy.path.abspath(path), "w", encoding="utf-8") as f:
        f.write(str(map_info))

