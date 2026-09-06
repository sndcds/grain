#include "X11App.hpp"

#include <memory>
#include <stdexcept>

namespace Grain::Platform {

X11App::X11App() {
    display_ = XOpenDisplay(nullptr);

    if (display_ == nullptr) {
        throw std::runtime_error(
            "Grain: unable to open X11 display"
            );
    }
}

X11App::~X11App() {
    if (display_ != nullptr) {
        XCloseDisplay(display_);
        display_ = nullptr;
    }
}

std::unique_ptr<App> createApp() {
    return std::make_unique<X11App>();
}

void X11App::run() {
    if (display_ == nullptr) {
        return;
    }

    while (true) {
        XEvent event{};

        XNextEvent(
            display_,
            &event
            );
    }
}

} // namespace Grain::Platform