#include <grain/Platform/Window.hpp>
#include <grain/UI/View.hpp>

#include "../../include/grain/Platform/Platform.hpp"

#include <iostream>
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


auto Window::rootView() -> View& {
    return *impl_->root_view_;
}


auto Window::rootView() const -> const View& {
    return *impl_->root_view_;
}


auto Window::contentBounds() const -> Rectd {
    if (!impl_ || !impl_->platform_window_) {
        return {};
    }

    return impl_->platform_window_->contentBounds();
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

    if (!impl_ || !impl_->root_view_) {
        return;
    }

    switch (event.type_) {

        case EventType::WindowWillClose: {
            closing.emit();
            break;
        }

        case EventType::WindowResize: {
            resized.emit(contentBounds().width, contentBounds().height);
            updateLayout();
            break;
        }

        case EventType::MouseMove:
        case EventType::MouseButtonDown:
        case EventType::MouseButtonUp:
        case EventType::MouseWheel: {
            impl_->root_view_->dispatchMouseEvent(event);
            break;
        }

        case EventType::KeyDown:
        case EventType::KeyUp:
        case EventType::ModifiersChanged: {
            impl_->root_view_->handleKeyEvent(event);
            break;
        }

        default: {
            impl_->root_view_->handleEvent(event);
            break;
        }
    }
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

    auto view = std::make_unique<View>();
    window->setRootView(std::move(view));

    return window;
}


void Window::updateLayout() {

    layout_engine_.layout(
        rootView(),
        contentBounds()
    );
}


} // namespace Grain
