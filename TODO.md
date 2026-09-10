# Noonlight Paradox

Noonlight Paradox TODO list.

### Todo

- [ ] Make better use of `engine/options.cppm`.
- [ ] `-nostdlib` and `-nodefaultlibs` in full power.
- [ ] Add `animated` component and `animator` system.
- [ ] Add `switch_camera` for the full camera switching.
- [ ] Find out why in some very rare cases the game runs painstakingly slow on some emulator (alignment maybe?)
- [ ] Add a general case `change_animation` function.
- [ ] Add support for level ups, like weapon upgrades.
- [ ] Optimize `find_first_from()`.
- [ ] Optimize `alloc_obj_tiles()`.
- [ ] Add run-time errors messages (for allocation errors etc.)
- [ ] Better particle system.
- [ ] Fix a bug in `sys::director::redraw_all()`.
- [ ] Fix a bug in `map_to_cpp.py` that causes incorrect alpha.
- [ ] Fix a bug in `map_to_cpp.py` that causes incorrect palette merging.
- [ ] Fix a bug in `map_to_cpp.py` that causes incorrect foreground tile colors.
- [ ] Refactor `map_info_to_cpp.py`.
- [ ] Optimize the `update_tilemask()` with shift support.
- [ ] Rewrite the player code.
    - [ ] Add rift option to the long jump (when right movement b is pressed.)
    - [ ] Add slight fly-ing control when long jump.
    - [ ] Add other weapon support.

### In Progress

### Done ✓

- [x] (converter_utils.py) Round to the closest color using a perceived distance.
- [x] For `Tilemaps`, do not copy empty cells.
- [x] Blender as a map editor.
- [x] A small fully playable demo.
