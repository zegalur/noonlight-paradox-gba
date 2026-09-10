cmake -B build-clang -DCLANG=ON -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -GNinja -S . 
ninja -C build-clang
