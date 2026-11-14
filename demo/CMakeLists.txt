cmake_minimum_required(VERSION 3.25)

project(GrainDemos LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(../cmake/GrainDependencies.cmake)

add_subdirectory(Audio)
add_subdirectory(Image)
add_subdirectory(Movie)
add_subdirectory(Color)
