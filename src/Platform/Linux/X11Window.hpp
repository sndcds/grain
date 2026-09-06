#pragma once

#include "../Platform.hpp"


#include <X11/Xlib.h>
#include <cairo/cairo.h>

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
        );

    ~X11Window() override;

    void show() override;

    void setRootView(
        Grain::View* view
        ) override;

    [[nodiscard]]
    ::Window nativeWindow() const noexcept {
        return window_;
    }

    void draw();

    void resize(
        int width,
        int height
        );

private:
    Display* display_ = nullptr;
    ::Window window_ = 0;

    std::string title_;

    Grain::View* view_ = nullptr;

    cairo_surface_t* cairo_surface_ = nullptr;
};

} // namespace Grain::Platform