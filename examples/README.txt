//------------------------------------------------------------------------------
//  Build libGrain.a - Debug
//------------------------------------------------------------------------------

cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DGRAIN_BUILD_TESTS=OFF \
  -DGRAIN_BUILD_EXAMPLES=OFF

cmake --build build --target Grain -j

clang++ -std=c++2b \
  -I./include/ \
  ./examples/WeightedSamples/WeightedSamples.cpp \
  build/libGrain.a \
  -framework AppKit \
  -framework CoreGraphics \
  -framework CoreVideo \
  -o weighted_samples_demo


//------------------------------------------------------------------------------
//  Build libGrain.a - Release
//------------------------------------------------------------------------------

cmake -S . -B build-release \
  -DCMAKE_BUILD_TYPE=Release \
  -DGRAIN_BUILD_TESTS=OFF \
  -DGRAIN_BUILD_EXAMPLES=OFF

cmake --build build-release --target Grain -j


//------------------------------------------------------------------------------
//  Compile program
//------------------------------------------------------------------------------

clang++ -std=c++2b \
  -O3 \
  -DNDEBUG \
  -I./include \
  ./examples/WeightedSamples/WeightedSamples.cpp \
  build-release/libGrain.a \
  -framework AppKit \
  -framework CoreGraphics \
  -framework CoreVideo \
  -o weighted_samples_demo


clang++ -std=c++2b \
  -O3 \
  -DNDEBUG \
  -I./include \
  ./examples/Image/Image.cpp \
  build-release/libGrain.a \
  -framework AppKit \
  -framework CoreGraphics \
  -framework CoreVideo \
  -o weighted_samples_demo

