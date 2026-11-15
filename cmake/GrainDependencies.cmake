
#--------------------------------------------------------------------------#
# Dependencies
#--------------------------------------------------------------------------#

find_package(libpqxx CONFIG REQUIRED)
find_package(CURL REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
pkg_check_modules(LUA REQUIRED IMPORTED_TARGET lua)

#--------------------------------------------------------------------------
# WebP
#--------------------------------------------------------------------------

execute_process(
        COMMAND brew --prefix webp
        OUTPUT_VARIABLE WEBP_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY
)

find_path(WEBP_INCLUDE_DIR
        NAMES webp/encode.h
        PATHS "${WEBP_PREFIX}/include"
        NO_DEFAULT_PATH
        REQUIRED
)

find_library(WEBP_LIBRARY
        NAMES webp
        PATHS "${WEBP_PREFIX}/lib"
        NO_DEFAULT_PATH
        REQUIRED
)

add_library(WebP::WebP UNKNOWN IMPORTED GLOBAL)

set_target_properties(WebP::WebP PROPERTIES
        IMPORTED_LOCATION "${WEBP_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${WEBP_INCLUDE_DIR}"
)


#--------------------------------------------------------------------------#
# FFmpeg
#--------------------------------------------------------------------------#

execute_process(
        COMMAND brew --prefix ffmpeg
        OUTPUT_VARIABLE FFMPEG_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY
)

find_path(FFMPEG_INCLUDE_DIR
        NAMES libavformat/avformat.h
        PATHS "${FFMPEG_PREFIX}/include"
        NO_DEFAULT_PATH
        REQUIRED
)

find_library(avformat_LIBRARY
        NAMES avformat
        PATHS "${FFMPEG_PREFIX}/lib"
        NO_DEFAULT_PATH
        REQUIRED
)

find_library(avcodec_LIBRARY
        NAMES avcodec
        PATHS "${FFMPEG_PREFIX}/lib"
        NO_DEFAULT_PATH
        REQUIRED
)

find_library(avutil_LIBRARY
        NAMES avutil
        PATHS "${FFMPEG_PREFIX}/lib"
        NO_DEFAULT_PATH
        REQUIRED
)

find_library(swscale_LIBRARY
        NAMES swscale
        PATHS "${FFMPEG_PREFIX}/lib"
        NO_DEFAULT_PATH
        REQUIRED
)

find_library(swresample_LIBRARY
        NAMES swresample
        PATHS "${FFMPEG_PREFIX}/lib"
        NO_DEFAULT_PATH
        REQUIRED
)

add_library(FFmpeg INTERFACE)

target_include_directories(FFmpeg INTERFACE
        "${FFMPEG_INCLUDE_DIR}"
)

target_link_libraries(FFmpeg INTERFACE
        "${avformat_LIBRARY}"
        "${avcodec_LIBRARY}"
        "${avutil_LIBRARY}"
        "${swscale_LIBRARY}"
        "${swresample_LIBRARY}"
)

#--------------------------------------------------------------------------#
# Grain library
#--------------------------------------------------------------------------#

add_library(Grain STATIC IMPORTED GLOBAL)

set_target_properties(Grain PROPERTIES
        IMPORTED_LOCATION
        "${CMAKE_CURRENT_LIST_DIR}/../build-release/libGrain.a"

        INTERFACE_INCLUDE_DIRECTORIES
        "${CMAKE_CURRENT_LIST_DIR}/../include"
)

target_compile_features(Grain INTERFACE cxx_std_23)

target_link_libraries(Grain INTERFACE
        pqxx
        CURL::libcurl
        PNG::PNG
        JPEG::JPEG
        WebP::WebP
        FFmpeg
        PkgConfig::LUA

        "-framework AppKit"
        "-framework CoreGraphics"
        "-framework CoreText"
        "-framework CoreFoundation"
        "-framework CoreVideo"
)
