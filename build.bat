cd build
cmake -G "Visual Studio 16 2019" -DMAYA_VERSION=2020 ../
cmake --build . --config Release --target Install
pause