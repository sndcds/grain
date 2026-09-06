cmake_minimum_required(VERSION 3.30)

project(
        Grain
        VERSION 0.1.0
        DESCRIPTION "Modern C++ foundation for the Grain framework"
        LANGUAGES CXX
)

# ---------------------------------------------------------------------------
# Project options
# ---------------------------------------------------------------------------

option(GRAIN_BUILD_TESTS
        "Build tests"
        ON
)

option(GRAIN_BUILD_EXAMPLES
        "Build examples"
        ON
)

option(GRAIN_ENABLE_SANITIZERS
        "Enable AddressSanitizer and UndefinedBehaviorSanitizer"
        OFF
)

option(GRAIN_ENABLE_CLANG_TIDY
        "Run clang-tidy during compilation"
        OFF
)

option(GRAIN_WARNINGS_AS_ERRORS
        "Treat compiler warnings as errors"
        OFF
)

# ---------------------------------------------------------------------------
# C++ standard
# ---------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate compile_commands.json for clang tooling.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------------------------------------------
# CPM
# ---------------------------------------------------------------------------

include(cmake/CPM.cmake)

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------

if(GRAIN_BUILD_TESTS)

    CPMAddPackage(
            "gh:catchorg/Catch2@3.8.1"
    )

endif()

# ---------------------------------------------------------------------------
# Compiler configuration
# ---------------------------------------------------------------------------

add_library(GrainWarnings INTERFACE)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")

    target_compile_options(
            GrainWarnings
            INTERFACE
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wsign-conversion
            -Wshadow
            -Wnon-virtual-dtor
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
            -Wimplicit-fallthrough
            -Woverloaded-virtual
            -Wold-style-cast
            -Wcast-align
            -Wextra-semi
            -Wundef
    )

elseif(MSVC)

    target_compile_options(
            GrainWarnings
            INTERFACE
            /W4
            /permissive-
    )

endif()

if(GRAIN_WARNINGS_AS_ERRORS)

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")

        target_compile_options(
                GrainWarnings
                INTERFACE
                -Werror
        )

    elseif(MSVC)

        target_compile_options(
                GrainWarnings
                INTERFACE
                /WX
        )

    endif()

endif()

# ---------------------------------------------------------------------------
# Library
# ---------------------------------------------------------------------------

add_library(Grain)

target_sources(
        Grain
        PRIVATE
        src/Core/ObjectBase.cpp
        src/UI/View.cpp

        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
        include
        FILES
        include/Grain.hpp
        include/grain/Core/ObjectBase.hpp
        include/grain/Containers/List.hpp
        include/grain/Platform/App.hpp
        include/grain/Platform/Window.hpp
        include/grain/UI/View.hpp
        include/grain/Graphics/GraphicContext.hpp
        include/grain/Geometry/Rect.hpp
)

# ---------------------------------------------------------------------------
# Platform implementation
# ---------------------------------------------------------------------------

if(APPLE)

    enable_language(OBJCXX)

    target_sources(
            Grain
            PRIVATE
            src/Platform/macOS/App.mm
            src/Platform/macOS/Window.mm
            src/Platform/macOS/GrainRootView.mm
            src/Platform/macOS/CoreGraphicContext.mm
    )

    target_link_libraries(
            Grain
            PUBLIC
            "-framework AppKit"
            "-framework Metal"
            "-framework QuartzCore"
    )

elseif(WIN32)

elseif(UNIX)

endif()

target_include_directories(
        Grain
        PUBLIC
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:include>"
)

target_compile_features(
        Grain
        PUBLIC
        cxx_std_23
)

target_link_libraries(
        Grain
        PRIVATE
        GrainWarnings
)

target_include_directories(
        Grain
        PUBLIC
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:include>"
)

target_compile_features(
        Grain
        PUBLIC
        cxx_std_23
)

target_link_libraries(
        Grain
        PRIVATE
        GrainWarnings
)

# ---------------------------------------------------------------------------
# Sanitizers
# ---------------------------------------------------------------------------

if(GRAIN_ENABLE_SANITIZERS)

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")

        add_library(GrainSanitizers INTERFACE)

        target_compile_options(
                GrainSanitizers
                INTERFACE
                -fsanitize=address,undefined
                -fno-omit-frame-pointer
        )

        target_link_options(
                GrainSanitizers
                INTERFACE
                -fsanitize=address,undefined
        )

        target_link_libraries(
                Grain
                PUBLIC
                GrainSanitizers
        )

    else()

        message(
                WARNING
                "GRAIN_ENABLE_SANITIZERS is not supported for this compiler"
        )

    endif()

endif()

# ---------------------------------------------------------------------------
# clang-tidy
# ---------------------------------------------------------------------------

if(GRAIN_ENABLE_CLANG_TIDY)

    find_program(
            CLANG_TIDY
            NAMES clang-tidy
            REQUIRED
    )

    set_target_properties(
            Grain
            PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY}"
    )

endif()

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------

if(GRAIN_BUILD_TESTS)

    enable_testing()

    add_subdirectory(tests)

endif()

# ---------------------------------------------------------------------------
# Examples
# ---------------------------------------------------------------------------

if(GRAIN_BUILD_EXAMPLES)

    add_subdirectory(examples/basic)

endif()