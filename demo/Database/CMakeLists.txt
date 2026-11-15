add_executable(
        psql_demo
        PSQLDemo.cpp
)

target_link_libraries(
        psql_demo
        PRIVATE
        Grain
)
