#pragma once

#include <Grain.hpp>

namespace Demo {

class TextFieldWindow {
public:
    explicit TextFieldWindow(Grain::App& app);

private:
    // Keep styles alive while the window and its components exist.
    Grain::UIStyles styles_;

    // The App owns the actual Window.
    Grain::Window* window_ = nullptr;
};

} // namespace Demo