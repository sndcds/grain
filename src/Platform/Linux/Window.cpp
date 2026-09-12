#include "X11App.hpp"
#include "X11Window.hpp"

#include "CairoGraphicContext.hpp"
#include <grain/UI/View.hpp>

#include <cairo/cairo-xlib.h>

#include <memory>
#include <string>
#include <string_view>

namespace Grain::Platform {

X11Window::X11Window(
    WindowId id,
    Display* display,
    std::string_view title,
    int width,
    int height
    )
    : windowId_(id),
      display_(display) {

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

    cairo_surface_ =
        cairo_xlib_surface_create(
            display_,
            window_,
            DefaultVisual(display_, screen),
            width,
            height
            );

    if (cairo_surface_ == nullptr ||
        cairo_surface_status(cairo_surface_) !=
        CAIRO_STATUS_SUCCESS) {

        if (cairo_surface_ != nullptr) {
            cairo_surface_destroy(cairo_surface_);
            cairo_surface_ = nullptr;
        }
    }
}

X11Window::~X11Window() {

    if (cairo_surface_ != nullptr) {
        cairo_surface_destroy(cairo_surface_);
        cairo_surface_ = nullptr;
    }

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

void X11Window::show() {

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

void X11Window::setRootView(
    Grain::View* view
    ) {
    view_ = view;

    if (view_ != nullptr) {
        draw();
    }
}

void X11Window::draw() {

    if (cairo_surface_ == nullptr ||
        view_ == nullptr) {
        return;
    }

    cairo_t* context =
        cairo_create(cairo_surface_);

    if (context == nullptr ||
        cairo_status(context) !=
        CAIRO_STATUS_SUCCESS) {

        if (context != nullptr) {
            cairo_destroy(context);
        }

        return;
    }

    Grain::Platform::CairoGraphicContext graphics_context(
        context
        );

    view_->draw(
        graphics_context
        );

    cairo_destroy(context);

    cairo_surface_flush(
        cairo_surface_
        );

    XFlush(display_);
}

void X11Window::resize(
    int width,
    int height
    ) {

    if (cairo_surface_ == nullptr) {
        return;
    }

    if (width <= 0 ||
        height <= 0) {
        return;
    }

    cairo_xlib_surface_set_size(
        cairo_surface_,
        width,
        height
        );
}

std::unique_ptr<Window> X11App::createWindow(
    WindowId id,
    std::string_view title,
    int width,
    int height
    ) {

    auto window =
        std::make_unique<X11Window>(
            id,
            display(),
            title,
            width,
            height
            );

    auto* x11_window =
        window.get();

    addWindow(x11_window);

    return window;
}

} // namespace Grain::Platform
