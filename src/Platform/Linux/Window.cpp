#include "X11App.hpp"

#include <grain/UI/View.hpp>

#include "../Platform.hpp"

#include <X11/Xlib.h>

#include <memory>
#include <string>
#include <string_view>

namespace Grain::Platform {

class X11Window final : public Window {
public:
    X11Window(
        Display* display,
        std::string_view title,
        int width,
        int height
        )
        : display_(display) {
        if (display_ == nullptr) {
            return;
        }

        const int screen =
            DefaultScreen(display_);

        const ::Window root =
            RootWindow(
                display_,
                screen
                );

        window_ = XCreateSimpleWindow(
            display_,
            root,
            100,
            100,
            static_cast<unsigned int>(width),
            static_cast<unsigned int>(height),
            0,
            BlackPixel(display_, screen),
            WhitePixel(display_, screen)
            );

        if (window_ == 0) {
            return;
        }

        title_ = std::string(title);

        XStoreName(
            display_,
            window_,
            title_.c_str()
            );

        XSelectInput(
            display_,
            window_,
            ExposureMask |
            StructureNotifyMask
            );
    }

    ~X11Window() override {
        if (display_ != nullptr &&
            window_ != 0) {
            XDestroyWindow(
                display_,
                window_
                );

            XFlush(display_);

            window_ = 0;
        }
    }

    void show() override {
        if (display_ == nullptr ||
            window_ == 0) {
            return;
        }

        XMapWindow(
            display_,
            window_
            );

        XFlush(display_);
    }

    void setRootView(
        Grain::View* view
        ) override {
        view_ = view;
    }

private:
    Display* display_ = nullptr;
    ::Window window_ = 0;

    std::string title_;

    Grain::View* view_ = nullptr;
};

std::unique_ptr<Window> X11App::createWindow(
    std::string_view title,
    int width,
    int height
    ) {
    auto window =
        std::make_unique<X11Window>(
            display(),
            title,
            width,
            height
            );

    return window;
}

} // namespace Grain::Platform