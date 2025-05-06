Сборка и тесты

mkdir cmake-build-debug

cd cmake-build-debug

cmake -G "MinGW Makefiles" -DBUILD_TESTS=ON ..

cmake --build .

.\ClothingClientTests.exe
