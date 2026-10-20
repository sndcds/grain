#include <grain/Platform/Window.hpp>
#include <grain/Platform/View.hpp>

#include "Platform.hpp"

#include <memory>
#include <utility>

namespace Grain {

class Window::Impl {
public:
    uint64_t id_;

    std::unique_ptr<Platform::Window> platform_window_;
    std::unique_ptr<View> root_view_;

    explicit Impl(
        uint64_t id,
        std::unique_ptr<Platform::Window> platform_window
    )
        : id_(id),
          platform_window_(std::move(platform_window))
    {
    }
};


Window::Window(std::unique_ptr<Impl> impl)
    : impl_(std::move(impl))
{
}

Window::~Window() = default;


auto Window::id() const -> uint64_t {
    return impl_->id_;
}


void Window::show() {
    if (impl_ == nullptr ||
        impl_->platform_window_ == nullptr) {
        return;
    }

    impl_->platform_window_->show();
}


void Window::requestRedraw() {
    if (impl_ == nullptr ||
        impl_->platform_window_ == nullptr) {
        return;
    }

    impl_->platform_window_->requestRedraw();
}


void Window::setRootView(std::unique_ptr<View> view) {
    if (impl_ == nullptr || impl_->platform_window_ == nullptr) {
        return;
    }

    impl_->root_view_ = std::move(view);

    if (impl_->root_view_ == nullptr) {
        impl_->platform_window_->setRootView(nullptr);
        return;
    }

    impl_->root_view_->attach();

    impl_->platform_window_->setRootView(
        impl_->root_view_->platformView()
    );
}


void Window::handleEvent(const Event& event) {
    (void)event;
    // TODO: Implement behaviour! Should the window close/delete?
}


auto Window::create(
    uint64_t id,
    std::unique_ptr<Platform::Window> platform_window
) -> std::unique_ptr<Window>
{
    if (!platform_window) {
        return nullptr;
    }

    auto impl = std::make_unique<Impl>(
        id,
        std::move(platform_window)
    );

    auto window = std::unique_ptr<Window>(
        new Window(std::move(impl))
    );

    Window* window_ptr = window.get();

    window->impl_->platform_window_->setEventHandler(
        [window_ptr](const Event& event) -> void {
            window_ptr->handleEvent(event);
        }
    );

    return window;
}


} // namespace Grain
