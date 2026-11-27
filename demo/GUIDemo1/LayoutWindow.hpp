#pragma once

#include <Grain.hpp>

namespace Demo {

class LayoutWindow {
public:
    explicit LayoutWindow(Grain::App& app);

private:
    // Styles must outlive all views and components using them.
    Grain::UIStyles styles_;

    // The App owns the actual Window.
    Grain::Window* window_ = nullptr;

    std::unique_ptr<Grain::Popup> popup_;
};

} // namespace Demo