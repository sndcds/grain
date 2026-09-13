#include <grain/Platform/Window.hpp>

#include <grain/UI/View.hpp>

#include "Platform.hpp"

#include <memory>
#include <utility>

namespace Grain {

class Window::Impl {
public:
    WindowId id;
    std::unique_ptr<Platform::Window> platformWindow;
    std::unique_ptr<View> rootView;

    explicit Impl(
        WindowId id,
        std::unique_ptr<Platform::Window> platform_window
        )
        : id(id),
          platformWindow(std::move(platform_window)) {
    }
};


Window::Window(
    std::unique_ptr<Impl> impl
    )
    : impl_(std::move(impl)) {
}


Window::~Window() = default;


WindowId Window::id() const noexcept {
    return impl_->id;
}


void Window::show() {
    if (impl_ == nullptr ||
        impl_->platformWindow == nullptr) {
        return;
    }

    impl_->platformWindow->show();
}


void Window::setRootView(
    std::unique_ptr<View> view
    ) {
    impl_->rootView = std::move(view);

    if (impl_ == nullptr ||
        impl_->platformWindow == nullptr) {
        return;
    }

    impl_->platformWindow->setRootView(
        impl_->rootView.get()
        );
}


std::unique_ptr<Window> Window::create(
    WindowId id,
    std::unique_ptr<Platform::Window> platformWindow
) {
    if (platformWindow == nullptr) {
        return nullptr;
    }

    auto impl =
        std::make_unique<Impl>(
            id,
            std::move(platformWindow)
        );

    return std::unique_ptr<Window>(
        new Window(std::move(impl))
    );
}

} // namespace Grain
