#include <grain/Platform/Window.hpp>

#include <grain/UI/View.hpp>

#include "Platform.hpp"

#include <memory>
#include <utility>

namespace Grain {

class Window::Impl {
public:
    WindowId id;
    std::unique_ptr<Platform::Window> platform_window;
    std::unique_ptr<View> root_view;

    explicit Impl(
        WindowId id,
        std::unique_ptr<Platform::Window> platform_window
        )
        : id(id),
          platform_window(std::move(platform_window)) {
    }
};


Window::Window(
    std::unique_ptr<Impl> impl
    )
    : impl_(std::move(impl)) {
}


Window::~Window() = default;


void Window::show() {
    if (impl_ == nullptr ||
        impl_->platform_window == nullptr) {
        return;
    }

    impl_->platform_window->show();
}


void Window::setRootView(
    std::unique_ptr<View> view
    ) {
    impl_->root_view = std::move(view);

    if (impl_ == nullptr ||
        impl_->platform_window == nullptr) {
        return;
    }

    impl_->platform_window->setRootView(
        impl_->root_view.get()
        );
}


std::unique_ptr<Window> Window::create(
    WindowId id,
    std::unique_ptr<Platform::Window> platform_window
) {
    if (platform_window == nullptr) {
        return nullptr;
    }

    auto impl =
        std::make_unique<Impl>(
            id,
            std::move(platform_window)
        );

    return std::unique_ptr<Window>(
        new Window(std::move(impl))
    );
}

} // namespace Grain