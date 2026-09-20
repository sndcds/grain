#include <grain/Platform/Window.hpp>
#include <grain/Platform/View.hpp>

#include "Platform.hpp"

#include <memory>
#include <utility>

namespace Grain {

class Window::Impl {
public:
    uint64_t id;

    std::unique_ptr<Platform::Window> platformWindow;
    std::unique_ptr<View> rootView;

    explicit Impl(
        uint64_t id,
        std::unique_ptr<Platform::Window> platformWindow
        )
        : id(id),
          platformWindow(std::move(platformWindow))
    {
        std::cout << "Window::Impl()" << std::endl;
    }
};


Window::Window(std::unique_ptr<Impl> impl)
    : impl_(std::move(impl))
{
    std::cout << "Window::Window()" << std::endl;
}

Window::~Window() = default;


uint64_t Window::id() const noexcept {
    return impl_->id;
}


void Window::show() {
    std::cout << "Window::show()" << std::endl;
    if (impl_ == nullptr ||
        impl_->platformWindow == nullptr) {
        return;
    }

    impl_->platformWindow->show();
}


void Window::requestRedraw() {
    std::cout << "Window::requestRedraw() id: " << id() << std::endl;
    if (impl_ == nullptr ||
        impl_->platformWindow == nullptr) {
        return;
    }

    impl_->platformWindow->requestRedraw();
}


void Window::setRootView(std::unique_ptr<View> view) {
    std::cout << "Window::setRootView()" << std::endl;

    if (impl_ == nullptr || impl_->platformWindow == nullptr) {
        return;
    }

    std::cout << "  Take ownership of the view" << std::endl;
    impl_->rootView = std::move(view);

    if (impl_->rootView == nullptr) {
        impl_->platformWindow->setRootView(nullptr);
        return;
    }

    std::cout << "  Attach the view" << std::endl;
    impl_->rootView->attach();

    std::cout << "  Set platform root view" << std::endl;
    impl_->platformWindow->setRootView(
        impl_->rootView->platformView()
    );
}


std::unique_ptr<Window> Window::create(
    uint64_t id,
    std::unique_ptr<Platform::Window> platformWindow
    )
{
    std::cout << "Window::create() id: " << id << std::endl;
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
