add_executable(
        GUIDemo1
        main.cpp
        LayoutWindow.cpp
        TextFieldWindow.cpp
)

target_link_libraries(
        GUIDemo1
        PRIVATE
        Grain
)