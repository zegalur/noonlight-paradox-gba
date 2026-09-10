${DEVKITARM}/bin/arm-none-eabi-cmake -GNinja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -B build .
ninja -C build
${DEVKITARM}/bin/arm-none-eabi-objcopy -O binary build/game.elf bin/game.gba
gbafix bin/game.gba
cp COPYING.NEWLIB bin/COPYING.NEWLIB
