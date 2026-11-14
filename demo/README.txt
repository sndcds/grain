//------------------------------------------------------------------------------
//  Build libGrain.a - Debug
//------------------------------------------------------------------------------

cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DGRAIN_BUILD_TESTS=OFF \
  -DGRAIN_BUILD_EXAMPLES=OFF

cmake --build build --target Grain -j


//------------------------------------------------------------------------------
//  Build libGrain.a - Release
//------------------------------------------------------------------------------

cmake -S . -B build-release \
  -DCMAKE_BUILD_TYPE=Release \
  -DGRAIN_BUILD_TESTS=OFF \
  -DGRAIN_BUILD_EXAMPLES=OFF

cmake --build build-release --target Grain -j


//------------------------------------------------------------------------------
//  Build demos
//------------------------------------------------------------------------------

cmake -S demo -B build-demos -DCMAKE_BUILD_TYPE=Release
cmake --build build-demos --parallel

