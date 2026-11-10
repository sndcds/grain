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
//  Compile program.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Weighted Samples Demo.
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


//------------------------------------------------------------------------------
//  Image Demo.
//------------------------------------------------------------------------------

clang++ -std=c++2b \
    -O3 \
    -DNDEBUG \
    -I./include \
    -I"$(brew --prefix jpeg-turbo)/include" \
    -I"$(brew --prefix libpng)/include" \
    -I"$(brew --prefix webp)/include" \
    ./examples/Image/Image.cpp \
    build-release/libGrain.a \
    -L"$(brew --prefix libpng)/lib" \
    -lpng \
    -L"$(brew --prefix jpeg-turbo)/lib" \
    -ljpeg \
    -L"$(brew --prefix webp)/lib" \
    -lwebp \
    -framework AppKit \
    -framework CoreGraphics \
    -framework CoreText \
    -framework CoreFoundation \
    -framework CoreVideo \
    -o image


//------------------------------------------------------------------------------
//  Movie Demo.
//------------------------------------------------------------------------------

clang++ -std=c++2b \
    -O3 \
    -DNDEBUG \
    -I./include \
    -I"$(brew --prefix jpeg-turbo)/include" \
    -I"$(brew --prefix libpng)/include" \
    -I"$(brew --prefix webp)/include" \
    -I"$(brew --prefix ffmpeg)/include" \
    ./examples/Movie/MovieExample.cpp \
    build-release/libGrain.a \
    -L"$(brew --prefix libpng)/lib" \
    -lpng \
    -L"$(brew --prefix jpeg-turbo)/lib" \
    -ljpeg \
    -L"$(brew --prefix webp)/lib" \
    -lwebp \
    -L"$(brew --prefix ffmpeg)/lib" \
    -lavformat \
    -lavcodec \
    -lavutil \
    -lswscale \
    -framework AppKit \
    -framework CoreGraphics \
    -framework CoreText \
    -framework CoreFoundation \
    -framework CoreVideo \
    -o movie


//------------------------------------------------------------------------------
//  Audio Demo.
//------------------------------------------------------------------------------

clang++ -std=c++2b \
    -O3 \
    -DNDEBUG \
    -I./include \
    -I"$(brew --prefix jpeg-turbo)/include" \
    -I"$(brew --prefix libpng)/include" \
    -I"$(brew --prefix webp)/include" \
    -I"$(brew --prefix ffmpeg)/include" \
    ./examples/Audio/AudioDemo.cpp \
    build-release/libGrain.a \
    -L"$(brew --prefix libpng)/lib" \
    -lpng \
    -L"$(brew --prefix jpeg-turbo)/lib" \
    -ljpeg \
    -L"$(brew --prefix webp)/lib" \
    -lwebp \
    -L"$(brew --prefix ffmpeg)/lib" \
    -lavformat \
    -lavcodec \
    -lavutil \
    -lswscale \
    -lswresample \
    -framework AppKit \
    -framework CoreGraphics \
    -framework CoreText \
    -framework CoreFoundation \
    -framework CoreVideo \
    -o audio_demo

