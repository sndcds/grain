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

    // Popup test objects. Declaration order is intentional:
    // destruction happens in reverse order.
    std::unique_ptr<Grain::View> popup_content_;
    std::unique_ptr<Grain::Platform::View> popup_platform_view_;
    std::unique_ptr<Grain::Platform::Popup> popup_;
};

} // namespace Demo