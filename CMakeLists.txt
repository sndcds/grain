cmake_minimum_required(VERSION 3.30)

project(
        Grain
        VERSION 0.1.0
        DESCRIPTION "Modern C++ foundation for the Grain framework"
        LANGUAGES CXX
)

# ------------------------------------------------------------------------------
# Project options
# ------------------------------------------------------------------------------

option(
        GRAIN_BUILD_TESTS
        "Build tests"
        ON
)

option(
        GRAIN_BUILD_EXAMPLES
        "Build examples"
        ON
)

option(
        GRAIN_ENABLE_SANITIZERS
        "Enable AddressSanitizer and UndefinedBehaviorSanitizer"
        OFF
)

option(
        GRAIN_ENABLE_CLANG_TIDY
        "Run clang-tidy during compilation"
        OFF
)

option(
        GRAIN_WARNINGS_AS_ERRORS
        "Treat compiler warnings as errors"
        OFF
)

set(
        GRAIN_LINUX_BACKEND
        "X11"
        CACHE STRING
        "Linux backend: X11 or GTK4"
)

set_property(
        CACHE GRAIN_LINUX_BACKEND
        PROPERTY STRINGS X11 GTK4
)

# ------------------------------------------------------------------------------
# C++ standard
# ------------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_OBJCXX_STANDARD 23)
set(CMAKE_OBJCXX_STANDARD_REQUIRED ON)
set(CMAKE_OBJCXX_EXTENSIONS OFF)

# Generate compile_commands.json for clang tooling.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------------------------
# CPM
# ------------------------------------------------------------------------------

include(cmake/CPM.cmake)

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

if(GRAIN_BUILD_TESTS)

    CPMAddPackage(
            "gh:catchorg/Catch2@3.8.1"
    )

endif()

# ------------------------------------------------------------------------------
# Compiler warnings
# ------------------------------------------------------------------------------

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

# ------------------------------------------------------------------------------
# Sanitizers
# ------------------------------------------------------------------------------

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

    else()

        message(
                WARNING
                "GRAIN_ENABLE_SANITIZERS is not supported for this compiler"
        )

    endif()

endif()

# ------------------------------------------------------------------------------
# Library
# ------------------------------------------------------------------------------

add_library(Grain)

target_sources(
        Grain
        PRIVATE

        src/Core/ObjectBase.cpp

        src/Math/Math.cpp
        src/Math/Functions.cpp
        src/Math/Interpolation.cpp
        src/Math/Easing.cpp
        src/Math/Polynomial.cpp
        src/Math/Circle.cpp
        src/Math/Sphere.cpp
        src/Math/Geo.cpp
        src/Math/Optics.cpp
        src/Math/Quantization.cpp
        src/Math/Vec2.cpp
        src/Math/Quadrilateral.cpp

        src/String/String.cpp
        src/String/StringUtf8.cpp

        src/Geometry/Bezier.cpp

        src/Graphics/Color.cpp
        src/Graphics/GraphicPathPoint.cpp
        src/Graphics/GraphicPath.cpp
        src/Graphics/Font.cpp

        src/Input/Input.cpp

        src/Platform/View.cpp

        src/UI/Button.cpp


        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
        include
        FILES

        include/Grain.hpp

        include/grain/Core/ObjectBase.hpp

        include/grain/Math/Math.hpp
        include/grain/Math/Functions.hpp
        include/grain/Math/Statistics.hpp
        include/grain/Math/Interpolation.hpp
        include/grain/Math/Easing.hpp
        include/grain/Math/Polynomial.hpp
        include/grain/Math/Circle.hpp
        include/grain/Math/Sphere.hpp
        include/grain/Math/Geo.hpp
        include/grain/Math/Optics.hpp
        include/grain/Math/Quantization.hpp
        include/grain/Math/Scalar.hpp
        include/grain/Math/Vec2.hpp
        include/grain/Math/Vec3.hpp
        include/grain/Math/Mat3.hpp
        include/grain/Math/Quadrilateral.hpp

        include/grain/Geometry/Size.hpp
        include/grain/Geometry/Rect.hpp
        include/grain/Geometry/RemapRect.hpp
        include/grain/Geometry/RectLayout.hpp
        include/grain/Geometry/Bezier.hpp
        include/grain/Geometry/Layout.hpp

        include/grain/String/String.hpp
        include/grain/String/StringUtf8.hpp

        include/grain/Containers/List.hpp

        include/grain/Input/Input.hpp
        include/grain/Input/Event.hpp

        include/grain/Platform/App.hpp
        include/grain/Platform/Window.hpp
        include/grain/Platform/View.hpp

        include/grain/Graphics/Color.hpp
        include/grain/Graphics/Canvas.hpp
        include/grain/Graphics/GraphicState.hpp
        include/grain/Graphics/GraphicContext.hpp
        include/grain/Graphics/GraphicPathPoint.hpp
        include/grain/Graphics/GraphicPath.hpp
        include/grain/Graphics/Font.hpp

        include/grain/UI/Component.hpp
        include/grain/UI/Button.hpp
)

