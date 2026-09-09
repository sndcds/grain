#include "X11App.hpp"
#include "X11Window.hpp"

#include <memory>
#include <stdexcept>

namespace Grain::Platform {

X11App::X11App() {

    display_ =
        XOpenDisplay(nullptr);

    if (display_ == nullptr) {
        throw std::runtime_error(
            "Grain: unable to open X11 display"
            );
    }
}

X11App::~X11App() {

    windows_.clear();

    if (display_ != nullptr) {
        XCloseDisplay(display_);
        display_ = nullptr;
    }
}

void X11App::addWindow(
    X11Window* window
    ) {

    if (window == nullptr) {
        return;
    }

    windows_.push_back(window);
}

std::unique_ptr<App> createApp() {

    return std::make_unique<X11App>();
}

void X11App::run() {

    if (display_ == nullptr) {
        return;
    }

    running_ = true;

    while (running_) {

        XEvent event{};

        XNextEvent(
            display_,
            &event
            );

        X11Window* target = nullptr;

        for (auto* window : windows_) {

            if (window == nullptr) {
                continue;
            }

            if (window->nativeWindow() ==
                event.xany.window) {

                target = window;
                break;
            }
        }

        if (target == nullptr) {
            continue;
        }

        switch (event.type) {

        case Expose:

            if (event.xexpose.count == 0) {
                target->draw();
            }

            break;

        case ConfigureNotify: {

            target->resize(
                event.xconfigure.width,
                event.xconfigure.height
                );

            Grain::Event grain_event;
            grain_event.type = EventType::WindowResize;
            grain_event.window_id = target->id();
            grain_event.width = event.xconfigure.width;
            grain_event.height = event.xconfigure.height;
            emitEvent(grain_event);

            break;
        }

        default:
            break;
        }
    }
}

void X11App::quit() {
    running_ = false;
}

} // namespace Grain::Platform
