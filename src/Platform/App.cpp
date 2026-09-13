#include <grain/Platform/App.hpp>

#include "Platform.hpp"

#include <memory>
#include <utility>
#include <vector>
#include <iostream>

namespace Grain {

class App::Impl {
public:
    std::unique_ptr<Platform::App> platformApp;
    std::vector<std::unique_ptr<Window> > windows;

    Input input;

    WindowId nextWindowId = 1;

    bool running = false;

    Impl()
        : platformApp(Platform::createApp()) {
    }
};

App& App::instance() {
    static App app;
    return app;
}

App::App()
    : impl_(std::make_unique<Impl>()) {

    if (impl_->platformApp != nullptr) {
        impl_->platformApp->setEventHandler(
            [this](const Event& event) {
                processEvent(event);
            }
            );
    }
}

App::~App() = default;

Window* App::createWindow(
    std::string_view title,
    int width,
    int height
    ) {
    if (impl_->platformApp == nullptr) {
        return nullptr;
    }

    const WindowId windowId = impl_->nextWindowId++;

    auto platformWindow =
        impl_->platformApp->createWindow(
            windowId,
            title,
            width,
            height
            );

    if (platformWindow == nullptr) {
        return nullptr;
    }

    auto window =
        Window::create(
            windowId,
            std::move(platformWindow)
            );

    if (window == nullptr) {
        return nullptr;
    }

    Window* result = window.get();

    impl_->windows.push_back(std::move(window));

    result->show();

    return result;
}

Input& App::input() noexcept {
    return impl_->input;
}

const Input& App::input() const noexcept {
    return impl_->input;
}

void App::run() {
    if (impl_ == nullptr ||
        impl_->platformApp == nullptr) {
        return;
    }

    impl_->running = true;

    impl_->input.beginFrame();

    impl_->platformApp->run();

    impl_->running = false;
}

void App::quit() noexcept {
    impl_->running = false;

    if (impl_->platformApp != nullptr) {
        impl_->platformApp->quit();
    }
}

void App::processEvent(const Event& event) {
    std::cout
        << "event type: " << static_cast<int>(event.type)
        << ", windowId: " << static_cast<int>(event.windowId)
        << ", mouseButton: " << static_cast<int>(event.mouseButton)
        << ", " << event.mouseX
        << ", " << event.mouseY
        << '\n';

    if (event.type == EventType::Quit) {
        quit();
        return;
    }

    impl_->input.processEvent(event);
}

} // namespace Grain