# ------------------------------------------------------------------------------
# Platform implementation
# ------------------------------------------------------------------------------

if(APPLE)

    enable_language(OBJCXX)

    target_sources(
            Grain
            PRIVATE

            src/Platform/App.cpp
            src/Platform/Window.cpp

            src/Platform/macOS/CocoaApp.mm
            src/Platform/macOS/CocoaWindow.mm
            src/Platform/macOS/CocoaView.mm
            src/Platform/macOS/CoreGraphicContext.mm
            src/Platform/macOS/CocoaFont.mm
            src/Platform/macOS/GrainRootView.mm
    )

    target_link_libraries(
            Grain
            PUBLIC
            "-framework AppKit"
            "-framework CoreGraphics"
    )

elseif(WIN32)

    # Windows backend not implemented yet.

elseif(UNIX AND NOT APPLE)

    find_package(PkgConfig REQUIRED)

    target_sources(
            Grain
            PRIVATE
            src/Platform/App.cpp
            src/Platform/Window.cpp
    )

    if(GRAIN_LINUX_BACKEND STREQUAL "GTK4")

        pkg_check_modules(
                GTK4
                REQUIRED
                IMPORTED_TARGET
                gtk4>=4.14
        )

        target_sources(
                Grain
                PRIVATE
                src/Platform/LinuxGTK4/Gtk4App.cpp
                src/Platform/LinuxGTK4/Gtk4Window.cpp
                src/Platform/LinuxGTK4/Gtk4GraphicContext.cpp
        )

        target_link_libraries(
                Grain
                PUBLIC
                PkgConfig::GTK4
        )

    elseif(GRAIN_LINUX_BACKEND STREQUAL "X11")

        pkg_check_modules(
                CAIRO
                REQUIRED
                IMPORTED_TARGET
                cairo
        )

        find_package(X11 REQUIRED)

        target_sources(
                Grain
                PRIVATE
                src/Platform/Linux/App.cpp
                src/Platform/Linux/Window.cpp
                src/Platform/Linux/CairoGraphicContext.cpp
        )

        target_link_libraries(
                Grain
                PUBLIC
                X11::X11
                PkgConfig::CAIRO
        )

    else()

        message(
                FATAL_ERROR
                "Unknown GRAIN_LINUX_BACKEND '${GRAIN_LINUX_BACKEND}'; "
                "expected X11 or GTK4"
        )

    endif()

endif()

# ------------------------------------------------------------------------------
# Library configuration
# ------------------------------------------------------------------------------

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

if(GRAIN_ENABLE_SANITIZERS)

    target_link_libraries(
            Grain
            PRIVATE
            GrainSanitizers
    )

endif()

# ------------------------------------------------------------------------------
# clang-tidy
# ------------------------------------------------------------------------------

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

# ------------------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------------------

if(GRAIN_BUILD_TESTS)

    enable_testing()

    add_subdirectory(tests)

endif()

# ------------------------------------------------------------------------------
# Examples
# ------------------------------------------------------------------------------

if(GRAIN_BUILD_EXAMPLES)

    add_subdirectory(examples/basic)

endif()