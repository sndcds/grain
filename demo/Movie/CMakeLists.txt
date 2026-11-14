add_executable(
        movie_demo
        MovieDemo.cpp
)

target_link_libraries(
        movie_demo
        PRIVATE
        Grain
)
